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

#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>
#include <xfs/xfs.h>
#include <xfs/jdm.h> /* only for util.h include */

#include "config.h"

#include "types.h"
#include "util.h" /* only for strncpyterm */
#include "mlog.h"
#include "dlog.h"
#include "global.h"
#include "getopt.h"
#include "swap.h"


/* declarations of externally defined global symbols *************************/

extern void usage( void );
extern bool_t pipeline;


/* forward declarations of locally defined static functions ******************/

#ifdef DUMP
static char * prompt_label( char *bufp, size_t bufsz );
#endif /* DUMP */

/* definition of locally defined global variables ****************************/


/* definition of locally defined static variables *****************************/


/* definition of locally defined global functions ****************************/

global_hdr_t *
global_hdr_alloc( int argc, char *argv[ ] )
{
	global_hdr_t *ghdrp;
	int c;
	char *dumplabel;
#ifdef DUMP
	char labelbuf[ GLOBAL_HDR_STRING_SZ ];
	struct stat64 statb;
#endif /* DUMP */

	int rval;

	/* sanity checks
	 */
	assert( sizeof( time32_t ) == GLOBAL_HDR_TIME_SZ );
	assert( sizeof( uuid_t ) == GLOBAL_HDR_UUID_SZ );

	/* allocate a global hdr
	 */
	ghdrp = ( global_hdr_t * )calloc( 1, sizeof( global_hdr_t ));
	assert( ghdrp );

	/* fill in the magic number
	 */
	strncpy( ghdrp->gh_magic, GLOBAL_HDR_MAGIC, GLOBAL_HDR_MAGIC_SZ );

	/* fill in the hdr version
	 */
	ghdrp->gh_version = GLOBAL_HDR_VERSION;

	/* fill in the timestamp: all changes made at or after this moment
	 * will be included in increments on this base. This may be
	 * overridden with the GETOPT_DUMPTIME option.
	 */
	ghdrp->gh_timestamp = (time32_t) time( 0 );

	/* fill in the host id: typecast to fit into a 64 bit field
	 */
	ghdrp->gh_ipaddr = ( uint64_t )( unsigned long )gethostid( );

#ifdef DUMP
	uuid_generate( ghdrp->gh_dumpid );
#endif /* DUMP */
#ifdef RESTORE
	uuid_clear( ghdrp->gh_dumpid );
#endif /* RESTORE */

	/* fill in the hostname
	 */
	rval = gethostname( ghdrp->gh_hostname, GLOBAL_HDR_STRING_SZ );
	if ( rval ) {
		mlog( MLOG_NORMAL | MLOG_ERROR,
		      _("unable to determine hostname: %s\n"),
		      strerror( errno ));
		return 0;
	}
	if ( ! strlen( ghdrp->gh_hostname )) {
		mlog( MLOG_NORMAL | MLOG_ERROR,
		      _("hostname length is zero\n") );
		return 0;
	}

	/* scan the command line for the dump session label
	 */
	dumplabel = 0;
	optind = 1;
	opterr = 0;
	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
                case GETOPT_DUMPLABEL:
                        if ( dumplabel ) {
                                mlog( MLOG_NORMAL,
                                      _("too many -%c arguments: "
                                      "\"-%c %s\" already given\n"),
                                      c,
                                      c,
                                      dumplabel );
                                usage( );
                                return 0;
                        }
                        if ( ! optarg || optarg[ 0 ] == '-' ) {
                                mlog( MLOG_NORMAL | MLOG_ERROR,
                                      _("-%c argument missing\n"),
                                      c );
                                usage( );
                                return 0;
                        }
                        dumplabel = optarg;
                        break;
#ifdef RESTORE
		case GETOPT_SESSIONID:
                        if ( ! uuid_is_null( ghdrp->gh_dumpid )) {
                                mlog( MLOG_NORMAL | MLOG_ERROR,
                                      _("too many -%c arguments\n"),
                                      c );
                                usage( );
                                return 0;
                        }
                        if ( ! optarg || optarg[ 0 ] == '-' ) {
                                mlog( MLOG_NORMAL | MLOG_ERROR,
                                      _("-%c argument missing\n"),
                                      c );
                                usage( );
                                return 0;
                        }
			
			if ( ! uuid_parse( optarg, ghdrp->gh_dumpid ) ) {
				mlog( MLOG_NORMAL | MLOG_ERROR,
				      _("-%c argument not a valid uuid\n"),
				      c );
                                usage( );
                                return 0;
                        }
                        break;
#endif /* RESTORE */
#ifdef DUMP
		case GETOPT_DUMPTIME:
			/* Use the timestamp of the specified file for the
			 * dump time, rather than using the current time.
			 */
                        if ( ! optarg || optarg[ 0 ] == '-' ) {
                                mlog( MLOG_NORMAL | MLOG_ERROR,
                                      _("-%c argument missing\n"),
                                      c );
                                usage( );
                                return 0;
                        }
			rval = stat64( optarg, &statb );
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_ERROR,
				      _("unable to stat %s: %s\n"),
				      optarg,
				      strerror( errno ));
				usage( );
				return 0;
			}
			ghdrp->gh_timestamp = statb.st_mtime;
			break;

		case GETOPT_FMT2COMPAT:
			ghdrp->gh_version = GLOBAL_HDR_VERSION_2;
			break;
#endif /* DUMP */
		}
	}

