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
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include "config.h"

#include "types.h"
#include "lock.h"
#include "mlog.h"
#include "namreg.h"
#include "openutil.h"
#include "mmap.h"

/* structure definitions used locally ****************************************/

#define NAMREG_AVGLEN	10

/* persistent context for a namreg - placed in first page
 * of the namreg file by namreg_init if not a sync
 */
struct namreg_pers {
	off64_t np_appendoff;
};

typedef struct namreg_pers namreg_pers_t;

#define NAMREG_PERS_SZ	pgsz

/* transient context for a namreg - allocated by namreg_init()
 */

#define	NAMREG_BUFSIZE	32768

struct namreg_tran {
	char *nt_pathname;
	int nt_fd;
	char *nt_map;
	bool_t nt_at_endpr;
	size_t nt_off;
	char nt_buf[NAMREG_BUFSIZE];
};

typedef struct namreg_tran namreg_tran_t;


#ifdef NAMREGCHK

/* macros for manipulating namreg handles when handle consistency
 * checking is enabled.
 */
#define CHKBITCNT		2
#define	CHKBITSHIFT		( NBBY * sizeof( nrh_t ) - CHKBITCNT )
#define	CHKBITLOMASK		( ( 1ULL << CHKBITCNT ) - 1 )
#define	CHKBITMASK		( CHKBITLOMASK << CHKBITSHIFT )
#define CHKHDLCNT		CHKBITSHIFT
#define CHKHDLMASK		( ( 1ULL << CHKHDLCNT ) - 1 )
#define CHKGETBIT( h )		( ( (h) >> CHKBITSHIFT ) & CHKBITLOMASK )
#define CHKGETHDL( h )		( (h) & CHKHDLMASK )
#define CHKMKHDL( c, h )	( ( ( (c) << CHKBITSHIFT ) & CHKBITMASK )	\
				  |					\
				  ( (h) & CHKHDLMASK ))
#define HDLMAX			( ( off64_t )CHKHDLMASK )

#else /* NAMREGCHK */

#define HDLMAX			( NRH_NULL - 1 )

#endif /* NAMREGCHK */


/* declarations of externally defined global symbols *************************/

extern size_t pgsz;

/* forward declarations of locally defined static functions ******************/

static rv_t namreg_flush( void );

/* definition of locally defined global variables ****************************/


/* definition of locally defined static variables *****************************/

static char *namregfile = "namreg";
static namreg_tran_t *ntp = 0;
static namreg_pers_t *npp = 0;


/* definition of locally defined global functions ****************************/

