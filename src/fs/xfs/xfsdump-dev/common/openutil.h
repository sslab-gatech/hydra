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
#ifndef OPENUTIL_H
#define OPENUTIL_H

/* openutil.[hc] - useful functions for opening tmp and housekeeping
 * files.
 */


/* allocate and fill a character sting buffer with a pathname generated
 * by catenating the dir and base args. if pid is non-zero, the decimal
 * representation of the pid will be appended to the pathname, beginning
 * with a '.'.
 */
extern char *open_pathalloc( char *dirname, char *basename, pid_t pid );

/* create the specified file, creating or truncating as necessary,
 * with read and write permissions, given a directory and base.
 * return the file descriptor, or -1 with errno set. uses mlog( MLOG_NORMAL...
 * if the creation fails.
 */
extern int open_trwdb( char *dirname, char *basename, pid_t pid );
extern int open_trwp( char *pathname );


/* open the specified file, with read and write permissions, given a
 * directory and base.* return the file descriptor, or -1 with errno set.
 * uses mlog( MLOG_NORMAL... if the open fails.
 */
extern int open_rwdb( char *dirname, char *basename, pid_t pid );
extern int open_rwp( char *pathname );


/* create and open the specified file, failing if already exists
 */
extern int open_erwp( char *pathname );
extern int open_erwdb( char *dirname, char *basename, pid_t pid );


/* create the specified directory, guaranteed to be initially empty. returns
 * 0 on success, -1 if trouble. uses mlog( MLOG_NORMAL... if the creation fails.
 */
extern int mkdir_tp( char *pathname );


#endif /* UTIL_H */
