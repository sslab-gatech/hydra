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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>
#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include "config.h"

#include "types.h"
#include "util.h"
#include "mlog.h"
#include "getdents.h"

extern size_t pgsz;

int
write_buf( char *bufp,
	   size_t bufsz,
	   void *contextp,
	   gwbfp_t get_write_buf_funcp,
	   wfp_t write_funcp )
{
	char *mbufp;	/* buffer obtained from manager */
	size_t mbufsz;/* size of buffer obtained from manager */

	while ( bufsz ) {
		int rval;

		assert( bufsz > 0 );
		mbufp = ( *get_write_buf_funcp )( contextp, bufsz, &mbufsz );
		assert( mbufsz <= bufsz );
		if ( bufp ) {
			(void)memcpy( ( void * )mbufp, ( void * )bufp, mbufsz );
		} else {
			(void)memset( ( void * )mbufp, 0, mbufsz );
		}
		rval = ( * write_funcp )( contextp, mbufp, mbufsz );
		if ( rval ) {
			return rval;
		}
		if ( bufp ) {
			bufp += mbufsz;
		}
		bufsz -= mbufsz;
	}

	return 0;
}

int
read_buf( char *bufp,
	  size_t bufsz, 
	  void *contextp,
	  rfp_t read_funcp,
	  rrbfp_t return_read_buf_funcp,
	  int *statp )
{
	char *mbufp;		/* manager's buffer pointer */
	size_t mbufsz;	/* size of manager's buffer */
	int nread;

	nread = 0;
	*statp = 0;
	while ( bufsz ) {
		mbufp = ( * read_funcp )( contextp, bufsz, &mbufsz, statp );
		if ( *statp ) {
			break;
		}
		assert( mbufsz <= bufsz );
		if ( bufp ) {
			( void )memcpy( (void *)bufp, (void *)mbufp, mbufsz );
			bufp += mbufsz;
		}
		bufsz -= mbufsz;
		nread += ( int )mbufsz;
		( * return_read_buf_funcp )( contextp, mbufp, mbufsz );
	}

	return nread;
}

char
*strncpyterm( char *s1, char *s2, size_t n )
{
	char *rval;

	if ( n < 1 ) return 0;
	rval = strncpy( s1, s2, n );
	s1[ n - 1 ] = 0;
	return rval;
}

int
bigstat_iter( jdm_fshandle_t *fshandlep,
	      int fsfd,
	      int selector,
	      xfs_ino_t start_ino,
	      bstat_cbfp_t fp,
	      void * cb_arg1,
	      bstat_seekfp_t seekfp,
	      void * seek_arg1,
	      int *statp,
	      bool_t ( pfp )( int ),
	      xfs_bstat_t *buf,
	      size_t buflenin )
{
	__s32 buflenout;
	xfs_ino_t lastino;
	int saved_errno;
	int bulkstatcnt;
        xfs_fsop_bulkreq_t bulkreq;

	/* stat set with return from callback func
	 */
	*statp = 0;

	/* NOT COOL: open will affect root dir's access time
	 */

	mlog( MLOG_NITTY,
	      "bulkstat iteration initiated: start_ino == %llu\n",
	      start_ino );

	/* quirk of the interface: I want to play in terms of the
	 * ino to begin with, and ino 0 is not used. so, ...
	 */
	if ( start_ino > 0 ) {
		lastino = start_ino - 1;
	} else {
		lastino = 0;
	}
	mlog( MLOG_NITTY + 1,
	      "calling bulkstat\n" );

	bulkstatcnt = 0;
	bulkreq.lastip = (__u64 *)&lastino;
	bulkreq.icount = buflenin;
	bulkreq.ubuffer = buf;
	bulkreq.ocount = &buflenout;
	while (!ioctl(fsfd, XFS_IOC_FSBULKSTAT, &bulkreq)) {
		xfs_bstat_t *p;
		xfs_bstat_t *endp;

		if ( buflenout == 0 ) {
			mlog( MLOG_NITTY + 1,
			      "bulkstat returns buflen %d\n",
			      buflenout );
			return 0;
		}
		mlog( MLOG_NITTY + 1,
		      "bulkstat returns buflen %d ino %llu\n",
		      buflenout,
		      buf->bs_ino );
		for ( p = buf, endp = buf + buflenout ; p < endp ; p++ ) {
			int rval;

			if ( p->bs_ino == 0 )
				continue;

			if ( !p->bs_nlink || !p->bs_mode ) {
				/* inode being modified, get synced data */
				mlog( MLOG_NITTY + 1,
				      "ino %llu needed second bulkstat\n",
				      p->bs_ino );

				if( bigstat_one( fsfd, p->bs_ino, p ) < 0 ) {
				    mlog( MLOG_WARNING,
					  _("failed to get bulkstat information for inode %llu\n"),
					  p->bs_ino );
				    continue;
				}
				if ( !p->bs_nlink || !p->bs_mode || !p->bs_ino ) {
					mlog( MLOG_TRACE,
					      _("failed to get valid bulkstat information for inode %llu\n"),
					      p->bs_ino );
					continue;
				}
			}

			if ( ( p->bs_mode & S_IFMT ) == S_IFDIR ) {
				if ( ! ( selector & BIGSTAT_ITER_DIR )){
					continue;
				}
			} else {
				if ( ! ( selector & BIGSTAT_ITER_NONDIR )){
					continue;
				}
			}
			rval = ( * fp )( cb_arg1, fshandlep, fsfd, p );
			if ( rval ) {
				*statp = rval;
				return 0;
			}
			if ( pfp ) ( pfp )( PREEMPT_PROGRESSONLY );
		}

		if ( pfp && (++bulkstatcnt % 10) == 0 &&
		     ( pfp )( PREEMPT_FULL )) {
			return EINTR;
		}

		if (seekfp) {
			lastino = seekfp(seek_arg1, lastino);
			if (lastino == INO64MAX) {
				mlog( MLOG_DEBUG,
				      "bulkstat seeked to EOS\n");
				return 0;
			}
			
			mlog( MLOG_DEBUG,
			      "bulkstat seeked to %llu\n", lastino);

			lastino = (lastino > 0) ? lastino - 1 : 0;
		}

		mlog( MLOG_NITTY + 1,
		      "calling bulkstat\n" );
	}

	saved_errno = errno;

	mlog( MLOG_NORMAL,
	      _("syssgi( SGI_FS_BULKSTAT ) on fsroot failed: %s\n"),
	      strerror( saved_errno ));

	return saved_errno;
}

