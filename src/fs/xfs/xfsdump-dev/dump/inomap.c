/*
 * Copyright (c) 2000-2002 Silicon Graphics, Inc.
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

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include "config.h"

#include "types.h"
#include "util.h"
#include "mlog.h"
#include "global.h"
#include "drive.h"
#include "media.h"
#include "content.h"
#include "content_inode.h"
#include "hsmapi.h"

#include "inomap.h"
#include "arch_xlate.h"
#include "exit.h"
#include <attr/attributes.h>

/* structure definitions used locally ****************************************/

#define BSTATBUFLEN	pgsz
	/* length (in bstat_t's) of buf passed to bigstat_iter
	 */

#define GETDENTBUFSZ	pgsz
	/* size (in bytes) of buf passed to diriter (when not recursive)
	 */

/* declarations of externally defined global symbols *************************/

extern bool_t preemptchk( int );
extern size_t pgsz;
extern hsm_fs_ctxt_t *hsm_fs_ctxtp;
extern uint64_t maxdumpfilesize;
extern bool_t allowexcludefiles_pr;

/* forward declarations of locally defined static functions ******************/

/* inomap construction callbacks
 */
static int cb_context( bool_t last,
			    time32_t,
			    bool_t,
			    time32_t,
			    size_t,
			    drange_t *,
			    startpt_t *,
			    size_t,
			    int,
			    bool_t,
			    bool_t *);
static void cb_context_free( void );
static int cb_count_inogrp( void *, int, xfs_inogrp_t *);
static int cb_add_inogrp( void *, int, xfs_inogrp_t * );
static int cb_add( void *, jdm_fshandle_t *, int, xfs_bstat_t * );
static bool_t cb_inoinresumerange( xfs_ino_t );
static bool_t cb_inoresumed( xfs_ino_t );
static void cb_accuminit_sz( void );
static void cb_spinit( void );
static int cb_startpt( void *,
			    jdm_fshandle_t *,
			    int,
			    xfs_bstat_t * );
static int supprt_prune( void *,
			      jdm_fshandle_t *,
			      int,
			      xfs_bstat_t *,
			      char * );
static off64_t quantity2offset( jdm_fshandle_t *, xfs_bstat_t *, off64_t );
static off64_t estimate_dump_space( xfs_bstat_t * );

/* inomap primitives
 */
static int inomap_init( int igrpcnt );
static void inomap_add( void *, xfs_ino_t ino, gen_t gen, int );
static int inomap_set_state( void *, xfs_ino_t ino, int );
static void inomap_set_gen(void *, xfs_ino_t, gen_t );

/* subtree abstraction
 */
static int subtree_descend_cb( void *,
				    jdm_fshandle_t *,
				    int fsfd,
				    xfs_bstat_t *,
				    char * );
static int subtreelist_parse_cb( void *,
				      jdm_fshandle_t *,
				      int fsfd,
				      xfs_bstat_t *,
				      char * );
static int subtreelist_parse( jdm_fshandle_t *,
				   int,
				   xfs_bstat_t *,
				   char *[],
				   ix_t );

/* definition of locally defined global variables ****************************/


/* definition of locally defined static variables *****************************/

static ix_t *inomap_statphasep;
static ix_t *inomap_statpassp;
static size64_t *inomap_statdonep;
static uint64_t inomap_exclude_filesize = 0;
static uint64_t inomap_exclude_skipattr = 0;

/* definition of locally defined global functions ****************************/

/* inomap_build - build an in-core image of the inode map for the
 * specified file system. identify startpoints in the non-dir inodes,
 * such that the total dump media required is divided into startptcnt segments.
 */
