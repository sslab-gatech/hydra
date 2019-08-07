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
#include <sys/stat.h>
#include <sys/param.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <malloc.h>
#include <sched.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>
#include <xfs/xfs.h>
#include <xfs/jdm.h>	/* needed only for util.h include */

#include "config.h"

#include "types.h"
#include "util.h"	/* needed onyl for I/O routines */
#include "stream.h"
#include "mlog.h"
#include "global.h"
#include "drive.h"
#include "media.h"
#include "arch_xlate.h"

/* this rmt junk is here because the rmt protocol supports writing ordinary
 * (non-device) files in the remote /dev directory! yuck!
 */
#define open    rmtopen
#define creat   rmtcreat
#define close   rmtclose
#define ioctl   rmtioctl
#define read    rmtread
#define write   rmtwrite

extern int rmtclose( int );
extern int rmtcreat (char *path, int mode);
extern int rmtioctl( int, int, ... );
extern int rmtopen( char *, int, ... );
extern int rmtread( int, void*, uint);
extern int rmtwrite( int, const void *, uint);


/* drive_simple.c - drive strategy for standard in or a file
 */


/* structure definitions used locally ****************************************/

/* drive context - drive-specific context
 * buf must be page-aligned and at least 1 page in size
 */
#define PGPERBUF	64	/* private read buffer */
#define BUFSZ		( PGPERBUF * PGSZ )

/* operational mode
 */
typedef enum { OM_NONE, OM_READ, OM_WRITE } om_t;

struct drive_context {
	char dc_buf[ BUFSZ ];	/* input/output buffer */
	om_t dc_mode;		/* current mode of operation */
	ix_t dc_fmarkcnt;	/* how many file marks to the left */
	char *dc_ownedp;	/* first byte owned by caller */
	size_t dc_ownedsz;	/* how much owned by caller (write only) */
	char *dc_nextp;		/* next byte avail. to read/write */
	char *dc_emptyp;	/* first empty slot in buffer */
	off64_t dc_bufstroff;	/* offset in stream of top of buf */
	int dc_fd;		/* input/output file descriptor */
	drive_mark_t dc_firstmark;/* first mark's offset within mfile (dump) */
	ix_t dc_markcnt;	/* count of marks set (dump) */
	bool_t dc_rampr;	/* can randomly access file (not a pipe) */
	bool_t dc_isrmtpr;	/* is accessed via rmt */
	bool_t dc_israwdevpr;	/* is a raw disk partition */
};

typedef struct drive_context drive_context_t;


/* declarations of externally defined global variables ***********************/

extern size_t pgsz;


/* forward declarations of locally defined static functions ******************/

/* strategy functions
 */
static int ds_match( int, char *[], drive_t * );
static int ds_instantiate( int, char *[], drive_t * );

/* declare manager operators
 */
static bool_t do_init( drive_t * );
static bool_t do_sync( drive_t * );
static int do_begin_read( drive_t * );
static char *do_read( drive_t *, size_t , size_t *, int * );
static void do_return_read_buf( drive_t *, char *, size_t );
static void do_get_mark( drive_t *, drive_mark_t * );
static int do_seek_mark( drive_t *, drive_mark_t * );
static int do_next_mark( drive_t * );
static void do_get_mark( drive_t *, drive_mark_t * );
static void do_end_read( drive_t * );
static int do_begin_write( drive_t * );
static void do_set_mark( drive_t *, drive_mcbfp_t, void *, drive_markrec_t * );
static char * do_get_write_buf( drive_t *, size_t , size_t * );
static int do_write( drive_t *, char *, size_t );
static size_t do_get_align_cnt( drive_t * );
static int do_end_write( drive_t *, off64_t * );
static int do_rewind( drive_t * );
static int do_erase( drive_t * );
static int do_get_device_class( drive_t * );
static void do_quit( drive_t * );


/* definition of locally defined global variables ****************************/

/* simple drive strategy for file or stdin. referenced by drive.c
 */
drive_strategy_t drive_strategy_simple = {
	DRIVE_STRATEGY_SIMPLE,		/* ds_id */
	"file dump (drive_simple)",	/* ds_description */
	ds_match,			/* ds_match */
	ds_instantiate,			/* ds_instantiate */
	0x1000000ll,			/* ds_recmarksep */
	OFF64MAX			/* ds_recmfilesz */
};


/* definition of locally defined static variables *****************************/

/* drive operators
 */
static drive_ops_t drive_ops = {
	do_init,		/* do_init */
	do_sync,		/* do_sync */
	do_begin_read,		/* do_begin_read */
	do_read,		/* do_read */
	do_return_read_buf,	/* do_return_read_buf */
	do_get_mark,		/* do_get_mark */
	do_seek_mark,		/* do_seek_mark */
	do_next_mark,		/* do_next_mark */
	do_end_read,		/* do_end_read */
	do_begin_write,		/* do_begin_write */
	do_set_mark,		/* do_set_mark */
	do_get_write_buf,	/* do_get_write_buf */
	do_write,		/* do_write */
	do_get_align_cnt,	/* do_get_align_cnt */
	do_end_write,		/* do_end_write */
	0,			/* do_fsf */
	0,			/* do_bsf */
	do_rewind,		/* do_rewind */
	do_erase,		/* do_erase */
	0,			/* do_eject_media */
	do_get_device_class,	/* do_get_device_class */
	0,			/* do_display_metrics */
	do_quit,		/* do_quit */
};

/* definition of locally defined global functions ****************************/


/* definition of locally defined static functions ****************************/

/* strategy match - determines if this is the right strategy
 */
