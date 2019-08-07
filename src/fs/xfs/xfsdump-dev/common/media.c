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

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <sys/dirent.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>

#include "config.h"

#include "types.h"
#include "util.h"
#include "mlog.h"
#include "getopt.h"
#include "stream.h"
#include "global.h"
#include "drive.h"
#include "media.h"

/* media.c - selects and initializes a media strategy
 */


/* declarations of externally defined global symbols *************************/

extern void usage( void );

/* declare all media strategies here
 */
extern media_strategy_t media_strategy_simple;
extern media_strategy_t media_strategy_rmvtape;


/* forward declarations of locally defined static functions ******************/

static media_t *media_alloc( drive_t *, char * );


/* definition of locally defined global variables ****************************/


/* definition of locally defined static variables *****************************/

/* media strategy array - ordered by precedence
 */
static media_strategy_t *strategyp[] = {
	&media_strategy_simple,
	&media_strategy_rmvtape,
};


/* definition of locally defined global functions ****************************/

/* media_create - select and initialize a media strategy.
 * and create and initialize media managers for each stream.
 */
media_strategy_t *
media_create( int argc, char *argv[ ], drive_strategy_t *dsp )
{
	int c;
	size_t mediaix;
	size_t mediacnt;
	media_t **mediapp;
	char *medialabel;
	media_strategy_t **spp = strategyp;
	media_strategy_t **epp = strategyp + sizeof( strategyp )
					     /
					     sizeof( strategyp[ 0 ] );
	media_strategy_t *chosen_sp;
	int id;
	bool_t ok;

	/* sanity check asserts
	 */
	assert( sizeof( media_hdr_t ) == MEDIA_HDR_SZ );
	assert( MEDIA_MARKLOG_SZ == sizeof( media_marklog_t ));

	/* scan the command line for a media label
	 */
	medialabel = 0;
	optind = 1;
	opterr = 0;
	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
#ifdef DUMP
		case GETOPT_MEDIALABEL:
			if ( medialabel ) {
				mlog( MLOG_NORMAL,
				      _("too many -%c arguments: "
				      "\"-%c %s\" already given\n"),
				      c,
				      c,
				      medialabel );
				usage( );
				return 0;
			}
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL,
				      _("-%c argument missing\n"),
				      c );
				usage( );
				return 0;
			}
			medialabel = optarg;
			break;
#endif /* DUMP */
		}
	}

	/* if no media label specified, synthesize one
	 */
	if ( ! medialabel ) {
		/* not useful
		mlog( MLOG_VERBOSE,
		      _("WARNING: no media label specified\n") );
		*/
		medialabel = "";
	}

	/* create a media_t array, and a media_ts for each drive.
	 * Initialize each media_t's generic portions. these will
	 * be lended to each media strategy during the strategy
	 * match phase, and given to the winning strategy.
	 */
	mediacnt = dsp->ds_drivecnt;
	mediapp = ( media_t ** )calloc( mediacnt, sizeof( media_t * ));
	assert( mediapp );
	for ( mediaix = 0 ; mediaix < mediacnt ; mediaix++ ) {
		mediapp[ mediaix ] = media_alloc( dsp->ds_drivep[ mediaix ],
					 	  medialabel );
	}

	/* choose the first strategy which claims appropriateness.
	 * if none match, return null. Also, initialize the strategy ID
	 * and pointer to the drive strategy. the ID is simply the index
	 * of the strategy in the strategy array. it is placed in the
	 * media_strategy_t as well as the write headers.
	 */
	chosen_sp = 0;
	for ( id = 0 ; spp < epp ; spp++, id++ ) {
		(*spp)->ms_id = id;
		if ( ! chosen_sp ) {
			/* lend the media_t array to the strategy
			 */
			(*spp)->ms_mediap = mediapp;
			(*spp)->ms_dsp = dsp;
			(*spp)->ms_mediacnt = mediacnt;
			for ( mediaix = 0 ; mediaix < mediacnt ; mediaix++ ) {
				media_t *mediap = mediapp[ mediaix ];
				mediap->m_strategyp = *spp;
				mediap->m_writehdrp->mh_strategyid = id;
			}
			if ( ( * (*spp)->ms_match )( argc, argv, dsp )) {
				chosen_sp = *spp;
			}
		}
	}
	if ( ! chosen_sp ) {
		mlog( MLOG_NORMAL,
#ifdef DUMP
		      _("no media strategy available for selected "
		      "dump destination(s)\n")
#endif /* DUMP */
#ifdef RESTORE
		      _("no media strategy available for selected "
		      "restore source(s)\n")
#endif /* RESTORE */
			);
		usage( );
		return 0;
	}

	/* give the media_t array to the chosen strategy
	 */
	for ( mediaix = 0 ; mediaix < mediacnt ; mediaix++ ) {
		media_t *mediap = mediapp[ mediaix ];
		mediap->m_strategyp = chosen_sp;
		mediap->m_writehdrp->mh_strategyid = chosen_sp->ms_id;
	}
	
	/* initialize the strategy. this will cause each of the managers
	 * to be initialized as well. if error, return 0.
	 */
	ok = ( * chosen_sp->ms_create )( chosen_sp, argc, argv );
	if ( ! ok ) {
		return 0;
	}

	/* return the selected strategy
	 */
	return chosen_sp;
}