/* ARGSUSED */
bool_t
inomap_build( jdm_fshandle_t *fshandlep,
	      int fsfd,
	      xfs_bstat_t *rootstatp,
	      bool_t last,
	      time32_t lasttime,
	      bool_t resume,
	      time32_t resumetime,
	      size_t resumerangecnt,
	      drange_t *resumerangep,
	      char *subtreebuf[],
	      ix_t subtreecnt,
	      bool_t skip_unchanged_dirs,
	      startpt_t *startptp,
	      size_t startptcnt,
	      ix_t *statphasep,
	      ix_t *statpassp,
	      size64_t statcnt,
	      size64_t *statdonep )
{
	xfs_bstat_t *bstatbufp;
	size_t bstatbuflen;
	bool_t pruneneeded = BOOL_FALSE;
	int igrpcnt = 0;
	int stat;
	int rval;

        /* do a sync so that bulkstat will pick up inode changes
         * that are currently in the inode cache. this is necessary
         * for incremental dumps in order to have the dump time
         * accurately reflect what inodes were included in this dump.
         * (PV 881455)
         */
	sync();

	/* copy stat ptrs
	 */
	inomap_statphasep = statphasep;
	inomap_statpassp = statpassp;
	inomap_statdonep = statdonep;

	/* allocate a bulkstat buf
	 */
	bstatbuflen = BSTATBUFLEN;
	bstatbufp = ( xfs_bstat_t * )memalign( pgsz,
					       bstatbuflen
					       *
					       sizeof( xfs_bstat_t ));
	assert( bstatbufp );

	/* count the number of inode groups, which will serve as a
	 * starting point for the size of the inomap.
	 */
	rval = inogrp_iter( fsfd, cb_count_inogrp, (void *)&igrpcnt, &stat );
	if ( rval || stat ) {
		free( ( void * )bstatbufp );
		return BOOL_FALSE;
	}

	/* initialize the callback context
	 */
	rval = cb_context( last,
			   lasttime,
			   resume,
			   resumetime,
			   resumerangecnt,
			   resumerangep,
			   startptp,
			   startptcnt,
			   igrpcnt,
			   skip_unchanged_dirs,
			   &pruneneeded );
 	if ( rval ) {
 		free( ( void * )bstatbufp );
 		return BOOL_FALSE;
 	}

	/* the inode map requires that inodes are added in increasing
	 * ino order. in the case of a subtree dump, inodes would be
	 * added in whatever order they were discovered when walking the
	 * subtrees. so pre-populate the inomap with all the inode groups
	 * in this filesystem. each inode will be marked unused until its
	 * correct state is set in cb_add.
	 */
	rval = inogrp_iter( fsfd, cb_add_inogrp, NULL, &stat );
 	if ( rval || stat ) {
		cb_context_free();
 		free( ( void * )bstatbufp );
 		return BOOL_FALSE;
 	}

	/* construct the ino map, based on the last dump time, resumed
	 * dump info, and subtree list. place all unchanged directories
	 * in the "needed for children" state (MAP_DIR_SUPPRT). these will be
	 * dumped even though they have not changed. a later pass will move
	 * some of these to "not dumped", such that only those necessary
	 * to represent the minimal tree containing only changes will remain.
	 * for subtree dumps, recurse over the specified subtrees calling
	 * the inomap constructor (cb_add). otherwise, if dumping the entire
	 * filesystem, use the bigstat iterator to add inos to the inomap.
	 * set a flag if any ino not put in a dump state. This will be used
	 * to decide if any pruning can be done.
	 */
	mlog( MLOG_VERBOSE | MLOG_INOMAP, _(
	      "ino map phase 1: "
	      "constructing initial dump list\n") );

	*inomap_statdonep = 0;
	*inomap_statphasep = 1;
	stat = 0;
	cb_accuminit_sz( );

	if ( subtreecnt ) {
		rval = subtreelist_parse( fshandlep,
					  fsfd,
					  rootstatp,
					  subtreebuf,
					  subtreecnt );
	} else {
		rval = bigstat_iter( fshandlep,
				     fsfd,
				     BIGSTAT_ITER_ALL,
				     ( xfs_ino_t )0,
				     cb_add,
				     NULL,
				     NULL,
				     NULL,
				     &stat,
				     preemptchk,
				     bstatbufp,
				     bstatbuflen );
	}
	*inomap_statphasep = 0;
	if ( rval || preemptchk( PREEMPT_FULL )) {
		cb_context_free();
		free( ( void * )bstatbufp );
		return BOOL_FALSE;
	}

	if ( inomap_exclude_filesize > 0 ) {
		mlog( MLOG_NOTE | MLOG_VERBOSE, _(
		      "pruned %llu files: maximum size exceeded\n"),
		      inomap_exclude_filesize );
	}
	if ( inomap_exclude_skipattr > 0 ) {
		mlog( MLOG_NOTE | MLOG_VERBOSE, _(
		      "pruned %llu files: skip attribute set\n"),
		      inomap_exclude_skipattr );
	}

	/* prune directories unchanged since the last dump and containing
	 * no children needing dumping.
	 */
	if ( pruneneeded ) {
		bool_t	rootdump = BOOL_FALSE;

		mlog( MLOG_VERBOSE | MLOG_INOMAP, _(
		      "ino map phase 2: "
		      "pruning unneeded subtrees\n") );
		*inomap_statdonep = 0;
		*inomap_statpassp = 0;
		*inomap_statphasep = 2;

		(void) supprt_prune( &rootdump,
				     fshandlep,
				     fsfd,
				     rootstatp,
				     NULL );
		*inomap_statphasep = 0;

		if ( preemptchk( PREEMPT_FULL )) {
			cb_context_free();
			free( ( void * )bstatbufp );
			return BOOL_FALSE;
		}

	} else {
		mlog( MLOG_VERBOSE | MLOG_INOMAP, _(
		      "ino map phase 2: "
		      "skipping (no pruning necessary)\n") );
	}

	/* initialize the callback context for startpoint calculation
	 */
	cb_spinit( );

	/* identify dump stream startpoints
	 */
	if ( startptcnt > 1 ) {
		mlog( MLOG_VERBOSE | MLOG_INOMAP, _(
		      "ino map phase 3: "
		      "identifying stream starting points\n") );
	} else {
		mlog( MLOG_VERBOSE | MLOG_INOMAP, _(
		      "ino map phase 3: "
		      "skipping (only one dump stream)\n") );
	}
	stat = 0;
	*inomap_statdonep = 0;
	*inomap_statphasep = 3;
	rval = bigstat_iter( fshandlep,
			     fsfd,
			     BIGSTAT_ITER_NONDIR,
			     ( xfs_ino_t )0,
			     cb_startpt,
			     NULL,
			     inomap_next_nondir,
			     inomap_alloc_context(),
			     &stat,
			     preemptchk,
			     bstatbufp,
			     bstatbuflen );
	*inomap_statphasep = 0;
	
	if ( rval ) {
		cb_context_free();
		free( ( void * )bstatbufp );
		return BOOL_FALSE;
	}

	if ( startptcnt > 1 ) {
		ix_t startptix;
		for ( startptix = 0 ; startptix < startptcnt ; startptix++ ) {
			startpt_t *p;
			startpt_t *ep;

			p = &startptp[ startptix ];
			if ( startptix == startptcnt - 1 ) {
				ep = 0;
			} else {
				ep = &startptp[ startptix + 1 ];
			}
			assert( ! p->sp_flags );
			mlog( MLOG_VERBOSE | MLOG_INOMAP,
			      _("stream %u: ino %llu offset %lld to "),
			      startptix,
			      p->sp_ino,
			      p->sp_offset );
			if ( ! ep ) {
				mlog( MLOG_VERBOSE | MLOG_BARE | MLOG_INOMAP,
				      _("end\n") );
			} else {
				mlog( MLOG_VERBOSE |  MLOG_BARE | MLOG_INOMAP,
				      _("ino %llu offset %lld\n"),
				      ep->sp_ino,
				      ep->sp_offset );
			}
		}
	}

	cb_context_free();
	free( ( void * )bstatbufp );
	mlog( MLOG_VERBOSE | MLOG_INOMAP, _(
	      "ino map construction complete\n") );
	return BOOL_TRUE;
}

void
inomap_skip( xfs_ino_t ino )
{
	int oldstate;

	oldstate = inomap_get_state( NULL, ino );
	if ( oldstate == MAP_NDR_CHANGE) {
		inomap_set_state( NULL, ino, MAP_NDR_NOCHNG );
	}

	if ( oldstate == MAP_DIR_CHANGE
	     ||
	     oldstate == MAP_DIR_SUPPRT ) {
		inomap_set_state( NULL, ino, MAP_DIR_NOCHNG );
	}
}


/* definition of locally defined static functions ****************************/

/* callback context and operators - inomap_build makes extensive use
 * of iterators. below are the callbacks given to these iterators.
 */
static bool_t cb_last;		/* set by cb_context() */
static time32_t cb_lasttime;	/* set by cb_context() */
static bool_t cb_resume;	/* set by cb_context() */
static time32_t cb_resumetime;	/* set by cb_context() */
static size_t cb_resumerangecnt;/* set by cb_context() */
static drange_t *cb_resumerangep;/* set by cb_context() */
static void *cb_inomap_contextp;/* set by cb_context() */
static startpt_t *cb_startptp;	/* set by cb_context() */
static size_t cb_startptcnt;	/* set by cb_context() */
static size_t cb_startptix;	/* set by cb_spinit(), incr. by cb_startpt */
static off64_t cb_datasz;	/* set by cb_context() */
static off64_t cb_hdrsz;	/* set by cb_context() */
static off64_t cb_accum;	/* set by cb_context(), cb_spinit() */
static off64_t cb_incr;		/* set by cb_spinit(), used by cb_startpt() */
static off64_t cb_target;	/* set by cb_spinit(), used by cb_startpt() */
static off64_t cb_dircnt;	/* number of dirs CHANGED or PRUNE */
static off64_t cb_nondircnt;	/* number of non-dirs CHANGED */
static bool_t *cb_pruneneededp; /* set by cb_context() */
static bool_t cb_skip_unchanged_dirs;	/* set by cb_context() */

