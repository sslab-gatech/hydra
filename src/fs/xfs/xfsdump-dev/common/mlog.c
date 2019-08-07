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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>

#include "config.h"

#include "types.h"
#include "qlock.h"
#include "stream.h"
#include "mlog.h"
#include "cldmgr.h"
#include "getopt.h"
#include "exit.h"
#include "global.h"
#include "drive.h"

extern char *progname;
extern void usage( void );
extern pthread_t parenttid;

#ifdef DUMP
static FILE *mlog_fp = NULL; /* stderr */;
#endif /* DUMP */
#ifdef RESTORE
static FILE *mlog_fp = NULL; /* stdout */;
#endif /* RESTORE */

int mlog_level_ss[ MLOG_SS_CNT ];

int mlog_showlevel = BOOL_FALSE;

int mlog_showss = BOOL_FALSE;

int mlog_timestamp = BOOL_FALSE;

static int mlog_sym_lookup( char * );

static size_t mlog_streamcnt;

static char mlog_levelstr[ 3 ]; 

#define MLOG_SS_NAME_MAX	15
#ifdef DUMP
#define PROGSTR "dump"
#define PROGSTR_CAPS "Dump"
#else
#define PROGSTR "restore"
#define PROGSTR_CAPS "Restore"
#endif /* DUMP */
#define N(a) (sizeof((a)) / sizeof((a)[0]))

static char mlog_ssstr[ MLOG_SS_NAME_MAX + 2 ];

static char mlog_tsstr[ 10 ];

struct mlog_sym {
	char *sym;
	int level;
};

typedef struct mlog_sym mlog_sym_t;

char *mlog_ss_names[ MLOG_SS_CNT ] = {
	"general",	/* MLOG_SS_GEN */
	"proc",		/* MLOG_SS_PROC */
	"drive",	/* MLOG_SS_DRIVE */
	"media",	/* MLOG_SS_MEDIA */
	"inventory",	/* MLOG_SS_INV */
#ifdef DUMP
	"inomap",	/* MLOG_SS_INOMAP */
#endif /* DUMP */
#ifdef RESTORE
	"tree",		/* MLOG_SS_TREE */
#endif /* RESTORE */
	"excluded_files" /* MLOG_SS_EXCLFILES */
};

static mlog_sym_t mlog_sym[ ] = {
	{"0",		MLOG_SILENT},
	{"1",		MLOG_VERBOSE},
	{"2",		MLOG_TRACE},
	{"3",		MLOG_DEBUG},
	{"4",		MLOG_NITTY},
	{"5",		MLOG_NITTY + 1},
	{"silent",	MLOG_SILENT},
	{"verbose",	MLOG_VERBOSE},
	{"trace",	MLOG_TRACE},
	{"debug",	MLOG_DEBUG},
	{"nitty",	MLOG_NITTY}
};

static qlockh_t mlog_qlockh;
static int mlog_main_exit_code = -1;
static rv_t mlog_main_exit_return = RV_NONE;
static rv_t mlog_main_exit_hint = RV_NONE;

void
mlog_init0( void )
{
	int i;

#ifdef DUMP
	mlog_fp = stderr;
#endif /* DUMP */
#ifdef RESTORE
	mlog_fp = stdout;
#endif /* RESTORE */

	/* initialize stream count. will be updated later by call to
	 * mlog_tell_streamcnt( ), after drive layer has counted drives
	 */
	mlog_streamcnt = 1;

	for( i = 0 ; i < MLOG_SS_CNT ; i++ ) {
		mlog_level_ss[ i ] = MLOG_VERBOSE;
	}
}

