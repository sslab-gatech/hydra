/*
 * Copyright (c) 2000-2001 Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>

#include "config.h"

#include "types.h"
#include "qlock.h"
#include "cldmgr.h"
#include "ring.h"

static int ring_slave_entry( void *ringctxp );

ring_t *
ring_create( size_t ringlen,
	     size_t bufsz,
	     bool_t pinpr,
	     ix_t drive_index,
	     int ( *readfunc )( void *clientctxp, char *bufp ),
	     int ( *writefunc )( void *clientctxp, char *bufp ),
	     void *clientctxp,
	     int *rvalp )
{
	bool_t ok;
	ring_t *ringp;
	size_t mix;

	/* pre-initialize return value
	 */
	*rvalp = 0;

	/* allocate a ring descriptor
	 */
	ringp = ( ring_t * )calloc( 1, sizeof( ring_t ));
	assert( ringp );
	ringp->r_len = ringlen;
	ringp->r_clientctxp = clientctxp;
	ringp->r_readfunc = readfunc;
	ringp->r_writefunc = writefunc;

	/* allocate counting semaphores for the ready and active queues,
	 * and initialize the queue input and output indices.
	 */
	ringp->r_ready_qsemh = qsem_alloc( ringlen );
	ringp->r_active_qsemh = qsem_alloc( 0 );
	ringp->r_ready_in_ix = 0;
	ringp->r_ready_out_ix = 0;
	ringp->r_active_in_ix = 0;
	ringp->r_active_out_ix = 0;
	ringp->r_client_cnt = 0;
	ringp->r_slave_cnt = 0;

	/* initialize the meters
	 */
	ringp->r_client_msgcnt = 0;
	ringp->r_slave_msgcnt = 0;
	ringp->r_client_blkcnt = 0;
	ringp->r_slave_blkcnt = 0;
	ringp->r_first_io_time = 0;
	ringp->r_all_io_cnt = 0;

	/* allocate the ring messages
	 */
	ringp->r_msgp = ( ring_msg_t * )calloc( ringlen, sizeof( ring_msg_t ));
	assert( ringp->r_msgp );

	/* allocate the buffers and initialize the messages
	 */
	for ( mix = 0 ; mix < ringlen ; mix++ ) {
		ring_msg_t *msgp = &ringp->r_msgp[ mix ];
		msgp->rm_mix = mix;
		msgp->rm_op = RING_OP_NONE;
		msgp->rm_stat = RING_STAT_INIT;
		msgp->rm_user = 0;
		msgp->rm_loc = RING_LOC_READY;

		msgp->rm_bufp = ( char * )memalign( PGSZ, bufsz );
		if ( ! msgp->rm_bufp ) {
			*rvalp = ENOMEM;
			return 0;
		}
		if ( pinpr ) {
			int rval;
			rval = mlock( ( void * )msgp->rm_bufp, bufsz );
			if ( rval ) {
				if ( errno == ENOMEM ) {
					*rvalp = E2BIG;
					return 0;
				}
				if ( errno == EPERM ) {
					*rvalp = EPERM;
					return 0;
				}
				assert( 0 );
			}
		}
	}

	/* kick off the slave thread
	 */
	ok = cldmgr_create( ring_slave_entry,
			    drive_index,
			    _("slave"),
			    ringp );
	assert( ok );

	return ringp;
}

ring_msg_t *
ring_get( ring_t *ringp )
{
	ring_msg_t *msgp;

	/* assert client currently holds no messages
	 */
	assert( ringp->r_client_cnt == 0 );

	/* bump client message count and note if client needs to block
	 */
	ringp->r_client_msgcnt++;
	if ( qsemPwouldblock( ringp->r_ready_qsemh )) {
		ringp->r_client_blkcnt++;
	}

	/* block until msg available on ready queue ("P")
	 */
	qsemP( ringp->r_ready_qsemh );

	/* get a pointer to the next msg on the queue
	 */
	msgp = &ringp->r_msgp[ ringp->r_ready_out_ix ];

	/* assert the message is where it belongs
	 */
	assert( msgp->rm_loc == RING_LOC_READY );

	/* verify the message index has not become corrupted
	 */
	assert( msgp->rm_mix == ringp->r_ready_out_ix );

	/* bump the output index
	 */
	ringp->r_ready_out_ix = ( ringp->r_ready_out_ix + 1 )
				%
				ringp->r_len;

	/* update the message location
	 */
	msgp->rm_loc = RING_LOC_CLIENT;

	/* bump the count of messages held by the client
	 */
	ringp->r_client_cnt++;

	/* return the msg to the client
	 */
	return msgp;
}