/* cb_context - initializes the call back context for the add and prune
 * phases of inomap_build().
 */
static int
cb_context( bool_t last,
	    time32_t lasttime,
	    bool_t resume,
	    time32_t resumetime,
	    size_t resumerangecnt,
	    drange_t *resumerangep,
	    startpt_t *startptp,
	    size_t startptcnt,
	    int igrpcnt,
	    bool_t skip_unchanged_dirs,
	    bool_t *pruneneededp )
{
	cb_last = last;
	cb_lasttime = lasttime;
	cb_resume = resume;
	cb_resumetime = resumetime;
	cb_resumerangecnt = resumerangecnt;
	cb_resumerangep = resumerangep;
	cb_startptp = startptp;
	cb_startptcnt = startptcnt;
	cb_accum = 0;
	cb_dircnt = 0;
	cb_nondircnt = 0;
	cb_pruneneededp = pruneneededp;
	cb_skip_unchanged_dirs = skip_unchanged_dirs;

	if (inomap_init( igrpcnt ))
		return -1;

	cb_inomap_contextp = inomap_alloc_context();
	if (!cb_inomap_contextp)
		return -1;

	return 0;
}

static void
cb_context_free( void )
{
	inomap_free_context( cb_inomap_contextp );
}

static int
cb_count_inogrp( void *arg1, int fsfd, xfs_inogrp_t *inogrp )
{
	int *count = (int *)arg1;
	(*count)++;
	return 0;
}

/* cb_add - called for all inodes in the file system. checks
 * mod and create times to decide if should be dumped. sets all
 * unmodified directories to be dumped for supprt. notes if any
 * files or directories have not been modified.
 */
/* ARGSUSED */
static int
cb_add( void *arg1,
	jdm_fshandle_t *fshandlep,
	int fsfd,
	xfs_bstat_t *statp )
{
	register time32_t mtime = statp->bs_mtime.tv_sec;
	register time32_t ctime = statp->bs_ctime.tv_sec;
	register time32_t ltime = max( mtime, ctime );
	register mode_t mode = statp->bs_mode & S_IFMT;
	xfs_off_t estimated_size = 0;
	xfs_ino_t ino = statp->bs_ino;
	bool_t changed;
	bool_t resumed;

	( *inomap_statdonep )++;

	/* skip if no links
	 */
	if ( statp->bs_nlink == 0 ) {
		return 0;
	}

	/* if no portion of this ino is in the resume range,
	 * then only dump it if it has changed since the interrupted
	 * dump.
	 *
	 * otherwise, if some or all of this ino is in the resume range,
	 * and has changed since the base dump upon which the original
	 * increment was based, dump it if it has changed since that
	 * original base dump.
	 */
	if ( cb_resume && ! cb_inoinresumerange( ino )) {
		if ( ltime >= cb_resumetime ) {
			changed = BOOL_TRUE;
		} else {
			changed = BOOL_FALSE;
		}
	} else if ( cb_last ) {
		if ( ltime >= cb_lasttime ) {
			changed = BOOL_TRUE;
		} else {
			changed = BOOL_FALSE;
		}
	} else {
		changed = BOOL_TRUE;
	}

	/* this is redundant: make sure any ino partially dumped
	 * is completed.
	 */
	if ( cb_resume && cb_inoresumed( ino )) {
		resumed = BOOL_TRUE;
	} else {
		resumed = BOOL_FALSE;
	}

	if ( changed ) {
		if ( mode == S_IFDIR ) {
			inomap_add( cb_inomap_contextp,
				    ino,
				    (gen_t)statp->bs_gen,
				    MAP_DIR_CHANGE );
			cb_dircnt++;
		} else {
			estimated_size = estimate_dump_space( statp );

			/* skip if size is greater than prune size. quota
			 * files are exempt from the check.
			 */
			if ( maxdumpfilesize > 0 &&
			     estimated_size > maxdumpfilesize &&
			     !is_quota_file(statp->bs_ino) ) {
				mlog( MLOG_DEBUG | MLOG_EXCLFILES,
				      "pruned ino %llu, owner %u, estimated size %llu: maximum size exceeded\n",
				      statp->bs_ino,
				      statp->bs_uid,
				      estimated_size );
				inomap_add( cb_inomap_contextp,
					    ino,
					    (gen_t)statp->bs_gen,
					    MAP_NDR_NOCHNG );
				inomap_exclude_filesize++;
				return 0;
			}

			if (allowexcludefiles_pr && statp->bs_xflags & XFS_XFLAG_NODUMP) {
				mlog( MLOG_DEBUG | MLOG_EXCLFILES,
				      "pruned ino %llu, owner %u, estimated size %llu: skip flag set\n",
				      statp->bs_ino,
				      statp->bs_uid,
				      estimated_size );
				inomap_add( cb_inomap_contextp,
					    ino,
					    (gen_t)statp->bs_gen,
					    MAP_NDR_NOCHNG );
				inomap_exclude_skipattr++;
				return 0;
			}

			inomap_add( cb_inomap_contextp,
				    ino,
				    (gen_t)statp->bs_gen,
				    MAP_NDR_CHANGE );
			cb_nondircnt++;
			cb_datasz += estimated_size;
			cb_hdrsz += ( EXTENTHDR_SZ * (statp->bs_extents + 1) );
		}
	} else if ( resumed ) {
		assert( mode != S_IFDIR );
		assert( changed );
	} else {
		if ( mode == S_IFDIR ) {
			if ( cb_skip_unchanged_dirs ) {
				inomap_add( cb_inomap_contextp,
					    ino,
					    (gen_t)statp->bs_gen,
					    MAP_DIR_NOCHNG );
			} else {
				*cb_pruneneededp = BOOL_TRUE;
				inomap_add( cb_inomap_contextp,
					    ino,
					    (gen_t)statp->bs_gen,
					    MAP_DIR_SUPPRT );
				cb_dircnt++;
			}
		} else {
			inomap_add( cb_inomap_contextp,
				    ino,
				    (gen_t)statp->bs_gen,
				    MAP_NDR_NOCHNG );
		}
	}

	return 0;
}

