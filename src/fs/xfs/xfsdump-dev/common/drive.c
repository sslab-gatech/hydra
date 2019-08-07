/*
 * Copyright (c) 2000-2002 Silicon Graphics, Inc.
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

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>

#include "config.h"

#include "types.h"
#include "mlog.h"
#include "dlog.h"
#include "path.h"
#include "getopt.h"
#include "global.h"
#include "drive.h"

/* drive.c - selects and initializes a drive strategy
 */


/* structure definitions used locally ****************************************/


/* declarations of externally defined global symbols *************************/

extern void usage( void );
extern char *homedir;

/* declare all drive strategies here
 */
extern drive_strategy_t drive_strategy_simple;
extern drive_strategy_t drive_strategy_scsitape;
extern drive_strategy_t drive_strategy_rmt;


/* forward declarations of locally defined static functions ******************/

static drive_t *drive_alloc( char *, size_t );
static void drive_allochdrs( drive_t *drivep,
			     global_hdr_t *gwhdrtemplatep,
			     ix_t driveix );


/* definition of locally defined global variables ****************************/

drive_t **drivepp;
size_t drivecnt; 
size_t partialmax;

/* definition of locally defined static variables *****************************/

/* drive strategy array - ordered by precedence
 */
static drive_strategy_t *strategypp[] = {
	&drive_strategy_simple,
	&drive_strategy_scsitape,
	&drive_strategy_rmt,
};


/* definition of locally defined global functions ****************************/

/* drive_init1 - select and instantiate a drive manager for each drive
 * specified on the command line.
 */
bool_t
drive_init1( int argc, char *argv[ ] )
{
	int c;
	ix_t driveix;

	/* sanity check asserts
	 */
	assert( sizeof( drive_hdr_t ) == DRIVE_HDR_SZ );

	/* count drive arguments
	 */
	optind = 1;
	opterr = 0;
	drivecnt = 0;
	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
		case GETOPT_DUMPDEST:
			drivecnt++;
			break;
		}
	}

	/* allocate an array to hold ptrs to drive descriptors
	 */
	if (drivecnt > 0) {
		drivepp = ( drive_t ** )calloc( drivecnt, sizeof( drive_t * ));
		assert( drivepp );
	}

	/* initialize the partialmax value.  Each drive can be completing a file
	 * started in another drive (except for drive 0) and leave one file to
	 * be completed by another drive.  This value is used to limit the 
	 * search in the list of partially completed files shared between all 
	 * restore streams.  Note, if drivecnt is one, then partialmax is zero
	 * to indicate no partial files can span streams.
	 */
	partialmax = (drivecnt <= 1 ? 0 : (drivecnt * 2) - 1);

	/* initialize drive descriptors from command line arguments
	 */
	optind = 1;
	opterr = 0;
	driveix = 0;
	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
		case GETOPT_DUMPDEST:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL,
				      _("-%c argument missing\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}

			/* allocate a drive descriptor
			 */
			drivepp[ driveix ] = drive_alloc( optarg, driveix );
			driveix++;
			break;
		}
	}
	assert( driveix == drivecnt );

	/* the user may specify stdin as the source, by
	 * a single dash ('-') with no option letter. This must appear
	 * between all lettered arguments and the file system pathname.
	 */
	if ( optind < argc && ! strcmp( argv[ optind ], "-" )) {
		if ( driveix > 0 ) {
			mlog( MLOG_NORMAL,
#ifdef DUMP
			_("cannot specify source files and stdout together\n")
#endif /* DUMP */
#ifdef RESTORE
			_("cannot specify source files and stdin together\n")
#endif /* RESTORE */
			      );
			usage( );
			return BOOL_FALSE;
		}

		drivecnt = 1;

		/* Adding this alloc to fix malloc corruption. 
		 * Bug #393618 - prasadb 04/16/97
		 * allocate an array to hold ptrs to drive descriptors
		 */
		drivepp = ( drive_t ** )calloc( drivecnt, sizeof( drive_t * ));
		assert( drivepp );

		drivepp[ 0 ] = drive_alloc( "stdio", 0 );

#ifdef DUMP   /* ifdef added around dlog_desist() by prasadb to fix 435626 */
		dlog_desist( ); 
#endif
	}

	/* verify that some dump destination(s) / restore source(s) specified
	 */
	if ( drivecnt == 0 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR,
#ifdef DUMP
			_("no destination file(s) specified\n")
#endif /* DUMP */
#ifdef RESTORE
			_("no source file(s) specified\n")
#endif /* RESTORE */
			);
		usage( );
		return BOOL_FALSE;
	}

	/* run each drive past each strategy, pick the best match
	 * and instantiate a drive manager.
	 */
	for ( driveix = 0 ; driveix < drivecnt ; driveix++ ) {
		drive_t *drivep = drivepp[ driveix ];
		int bestscore = 0 - INTGENMAX;
		ix_t six;
		ix_t scnt = sizeof( strategypp ) / sizeof( strategypp[ 0 ] );
		drive_strategy_t *bestsp = 0;
		bool_t ok;

		for ( six = 0 ; six < scnt ; six++ ) {
			drive_strategy_t *sp = strategypp[ six ];
			int score;
			score = ( * sp->ds_match )( argc,
						    argv,
						    drivep );
			if ( ! bestsp || score > bestscore ) {
				bestsp = sp;
				bestscore = score;
			}
		}
		assert( bestsp );
		drivep->d_strategyp = bestsp;
		drivep->d_recmarksep = bestsp->ds_recmarksep;
		drivep->d_recmfilesz = bestsp->ds_recmfilesz;
		mlog( MLOG_VERBOSE,
		      _("using %s strategy\n"),
		      bestsp->ds_description );
		ok = ( * bestsp->ds_instantiate )( argc,
						   argv,
						   drivep );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
	}

	return BOOL_TRUE;
}


