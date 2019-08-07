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

#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <memory.h>
#include <limits.h>
#include <assert.h>

#include "config.h"

#include "types.h"
#include "mlog.h"
#include "win.h"
#include "node.h"
#include "mmap.h"

extern size_t pgsz;
extern size_t pgmask;

/* node handle limits
 */
#ifdef NODECHK

/* macros for manipulating node handles when handle consistency
 * checking is enabled. the upper bits of a handle will be loaded
 * with the node gen count, described below. this should not be
 * used for production code, it cuts into the number of dirents
 * that xfsrestore can handle.
 */
#define HDLGENCNT		4
#define	HDLGENSHIFT		( NBBY * sizeof ( nh_t ) - HDLGENCNT )
#define	HDLGENLOMASK		( ( 1 << HDLGENCNT ) - 1 )
#define	HDLGENMASK		( HDLGENLOMASK << HDLGENSHIFT )
#define HDLMASK			( ( 1 << HDLGENSHIFT ) - 1 )
#define HDLGETGEN( h )		( ( u_char_t )				\
				  ( ( ( int )h >> HDLGENSHIFT )		\
				    &					\
				    HDLGENLOMASK ))
#define HDLGETNHDL( h )		( ( nh_t )( ( int )h & HDLMASK ))
#define HDLMKHDL( g, n )	( ( nh_t )( ( ( ( int )g << HDLGENSHIFT )\
					      &				\
					      HDLGENMASK )		\
					  |				\
					  ( ( int )n & HDLMASK )))
#define NH_MAX			( HDLMASK )

/* the housekeeping byte of each node will hold two check fields:
 * a gen count, initialized to 0 and incremented each time a node
 * is allocated, to catch re-use of stale handles; and unique pattern, to
 * differentiate a valid node from random memory. two unique patterns will
 * be used; one when the node is on the free list, another when it is
 * allocated. this allows me to detect use of handles to nodes which have
 * be freed.
 */
#define HKPGENCNT		HDLGENCNT
#define HKPGENSHIFT		( NBBY - HKPGENCNT )
#define HKPGENLOMASK		( ( 1 << HKPGENCNT ) - 1 )
#define HKPGENMASK		( HKPGENLOMASK << HKPGENSHIFT )
#define HKPUNQCNT		( NBBY - HKPGENCNT )
#define HKPUNQMASK		( ( 1 << HKPUNQCNT ) - 1 )
#define HKPGETGEN( b )		( ( u_char_t )				\
				  ( ( ( int )b >> HKPGENSHIFT )		\
				    &					\
				    HKPGENLOMASK ))
#define HKPGETUNQ( b )		( ( u_char_t )( ( int )b & HKPUNQMASK ))
#define HKPMKHKP( g, u )	( ( u_char_t )				\
				  ( ( ( ( int )g << HKPGENSHIFT )	\
				      &					\
				      HKPGENMASK )			\
				    |					\
				    ( ( int )u & HKPUNQMASK )))

/* simple patterns for detecting a node
 */
#define NODEUNQFREE			0x9
#define NODEUNQALCD			0x6

#else /* NODECHK */

#define NH_MAX			( NH_NULL - 1 )

#endif /* NODECHK */

/* window constraints
 */
#define NODESPERSEG_MIN	1048576
#define WINMAP_MIN	4

/* reserve the firstpage for a header to save persistent context
 */
#define NODE_HDRSZ	pgsz

typedef int relnix_t;