static bool_t
cb_inoinresumerange( xfs_ino_t ino )
{
	register size_t streamix;

	for ( streamix = 0 ; streamix < cb_resumerangecnt ; streamix++ ) {
		register drange_t *rp = &cb_resumerangep[ streamix ];
		if ( ! ( rp->dr_begin.sp_flags & STARTPT_FLAGS_END )
		     &&
		     ino >= rp->dr_begin.sp_ino
		     &&
		     ( ( rp->dr_end.sp_flags & STARTPT_FLAGS_END )
		       ||
		       ino < rp->dr_end.sp_ino
		       ||
		       ( ino == rp->dr_end.sp_ino
			 &&
			 rp->dr_end.sp_offset != 0 ))) {
			return BOOL_TRUE;
		}
	}

	return BOOL_FALSE;
}

static bool_t
cb_inoresumed( xfs_ino_t ino )
{
	size_t streamix;

	for ( streamix = 0 ; streamix < cb_resumerangecnt ; streamix++ ) {
		drange_t *rp = &cb_resumerangep[ streamix ];
		if ( ! ( rp->dr_begin.sp_flags & STARTPT_FLAGS_END )
		     &&
		     ino == rp->dr_begin.sp_ino
		     &&
		     rp->dr_begin.sp_offset != 0 ) {
			return BOOL_TRUE;
		}
	}

	return BOOL_FALSE;
}

/* supprt_prune -  does supprt directory entry pruning.
 * recurses downward looking for modified inodes, & clears supprt
 * (-> nochng) on the way back up after examining all descendents.
 */
/* ARGSUSED */
static bool_t			/* false, used as diriter callback */
supprt_prune( void *arg1,	/* ancestors marked as changed? */
	      jdm_fshandle_t *fshandlep,
	      int fsfd,
	      xfs_bstat_t *statp,
	      char *name )
{
	static bool_t cbrval = BOOL_FALSE;
	int state;

	if ( ( statp->bs_mode & S_IFMT ) == S_IFDIR ) {
		bool_t changed_below = BOOL_FALSE;

		state = inomap_get_state( cb_inomap_contextp, statp->bs_ino );
		if ( state != MAP_DIR_CHANGE &&
                     state != MAP_DIR_NOCHNG &&
		     state != MAP_DIR_SUPPRT) {
			/* 
			 * if file is now a dir then it has
			 * certainly changed.
			 */
			state = MAP_DIR_CHANGE;
			inomap_set_state( cb_inomap_contextp,
					  statp->bs_ino,
					  state );
		}

		( void )diriter( fshandlep,
				 fsfd,
				 statp,
				 supprt_prune,
				 (void *)&changed_below,
				 &cbrval,
				 NULL,
				 0 );

		if ( state == MAP_DIR_SUPPRT ) {
			if ( changed_below == BOOL_FALSE ) {
				inomap_set_state( cb_inomap_contextp,
						  statp->bs_ino,
						  MAP_DIR_NOCHNG );
				cb_dircnt--;	/* dump size just changed! */
			}
			else {
				/* Directory entries back up the hierarchy */
				/* to be dumped - as either MAP_DIR_SUPPRT */
				/* or as MAP_DIR_CHANGE in inode state map */
				*( bool_t * )arg1 = BOOL_TRUE;
			}
		}
		else if ( state == MAP_DIR_CHANGE ) {
			/* Directory entries back up the hierarchy must get */
			/* dumped - as either MAP_DIR_SUPPRT/MAP_DIR_CHANGE */
			*( bool_t * )arg1 = BOOL_TRUE;
		}
		return cbrval;
	}

	if ( *(bool_t *)arg1 == BOOL_TRUE ) {	/* shortcut, sibling changed */
		return cbrval;
	}

	state = inomap_get_state( cb_inomap_contextp, statp->bs_ino );
	if ( state != MAP_NDR_CHANGE &&
	     state != MAP_NDR_NOCHNG ) {
		/* 
		 * if dir is now a file then it has
		 * certainly changed.
		 */
		state = MAP_NDR_CHANGE;
		inomap_set_state( cb_inomap_contextp, statp->bs_ino, state );
	}
	if ( state == MAP_NDR_CHANGE ) {
		/* Directory entries back up the hierarchy must get */
		/* dumped - as either MAP_DIR_SUPPRT/MAP_DIR_CHANGE */
		*( bool_t * )arg1 = BOOL_TRUE;
	}
	return cbrval;
}


static void
cb_accuminit_sz( void )
{
	cb_datasz = 0;
	cb_hdrsz = 0;
}

/* cb_spinit - initializes context for the startpoint calculation phase of
 * inomap_build. cb_startptix is the index of the next startpoint to
 * record. cb_incr is the dump space distance between each startpoint.
 * cb_target is the target accum value for the next startpoint.
 * cb_accum accumulates the dump space.
 */
static void
cb_spinit( void )
{
	cb_startptix = 0;
	cb_incr = (cb_datasz + cb_hdrsz) / ( off64_t )cb_startptcnt;
	cb_target = 0; /* so first ino will push us over the edge */
	cb_accum = 0;
}

/* cb_startpt - called for each non-directory inode. accumulates the
 * require dump space, and notes startpoints. encodes a heuristic for
 * selecting startpoints. decides for each file whether to include it
 * in the current stream, start a new stream beginning with that file,
 * or split the file between the old and new streams. in the case of
 * a split decision, always split at a BBSIZE boundary.
 */
#define TOO_SHY		1000000	/* max accept. accum short of target */
#define TOO_BOLD	1000000	/* max accept. accum beyond target */

typedef enum {
	HOLD,	/* don't change */
	BUMP,	/* set a new start point and put this file after it */
	SPLIT,	/* set a new start point and split this file across it */
	YELL	/* impossible condition; complain */
} action_t;