#ifdef DUMP
	/* if no dump label specified, no pipes in use, and dialogs
	 * are allowed, prompt for one
	 */
	if ( ! dumplabel && dlog_allowed( )) {
		dumplabel = prompt_label( labelbuf, sizeof( labelbuf ));
	}
#endif /* DUMP */

	if ( ! dumplabel || ! strlen( dumplabel )) {
#ifdef DUMP
		if ( ! pipeline ) {
			mlog( MLOG_VERBOSE | MLOG_WARNING,
			      _("no session label specified\n") );
		}
#endif /* DUMP */
		dumplabel = "";
	}

	strncpyterm( ghdrp->gh_dumplabel,
		     dumplabel,
		     sizeof( ghdrp->gh_dumplabel ));

	return ghdrp;
}


void
global_hdr_free( global_hdr_t *ghdrp )
{
    free( ( void * )ghdrp );
}

/* global_hdr_checksum_set - fill in the global media file header checksum.
 * utility function for use by drive-specific strategies.
 */
void
global_hdr_checksum_set( global_hdr_t *hdrp )
{
	uint32_t *beginp = ( uint32_t * )&hdrp[ 0 ];
	uint32_t *endp = ( uint32_t * )&hdrp[ 1 ];
	uint32_t *p;
	uint32_t accum;

	hdrp->gh_checksum = 0;
	accum = 0;
	for ( p = beginp ; p < endp ; p++ ) {
		accum += INT_GET(*p, ARCH_CONVERT);
	}
	INT_SET(hdrp->gh_checksum, ARCH_CONVERT, (int32_t)(~accum + 1));
}

/* global_hdr_checksum_check - check the global media file header checksum.
 * utility function for use by drive-specific strategies.
 * returns BOOL_TRUE if ok, BOOL_FALSE if bad
 */
bool_t
global_hdr_checksum_check( global_hdr_t *hdrp )
{
	uint32_t *beginp = ( uint32_t * )&hdrp[ 0 ];
	uint32_t *endp = ( uint32_t * )&hdrp[ 1 ];
	uint32_t *p;
	uint32_t accum;

	accum = 0;
	for ( p = beginp ; p < endp ; p++ ) {
		accum += INT_GET(*p, ARCH_CONVERT);
	}
	return accum == 0 ? BOOL_TRUE : BOOL_FALSE;
}

/* global_version_check - if we know this version number, return BOOL_TRUE 
 * else return BOOL_FALSE
 */
bool_t 
global_version_check( uint32_t version )
{
	switch (version) {
		case GLOBAL_HDR_VERSION_0:
		case GLOBAL_HDR_VERSION_1:
		case GLOBAL_HDR_VERSION_2:
		case GLOBAL_HDR_VERSION_3:
			return BOOL_TRUE;
		default:
			return BOOL_FALSE;
	}
}

/* definition of locally defined static functions ****************************/

#ifdef DUMP
#define PREAMBLEMAX	3
#define QUERYMAX	1
#define CHOICEMAX	1
#define ACKMAX		3
#define POSTAMBLEMAX	3
#define DLOG_TIMEOUT	300

/* ARGSUSED */
static void
prompt_label_cb( void *uctxp, dlog_pcbp_t pcb, void *pctxp )
{
	/* query: ask for a dump label
	 */
	( * pcb )( pctxp,
		   _("please enter label for this dump session") );
}

static char *
prompt_label( char *bufp, size_t bufsz )
{
	fold_t fold;
	char *preamblestr[ PREAMBLEMAX ];
	size_t preamblecnt;
	char *ackstr[ ACKMAX ];
	size_t ackcnt;
	char *postamblestr[ POSTAMBLEMAX ];
	size_t postamblecnt;
	const ix_t abortix = 1;
	const ix_t okix = 2;
	ix_t responseix;

	preamblecnt = 0;
	fold_init( fold, _("dump label dialog"), '=' );
	preamblestr[ preamblecnt++ ] = "\n";
	preamblestr[ preamblecnt++ ] = fold;
	preamblestr[ preamblecnt++ ] = "\n\n";
	assert( preamblecnt <= PREAMBLEMAX );
	dlog_begin( preamblestr, preamblecnt );

	responseix = dlog_string_query( prompt_label_cb,
					0,
					bufp,
					bufsz,
					DLOG_TIMEOUT,
					abortix,/* timeout ix */
					IXMAX, /* sigint ix */
					IXMAX,  /* sighup ix */
					IXMAX,  /* sigquit ix */
					okix );   /* ok ix */
	ackcnt = 0;
	if ( responseix == okix ) {
		ackstr[ ackcnt++ ] = _("session label entered: \"");
		ackstr[ ackcnt++ ] = bufp;
		ackstr[ ackcnt++ ] = "\"\n";
	} else {
		ackstr[ ackcnt++ ] = _("session label left blank\n");
	}

	assert( ackcnt <= ACKMAX );
	dlog_string_ack( ackstr,
			 ackcnt );

	postamblecnt = 0;
	fold_init( fold, _("end dialog"), '-' );
	postamblestr[ postamblecnt++ ] = "\n";
	postamblestr[ postamblecnt++ ] = fold;
	postamblestr[ postamblecnt++ ] = "\n\n";
	assert( postamblecnt <= POSTAMBLEMAX );
	dlog_end( postamblestr,
		  postamblecnt );

	if ( responseix == okix ) {
		return bufp;
	} else {
		return 0;
	}
}
#endif /* DUMP */