/* drive_init2 - second phase strategy initialization.
 * allocates global read and write hdrs, copying global hdr template
 * into the write hdrs (DUMP only). kicks off async init for each drive,
 * which will be synchronized with drive_init3.
 */
/* ARGSUSED */
bool_t
drive_init2( int argc,
	     char *argv[ ],
	     global_hdr_t *gwhdrtemplatep )
{
	ix_t driveix;

	for ( driveix = 0 ; driveix < drivecnt ; driveix++ ) {
		drive_t *drivep = drivepp[ driveix ];
		bool_t ok;

		drive_allochdrs( drivep, gwhdrtemplatep, driveix );
		ok = ( * drivep->d_opsp->do_init )( drivep );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
	}

	return BOOL_TRUE;
}


/* drive_init3 - third phase strategy initialization.
 * synchronizes with async operations begun by drive_init2.
 */
bool_t
drive_init3( void )
{
	ix_t driveix;

	for ( driveix = 0 ; driveix < drivecnt ; driveix++ ) {
		drive_t *drivep = drivepp[ driveix ];
		bool_t ok;

		ok = ( * drivep->d_opsp->do_sync )( drivep );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
	}

	return BOOL_TRUE;
}


/* drive_mark_commit - commits and unlinks all accumulated marks with
 * offsets less than or equal to the offset of the next (as yet unwritten)
 * byte in the media file.
 * utility function for use by drive-specific strategies.
 */
void
drive_mark_commit( drive_t *drivep, off64_t ncommitted )
{
	drive_markrec_t *dmp;

	for ( dmp = drivep->d_markrecheadp
	;
	dmp && dmp->dm_log <= ( drive_mark_t )ncommitted
	;
	) {
		drivep->d_markrecheadp = dmp->dm_nextp;
		( * dmp->dm_cbfuncp )( dmp->dm_cbcontextp, dmp, BOOL_TRUE );
		dmp = drivep->d_markrecheadp;
	}
}

/* drive_mark_discard - unlinks all accumulated marks, calling their callbacks
 * indicating the mark was NOT committed.
 * utility function for use by drive-specific strategies.
 */