/* ARGSUSED */
static int
cb_startpt( void *arg1,
	    jdm_fshandle_t *fshandlep,
	    int fsfd,
	    xfs_bstat_t *statp )
{
	register int state;

	off64_t estimate;
	off64_t old_accum = cb_accum;
	off64_t qty;	/* amount of a SPLIT file to skip */
	action_t action;

	( *inomap_statdonep )++;

	/* skip if no links
	 */
	if ( statp->bs_nlink == 0 ) {
		return 0;
	}

	/* skip if not in inomap or not a non-dir
	 */
	state = inomap_get_state( cb_inomap_contextp, statp->bs_ino );
	if ( state != MAP_NDR_CHANGE ) {
		return 0;
	}

	assert( cb_startptix < cb_startptcnt );

	estimate = estimate_dump_space( statp );
	cb_accum += estimate + ( EXTENTHDR_SZ * (statp->bs_extents + 1) );

	/* loop until no new start points found. loop is necessary
	 * to handle the pathological case of a huge file so big it
	 * spans several streams.
	 */
	action = ( action_t )HOLD; /* irrelevant, but demanded by lint */
	do {
		/* decide what to do: hold, bump, or split. there are
		 * 8 valid cases to consider:
		 * 1) accum prior to this file is way too short of the
		 *    target, and accum incl. this file is also shy: HOLD;
		 * 2) accum prior to this file is way too short of the
		 *    target, and accum incl. this file is close to but
		 *    still short of target: HOLD;
		 * 3) accum prior to this file is way too short of the
		 *    target, and accum incl. this file is a little beyond
		 *    the target: HOLD;
		 * 4) accum prior to this file is way too short of the
		 *    target, and accum incl. this file is way beyond
		 *    the target: SPLIT;
		 * 5) accum prior to this file is close to target, and
		 *    accum incl. this file is still short of target: HOLD;
		 * 6) accum prior to this file is close to target, and
		 *    accum incl. this file is a little beyond the target,
		 *    and excluding this file would be less short of target
		 *    than including it would be beyond the target: BUMP;
		 * 7) accum prior to this file is close to target, and
		 *    accum incl. this file is a little beyond the target,
		 *    and including this file would be less beyond target
		 *    than excluding it would be short of target: HOLD;
		 * 8) accum prior to this file is close to target, and
		 *    accum incl. this file is would be way beyond the
		 *    target: HOLD.
		 */
		if ( cb_target - old_accum >= TOO_SHY ) {
			if ( cb_target - cb_accum >= TOO_SHY ) {
				action = ( action_t )HOLD;
			} else if ( cb_accum <= cb_target ) {
				action = ( action_t )HOLD;
			} else if ( cb_accum - cb_target < TOO_BOLD ) {
				action = ( action_t )HOLD;
			} else {
				action = ( action_t )SPLIT;
			}
		} else {
			if ( cb_target - cb_accum >= TOO_SHY ) {
				action = ( action_t )YELL;
			} else if ( cb_accum < cb_target ) {
				action = ( action_t )HOLD;
			} else if ( cb_accum - cb_target < TOO_BOLD ) {
				if ( cb_accum - cb_target >=
						      cb_target - old_accum ) {
					action = ( action_t )BUMP;
				} else {
					action = ( action_t )HOLD;
				}
			} else {
				action = ( action_t )BUMP;
			}
		}

		/* perform the action selected above
		 */
		switch ( action ) {
		case ( action_t )HOLD:
			break;
		case ( action_t )BUMP:
			cb_startptp->sp_ino = statp->bs_ino;
			cb_startptp->sp_offset = 0;
			cb_startptix++;
			cb_startptp++;
			cb_target += cb_incr;
			if ( cb_startptix == cb_startptcnt ) {
				return 1; /* done; abort the iteration */
			}
			break;
		case ( action_t )SPLIT:
			cb_startptp->sp_ino = statp->bs_ino;
			qty = ( cb_target - old_accum )
			      &
			      ~( off64_t )( BBSIZE - 1 );
			cb_startptp->sp_offset =
					quantity2offset( fshandlep,
							 statp,
							 qty );
			cb_startptix++;
			cb_startptp++;
			cb_target += cb_incr;
			if ( cb_startptix == cb_startptcnt ) {
				return 1; /* done; abort the iteration */
			}
			break;
		default:
			assert( 0 );
			return 1;
		}
	} while ( action == ( action_t )BUMP || action == ( action_t )SPLIT );

	return 0;
}

/* map context and operators
 */

/* define structure for ino to gen mapping.
 */
struct i2gseg {
	uint64_t s_valid;
	gen_t s_gen[ INOPERSEG ];
};
typedef struct i2gseg i2gseg_t;

typedef struct seg_addr {
	int hnkoff;
	int segoff;
	int inooff;
} seg_addr_t;

static struct inomap {
	hnk_t *hnkmap;
	int hnkmaplen;
	i2gseg_t *i2gmap;
	seg_addr_t lastseg;
} inomap;

static inline void
SEG_SET_BITS( seg_t *segp, xfs_ino_t ino, int state )
{
	register xfs_ino_t relino;
	register uint64_t mask;
	register uint64_t clrmask;
	relino = ino - segp->base;
	mask = ( uint64_t )1 << relino;
	clrmask = ~mask;
	switch( state ) {
	case 0:
		segp->lobits &= clrmask;
		segp->mebits &= clrmask;
		segp->hibits &= clrmask;
		break;
	case 1:
		segp->lobits |= mask;
		segp->mebits &= clrmask;
		segp->hibits &= clrmask;
		break;
	case 2:
		segp->lobits &= clrmask;
		segp->mebits |= mask;
		segp->hibits &= clrmask;
		break;
	case 3:
		segp->lobits |= mask;
		segp->mebits |= mask;
		segp->hibits &= clrmask;
		break;
	case 4:
		segp->lobits &= clrmask;
		segp->mebits &= clrmask;
		segp->hibits |= mask;
		break;
	case 5:
		segp->lobits |= mask;
		segp->mebits &= clrmask;
		segp->hibits |= mask;
		break;
	case 6:
		segp->lobits &= clrmask;
		segp->mebits |= mask;
		segp->hibits |= mask;
		break;
	case 7:
		segp->lobits |= mask;
		segp->mebits |= mask;
		segp->hibits |= mask;
		break;
	}
}

static inline int
SEG_GET_BITS( seg_t *segp, xfs_ino_t ino )
{
	int state;
	register xfs_ino_t relino;
	register uint64_t mask;
	relino = ino - segp->base;
	mask = ( uint64_t )1 << relino;
	if ( segp->lobits & mask ) {
		state = 1;
	} else {
		state = 0;
	}
	if ( segp->mebits & mask ) {
		state |= 2;
	}
	if ( segp->hibits & mask ) {
		state |= 4;
	}

	return state;
}

/* context for inomap construction - initialized by map_init
 */
static int
inomap_init( int igrpcnt )
{
	assert( sizeof( hnk_t ) == HNKSZ );

	/* lastseg must be initialized with -1 offsets since
	 * no segments have been added yet */
	inomap.lastseg.hnkoff = -1;
	inomap.lastseg.segoff = -1;
	inomap.hnkmaplen = (igrpcnt + SEGPERHNK - 1) / SEGPERHNK;
	inomap.hnkmap = (hnk_t *)malloc(inomap.hnkmaplen * HNKSZ);
	inomap.i2gmap = (i2gseg_t *)
		calloc( inomap.hnkmaplen * SEGPERHNK, sizeof(i2gseg_t) );
	if (!inomap.hnkmap || !inomap.i2gmap)
		return -1;
	return 0;
}