bool_t
mlog_init1( int argc, char *argv[ ] )
{
	char **suboptstrs;
	ix_t ssix;
	ix_t soix;
	size_t vsymcnt;
	int c;

	/* prepare an array of suboption token strings. this will be the
	 * concatenation of the subsystem names with the verbosity symbols.
	 * this array of char pts must be null terminated for getsubopt( 3 ).
	 */
	vsymcnt = sizeof( mlog_sym ) / sizeof( mlog_sym[ 0 ] );
	suboptstrs = ( char ** )calloc( MLOG_SS_CNT + vsymcnt + 1,
					sizeof( char * ));
	assert( suboptstrs );
	for ( soix = 0 ; soix < MLOG_SS_CNT ; soix++ ) {
		assert( strlen( mlog_ss_names[ soix ] ) <= MLOG_SS_NAME_MAX );
			/* unrelated, but opportunity to chk */
		suboptstrs[ soix ] = mlog_ss_names[ soix ];
	}
	for ( ; soix < MLOG_SS_CNT + vsymcnt ; soix++ ) {
		suboptstrs[ soix ] = mlog_sym[ soix - MLOG_SS_CNT ].sym;
	}
	suboptstrs[ soix ] = 0;

	/* set all of the subsystem log levels to -1, so we can see which
	 * subsystems where explicitly called out. those which weren't will
	 * be given the "general" level.
	 */
	for ( ssix = 0 ; ssix < MLOG_SS_CNT ; ssix++ ) {
		mlog_level_ss[ ssix ] = -1;
	}
	mlog_level_ss[ MLOG_SS_GEN ] = MLOG_VERBOSE;

	/* get command line options
	 */
	optind = 1;
	opterr = 0;
	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		char *options;

		switch ( c ) {
		case GETOPT_VERBOSITY:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				fprintf( stderr,
					 _("%s: -%c argument missing\n"),
					 progname,
					 c );
				usage( );
				return BOOL_FALSE;
			}
			options = optarg;
			while ( *options ) {
				int suboptix;
				char *valstr;

				suboptix = getsubopt( &options, 
						      (constpp)suboptstrs,
						      &valstr );
				if ( suboptix < 0 ) {
					fprintf( stderr,
						 _("%s: -%c argument invalid\n"),
						 progname,
						 c );
					usage( );
					return BOOL_FALSE;
				}
				assert( ( ix_t )suboptix
					<
					MLOG_SS_CNT + vsymcnt );
				if ( suboptix < MLOG_SS_CNT ) {
					if ( ! valstr ) {
						fprintf( stderr,
							 _("%s: -%c subsystem "
							 "subargument "
							 "%s requires a "
							 "verbosity value\n"),
							 progname,
							 c,
						mlog_ss_names[ suboptix ] );
						usage( );
						return BOOL_FALSE;
					}
					ssix = ( ix_t )suboptix;
					mlog_level_ss[ ssix ] =
						    mlog_sym_lookup( valstr );
				} else {
					if ( valstr ) {
						fprintf( stderr,
							 _("%s: -%c argument "
							 "does not require "
							 "a value\n"),
							 progname,
							 c );
						usage( );
						return BOOL_FALSE;
					}
					ssix = MLOG_SS_GEN;
					mlog_level_ss[ ssix ] =
				    mlog_sym_lookup( suboptstrs[ suboptix ] );
				}
				if ( mlog_level_ss[ ssix ] < 0 ) {
					fprintf( stderr,
						 _("%s: -%c argument "
						 "invalid\n"),
						 progname,
						 c );
					usage( );
					return BOOL_FALSE;
				}
			}
			break;
		case GETOPT_SHOWLOGLEVEL:
			mlog_showlevel = BOOL_TRUE;
			break;
		case GETOPT_SHOWLOGSS:
			mlog_showss = BOOL_TRUE;
			break;
		case GETOPT_TIMESTAMP:
			mlog_timestamp = BOOL_TRUE;
			break;
		}
	}

	free( ( void * )suboptstrs );

	/* give subsystems not explicitly called out the "general" verbosity
	 */
	for ( ssix = 0 ; ssix < MLOG_SS_CNT ; ssix++ ) {
		if ( mlog_level_ss[ ssix ] < 0 ) {
			assert( mlog_level_ss[ ssix ] == -1 );
			assert( mlog_level_ss[ MLOG_SS_GEN ] >= 0 );
			mlog_level_ss[ ssix ] = mlog_level_ss[ MLOG_SS_GEN ];
		}
	}

	/* prepare a string for optionally displaying the log level
	 */
	mlog_levelstr[ 0 ] = 0;
	mlog_levelstr[ 1 ] = 0;
	mlog_levelstr[ 2 ] = 0;
	if ( mlog_showlevel ) {
		mlog_levelstr[ 0 ] = ':';
	}

#ifdef DUMP
	/* note if dump going to stdout. if so, can't
	 * send mlog output there. since at compile time
	 * mlog_fd set to stderr, see if we can switch
	 * to stdout.
	 */
	if ( optind >= argc ||  strcmp( argv[ optind ], "-" )) {
		mlog_fp = stdout;
	}
#endif /* DUMP */

	mlog_qlockh = QLOCKH_NULL;

	return BOOL_TRUE;
}

