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

#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "rmtlib.h"


/*
 *	_rmt_command --- attempt to perform a remote tape command
 */

int _rmt_command(fildes, buf)
int fildes;
char *buf;
{
	register int blen;

	_rmt_msg(RMTDBG, "rmtcommand: fd = %d, buf = %s\n", fildes, buf);

/*
 *	try to make the request
 */

	blen = strlen(buf);
	if (write(WRITE(fildes), buf, blen) == blen)
	{
		return(0);
	}

/*
 *	something went wrong. close down and go home
 */

	_rmt_abort(fildes);

	setoserror( EIO );
	return(-1);
}