struct node_hdr {
	size_t nh_nodesz;
		/* internal node size - user may see something smaller
		 */
	ix_t nh_nodehkix;
		/* index of byte in each node the user has reserved
		 * for use by me
		 */
	nh_t nh_nodesperseg;
		/* an integral number of internal nodes must fit into a
		 * segment
		 */
	size64_t nh_segsz;
		/* the backing store is partitoned into segment, which
		 * can be mapped into VM windows  by the win abstraction
		 */
	size_t nh_winmapmax;
		/* maximum number of windows which can be mapped
		 */
	size_t nh_nodealignsz;
		/* user's constraint on node alignment
		 */
	nh_t nh_freenh;
		/* handle of first node of singly-linked list of free nodes
		 */
	off64_t nh_firstsegoff;
		/* offset into backing store of the first segment
		 */
	nh_t nh_virgnh;
		/* handle of next virgin node
		 */
	int nh_segixshift;
		/* bitshift used to extract the segment index from an nh_t
		 */
	relnix_t nh_relnixmask;
		/* bitmask used to extract the node index from an nh_t
		 * (relative to the start of a segment)
		 */
};

typedef struct node_hdr node_hdr_t;

static node_hdr_t *node_hdrp;
static int node_fd;

static inline segix_t
nh2segix( nh_t nh )
{
	return nh >> node_hdrp->nh_segixshift;
}

static inline relnix_t
nh2relnix( nh_t nh )
{
	return nh & node_hdrp->nh_relnixmask;
}

static inline void
node_map_internal( nh_t nh, void **pp )
{
	win_map( nh2segix( nh ), pp );
	if ( *pp != NULL ) {
		relnix_t relnix = nh2relnix( nh );
		*pp = ( void * )( ( char * )( *pp ) +
				( ( off64_t )relnix *
				  node_hdrp->nh_nodesz ) );
	}
}

/* ARGSUSED */
static inline void
node_unmap_internal( nh_t nh, void **pp, bool_t freepr )
{
#ifdef NODECHK
	register u_char_t hkp;
	register u_char_t hdlgen;
	register u_char_t nodegen;
	register u_char_t nodeunq;
#endif /* NODECHK */

	assert( pp );
	assert( *pp );
	assert( nh != NH_NULL );

	/* convert the handle into an index
	 */
#ifdef NODECHK
	hdlgen = HDLGETGEN( nh );
	nh = HDLGETNHDL( nh );
#endif /* NODECHK */

	assert( nh <= NH_MAX );

#ifdef NODECHK
	hkp = *( *( u_char_t ** )pp + node_hdrp->nh_nodehkix );
	nodegen = HKPGETGEN( hkp );
	assert( nodegen == hdlgen );
	nodeunq = HKPGETUNQ( hkp );
	if ( ! freepr ) {
		assert( nodeunq != NODEUNQFREE );
		assert( nodeunq == NODEUNQALCD );
	} else {
		assert( nodeunq != NODEUNQALCD );
		assert( nodeunq == NODEUNQFREE );
	}
#endif /* NODECHK */

	/* unmap the window containing the node
	 */
	win_unmap( nh2segix( nh ), pp ); /* zeros *pp */
}

