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

#include <pthread.h>
#include <assert.h>
#include <string.h>

#include "config.h"

#include "types.h"
#include "exit.h"
#include "stream.h"
#include "lock.h"
#include "mlog.h"

#define N(a) (sizeof((a)) / sizeof((a)[0]))

struct spm {
	stream_state_t	s_state;
	pthread_t	s_tid;
	int	s_ix;
	int		s_exit_code;
	rv_t		s_exit_return;
	rv_t		s_exit_hint;
};

typedef struct spm spm_t;
static spm_t spm[ STREAM_SIMMAX * 3 ];
static bool_t initialized = BOOL_FALSE;

void
stream_init( void )
{
	( void )memset( ( void * )spm, 0, sizeof( spm ));
	initialized = BOOL_TRUE;
}

/*
 * Note that the stream list structure (updated via the stream_* functions)
 * is indexed by pthread_t (tid). Multiple processes can be registered against
 * the same stream index, typically: the primary content process that does the
 * work; and the drive slave process, which just processes stuff off the ring
 * buffer. In general having multiple tids registered per stream is not an issue
 * for termination status reporting, as the mlog_exit* logging functions only
 * ever get called out of the primary content process.
 */

void
stream_register( pthread_t tid, int streamix )
{
	spm_t *p = spm;
	spm_t *ep = spm + N(spm);

	assert( streamix < STREAM_SIMMAX );

	lock();
	for ( ; p < ep ; p++ ) {
		if ( p->s_state == S_FREE ) {
			p->s_state = S_RUNNING;
			break;
		}
	}
	unlock();
	assert( p < ep );

	if ( p >= ep ) return;

	p->s_tid = tid;
	p->s_ix = streamix;
	p->s_exit_code = -1;
	p->s_exit_return = RV_NONE;
	p->s_exit_hint = RV_NONE;
}

/* NOTE: lock() must be held when calling stream_dead() */
void
stream_dead( pthread_t tid )
{
	spm_t *p = spm;
	spm_t *ep = spm + N(spm);

	for ( ; p < ep ; p++ )
		if ( pthread_equal( p->s_tid, tid ) ) {
			p->s_state = S_ZOMBIE;
			break;
		}
	assert( p < ep );
}

void
stream_free( pthread_t tid )
{
	spm_t *p = spm;
	spm_t *ep = spm + N(spm);

	lock();
	for ( ; p < ep ; p++ ) {
		if ( pthread_equal( p->s_tid, tid ) ) {
			(void) memset( (void *) p, 0, sizeof(spm_t) );
			p->s_state = S_FREE;
			break;
		}
	}
	unlock();
	assert( p < ep );
}

int
stream_find_all( stream_state_t states[], int nstates,
		 pthread_t tids[], int ntids )
{
	int i, count = 0;
	spm_t *p = spm;
	spm_t *ep = spm + N(spm);
	assert(nstates > 0 && ntids > 0);

	if (!initialized)
		return 0;

	/* lock - make sure we get a consistent snapshot of the stream status */
	lock();
	for ( ; p < ep && count < ntids; p++ )
		for (i = 0; i < nstates; i++)
			if (p->s_state == states[i]) {
				tids[count++] = p->s_tid;
				break;
			}
	unlock();
	return count;
}