/* ARGSUSED */
int
bigstat_one( int fsfd,
	     xfs_ino_t ino,
	     xfs_bstat_t *statp )
{
        xfs_fsop_bulkreq_t bulkreq;
	int count = 0;

	assert( ino > 0 );
        bulkreq.lastip = (__u64 *)&ino;
        bulkreq.icount = 1;
        bulkreq.ubuffer = statp;
        bulkreq.ocount = &count;
	return xfsctl(NULL, fsfd, XFS_IOC_FSBULKSTAT_SINGLE, &bulkreq);
}

/* call the given callback for each inode group in the filesystem.
 */
#define INOGRPLEN	256
int
inogrp_iter( int fsfd,
	     int ( * fp )( void *arg1,
				int fsfd,
				xfs_inogrp_t *inogrp ),
	     void * arg1,
	     int *statp )
{
	xfs_ino_t lastino;
	int inogrpcnt;
	xfs_inogrp_t *igrp;
        xfs_fsop_bulkreq_t bulkreq;

	/* stat set with return from callback func */
	*statp = 0;

	igrp = malloc(INOGRPLEN * sizeof(xfs_inogrp_t));
	if (!igrp) {
		mlog(MLOG_NORMAL | MLOG_ERROR,
		     _("malloc of stream context failed (%d bytes): %s\n"),
		     INOGRPLEN * sizeof(xfs_inogrp_t),
		     strerror(errno));
		return -1;
	}

	lastino = 0;
	inogrpcnt = 0;
	bulkreq.lastip = (__u64 *)&lastino;
	bulkreq.icount = INOGRPLEN;
	bulkreq.ubuffer = igrp;
	bulkreq.ocount = &inogrpcnt;
	while (!ioctl(fsfd, XFS_IOC_FSINUMBERS, &bulkreq)) {
		xfs_inogrp_t *p, *endp;

		if ( inogrpcnt == 0 ) {
			free(igrp);
			return 0;
		}
		for ( p = igrp, endp = igrp + inogrpcnt ; p < endp ; p++ ) {
			int rval;

			rval = ( * fp )( arg1, fsfd, p );
			if ( rval ) {
				*statp = rval;
				free(igrp);
				return 0;
			}
		}
	}

	free(igrp);
	return errno;
}

/* calls the callback for every entry in the directory specified
 * by the stat buffer. supplies the callback with a file system
 * handle and a stat buffer, and the name from the dirent.
 *
 * NOTE: does NOT invoke callback for "." or ".."!
 *
 * if the callback returns non-zero, returns 1 with cbrval set to the
 * callback's return value. if syscall fails, returns -1 with errno set.
 * otherwise returns 0.
 *
 * caller may supply a dirent buffer. if not, will malloc one
 */