void
drive_mark_discard( drive_t *drivep )
{
	drive_markrec_t *dmp;

	for ( dmp = drivep->d_markrecheadp
	;
	dmp
	;
	drivep->d_markrecheadp = dmp->dm_nextp, dmp = dmp->dm_nextp ) {

		( * dmp->dm_cbfuncp )( dmp->dm_cbcontextp, dmp, BOOL_FALSE );
	}
}

/* drive_display_metrics - called by main thread during interactive dialog
 * to print drive throughput and streaming metrics.
 */
void
drive_display_metrics( void )
{
	ix_t driveix;

	for ( driveix = 0 ; driveix < drivecnt ; driveix++ ) {
		drive_t *drivep = drivepp[ driveix ];
		drive_ops_t *dop = drivep->d_opsp;
		if ( dop->do_display_metrics ) {
			( * dop->do_display_metrics )( drivep );
		}
	}
}


/* definition of locally defined static functions ****************************/

/* drive_alloc - allocate and initialize the generic portions of a drive 
 * descriptor. do NOT allocate hdr buffers.
 */
static drive_t *
drive_alloc( char *pathname, ix_t driveix )
{
	drive_t *drivep;
	struct stat64 statbuf;

	/* allocate the descriptor
	 */
	drivep = ( drive_t * )calloc( 1, sizeof( drive_t ));
	assert( drivep );

	/* convert the pathname to an absolute pathname
	 * NOTE: string "stdio" is reserved to mean send to standard out
	 */
	if ( strcmp( pathname, "stdio" )) {
		pathname = path_reltoabs( pathname, homedir );
	}

	/* set pipe flags
	 */
	if ( ! strcmp( pathname, "stdio" )) {
		drivep->d_isunnamedpipepr = BOOL_TRUE;
	} else if ( ! stat64( pathname, &statbuf )
		    &&
		    ( statbuf.st_mode & S_IFMT ) == S_IFIFO ) {
		drivep->d_isnamedpipepr = BOOL_TRUE;
	}

	/* complete the drive manager
	 */
	drivep->d_pathname = pathname;
	drivep->d_index = driveix;

	return drivep;
}

/* drive_allochdrs - allocate and initialize the drive read and write
 * hdrs, and ptrs into the hdrs.
 */
static void
drive_allochdrs( drive_t *drivep, global_hdr_t *gwhdrtemplatep, ix_t driveix )
{
	global_hdr_t *grhdrp;
	drive_hdr_t *drhdrp;
	global_hdr_t *gwhdrp;
	drive_hdr_t *dwhdrp;

	/* allocate the read header
	 */
	grhdrp = ( global_hdr_t * )calloc( 1, sizeof( global_hdr_t ));
	assert( grhdrp );
	gwhdrp = NULL;
	dwhdrp = NULL;

	/* calculate pointer to the drive portion of the read header
	 */
	drhdrp = ( drive_hdr_t * )grhdrp->gh_upper;

	/* global write hdr used only for dumps. will be NULL for restore
	 */
	if ( gwhdrtemplatep ) {
		/* allocate the write header
		 */
		gwhdrp = ( global_hdr_t * )calloc( 1, sizeof( global_hdr_t ));
		assert( gwhdrp );

		/* copy the template
		 */
		*gwhdrp = *gwhdrtemplatep;

		/* calculate pointer to the drive portion of the read header
		 */
		dwhdrp = ( drive_hdr_t * )gwhdrp->gh_upper;

		/* fill in generic drive fields of write hdr
		 */
		dwhdrp->dh_strategyid = drivep->d_strategyp->ds_id;
		dwhdrp->dh_driveix = driveix;
		dwhdrp->dh_drivecnt = drivecnt;
	}

	/* complete the drive manager
	 */
	drivep->d_greadhdrp = grhdrp;
	drivep->d_readhdrp = drhdrp;
	drivep->d_gwritehdrp = gwhdrp;
	drivep->d_writehdrp = dwhdrp;
}