void
ring_put( ring_t *ringp, ring_msg_t *msgp )
{
	/* assert the client holds exactly one message
	 */
	assert( ringp->r_client_cnt == 1 );

	/* assert the client is returning the right message
	 */
	assert( msgp->rm_mix == ringp->r_active_in_ix );

	/* assert the message is where it belongs
	 */
	assert( msgp->rm_loc == RING_LOC_CLIENT );

	/* decrement the count of messages held by the client
	 */
	ringp->r_client_cnt--;

	/* update the message location
	 */
	msgp->rm_loc = RING_LOC_ACTIVE;

	/* bump the active queue input ix
	 */
	ringp->r_active_in_ix = ( ringp->r_active_in_ix + 1 )
				%
				ringp->r_len;
	
	/* bump the semaphore for the active queue ("V")
	 */
	qsemV( ringp->r_active_qsemh );
}

void
ring_reset( ring_t *ringp, ring_msg_t *msgp )
{
	size_t mix;

	/* if the client is not holding a message, get the next message
	 */
	if ( ringp->r_client_cnt == 0 ) {
		assert( ! msgp );
		msgp = ring_get( ringp );
		assert( msgp );
		assert( ringp->r_client_cnt == 1 );
	} else {
		assert( msgp );
		assert( ringp->r_client_cnt == 1 );
	}

	/* tell the slave to abort
	 */
	msgp->rm_op = RING_OP_RESET;
	ring_put( ringp, msgp );

	/* wait for the reset to be acknowledged
	 */
	assert( ringp->r_client_cnt == 0 );
	do {
		/* pull a message from the ready queue
		 */
		qsemP( ringp->r_ready_qsemh );
		msgp = &ringp->r_msgp[ ringp->r_ready_out_ix ];
		assert( msgp->rm_loc == RING_LOC_READY );
		ringp->r_ready_out_ix = ( ringp->r_ready_out_ix + 1 )
					%
					ringp->r_len;
		ringp->r_client_cnt++;
	} while ( msgp->rm_stat != RING_STAT_RESETACK );
	assert( ringp->r_client_cnt == ringp->r_len );

	/* re-initialize the ring
	 */
	assert( qsemPavail( ringp->r_ready_qsemh ) == 0 );
	assert( qsemPavail( ringp->r_active_qsemh ) == 0 );
	ringp->r_ready_in_ix = 0;
	ringp->r_ready_out_ix = 0;
	ringp->r_active_in_ix = 0;
	ringp->r_active_out_ix = 0;
	ringp->r_client_cnt = 0;
	ringp->r_slave_cnt = 0;
	for ( mix = 0 ; mix < ringp->r_len ; mix++ ) {
		ring_msg_t *msgp = &ringp->r_msgp[ mix ];
		msgp->rm_mix = mix;
		msgp->rm_op = RING_OP_NONE;
		msgp->rm_stat = RING_STAT_INIT;
		msgp->rm_user = 0;
		msgp->rm_loc = RING_LOC_READY;
		qsemV( ringp->r_ready_qsemh );
	}
	assert( qsemPavail( ringp->r_ready_qsemh ) == ringp->r_len );
	assert( qsemPavail( ringp->r_active_qsemh ) == 0 );
}

void
ring_destroy( ring_t *ringp )
{
	ring_msg_t *msgp;

	/* the client must not be holding a message
	 */
	assert( ringp->r_client_cnt == 0 );

	/* get a message
	 */
	msgp = ring_get( ringp );

	/* tell the slave to exit
	 */
	msgp->rm_op = RING_OP_DIE;
	ring_put( ringp, msgp );

	/* wait for the die to be acknowledged
	 */
	do {
		/* pull a message from the ready queue
		 */
		qsemP( ringp->r_ready_qsemh );
		msgp = &ringp->r_msgp[ ringp->r_ready_out_ix ];
		assert( msgp->rm_loc == RING_LOC_READY );
		ringp->r_ready_out_ix = ( ringp->r_ready_out_ix + 1 )
					%
					ringp->r_len;
	} while ( msgp->rm_stat != RING_STAT_DIEACK );

	/* the slave is dead.
	 */
	qsem_free( ringp->r_ready_qsemh );
	qsem_free( ringp->r_active_qsemh );
	free( ( void * )ringp );
}


static ring_msg_t *
ring_slave_get( ring_t *ringp )
{
	ring_msg_t *msgp;

	/* assert slave currently holds no messages
	 */
	assert( ringp->r_slave_cnt == 0 );

	/* bump slave message count and note if slave needs to block
	 */
	ringp->r_slave_msgcnt++;
	if ( qsemPwouldblock( ringp->r_active_qsemh )) {
		ringp->r_slave_blkcnt++;
	}

	/* block until msg available on active queue ("P")
	 */
	qsemP( ringp->r_active_qsemh );

	/* get a pointer to the next msg on the queue
	 */
	msgp = &ringp->r_msgp[ ringp->r_active_out_ix ];

	/* assert the message is where it belongs
	 */
	assert( msgp->rm_loc == RING_LOC_ACTIVE );

	/* verify the message index has not become corrupted
	 */
	assert( msgp->rm_mix == ringp->r_active_out_ix );

	/* bump the output index
	 */
	ringp->r_active_out_ix = ( ringp->r_active_out_ix + 1 )
				 %
				 ringp->r_len;

	/* update the message location
	 */
	msgp->rm_loc = RING_LOC_SLAVE;

	/* bump the count of messages held by the slave
	 */
	ringp->r_slave_cnt++;

	/* return the msg to the slave
	 */
	return msgp;
}

