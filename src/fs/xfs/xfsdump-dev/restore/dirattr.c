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

/*
 * Directory attributes are written on tape during the directory dump phase,
 * which precedes the non-dir dump phase. The directory attributes cannot be
 * restored, however, until all of the non-dir files have been restored
 * (directory timestamps would be wrong, directory inherit flags would
 * interfere, etc.) These routines allow the directory attributes to be stored
 * on disk when they are read from the dump stream so that they can be applied
 * at a later time.
 */

#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>

#include "config.h"

#include "types.h"
#include "util.h"
#include "mlog.h"
#include "global.h"
#include "drive.h"
#include "media.h"
#include "content.h"
#include "content_inode.h"
#include "dirattr.h"
#include "openutil.h"
#include "mmap.h"

/* structure definitions used locally ****************************************/

/* node handle limits
 */
#ifdef DIRATTRCHK

/* macros for manipulating dirattr handles when handle consistency
 * checking is enabled. the upper bits of a handle will be loaded
 * with a handle checksum.
 */
#define HDLSUMCNT		4
#define	HDLSUMSHIFT		( NBBY * sizeof ( dah_t ) - HDLSUMCNT )
#define	HDLSUMLOMASK		( ( 1 << HDLSUMCNT ) - 1 )
#define	HDLSUMMASK		( HDLSUMLOMASK << HDLSUMSHIFT )
#define HDLDIXCNT		HDLSUMSHIFT
#define HDLDIXMASK		( ( 1 << HDLDIXCNT ) - 1 )
#define HDLGETSUM( h )		( ( uint16_t )				\
				  ( ( ( int )h >> HDLSUMSHIFT )		\
				    &					\
				    HDLSUMLOMASK ))
#define HDLGETDIX( h )		( ( dix_t )( ( int )h & HDLDIXMASK ))
#define HDLMKHDL( s, d )	( ( dah_t )( ( ( ( int )s << HDLSUMSHIFT )\
					       &			\
					       HDLSUMMASK )		\
					  |				\
					  ( ( int )d & HDLDIXMASK )))
#define DIX_MAX			( ( off64_t )HDLDIXMASK )

/* each dirattr will hold two check fields: a handle checksum, and unique
 * pattern, to differentiate a valid dirattr from random file contents.
 */
#define DIRATTRUNQ		0xa116

#else /* DIRATTRCHK */

#define DIX_MAX			( ( ( off64_t )1			\
				    <<					\
				    ( ( off64_t )NBBY			\
				      *					\
				      ( off64_t )sizeof( dah_t )))	\
				  -					\
				  ( off64_t )2 ) /* 2 to avoid DAH_NULL */

#endif /* DIRATTRCHK */

/* dirattr definition
 */
struct dirattr {
#ifdef DIRATTRCHK
	uint16_t d_unq;
	uint16_t d_sum; 
#endif /* DIRATTRCHK */
	mode_t d_mode;
	uid_t d_uid;
	gid_t d_gid;
	time32_t d_atime;
	time32_t d_mtime;
	time32_t d_ctime;
	uint32_t d_xflags;
	uint32_t d_extsize;
	uint32_t d_projid;
	uint32_t d_dmevmask;
	uint32_t d_dmstate;
	off64_t d_extattroff;
};

typedef struct dirattr dirattr_t;

#define DIRATTR_EXTATTROFFNULL	( ( off64_t )OFF64MAX )

/* dirattr persistent context definition
 */
struct dirattr_pers {
	off64_t dp_appendoff;
};

typedef struct dirattr_pers dirattr_pers_t;

#define DIRATTR_PERS_SZ	pgsz

/* dirattr transient context definition
 */

#define	DIRATTR_BUFSIZE	32768

struct dirattr_tran {
	char *dt_pathname;
	int dt_fd;
	bool_t dt_at_endpr;
	dah_t dt_cachedh;
	dirattr_t dt_cached_dirattr;
	size_t dt_off;
	char dt_buf[DIRATTR_BUFSIZE];
	char *dt_extattrpathname;
	int dt_extattrfd;
	bool_t dt_extattrfdbadpr;
};