bool_t
mlog_init2( void )
{
	/* allocate a qlock
	 */
	mlog_qlockh = qlock_alloc( QLOCK_ORD_MLOG );

	return BOOL_TRUE;
}

void
mlog_tell_streamcnt( size_t streamcnt )
{
	mlog_streamcnt = streamcnt;
}

void
mlog_lock( void )
{
	qlock_lock( mlog_qlockh );
}

void
mlog_unlock( void )
{
	qlock_unlock( mlog_qlockh );
}

/*
 * Override the -v option.
 * Useful for debugging at particular points
 * where doing it program-wide would produce
 * too much output.
 */
void
mlog_override_level( int levelarg )
{
	int level;
	ix_t ss; /* SubSystem */

	level = levelarg & MLOG_LEVELMASK;
	ss = ( ix_t )( ( levelarg & MLOG_SS_MASK ) >> MLOG_SS_SHIFT );

	if (ss == MLOG_SS_GEN) { /* do level for all subsys */  
	    for (ss = 0 ; ss < MLOG_SS_CNT ; ss++ ) {
		mlog_level_ss[ ss ] = level;
	    }
	}
	else { /* do a particular subsys */
	    mlog_level_ss[ ss ] = level;
	}
}

void
mlog( int levelarg, char *fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	mlog_va( levelarg, fmt, args );
	va_end( args );
}

void
mlog_va( int levelarg, char *fmt, va_list args )
{
	int level;
	ix_t ss;

	level = levelarg & MLOG_LEVELMASK;
	ss = ( ix_t )( ( levelarg & MLOG_SS_MASK ) >> MLOG_SS_SHIFT );

	assert( ss < MLOG_SS_CNT );
	if ( level > mlog_level_ss[ ss ] ) {
		return;
	}
	
	if ( ! ( levelarg & MLOG_NOLOCK )) {
		mlog_lock( );
	}

	if ( ! ( levelarg & MLOG_BARE )) {
		int streamix;
		streamix = stream_getix( pthread_self( ) );

		if ( mlog_showss ) {
			sprintf( mlog_ssstr, ":%s", mlog_ss_names[ ss ] );
		} else {
			mlog_ssstr[ 0 ] = 0;
		}

		if ( mlog_timestamp ) {
			time_t now = time( 0 );
			struct tm *tmp = localtime( &now );
			sprintf( mlog_tsstr,
				 ":%02d.%02d.%02d",
				 tmp->tm_hour,
				 tmp->tm_min,
				 tmp->tm_sec );
			assert( strlen( mlog_tsstr ) < sizeof( mlog_tsstr ));
		} else {
			mlog_tsstr[ 0 ] = 0;
		}

		if ( mlog_showlevel ) {
			mlog_levelstr[ 0 ] = ':';
			if ( level > 9 ) {
				mlog_levelstr[ 1 ] = '?';
			} else {
				mlog_levelstr[ 1 ] = ( char )
						     ( level
						       +
						       ( int )'0' );
			}
		} else {
			mlog_levelstr[ 0 ] = 0;
		}
		if ( streamix != -1 && mlog_streamcnt > 1 ) {
			fprintf( mlog_fp,
				 _("%s%s%s%s: drive %d: "),
				 progname,
				 mlog_tsstr,
				 mlog_ssstr,
				 mlog_levelstr,
				 streamix );
		} else {
			fprintf( mlog_fp,
				 "%s%s%s%s: ",
				 progname,
				 mlog_tsstr,
				 mlog_ssstr,
				 mlog_levelstr );
		}
		if ( levelarg & MLOG_NOTE ) {
			fprintf( mlog_fp,
				 "NOTE: " );
		}
		if ( levelarg & MLOG_WARNING ) {
			fprintf( mlog_fp,
				 "WARNING: " );
		}
		if ( levelarg & MLOG_ERROR ) {
			fprintf( mlog_fp,
				 "ERROR: " );
		}
	}

	vfprintf( mlog_fp, fmt, args );
	fflush( mlog_fp );

	if ( ! ( levelarg & MLOG_NOLOCK )) {
		mlog_unlock( );
	}
}

/*
 * Map RV codes to actual error messages.
 */

struct rv_map {
	int		rv;
	const char *	rv_string;
	const char *	rv_desc;
};