int
diriter( jdm_fshandle_t *fshandlep,
	 int fsfd,
	 xfs_bstat_t *statp,
	 int ( *cbfp )( void *arg1,
			     jdm_fshandle_t *fshandlep,
			     int fsfd,
			     xfs_bstat_t *statp,
			     char *namep ),
	 void *arg1,
	 int *cbrvalp,
	 char *usrgdp,
	 size_t usrgdsz )
{
	size_t gdsz;
	struct dirent *gdp;
	int fd;
	int gdcnt;
	int scrval;
	int cbrval;

	if ( usrgdp ) {
		assert( usrgdsz >= sizeof( struct dirent ) );
		gdsz = usrgdsz;
		gdp = ( struct dirent * )usrgdp;
	} else {
		gdsz = pgsz;
		gdp = ( struct dirent * )malloc( gdsz );
		assert( gdp );
	}

	/* open the directory
	 */
	fd = jdm_open( fshandlep, statp, O_RDONLY );
	if ( fd < 0 ) {
		mlog( MLOG_NORMAL,
		      _("WARNING: unable to open directory ino %llu: %s\n"),
		      statp->bs_ino,
		      strerror( errno ));
		*cbrvalp = 0;
		if ( ! usrgdp ) {
			free( ( void * )gdp );
		}
		return -1;
	}
	assert( ( statp->bs_mode & S_IFMT ) == S_IFDIR );

	/* lots of buffering done here, to achieve OS-independence.
	 * if proves to be to much overhead, can streamline.
	 */
	scrval = 0;
	cbrval = 0;
	for ( gdcnt = 1 ; ; gdcnt++ ) {
		struct dirent *p;
		int nread;
		register size_t reclen;

		assert( scrval == 0 );
		assert( cbrval == 0 );

		nread = getdents_wrap( fd, (char *)gdp, gdsz );
		
		/* negative count indicates something very bad happened;
		 * try to gracefully end this dir.
		 */
		if ( nread < 0 ) {
			mlog( MLOG_NORMAL,
			      _("WARNING: unable to read dirents (%d) for "
			      "directory ino %llu: %s\n"),
			      gdcnt,
			      statp->bs_ino,
			      strerror( errno ));
			nread = 0; /* pretend we are done */
		}

		/* no more directory entries: break;
		 */
		if ( nread == 0 ) {
			break;
		}

		/* translate and invoke cb each entry: skip "." and "..".
		 */
		for ( p = gdp,
		      reclen = ( size_t )p->d_reclen
		      ;
		      nread > 0
		      ;
		      nread -= ( int )reclen,
		      assert( nread >= 0 ),
		      p = ( struct dirent * )( ( char * )p + reclen ),
		      reclen = ( size_t )p->d_reclen ) {
			xfs_bstat_t statbuf;
			assert( scrval == 0 );
			assert( cbrval == 0 );

			/* skip "." and ".."
			 */
			if ( *( p->d_name + 0 ) == '.'
			     &&
			     ( *( p->d_name + 1 ) == 0
			       ||
			       ( *( p->d_name + 1 ) == '.'
				 &&
				 *( p->d_name + 2 ) == 0 ))) {
				continue;
			}

			/* use bigstat
			 */
			scrval = bigstat_one( fsfd,
					      p->d_ino,
					      &statbuf );
			if ( scrval ) {
				mlog( MLOG_NORMAL,
				      _("WARNING: could not stat "
				      "dirent %s ino %llu: %s\n"),
				      p->d_name,
				      ( xfs_ino_t )p->d_ino,
				      strerror( errno ));
				scrval = 0;
				continue;
			}

			/* if getdents64() not yet available, and ino
			 * occupied more than 32 bits, warn and skip.
			 */
#ifndef __USE_LARGEFILE64
			if ( statbuf.bs_ino > ( xfs_ino_t )INOMAX ) {
				mlog( MLOG_NORMAL,
				      _("WARNING: unable to process dirent %s: "
				      "ino %llu too large\n"),
				      p->d_name,
				      ( xfs_ino_t )p->d_ino );
				continue;
			}
#endif

			/* invoke the callback
			 */
			cbrval = ( * cbfp )( arg1,
					     fshandlep,
					     fsfd,
					     &statbuf,
					     p->d_name );

			/* abort the iteration if the callback returns non-zero
			 */
			if ( cbrval ) {
				break;
			}
		}

		if ( scrval || cbrval ) {
			break;
		}
	}

	( void )close( fd );
	if ( ! usrgdp ) {
		free( ( void * )gdp );
	}

	if ( scrval ) {
		*cbrvalp = 0;
		return -1;
	} else if ( cbrval ) {
		*cbrvalp = cbrval;
		return 1;
	} else {
		*cbrvalp = 0;
		return 0;
	}
}

int 
cvtnum( int blocksize, char *s )
{
	int i;
	char *sp;

	i = (int)strtoll(s, &sp, 0);
	if (i == 0 && sp == s)
		return -1;
	if (*sp == '\0')
		return i;
	if (*sp == 'b' && blocksize && sp[1] == '\0')
		return i * blocksize;
	if (*sp == 'k' && sp[1] == '\0')
		return 1024 * i;
	if (*sp == 'm' && sp[1] == '\0')
		return 1024 * 1024 * i;
	return -1;
}
