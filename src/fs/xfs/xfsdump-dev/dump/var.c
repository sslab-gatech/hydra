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

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <uuid/uuid.h>

#include "config.h"

#include "types.h"
#include "fs.h"
#include "openutil.h"
#include "mlog.h"
#include "global.h"
#include "inventory.h"

static void var_skip_recurse( char *, void ( * )( xfs_ino_t ));
static int  var_create_component( char * );

void
var_create( void )
{
	char path[PATH_MAX];
	char *p;

	p = strcpy( path, XFSDUMP_DIRPATH );
	mlog( MLOG_DEBUG, "creating directory %s\n", path );

	do {
		p++;
		if ( *p == '/' ) {
			*p = '\0';
			if ( ! var_create_component( path ) )
				return;
			*p = '/';
		}
	} while ( *p );

	( void ) var_create_component( path );
}

static int
var_create_component( char *path )
{
	int rval = mkdir( path, 0755 );

	if ( rval && errno != EEXIST ) {
		mlog( MLOG_NORMAL, _("unable to create %s: %s\n"),
		      path, strerror( errno ));
		return 0;
	}
	if ( rval == 0 ) {
		rval = chown( path, 0, 0 );
		if ( rval ) {
			mlog( MLOG_NORMAL, _("unable to chown %s: %s\n"),
			      path, strerror( errno ));
		}
	}
	return 1;
}

void
var_skip( uuid_t *dumped_fsidp, void ( *cb )( xfs_ino_t ino ))
{
	uuid_t fsid;
	int rval;

	/* see if the fs uuid's match
	 */
	rval = fs_getid( XFSDUMP_DIRPATH, &fsid );
	if ( rval ) {
#ifdef HIDDEN
                /* NOTE: this will happen for non-XFS file systems */
                /*       and is expected, so no msg */
		mlog( MLOG_NORMAL, _(
		      "unable to determine uuid of fs containing %s: "
		      "%s\n"),
		      XFSDUMP_DIRPATH,
		      strerror( errno ));
#endif
		return;
	}

	if ( uuid_compare( *dumped_fsidp, fsid ) != 0) {
		return;
	}

	/* traverse the xfsdump directory, getting inode numbers of it
	 * and all of its children, and reporting those to the callback.
	 */
	var_skip_recurse( XFSDUMP_DIRPATH, cb );
}

static void
var_skip_recurse( char *base, void ( *cb )( xfs_ino_t ino ))
{
	struct stat64 statbuf;
	DIR *dirp;
	struct dirent *direntp;
	int rval;

	rval = lstat64( base, &statbuf );
	if ( rval ) {
		mlog( MLOG_NORMAL, _(
		      "unable to get status of %s: %s\n"),
		      base,
		      strerror( errno ));
		return;
	}

	mlog( MLOG_DEBUG,
	      "excluding %s from dump\n",
	      base );

	( * cb )( statbuf.st_ino );

	if ( ( statbuf.st_mode & S_IFMT ) != S_IFDIR ) {
		return;
	}

	dirp = opendir( base );
	if ( ! dirp ) {
		mlog( MLOG_NORMAL, _(
		      "unable to open directory %s\n"),
		      base );
		return;
	}

	while ( ( direntp = readdir( dirp )) != NULL ) {
		char *path;

		/* skip "." and ".."
		 */
		if ( *( direntp->d_name + 0 ) == '.'
		     &&
		     ( *( direntp->d_name + 1 ) == 0
		       ||
		       ( *( direntp->d_name + 1 ) == '.'
			 &&
			 *( direntp->d_name + 2 ) == 0 ))) {
			continue;
		}

		path = open_pathalloc( base, direntp->d_name, 0 );
		var_skip_recurse( path, cb );
		free( ( void * )path );
	}

	closedir( dirp );
}