/* ARGSUSED */
bool_t
node_init( int fd,
	   off64_t off,
	   size_t usrnodesz,
	   ix_t nodehkix,
	   size_t nodealignsz,
	   size64_t vmsz,
	   size64_t dirs_nondirs_cnt )
{
	size_t nodesz;
	size64_t segsz;
	nh_t nodesperseg;
	size_t max_segments;
	size_t winmapmax;
	size_t segcount;
	int segixshift;

	/* sanity checks
	 */
	assert( sizeof( node_hdr_t ) <= NODE_HDRSZ );
	assert( sizeof( nh_t ) < sizeof( off64_t ));
	assert( sizeof( nh_t ) <= sizeof( segix_t ));
	assert( sizeof( nh_t ) <= sizeof( relnix_t ));
	assert( nodehkix < usrnodesz );
	assert( usrnodesz >= sizeof( char * ) + 1 );
		/* so node is at least big enough to hold
		 * the free list linkage and the housekeeping byte
		 */
	assert( nodehkix > sizeof( char * ));
		/* since beginning of each node is used to
		 * link it in the free list.
		 */

	/* adjust the user's node size to meet user's alignment constraint
	*/
	nodesz = ( usrnodesz + nodealignsz - 1 ) & ~( nodealignsz - 1 );

	/* Calculate the node table params based on the number of inodes in the
	 * dump, since that's all we know. Ideally we'd base this on the number
	 * of dirents in the dump instead as there's a node per dirent.
	 *
	 * Due to virtual memory constraints and the fact that we don't know
	 * the final node table size, we can't guarantee that the entire node
	 * table can be mapped at once. Instead segments will be mapped using a
	 * window abstraction. Some operations require WINMAP_MIN nodes to be
	 * referenced at a time, therefore we must ensure that this many
	 * segments fit in virtual memory.
	 *
	 * nodesperseg must be a power of two. Earlier versions did not enforce
	 * this, and profiling showed that nearly 50% of cpu time during the
	 * node table construction was consumed doing division and modulo to
	 * convert an nh_t to a segment index and node offset.  By making
	 * nodesperseg a power of two we can use shift and bitwise-and instead.
	 *
	 * Each segment must hold an integral number of nodes and be an integral
	 * number of pages. #1 ensures this except when nodesperseg is small, so
	 * the smallest allowed segsz is pgsz * nodesz (i.e., nodesperseg ==
	 * pgsz). However this is of no consequence as we enforce a larger
	 * minimum nodesperseg (NODESPERSEG_MIN) anyway in order to place a
	 * reasonable cap on the max number of segments.
	 */

	assert( NODESPERSEG_MIN >= pgsz );

	if ( vmsz < WINMAP_MIN * NODESPERSEG_MIN * nodesz ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		  "not enough virtual memory for node abstraction: "
		  "remaining-vsmz=%llu need=%llu\n"),
		  vmsz, WINMAP_MIN * NODESPERSEG_MIN * nodesz );
		return BOOL_FALSE;
	}

	/* This is checked as nodes are allocated as well (remember that
	 * dirs_nondirs_cnt may be less than the number of nodes/dirents).
	 * Checking this here prevents potential overflow in the logic below.
	 */
	if ( dirs_nondirs_cnt > NH_MAX ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		  "dump contains %llu inodes, restore can only handle %u\n"),
		  dirs_nondirs_cnt, NH_MAX );
		return BOOL_FALSE;
	}

	for ( winmapmax = 0, segcount = 1; winmapmax < WINMAP_MIN; segcount <<= 1 ) {

		nodesperseg = max( dirs_nondirs_cnt / segcount, NODESPERSEG_MIN );

		/* nodesperseg must be a power of 2 */
		for ( segixshift = 0;
		      ( 1ULL << segixshift ) < nodesperseg;
		      segixshift++ );

		/* rounding up to a power of 2 may have caused overflow */
		if ( ( 1ULL << segixshift ) > NH_MAX )
			segixshift--;

		nodesperseg = 1UL << segixshift;

		max_segments = 1UL << ( NBBY * sizeof(nh_t) - segixshift );

		segsz = nodesperseg * nodesz;

		/* max number of segments that will fit in virtual memory,
		 * capped at the max possible number of segments
		 */
		winmapmax = min( vmsz / segsz, max_segments );
	}

	/* map the abstraction header
	 */
	assert( ( NODE_HDRSZ & pgmask ) == 0 );
	assert( ! ( NODE_HDRSZ % pgsz ));
	assert( off <= OFF64MAX );
	assert( ! ( off % ( off64_t )pgsz ));
	node_hdrp = ( node_hdr_t * )mmap_autogrow(
					    NODE_HDRSZ,
					    fd,
					    off );
	if ( node_hdrp == (node_hdr_t *)-1 ) {
	    mlog( MLOG_NORMAL | MLOG_ERROR, _(
		  "unable to map node hdr of size %d: %s\n"),
		  NODE_HDRSZ,
		  strerror( errno ));
	    return BOOL_FALSE;
	}

	/* initialize and save persistent context.
	 */
	node_hdrp->nh_nodesz = nodesz;
	node_hdrp->nh_nodehkix = nodehkix;
	node_hdrp->nh_segsz = segsz;
	node_hdrp->nh_winmapmax = winmapmax;
	node_hdrp->nh_nodesperseg = nodesperseg;
	node_hdrp->nh_nodealignsz = nodealignsz;
	node_hdrp->nh_freenh = NH_NULL;
	node_hdrp->nh_firstsegoff = off + ( off64_t )NODE_HDRSZ;
	node_hdrp->nh_virgnh = 0;
	node_hdrp->nh_segixshift = segixshift;
	node_hdrp->nh_relnixmask = nodesperseg - 1;

	/* save transient context
	 */
	node_fd = fd;

	/* initialize the window abstraction
	 */
	win_init( fd,
		  node_hdrp->nh_firstsegoff,
		  segsz,
		  winmapmax );
	
	/* announce the results
	 */
	mlog( MLOG_DEBUG | MLOG_TREE,
	      "node_init:"
	      " vmsz = %llu (0x%llx)"
	      " segsz = %llu (0x%llx)"
	      " nodesperseg = %u (0x%x)"
	      " winmapmax = %lu (0x%lx)"
	      "\n",
	      vmsz, vmsz,
	      segsz, segsz,
	      nodesperseg, nodesperseg,
	      winmapmax, winmapmax );

	return BOOL_TRUE;
}

