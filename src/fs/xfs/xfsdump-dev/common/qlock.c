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

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#include "config.h"

#include "types.h"
#include "qlock.h"
#include "mlog.h"

struct qlock {
	ix_t ql_ord;
		/* ordinal position of this lock
		 */
	pthread_mutex_t ql_mutex;
};

typedef struct  qlock qlock_t;
	/* internal qlock
	 */

typedef size_t ordmap_t;
	/* bitmap of ordinals. used to track what ordinals have
	 * been allocated.
	 */

#define ORDMAX					( 8 * sizeof( ordmap_t ))
	/* ordinals must fit into a wordsize bitmap
	 */

static ordmap_t qlock_ordalloced;
	/* to enforce allocation of only one lock to each ordinal value
	 */

static __thread ordmap_t thread_ordmap;
	/* holds the ordmap for each thread
	 */

#define QLOCK_ORDMAP_SET( ordmap, ord )	( ordmap |= 1U << ord )
	/* sets the ordinal bit in an ordmap
	 */

#define QLOCK_ORDMAP_CLR( ordmap, ord )	( ordmap &= ~( 1U << ord ))
	/* clears the ordinal bit in an ordmap
	 */

#define QLOCK_ORDMAP_GET( ordmap, ord )	( ordmap & ( 1U << ord ))
	/* checks if ordinal set in ordmap
	 */

#define QLOCK_ORDMAP_CHK( ordmap, ord )	( ordmap & (( 1U << ord ) - 1U ))
	/* checks if any bits less than ord are set in the ordmap
	 */


qlockh_t
qlock_alloc( ix_t ord )
{
	qlock_t *qlockp;

	/* verify the ordinal is not already taken, and mark as taken
	 */
	assert( ! QLOCK_ORDMAP_GET( qlock_ordalloced, ord ));
	QLOCK_ORDMAP_SET( qlock_ordalloced, ord );

	/* allocate lock memory
	 */
	qlockp = ( qlock_t * )calloc( 1, sizeof( qlock_t ));
	assert( qlockp );

	/* initialize the mutex
	 */
	pthread_mutex_init( &qlockp->ql_mutex, NULL );

	/* assign the ordinal position
	 */
	qlockp->ql_ord = ord;

	return ( qlockh_t )qlockp;
}

void
qlock_lock( qlockh_t qlockh )
{
	qlock_t *qlockp = ( qlock_t * )qlockh;
	pthread_t tid;
	/* REFERENCED */
	int rval;
	
	/* get the caller's tid
	 */
	tid = pthread_self();

	/* assert that this lock not already held by this thread
	 */
	if ( QLOCK_ORDMAP_GET( thread_ordmap, qlockp->ql_ord )) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_NOLOCK,
		      _("lock already held: tid %lu ord %d map %x\n"),
		      tid,
		      qlockp->ql_ord,
		      thread_ordmap );
	}
	assert( ! QLOCK_ORDMAP_GET( thread_ordmap, qlockp->ql_ord ));

	/* assert that no locks with a lesser ordinal are held by this thread
	 */
	if ( QLOCK_ORDMAP_CHK( thread_ordmap, qlockp->ql_ord )) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_NOLOCK,
		      _("lock ordinal violation: tid %lu ord %d map %x\n"),
		      tid,
		      qlockp->ql_ord,
		      thread_ordmap );
	}
	assert( ! QLOCK_ORDMAP_CHK( thread_ordmap, qlockp->ql_ord ));

	/* acquire the lock
	 */
	rval = pthread_mutex_lock( &qlockp->ql_mutex );
	assert( !rval );

	/* add ordinal to this threads ordmap
	 */
	QLOCK_ORDMAP_SET( thread_ordmap, qlockp->ql_ord );
}

void
qlock_unlock( qlockh_t qlockh )
{
	qlock_t *qlockp = ( qlock_t * )qlockh;
	/* REFERENCED */
	int rval;
	
	/* verify lock is held by this thread
	 */
	assert( QLOCK_ORDMAP_GET( thread_ordmap, qlockp->ql_ord ));

	/* clear lock's ord from thread's ord map
	 */
	QLOCK_ORDMAP_CLR( thread_ordmap, qlockp->ql_ord );
	
	/* release the lock
	 */
	rval = pthread_mutex_unlock( &qlockp->ql_mutex );
	assert( ! rval );
}

qsemh_t
qsem_alloc( ix_t cnt )
{
	sem_t *semp;
	int rval;

	/* allocate a semaphore
	 */
	semp = ( sem_t * )calloc( 1, sizeof( sem_t ));
	assert( semp );

	/* initialize the semaphore
	 */
	rval = sem_init( semp, 0, cnt );
	assert( !rval );

	return ( qsemh_t )semp;
}

void
qsem_free( qsemh_t qsemh )
{
	sem_t *semp = ( sem_t * )qsemh;
	int rval;

	/* destroy the mutex and condition
	 */
	rval = sem_destroy( semp );
	assert( !rval );

	/* free the semaphore
	 */
	free( semp );
}

void
qsemP( qsemh_t qsemh )
{
	sem_t *semp = ( sem_t * )qsemh;
	int rval;
	
	/* "P" the semaphore
	 */
	rval = sem_wait( semp );
	assert( !rval );
}

void
qsemV( qsemh_t qsemh )
{
	sem_t *semp = ( sem_t * )qsemh;
	int rval;
	
	/* "V" the semaphore
	 */
	rval = sem_post( semp );
	assert( !rval );
}

bool_t
qsemPwouldblock( qsemh_t qsemh )
{
	sem_t *semp = ( sem_t * )qsemh;
	int count;
	int rval;

	rval = sem_getvalue( semp, &count );
	assert( !rval );

	return count <= 0 ? BOOL_TRUE : BOOL_FALSE;
}

size_t
qsemPavail( qsemh_t qsemh )
{
	sem_t *semp = ( sem_t * )qsemh;
	int count;
	int rval;

	rval = sem_getvalue( semp, &count );
	assert( !rval );

	return count < 0 ? 0 : count;
}
