/*
 * Copyright (c) 2009 Silicon Graphics, Inc.
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
#ifndef TIMEUTIL_H
#define TIMEUTIL_H

/* timeutil.[hc] - time32_t related functions
 */


/* ctime32 - takes a time32_t instead of a time_t
 */
extern char *ctime32( const time32_t *timep );

/* ctime32_r - takes a time32_t instead of a time_t
 */
extern char *ctime32_r( const time32_t *timep, char *buf );

/* ctimennl - ctime(3C) with newline removed
 */
extern char *ctimennl( const time32_t *clockp );

#endif /* TIMEUTIL_H */