/* ARGSUSED */
static int
ds_match( int argc, char *argv[], drive_t *drivep )
{
	bool_t isrmtpr;
	struct stat64 statbuf;

	/* sanity checks
	 */
	assert( ! ( sizeofmember( drive_context_t, dc_buf ) % PGSZ ));

	/* determine if this is an rmt file. if so, give a weak match:
	 * might be an ordinary file accessed via the rmt protocol.
	 */
	if ( strchr( drivep->d_pathname, ':') ) {
		isrmtpr = BOOL_TRUE;
	} else {
		isrmtpr = BOOL_FALSE;
	}
	if ( isrmtpr ) {
		return 1;
	}

	/* willing to pick up anything not picked up by other strategies,
	 * as long as it exists and is not a directory
	 */
	if ( ! strcmp( drivep->d_pathname, "stdio" )) {
		return 1;
	}

	if ( stat64( drivep->d_pathname, &statbuf )) {
		return -1;
	}

	if ( S_ISDIR( statbuf.st_mode )) {
		return -1;
	}

	return 1;
}

/* strategy instantiate - initializes the pre-allocated drive descriptor
 */
/*ARGSUSED*/
static bool_t
ds_instantiate( int argc, char *argv[], drive_t *drivep )
{
	drive_context_t *contextp;

	/* hook up the drive ops
	 */
	drivep->d_opsp = &drive_ops;

	/* initialize the drive context - allocate a page-aligned
	 * structure, so the buffer is page-aligned.
	 */
	contextp = ( drive_context_t * )memalign( PGSZ,
						  sizeof( drive_context_t ));
	assert( contextp );
	assert( ( void * )contextp->dc_buf == ( void * )contextp );
	memset( ( void * )contextp, 0, sizeof( *contextp ));

	/* scan drive device pathname to see if remote tape
	 */
	if ( strchr( drivep->d_pathname, ':') ) {
		contextp->dc_isrmtpr = BOOL_TRUE;
	} else {
		contextp->dc_isrmtpr = BOOL_FALSE;
	}

	/* determine drive capabilities of and open the named file.
	 */
	drivep->d_capabilities = 0;
	drivep->d_capabilities |= DRIVE_CAP_AUTOREWIND;
	if ( ! strcmp( drivep->d_pathname, "stdio" )) {
#ifdef DUMP
		contextp->dc_fd = 1;
#endif /* DUMP */
#ifdef RESTORE
		drivep->d_capabilities |= DRIVE_CAP_READ;
		contextp->dc_fd = 0;
#endif /* RESTORE */
	} else if ( contextp->dc_isrmtpr ) {
		int oflags;
#ifdef DUMP
		oflags = O_WRONLY | O_CREAT | O_TRUNC;
#endif /* DUMP */
#ifdef RESTORE
		oflags = O_RDONLY;
		drivep->d_capabilities |= DRIVE_CAP_READ;
#endif /* RESTORE */
		contextp->dc_rampr = BOOL_FALSE;
		contextp->dc_fd = open( drivep->d_pathname,
					oflags,
				        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
		if ( contextp->dc_fd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
			      _("unable to open %s: %s\n"),
			      drivep->d_pathname,
			      strerror( errno ));
			return BOOL_FALSE;
		}
	} else {
		int oflags = 0;
		struct stat statbuf;
		int rval;
		rval = stat( drivep->d_pathname, &statbuf );
#ifdef DUMP
		if ( rval ) {
			if ( errno != ENOENT ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
				      _("stat of %s failed: %s\n"),
				      drivep->d_pathname,
				      strerror( errno ));
				return BOOL_FALSE;
			}
			drivep->d_capabilities |= DRIVE_CAP_REWIND;
			drivep->d_capabilities |= DRIVE_CAP_READ;
			drivep->d_capabilities |= DRIVE_CAP_ERASE;
			contextp->dc_rampr = BOOL_TRUE;
			oflags = O_RDWR | O_CREAT;

		} else {
			switch( statbuf.st_mode & S_IFMT ) {
			case S_IFREG:
				drivep->d_capabilities |= DRIVE_CAP_ERASE;
				drivep->d_capabilities |= DRIVE_CAP_REWIND;
				drivep->d_capabilities |= DRIVE_CAP_READ;
				contextp->dc_rampr = BOOL_TRUE;
				oflags = O_RDWR;
				break;
			case S_IFCHR:
				contextp->dc_israwdevpr = BOOL_TRUE;
				/* intentional fall-through */
			case S_IFBLK:
				/* intentional fall-through */
			case S_IFIFO:
				oflags = O_WRONLY;
				break;
			default:
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
				      _("cannot dump to %s "
				      "file type %x\n"),
				      drivep->d_pathname,
				      statbuf.st_mode & S_IFMT );
				return BOOL_FALSE;
			}
		}
#endif /* DUMP */
#ifdef RESTORE
		if ( rval ) {
			mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
			      _("stat of %s failed: %s\n"),
			      drivep->d_pathname,
			      strerror( errno ));
			return BOOL_FALSE;
		
		}
		oflags = O_RDONLY;
		switch( statbuf.st_mode & S_IFMT ) {
		case S_IFREG:
		case S_IFCHR:
		case S_IFBLK:
			drivep->d_capabilities |= DRIVE_CAP_REWIND;
			drivep->d_capabilities |= DRIVE_CAP_READ;
			break;
		case S_IFIFO:
			drivep->d_capabilities |= DRIVE_CAP_READ;
			break;
		default:
			mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
			      _("cannot restore from %s file type %x\n"),
			      drivep->d_pathname,
			      statbuf.st_mode & S_IFMT );
			return BOOL_FALSE;
		}
#endif /* RESTORE */
		contextp->dc_fd = open( drivep->d_pathname,
					oflags,
				        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
		if ( contextp->dc_fd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
			      _("unable to open %s: %s\n"),
			      drivep->d_pathname,
			      strerror( errno ));
			return BOOL_FALSE;
		}
	}

	/* initialize the operational mode. fmarkcnt is bumped on each
	 * end_read and end_write, set back to 0 on rewind.
	 */
	contextp->dc_mode = OM_NONE;
	contextp->dc_fmarkcnt = 0;

	drivep->d_contextp = ( void * )contextp;

	drivep->d_cap_est = -1;
	drivep->d_rate_est = -1;

	return BOOL_TRUE;
}