static spm_t *
stream_find( pthread_t tid, stream_state_t s[], int nstates )
{
	int i;
	spm_t *p = spm;
	spm_t *ep = spm + N(spm);

	assert(nstates > 0);

	/* note we don't lock the stream array in this function */
	for ( ; p < ep ; p++ )
		if ( pthread_equal( p->s_tid, tid ) ) {
			/* check state */
			for (i = 0; i < nstates; i++)
				if (p->s_state == s[i])
					return p;
		}

#ifdef STREAM_DEBUG
	{
		static const char *state_strings[] = { "S_FREE", "S_RUNNING", "S_ZOMBIE" };
		mlog( MLOG_DEBUG | MLOG_ERROR | MLOG_NOLOCK | MLOG_BARE,
		      "stream_find(): no stream with tid: %lu and state%s:",
		      tid, nstates == 1 ? "" : "s" );
		for (i = 0; i < nstates; i++)
			mlog( MLOG_DEBUG | MLOG_ERROR | MLOG_NOLOCK | MLOG_BARE,
			      " %s", state_strings[s[i]]);
		mlog( MLOG_DEBUG | MLOG_ERROR | MLOG_NOLOCK | MLOG_BARE, "\n");
	}
#endif /* STREAM_DEBUG */

	return (spm_t *) NULL;
}

/*
 * Note, the following function is called from mlog.c:mlog_va(),
 * where locking may be disabled and we are already protected by
 * another lock. So no locking is done in this function.
 */

int
stream_getix( pthread_t tid )
{
	stream_state_t states[] = { S_RUNNING };
	spm_t *p;
	int ix;
	p = stream_find( tid, states, N(states) );
	ix = p ? p->s_ix : -1;
	return ix;
}


/*
 * We don't currently export spm_t and don't really want callers
 * keeping pointers into the stream array, so use explicit access
 * functions. Note we only expect these to be called from running/owner
 * streams.
 */

#define stream_set(field_name, tid, value)				\
	stream_state_t states[] = { S_RUNNING };			\
	spm_t *p;							\
	pthread_t mytid = pthread_self();				\
									\
	if ( !pthread_equal(mytid, (tid))) {				\
		mlog( MLOG_DEBUG | MLOG_ERROR | MLOG_NOLOCK,		\
		      "stream_set_" #field_name "(): "			\
		      "foreign stream (tid %lu) "			\
		      "not permitted to update this stream (tid %lu)\n",\
		      mytid, (tid));					\
		return;							\
	}								\
									\
	lock();								\
	p = stream_find( (tid), states, N(states) );			\
	if (p) p->s_exit_ ## field_name = (value);			\
	unlock();

void stream_set_code( pthread_t tid, int exit_code )
{
	stream_set( code, tid, exit_code );
}

void stream_set_return( pthread_t tid, rv_t rv )
{
	stream_set( return, tid, rv );
}

void stream_set_hint( pthread_t tid, rv_t rv )
{
	stream_set( hint, tid, rv );
}


bool_t
stream_get_exit_status( pthread_t tid,
			stream_state_t states[],
			int nstates,
			stream_state_t *state,
			int *ix,
			int *exit_code,
			rv_t *exit_return,
			rv_t *exit_hint)
{
	bool_t sts = BOOL_FALSE;
	spm_t *p;

	lock();
	p = stream_find( tid, states, nstates );
	if (! p) goto unlock;

	if (state) *state = p->s_state;
	if (ix) *ix = p->s_ix;
	if (exit_code) *exit_code = p->s_exit_code;
	if (exit_return) *exit_return = p->s_exit_return;
	if (exit_hint) *exit_hint = p->s_exit_hint;
	sts = BOOL_TRUE;

 unlock:
	unlock();
	return sts;
}

size_t
stream_cnt( void )
{
	spm_t *p = spm;
	spm_t *ep = spm + N(spm);
	size_t ixmap = 0;
	size_t ixcnt;
	size_t bitix;

	assert( sizeof( ixmap ) * NBBY >= STREAM_SIMMAX );
	
	lock();
	for ( ; p < ep ; p++ ) {
		if ( p->s_state == S_RUNNING ) {
			ixmap |= ( size_t )1 << p->s_ix;
		}
	}
	unlock();

	ixcnt = 0;
	for ( bitix = 0 ; bitix < STREAM_SIMMAX ; bitix++ ) {
		if ( ixmap & ( ( size_t )1 << bitix )) {
			ixcnt++;
		}
	}

	return ixcnt;
}
