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

#include "config.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <mntent.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>
#include <xfs/xfs.h>

#include "config.h"

#include "types.h"
#include "mlog.h"
#include "fs.h"


/* fs_info - a magic, highly heuristic function to convert a user-supplied
 * string into a file system type, character special device pathname,
 * a mount point, and file system ID (uuid). The primary source of information
 * is the getmntent(3) routines. Correspondence between "standard" disk
 * block and character device pathnames is used. The fstyp(1) utility
 * may be invoked as well.
 *
 * returns BOOL_TRUE on success, BOOL_FALSE on failure.
 *
 * coding rules: to make this very complex and lengthy decision tree
 * more graspable, some very strict coding rules were followed:
 *
 *	1) function arguments are ordered reference returns first, input
 *	parameters second.
 *
 *	2) all buffer-like refence return arguments are followed by a
 *	caller-supplied buffer size.
 *
 *	3) wherever possible functions return the result buffer pointer.
 *
 *	4) boolean functions return TRUE on success, FALSE on failure.
 *
 * all variables, parameters, and structure members are named as follows:
 *	object types:
 *		usr - user-specified mystery
 *		typ - file system type
 *		mnt - mount point pathname
 *		blk - block device pathname
 *		chr - character device pathname
 *		id - file system ID
 *		stat - stat buffer
 *		te - file system table entry
 *	object modifiers: appended to object type:
 *		b - buffer
 *		s - string
 *		d - default string
 *		p - pointer
 *	object size indication modifiers: appended to a modified object type:
 *		z - buffer size
 *		l - string length
 */

/* declarations of externally defined global variables
 */


/* definitions used locally
 */
struct fs_tab_ent {
	char *fte_blks;
	char *fte_mnts;
	char *fte_typs;
	struct fs_tab_ent *fte_nextp;
};

typedef struct fs_tab_ent fs_tab_ent_t;

static	fs_tab_ent_t *fs_tabp;

/* forward declarations
 */
static void fs_tab_build( void );
static void fs_tab_free( void );
static fs_tab_ent_t *fs_tab_ent_build( struct mntent * );
static void fs_tab_ent_free( fs_tab_ent_t * );
static fs_tab_ent_t *fs_tab_lookup_blk( char * );
static fs_tab_ent_t *fs_tab_lookup_mnt( char * );

/* ARGSUSED */
bool_t
fs_info( char *typb,		/* out */
	 int typbz,
	 char *typd,
	 char *blkb,		/* out */
	 int blkbz,
	 char *mntb,		/* out */
	 int mntbz,
	 uuid_t *idb,		/* out */
	 char *usrs )		/* in */
{
	struct stat64 statb;
	fs_tab_ent_t *tep;
	char *blks;
	char *mnts;
	char *typs;
	bool_t canstat;
	bool_t ok = BOOL_UNKNOWN;

	fs_tab_build( );

	canstat = ( stat64( usrs, &statb ) == 0 );
	if ( canstat && ( statb.st_mode & S_IFMT ) == S_IFBLK ) {
		if ( ( tep = fs_tab_lookup_blk( usrs )) != 0 ) {
			blks = tep->fte_blks;
			assert( strlen( blks ) < ( size_t )blkbz );
			strcpy( blkb, blks );
			mnts = tep->fte_mnts;
			if ( mnts ) {
				assert( strlen( mnts ) < ( size_t )mntbz );
				strcpy( mntb, mnts );
			} else {
				mntb[ 0 ] = 0;
			}
			if ( ( typs = tep->fte_typs ) == 0 ) {
				typs = typd;
			}
			assert( strlen( typs ) < ( size_t )typbz );
			strcpy( typb, typs );
			ok = BOOL_TRUE;
		} else {
			ok = BOOL_FALSE;
		}
	} else if ( ( tep = fs_tab_lookup_mnt( usrs )) != 0 ) {
		blks = tep->fte_blks;
		assert( strlen( blks ) < ( size_t )blkbz );
		strcpy( blkb, blks );
		mnts = tep->fte_mnts;
		assert( strlen( mnts ) < ( size_t )mntbz );
		strcpy( mntb, mnts );
		typs = tep->fte_typs;
		assert( strlen( typs ) < ( size_t )typbz );
		strcpy( typb, typs );
		ok = BOOL_TRUE;
	} else {
		ok = BOOL_FALSE;
	}

	fs_tab_free( );
	assert( ok != BOOL_UNKNOWN );

	if ( ok == BOOL_TRUE ) {
		int rval = fs_getid( mntb, idb );
		if ( rval ) {
			mlog( MLOG_NORMAL,
			      _("unable to determine uuid of fs mounted at %s: "
			      "%s\n"),
			      mntb,
			      strerror( errno ));
		}
		{
			char string_uuid[37];
			uuid_unparse( *idb, string_uuid );
			mlog( MLOG_DEBUG,
			      "fs %s uuid [%s]\n",
			      mntb,
			      string_uuid );
		}
	}

	return ok;
}