/* drive op init - second pass drive manager init - nothing to do,
 * since async I/O not used.
 */
/* ARGSUSED */
static bool_t
do_init( drive_t *drivep )
{
#ifdef DUMP
	drive_hdr_t *dwhdrp = drivep->d_writehdrp;
	media_hdr_t *mwhdrp = ( media_hdr_t * )dwhdrp->dh_upper;
#endif /* DUMP */

#ifdef DUMP
	/* fill in media strategy id: artifact of first version of xfsdump
	 */
	mwhdrp->mh_strategyid = MEDIA_STRATEGY_SIMPLE;
#endif /* DUMP */

	return BOOL_TRUE;
}

/* drive op init - third pass drive manager init - nothing to do,
 * since async I/O not used.
 */
/* ARGSUSED */
static bool_t
do_sync( drive_t *drivep )
{
	return BOOL_TRUE;
}

/* drive op begin_read - prepare file for reading - main job is to
 * read the media hdr
 */
static int
do_begin_read( drive_t *drivep )
{
#ifdef DEBUG
	int dcaps = drivep->d_capabilities;
#endif
	global_hdr_t *grhdrp = drivep->d_greadhdrp;
	drive_hdr_t *drhdrp = drivep->d_readhdrp;
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	int nread;
	int rval;
	global_hdr_t		*tmphdr = (global_hdr_t	*)malloc(GLOBAL_HDR_SZ);
	drive_hdr_t		*tmpdh = (drive_hdr_t *)tmphdr->gh_upper;
	media_hdr_t		*tmpmh = (media_hdr_t *)tmpdh->dh_upper;
	content_hdr_t		*tmpch = (content_hdr_t *)tmpmh->mh_upper;
	content_inode_hdr_t	*tmpcih = (content_inode_hdr_t *)tmpch->ch_specific;
	drive_hdr_t		*dh = (drive_hdr_t *)grhdrp->gh_upper;
	media_hdr_t		*mh = (media_hdr_t *)dh->dh_upper;
	content_hdr_t		*ch = (content_hdr_t *)mh->mh_upper;
	content_inode_hdr_t	*cih = (content_inode_hdr_t *)ch->ch_specific;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple begin_read( )\n" );

	/* verify protocol being followed
	 */
	assert( dcaps & DRIVE_CAP_READ );
	assert( contextp->dc_fd >= 0 );
	assert( contextp->dc_mode == OM_NONE );

	/* can only read one media file
	 */
	if ( contextp->dc_fmarkcnt > 0 ) {
		return DRIVE_ERROR_EOM;
	}

	/* prepare the drive context
	 */
	contextp->dc_ownedp = 0;
	contextp->dc_emptyp = &contextp->dc_buf[ 0 ];
	contextp->dc_nextp = contextp->dc_emptyp;
	contextp->dc_bufstroff = 0;

	/* read the global header using the read_buf() utility function and
	 * my own read and return_read_buf operators. spoof the mode
	 */
	contextp->dc_mode = OM_READ;

	nread = read_buf( ( char * )tmphdr,
			  GLOBAL_HDR_SZ,
			  ( void * )drivep,
			  ( rfp_t )drivep->d_opsp->do_read,
			  ( rrbfp_t )drivep->d_opsp->do_return_read_buf,
			  &rval );
	contextp->dc_mode = OM_NONE;

	/* if EOD and nread is zero, there is no data after the file mark
	 */
	if ( rval == DRIVE_ERROR_EOD ) {
		if ( nread == 0 ) {
			free(tmphdr);
			return DRIVE_ERROR_BLANK;
		} else {
			free(tmphdr);
			return DRIVE_ERROR_FORMAT;
		}
	}
	if  ( rval ) {
		free(tmphdr);
		return rval;
	}
	assert( ( size_t )nread == GLOBAL_HDR_SZ );
	
	mlog(MLOG_NITTY, "do_begin_read: global_hdr\n"
	     "\tgh_magic %.100s\n"
	     "\tgh_version %u\n"
	     "\tgh_checksum %u\n"
	     "\tgh_timestamp %u\n"
	     "\tgh_ipaddr %llu\n"
	     "\tgh_hostname %.100s\n"
	     "\tgh_dumplabel %.100s\n",
	     tmphdr->gh_magic,
	     tmphdr->gh_version,
	     tmphdr->gh_checksum,
	     tmphdr->gh_timestamp,
	     tmphdr->gh_ipaddr,
	     tmphdr->gh_hostname,
	     tmphdr->gh_dumplabel);

	/* check the checksum
	 */
	if ( ! global_hdr_checksum_check( tmphdr )) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
		      _("media file header checksum error\n") );
		free(tmphdr);
		return DRIVE_ERROR_CORRUPTION;
	}

	xlate_global_hdr(tmphdr, grhdrp, 1);
	xlate_drive_hdr(tmpdh, dh, 1);
	*(( drive_mark_t * )dh->dh_specific) =
		INT_GET(*(( drive_mark_t * )tmpdh->dh_specific), ARCH_CONVERT);
	xlate_media_hdr(tmpmh, mh, 1);
	xlate_content_hdr(tmpch, ch, 1);
	xlate_content_inode_hdr(tmpcih, cih, 1);
	free(tmphdr);

	/* check the magic number
	 */
	if ( strncmp( grhdrp->gh_magic, GLOBAL_HDR_MAGIC, GLOBAL_HDR_MAGIC_SZ)) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
		      _("media file header magic number mismatch: %s, %s\n"),
		      grhdrp->gh_magic,
		      GLOBAL_HDR_MAGIC);
		return DRIVE_ERROR_FORMAT;
	}

	/* check the version
	 */
	if ( global_version_check( grhdrp->gh_version ) != BOOL_TRUE ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
		      _("unrecognized media file header version (%d)\n"),
		      grhdrp->gh_version );
		return DRIVE_ERROR_VERSION;
	}

	/* check the strategy id
	 */
	if ( drhdrp->dh_strategyid != drive_strategy_simple.ds_id ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
		      _("unrecognized drive strategy ID "
		      "(media says %d, expected %d)\n"),
		      drhdrp->dh_strategyid, drive_strategy_simple.ds_id );
		return DRIVE_ERROR_FORMAT;
	}

	/* record the offset of the first mark
	 */
	contextp->dc_firstmark = *( drive_mark_t * )drhdrp->dh_specific;

	/* adjust the drive capabilities based on presence of first mark.
	 * this is a hack workaround for a bug in xfsdump which causes the
	 * first mark offset to not always be placed in the hdr.
	 */
	if ( contextp->dc_firstmark ) {
		drivep->d_capabilities |= DRIVE_CAP_NEXTMARK;
	}

	/* note that a successful begin_read ocurred
	 */
	contextp->dc_mode = OM_READ;
	return 0;
}