static struct rv_map
rvs[_RV_NUM] = {
       /* Return Code	Displayed Code	Explanation */
	{ RV_OK,	"OK",		"success" }, 
	{ RV_NOTOK,	"ERASE_FAILED",	"media erase request denied" },
	{ RV_NOMORE,	"NOMORE",	"no more work to do" },
	{ RV_EOD,	"EOD",		"ran out of data" },
	{ RV_EOF,	"EOF",		"hit end of media file" },
 	{ RV_EOM,	"EOM",		"hit end of media" },
	{ RV_ERROR,	"ERROR",	"operator error or resource exhaustion" },
	{ RV_DONE,	"ALREADY_DONE",	"another stream completed the operation" },
	{ RV_INTR,	"INTERRUPT",	PROGSTR " interrupted" },
	{ RV_CORRUPT,	"CORRUPTION",	"corrupt data encountered" },
	{ RV_QUIT,	"QUIT",		"media is no longer usable" },
	{ RV_DRIVE,	"DRIVE_ERROR",	"drive error" },
	{ RV_TIMEOUT,	"TIMEOUT",	"operation timed out" },
	{ RV_MEDIA,	"NO_MEDIA",	"no media in drive" },
	{ RV_PROTECTED,	"WRITE_PROTECTED","object write protected" },
	{ RV_CORE,	"CORE",		"fatal error - core dumped" },
	{ RV_OPT,	"OPT_ERROR",	"bad command line option" },
	{ RV_INIT,	"INIT_ERROR",	"could not initialise subsystem" },
	{ RV_PERM,	"NO_PERMISSION","insufficient privilege" },
	{ RV_COMPAT,	"INCOMPATIBLE",	"cannot apply - dump incompatible" },
	{ RV_INCOMPLETE,"INCOMPLETE",	"the " PROGSTR " is incomplete" },
	{ RV_KBD_INTR,	"KEYBOARD_INTERRUPT", "keyboard interrupt" },
	{ RV_INV,	"INVENTORY",	"error updating session inventory" },
	{ RV_USAGE,	"USAGE_ONLY",	"printing usage only" },
	{ RV_EXISTS,	"EXISTS",	"file or directory already exists" },
	{ RV_NONE,	"NONE",		"no error code" },
	{ RV_UNKNOWN,	"UNKNOWN",	"unknown error" },
};

static struct rv_map 
rv_unknown = {
	  _RV_NUM,	"???",		"unknown error code"
};

static const struct rv_map *
rv_getdesc(rv_t rv)
{
	int rvidx;

	if (rv < 0 || rv >= _RV_NUM) {
		return &rv_unknown;
	}

	for (rvidx = 0; rvidx < _RV_NUM; rvidx++)
		if (rv == rvs[rvidx].rv)
			return &rvs[rvidx];

	return &rv_unknown;
}


/*
 * mlog_exit takes two arguments an exit code (EXIT_*) and the internal
 * return code (RV_*) that was signalled prior to the exit. mlog_exit
 * stores these values in a per-stream structure managed by the stream_*
 * functions.
 *
 * mlog_exit is called for: all returns from the content threads
 * (content_stream_dump and content_stream_restore); for returns from
 * the main process; and also from a couple of other locations where an
 * error is known to directly lead to the termination of the program.
 *
 * For example, in the places mentioned above "return EXIT_ERROR;" has
 * now been replaced with a call like
 * "return mlog_exit(EXIT_ERROR, RV_DRIVE);" that specifies both the exit
 * code, and the reason why the program is terminating.
 *
 * mlog_exit_flush uses the per-stream exit information recorded using
 * mlog_exit to print a detailed status report, showing both the exit
 * status of each stream, and the overall exit status of the
 * program. This additional log information allows the user to detect
 * failures that cannot be distinguished by looking at the exit code
 * alone.  In particular, the exit code does not currently allow the
 * user to distinguish EOM conditions from user interruptions, and to
 * detect an incomplete dump (caused, for example, by drive errors or
 * corruption).
 *
 * Note, that to maintain backwards compatibility the exit codes
 * returned by dump/restore have _not_ been changed. For more
 * information see PV #784355.
 *
 * While mlog_exit provides considerably more information about the
 * reasons for a dump terminating, there are a number of cases where
 * dump maps return codes that have specific values such as RV_DRIVE,
 * to return codes with less specific values such as RV_INTR, and in
 * doing so throws away information that would have been useful in
 * diagnosing the reasons for a failure. To alleviate this, an
 * additional function mlog_exit_hint is provided that allows a "hint"
 * to be made about the real reason a stream is terminating. A call to
 * mlog_exit_hint should be made anywhere in the code a change in
 * program state has occured that might lead to the termination of the
 * dump. The mlog_exit_flush routine uses this additional information
 * help work out what really happened.
 */

