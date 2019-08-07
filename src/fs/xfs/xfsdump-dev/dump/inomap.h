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
#ifndef INOMAP_H
#define INOMAP_H

/* inomap.[hc] - inode map abstraction
 *
 * an inode map describes the inode numbers (inos) in a file system dump.
 * the map identifies which inos are in-use by the fs, which of those are
 * directories, and which are dumped.
 *
 * the map is represented as a list of map segments. a map segment is
 * a 64-bit starting ino and two 64-bit bitmaps. the bitmaps describe
 * the 64 inos beginning with the starting ino. two bits are available
 * for each ino.
 */
struct xfs_bstat;

/* inomap_build - this function allocates and constructs an in-memory
 * representation of the bitmap. it prunes from the map inos of files not
 * changed since the last dump, inos not identified by the subtree list,
 * and directories not needed to represent a hierarchy containing
 * changed inodes. it handles hard links; a file linked to multiple
 * directory entries will not be pruned if at least one of those
 * directories has an ancestor in the subtree list.
 *
 * it returns by reference an array of startpoints in the non-directory
 * portion of the dump, as well as the count of dir and nondir inos
 * makred as present and to be dumped. A startpoint identifies a non-dir ino,
 * and a non-hole accumulated size position within that file. only very large
 * files will contain a startpoint; in all other cases the startpoints will
 * fall at file boundaries. returns BOOL_FALSE if error encountered (should
 * abort the dump; else returns BOOL_TRUE.
 */
extern bool_t inomap_build( void *fshandlep,
			    int fsfd,
			    struct xfs_bstat *rootstatp,
			    bool_t last,
	      		    time32_t lasttime,
			    bool_t resume,
	      		    time32_t resumetime,
			    size_t resumerangecnt,
			    drange_t *resumerangep,
			    char *subtreebuf[],
			    ix_t subtreecnt,
			    bool_t skip_unchanged_dirs,
			    startpt_t startptp[],
	      		    size_t startptcnt,
			    ix_t *statphasep,
			    ix_t *statpassp,
			    size64_t statcnt,
			    size64_t *statdonep );

extern uint64_t inomap_getsz( void );

/* inomap_skip - tell inomap about inodes to skip in the dump
 */
extern void inomap_skip( xfs_ino_t ino );


/* inomap_writehdr - updates the write header with inomap-private info
 * to be communicated to the restore side
 */
extern void inomap_writehdr( content_inode_hdr_t *scwhdrp );


/* inomap_dump - dumps the map to media - content-abstraction-knowledgable
 *
 * returns error from media write op
 */
extern rv_t inomap_dump( drive_t *drivep );


/* map state values
 */
#define MAP_INO_UNUSED	0       /* ino not in use by fs */
#define MAP_DIR_NOCHNG	1       /* dir, ino in use by fs, but not dumped */
#define MAP_NDR_NOCHNG	2       /* non-dir, ino in use by fs, but not dumped */
#define MAP_DIR_CHANGE	3       /* dir, changed since last dump */
#define MAP_NDR_CHANGE	4       /* non-dir, changed since last dump */
#define MAP_DIR_SUPPRT	5       /* dir, unchanged but needed for hierarchy */
#define MAP_RESERVED1	6       /* this state currently not used */
#define MAP_RESERVED2	7       /* this state currently not used */

/* the inomap is implemented as a linked list of chunks. each chunk contains
 * an array of map segments. a map segment contains a start ino and a
 * bitmap of 64 3-bit state values (see MAP_... in inomap.h). the SEG_macros
 * index and manipulate the 3-bit state values.
 */
struct seg {
	xfs_ino_t base;
	uint64_t lobits;
	uint64_t mebits;
	uint64_t hibits;
};

typedef struct seg seg_t;

#define INOPERSEG	( sizeof( (( seg_t * )0 )->lobits ) * NBBY )

#define HNKSZ		( 4 * PGSZ )
#define SEGPERHNK	( ( HNKSZ / sizeof( seg_t )) - 1 )

struct hnk {
	seg_t seg[ SEGPERHNK ];
	xfs_ino_t maxino;
	struct hnk *nextp;		/* no longer used, kept for binary compat */
	char pad[sizeof( seg_t ) - sizeof( xfs_ino_t ) - sizeof( struct hnk * )];
};

typedef struct hnk hnk_t;

/* functions to get state and gen information from the inomap.
 * requires a pointer to a context block, obtained from
 * inomap_alloc_context(), and released by inomap_free_context().
 */
extern void *inomap_alloc_context( void );
extern void inomap_reset_context( void *contextp );
extern void inomap_free_context( void *contextp );
extern int inomap_get_state( void *contextp, xfs_ino_t ino );
extern int inomap_get_gen( void *contextp, xfs_ino_t ino, gen_t *gen );


/* generators returning the next dir or non-dir ino selected in this dump.
 * returns INO64MAX when no more inos.
 * requires a pointer to a context block, obtained from
 * inomap_alloc_context(), and released by inomap_free_context().
 */
extern xfs_ino_t inomap_next_nondir(void *contextp, xfs_ino_t lastino);
extern xfs_ino_t inomap_next_dir(void *contextp, xfs_ino_t lastino);

#endif /* INOMAP_H */
