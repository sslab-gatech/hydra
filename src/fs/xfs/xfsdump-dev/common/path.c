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

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "path.h"

struct pem {
	char *pem_head;
	char *pem_next;
};

typedef struct pem pem_t;

static pem_t * pem_alloc( char *path );
static void pem_free( pem_t *pemp );
static char * pem_next( pem_t *pemp );

#define PAMAX	1024

struct pa {
	char *pa_array[ PAMAX ];
	int pa_cnt;
};

typedef struct pa pa_t;

static pa_t * pa_alloc( void );
static void pa_free( pa_t *pap );
static void pa_append( pa_t *pap, char *pep );
static int pa_peel( pa_t *pap );
static char * pa_gen( pa_t *pap );

char *
path_diff( char *path, char *base )
{
	char *diff;

	assert( *base == '/' );
	assert( *path == '/' );

	if ( ! path_beginswith( path, base )) {
		return 0;
	}

	for ( ; *base && *path == *base ; path++, base++ )
		;

	if ( *path == 0 ) {
		return 0;
	}

	if ( *path == '/' ) {
		path++;
	}

	diff = ( char * )calloc( 1, strlen( path ) + 1 );
	assert( diff );
	strcpy( diff, path );

	return diff;
}

int
path_beginswith( char *path, char *base )
{
	if ( ! base ) {
		return 0;
	}
	return ! strncmp( base, path, strlen( base ));
}

char *
path_reltoabs( char *dir, char *basedir )
{
	char *absdir;

	/* check if the path starts with a / or
	 * is a remote path (i.e. contains  machine:/path/name ).
	 */
	if ( ( *dir != '/' ) && ( strchr(dir, ':') == 0 ) ) {
		char *absdir;
		absdir = ( char * )malloc( strlen( basedir )
					   +
					   1
					   +
					   strlen( dir )
					   +
					   1 );
		assert( absdir );

		( void )sprintf( absdir, "%s/%s", basedir, dir );

		dir = absdir;
	}

	if ( strchr(dir, ':') == 0 ) {
		absdir = path_normalize( dir );
	} else {
		absdir = ( char * )malloc( strlen( dir )  + 1);
		( void )sprintf( absdir, "%s", dir);
	}

	return absdir;
}

char *
path_normalize( char *path )
{
	pem_t *pemp = pem_alloc( path );
	pa_t *pap = pa_alloc( );
	char *pep;
	char *npath;

	assert( path[ 0 ] == '/' );

	while ( ( pep = pem_next( pemp )) != 0 ) {
		if ( ! strcmp( pep, "" )) {
			free( ( void * )pep );
			continue;
		}
		if ( ! strcmp( pep, "." )) {
			free( ( void * )pep );
			continue;
		}
		if ( ! strcmp( pep, ".." )) {
			int ok;
			free( ( void * )pep );
			ok = pa_peel( pap );
			if ( ! ok ) {
				pa_free( pap );
				pem_free( pemp );
				return 0;
			}
			continue;
		}
		pa_append( pap, pep );
	}

	npath = pa_gen( pap );
	pa_free( pap );
	pem_free( pemp );

	return npath;
}

static pem_t *
pem_alloc( char *path )
{
	pem_t *pemp = ( pem_t * )calloc( 1, sizeof( pem_t ));
	assert( pemp );
	pemp->pem_head = path;
	pemp->pem_next = pemp->pem_head;

	return pemp;
}

static void
pem_free( pem_t *pemp )
{
	free( ( void * )pemp );
}

static char *
pem_next( pem_t *pemp )
{
	char *nextnext;
	size_t len;
	char *p;

	/* no more left
	 */
	if ( *pemp->pem_next == 0 ) {
		return 0;
	}

	/* find the following slash
	 */
	nextnext = strchr( pemp->pem_next + 1, '/' );

	/* if end of string encountered, place next next at end of string
	 */
	if ( ! nextnext ) {
		for ( nextnext = pemp->pem_next ; *nextnext ; nextnext++ )
			;
	}

	/* determine the length of the path element, sans the leading slash
	 */
	len = ( size_t )( nextnext - pemp->pem_next - 1 );

	/* allocate buffer to hold the path element, incl null termination
	 */
	p = ( char * )malloc( len + 1 );
	assert( p );

	/* copy the path element into the buffer
	 */
	strncpy( p, pemp->pem_next + 1, len );

	/* null-terminate
	 */
	p[ len ] = 0;

	/* update next
	 */
	pemp->pem_next = nextnext;

	/* return the allocated buffer to the caller
	 */
	return p;
}

static pa_t *
pa_alloc( void )
{
	pa_t *pap = ( pa_t * )calloc( 1, sizeof( pa_t ));
	assert( pap );

	return pap;
}

static void
pa_free( pa_t *pap )
{
	int i;

	for ( i = 0 ; i < pap->pa_cnt ; i++ ) {
		free( ( void * )pap->pa_array[ i ] );
	}

	free( ( void * )pap );
}

static void
pa_append( pa_t *pap, char *pep )
{
	assert( pap->pa_cnt < PAMAX );

	pap->pa_array[ pap->pa_cnt ] = pep;

	pap->pa_cnt++;
}

static int
pa_peel( pa_t *pap )
{
	if ( pap->pa_cnt <= 0 ) {
		assert( pap->pa_cnt == 0 );
		return 0;
	}

	pap->pa_cnt--;
	assert( pap->pa_array[ pap->pa_cnt ] );
	free( ( void * )pap->pa_array[ pap->pa_cnt ] );
	pap->pa_array[ pap->pa_cnt ] = 0;

	return 1;
}

static char *
pa_gen( pa_t *pap )
{
	size_t sz;
	int i;
	char *retp;
	char *p;

	sz = 0;
	for ( i = 0 ; i < pap->pa_cnt ; i++ ) {
		sz += strlen( pap->pa_array[ i ] ) + 1;
	}
	if ( i == 0 )
		sz++;
	sz++;

	retp = ( char * )malloc( sz );

	if (  pap->pa_cnt <= 0 ) {
		assert(  pap->pa_cnt == 0 );
		sprintf( retp, "/" );
	} else {
		p = retp;
		for ( i = 0 ; i < pap->pa_cnt ; i++ ) {
			sprintf( p, "/%s", pap->pa_array[ i ] );
			p += strlen( p );
		}
	}

	return retp;
}