typedef struct dirattr_tran dirattr_tran_t;


/* a dirattr is identified internally by its index into the backing store.
 * this index is the offset of the dirattr (relative to the end of the dirattr
 * persistent state hdr) into the backing store divided by the size of a
 * dirattr. a special index is reserved to represent the null index. a type
 * is defined for dirattr index (dix_t). it is a 64 bit signed for direct use
 * in the lseek64 system call.
 */
typedef off64_t dix_t;
#define DIX2OFF( dix )	( ( off64_t )( dix * ( off64_t )sizeof( dirattr_t )   \
				       +				      \
				       ( off64_t )DIRATTR_PERS_SZ ))
#define OFF2DIX( doff )	( ( dix_t )( ( doff - ( off64_t )DIRATTR_PERS_SZ )    \
				     /					      \
				     ( off64_t )sizeof( dirattr_t )))


/* declarations of externally defined global symbols *************************/

extern size_t pgsz;

/* forward declarations of locally defined static functions ******************/

static void dirattr_get( dah_t );
static void dirattr_cacheflush( void );
#ifdef DIRATTRCHK
static uint16_t calcdixcum( dix_t dix );
#endif /* DIRATTRCHK */


/* definition of locally defined global variables ****************************/


/* definition of locally defined static variables *****************************/

static char *dirattrfile = "dirattr";
static char *dirextattrfile = "dirextattr";
static dirattr_tran_t *dtp = 0;
static dirattr_pers_t *dpp = 0;


/* definition of locally defined global functions ****************************/

bool_t
dirattr_init( char *hkdir, bool_t resume, uint64_t dircnt )
{
	if ( dtp ) {
		return BOOL_TRUE;
	}

	/* sanity checks
	 */
	assert( sizeof( dirattr_pers_t ) <= DIRATTR_PERS_SZ );
	assert( ! dtp );
	assert( ! dpp );

	/* allocate and initialize context
	 */
	dtp = ( dirattr_tran_t * )calloc( 1, sizeof( dirattr_tran_t ));
	assert( dtp );
	dtp->dt_cachedh = DAH_NULL;
	dtp->dt_fd = -1;
	dtp->dt_extattrfd = -1;

	/* generate a string containing the pathname of the dirattr file
	 */
	dtp->dt_pathname = open_pathalloc( hkdir, dirattrfile, 0 );

	/* open the dirattr file
	 */
	if ( resume ) {
		/* open existing file
		 */
		dtp->dt_fd = open( dtp->dt_pathname, O_RDWR );
		if ( dtp->dt_fd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "could not find directory attributes file %s: "
			      "%s\n"),
			      dtp->dt_pathname,
			      strerror( errno ));
			return BOOL_FALSE;
		}
	} else {
		/* create the dirattr file, first unlinking any older version
		 * laying around
		 */
		( void )unlink( dtp->dt_pathname );
		dtp->dt_fd = open( dtp->dt_pathname,
				   O_RDWR | O_CREAT | O_EXCL,
				   S_IRUSR | S_IWUSR );
		if ( dtp->dt_fd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "could not create directory attributes file %s: "
			      "%s\n"),
			      dtp->dt_pathname,
			      strerror( errno ));
			return BOOL_FALSE;
		}

		/* reserve space for the backing store. try to use RESVSP64.
		 * if doesn't work, try ALLOCSP64. the former is faster, as
		 * it does not zero the space.
		 */
		{
		bool_t successpr;
		unsigned int ioctlcmd;
		int loglevel;
		size_t trycnt;

		for ( trycnt = 0,
		      successpr = BOOL_FALSE,
		      ioctlcmd = XFS_IOC_RESVSP64,
		      loglevel = MLOG_VERBOSE
		      ;
		      ! successpr && trycnt < 2
		      ;
		      trycnt++,
		      ioctlcmd = XFS_IOC_ALLOCSP64,
		      loglevel = max( MLOG_NORMAL, loglevel - 1 )) {
			off64_t initsz;
			struct flock64 flock64;
			int rval;

			if ( ! ioctlcmd ) {
				continue;
			}

			initsz = ( off64_t )DIRATTR_PERS_SZ
				 +
				 ( ( off64_t )dircnt * sizeof( dirattr_t ));
			flock64.l_whence = 0;
			flock64.l_start = 0;
			flock64.l_len = initsz;
			rval = ioctl( dtp->dt_fd, ioctlcmd, &flock64 );
			if ( rval ) {
				if ( errno != ENOTTY ) {
					mlog( loglevel | MLOG_NOTE, _(
					      "attempt to reserve %lld bytes for %s "
					      "using %s "
					      "failed: %s (%d)\n"),
					      initsz,
					      dtp->dt_pathname,
					      ioctlcmd == XFS_IOC_RESVSP64
					      ?
					      "XFS_IOC_RESVSP64"
					      :
					      "XFS_IOC_ALLOCSP64",
					      strerror( errno ),
					      errno );
				}
			} else {
				successpr = BOOL_TRUE;
			}
		}
		}
	}

	/* mmap the persistent descriptor
	 */
	assert( ! ( DIRATTR_PERS_SZ % pgsz ));
	dpp = ( dirattr_pers_t * )mmap_autogrow( DIRATTR_PERS_SZ,
				        dtp->dt_fd,
				        ( off_t )0 );
	assert( dpp );
	if ( dpp == ( dirattr_pers_t * )-1 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "unable to map %s: %s\n"),
		      dtp->dt_pathname,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* initialize persistent state
	 */
	if ( ! resume ) {
		dpp->dp_appendoff = ( off64_t )DIRATTR_PERS_SZ;
	}

	/* initialize transient state
	 */
	dtp->dt_at_endpr = BOOL_FALSE;

	/* calculate the dir extattr pathname, and set the fd to -1.
	 * file will be created on demand.
	 */
	dtp->dt_extattrpathname = open_pathalloc( hkdir, dirextattrfile, 0 );
	dtp->dt_extattrfd = -1;
	dtp->dt_extattrfdbadpr = BOOL_FALSE;
	if ( resume ) {
		( void )unlink( dtp->dt_extattrpathname );
	}

	return BOOL_TRUE;
}

