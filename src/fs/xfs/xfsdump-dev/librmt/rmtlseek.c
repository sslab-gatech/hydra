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

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "rmtlib.h"

static off_t _rmt_lseek(int, off_t, int);

/*
 *	Perform lseek on file.  Looks just like lseek(2) to caller.
 */

off_t rmtlseek (fildes, offset, whence)
int fildes;
off_t offset;
int whence;
{
	if (isrmt (fildes))
	{
		return (_rmt_lseek (fildes - REM_BIAS, offset, whence));
	}
	else
	{
		return (lseek (fildes, offset, whence));
	}
}


/*
 *	_rmt_lseek --- perform an imitation lseek operation remotely
 */

static off_t _rmt_lseek(int fildes, off_t offset, int whence)
{
	char buffer[BUFMAGIC];

	sprintf(buffer, "L%ld\n%d\n", (long)offset, whence);
	if (_rmt_command(fildes, buffer) == -1)
		return(-1);

	return(_rmt_status(fildes));
}