bool_t
node_sync( int fd, off64_t off )
{
	/* sanity checks
	 */
	assert( sizeof( node_hdr_t ) <= NODE_HDRSZ );

	/* map the abstraction header
	 */
	assert( ( NODE_HDRSZ & pgmask ) == 0 );
	assert( off <= ( off64_t )OFF64MAX );
	assert( ! ( off % ( off64_t )pgsz ));
	node_hdrp = ( node_hdr_t * )mmap_autogrow(
					    NODE_HDRSZ,
					    fd,
					    off );
	if ( node_hdrp == (node_hdr_t *)-1 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "unable to map node hdr of size %d: %s\n"),
		      NODE_HDRSZ,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* save transient context
	 */
	node_fd = fd;

	/* initialize the window abstraction
	 */
	win_init( fd,
		  node_hdrp->nh_firstsegoff,
		  node_hdrp->nh_segsz,
		  node_hdrp->nh_winmapmax );

	return BOOL_TRUE;
}

nh_t
node_alloc( void )
{
	u_char_t *p;
	nh_t nh;
#ifdef NODECHK
	u_char_t *hkpp;
	u_char_t gen = 0;
	u_char_t unq;
#endif /* NODECHK */

	/* if there's a node available on the free list, use it.
	 * otherwise get the next one from the current virgin segment,
	 * or allocate a new virgin segment if the current one is depleted.
	 */
	if ( node_hdrp->nh_freenh != NH_NULL ) {
		nh_t *linkagep;

		nh = node_hdrp->nh_freenh;

		node_map_internal( nh, ( void ** )&p );
		if (p == NULL)
			return NH_NULL;
#ifdef NODECHK
		hkpp = p + node_hdrp->nh_nodehkix;
		gen = ( u_char_t )( HKPGETGEN( *p ) + ( u_char_t )1 );
		unq = HKPGETUNQ( *hkpp );
		assert( unq != NODEUNQALCD );
		assert( unq == NODEUNQFREE );
#endif /* NODECHK */

		/* adjust the free list */
		linkagep = ( nh_t * )p;
		node_hdrp->nh_freenh = *linkagep;

		node_unmap_internal( nh, ( void ** )&p, BOOL_TRUE );

	} else {
		if ( nh2relnix( node_hdrp->nh_virgnh ) == 0 ) {
			/* need to start a new virgin segment */
			int rval;
			off64_t new_seg_off =
				node_hdrp->nh_firstsegoff +
				( off64_t )nh2segix( node_hdrp->nh_virgnh ) *
				( off64_t )node_hdrp->nh_segsz;

			assert( new_seg_off
				<=
				OFF64MAX - ( off64_t )node_hdrp->nh_segsz );
			mlog( MLOG_DEBUG,
			      "pre-growing new node array segment at %lld "
			      "size %lld\n",
			      new_seg_off,
			      ( off64_t )node_hdrp->nh_segsz );
			rval = ftruncate64( node_fd,
					    new_seg_off
					    +
					    ( off64_t )node_hdrp->nh_segsz );
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
				      "unable to autogrow node segment %u: "
				      "%s (%d)\n"),
				      nh2segix( node_hdrp->nh_virgnh ),
				      strerror( errno ),
				      errno );
			}
		}

		nh = node_hdrp->nh_virgnh++;
	}

	/* build a handle for node
	 */
	if ( nh > NH_MAX ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		  "dump contains too many dirents, "
		  "restore can only handle %llu\n"),
		  NH_MAX );
		return NH_NULL;
	}