void
dirattr_cleanup( void )
{
	/* REFERENCED */
	int rval;

	if ( ! dtp ) {
		return;
	}
	if ( dpp ) {
		rval = munmap( ( void * )dpp, DIRATTR_PERS_SZ );
		assert( ! rval );
		dpp = 0;
	}
	if ( dtp->dt_fd >= 0 ) {
		( void )close( dtp->dt_fd );
		dtp->dt_fd = -1;
	}
	if ( dtp->dt_pathname ) {
		( void )unlink( dtp->dt_pathname );
		free( ( void * )dtp->dt_pathname );
	}
	if ( dtp->dt_extattrfd >= 0 ) {
		( void )close( dtp->dt_extattrfd );
		dtp->dt_extattrfd = -1;
	}
	if ( dtp->dt_extattrpathname ) {
		( void )unlink( dtp->dt_extattrpathname );
		free( ( void * )dtp->dt_extattrpathname );
	}

	free( ( void * )dtp );
	dtp = 0;
}

dah_t
dirattr_add( filehdr_t *fhdrp )
{
	dirattr_t dirattr;
	off64_t oldoff;
	dix_t dix;
#ifdef DIRATTRCHK
	uint16_t sum;
#endif /* DIRATTRCHK */
	dah_t dah;
	
	/* sanity checks
	 */
	assert( dtp );
	assert( dpp );

	/* make sure file pointer is positioned to write at end of file
	 */
	if ( ! dtp->dt_at_endpr ) {
		off64_t newoff;
		newoff = lseek64( dtp->dt_fd, dpp->dp_appendoff, SEEK_SET );
		if ( newoff == ( off64_t )-1 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "lseek of dirattr failed: %s\n"),
			      strerror( errno ));
			return DAH_NULL;
		}
		assert( dpp->dp_appendoff == newoff );
		dtp->dt_at_endpr = BOOL_TRUE;
	}

	if (dtp->dt_off + sizeof(dirattr_t) > sizeof(dtp->dt_buf)) {
		if (dirattr_flush() != RV_OK) {
			return DAH_NULL;
		}
	}

	/* calculate the index of this dirattr
	 */
	oldoff = dpp->dp_appendoff;
	dix = OFF2DIX( oldoff );
	assert( dix <= DIX_MAX );

	/* populate a dirattr
	 */
	dirattr.d_mode = ( mode_t )fhdrp->fh_stat.bs_mode;
	dirattr.d_uid = ( uid_t )fhdrp->fh_stat.bs_uid;
	dirattr.d_gid = ( gid_t )fhdrp->fh_stat.bs_gid;
	dirattr.d_atime = ( time32_t )fhdrp->fh_stat.bs_atime.tv_sec;
	dirattr.d_mtime = ( time32_t )fhdrp->fh_stat.bs_mtime.tv_sec;
	dirattr.d_ctime = ( time32_t )fhdrp->fh_stat.bs_ctime.tv_sec;
	dirattr.d_xflags = fhdrp->fh_stat.bs_xflags;
	dirattr.d_extsize = ( uint32_t )fhdrp->fh_stat.bs_extsize;
	dirattr.d_projid = bstat_projid(&(fhdrp->fh_stat));
	dirattr.d_dmevmask = fhdrp->fh_stat.bs_dmevmask;
	dirattr.d_dmstate = ( uint32_t )fhdrp->fh_stat.bs_dmstate;