int
_mlog_exit( const char *file, int line, int exit_code, rv_t rv )
{
	pthread_t tid;
	const struct rv_map *rvp;

	tid = pthread_self();
	rvp = rv_getdesc(rv);


	mlog( MLOG_DEBUG | MLOG_NOLOCK,
	      "%s: %d: mlog_exit called: "
	      "exit_code: %s return: %s (%s)\n",
	      file, line,
	      exit_codestring(exit_code),
	      rvp->rv_string, rvp->rv_desc);

	if (rv < 0 || rv >= _RV_NUM) {
		mlog( MLOG_DEBUG | MLOG_ERROR | MLOG_NOLOCK,
		      "mlog_exit(): bad return code");
		return exit_code;
	}

	/*
	 * NOTE: we record the first call only. Exit codes can be mapped from
	 * more specific values to less specific values as we return up the
	 * call chain. We assume therefore that the first call contains the
	 * most accurate information about the termination condition.
	 */

	if ( pthread_equal( tid, parenttid ) ) {
		if (mlog_main_exit_code == -1) {
			mlog_main_exit_code = exit_code;
			mlog_main_exit_return = rv;
		}
	}
	else {
		stream_state_t states[] = { S_RUNNING };
		stream_state_t state;
		int streamix;
		int exit_code;
		rv_t exit_return, exit_hint;

		if (stream_get_exit_status(tid,
					   states,
					   N(states),
					   &state,
					   &streamix,
					   &exit_code,
					   &exit_return,
					   &exit_hint))
		{
			if (exit_code == -1) {
				stream_set_code(tid, exit_code);
				stream_set_return(tid, rv);
			}
		}
	}

	return exit_code;
}

void
_mlog_exit_hint( const char *file, int line, rv_t rv )
{
	pthread_t tid;
	const struct rv_map *rvp;

	tid = pthread_self();
	rvp = rv_getdesc(rv);
	
	mlog( MLOG_DEBUG | MLOG_NOLOCK,
	      "%s: %d: mlog_exit_hint called: "
	      "hint: %s (%s)\n",
	      file, line,
	      rvp->rv_string, rvp->rv_desc);

	if (rv < 0 || rv >= _RV_NUM) {
		mlog( MLOG_DEBUG | MLOG_ERROR | MLOG_NOLOCK,
		      "mlog_exit_hint(): bad return code");
		return;
	}

	/*
	 * NOTE: we use the last hint before exit. Unlike exit codes we've added
	 * calls to mlog_exit_hint to improve error reporting. In general the
	 * call closest to the final exit point will provide the most accurate
	 * information about the termination condition.
	 */

	if ( pthread_equal( tid, parenttid ) )
		mlog_main_exit_hint = rv;
	else 
		stream_set_hint( tid, rv );

}

rv_t
mlog_get_hint( void )
{
	stream_state_t states[] = { S_RUNNING };
	/* REFERENCED */
	bool_t ok;
	rv_t hint;

	if ( pthread_equal( pthread_self(), parenttid ) )
		return mlog_main_exit_hint;

	ok = stream_get_exit_status(pthread_self(), states, N(states),
				    NULL, NULL, NULL, NULL, &hint);
	assert(ok);
	return hint;
}


#define IS_INCOMPLETE(rv)			\
	((rv) == RV_CORRUPT ||			\
	 (rv) == RV_INCOMPLETE ||		\
	 (rv) == RV_EOD ||			\
	 (rv) == RV_EOF ||			\
	 (rv) == RV_EOM)

#define VALID_EXIT_CODE(code)			\
	((code) == EXIT_NORMAL ||		\
	 (code) == EXIT_ERROR ||		\
	 (code) == EXIT_INTERRUPT ||		\
	 (code) == EXIT_FAULT)


