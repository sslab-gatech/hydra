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
#ifndef TREE_H
#define TREE_H

/* tree_init - creates a new tree abstraction.
 */
extern bool_t tree_init( char *hkdir,
			 char *dstdir,
			 bool_t toconlypr,
			 bool_t ownerpr,
			 xfs_ino_t rootino,
			 xfs_ino_t firstino,
			 xfs_ino_t lastino,
			 size64_t dircnt,
			 size64_t nondircnt,
			 size64_t vmsz,
			 bool_t fullpr,
			 bool_t restoredmpr,
			 bool_t dstdirisxfspr,
			 uint32_t dumpformat,
			 bool_t truncategenpr );

/* tree_sync - synchronizes with an existing tree abstraction
 */
extern bool_t tree_sync( char *hkdir,
			 char *dstdir,
			 bool_t toconlypr,
			 bool_t fullpr,
			 bool_t dstdirisxfspr );

/* tree_check_dump_format - detect the rare case where a
 * cumulative restore begins with a format 3 (or newer)
 * dump, and a later restore in the series encounters
 * a format 2 dump. the restore will fail unless the
 * original restore was told to use format 2 gen numbers.
 */
extern bool_t tree_check_dump_format( uint32_t dumpformat );


/* tree_begindir - begins application of dumped directory to tree.
 * returns handle to dir node. returns by reference the dirattr
 * handle if new. caller must pre-zero (DAH_NULL).
 */
extern nh_t tree_begindir( filehdr_t *fhdrp, dah_t *dahp );

/* tree_addent - adds a directory entry; takes dirh from above call
 */
extern rv_t tree_addent( nh_t dirh,
			 xfs_ino_t ino,
			 gen_t gen,
			 char *name,
			 size_t namelen );

/* ends application of dir
 */
extern void tree_enddir( nh_t dirh );

#ifdef TREE_CHK
/* tree_chk - do a sanity check of the tree prior to post-processing and
 * non-dir restoral. returns FALSE if corruption detected.
 */
extern bool_t tree_chk( void );
#endif /* TREE_CHK */

/* tree_marknoref - mark all nodes as no reference, not dumped dirs, and
 * clear all directory attribute handles. done at the beginning
 * of the restoral of a dump session, in order to detect directory entries
 * no longer needed.
 */
extern void tree_marknoref( void );

/* mark all nodes in tree as either selected or unselected, depending on sense
 */
extern void tree_markallsubtree( bool_t sensepr );

extern bool_t tree_subtree_parse( bool_t sensepr, char *path );

extern bool_t tree_post( char *path1, char *path2 );

extern rv_t tree_cb_links( xfs_ino_t ino,
			   gen_t gen,
			   int32_t ctime,
			   int32_t mtime,
			   bool_t ( * funcp )( void *contextp,
					       bool_t linkpr,
					       char *path1,
					       char *path2 ),
			   void *contextp,
			   char *path1,
			   char *path2 );

/* called after all dirs have been restored. adjusts the ref flags,
 * by noting that dirents not refed because their parents were not dumped
 * are virtually reffed if their parents are refed.
 */
extern bool_t tree_adjref( void );

extern bool_t tree_setattr( char *path );
extern bool_t tree_delorph( void );
extern bool_t tree_subtree_inter( void );

extern bool_t tree_extattr( bool_t ( * cbfunc )( char *path, dah_t dah ),
			    char *path );
	/* does a depthwise bottom-up traversal of the tree, calling
	 * the supplied callback for all directories with a non-NULL dirattr
	 * handle. The callback will get called with the directory's pathname
	 * and it dirattr handle. the traversal will be aborted if the
	 * callback returns FALSE. returns FALSE if operator requests
	 * an interrupt.
	 */

#endif /* TREE_H */