#ifdef DIRATTRCHK
	dirattr.d_unq = DIRATTRUNQ;
	sum = calcdixcum( dix );
	dirattr.d_sum = sum;
#endif /* DIRATTRCHK */
	dirattr.d_extattroff = DIRATTR_EXTATTROFFNULL;

	/* write the entry into our buffer
	 */
	memcpy(dtp->dt_buf + dtp->dt_off, (void *)&dirattr, sizeof(dirattr_t));
	dtp->dt_off += sizeof(dirattr_t);

	/* update the next write offset
	 */
	assert( dpp->dp_appendoff <= OFF64MAX - ( off64_t )sizeof(dirattr_t) );
	dpp->dp_appendoff += ( off64_t )sizeof(dirattr_t);

#ifdef DIRATTRCHK
	dah = HDLMKHDL( sum, dix );
#else /* DIRATTRCHK */
	dah = ( dah_t )dix;
#endif /* DIRATTRCHK */

	return dah;
}

void
dirattr_addextattr( dah_t dah, extattrhdr_t *ahdrp )
{
	off64_t oldoff;
	off64_t off;
	off64_t seekoff;
	off64_t nulloff;
	int nread;
	int nwritten;

	/* pull the selected dir attributes into the cache
	 */
	dirattr_get( dah );

	/* open/create extended attributes file if not yet done
	 */
	if ( dtp->dt_extattrfd < 0 ) {
		if ( dtp->dt_extattrfdbadpr ) {
			return;
		}
		dtp->dt_extattrfd = open( dtp->dt_extattrpathname,
					  O_RDWR | O_CREAT,
					  S_IRUSR | S_IWUSR );
		if ( dtp->dt_extattrfd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not open/create directory "
			      "extended attributes file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return;
		}
	}

	/* seek to the end of the dir extattr list
	 */
	off = dtp->dt_cached_dirattr.d_extattroff;
	oldoff = DIRATTR_EXTATTROFFNULL;
	while ( off != DIRATTR_EXTATTROFFNULL ) {
		seekoff = lseek64( dtp->dt_extattrfd, off, SEEK_SET );
		if ( seekoff < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not seek to into extended attributes "
			      "file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return;
		}
		assert( seekoff == off );

		oldoff = off;

		nread = read( dtp->dt_extattrfd,
			      ( void * )&off,
			      sizeof( off ));
		if ( nread != ( int )sizeof( off )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not read extended attributes "
			      "file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return;
		}
	}

	/* append the extended attributes
	 */
	off = lseek64( dtp->dt_extattrfd, 0, SEEK_END );
	if ( off < 0 ) {
		mlog( MLOG_NORMAL | MLOG_WARNING, _(
		      "could not seek to end of extended attributes "
		      "file %s: "
		      "%s (%d)\n"),
		      dtp->dt_extattrpathname,
		      strerror( errno ),
		      errno );
		dtp->dt_extattrfdbadpr = BOOL_TRUE;
		return;
	}
	nulloff = DIRATTR_EXTATTROFFNULL;
	nwritten = write( dtp->dt_extattrfd,
			  ( void * )&nulloff,
			  sizeof( nulloff ));
	if ( nwritten != ( int )sizeof( nulloff )) {
		mlog( MLOG_NORMAL | MLOG_WARNING, _(
		      "could not write extended attributes "
		      "file %s: "
		      "%s (%d)\n"),
		      dtp->dt_extattrpathname,
		      strerror( errno ),
		      errno );
		dtp->dt_extattrfdbadpr = BOOL_TRUE;
		return;
	}
	nwritten = write( dtp->dt_extattrfd, ( void * )ahdrp, ahdrp->ah_sz );
	if ( nwritten != ( int )( ahdrp->ah_sz )) {
		mlog( MLOG_NORMAL | MLOG_WARNING, _(
		      "could not write at end of extended attributes "
		      "file %s: "
		      "%s (%d)\n"),
		      dtp->dt_extattrpathname,
		      strerror( errno ),
		      errno );
		dtp->dt_extattrfdbadpr = BOOL_TRUE;
		return;
	}

	/* fill in the offset of the extended attributes into the
	 * linked list
	 */
	if ( oldoff == DIRATTR_EXTATTROFFNULL ) {
		dtp->dt_cached_dirattr.d_extattroff = off;
		dirattr_cacheflush( );
	} else {
		seekoff = lseek64( dtp->dt_extattrfd, oldoff, SEEK_SET );
		if ( seekoff < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not seek to into extended attributes "
			      "file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return;
		}
		assert( seekoff == oldoff );
		nwritten = write( dtp->dt_extattrfd,
				  ( void * )&off,
				  sizeof( off ));
		if ( nwritten != ( int )sizeof( off )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not write extended attributes "
			      "file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return;
		}
	}
}

