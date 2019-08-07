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

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "rmtlib.h"

static int _rmt_write(int, char *, unsigned int);

/*
 *	Write to stream.  Looks just like write(2) to caller.
 */

int rmtwrite (fildes, buf, nbyte)
int fildes;
char *buf;
unsigned int nbyte;
{
	if (isrmt (fildes))
	{
		return (_rmt_write (fildes - REM_BIAS, buf, nbyte));
	}
	else
	{
		return (write (fildes, buf, nbyte));
	}
}


/*
 *	_rmt_write --- write a buffer to the remote tape
 */

static int _rmt_write(int fildes, char *buf, unsigned int nbyte)
{
	char buffer[BUFMAGIC];

	sprintf(buffer, "W%d\n", nbyte);
	if (_rmt_command(fildes, buffer) == -1)
		return(-1);

	if (write(WRITE(fildes), buf, nbyte) == nbyte)
	{
		return(_rmt_status(fildes));
	}

	_rmt_abort(fildes);
	setoserror( EIO );
	return(-1);
}