bool_t
namreg_init( char *hkdir, bool_t resume, uint64_t inocnt )
{
	if ( ntp ) {
		return BOOL_TRUE;
	}

	/* sanity checks
	 */
	assert( ! ntp );
	assert( ! npp );

	assert( sizeof( namreg_pers_t ) <= NAMREG_PERS_SZ );

	/* allocate and initialize context
	 */
	ntp = ( namreg_tran_t * )calloc( 1, sizeof( namreg_tran_t ));
	assert( ntp );

	/* generate a string containing the pathname of the namreg file
	 */
	ntp->nt_pathname = open_pathalloc( hkdir, namregfile, 0 );

	/* open the namreg file
	 */
	if ( resume ) {
		/* open existing file
		 */
		ntp->nt_fd = open( ntp->nt_pathname, O_RDWR );
		if ( ntp->nt_fd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "could not find name registry file %s: "
			      "%s\n"),
			      ntp->nt_pathname,
			      strerror( errno ));
			return BOOL_FALSE;
		}
	} else {
		/* create the namreg file, first unlinking any older version
		 * laying around
		 */
		( void )unlink( ntp->nt_pathname );
		ntp->nt_fd = open( ntp->nt_pathname,
				   O_RDWR | O_CREAT | O_EXCL,
				   S_IRUSR | S_IWUSR );
		if ( ntp->nt_fd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "could not create name registry file %s: "
			      "%s\n"),
			      ntp->nt_pathname,
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

			initsz = ( off64_t )NAMREG_PERS_SZ
				 +
				 ( ( off64_t )inocnt * NAMREG_AVGLEN );
			flock64.l_whence = 0;
			flock64.l_start = 0;
			flock64.l_len = initsz;
			rval = ioctl( ntp->nt_fd, ioctlcmd, &flock64 );
			if ( rval ) {
				if ( errno != ENOTTY ) {
					mlog( loglevel | MLOG_NOTE, _(
					      "attempt to reserve %lld bytes for %s "
					      "using %s "
					      "failed: %s (%d)\n"),
					      initsz,
					      ntp->nt_pathname,
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
	assert( ! ( NAMREG_PERS_SZ % pgsz ));
	npp = ( namreg_pers_t * ) mmap_autogrow(
				        NAMREG_PERS_SZ,
				        ntp->nt_fd,
				        ( off_t )0 );
	if ( npp == ( namreg_pers_t * )-1 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "unable to map %s: %s\n"),
		      ntp->nt_pathname,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* initialize persistent state
	 */
	if ( ! resume ) {
		npp->np_appendoff = ( off64_t )NAMREG_PERS_SZ;
	}

	/* initialize transient state
	 */
	ntp->nt_at_endpr = BOOL_FALSE;

	return BOOL_TRUE;
}

nrh_t
namreg_add( char *name, size_t namelen )
{
	off64_t oldoff;
	unsigned char c;
	nrh_t nrh;
	
	/* sanity checks
	 */
	assert( ntp );
	assert( npp );
	assert( !ntp->nt_map );

	/* make sure file pointer is positioned to append
	 */
	if ( ! ntp->nt_at_endpr ) {
		off64_t newoff;
		newoff = lseek64( ntp->nt_fd, npp->np_appendoff, SEEK_SET );
		if ( newoff == ( off64_t )-1 ) {
			mlog( MLOG_NORMAL, _(
			      "lseek of namreg failed: %s\n"),
			      strerror( errno ));
			assert( 0 );
			return NRH_NULL;
		}
		assert( npp->np_appendoff == newoff );
		ntp->nt_at_endpr = BOOL_TRUE;
	}

	if (ntp->nt_off + namelen + 1 > sizeof(ntp->nt_buf)) {
		if (namreg_flush() != RV_OK) {
			return NRH_NULL;
		}
	}

	/* save the current offset
	 */
	oldoff = npp->np_appendoff;

	/* write a one byte unsigned string length into the buffer.
	 */
	assert( namelen < 256 );
	c = ( unsigned char )( namelen & 0xff );
	ntp->nt_buf[ntp->nt_off++] = c;

	/* write the name string into the buffer.
	 */
	memcpy(ntp->nt_buf + ntp->nt_off, name, namelen);
	ntp->nt_off += namelen;

	npp->np_appendoff += ( off64_t )( 1 + namelen );
	assert( oldoff <= HDLMAX );

#ifdef NAMREGCHK

	/* encode the lsb of the len plus the first character into the handle.
	 */
	nrh = CHKMKHDL( ( nrh_t )namelen + ( nrh_t )*name, ( nrh_t )oldoff );

#else /* NAMREGCHK */

	nrh = ( nrh_t )oldoff;

#endif /* NAMREGCHK */

	return nrh;
}

/* ARGSUSED */
void
namreg_del( nrh_t nrh )
{
	/* currently not implemented - grows, never shrinks
	 */
}

static rv_t
namreg_flush( void )
{
	ssize_t nwritten;

	/* sanity checks
	*/
	assert( ntp );

	if (ntp->nt_off) {

		/* write the accumulated name strings.
		*/
		nwritten = write( ntp->nt_fd, ( void * )ntp->nt_buf, ntp->nt_off );
		if ( nwritten != ntp->nt_off ) {
			if ( nwritten < 0 ) {
				mlog( MLOG_NORMAL | MLOG_ERROR,
					_("write of namreg buffer failed: %s\n"),
					strerror( errno ));
			} else {
				mlog( MLOG_NORMAL | MLOG_ERROR,
					_("write of namreg buffer failed: "
					"expected to write %ld, actually "
					"wrote %ld\n"), ntp->nt_off, nwritten);
			}
			assert( 0 );
			return RV_UNKNOWN;
		}
		ntp->nt_off = 0;
	}
	return RV_OK;
}

int
namreg_get( nrh_t nrh,
	    char *bufp,
	    size_t bufsz )
{
	off64_t newoff;
	int nread;
	size_t len;
	char *in_bufp;
	static char read_buf[256];
	/* long enough for the longest allowed name (255), plus 1 for length */
#ifdef NAMREGCHK
	nrh_t chkbit;
#endif /* NAMREGCHK */

	/* sanity checks
	 */
	assert( ntp );
	assert( npp );

	/* make sure we aren't being given a NULL handle
	 */
	assert( nrh != NRH_NULL );

	/* convert the handle into the offset
	 */
#ifdef NAMREGCHK

	newoff = ( off64_t )( size64_t )CHKGETHDL( nrh );
	chkbit = CHKGETBIT( nrh );

#else /* NAMREGCHK */

	newoff = ( off64_t )( size64_t )nrh;

#endif /* NAMREGCHK */

	/* do sanity check on offset
	 */
	assert( newoff <= HDLMAX );
	assert( newoff < npp->np_appendoff );
	assert( newoff >= ( off64_t )NAMREG_PERS_SZ );

	lock( );

	if ( ntp->nt_map ) {

		in_bufp = ntp->nt_map + newoff - NAMREG_PERS_SZ;

	} else {

		if ( ntp->nt_at_endpr && ntp->nt_off ) {
			if (namreg_flush() != RV_OK) {
				unlock( );
				return -3;
			}
		}

		/* seek to the name
		*/
		newoff = lseek64( ntp->nt_fd, newoff, SEEK_SET );
		if ( newoff == ( off64_t )-1 ) {
			unlock( );
			mlog( MLOG_NORMAL, _(
				"lseek of namreg failed: %s\n"),
				strerror( errno ));
			return -3;
		}
		ntp->nt_at_endpr = BOOL_FALSE;

		/* read the name length and the name itself in one call
		 * NOTE: assumes read_buf is big enough for the longest
		 * allowed name (255 chars) plus one byte for length.
		 */
		nread = read( ntp->nt_fd, ( void * )read_buf, sizeof(read_buf) );
		if ( nread <= 0 ) {
			unlock( );
			mlog( MLOG_NORMAL, _(
				"read of namreg failed: %s (nread = %d)\n"),
				strerror( errno ),
				nread );
			return -3;
		}

		in_bufp = read_buf;
	}

	/* deal with a short caller-supplied buffer
	 */
	len = ( size_t )in_bufp[0];
	if ( bufsz < len + 1 ) {
		unlock( );
		return -1;
	}

	/* copy the name into the caller-supplied buffer.
	 */
	strncpy(bufp, in_bufp+1, len);

#ifdef NAMREGCHK

	/* validate the checkbit
	 */
	assert( chkbit
		==
		( ( ( nrh_t )len + ( nrh_t )bufp[ 0 ] ) & CHKBITLOMASK ));

#endif /* NAMREGCHK */

	/* null-terminate the string if room
	 */
	bufp[ len ] = 0;
	
	unlock( );

	return ( int )len;
}

rv_t
namreg_map( void )
{
	rv_t rv;

	/* ensure all entries have been written */
	if ( (rv = namreg_flush()) != RV_OK ) {
		return rv;
	}

	ntp->nt_map = ( char * ) mmap_autogrow(
					npp->np_appendoff - NAMREG_PERS_SZ,
					ntp->nt_fd,
					NAMREG_PERS_SZ );

	/* it's okay if this fails, just fall back to (the much slower)
	 * seek-and-read lookups.
	 */
	if ( ntp->nt_map == ( char * )-1 ) {
		mlog( MLOG_DEBUG, "failed to map namreg: %s\n",
			strerror( errno ) );
		ntp->nt_map = NULL;
	}

	return RV_OK;
}

/* definition of locally defined static functions ****************************/