bool_t
dirattr_cb_extattr( dah_t dah,
		    bool_t ( * cbfunc )( extattrhdr_t *ahdrp,
				         void *ctxp ),
		    extattrhdr_t *ahdrp,
		    void *ctxp )
{
	off64_t off;

	/* pull the selected dir attributes into the cache
	 */
	dirattr_get( dah );

	/* open/create extended attributes file if not yet done
	 */
	if ( dtp->dt_extattrfd < 0 ) {
		if ( dtp->dt_extattrfdbadpr ) {
			return BOOL_TRUE;
		}
		dtp->dt_extattrfd = open( dtp->dt_extattrpathname,
					  O_RDWR | O_CREAT,
					  S_IRUSR | S_IWUSR );
		if ( dtp->dt_extattrfd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not open/create directory "
			      "extended attributes file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return BOOL_TRUE;
		}
	}

	/* walk through the dirattr list for this dah
	 */
	off = dtp->dt_cached_dirattr.d_extattroff;
	while ( off != DIRATTR_EXTATTROFFNULL ) {
		off64_t seekoff;
		int nread;
		off64_t nextoff;
		size_t recsz;
		bool_t ok;

		/* seek to the extattr
		 */
		seekoff = lseek64( dtp->dt_extattrfd, off, SEEK_SET );
		if ( seekoff < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not seek to into extended attributes "
			      "file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return BOOL_TRUE;
		}
		assert( seekoff == off );

		/* peel off the next offset
		 */
		nread = read( dtp->dt_extattrfd,
			      ( void * )&nextoff,
			      sizeof( nextoff ));
		if ( nread != ( int )sizeof( nextoff )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not read extended attributes "
			      "file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return BOOL_TRUE;
		}

		/* read the extattr hdr
		 */
		nread = read( dtp->dt_extattrfd,
			      ( void * )ahdrp,
			      EXTATTRHDR_SZ );
		if ( nread != EXTATTRHDR_SZ ) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not read extended attributes "
			      "file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return BOOL_TRUE;
		}

		/* read the remainder of the extattr
		 */
		recsz = ( size_t )ahdrp->ah_sz;
		assert( recsz >= EXTATTRHDR_SZ );
		nread = read( dtp->dt_extattrfd,
			      ( void * )&ahdrp[ 1 ],
			      recsz - EXTATTRHDR_SZ );
		if ( nread != ( int )( recsz - EXTATTRHDR_SZ )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "could not read extended attributes "
			      "file %s: "
			      "%s (%d)\n"),
			      dtp->dt_extattrpathname,
			      strerror( errno ),
			      errno );
			dtp->dt_extattrfdbadpr = BOOL_TRUE;
			return BOOL_TRUE;
		}

		/* call the callback func
		 */
		ok = ( * cbfunc )( ahdrp, ctxp );
		if ( ! ok ) {
			return BOOL_FALSE;
		}

		/* go th the next one
		 */
		off = nextoff;
	}

	return BOOL_TRUE;
}