/* read - supply the caller with some filled buffer
 */
static char *
do_read( drive_t *drivep,
         size_t wantedcnt,
         size_t *actualcntp,
         int *rvalp )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	size_t remainingcnt;
	size_t actualcnt;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple read( want %u )\n",
	      wantedcnt );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_READ );
	assert( ! contextp->dc_ownedp );
	assert( wantedcnt > 0 );

	/* pre-initialize reference return
	 */
	*rvalp = 0;

	/* count number of unread bytes in buffer
	 */
	assert( contextp->dc_emptyp >= contextp->dc_nextp );
	remainingcnt = ( size_t )( contextp->dc_emptyp - contextp->dc_nextp );

	/* if no unread bytes in buffer, refill
	 */
	if ( remainingcnt == 0 ) {
		size_t bufhowfullcnt;
		int nread;

		/* calculate how many bytes were in the buffer. this will
		 * be used to adjust the recorded offset (relative to the
		 * beginning of the media file) of the top of the buffer
		 * after we refill the buffer.
		 */
		bufhowfullcnt = ( size_t )
				( contextp->dc_emptyp - contextp->dc_buf );

		/* attempt to fill the buffer. nread may be less if at EOF
		 */
		nread = read( contextp->dc_fd, contextp->dc_buf, BUFSZ );
		if ( nread < 0 ) {
			*rvalp = DRIVE_ERROR_DEVICE;
			return 0;
		}

		/* adjust the recorded offset of the top of the buffer
		 * relative to the beginning of the media file
		 */
		contextp->dc_bufstroff += ( off64_t )bufhowfullcnt;

		/* record the ptrs to the first empty byte and the next
		 * byte to be read
		 */
		assert( nread <= BUFSZ );
		contextp->dc_emptyp = contextp->dc_buf + nread;
		contextp->dc_nextp = contextp->dc_buf;

		/* if no bytes were read, the caller has seen all bytes.
		 */
		if ( nread == 0 ) {
			*rvalp = DRIVE_ERROR_EOD;
			return 0;
		}

		/* adjust the remaining count
		 */
		remainingcnt = ( size_t )nread;
	}

	/* the caller specified at most how many bytes he wants. if less
	 * than that remain unread in buffer, just return that many.
	 */
	actualcnt = min( wantedcnt, remainingcnt );

	/* set the owned ptr to the first byte to be supplied
	 */
	contextp->dc_ownedp = contextp->dc_nextp;

	/* advance the next ptr to the next byte to be supplied
	 */
	contextp->dc_nextp += actualcnt;
	assert( contextp->dc_nextp <= contextp->dc_emptyp );

	/* return the actual number of bytes supplied, and a ptr to the first
	 */
	*actualcntp = actualcnt;
	return contextp->dc_ownedp;
}

/* return_read_buf - lets the caller give back all of the
 * buffer obtained from a call to do_read().
 */
/* ARGSUSED */
static void
do_return_read_buf( drive_t *drivep, char *retp, size_t retcnt )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	/* REFERENCED */
	size_t ownedcnt;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple return_read_buf( returning %u )\n",
	      retcnt );

	/* verify protocol
	 */
	assert( contextp->dc_mode == OM_READ );
	assert( contextp->dc_ownedp );

	/* verify returning right buffer
	 */
	assert( retp == contextp->dc_ownedp );

	/* verify all of buffer provided is being returned
	 */
	ownedcnt = ( size_t )( contextp->dc_nextp - contextp->dc_ownedp );
	assert( retcnt == ownedcnt );

	/* indicate nothing now owned by caller
	 */
	contextp->dc_ownedp = 0;
}

/* the mark is simply the offset into the media file of the
 * next byte to be read
 */
static void
do_get_mark( drive_t *drivep, drive_mark_t *markp )
{
        drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	off64_t nextoff;
	off64_t strmoff;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple get_mark( )\n" );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_READ );
	assert( ! contextp->dc_ownedp );

	/* calculate the offset of the next byte to be supplied relative to
	 * the beginning of the buffer and relative to the beginning of
	 * ther media file.
	 */
	nextoff = ( off64_t )( contextp->dc_nextp - contextp->dc_buf );
	strmoff = contextp->dc_bufstroff + nextoff;
	*markp = ( drive_mark_t )strmoff;
}

/* seek forward to the specified mark. the caller must not have already read
 * past that point.
 */
static int
do_seek_mark( drive_t *drivep, drive_mark_t *markp )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	off64_t mark = *( off64_t * )markp;
	off64_t nextoff;
	off64_t strmoff;
	/* REFERENCED */
	int nread;
	off64_t nreadneeded64;
	int nreadneeded;
	int rval;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple seek_mark( )\n" );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_READ );
	assert( ! contextp->dc_ownedp );

	/* calculate the current offset within the media file
	 * of the next byte to be read
	 */
	nextoff = ( off64_t )( contextp->dc_nextp - contextp->dc_buf );
	strmoff = contextp->dc_bufstroff + nextoff;

	/* if the caller attempts to seek past the current offset,
	 * this is really bad
	 */
	if ( strmoff > mark ) {
		return DRIVE_ERROR_CORE;
	}
	
	/* use read_buf util func to eat up difference
	 */
	nreadneeded64 = mark - strmoff;
	while ( nreadneeded64 > 0 ) {
		if ( nreadneeded64 > INTGENMAX )
			nreadneeded = INTGENMAX;
		else
			nreadneeded = ( int )nreadneeded64;
		nread = read_buf( 0, nreadneeded, drivep,
				  ( rfp_t )drivep->d_opsp->do_read,
				( rrbfp_t )drivep->d_opsp->do_return_read_buf,
				  &rval );
		if  ( rval ) {
			return rval;
		}
		nreadneeded64 -= nread;
	}

	/* verify we are on the mark
	 */
	nextoff = ( off64_t )( contextp->dc_nextp - contextp->dc_buf );
	strmoff = contextp->dc_bufstroff + nextoff;
	assert( strmoff == mark );

	return 0;
}