#ifdef NODECHK
	node_map_internal( nh , ( void ** )&p );
	if (p == NULL)
		abort();
	hkpp = p + ( int )node_hdrp->nh_nodehkix;
	nh = HDLMKHDL( gen, nh );
	*hkpp = HKPMKHKP( gen, NODEUNQALCD );
	node_unmap_internal( nh, ( void ** )&p, BOOL_FALSE );
#endif /* NODECHK */

	return nh;
}

void *
node_map( nh_t nh )
{
	u_char_t *p;
#ifdef NODECHK
	register u_char_t hkp;
	register u_char_t hdlgen;
	register u_char_t nodegen;
	register u_char_t nodeunq;
#endif /* NODECHK */

	assert( nh != NH_NULL );

	/* convert the handle into an index
	 */
#ifdef NODECHK
	hdlgen = HDLGETGEN( nh );
	nh = HDLGETNHDL( nh );
#endif /* NODECHK */

	assert( nh <= NH_MAX );

	/* map in
	 */
	p = 0; /* keep lint happy */
	node_map_internal( nh, ( void ** )&p );
	if (p == NULL)
	    return NULL;

#ifdef NODECHK
	hkp = *( p + node_hdrp->nh_nodehkix );
	nodegen = HKPGETGEN( hkp );
	nodeunq = HKPGETUNQ( hkp );
	assert( nodegen == hdlgen );
	assert( nodeunq != NODEUNQFREE );
	assert( nodeunq == NODEUNQALCD );
#endif /* NODECHK */

	return ( void * )p;
}

void
node_unmap( nh_t nh, void **pp )
{
	node_unmap_internal( nh, pp, BOOL_FALSE );
}

void
node_free( nh_t *nhp )
{
	nh_t nh;
	u_char_t *p;
	register nh_t *linkagep;
#ifdef NODECHK
	register u_char_t *hkpp;
	register u_char_t hdlgen;
	register u_char_t nodegen;
	register u_char_t nodeunq;
#endif /* NODECHK */

	assert( nhp );
	nh = *nhp;
	assert( nh != NH_NULL );

	/* convert the handle into an index
	 */
#ifdef NODECHK
	hdlgen = HDLGETGEN( nh );
	nh = HDLGETNHDL( nh );
#endif /* NODECHK */

	assert( nh <= NH_MAX );

	/* map in
	 */
	p = ( u_char_t * )node_map( nh );
	if (p == NULL){
	    *nhp = NH_NULL;
	    return;
	}

#ifdef NODECHK
	/* fix up unique field
	 */
	hkpp = p + node_hdrp->nh_nodehkix;
	nodegen = HKPGETGEN( *hkpp );
	nodeunq = HKPGETUNQ( *hkpp );
	assert( nodegen == hdlgen );
	assert( nodeunq != NODEUNQFREE );
	assert( nodeunq == NODEUNQALCD );
	*hkpp = HKPMKHKP( nodegen, NODEUNQFREE );
#endif /* NODECHK */

	/* put node on free list
	 */
	linkagep = ( nh_t * )p;
	*linkagep = node_hdrp->nh_freenh;
	node_hdrp->nh_freenh = nh;

	/* map out
	 */
	node_unmap_internal( nh, ( void ** )&p, BOOL_TRUE );

	/* invalidate caller's handle
	 */
	*nhp = NH_NULL;
}
