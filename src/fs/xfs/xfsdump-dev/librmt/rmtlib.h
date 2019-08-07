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
 *	This file is only included by the library routines.  It is not
 *	required for user code.
 *
 */

/*
 *	Note that local vs remote file descriptors are distinquished
 *	by adding a bias to the remote descriptors.  This is a quick
 *	and dirty trick that may not be portable to some systems.
 *  It should be greater than the largest open filedescriptor
 *  than can be returned by the OS, and should be a power of 2.
 */

#define REM_BIAS 8192


/*
 *	BUFMAGIC --- Magic buffer size
 *	MAXUNIT --- Maximum number of remote tape file units
 */

#define BUFMAGIC	64
#define MAXUNIT		4

/*
 *	Useful macros.
 *
 *	READ --- Return the number of the read side file descriptor
 *	WRITE --- Return the number of the write side file descriptor
 *	RMTHOST --- Return an id which says host type from uname    
 */

/* rmt msg types */
#define RMTWARN 1
#define RMTDBG 	2 /* includes warning */

#define READ(fd)	(_rmt_Ctp[fd][0])
#define WRITE(fd)	(_rmt_Ptc[fd][1])
#define RMTHOST(fd)	(_rmt_host[fd])

#define RSH_PATH        "/usr/bin/rsh"
#define RMT_PATH        "/etc/rmt"

#define UNAME_UNDEFINED	-1
#define UNAME_LINUX	0
#define UNAME_IRIX	1
#define UNAME_UNKNOWN	2

extern int _rmt_Ctp[MAXUNIT][2];
extern int _rmt_Ptc[MAXUNIT][2];
extern int _rmt_host[MAXUNIT];

#define setoserror(err) (errno = err) /* TODO: multithread check */

/* prototypes */
int isrmt (int);
void _rmt_abort(int);
int _rmt_command(int, char *);
int _rmt_dev (char *);
int _rmt_status(int);
int _rmt_msgson(void);
void _rmt_msg(int level, const char *msg, ...);
void _rmt_turnonmsgsbyenv(void);
void rmt_turnonmsgs(int code);