void
mlog_exit_flush(void)
{
	pthread_t tids[STREAM_SIMMAX];
	int i, ntids;
	const struct rv_map *rvp;
	stream_state_t states[] = { S_RUNNING, S_ZOMBIE };
	bool_t incomplete = BOOL_FALSE;
	bool_t quit = BOOL_FALSE;
	bool_t interrupt = BOOL_FALSE;
	const char *status_str;
	rv_t rv;

	if (mlog_level_ss[MLOG_SS_GEN] == MLOG_SILENT)
		return;

	if (mlog_main_exit_hint == RV_USAGE)
		return;

	ntids = stream_find_all(states, N(states), tids, STREAM_SIMMAX);
	if (ntids > 0) {

		/* print the state of all the streams */
		fprintf(mlog_fp, _("%s: %s Summary:\n"), progname, PROGSTR_CAPS );

		for (i = 0; i < ntids; i++) {
			stream_state_t state;
			int streamix;
			int exit_code;
			rv_t exit_return, exit_hint;
			/* REFERENCED */
			bool_t ok;

			ok = stream_get_exit_status(tids[i],
						    states,
						    N(states),
						    &state,
						    &streamix,
						    &exit_code,
						    &exit_return,
						    &exit_hint);
			assert(ok);

			/* hint takes priority over return */
			rv = (exit_hint != RV_NONE) ? exit_hint : exit_return;

			/* print status of this stream */
			rvp = rv_getdesc(rv);
			fprintf(mlog_fp,
				_("%s:   stream %d %s "
				"%s (%s)\n"),
				progname,
				streamix,
				drivepp[streamix]->d_pathname,
				rvp->rv_string,
				rvp->rv_desc);

			/* If the following conditions are true for any stream
			 * then they are true for the entire dump/restore.
			 */
			if (rv == RV_INTR) interrupt = BOOL_TRUE;
			else if (rv == RV_QUIT) quit = BOOL_TRUE;
			else if (IS_INCOMPLETE(rv)) incomplete = BOOL_TRUE;
		}
	}

	/* Also check return codes for the main process
	 */
	rv = (mlog_main_exit_hint != RV_NONE) ? mlog_main_exit_hint
	    : mlog_main_exit_return;

	if (rv == RV_INTR) interrupt = BOOL_TRUE;
	else if (rv == RV_QUIT) quit = BOOL_TRUE;
	else if (IS_INCOMPLETE(rv)) incomplete = BOOL_TRUE;

	/* if we don't have an exit code here there is a problem */
	assert(VALID_EXIT_CODE(mlog_main_exit_code));
	if (interrupt) status_str = "INTERRUPT";
	else if (quit) status_str = "QUIT";
	else if (incomplete) status_str = "INCOMPLETE";
	else status_str = exit_codestring(mlog_main_exit_code);

	/* now print the overall state of the dump/restore */
	fprintf(mlog_fp, "%s: %s Status: %s\n", progname, PROGSTR_CAPS, status_str);
	fflush(mlog_fp);
}

static int
mlog_sym_lookup( char *sym )
{
	mlog_sym_t *p = mlog_sym;
	mlog_sym_t *ep = mlog_sym
			 +
			 sizeof( mlog_sym ) / sizeof( mlog_sym[ 0 ] );

	for ( ; p < ep ; p++ ) {
		if ( ! strcmp( sym, p->sym )) {
			return p->level;
		}
	}

	return -1;
}

void
fold_init( fold_t fold, char *infostr, char c )
{
	size_t infolen;
	size_t dashlen;
	size_t predashlen;
	size_t postdashlen;
	char *p;
	char *endp;
	ix_t cnt;

	assert( sizeof( fold_t ) == FOLD_LEN + 1 );

	infolen = strlen( infostr );
	if ( infolen > FOLD_LEN - 4 ) {
		infolen = FOLD_LEN - 4;
	}
	dashlen = FOLD_LEN - infolen - 3;
	predashlen = dashlen / 2;
	postdashlen = dashlen - predashlen;

	p = &fold[ 0 ];
	endp = &fold[ sizeof( fold_t ) - 1 ];

	assert( p < endp );
	*p++ = ' ';
	for ( cnt = 0 ; cnt < predashlen && p < endp ; cnt++, p++ ) {
		*p = c;
	}
	assert( p < endp );
	*p++ = ' ';
	assert( p < endp );
	assert( p + infolen < endp );
	strcpy( p, infostr );
	p += infolen;
	assert( p < endp );
	*p++ = ' ';
	assert( p < endp );
	for ( cnt = 0 ; cnt < postdashlen && p < endp ; cnt++, p++ ) {
		*p = c;
	}
	assert( p <= endp );
	*p = 0;
}