/* fs_mounted - a predicate determining if the specified file system
 * is actually currently mounted at its mount point.
 * will not take time to code this now - just check if mntpt is non-NULL.
 */
/* ARGSUSED */
bool_t
fs_mounted( char *typs, char *chrs, char *mnts, uuid_t *idp )
{
	return strlen( mnts ) > 0 ? BOOL_TRUE : BOOL_FALSE;
}

int
fs_getid( char *mnts, uuid_t *idb )
{
	xfs_fsop_geom_v1_t geo;
	int fd;

	fd = open( mnts, O_RDONLY );
	if ( fd < 0 ) {
		uuid_clear( *idb );
		return -1;
	}
	if ( ioctl(fd, XFS_IOC_FSGEOMETRY_V1, &geo ) ) {
		uuid_clear( *idb );
		close(fd);
		return -1;
	}
	close(fd);
	uuid_copy( *idb, geo.uuid );

	return 0;
}

size_t
fs_getinocnt( char *mnts )
{
	struct statvfs vfsstat;
	int rval;

	rval = statvfs( mnts, &vfsstat );
	if ( rval ) {
		return 0;
	}

	if ( vfsstat.f_files < vfsstat.f_ffree ) {
		return 0;
	}

	return ( size_t )( vfsstat.f_files - vfsstat.f_ffree );
}

static void
fs_tab_build( void )
{
	register struct mntent *mntentp;
	fs_tab_ent_t *tep;
	FILE *fp;

	fs_tabp = 0;
	fp = setmntent( MOUNTED, "r" );
        if ( fp == NULL ) {
		mlog( MLOG_NORMAL,
		      _("Can't open %s for mount information\n"),
		      MOUNTED );
		return;
	}
	while ( ( mntentp = getmntent( fp )) != 0 ) {
		tep = fs_tab_ent_build( mntentp );
		tep->fte_nextp = fs_tabp;
		fs_tabp = tep;
	}
	endmntent( fp );
}

static void
fs_tab_free( void )
{
	fs_tab_ent_t *tep;
	fs_tab_ent_t *otep;

	for ( tep = fs_tabp
	      ;
	      tep
	      ;
	      otep = tep, tep = tep->fte_nextp, fs_tab_ent_free( otep ) )

		;
}

static fs_tab_ent_t *
fs_tab_ent_build( struct mntent *mntentp )
{
	fs_tab_ent_t *tep;
	char *cp;

	tep = ( fs_tab_ent_t * )calloc( 1, sizeof( fs_tab_ent_t ));
	assert( tep );

	if ( mntentp->mnt_dir ) {
		cp = calloc( 1, strlen( mntentp->mnt_dir ) + 1 );
		assert( cp );
		( void )strcpy( cp, mntentp->mnt_dir );
		tep->fte_mnts = cp;
	} else {
		tep->fte_mnts = 0;
	}

	if ( mntentp->mnt_type ) {
		cp = calloc( 1, strlen( mntentp->mnt_type ) + 1 );
		assert( cp );
		( void )strcpy( cp, mntentp->mnt_type );
		tep->fte_typs = cp;
	} else {
		tep->fte_typs = 0;
	}

	if ( mntentp->mnt_fsname ) {
		cp = calloc( 1, strlen( mntentp->mnt_fsname ) + 1 );
		assert( cp );
		( void )strcpy( cp, mntentp->mnt_fsname );
		tep->fte_blks = cp;
	} else {
		tep->fte_blks = 0;
	}

	return tep;
}

static void
fs_tab_ent_free( fs_tab_ent_t *tep )
{
	if ( tep->fte_blks ) free( tep->fte_blks );
	if ( tep->fte_mnts ) free( tep->fte_mnts );
	if ( tep->fte_typs ) free( tep->fte_typs );
	memset( ( void * )tep, 0, sizeof( *tep )); /* bug catcher */
	free( tep );
}

static fs_tab_ent_t *
fs_tab_lookup_blk( char *blks )
{
	fs_tab_ent_t *tep;

	for ( tep = fs_tabp ; tep ; tep = tep->fte_nextp ) {
		struct stat64 stata;
		bool_t aok;
		struct stat64 statb;
		bool_t bok;

		if ( ! tep->fte_blks ) {
			continue;
		}

		if ( ! strcmp( tep->fte_blks, blks )) {
			return tep;
		}

		aok = ! stat64( blks, &stata );
		bok = ! stat64( tep->fte_blks, &statb );
		if ( aok && bok && stata.st_rdev == statb.st_rdev ) {
			return tep;
		}
	}
	return 0;
}

static fs_tab_ent_t *
fs_tab_lookup_mnt( char *mnts )
{
	fs_tab_ent_t *tep;

	for ( tep = fs_tabp ; tep ; tep = tep->fte_nextp ) {
		if ( tep->fte_mnts && ! strcmp( tep->fte_mnts, mnts )) {
			return tep;
		}
	}
	return 0;
}
