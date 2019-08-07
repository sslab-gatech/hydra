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

#include <sys/types.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "timeutil.h"

char *
ctime32(const time32_t *timep)
{
	time_t t = (time_t) *timep;
	return ctime(&t);
}

char *
ctime32_r(const time32_t *timep, char *buf)
{
	time_t t = (time_t) *timep;
	return ctime_r(&t, buf);
}

char *
ctimennl( const time32_t *clockp )
{
	char *p = ctime32( clockp );

	if ( p && strlen( p ) > 0 ) {
		p[ strlen( p ) - 1 ] = 0;
	}

	return p;
}
