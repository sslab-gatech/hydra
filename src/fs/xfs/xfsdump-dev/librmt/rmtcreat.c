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

/*
 *	Create a file from scratch.  Looks just like creat(2) to the caller.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>		/* use this one for S5 with remote stuff */
#include "rmtlib.h"

extern int rmtopen(char*, int, int);

int rmtcreat (path, mode)
char *path;
int mode;
{
	if (_rmt_dev (path))
	{
		return (rmtopen (path, 1 | O_CREAT, mode));
	}
	else
	{
		return (creat (path, mode));
	}
}