void
dirattr_update( dah_t dah, filehdr_t *fhdrp )
{
	dix_t dix;
#ifdef DIRATTRCHK
	uint16_t sum;
#endif /* DIRATTRCHK */
	off64_t argoff;
	off64_t newoff;
	dirattr_t dirattr;
	int nwritten;

	/* sanity checks
	 */
	assert( dtp );
	assert( dpp );

	assert( dah != DAH_NULL );

#ifdef DIRATTRCHK
	sum = HDLGETSUM( dah );
	dix = HDLGETDIX( dah );
#else /* DIRATTRCHK */
	dix = ( dix_t )dah;
#endif /* DIRATTRCHK */

	assert( dix >= 0 );
	assert( dix <= DIX_MAX );

	argoff = DIX2OFF( dix );
	assert( argoff >= 0 );
	assert( argoff >= ( off64_t )DIRATTR_PERS_SZ );
	assert( argoff <= dpp->dp_appendoff - ( off64_t )sizeof( dirattr_t ));

#ifdef DIRATTRCHK
	dirattr_get( dah );
	assert( dtp->dt_cached_dirattr.d_unq == DIRATTRUNQ );
	assert( dtp->dt_cached_dirattr.d_sum == sum );
#endif /* DIRATTRCHK */

	if ( dtp->dt_at_endpr && dtp->dt_off ) {
		if (dirattr_flush() != RV_OK) {
			assert( 0 );
			return;
		}
	}

	/* seek to the dirattr
	 */
	newoff = lseek64( dtp->dt_fd, argoff, SEEK_SET );
	if ( newoff == ( off64_t )-1 ) {
		mlog( MLOG_NORMAL, _(
		      "lseek of dirattr failed: %s\n"),
		      strerror( errno ));
		assert( 0 );
	}
	assert( newoff == argoff );

	/* populate a dirattr
	 */
	dirattr.d_mode = ( mode_t )fhdrp->fh_stat.bs_mode;
	dirattr.d_uid = ( uid_t )fhdrp->fh_stat.bs_uid;
	dirattr.d_gid = ( gid_t )fhdrp->fh_stat.bs_gid;
	dirattr.d_atime = ( time32_t )fhdrp->fh_stat.bs_atime.tv_sec;
	dirattr.d_mtime = ( time32_t )fhdrp->fh_stat.bs_mtime.tv_sec;
	dirattr.d_ctime = ( time32_t )fhdrp->fh_stat.bs_ctime.tv_sec;
	dirattr.d_xflags = fhdrp->fh_stat.bs_xflags;
	dirattr.d_extsize = ( uint32_t )fhdrp->fh_stat.bs_extsize;
	dirattr.d_projid = bstat_projid(&(fhdrp->fh_stat));
	dirattr.d_dmevmask = fhdrp->fh_stat.bs_dmevmask;
	dirattr.d_dmstate = ( uint32_t )fhdrp->fh_stat.bs_dmstate;
	dirattr.d_extattroff = DIRATTR_EXTATTROFFNULL;

	/* write the dirattr
	 */
	nwritten = write( dtp->dt_fd, ( void * )&dirattr, sizeof( dirattr ));
	if ( ( size_t )nwritten != sizeof( dirattr )) {
		mlog( MLOG_NORMAL, _(
		      "update of dirattr failed: %s\n"),
		      strerror( errno ));
		assert( 0 );
	}

	dtp->dt_at_endpr = BOOL_FALSE;
	dtp->dt_cachedh = dah;
}

