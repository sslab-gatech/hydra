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
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "config.h"

#include "types.h"
#include "mlog.h"
#include "bag.h"

bag_t *
bag_alloc( void )
{
	bag_t *bagp;

	bagp = ( bag_t * )calloc( 1, sizeof( bag_t ));
	assert( bagp );
	return bagp;
}

void
bag_insert( bag_t *bagp,
	    bagelem_t *newp,
	    size64_t key,
	    void *payloadp )
{
	register bagelem_t *nextp;
	register bagelem_t *prevp;

	assert( ! newp->be_loaded );
	newp->be_loaded = BOOL_TRUE;
	assert( ! newp->be_bagp );
	newp->be_bagp = bagp;

	newp->be_key = key;
	newp->be_payloadp = payloadp;

	if ( bagp->b_headp ) {
		nextp = bagp->b_headp;
		prevp = bagp->b_headp->be_prevp;
	} else {
		nextp = newp;
		prevp = newp;
	}

	newp->be_nextp = nextp;
	newp->be_prevp = prevp;
	nextp->be_prevp = newp;
	prevp->be_nextp = newp;

	bagp->b_headp = newp;
}

void
bag_remove( bag_t *bagp,
	    bagelem_t *oldp,
	    size64_t *keyp,
	    void **payloadpp )
{
	register bagelem_t *nextp;
	register bagelem_t *prevp;

	assert( oldp->be_loaded );
	assert( oldp->be_bagp == bagp );

	nextp = oldp->be_nextp;
	prevp = oldp->be_prevp;

	nextp->be_prevp = prevp;
	prevp->be_nextp = nextp;

	if ( bagp->b_headp == oldp ) {
		if ( nextp == oldp ) {
			assert( prevp == oldp );
			bagp->b_headp = 0;
		} else {
			bagp->b_headp = nextp;
		}
	}

	*keyp = oldp->be_key;
	*payloadpp = oldp->be_payloadp;

	memset( ( void * )oldp, 0, sizeof( bagelem_t ));
}

bagelem_t *
bag_find( bag_t *bagp,
	 size64_t key,
	 void **payloadpp )
{
	register bagelem_t *p;

	for ( p = bagp->b_headp
	      ;
	      p && p->be_nextp != bagp->b_headp && p->be_key != key
	      ;
	      p = p->be_nextp )
		;
	
	if ( ! p || p->be_key != key ) {
		*payloadpp = 0;
		return 0;
	} else {
		assert( p->be_loaded );
		assert( p->be_bagp == bagp );
		*payloadpp = p->be_payloadp;
		return p;
	}
}


void
bagiter_init( bag_t *bagp, bagiter_t *iterp )
{
	iterp->bi_bagp = bagp;
	if ( ! bagp->b_headp ) {
		iterp->bi_nextp = 0;
		return;
	}
	iterp->bi_lastp = bagp->b_headp->be_prevp;
	iterp->bi_nextp = bagp->b_headp;
}

bagelem_t *
bagiter_next( bagiter_t *iterp, void **payloadpp )
{
	bagelem_t *returnp;

	/* termination condition
	 */
	if ( ! iterp->bi_nextp ) {
		*payloadpp = 0;
		return 0;
	}

	/* save the element to be returned
	 */
	returnp = iterp->bi_nextp;

	/* calculate next. if returning last, set next to NULL
	 */
	if ( iterp->bi_nextp == iterp->bi_lastp ) {
		iterp->bi_nextp = 0;
	} else {
		iterp->bi_nextp = iterp->bi_nextp->be_nextp;
	}

	*payloadpp = returnp->be_payloadp;
	return returnp;
}

void
bag_free( bag_t *bagp )
{
	register bagelem_t *p;

	p = bagp->b_headp;
	while ( p ) {
		register bagelem_t *nextp = p->be_nextp;
		memset( ( void * )p, 0, sizeof( bagelem_t ));
		p = nextp;
		if ( p == bagp->b_headp ) {
			break;
		}
		assert( p );
	}

	memset( ( void * )bagp, 0, sizeof( bag_t ));
	free( ( void * )bagp );
}
