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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

#include "config.h"

#include "types.h"
#include "mlog.h"

char *
open_pathalloc( char *dirname, char *basename, pid_t pid )
{
	size_t dirlen;
	size_t pidlen;
	size_t namelen;
	char *namebuf;

	if ( strcmp( dirname, "/" )) {
		dirlen = strlen( dirname );
	} else {
		dirlen = 0;
		dirname = "";
	}

	/*
	 * We could calculate the length of pid string
	 * = trunc(log10(pid))+1,
	 * but we are restricted to 32 bits for pid anyway.
	 * 32 bits => trunc(log10(2^32))+1 = 10
	 * And if it ever became 64 bits,
	 * 64 bits => trunc(log10(2^64))+1 = 20
         */
  	if ( pid ) {
		pidlen = 1 + 20;
  	} else {
  		pidlen = 0;
  	}
	namelen = dirlen + 1 + strlen( basename ) + pidlen + 1;
	namebuf = ( char * )calloc( 1, namelen );
  	assert( namebuf );

  	if ( pid ) {
		( void )snprintf( namebuf, namelen, "%s/%s.%d", dirname, basename, pid );
  	} else {
		( void )snprintf( namebuf, namelen, "%s/%s", dirname, basename );
  	}

  	return namebuf;
}

int
open_trwp( char *pathname )
{
	int fd;

	fd = open( pathname, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR );
	if ( fd < 0 ) {
		mlog( MLOG_NORMAL,
		      _("could not create %s: %s\n"),
		      pathname,
		      strerror( errno ));
	}

	return fd;
}

int
open_erwp( char *pathname )
{
	int fd;

	fd = open( pathname, O_EXCL | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
	if ( fd < 0 ) {
		mlog( MLOG_NORMAL,
		      _("could not create %s: %s\n"),
		      pathname,
		      strerror( errno ));
	}

	return fd;
}

int
open_rwp( char *pathname )
{
	int fd;

	fd = open( pathname, O_RDWR );

	return fd;
}

int
mkdir_tp( char *pathname )
{
	int rval;

	rval = mkdir( pathname, S_IRWXU );

	return rval;
}

int
open_trwdb( char *dirname, char *basename, pid_t pid )
{
	char *pathname;
	int fd;

	pathname = open_pathalloc( dirname, basename, pid );
	fd = open_trwp( pathname );
	free( ( void * )pathname );

	return fd;
}

int
open_erwdb( char *dirname, char *basename, pid_t pid )
{
	char *pathname;
	int fd;

	pathname = open_pathalloc( dirname, basename, pid );
	fd = open_erwp( pathname );
	free( ( void * )pathname );

	return fd;
}

int
open_rwdb( char *dirname, char *basename, pid_t pid )
{
	char *pathname;
	int fd;

	pathname = open_pathalloc( dirname, basename, pid );
	fd = open_rwp( pathname );
	free( ( void * )pathname );

	return fd;
}
