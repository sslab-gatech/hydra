/*
 * Copyright (c) 2000-2001 Silicon Graphics, Inc.; provided copyright in
 * certain portions may be held by third parties as indicated herein.
 * All Rights Reserved.
 *
 * The code in this source file represents an aggregation of work from
 * Georgia Tech, Fred Fish, Jeff Lee, Arnold Robbins and other Silicon
 * Graphics engineers over the period 1985-2000.
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

#include "rmtlib.h"

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

static int _rmt_fstat(int, char *);

/*
 *	Get file status.  Looks just like fstat(2) to caller.
 */

int rmtfstat (fildes, buf)
int fildes;
struct stat *buf;
{
	if (isrmt (fildes))
	{
		return (_rmt_fstat (fildes - REM_BIAS, (char *)buf));
	}
	else
	{
		int i;
		i = fstat(fildes, buf);
		return i;
	}
}

static int
_rmt_fstat(int fildes, char *arg)
{
	char buffer[ BUFMAGIC ];
	int rc, cnt, adj_rc;

	sprintf( buffer, "Z%d\n", fildes );

	/*
	 *	grab the status and read it directly into the structure
	 *	this assumes that the status buffer is (hopefully) not
	 *	padded and that 2 shorts fit in a long without any word
	 *	alignment problems, ie - the whole struct is contiguous
	 *	NOTE - this is probably NOT a good assumption.
	 */

	if (_rmt_command(fildes, buffer) == -1 ||
	    (rc = _rmt_status(fildes)) == -1)
		return(-1);

	/* adjust read count to prevent overflow */

	adj_rc = (rc > sizeof(struct stat)) ? sizeof(struct stat) : rc ;
	rc -= adj_rc;

	for (; adj_rc > 0; adj_rc -= cnt, arg += cnt)
	{
		cnt = read(READ(fildes), arg, adj_rc);
		if (cnt <= 0)
		{
abortit:
			_rmt_abort(fildes);
			setoserror( EIO );
			return(-1);
		}
	}

	/* handle any bytes we didn't know what to do with */
	while (rc-- > 0)
		if (read(READ(fildes), buffer, 1) <= 0)
			goto abortit;

	return(0);
}