uint64_t
inomap_getsz( void )
{
	return (inomap.lastseg.hnkoff + 1) * HNKSZ;
}

static inline bool_t
inomap_validaddr( seg_addr_t *addrp )
{
	int maxseg;

	if ( addrp->hnkoff < 0 || addrp->hnkoff > inomap.lastseg.hnkoff )
		return BOOL_FALSE;

	maxseg = ( addrp->hnkoff == inomap.lastseg.hnkoff ) ?
			inomap.lastseg.segoff : SEGPERHNK - 1;

	if ( addrp->segoff < 0 || addrp->segoff > maxseg )
		return BOOL_FALSE;

	return BOOL_TRUE;
}

static inline hnk_t *
inomap_addr2hnk( seg_addr_t *addrp )
{
	return &inomap.hnkmap[addrp->hnkoff];
}

static inline seg_t *
inomap_addr2seg( seg_addr_t *addrp )
{
	hnk_t *hunkp = inomap_addr2hnk( addrp );
	return &hunkp->seg[addrp->segoff];
}

static inline int
inomap_addr2segix( seg_addr_t *addrp )
{
	return ( addrp->hnkoff * SEGPERHNK ) + addrp->segoff;
}

static inline int
inomap_lastseg( int hnkoff )
{
	if ( hnkoff == inomap.lastseg.hnkoff )
		return inomap.lastseg.segoff;
	else
		return SEGPERHNK - 1;
}

/* called for every inode group in the filesystem in increasing inode
 * order. adds a new segment to the inomap and ino-to-gen map.
 */
static int
cb_add_inogrp( void *arg1, int fsfd, xfs_inogrp_t *inogrp )
{
	hnk_t *hunk;
	seg_t *segp;
	seg_addr_t *lastsegp = &inomap.lastseg;

	/* if out of segments on the current hnk or this is the first segment */
	lastsegp->segoff++;
	if (lastsegp->segoff == SEGPERHNK || lastsegp->segoff == 0) {
		lastsegp->hnkoff++;
		lastsegp->segoff = 0;

		if (lastsegp->hnkoff == inomap.hnkmaplen) {
			int numsegs;

			inomap.hnkmaplen++;
			inomap.hnkmap = (hnk_t *)
				realloc(inomap.hnkmap, inomap.hnkmaplen * HNKSZ);

			numsegs = inomap.hnkmaplen * SEGPERHNK;
			if (numsegs < 0)
				return -1;
			inomap.i2gmap = (i2gseg_t *)
				realloc(inomap.i2gmap, numsegs * sizeof(i2gseg_t));

			if (!inomap.hnkmap || !inomap.i2gmap)
				return -1;

			/* zero the new portion of the i2gmap */
			memset(&inomap.i2gmap[numsegs - SEGPERHNK], 0,
			       SEGPERHNK * sizeof(i2gseg_t));
		}

		memset(inomap_addr2hnk( lastsegp ), 0, HNKSZ);
	}

	hunk = inomap_addr2hnk( lastsegp );
	hunk->maxino = inogrp->xi_startino + INOPERSEG - 1;

	segp = inomap_addr2seg( lastsegp );
	segp->base = inogrp->xi_startino;

	return 0;
}

/* called for every ino to be added to the map.
 */
static void
inomap_add( void *contextp, xfs_ino_t ino, gen_t gen, int state )
{
	inomap_set_state( contextp, ino, state );
	inomap_set_gen( contextp, ino, gen );
}

void *
inomap_alloc_context( void )
{
	void *addr = calloc( 1, sizeof(seg_addr_t) );
	if (!addr) {
		mlog( MLOG_NORMAL | MLOG_ERROR,
		      _("failed to allocate inomap context: %s\n"),
		      strerror(errno) );
	}
	return addr;
}

void
inomap_reset_context( void *p )
{
	memset( p, 0, sizeof(seg_addr_t) );
}

void
inomap_free_context( void *p )
{
	free( p );
}

/* use binary search to find the hunk containing the given inode.
 * use the supplied addr as the starting point for the search.
 */
static bool_t
inomap_find_hnk( seg_addr_t *addrp, xfs_ino_t ino )
{
	hnk_t *hunkp;
	int lower;
	int upper;

	lower = 0;
	upper = inomap.lastseg.hnkoff;
	while ( upper >= lower ) {
		hunkp = inomap_addr2hnk( addrp );

		if ( hunkp->seg[0].base > ino ) {
			upper = addrp->hnkoff - 1;
		} else if ( hunkp->maxino < ino ) {
			lower = addrp->hnkoff + 1;
		} else {
			return BOOL_TRUE;
		}

		addrp->hnkoff = lower + ((upper - lower) / 2);
		addrp->segoff = 0;
	}

	return BOOL_FALSE;
}

/* use binary search to find the hunk containing the given
 * inode, and then binary search the hunk to find the correct
 * segment, if any. use the supplied addr as the starting
 * point for the search.
 */
static bool_t
inomap_find_seg( seg_addr_t *addrp, xfs_ino_t ino )
{
	seg_t *segp;
	int lower;
	int upper;

	if ( !inomap_validaddr( addrp ) ) {
		inomap_reset_context( addrp );
	}

	if ( !inomap_find_hnk( addrp, ino ) )
		return BOOL_FALSE;

	/* find the correct segment */
	lower = 0;
	upper = inomap_lastseg(addrp->hnkoff);

	while ( upper >= lower ) {
		segp = inomap_addr2seg( addrp );

		if ( segp->base > ino ) {
			upper = addrp->segoff - 1;
		} else if ( segp->base + INOPERSEG <= ino ) {
			lower = addrp->segoff + 1;
		} else {
			return BOOL_TRUE;
		}

		addrp->segoff = lower + ((upper - lower) / 2);
	}

	return BOOL_FALSE;
}

static xfs_ino_t
inomap_iter( void *contextp, int statemask )
{
	xfs_ino_t ino, endino;
	seg_t *segp;
	seg_addr_t *addrp = (seg_addr_t *)contextp;

	for ( ;
	      addrp->hnkoff <= inomap.lastseg.hnkoff;
	      addrp->hnkoff++, addrp->segoff = 0, addrp->inooff = 0 ) {

		for ( ;
		      addrp->segoff <= inomap_lastseg(addrp->hnkoff);
		      addrp->segoff++, addrp->inooff = 0 ) {

			segp = inomap_addr2seg( addrp );

			ino = segp->base + addrp->inooff;
			endino = segp->base + INOPERSEG;
			for ( ; ino < endino ; ino++, addrp->inooff++ ) {
				int st;
				st = SEG_GET_BITS( segp, ino );
				if ( statemask & ( 1 << st )) {
					addrp->inooff++; /* for next call */
					return ino;
				}
			}
		}
	}

	return INO64MAX;
}