/* seek forward to the next mark. we only know of one mark, the first
 * mark in the media file (recorded in the header). if the caller
 * has already read past that mark, blow up.
 */
static int
do_next_mark( drive_t *drivep )
{
#ifdef DEBUG
	int dcaps = drivep->d_capabilities;
#endif
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	drive_mark_t mark = contextp->dc_firstmark;
	int rval;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple next_mark( )\n" );

	/* assert protocol
	 */
	assert( dcaps & DRIVE_CAP_NEXTMARK );
	assert( contextp->dc_mode == OM_READ );
	assert( ! contextp->dc_ownedp );

	if ( ! mark ) {
		return DRIVE_ERROR_EOF;
	}

	rval = do_seek_mark( drivep, ( drive_mark_t * )&mark );
	if ( rval ) {
		return rval;
	}

	return 0;
}

/* end_read - tell the drive we are done reading the media file
 * just discards any buffered data
 */
void
do_end_read( drive_t *drivep )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple end_read( )\n" );

	/* be sure we are following protocol
	 */
	assert( contextp->dc_mode == OM_READ );
	contextp->dc_mode = OM_NONE;

	/* bump the file mark cnt
	 */
	contextp->dc_fmarkcnt++;
	assert( contextp->dc_fmarkcnt == 1 );
}

/* begin_write - prepare file for writing
 */
static int
do_begin_write( drive_t *drivep )
{
	int dcaps = drivep->d_capabilities;
	global_hdr_t *gwhdrp = drivep->d_gwritehdrp;
	drive_hdr_t *dwhdrp = drivep->d_writehdrp;
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	int rval;
	global_hdr_t		*tmphdr;
	drive_hdr_t		*tmpdh;
	media_hdr_t		*tmpmh;
	content_hdr_t		*tmpch;
	content_inode_hdr_t	*tmpcih;
	drive_hdr_t		*dh = (drive_hdr_t *)gwhdrp->gh_upper;
	media_hdr_t		*mh = (media_hdr_t *)dh->dh_upper;
	content_hdr_t		*ch = (content_hdr_t *)mh->mh_upper;
	content_inode_hdr_t	*cih = (content_inode_hdr_t *)ch->ch_specific;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple begin_write( )\n" );

	/* sanity checks
	 */
	assert( dwhdrp->dh_strategyid == DRIVE_STRATEGY_SIMPLE );

	/* assert protocol
	 */
	assert( contextp->dc_fd >= 0 );
	assert( contextp->dc_mode == OM_NONE );

	/* only one media file may be written
	 */
	if ( contextp->dc_fmarkcnt > 0 ) {
		return DRIVE_ERROR_EOM;
	}

	/* indicate in the header that there is no recorded mark.
	 */
	*( ( off64_t * )dwhdrp->dh_specific ) = 0;
	
	/* prepare the drive context. initially the caller does not own
	 * any of the write buffer, so the next portion of the buffer to
	 * be supplied is the top of the buffer. emptyp never changes;
	 * it always points to the byte after the end of the buffer. markcnt
	 * keeps track of the number marks the caller has set in the media file.
	 */
	contextp->dc_ownedp = 0;
	contextp->dc_nextp = contextp->dc_buf;
	contextp->dc_emptyp = contextp->dc_buf + sizeof( contextp->dc_buf );
	contextp->dc_bufstroff = 0;
	contextp->dc_markcnt = 0;

	/* truncate the destination if it supports read.
	 */
	if ( dcaps & DRIVE_CAP_READ ) {
		rval = ftruncate( contextp->dc_fd, 0 );
		if ( rval ) {
			mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_DRIVE,
			      _("attempt to truncate %s failed: %d (%s)\n"),
			      drivep->d_pathname,
			      errno,
			      strerror( errno ));
		}
	}

	/* set the mode
	 */
	contextp->dc_mode = OM_WRITE;

	tmphdr = (global_hdr_t *)malloc(GLOBAL_HDR_SZ);
	assert(tmphdr);
	memset(tmphdr, 0, GLOBAL_HDR_SZ);
	tmpdh = (drive_hdr_t *)tmphdr->gh_upper;
	tmpmh = (media_hdr_t *)tmpdh->dh_upper;
	tmpch = (content_hdr_t *)tmpmh->mh_upper;
	tmpcih = (content_inode_hdr_t *)tmpch->ch_specific;

	xlate_global_hdr(gwhdrp, tmphdr, 1);
	xlate_drive_hdr(dh, tmpdh, 1);
	INT_SET(*(( drive_mark_t * )tmpdh->dh_specific),
		ARCH_CONVERT,
		*(( drive_mark_t * )dh->dh_specific));
	xlate_media_hdr(mh, tmpmh, 1);
	xlate_content_hdr(ch, tmpch, 1);
	xlate_content_inode_hdr(cih, tmpcih, 1);

	/* checksum the header
	 */
	global_hdr_checksum_set( tmphdr );

	mlog(MLOG_NITTY, "do_begin_write: global_hdr\n"
	     "\tgh_magic %.100s\n"
	     "\tgh_version %u\n"
	     "\tgh_checksum %u\n"
	     "\tgh_timestamp %u\n"
	     "\tgh_ipaddr %llu\n"
	     "\tgh_hostname %.100s\n"
	     "\tgh_dumplabel %.100s\n",
	     tmphdr->gh_magic,
	     tmphdr->gh_version,
	     tmphdr->gh_checksum,
	     tmphdr->gh_timestamp,
	     tmphdr->gh_ipaddr,
	     tmphdr->gh_hostname,
	     tmphdr->gh_dumplabel);

	if ( ! global_hdr_checksum_check( tmphdr )) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_DRIVE,
		      _("media file header checksum error\n") );
	}
	else {
		mlog( MLOG_NITTY, "media file header checksum OK!\n" );
	}

	/* write the header using the write_buf() utility function and
	 * my own get_write_buf and write operators.
	 */
	rval = write_buf( ( char * )tmphdr,
			  GLOBAL_HDR_SZ,
			  ( void * )drivep,
			  ( gwbfp_t )drivep->d_opsp->do_get_write_buf,
			  ( wfp_t )drivep->d_opsp->do_write );

	free(tmphdr);

	/* if error while writing hdr, undo mode
	 */
	if ( rval ) {
		contextp->dc_mode = OM_NONE;
	}

	return rval;
}

