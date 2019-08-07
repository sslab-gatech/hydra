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
#ifndef RING_H
#define RING_H

/* ring - readahead/writeahead abstraction
 *
 * the ring is conceptually an ordered set of messages circulating between the
 * client thread and the I/O slave thread. a message can be in one of four
 * places: on the ready queue, held by the client, on the active queue, or held
 * by the slave. The client and slave can each hold at most one message at a
 * time. all others must be on one of the two queues. the messages must
 * circulate in that order: ready, client, active, slave, ready, ...
 * initially all messages are on the ready queue, with status set to
 * INIT. The client uses ring_get to remove a message from the ready queue.
 * the client can then use the message to read or write. to read, the client
 * sets the op field to READ, and places the message on the active queue. the
 * slave will remove messages from the active queue, invoke the client-supplied
 * read function with the message's buffer, record the read function's return
 * value in the message, set the message status to OK (read function returned 0)
 * or ERROR (read returned non-zero), and place the message on the ready queue.
 * the client will see the message after removing all messages ahead of it on
 * the ready queue. to write, the client follows almost the same procedure,
 * except the client fills the buffer and sets the op to WRITE prior to placing
 * the message on the active queue.
 *
 * if the client-supplied read or write function returns an error, the slave
 * will set the message status to ERROR. the slave will pass all subsequent
 * messages appearing on the active queue directly to the ready queue with
 * no I/O done and the message status set to IGNORE. the slave will remain
 * in this state until a reset is performed (see below).
 *
 * The client may at anytime place a NOP msg on the ring. the slave does
 * nothing with this mmessage other than to place it back on the ready queue
 * with NOPACK status. This is useful for inhibiting read-ahead.
 *
 * To flush the ring, the client must repetatively place TRACE messages on the
 * active queue until it sees an IGNORE msg on the ready queue. the slave will
 * simply transfer TRACErs from active to ready with no other action taken
 * (other than to set the message status to IGNORE).
 *
 * the client may at any time reset the ring. the reset will return to the
 * client when the current I/O being executed by the slave completes, and
 * all messages have been wiped clean and placed on the ready queue with
 * status set to INIT. the ring_reset function accomplishes this internally by
 * placing a RESET message on the active QUEUE, and continuing to remove
 * messages from the ready queue (without placing them on the active queue)
 * until the RESET message is seen the slave responds to a reset message by
 * setting the status to RESETACK, queueing the message on the ready queue, and
 * waiting for a message from the active queue. ring_reset will then re-
 * initialize the ring and return. note that the client may be holding one
 * message at the time the reset is called. if so, it must pass a pointer to
 * that message into the reset call. otherwise it must pass in NULL.
 *
 * the ring_destroy function may be invoked to shut down the ring and kill the
 * slave thread. it simply places a DIE message on the active queue, and waits
 * for a DIEACK response. it then de-allocates all semaphores memory allocated
 * by ring_create.
 *
 * the message structure includes a 64 bit field for the convenience
 * of the client. it is not perturbed during any ring operations.
 *
 * the ring maintains four performance metering values: the number of times
 * the slave and client attempted to get a message, and the number of times
 * those attempts resulting in blocking.
 */


/* ring_msg - structure of messages managed by ring
 */
enum ring_op { RING_OP_NONE,
	       RING_OP_READ,
	       RING_OP_WRITE,
	       RING_OP_NOP,
	       RING_OP_TRACE,
	       RING_OP_RESET,
	       RING_OP_DIE };

typedef enum ring_op ring_op_t;

enum ring_stat { RING_STAT_INIT,
	         RING_STAT_OK,
	         RING_STAT_ERROR,
	         RING_STAT_NOPACK,
	         RING_STAT_IGNORE,
	         RING_STAT_RESETACK,
	         RING_STAT_DIEACK };

typedef enum ring_stat ring_stat_t;

enum ring_loc { RING_LOC_READY,
	        RING_LOC_ACTIVE,
	        RING_LOC_CLIENT,
	        RING_LOC_SLAVE };

typedef enum ring_loc ring_loc_t;

struct ring_msg {
	ring_op_t rm_op;
	ring_stat_t rm_stat;
	int rm_rval;
	off64_t rm_user;
	char *rm_bufp;
/* ALL BELOW PRIVATE!!! */
	size_t rm_mix;
	ring_loc_t rm_loc;
};

typedef struct ring_msg ring_msg_t;


/* ring - instantiation of a ring
 */
struct ring {
	off64_t r_client_msgcnt;
	off64_t r_slave_msgcnt;
	off64_t r_client_blkcnt;
	off64_t r_slave_blkcnt;
	time32_t r_first_io_time;
	off64_t r_all_io_cnt;
/* ALL BELOW PRIVATE!!! */
	size_t r_len;
	ring_msg_t *r_msgp;
	size_t r_ready_in_ix;
	size_t r_ready_out_ix;
	qsemh_t r_ready_qsemh;
	size_t r_active_in_ix;
	size_t r_active_out_ix;
	qsemh_t r_active_qsemh;
	size_t r_client_cnt;
	size_t r_slave_cnt;
	int ( *r_readfunc )( void *contextp, char *bufp );
	int ( *r_writefunc )( void *contextp, char *bufp );
	void *r_clientctxp;
};

typedef struct ring ring_t;
	

/* ring_create - creates a ring. parameters supply the length of the ring,
 * the read/write buffer size, the drive index, a function for reading, a
 * function for writing, and a pointer to client context for the read and
 * write functions. returns a pointer to a ring if successful, a NULL
 * pointer if not. the read and write functions should return 0 on success,
 * or an error code on failure which will be recorded in the rm_rval field
 * of the message invoking the failed operation. if null pointer returned,
 * the location pointed to by rvalp will contain one of the following:
 * ENOMEM - could not allocate some portion of the ring memory;
 * E2BIG - insufficient physical memory available for pinning;
 * EPERM - exceeds allowed amount of pinned down memory.
 */
extern ring_t *ring_create( size_t ringlen,
			    size_t bufsz,
			    bool_t pinpr,
			    ix_t drive_index,
			    int ( * readfunc )( void *clientctxp, char *bufp ),
			    int ( * writefunc )( void *clientctxp, char *bufp ),
			    void *clientctxp,
			    int *rvalp );


/* ring_get - get a message off the ready queue
 */
extern ring_msg_t *ring_get( ring_t *ringp );


/* ring_put - put a message on the active queue
 */
extern void ring_put( ring_t *ringp, ring_msg_t *msgp );


/* ring_reset - re-initialize the ring, after the current I/O completes.
 * msgp must be NULL if the client is not currently holding a ring message.
 * otherwise it must point to that message.
 */
extern void ring_reset( ring_t *ringp, ring_msg_t *msgp );

/* ring_destroy - de-allocates ring
 */
extern void ring_destroy( ring_t *ringp );

#endif /* RING_H */
