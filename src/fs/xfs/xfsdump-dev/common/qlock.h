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
#ifndef QLOCK_H
#define QLOCK_H

/* qlock - quick locks abstraction
 *
 * threads may allocate quick locks using qlock_alloc, and free them with
 * qlock_free.
 *
 * deadlock detection is accomplished by giving an ordinal number to each
 * lock allocated, and record all locks held by each thread. locks may not
 * be acquired out of order. that is, subsequently acquired locks must have
 * a lower ordinal than all locks currently held. for convenience, the ordinals
 * of all locks to be allocated will be defined in this file.
 */

#define QLOCK_ORD_CRIT	0
	/* ordinal for global critical region lock
	 */
#define QLOCK_ORD_WIN	1
	/* ordinal for win abstraction critical regions
	 */
#define QLOCK_ORD_PI	2
	/* ordinal for persistent inventory abstraction critical regions
	 */
#define QLOCK_ORD_MLOG	3
	/* ordinal for mlog lock
	 */

typedef void *qlockh_t;
#define QLOCKH_NULL	0
	/* opaque handle
	 */

extern qlockh_t qlock_alloc( ix_t ord );
	/* allocates a qlock with the specified ordinal. returns
	 * NULL if lock can't be allocated.
	 */
extern void qlock_lock( qlockh_t qlockh );
	/* acquires the specified lock.
	 */
extern void qlock_unlock( qlockh_t qlockh );
	/* releases the specified lock.
	 */

typedef void *qsemh_t;
#define QSEMH_NULL	0
	/* opaque handle
	 */

extern qsemh_t qsem_alloc( size_t cnt );
	/* allocates a counting semaphore initialized to the specified
	 * count. returns a qsem handle
	 */
extern void qsem_free( qsemh_t qsemh );
	/* frees the counting semaphore
	 */
extern void qsemP( qsemh_t qsemh );
	/* "P" (decrement) op
	 */
extern void qsemV( qsemh_t qsemh );
	/* "V" (increment) op
	 */
extern bool_t qsemPwouldblock( qsemh_t qsemh );
	/* returns true if a qsemP op would block
	 */
extern size_t qsemPavail( qsemh_t qsemh );
	/* number of resources available
	 */

#endif /* QLOCK_H */
