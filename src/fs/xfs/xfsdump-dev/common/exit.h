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
#ifndef EXIT_H
#define EXIT_H

/* exit codes for main and child processes
 */
#define EXIT_NORMAL	0	/* normal completion / don't exit */
#define EXIT_ERROR	1	/* resource error or resource exhaustion */
#define EXIT_INTERRUPT	2	/* interrupted (operator or device error) */
#define EXIT_FAULT	4	/* code fault */

static inline const char *
exit_codestring( int code )
{
	switch ( code ) {
	case EXIT_NORMAL:    return "SUCCESS";
	case EXIT_ERROR:     return "ERROR";
	case EXIT_INTERRUPT: return "INTERRUPT";
	case EXIT_FAULT:     return "FAULT";
	}
	return "UNKNOWN";
}

#endif /* EXIT_H */
