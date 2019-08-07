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

#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <assert.h>
#include <string.h>

#include "config.h"


/*----------------------------------------------------------------------*/
/*                                                                      */
/* Functions which access inventory paths: /var/[lib/]xfsdump/inventory */
/* The [lib] component was introduced for FHS compliance on Linux.      */
/*                                                                      */
/*----------------------------------------------------------------------*/

#define MGR_BASE_NEW	"/var/lib/xfsdump"
#define MGR_BASE_OLD	"/var/xfsdump"
#define MGR_DIRPATH	"/inventory"		/* preceed with inv_base */
#define MGR_FSTAB	MGR_DIRPATH"/fstab"	/* preceed with inv_base */
#define MGR_LOCKFILE	MGR_DIRPATH"/invlock"	/* preceed with inv_base */
#define MGR_PATH_MAX	64	/* PATH_MAX would be a tad wasteful here */

static char *inv_base;
static char new_inv_base[] = MGR_BASE_NEW;
static char old_inv_base[] = MGR_BASE_OLD;

static char inv_fstabp[MGR_PATH_MAX];
static char inv_dirpathp[MGR_PATH_MAX];
static char inv_lockfilep[MGR_PATH_MAX];

char *
inv_dirpath( void )
{
	assert(inv_base);
	return inv_dirpathp;
}

char *
inv_fstab( void )
{
	assert(inv_base);
	return inv_fstabp;
}

char *
inv_lockfile( void )
{
	assert(inv_base);
	return inv_lockfilep;
}

char *
inv_basepath( void )
{
	assert(inv_base);
	return inv_base;
}

int
inv_setup_base( void )
{
	struct stat64 new_sbuf, old_sbuf;
	int new_exists, old_exists;

	errno = 0;
	new_exists = !(stat64(new_inv_base, &new_sbuf) < 0 && errno == ENOENT);
	errno = 0;
	old_exists = !(stat64(old_inv_base, &old_sbuf) < 0 && errno == ENOENT);

	if (new_exists && old_exists) {
		if (new_sbuf.st_ino == old_sbuf.st_ino &&
		    new_sbuf.st_dev == old_sbuf.st_dev) {
			old_exists = 0;	/* both the same, pretend */
		}
		else {	/* die! */
			return 0;
		}
	}

	if (!old_exists)
		inv_base = new_inv_base;
	else
		inv_base = old_inv_base;

	/* also setup the dependent paths now
	 */
	strcpy(inv_dirpathp, inv_base);
	strcat(inv_dirpathp, MGR_DIRPATH);

	strcpy(inv_fstabp, inv_base);
	strcat(inv_fstabp, MGR_FSTAB);

	strcpy(inv_lockfilep, inv_base);
	strcat(inv_lockfilep, MGR_LOCKFILE);

	return 1;
}