/* do_set_mark - record a markrecord and callback
 */
static void
do_set_mark( drive_t *drivep,
	     drive_mcbfp_t cbfuncp,
	     void *cbcontextp,
	     drive_markrec_t *markrecp )
{
	drive_context_t		*contextp = ( drive_context_t * )drivep->d_contextp;
	drive_mark_t		mark;
	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple set_mark( )\n" );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_WRITE );
	assert( ! contextp->dc_ownedp );
	assert( contextp->dc_nextp );

	/* calculate the mark offset
	 */
	mark = ( drive_mark_t )( contextp->dc_bufstroff
				 +
				 ( off64_t )
				 ( contextp->dc_nextp - contextp->dc_buf ));

	/* fill in the mark field of the mark record
	 */
	markrecp->dm_log = mark;

	/* bump the mark count. if this is the first mark, record it
	 * in the drive strategy-specific header. this allows multiple
	 * media object restores to work. NOTE that the mark will not
	 * be recorded if the destination does not support random access
	 * and the write buffer has been flushed at least once.
	 * this is hidden by save_first_mark, and detected during restore
	 * by noting the first mark offset is NULL. to do this, must seek
	 * back to the header, rewrite and rechecksum the header,
	 * and seek back to the current position. HOWEVER, if the write
	 * buffer has not yet been flushed, we can just edit the buffer.
	 */
	contextp->dc_markcnt++;
	if ( contextp->dc_markcnt == 1 ) {
		if ( contextp->dc_bufstroff == 0 ) {
			/* cast the write buffer into a media file hdr
			 */
			global_hdr_t		*gwhdrp  =
				( global_hdr_t * )contextp->dc_buf;
			drive_hdr_t		*dwhdrp = ( drive_hdr_t * )gwhdrp->gh_upper;

			mlog( MLOG_NITTY | MLOG_DRIVE,
			     "re-writing media file header with first mark "
			     "(in buffer)\n" );

			/* record mark in hdr
			 */
			INT_SET(*( ( drive_mark_t * )dwhdrp->dh_specific ), ARCH_CONVERT, mark);

			/* adjust header checksum
			 */
			global_hdr_checksum_set( gwhdrp );

		} else if ( contextp->dc_rampr ) {
			global_hdr_t		*gwhdrp = drivep->d_gwritehdrp;
			drive_hdr_t		*dwhdrp = drivep->d_writehdrp;
			off64_t			newoff;
			/* REFERENCED */
			int		nwritten;

			/* assert the header has been flushed
			 */
			assert( contextp->dc_bufstroff >= sizeof( *gwhdrp ));

			/* record mark in hdr
			 */
			INT_SET(*( ( drive_mark_t * )dwhdrp->dh_specific ), ARCH_CONVERT, mark);

			/* adjust header checksum
			 */
			global_hdr_checksum_set( gwhdrp );

			/* seek to beginning
			 */
			newoff = lseek64( contextp->dc_fd, ( off64_t )0, SEEK_SET );
			if ( newoff < 0 ) {
				mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_DRIVE,
				      _("could not save first mark: %d (%s)\n"),
				      errno,
				      strerror( errno ));
			} else {
				global_hdr_t		*tmphdr;
				drive_hdr_t		*tmpdh;
				media_hdr_t		*tmpmh;
				content_hdr_t		*tmpch;
				content_inode_hdr_t	*tmpcih;
				drive_hdr_t		*dh = (drive_hdr_t *)gwhdrp->gh_upper;
				media_hdr_t		*mh = (media_hdr_t *)dh->dh_upper;
				content_hdr_t		*ch = (content_hdr_t *)mh->mh_upper;
				content_inode_hdr_t	*cih = (content_inode_hdr_t *)ch->ch_specific;

				assert( newoff == 0 );

				/* write and seek back to current offset
				 */
				mlog( MLOG_NITTY | MLOG_DRIVE,
				     "re-writing media file header "
				     "with first mark "
				     "(on media)\n" );

				tmphdr = (global_hdr_t *)malloc(GLOBAL_HDR_SZ);
				assert(tmphdr);
				tmpdh = (drive_hdr_t *)tmphdr->gh_upper;
				tmpmh = (media_hdr_t *)tmpdh->dh_upper;
				tmpch = (content_hdr_t *)tmpmh->mh_upper;
				tmpcih = (content_inode_hdr_t *)tmpch->ch_specific;
				xlate_global_hdr(gwhdrp, tmphdr, 1);
				xlate_drive_hdr(dh, tmpdh, 1);
				INT_SET(*(( drive_mark_t * )tmpdh->dh_specific),
					ARCH_CONVERT,
					*(( drive_mark_t * )dh->dh_specific));
				xlate_media_hdr(mh, tmpmh, 1);
				xlate_content_hdr(ch, tmpch, 1);
				xlate_content_inode_hdr(cih, tmpcih, 1);

				/* adjust header checksum
				 */
				global_hdr_checksum_set( tmphdr );

				mlog(MLOG_NITTY, "do_set_mark: global_hdr\n"
				     "\tgh_magic %.100s\n"
				     "\tgh_version %u\n"
				     "\tgh_checksum %u\n"
				     "\tgh_timestamp %u\n"
				     "\tgh_ipaddr %llu\n"
				     "\tgh_hostname %.100s\n"
				     "\tgh_dumplabel %.100s\n",
				     tmphdr->gh_magic,
				     tmphdr->gh_version,
				     tmphdr->gh_checksum,
				     tmphdr->gh_timestamp,
				     tmphdr->gh_ipaddr,
				     tmphdr->gh_hostname,
				     tmphdr->gh_dumplabel);

				nwritten = write( contextp->dc_fd,
						  tmphdr,
						  sizeof( *tmphdr ));
				assert( ( size_t )nwritten == sizeof( *tmphdr ));
				free(tmphdr);

				newoff = lseek64( contextp->dc_fd,
						  contextp->dc_bufstroff,
						  SEEK_SET );
				assert( newoff == contextp->dc_bufstroff );
			}
		}
	}

	/* if all written are committed, send the mark back immediately.
	 * otherwise put the mark record on the tail of the queue.
	 */
	if ( contextp->dc_nextp == contextp->dc_buf ) {
		assert( drivep->d_markrecheadp == 0 );
		( * cbfuncp )( cbcontextp, markrecp, BOOL_TRUE );
		return;
	} else {
		markrecp->dm_cbfuncp = cbfuncp;
		markrecp->dm_cbcontextp = cbcontextp;
		markrecp->dm_nextp = 0;
		if ( drivep->d_markrecheadp == 0 ) {
			drivep->d_markrecheadp = markrecp;
			drivep->d_markrectailp = markrecp;
		} else {
			assert( drivep->d_markrectailp );
			drivep->d_markrectailp->dm_nextp = markrecp;
			drivep->d_markrectailp = markrecp;
		}
	}
}

