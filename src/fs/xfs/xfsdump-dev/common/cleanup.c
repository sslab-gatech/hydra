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
#include <assert.h>

#include "cleanup.h"

struct cu {
	void ( * cu_funcp )( void *arg1, void *arg2 );
	void *cu_arg1;
	void *cu_arg2;
	int  cu_flags;
	struct cu *cu_nextp;
};

/* Cleanup structure flags
 */
#define CU_EARLY	0x00000001	
			/* call cleanup routine before calling killall */

typedef struct cu cu_t;

static cu_t *cu_rootp;

void
cleanup_init( void )
{
	cu_rootp = 0;
}

static cleanup_t *
cleanup_register_base( void ( * funcp )( void *arg1, void *arg2 ),
		  void *arg1,
		  void *arg2 )
{
	cu_t *p;

	p = ( cu_t * )calloc( 1, sizeof( cu_t ));
	assert( p );
	p->cu_funcp = funcp;
	p->cu_arg1 = arg1;
	p->cu_arg2 = arg2;
	p->cu_flags = 0;
	p->cu_nextp = cu_rootp;
	cu_rootp = p;

	return ( cleanup_t *)p;
}

cleanup_t *
cleanup_register( void ( * funcp )( void *arg1, void *arg2 ),
		  void *arg1,
		  void *arg2 )
{
	cu_t *p;

	p = cleanup_register_base( funcp, arg1, arg2 );

	return ( cleanup_t *)p;
}

cleanup_t *
cleanup_register_early( void ( * funcp )( void *arg1, void *arg2 ),
		  void *arg1,
		  void *arg2 )
{
	cu_t *p;

	p = cleanup_register_base( funcp, arg1, arg2 );
	p->cu_flags = CU_EARLY;

	return ( cleanup_t *)p;
}

void
cleanup_cancel( cleanup_t *cleanupp )
{
	cu_t *p = ( cu_t *)cleanupp;
	cu_t *nextp;
	cu_t *prevp;

	assert( cu_rootp );

	for ( prevp = 0, nextp = cu_rootp
	      ;
	      nextp && nextp != p
	      ;
	      prevp = nextp, nextp = nextp->cu_nextp )
	;

	assert( nextp );
	if ( prevp ) {
		prevp->cu_nextp = p->cu_nextp;
	} else {
		cu_rootp = p->cu_nextp;
	}
	free( ( void * )p );
}

void
cleanup( void )
{
	while ( cu_rootp ) {
		cu_t *p = cu_rootp;
		( * p->cu_funcp )( p->cu_arg1, p->cu_arg2 );
		cu_rootp = p->cu_nextp;
		free( ( void * )p );
	}
}

void
cleanup_early( void )
{
	cu_t *cuptr, *cuprevp;

	cuptr = cu_rootp;
	cuprevp = NULL;

	while ( cuptr ) {
		cu_t *cunextp = cuptr->cu_nextp;

		if ( cuptr->cu_flags & CU_EARLY) {
			( * cuptr->cu_funcp )( cuptr->cu_arg1, cuptr->cu_arg2 );
			free( ( void * )cuptr );
			if ( cuprevp )  {
				cuprevp->cu_nextp = cunextp;
			} else {
				cu_rootp = cunextp;
			}
			cuptr = cunextp;
		} else {
			cuprevp = cuptr;
			cuptr = cunextp;
		}
	}
}
