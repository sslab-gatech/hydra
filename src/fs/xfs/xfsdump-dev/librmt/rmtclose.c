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
#include "rmtlib.h"

static int _rmt_close(int);
/*
 *	Close a file.  Looks just like close(2) to caller.
 */

int rmtclose (fildes)
int fildes;
{
	if (isrmt (fildes))
	{
		/* no longer know what host we have for this fildes */
		RMTHOST(fildes - REM_BIAS) = UNAME_UNKNOWN;

		return (_rmt_close (fildes - REM_BIAS));
	}
	else
	{
		return (close (fildes));
	}
}

/*
 *	_rmt_close --- close a remote magtape unit and shut down
 */

static int _rmt_close(int fildes)
{
	int rc;

	if (_rmt_command(fildes, "C\n") != -1)
	{
		rc = _rmt_status(fildes);

		_rmt_abort(fildes);
		return(rc);
	}

	return(-1);
}