bool_t
media_init( media_strategy_t *msp, int argc, char *argv[] )
{
	bool_t ok;

	ok = ( * msp->ms_init )( msp, argc, argv );

	return ok;
}

void
media_complete( media_strategy_t *msp )
{
	( * msp->ms_complete )( msp );
}

/* media_get_upper_hdrs - supply pointers to portion of media file headers
 * set aside for upper software layers, as well as to the global hdrs
 */
void
media_get_upper_hdrs( media_t *mediap,
		      global_hdr_t **grhdrpp,
		      char **rhdrpp,
		      size_t *rhdrszp,
		      global_hdr_t **gwhdrpp,
		      char **whdrpp,
		      size_t *whdrszp )
{
	*grhdrpp = mediap->m_greadhdrp;
	*rhdrpp = mediap->m_readhdrp->mh_upper;
	*rhdrszp = sizeof( mediap->m_readhdrp->mh_upper );

	*gwhdrpp = mediap->m_gwritehdrp;
	*whdrpp = mediap->m_writehdrp->mh_upper;
	*whdrszp = sizeof( mediap->m_writehdrp->mh_upper );
}


/* definition of locally defined static functions ****************************/

/* media_alloc - allocate and initialize the generic portions of a media 
 * descriptor and read and write media headers
 */
static media_t *
media_alloc( drive_t *drivep,
	     char *medialabel )
{
	media_t *mediap;
	global_hdr_t *grhdrp;
	global_hdr_t *gwhdrp;
	media_hdr_t *mrhdrp;
	media_hdr_t *mwhdrp;
	size_t mrhdrsz;
	size_t mwhdrsz;

	mediap = ( media_t * )calloc( 1, sizeof( media_t ));
	assert( mediap );

	grhdrp = 0;
	gwhdrp = 0;
	mrhdrp = 0;
	mwhdrp = 0;
	drive_get_upper_hdrs( drivep,
			      &grhdrp,
			      ( char ** )&mrhdrp,
			      &mrhdrsz,
			      &gwhdrp,
			      ( char ** )&mwhdrp,
			      &mwhdrsz );
	assert( grhdrp );
	assert( gwhdrp );
	assert( mrhdrp );
	assert( mwhdrp );
	assert( mrhdrsz == MEDIA_HDR_SZ );
	assert( mwhdrsz == MEDIA_HDR_SZ );

	mediap->m_greadhdrp = grhdrp;
	mediap->m_gwritehdrp = gwhdrp;
	mediap->m_readhdrp = mrhdrp;
	mediap->m_writehdrp = mwhdrp;
	mediap->m_drivep = drivep;

	strncpyterm( mwhdrp->mh_medialabel,
		     medialabel,
		     sizeof( mwhdrp->mh_medialabel ));

#ifdef DUMP
	uuid_create( mwhdrp->mh_mediaid );
#else /* DUMP */
	uuid_clear( mwhdrp->mh_mediaid );
#endif /* DUMP */

	return mediap;
}