/* ARGSUSED */
void
dirattr_del( dah_t dah )
{
}

mode_t
dirattr_get_mode( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_mode;
}

uid_t
dirattr_get_uid( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_uid;
}

uid_t
dirattr_get_gid( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_gid;
}

time32_t
dirattr_get_atime( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_atime;
}

time32_t
dirattr_get_mtime( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_mtime;
}

time32_t
dirattr_get_ctime( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_ctime;
}

uint32_t
dirattr_get_xflags( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_xflags;
}

uint32_t
dirattr_get_extsize( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_extsize;
}

uint32_t
dirattr_get_projid( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_projid;
}

uint32_t
dirattr_get_dmevmask( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_dmevmask;
}

uint32_t
dirattr_get_dmstate( dah_t dah )
{
	dirattr_get( dah );
	return dtp->dt_cached_dirattr.d_dmstate;
}

rv_t
dirattr_flush()
{
	ssize_t nwritten;

	/* sanity checks
	*/
	assert ( dtp );

	if (dtp->dt_off) {
		/* write the accumulated dirattr entries
		*/
		nwritten = write( dtp->dt_fd, ( void * )dtp->dt_buf, dtp->dt_off);
		if ( nwritten != dtp->dt_off ) {
			if ( nwritten < 0 ) {
				mlog( MLOG_NORMAL | MLOG_ERROR,
					_("write of dirattr buffer failed: %s\n"),
					strerror( errno ));
			} else {
				mlog( MLOG_NORMAL | MLOG_ERROR,
					_("write of dirattr buffer failed: "
					"expected to write %ld, actually "
					"wrote %ld\n"), dtp->dt_off, nwritten);
			}
			assert( 0 );
			return RV_UNKNOWN;
		}
		dtp->dt_off = 0;
	}
	return RV_OK;
}

/* definition of locally defined static functions ****************************/

static void
dirattr_get( dah_t dah )
{
	dix_t dix;
	off64_t argoff;
	off64_t newoff;
	int nread;
#ifdef DIRATTRCHK
	uint16_t sum;
#endif /* DIRATTRCHK */

	/* sanity checks
	 */
	assert( dtp );
	assert( dpp );

	assert( dah != DAH_NULL );

	/* if we are already holding this dirattr in cache,
	 * just return
	 */
	if ( dtp->dt_cachedh == dah ) {
		return;
	}

#ifdef DIRATTRCHK
	sum = HDLGETSUM( dah );
	dix = HDLGETDIX( dah );
#else /* DIRATTRCHK */
	dix = ( dix_t )dah;
#endif /* DIRATTRCHK */
	assert( dix >= 0 );
	assert( dix <= DIX_MAX );

	argoff = DIX2OFF( dix );
	assert( argoff >= 0 );
	assert( argoff >= ( off64_t )DIRATTR_PERS_SZ );
	assert( argoff <= dpp->dp_appendoff - ( off64_t )sizeof( dirattr_t ));

	if ( dtp->dt_at_endpr && dtp->dt_off ) {
		if (dirattr_flush() != RV_OK) {
			assert( 0 );
			return;
		}
	}

	/* seek to the dirattr
	 */
	newoff = lseek64( dtp->dt_fd, argoff, SEEK_SET );
	if ( newoff == ( off64_t )-1 ) {
		mlog( MLOG_NORMAL, _(
		      "lseek of dirattr failed: %s\n"),
		      strerror( errno ));
		assert( 0 );
	}
	assert( newoff == argoff );

	/* read the dirattr
	 */
	nread = read( dtp->dt_fd,
		      ( void * )&dtp->dt_cached_dirattr,
		      sizeof( dtp->dt_cached_dirattr ));
	if ( ( size_t )nread != sizeof( dtp->dt_cached_dirattr )) {
		mlog( MLOG_NORMAL, _(
		      "read of dirattr failed: %s\n"),
		      strerror( errno ));
		assert( 0 );
	}

#ifdef DIRATTRCHK
	assert( dtp->dt_cached_dirattr.d_unq == DIRATTRUNQ );
	assert( dtp->dt_cached_dirattr.d_sum == sum );
#endif /* DIRATTRCHK */

	dtp->dt_at_endpr = BOOL_FALSE;
	dtp->dt_cachedh = dah;
}