/* get_write_buf - supply the caller with buffer space. the caller
 * will fill the space with data, then call write() to request that
 * some or all of the buffer be written and to return the buffer space.
 */
/*ARGSUSED*/
static char *
do_get_write_buf( drive_t *drivep, size_t wanted_bufsz, size_t *actual_bufszp )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	size_t remaining_bufsz;
	size_t actual_bufsz;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple get_write_buf( want %u )\n",
	      wanted_bufsz );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_WRITE );
	assert( ! contextp->dc_ownedp );
	assert( contextp->dc_nextp );
	assert( contextp->dc_nextp < contextp->dc_emptyp );
	assert( contextp->dc_ownedsz == 0 );

	/* calculate how much buffer remains
	 */
	remaining_bufsz =( size_t )( contextp->dc_emptyp - contextp->dc_nextp );

	/*  give the caller the lesser of what he wants and what is available
	 */
	actual_bufsz = min( wanted_bufsz, remaining_bufsz );

	/* caller will own that portion of buffer
	 */
	contextp->dc_ownedp = contextp->dc_nextp;
	contextp->dc_ownedsz = actual_bufsz;

	/* won't know nextp until write
	 */
	contextp->dc_nextp = 0;

	/* return the portion of the buffer to the caller
	 */
	*actual_bufszp = actual_bufsz;
	return contextp->dc_ownedp;
}

/* write - write and accept ownership of the portion of the buffer owned by the
 * caller.
 */
/*ARGSUSED*/
static int
do_write( drive_t *drivep, char *bufp, size_t writesz )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	off64_t ownedstroff = contextp->dc_bufstroff
			      +
			      ( off64_t )
			      ( contextp->dc_ownedp - contextp->dc_buf );

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple write( "
	      "offset %lld (0x%llx 0%llo) "
	      "size %u (0x%x 0%o) "
	      ")\n",
	      ownedstroff,
	      ownedstroff,
	      ownedstroff,
	      writesz,
	      writesz,
	      writesz,
	      0 );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_WRITE );
	assert( contextp->dc_ownedp );
	assert( bufp == contextp->dc_ownedp );
	assert( ! contextp->dc_nextp );
	assert( contextp->dc_ownedp < contextp->dc_emptyp );
	assert( writesz == contextp->dc_ownedsz );

	/* calculate next portion of buffer available for get_write_buf,
	 * and indicate no portion is owned.
	 */
	contextp->dc_nextp = contextp->dc_ownedp + writesz;
	assert( contextp->dc_nextp <= contextp->dc_emptyp );
	contextp->dc_ownedp = 0;
	contextp->dc_ownedsz = 0;

	if ( writesz == 0 ) {
		return 0; /* returning unused buffer */
	}

	/* if buffer is full, flush it
	 */
	if ( contextp->dc_nextp == contextp->dc_emptyp ) {
		int nwritten;

		mlog( MLOG_DEBUG | MLOG_DRIVE,
		      "flushing write buf addr 0x%x size 0x%x\n",
		      contextp->dc_buf,
		      sizeof( contextp->dc_buf ));

		contextp->dc_nextp = 0;
		nwritten = write( contextp->dc_fd,
				  contextp->dc_buf,
				  sizeof( contextp->dc_buf ));
		if ( nwritten < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_DRIVE,
			      _("write to %s failed: %d (%s)\n"),
			      drivep->d_pathname,
			      errno,
			      strerror( errno ));
			nwritten = 0;
		}
		contextp->dc_bufstroff += ( off64_t )nwritten;
		drive_mark_commit( drivep, contextp->dc_bufstroff );
		contextp->dc_nextp = contextp->dc_buf;
		if ( ( size_t )nwritten < sizeof( contextp->dc_buf )) {
			return DRIVE_ERROR_EOM;
		}
	}

	return 0;
}

/* get_align_cnt - returns the number of bytes which must be written to
 * cause the next call to get_write_buf() to be page-aligned.
 */