xfs_ino_t
inomap_next_nondir(void *contextp, xfs_ino_t lastino)
{
	int state = 1 << MAP_NDR_CHANGE;
	xfs_ino_t nextino;

	do {
		nextino = inomap_iter(contextp, state);
	} while (nextino <= lastino);

	return nextino;
}

xfs_ino_t
inomap_next_dir(void *contextp, xfs_ino_t lastino)
{
	int state = (1 << MAP_DIR_CHANGE) | (1 << MAP_DIR_SUPPRT);
	xfs_ino_t nextino;

	do {
		nextino = inomap_iter(contextp, state);
	} while (nextino <= lastino);

	return nextino;
}

static int
inomap_set_state( void *contextp, xfs_ino_t ino, int state )
{
	int oldstate;
	seg_addr_t *addrp;
	seg_addr_t addr;
	seg_t *segp;

	addrp = contextp ? (seg_addr_t *)contextp : &addr;
	if ( !inomap_find_seg( addrp, ino ) )
		return MAP_INO_UNUSED;

	segp = inomap_addr2seg( addrp );

	oldstate = SEG_GET_BITS( segp, ino );
	SEG_SET_BITS( segp, ino, state );

	return oldstate;
}

int
inomap_get_state( void *contextp, xfs_ino_t ino )
{
	seg_addr_t *addrp;
	seg_addr_t addr;
	seg_t *segp;

	addrp = contextp ? (seg_addr_t *)contextp : &addr;
	if ( !inomap_find_seg( addrp, ino ) )
		return MAP_INO_UNUSED;

	segp = inomap_addr2seg( addrp );

	return SEG_GET_BITS( segp, ino );
}

static void
inomap_set_gen(void *contextp, xfs_ino_t ino, gen_t gen)
{
	seg_addr_t *addrp;
	seg_addr_t addr;
	seg_t *segp;
	i2gseg_t *i2gsegp;
	xfs_ino_t relino;

	addrp = contextp ? (seg_addr_t *)contextp : &addr;
	if ( !inomap_find_seg( addrp, ino ) )
		return;

	segp = inomap_addr2seg( addrp );
	i2gsegp = &inomap.i2gmap[inomap_addr2segix( addrp )];

	relino = ino - segp->base;
	i2gsegp->s_valid |= (uint64_t)1 << relino;
	i2gsegp->s_gen[relino] = gen;
}

int
inomap_get_gen( void *contextp, xfs_ino_t ino, gen_t *gen )
{
	seg_addr_t *addrp;
	seg_addr_t addr;
	seg_t *segp;
	i2gseg_t *i2gsegp;
	xfs_ino_t relino;

	addrp = contextp ? (seg_addr_t *)contextp : &addr;
	if ( !inomap_find_seg( addrp, ino ) )
		return 1;

	segp = inomap_addr2seg( addrp );
	i2gsegp = &inomap.i2gmap[inomap_addr2segix( addrp )];

	relino = ino - segp->base;
	if ( ! (i2gsegp->s_valid & ((uint64_t)1 << relino)) )
		return 1;

	*gen = i2gsegp->s_gen[relino];
	return 0;
}

void
inomap_writehdr( content_inode_hdr_t *scwhdrp )
{
	/* update the inomap info in the content header
	 */
	scwhdrp->cih_inomap_hnkcnt = inomap.lastseg.hnkoff + 1;
	scwhdrp->cih_inomap_segcnt = inomap_addr2segix( &inomap.lastseg ) + 1;
	scwhdrp->cih_inomap_dircnt = ( uint64_t )cb_dircnt;
	scwhdrp->cih_inomap_nondircnt = ( uint64_t )cb_nondircnt;
	scwhdrp->cih_inomap_firstino = inomap.hnkmap[0].seg[ 0 ].base;
	scwhdrp->cih_inomap_lastino = inomap.hnkmap[inomap.lastseg.hnkoff].maxino;
	scwhdrp->cih_inomap_datasz = ( uint64_t )cb_datasz;
}

rv_t
inomap_dump( drive_t *drivep )
{
	seg_addr_t addr;
	hnk_t *hnkp;
	hnk_t tmphnkp;

	/* use write_buf to dump the hunks
	 */
	for ( addr.hnkoff = 0 ;
	      addr.hnkoff <= inomap.lastseg.hnkoff ;
	      addr.hnkoff++ ) {
		int rval;
		rv_t rv;
		drive_ops_t *dop = drivep->d_opsp;

		hnkp = inomap_addr2hnk( &addr );

		xlate_hnk(hnkp, &tmphnkp, 1);
		rval = write_buf( ( char * )&tmphnkp,
				  sizeof( tmphnkp ),
				  ( void * )drivep,
				  ( gwbfp_t )dop->do_get_write_buf,
				  ( wfp_t )dop->do_write );
		switch ( rval ) {
		case 0:
			rv = RV_OK;
			break;
		case DRIVE_ERROR_MEDIA:
		case DRIVE_ERROR_EOM:
			rv = RV_EOM;
			break;
		case DRIVE_ERROR_EOF:
			rv = RV_EOF;
			break;
		case DRIVE_ERROR_DEVICE:
			rv = RV_DRIVE;
			break;
		case DRIVE_ERROR_CORE:
		default:
			rv = RV_CORE;
			break;
		}
		if ( rv != RV_OK ) {
			return rv;
		}
	}

	return RV_OK;
}

static int
subtreelist_parse( jdm_fshandle_t *fshandlep,
		   int fsfd,
		   xfs_bstat_t *rootstatp,
		   char *subtreebuf[],
		   ix_t subtreecnt )
{
	ix_t subtreeix;

	/* add the root ino to the dump
	 */
	cb_add( NULL, fshandlep, fsfd, rootstatp );

	/* do a recursive descent for each subtree specified
	 */
	for ( subtreeix = 0 ; subtreeix < subtreecnt ; subtreeix++ ) {
		int cbrval = 0;
		char *currentpath = subtreebuf[ subtreeix ];
		assert( *currentpath != '/' );
		( void )diriter( fshandlep,
				 fsfd,
				 rootstatp,
				 subtreelist_parse_cb,
				 ( void * )currentpath,
				 &cbrval,
				 NULL,
				 0 );
		if ( cbrval != 1 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_INOMAP,
			      "%s: %s\n",
			      cbrval == 0 ? _("subtree not present")
					  : _("invalid subtree specified"),
			      currentpath );
			return -1;
		}
	}

	return 0;
}