static void
dirattr_cacheflush( void )
{
	dah_t dah;
	dix_t dix;
#ifdef DIRATTRCHK
	uint16_t sum;
#endif /* DIRATTRCHK */
	off64_t argoff;
	off64_t newoff;
	int nwritten;

	/* sanity checks
	 */
	assert( dtp );
	assert( dpp );

	/* if nothing in the cache, ignore
	 */
	dah = dtp->dt_cachedh;
	assert( dah != DAH_NULL );
	if ( dah == DAH_NULL ) {
		return;
	}

#ifdef DIRATTRCHK
	sum = HDLGETSUM( dah );
	dix = HDLGETDIX( dah );
#else /* DIRATTRCHK */
	dix = ( dix_t )dah;
#endif /* DIRATTRCHK */

#ifdef DIRATTRCHK
	assert( dtp->dt_cached_dirattr.d_unq == DIRATTRUNQ );
	assert( dtp->dt_cached_dirattr.d_sum == sum );
#endif /* DIRATTRCHK */

	assert( dix >= 0 );
	assert( dix <= DIX_MAX );

	argoff = DIX2OFF( dix );
	assert( argoff >= 0 );
	assert( argoff >= ( off64_t )DIRATTR_PERS_SZ );
	assert( argoff <= dpp->dp_appendoff - ( off64_t )sizeof( dirattr_t ));

	/* seek to the dirattr
	 */
	newoff = lseek64( dtp->dt_fd, argoff, SEEK_SET );
	if ( newoff == ( off64_t )-1 ) {
		mlog( MLOG_NORMAL, _(
		      "lseek of dirattr failed: %s\n"),
		      strerror( errno ));
		assert( 0 );
	}
	assert( newoff == argoff );

	/* write the dirattr
	 */
	nwritten = write( dtp->dt_fd,
			  ( void * )&dtp->dt_cached_dirattr,
			  sizeof( dtp->dt_cached_dirattr ));
	if ( ( size_t )nwritten != sizeof( dtp->dt_cached_dirattr )) {
		mlog( MLOG_NORMAL, _(
		      "flush of dirattr failed: %s\n"),
		      strerror( errno ));
		assert( 0 );
	}

	dtp->dt_at_endpr = BOOL_FALSE;
}

#ifdef DIRATTRCHK

static uint16_t
calcdixcum( dix_t dix )
{
	uint16_t sum;
	ix_t nibcnt;
	ix_t nibix;

	assert( ( sizeof( dah_t ) / HDLSUMCNT ) * HDLSUMCNT == sizeof( dah_t ));

	nibcnt = ( sizeof( dah_t ) / HDLSUMCNT ) - 1;
	sum = 0;
	for ( nibix = 0 ; nibix < nibcnt ; nibix++ ) {
		sum += ( uint16_t )( dix & HDLSUMLOMASK );
		dix >>= HDLSUMCNT;
	}
	sum = ( uint16_t )( ( ~sum + 1 ) & HDLSUMLOMASK );

	return sum;
}

#endif /* DIRATTRCHK */