static size_t
do_get_align_cnt( drive_t *drivep )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	intptr_t next_alignment_off;
	char *next_alignment_point;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple get_align_cnt( )\n" );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_WRITE );
	assert( ! contextp->dc_ownedp );
	assert( contextp->dc_nextp );
	assert( contextp->dc_nextp < contextp->dc_emptyp );

	/* calculate the next alignment point at or beyond the current nextp.
	 * the following algorithm works because dc_buf is page-aligned and
	 * a multiple of PGSZ.
	 */
	next_alignment_off = ( intptr_t )contextp->dc_nextp;
	next_alignment_off +=  PGMASK;
	next_alignment_off &= ~PGMASK;
	next_alignment_point = ( char * )next_alignment_off;
	assert( next_alignment_point <= contextp->dc_emptyp );

	/* return the number of bytes to the next alignment point
	 */
	return ( size_t )( next_alignment_point - contextp->dc_nextp );
}

/* end_write - flush any buffered data, and return by reference how many
 * bytes were committed.
 */
static int
do_end_write( drive_t *drivep, off64_t *ncommittedp )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	size_t remaining_bufsz;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple end_write( )\n" );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_WRITE );
	assert( ! contextp->dc_ownedp );
	assert( contextp->dc_nextp );
	assert( contextp->dc_nextp < contextp->dc_emptyp );

	/* calculate length of un-written portion of buffer
	 */
	assert( contextp->dc_nextp >= contextp->dc_buf );
	remaining_bufsz = ( size_t )( contextp->dc_nextp - contextp->dc_buf );

	if ( remaining_bufsz ) {
		int nwritten;
		if ( contextp->dc_israwdevpr ) {
			remaining_bufsz = ( remaining_bufsz + ( BBSIZE - 1 ))
					  &
					  ~( BBSIZE - 1 );
		}

		mlog( MLOG_DEBUG | MLOG_DRIVE,
		      "flushing write buf addr 0x%x size 0x%x\n",
		      contextp->dc_buf,
		      remaining_bufsz );

		nwritten = write( contextp->dc_fd,
				  contextp->dc_buf,
				  remaining_bufsz );
		if ( nwritten < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_DRIVE,
			      _("write to %s failed: %d (%s)\n"),
			      drivep->d_pathname,
			      errno,
			      strerror( errno ));
			drive_mark_discard( drivep );
			*ncommittedp = contextp->dc_bufstroff;
			contextp->dc_mode = OM_NONE;
			return DRIVE_ERROR_DEVICE;
		}
		contextp->dc_bufstroff += ( off64_t )nwritten;
		drive_mark_commit( drivep, contextp->dc_bufstroff );
		if ( ( size_t )nwritten < remaining_bufsz ) {
			*ncommittedp = contextp->dc_bufstroff;
			contextp->dc_mode = OM_NONE;
			return DRIVE_ERROR_EOM;
		}
	}

	/* bump the file mark cnt
	 */
	contextp->dc_fmarkcnt++;
	assert( contextp->dc_fmarkcnt == 1 );

	*ncommittedp = contextp->dc_bufstroff;
	contextp->dc_mode = OM_NONE;
	return 0;
}

/* rewind - return the current file offset to the beginning
 */
int
do_rewind( drive_t *drivep )
{
#ifdef DEBUG
	int dcaps = drivep->d_capabilities;
#endif
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	off64_t newoff;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple rewind( )\n" );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_NONE );
	assert( dcaps & DRIVE_CAP_REWIND );
	assert( contextp->dc_fd >= 0 );

	/* seek to beginning of file
	 */
	newoff = lseek64( contextp->dc_fd, ( off64_t )0, SEEK_SET );
	if ( newoff ) {
		assert( newoff < 0 );
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_DRIVE,
		      _("could not rewind %s: %s\n"),
		      drivep->d_pathname,
		      strerror( errno ));
		return DRIVE_ERROR_DEVICE;
	}

	return 0;
}

/* erase - truncate to zero length
 */
int
do_erase( drive_t *drivep )
{
#ifdef DEBUG
	int dcaps = drivep->d_capabilities;
#endif
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;
	off64_t newoff;
	int rval;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple erase( )\n" );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_NONE );
	assert( dcaps & DRIVE_CAP_ERASE );
	assert( contextp->dc_fd >= 0 );

	/* seek to beginning of file
	 */
	newoff = lseek64( contextp->dc_fd, ( off64_t )0, SEEK_SET );
	if ( newoff ) {
		assert( newoff < 0 );
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_DRIVE,
		      _("could not rewind %s in prep for erase: %s\n"),
		      drivep->d_pathname,
		      strerror( errno ));
		return DRIVE_ERROR_DEVICE;
	}

	/* erase to beginning of file
	 */
	rval = ftruncate64( contextp->dc_fd, ( off64_t )0 );
	if ( rval ) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_DRIVE,
		      _("could not erase %s: %s (%d)\n"),
		      drivep->d_pathname,
		      strerror( errno ),
		      errno );
		return DRIVE_ERROR_DEVICE;
	}
	contextp->dc_fmarkcnt = 0;

	return 0;
}

/* get_media_class()
 */
/* ARGSUSED */
static int 
do_get_device_class( drive_t *drivep )
{
	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple get_device_class( )\n" );
	assert( drivep );
	return DEVICE_NONREMOVABLE;
}

static void
do_quit( drive_t *drivep )
{
	drive_context_t *contextp = ( drive_context_t * )drivep->d_contextp;

	mlog( MLOG_NITTY | MLOG_DRIVE,
	      "drive_simple quit( )\n" );

	/* assert protocol
	 */
	assert( contextp->dc_mode == OM_NONE );
	assert( contextp );

	/* close file
	 */
	if ( contextp->dc_fd > 1 ) {
		close( contextp->dc_fd );
	}
	contextp->dc_fd = -1;

	/* free context
	 */
	free( ( void * )contextp );
	drivep->d_contextp = 0;
}