static int
subtreelist_parse_cb( void *arg1,
		      jdm_fshandle_t *fshandlep,
		      int fsfd,
		      xfs_bstat_t *statp,
		      char *name  )
{
	int cbrval = 0;

	/* arg1 is used to carry the tail of the subtree path
	 */
	char *subpath = ( char * )arg1;

	/* temporarily terminate the subpath at the next slash
	 */
	char *nextslash = strchr( subpath, '/' );
	if ( nextslash ) {
		*nextslash = 0;
	}

	/* if the first element of the subpath doesn't match this
	 * directory entry, try the next entry.
	 */
	if ( strcmp( subpath, name )) {
		if ( nextslash ) {
			*nextslash = '/';
		}
		return 0;
	}

	/* it matches, so add ino to list and continue down the path
	 */
	cb_add( NULL, fshandlep, fsfd, statp );

	if ( nextslash ) {

		/* if we're not at the end of the path, yet the current
		 * path element is not a directory, complain and abort the
		 * iteration in a way which terminates the application
		 */
		if ( ( statp->bs_mode & S_IFMT ) != S_IFDIR ) {
			*nextslash = '/';
			return 2;
		}

		/* repair the subpath
		*/
		*nextslash = '/';

		/* peel the first element of the subpath and recurse
		*/
		( void )diriter( fshandlep,
				 fsfd,
				 statp,
				 subtreelist_parse_cb,
				 ( void * )( nextslash + 1 ),
				 &cbrval,
				 NULL,
				 0 );
		return cbrval;

	} else {
		/* we've reached the specified subpath, so if we're
		 * at a directory, recurse down and add all children
		 * to the inomap.
		 */

		if ( ( statp->bs_mode & S_IFMT ) != S_IFDIR ) {
			return 1;
		}

		( void )diriter( fshandlep,
				 fsfd,
				 statp,
				 subtree_descend_cb,
				 NULL,
				 &cbrval,
				 0,
				 0 );
		return 1;
	}
}

static int
subtree_descend_cb( void *arg1,
		    jdm_fshandle_t *fshandlep,
		    int fsfd,
		    xfs_bstat_t *statp,
		    char *name  )
{
	int cbrval = 0;

	cb_add( NULL, fshandlep, fsfd, statp );

	if ( ( statp->bs_mode & S_IFMT ) == S_IFDIR ) {

		( void )diriter( fshandlep,
				 fsfd,
				 statp,
				 subtree_descend_cb,
				 NULL,
				 &cbrval,
				 NULL,
				 0 );
	}

	return cbrval;
}

/* uses the extent map to figure the first offset in the file
 * with qty real (non-hole) bytes behind it
 */
#define BMAP_LEN	512

static off64_t
quantity2offset( jdm_fshandle_t *fshandlep, xfs_bstat_t *statp, off64_t qty )
{
	int fd;
	getbmapx_t bmap[ BMAP_LEN ];
	off64_t offset;
	off64_t offset_next;
	off64_t qty_accum;

	/* If GETOPT_DUMPASOFFLINE was specified and the HSM provided an
	 * estimate, then use it.
	 */

	if (hsm_fs_ctxtp) {
		if (HsmEstimateFileOffset(hsm_fs_ctxtp, statp, qty, &offset))
			return offset;
	}

	offset = 0;
	offset_next = 0;
	qty_accum = 0;
	bmap[ 0 ].bmv_offset = 0;
	bmap[ 0 ].bmv_length = -1;
	bmap[ 0 ].bmv_count = BMAP_LEN;
	bmap[ 0 ].bmv_iflags = BMV_IF_NO_DMAPI_READ;
	bmap[ 0 ].bmv_entries = -1;
	fd = jdm_open( fshandlep, statp, O_RDONLY );
	if ( fd < 0 ) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_INOMAP, _(
		      "could not open ino %llu to read extent map: %s\n"),
		      statp->bs_ino,
		      strerror( errno ));
		return 0;
	}

	for ( ; ; ) {
		int eix;
		int rval;

		rval = ioctl( fd, XFS_IOC_GETBMAPX, bmap );
		if ( rval ) {
			mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_INOMAP, _(
			      "could not read extent map for ino %llu: %s\n"),
			      statp->bs_ino,
			      strerror( errno ));
			( void )close( fd );
			return 0;
		}

		if ( bmap[ 0 ].bmv_entries <= 0 ) {
			assert( bmap[ 0 ].bmv_entries == 0 );
			( void )close( fd );
			return offset_next;
		}

		for ( eix = 1 ; eix <= bmap[ 0 ].bmv_entries ; eix++ ) {
			getbmapx_t *bmapp = &bmap[ eix ];
			off64_t qty_new;
			if ( bmapp->bmv_block == -1 ) {
				continue; /* hole */
			}
			offset = bmapp->bmv_offset * BBSIZE;
			qty_new = qty_accum + bmapp->bmv_length * BBSIZE;
			if ( qty_new >= qty ) {
				( void )close( fd );
				return offset + ( qty - qty_accum );
			}
			offset_next = offset + bmapp->bmv_length * BBSIZE;
			qty_accum = qty_new;
		}
	}
	/* NOTREACHED */
}


static off64_t
estimate_dump_space( xfs_bstat_t *statp )
{
	switch ( statp->bs_mode & S_IFMT ) {
	case S_IFREG:
		/* very rough: must improve this.  If GETOPT_DUMPASOFFLINE was
		 * specified and the HSM provided an estimate, then use it.
		 */
		if (hsm_fs_ctxtp) {
			off64_t	bytes;
			int accurate;

			/* if -z or multiple streams are being used,
			 * we need an accurate estimate. otherwise a
			 * quick estimate will do.
			 */
			accurate = maxdumpfilesize || drivecnt > 1;

			if (HsmEstimateFileSpace(hsm_fs_ctxtp, NULL, statp, &bytes, accurate))
				return bytes;
		}
		return statp->bs_blocks * ( off64_t )statp->bs_blksize;
	case S_IFIFO:
	case S_IFCHR:
	case S_IFDIR:
#ifdef S_IFNAM
	case S_IFNAM:
#endif
	case S_IFBLK:
	case S_IFSOCK:
	case S_IFLNK:
	/* not yet
	case S_IFUUID:
	*/
		return 0;
	default:
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_INOMAP, _(
		      "unknown inode type: ino=%llu, mode=0x%04x 0%06o\n"),
		      statp->bs_ino,
		      statp->bs_mode,
		      statp->bs_mode );
		return 0;
	}
}