static void
ring_slave_put( ring_t *ringp, ring_msg_t *msgp )
{
	/* assert the slave holds exactly one message
	 */
	assert( ringp->r_slave_cnt == 1 );

	/* assert the slave is returning the right message
	 */
	assert( msgp->rm_mix == ringp->r_ready_in_ix );

	/* assert the message is where it belongs
	 */
	assert( msgp->rm_loc == RING_LOC_SLAVE );

	/* decrement the count of messages held by the slave
	 */
	ringp->r_slave_cnt--;

	/* update the message location
	 */
	msgp->rm_loc = RING_LOC_READY;

	/* bump the ready queue input ix
	 */
	ringp->r_ready_in_ix = ( ringp->r_ready_in_ix + 1 )
			       %
			       ringp->r_len;
	
	/* bump the semaphore for the ready queue ("V")
	 */
	qsemV( ringp->r_ready_qsemh );
}

static int
ring_slave_entry( void *ringctxp )
{
	sigset_t blocked_set;
	ring_t *ringp = ( ring_t * )ringctxp;
	enum { LOOPMODE_NORMAL, LOOPMODE_IGNORE, LOOPMODE_DIE } loopmode;

	/* block signals, let the main thread handle them
	 */
	sigemptyset( &blocked_set );
	sigaddset( &blocked_set, SIGINT );
	sigaddset( &blocked_set, SIGHUP );
	sigaddset( &blocked_set, SIGTERM );
	sigaddset( &blocked_set, SIGQUIT );
	sigaddset( &blocked_set, SIGALRM );
	pthread_sigmask( SIG_SETMASK, &blocked_set, NULL );

	/* loop reading and precessing messages until told to die
	 */
	for ( loopmode = LOOPMODE_NORMAL ; loopmode != LOOPMODE_DIE ; ) {
		ring_msg_t *msgp;
		int rval;

		msgp = ring_slave_get( ringp );
		msgp->rm_rval = 0;

		switch( msgp->rm_op ) {
		case RING_OP_READ:
			if ( loopmode == LOOPMODE_IGNORE ) {
				msgp->rm_stat = RING_STAT_IGNORE;
				break;
			}
			if ( ! ringp->r_first_io_time ) {
				ringp->r_first_io_time = time( 0 );
				assert( ringp->r_first_io_time );
			}
			rval = ( ringp->r_readfunc )( ringp->r_clientctxp,
						      msgp->rm_bufp );
			msgp->rm_rval = rval;
			ringp->r_all_io_cnt++;
			if ( msgp->rm_rval == 0 ) {
				msgp->rm_stat = RING_STAT_OK;
			} else {
				msgp->rm_stat = RING_STAT_ERROR;
				loopmode = LOOPMODE_IGNORE;
			}
			break;
		case RING_OP_WRITE:
			if ( loopmode == LOOPMODE_IGNORE ) {
				msgp->rm_stat = RING_STAT_IGNORE;
				break;
			}
			if ( ! ringp->r_first_io_time ) {
				ringp->r_first_io_time = time( 0 );
				assert( ringp->r_first_io_time );
			}
			rval = ( ringp->r_writefunc )( ringp->r_clientctxp,
						       msgp->rm_bufp );
			msgp->rm_rval = rval;
			ringp->r_all_io_cnt++;
			if ( msgp->rm_rval == 0 ) {
				msgp->rm_stat = RING_STAT_OK;
			} else {
				msgp->rm_stat = RING_STAT_ERROR;
				loopmode = LOOPMODE_IGNORE;
			}
			break;
		case RING_OP_NOP:
			msgp->rm_stat = RING_STAT_NOPACK;
			break;
		case RING_OP_TRACE:
			msgp->rm_stat = RING_STAT_IGNORE;
			break;
		case RING_OP_RESET:
			loopmode = LOOPMODE_NORMAL;
			msgp->rm_stat = RING_STAT_RESETACK;
			break;
		case RING_OP_DIE:
			msgp->rm_stat = RING_STAT_DIEACK;
			loopmode = LOOPMODE_DIE;
			break;
		default:
			msgp->rm_stat = RING_STAT_IGNORE;
			break;
		}
		ring_slave_put( ringp, msgp );
	}

	return 0;
}
