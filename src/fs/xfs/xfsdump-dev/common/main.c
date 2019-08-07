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
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#include <getopt.h>
#include <stdint.h>
#include <sched.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>
#include <locale.h>

#include "config.h"

#include "exit.h"
#include "types.h"
#include "stream.h"
#include "cldmgr.h"
#include "getopt.h"
#include "mlog.h"
#include "qlock.h"
#include "lock.h"
#include "dlog.h"
#include "global.h"
#include "drive.h"
#include "media.h"
#include "content.h"
#include "inventory.h"

#ifdef DUMP
/* main.c - main for dump
 */
#endif /* DUMP */
#ifdef RESTORE
/* main.c - main for restore
 */
#endif /* RESTORE */


/* structure definitions used locally ****************************************/

#ifdef RESTORE
#define VMSZ_PER	4	/* proportion of available vm to use in tree */
#endif /* RESTORE */
#define DLOG_TIMEOUT	60	/* time out operator dialog */
#define STOP_TIMEOUT	600	/* seconds after stop req. before abort */
#define ABORT_TIMEOUT	10	/* seconds after abort req. before abort */
#define MINSTACKSZ	0x02000000
#define MAXSTACKSZ	0x08000000


/* declarations of externally defined global symbols *************************/

extern void rmt_turnonmsgs(int);

/* forward declarations of locally defined global functions ******************/

void usage( void );
bool_t preemptchk( int );


/* forward declarations of locally defined static functions ******************/

static bool_t loadoptfile( int *argcp, char ***argvp );
static char * stripquotes( char *p );
static void shiftleftby1( char *p, char *endp );
static void sighandler( int );
static int childmain( void * );
static bool_t sigint_dialog( void );
static char *sigintstr( void );
#ifdef DUMP
static bool_t set_rlimits( void );
#endif /* DUMP */
#ifdef RESTORE
static bool_t set_rlimits( size64_t * );
#endif /* RESTORE */
static char *sig_numstring( int num );
static char *strpbrkquotes( char *p, const char *sep );


/* definition of locally defined global variables ****************************/

char *progname = 0;			/* used in all error output */
char *homedir = 0;			/* directory invoked from */
bool_t pipeline = BOOL_FALSE;
bool_t stdoutpiped = BOOL_FALSE;
pthread_t parenttid;
char *sistr;
size_t pgsz;
size_t pgmask;


/* definition of locally defined static variables *****************************/

static rlim64_t minstacksz;
static rlim64_t maxstacksz;
#ifdef RESTORE
static size64_t vmsz;
#endif /* RESTORE */
static time32_t stop_deadline;
static bool_t stop_in_progress;
static bool_t sighup_received;
static bool_t sigterm_received;
static bool_t sigquit_received;
static bool_t sigint_received;
/* REFERENCED */
static int sigstray_received;
static bool_t progrpt_enabledpr;
static time32_t progrpt_interval;
static time32_t progrpt_deadline;


/* definition of locally defined global functions ****************************/


int
main( int argc, char *argv[] )
{
	int c;
#ifdef DUMP
	uid_t euid;
#endif /* DUMP */
	ix_t stix; /* stream index */
	bool_t infoonly;
#ifdef DUMP
	global_hdr_t *gwhdrtemplatep;
#endif /* DUMP */
	bool_t init_error;
	bool_t coredump_requested = BOOL_FALSE;
	int exitcode;
	rlim64_t tmpstacksz;
	struct sigaction sa;
	int prbcld_xc = EXIT_NORMAL;
	int xc;
	bool_t ok;
	/* REFERENCED */
	int rval;
	int err;

	/* sanity checks
	 */
	assert( sizeof( char_t ) == 1 );
	assert( sizeof( u_char_t ) == 1 );
	assert( sizeof( int32_t ) == 4 );
	assert( sizeof( uint32_t ) == 4 );
	assert( sizeof( size32_t ) == 4 );
	assert( sizeof( int64_t ) == 8 );
	assert( sizeof( uint64_t ) == 8 );
	assert( sizeof( size64_t ) == 8 );

	/* record the command name used to invoke
	 */
	progname = argv[ 0 ];

	/* setup I18N support */
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	/* bootstrap message logging (stage 0)
	*/
	mlog_init0();

	/* Get the parent's pthread id. will be used
	 * to differentiate parent from children.
	 */
	parenttid = pthread_self( );
	rval = atexit(mlog_exit_flush);
	assert(rval == 0);

	/* pre-scan the command line for the option file option.
	 * if found, create a new argv.
	 */
	ok = loadoptfile( &argc, &argv );
	if ( ! ok ) {
		return mlog_exit(EXIT_ERROR, RV_OPT);
	}
	
	/* initialize message logging (stage 1)
	 */
	ok = mlog_init1( argc, argv );
	if ( ! ok ) {
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}
	/* scan the command line for the info, progress
	 * report options, and stacksz.
	 */
	minstacksz = MINSTACKSZ;
	maxstacksz = MAXSTACKSZ;
	infoonly = BOOL_FALSE;
	progrpt_enabledpr = BOOL_FALSE;
	optind = 1;
	opterr = 0;
	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
                case GETOPT_MINSTACKSZ:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_NOLOCK,
				      _("-%c argument missing\n"),
				      c );
				usage( );
				return mlog_exit(EXIT_ERROR, RV_OPT);
			}
			errno = 0;
			tmpstacksz = strtoull( optarg, 0, 0 );
			if ( tmpstacksz == UINT64_MAX
			     ||
			     errno == ERANGE ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_NOLOCK,
				      _("-%c argument (%s) invalid\n"),
				      c,
				      optarg );
				usage( );
				return mlog_exit(EXIT_ERROR, RV_OPT);
			}
			minstacksz = tmpstacksz;
			break;
                case GETOPT_MAXSTACKSZ:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_NOLOCK,
				      _("-%c argument missing\n"),
				      c );
				usage( );
				return mlog_exit(EXIT_ERROR, RV_OPT);
			}
			errno = 0;
			tmpstacksz = strtoull( optarg, 0, 0 );
			if ( tmpstacksz == UINT64_MAX
			     ||
			     errno == ERANGE ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_NOLOCK,
				      _("-%c argument (%s) invalid\n"),
				      c,
				      optarg );
				usage( );
				return mlog_exit(EXIT_ERROR, RV_OPT);
			}
			maxstacksz = tmpstacksz;
			break;
		case GETOPT_HELP:
			infoonly = BOOL_TRUE;
			mlog_exit_hint(RV_USAGE);
			break;
		case GETOPT_PROGRESS:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_NOLOCK,
				      _("-%c argument missing\n"),
				      c );
				usage( );
				return mlog_exit(EXIT_ERROR, RV_OPT);
			}
			progrpt_interval = ( time32_t )atoi( optarg );
			if ( progrpt_interval > 0 ) {
				progrpt_enabledpr = BOOL_TRUE;
			} else {
				progrpt_enabledpr = BOOL_FALSE;
			}
			break;
		}
	}

	/* sanity check resultant stack size limits
	 */
	if ( minstacksz > maxstacksz ) {
		mlog( MLOG_NORMAL
		      |
		      MLOG_ERROR
		      |
		      MLOG_NOLOCK
		      |
		      MLOG_PROC,
		      _("specified minimum stack size is larger than maximum: "
		      "min is 0x%llx,  max is 0x%llx\n"),
		      minstacksz,
		      maxstacksz );
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}

	if ( argc == 1 ) {
		infoonly = BOOL_TRUE;
	}

	/* set a progress report deadline to allow preemptchk() to
	 * report
	 */
	if ( progrpt_enabledpr ) {
		progrpt_deadline = time( 0 ) + progrpt_interval;
	}

	/* intitialize the stream manager
	 */
	stream_init( );

#ifdef DUMP
	/* set the memory limits to their appropriate values.
	 */
	ok = set_rlimits( );
#endif /* DUMP */
#ifdef RESTORE
	/* set the memory limits to their appropriate values. this is necessary
	 * to accomodate the tree abstraction and some recursive functions.
	 * also determines maximum vm, which will be budgeted among the
	 * various abstractions.
	 */
	ok = set_rlimits( &vmsz );
#endif /* RESTORE */
	if ( ! ok ) {
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}

	/* initialize message logging (stage 2) - allocate the message lock
	 */
	ok = mlog_init2( );
	if ( ! ok ) {
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}

	/* initialize the critical region lock
	 */
	lock_init( );
	rmt_turnonmsgs(1); /* turn on WARNING msgs for librmt */

	mlog( MLOG_NITTY + 1, "INTGENMAX == %ld (0x%lx)\n", INTGENMAX, INTGENMAX );
	mlog( MLOG_NITTY + 1, "UINTGENMAX == %lu (0x%lx)\n", UINTGENMAX, UINTGENMAX );
	mlog( MLOG_NITTY + 1, "OFF64MAX == %lld (0x%llx)\n", OFF64MAX, OFF64MAX );
	mlog( MLOG_NITTY + 1, "OFFMAX == %ld (0x%lx)\n", OFFMAX, OFFMAX );
	mlog( MLOG_NITTY + 1, "SIZEMAX == %lu (0x%lx)\n", SIZEMAX, SIZEMAX );
	mlog( MLOG_NITTY + 1, "INOMAX == %lu (0x%lx)\n", INOMAX, INOMAX );
	mlog( MLOG_NITTY + 1, "TIMEMAX == %ld (0x%lx)\n", TIMEMAX, TIMEMAX );
	mlog( MLOG_NITTY + 1, "SIZE64MAX == %llu (0x%llx)\n", SIZE64MAX, SIZE64MAX );
	mlog( MLOG_NITTY + 1, "INO64MAX == %llu (0x%llx)\n", INO64MAX, INO64MAX );
	mlog( MLOG_NITTY + 1, "UINT64MAX == %llu (0x%llx)\n", UINT64MAX, UINT64MAX );
	mlog( MLOG_NITTY + 1, "INT64MAX == %lld (0x%llx)\n", INT64MAX, INT64MAX );
	mlog( MLOG_NITTY + 1, "UINT32MAX == %u (0x%x)\n", UINT32MAX, UINT32MAX );
	mlog( MLOG_NITTY + 1, "INT32MAX == %d (0x%x)\n", INT32MAX, INT32MAX );
	mlog( MLOG_NITTY + 1, "INT16MAX == %d (0x%x)\n", INT16MAX, INT16MAX );
	mlog( MLOG_NITTY + 1, "UINT16MAX == %u (0x%x)\n", UINT16MAX, UINT16MAX );

	/* ask the system for the true vm page size, which must be used
	 * in all mmap calls
	 */
	pgsz = ( size_t )getpagesize( );
	mlog( MLOG_DEBUG | MLOG_PROC,
	      "getpagesize( ) returns %u\n",
	      pgsz );
	assert( ( int )pgsz > 0 );
	pgmask = pgsz - 1;

	/* report parent tid
         */
	mlog( MLOG_DEBUG | MLOG_PROC,
	      "parent tid is %lu\n",
	      parenttid );

	/* get the current working directory: this is where we will dump
	 * core, if necessary. some tmp files may be placed here as well.
	 */
	homedir = getcwd( 0, MAXPATHLEN );
	if ( ! homedir ) {
		mlog( MLOG_NORMAL | MLOG_ERROR,
		      _("unable to determine current directory: %s\n"),
		      strerror( errno ));
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}

	/* sanity check the inventory database directory, setup global paths
	 */
	ok = inv_setup_base( );
	if ( ! ok ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_NOLOCK,
		      _("both /var/lib/xfsdump and /var/xfsdump exist - fatal\n"));
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}

	/* if just looking for info, oblige
	 */
	if ( infoonly ) {
		mlog( MLOG_NORMAL,
		      _("version %s (dump format %d.0)\n"),
		      VERSION, GLOBAL_HDR_VERSION );
		usage( );
		return mlog_exit(EXIT_NORMAL, RV_OK); /* normal termination */
	}

	/* if an inventory display is requested, do it and exit
	 */
	if ( ! inv_DEBUG_print( argc, argv )) {
		return mlog_exit(EXIT_NORMAL, RV_OK); /* normal termination */
	}

#ifdef DUMP
	/* insist that the effective user id is root.
	 * this must appear after inv_DEBUG_print(),
	 * so it may be done without root privilege.
	 */
	euid = geteuid( );
	mlog( MLOG_DEBUG | MLOG_PROC,
	      "effective user id is %d\n",
	      euid );
	if ( euid != 0 ) {
		mlog( MLOG_NORMAL,
		      _("effective user ID must be root\n") );
		return mlog_exit(EXIT_ERROR, RV_PERM);
	}
#endif /* DUMP */

	/* initialize operator dialog capability
	 */
	ok = dlog_init( argc, argv );
	if ( ! ok ) {
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}

	/* initialize the child process manager
	 */
	ok = cldmgr_init( );
	if ( ! ok ) {
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}

	/* select and instantiate a drive manager for each stream. this
	 * is the first pass at initialization, so don't do anything
	 * terribly time-consuming here. A second initialization pass
	 * will be done shortly.
	 */
	ok = drive_init1( argc, argv );
	if ( ! ok ) {
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}

	/* check the drives to see if we're in a pipeline.
	 * if not, check stdout anyway, in case someone is trying to pipe
	 * the log messages into more, tee, ...
	 */
	if ( drivepp[ 0 ]->d_isunnamedpipepr ) {
		mlog( MLOG_DEBUG | MLOG_NOTE,
		      "pipeline detected\n" );
		pipeline = BOOL_TRUE;
	} else {
		struct stat64 statbuf;
		if ( fstat64( 1, &statbuf ) == 0
		     &&
		     ( statbuf.st_mode & S_IFMT ) == S_IFIFO ) {
			stdoutpiped = BOOL_TRUE;
		}
	}

	/* announce version and instructions
	 */
	sistr = sigintstr( );
	mlog( MLOG_VERBOSE,
	      _("version %s (dump format %d.0)"),
	      VERSION, GLOBAL_HDR_VERSION );
	if ( ! pipeline && ! stdoutpiped && sistr && dlog_allowed( )) {
		mlog( MLOG_VERBOSE | MLOG_BARE, _(
		      " - "
		      "type %s for status and control\n"),
		      sistr );
	} else {
		mlog( MLOG_VERBOSE | MLOG_BARE,
		      "\n" );
	}

#ifdef DUMP
	/* build a global write header template
	 */
	gwhdrtemplatep = global_hdr_alloc( argc, argv );
	if ( ! gwhdrtemplatep ) {
		return mlog_exit(EXIT_ERROR, RV_INIT);
	}
#endif /* DUMP */

	/* tell mlog how many streams there are. the format of log messages
	 * depends on whether there are one or many.
	 */
	mlog_tell_streamcnt( drivecnt );

	/* initialize the state of signal processing. if in a pipeline, just
	 * want to exit when a signal is received. otherwise, hold signals so
	 * they don't interfere with sys calls; they will be released at
	 * pre-emption points and upon pausing in the main loop.
	 *
	 * note that since we're multi-threaded, handling SIGCHLD causes
	 * problems with system()'s ability to obtain a child's exit status
	 * (because the main thread may process SIGCHLD before the thread
	 * running system() calls waitpid()). likewise explicitly ignoring
	 * SIGCHLD also prevents system() from getting an exit status.
	 * therefore we don't do anything with SIGCHLD.
	 */

	sigfillset(&sa.sa_mask);
	sa.sa_flags = 0;

	/* always ignore SIGPIPE, instead handle EPIPE as part
	 * of normal sys call error handling.
	 */
	sa.sa_handler = SIG_IGN;
	sigaction( SIGPIPE, &sa, NULL );

	if ( ! pipeline ) {
		sigset_t blocked_set;

		stop_in_progress = BOOL_FALSE;
		coredump_requested = BOOL_FALSE;
		sighup_received = BOOL_FALSE;
		sigterm_received = BOOL_FALSE;
		sigint_received = BOOL_FALSE;
		sigquit_received = BOOL_FALSE;
		sigstray_received = BOOL_FALSE;

		alarm( 0 );

		sigemptyset( &blocked_set );
		sigaddset( &blocked_set, SIGINT );
		sigaddset( &blocked_set, SIGHUP );
		sigaddset( &blocked_set, SIGTERM );
		sigaddset( &blocked_set, SIGQUIT );
		sigaddset( &blocked_set, SIGALRM );
		sigaddset( &blocked_set, SIGUSR1 );
		pthread_sigmask( SIG_SETMASK, &blocked_set, NULL );

		sa.sa_handler = sighandler;
		sigaction( SIGINT, &sa, NULL );
		sigaction( SIGHUP, &sa, NULL );
		sigaction( SIGTERM, &sa, NULL );
		sigaction( SIGQUIT, &sa, NULL );
		sigaction( SIGALRM, &sa, NULL );
		sigaction( SIGUSR1, &sa, NULL );
	}

	/* do content initialization.
	 */
#ifdef DUMP
	ok = content_init( argc, argv, gwhdrtemplatep );
#endif /* DUMP */
#ifdef RESTORE
	ok = content_init( argc, argv, vmsz / VMSZ_PER );
#endif /* RESTORE */
	if ( ! ok ) {
		err = mlog_exit(EXIT_ERROR, RV_INIT);
		goto err_free;
	}

	/* if in a pipeline, go single-threaded with just one stream.
	 */
	if ( pipeline ) {
		int exitcode;

		sa.sa_handler = sighandler;
		sigaction( SIGINT, &sa, NULL );
		sigaction( SIGHUP, &sa, NULL );
		sigaction( SIGTERM, &sa, NULL );
		sigaction( SIGQUIT, &sa, NULL );

		ok = drive_init2( argc,
				  argv,
#ifdef DUMP
				  gwhdrtemplatep );
#endif /* DUMP */
#ifdef RESTORE
				  ( global_hdr_t * )0 );
#endif /* RESTORE */
		if ( ! ok ) {
			err = mlog_exit(EXIT_ERROR, RV_INIT);
			goto err_free;
		}
		ok = drive_init3( );
		if ( ! ok ) {
			err = mlog_exit(EXIT_ERROR, RV_INIT);
			goto err_free;
		}
#ifdef DUMP
		exitcode = content_stream_dump( 0 );
#endif /* DUMP */
#ifdef RESTORE
		exitcode = content_stream_restore( 0 );
#endif /* RESTORE */
		if ( exitcode != EXIT_NORMAL ) {
			( void )content_complete( );
						/* for cleanup side-effect */
			err = mlog_exit(exitcode, RV_UNKNOWN);
		} else if ( content_complete( )) {
			err = mlog_exit(EXIT_NORMAL, RV_OK);
		} else {
			err = mlog_exit(EXIT_INTERRUPT, RV_UNKNOWN);
		}
		goto err_free;
	}

	/* used to skip to end if errors occur during any
	 * stage of initialization.
	 */
	init_error = BOOL_FALSE;

	/* now do the second and third passes of drive initialization.
	 * allocate per-stream write and read headers. if a drive
	 * manager uses a slave process, it should be created now,
	 * using cldmgr_create( ). each drive manager may use the slave to
	 * asynchronously read the media file header, typically a very
	 * time-consuming chore. drive_init3 will synchronize with each slave.
	 */
	if ( ! init_error ) {
		ok = drive_init2( argc,
				  argv,
#ifdef DUMP
				  gwhdrtemplatep );
#endif /* DUMP */
#ifdef RESTORE
				  ( global_hdr_t * )0 );
#endif /* RESTORE */
		if ( ! ok ) {
			init_error = BOOL_TRUE;
		}
	}
	if ( ! init_error ) {
		ok = drive_init3( );
		if ( ! ok ) {
			init_error = BOOL_TRUE;
		}
	}

	/* create a child thread for each stream. drivecnt global from
	 * drive.h, initialized by drive_init[12]
	 */
	if ( ! init_error ) {
		for ( stix = 0 ; stix < drivecnt ; stix++ ) {
			ok = cldmgr_create( childmain,
					    stix,
					    "child",
					    ( void * )stix );
			if ( ! ok ) {
				init_error = BOOL_TRUE;
			}
		}
	}

	/* loop here, waiting for children to die, processing operator
	 * signals.
	 */
	if ( progrpt_enabledpr ) {
		( void )alarm( ( uint )progrpt_interval );
	}
	for ( ; ; ) {
		time32_t now;
		bool_t stop_requested = BOOL_FALSE;
		int stop_timeout = -1;
		sigset_t empty_set;

		/* if there was an initialization error,
		 * immediately stop all children.
		 */
		if ( init_error ) {
			stop_timeout = STOP_TIMEOUT;
			stop_requested = BOOL_TRUE;
		}

		/* if one or more children died abnormally, request a
		 * stop. furthermore, note that core should be dumped if
		 * the child explicitly exited with EXIT_FAULT.
		 */
		xc = cldmgr_join( );
		if ( xc ) {
			if ( xc == EXIT_FAULT ) {
				coredump_requested = BOOL_TRUE;
				stop_timeout = ABORT_TIMEOUT;
			} else {
				stop_timeout = STOP_TIMEOUT;
			}
			prbcld_xc = xc;
			stop_requested = BOOL_TRUE;
		}
			
		/* all children died normally. break out.
		 */
		if ( cldmgr_remainingcnt( ) == 0 ) {
			mlog( MLOG_DEBUG,
			      "all children have exited\n" );
			break;
		}

		/* get the current time
		 */
		now = time( 0 );

		/* check for stop timeout. request a core dump and bail
		 */
		if ( stop_in_progress && now >= stop_deadline ) {
			mlog( MLOG_NORMAL | MLOG_ERROR,
			      _("session interrupt timeout\n") );
			coredump_requested = BOOL_TRUE;
			break;
		}

		/* operator sent SIGINT. if dialog allowed, enter dialog.
		 * otherwise treat as a hangup and request a stop.
		 */
		if ( sigint_received ) {
			mlog( MLOG_DEBUG | MLOG_PROC,
			      "SIGINT received\n" );
			if ( stop_in_progress ) {
				if ( dlog_allowed( )) {
					( void )sigint_dialog( );
				}
				/*
				mlog( MLOG_NORMAL,
				      _("session interrupt in progress: "
				      "please wait\n") );
				 */
			} else {
				if ( dlog_allowed( )) {
					stop_requested = sigint_dialog( );
				} else {
					stop_requested = BOOL_TRUE;
				}
				stop_timeout = STOP_TIMEOUT;
			}
				
			/* important that this appear after dialog.
			 * allows dialog to be terminated with SIGINT,
			 * without infinite loop.
			 */
			sigint_received = BOOL_FALSE;
		}

		/* refresh the current time in case in dialog for a while
		 */
		now = time( 0 );

		/* request a stop on hangup
		 */
		if ( sighup_received ) {
			mlog( MLOG_DEBUG | MLOG_PROC,
			      "SIGHUP received\n" );
			stop_requested = BOOL_TRUE;
			stop_timeout = STOP_TIMEOUT;
			sighup_received = BOOL_FALSE;
		}

		/* request a stop on termination request
		 */
		if ( sigterm_received ) {
			mlog( MLOG_DEBUG | MLOG_PROC,
			      "SIGTERM received\n" );
			stop_requested = BOOL_TRUE;
			stop_timeout = STOP_TIMEOUT;
			sigterm_received = BOOL_FALSE;
		}

		/* operator send SIGQUIT. treat like an interrupt,
		 * but force a core dump
		 */
		if ( sigquit_received ) {
			mlog( MLOG_NORMAL | MLOG_PROC,
			      "SIGQUIT received\n" );
			if ( stop_in_progress ) {
				mlog( MLOG_NORMAL,
				      _("session interrupt in progress: "
				      "please wait\n") );
				stop_deadline = now;
			} else {
				stop_requested = BOOL_TRUE;
				stop_timeout = ABORT_TIMEOUT;
				sigquit_received = BOOL_FALSE;
				coredump_requested = BOOL_TRUE;
			}
		}
		
		/* see if need to initiate a stop
		 */
		if ( stop_requested && ! stop_in_progress ) {
			mlog( MLOG_NORMAL,
			      _("initiating session interrupt (timeout in %d sec)\n"),
			      stop_timeout);
			mlog_exit_hint(RV_INTR);
			stop_in_progress = BOOL_TRUE;
			cldmgr_stop( );
			assert( stop_timeout >= 0 );
			stop_deadline = now + ( time32_t )stop_timeout;
		}
		
		/* set alarm if needed (note time stands still during dialog)
		 */
		if ( stop_in_progress ) {
			int timeout = ( int )( stop_deadline - now );
			if ( timeout < 0 ) {
				timeout = 0;
			}
			mlog( MLOG_DEBUG | MLOG_PROC,
			      "setting alarm for %d second%s\n",
			      timeout,
			      timeout == 1 ? "" : "s" );
			( void )alarm( ( uint )timeout );
			if ( timeout == 0 ) {
				continue;
			}
		}

		if ( progrpt_enabledpr && ! stop_in_progress ) {
			bool_t need_progrptpr = BOOL_FALSE;
			while ( now >= progrpt_deadline ) {
				need_progrptpr = BOOL_TRUE;
				progrpt_deadline += progrpt_interval;
			}
			if ( need_progrptpr ) {
				size_t statlinecnt;
				char **statline;
				ix_t i;
				statlinecnt = content_statline( &statline );
				for ( i = 0 ; i < statlinecnt ; i++ ) {
					mlog( MLOG_NORMAL,
					      statline[ i ] );
				}
			}
			( void )alarm( ( uint )( progrpt_deadline
						       -
						       now ));
		}

		/* sleep until next signal
		 */
		sigemptyset( &empty_set );
		sigsuspend( &empty_set );
		( void )alarm( 0 );
	}

	/* check if core dump requested
	 */
	if ( coredump_requested ) {
		mlog( MLOG_DEBUG | MLOG_PROC,
		      "core dump requested, aborting (pid %d)\n",
		      getpid() );
		abort();
	}

	/* determine if dump or restore was interrupted
	 * or an initialization error occurred.
	 */
	if ( init_error ) {
		( void )content_complete( );
		exitcode = EXIT_ERROR;
	} else {
		if ( content_complete( ) ) {
			if (prbcld_xc != EXIT_NORMAL)
				exitcode = EXIT_ERROR;
			else
				exitcode = EXIT_NORMAL;
		} else {
			exitcode = EXIT_INTERRUPT;
			if ( mlog_get_hint() == RV_NONE )
				mlog_exit_hint(RV_INCOMPLETE);
		}
	}

	err = mlog_exit(exitcode, RV_UNKNOWN);

err_free:
#ifdef DUMP
	global_hdr_free( gwhdrtemplatep );
#endif /* DUMP */
	return err;
}

#define ULO( f, o )	fprintf( stderr,		\
				 "%*s[ -%c %s ]\n",	\
				 ps,			\
				 ns,			\
				 o,			\
				 f ),			\
			ps = pfxsz

#define ULN( f )	fprintf( stderr,		\
				 "%*s[ %s ]\n",		\
				 ps,			\
				 ns,			\
				 f ),			\
			ps = pfxsz

void
usage( void )
{
	int pfxsz;
	int ps = 0;
	char *ns = "";

	pfxsz = fprintf(stderr, _("%s: usage: %s "),
				progname, basename(progname));

#ifdef DUMP
	ULO(_("(dump DMF dualstate files as offline)"),	GETOPT_DUMPASOFFLINE );
	ULO(_("<blocksize>"),				GETOPT_BLOCKSIZE );
	ULO(_("<media change alert program> "),		GETOPT_ALERTPROG );
	ULO(_("<dump media file size> "),		GETOPT_FILESZ );
	ULO(_("(allow files to be excluded)"),		GETOPT_EXCLUDEFILES );
	ULO(_("<destination> ..."),			GETOPT_DUMPDEST );
	ULO(_("(help)"),				GETOPT_HELP );
	ULO(_("<level>"),				GETOPT_LEVEL );
	ULO(_("(force usage of minimal rmt)"),		GETOPT_MINRMT );
	ULO(_("(overwrite tape)"),			GETOPT_OVERWRITE );
	ULO(_("<seconds between progress reports>"),	GETOPT_PROGRESS );
	ULO(_("<use QIC tape settings>"),		GETOPT_QIC );
	ULO(_("<subtree> ..."),				GETOPT_SUBTREE );
	ULO(_("<file> (use file mtime for dump time"),	GETOPT_DUMPTIME );
	ULO(_("<verbosity {silent, verbose, trace}>"),	GETOPT_VERBOSITY );
	ULO(_("<maximum file size>"),			GETOPT_MAXDUMPFILESIZE );
	ULO(_("(don't dump extended file attributes)"),	GETOPT_NOEXTATTR );
	ULO(_("<base dump session id>"),		GETOPT_BASED );
#ifdef REVEAL
	ULO(_("(generate tape record checksums)"),	GETOPT_RECCHKSUM );
#endif /* REVEAL */
	ULO(_("(skip unchanged directories)"),		GETOPT_NOUNCHANGEDDIRS );
	ULO(_("(pre-erase media)"),			GETOPT_ERASE );
	ULO(_("(don't prompt)"),			GETOPT_FORCE );
#ifdef REVEAL
	ULO(_("<minimum thread stack size>"),		GETOPT_MINSTACKSZ );
	ULO(_("<maximum thread stack size>"),		GETOPT_MAXSTACKSZ );
#endif /* REVEAL */
	ULO(_("(display dump inventory)"),		GETOPT_INVPRINT );
	ULO(_("(inhibit inventory update)"),		GETOPT_NOINVUPDATE );
	ULO(_("(generate format 2 dump)"),		GETOPT_FMT2COMPAT );
	ULO(_("<session label>"),			GETOPT_DUMPLABEL );
	ULO(_("<media label> ..."),			GETOPT_MEDIALABEL );
#ifdef REVEAL
	ULO(_("(timestamp messages)"),			GETOPT_TIMESTAMP );
#endif /* REVEAL */
	ULO(_("<options file>"),			GETOPT_OPTFILE );
#ifdef REVEAL
	ULO(_("(pin down I/O buffers)"),		GETOPT_RINGPIN );
#endif /* REVEAL */
	ULO(_("(resume)"),				GETOPT_RESUME );
	ULO(_("(don't timeout dialogs)"),		GETOPT_NOTIMEOUTS );
#ifdef REVEAL
	ULO(_("(unload media when change needed)"),	GETOPT_UNLOAD );
	ULO(_("(show subsystem in messages)"),		GETOPT_SHOWLOGSS );
	ULO(_("(show verbosity in messages)"),		GETOPT_SHOWLOGLEVEL );
#endif /* REVEAL */
	ULO(_("<I/O buffer ring length>"),		GETOPT_RINGLEN );
	ULN(_("- (stdout)") );
	ULN(_("<source (mntpnt|device)>") );
#endif /* DUMP */
#ifdef RESTORE
	ULO(_("<alt. workspace dir> ..."),		GETOPT_WORKSPACE );
	ULO(_("<blocksize>"),				GETOPT_BLOCKSIZE );
	ULO(_("<media change alert program> "),		GETOPT_ALERTPROG );
	ULO(_("(don't overwrite existing files)"),	GETOPT_EXISTING );
	ULO(_("<source> ..."),				GETOPT_DUMPDEST );
	ULO(_("(help)"),				GETOPT_HELP );
	ULO(_("(interactive)"),				GETOPT_INTERACTIVE );
	ULO(_("(force usage of minimal rmt)"),		GETOPT_MINRMT );
	ULO(_("<file> (restore only if newer than)"),	GETOPT_NEWER );
	ULO(_("(restore owner/group even if not root)"),GETOPT_OWNER );
	ULO(_("<seconds between progress reports>"),	GETOPT_PROGRESS );
	ULO(_("<use QIC tape settings>"),		GETOPT_QIC );
	ULO(_("(cumulative restore)"),			GETOPT_CUMULATIVE );
	ULO(_("<subtree> ..."),				GETOPT_SUBTREE );
	ULO(_("(contents only)"),			GETOPT_TOC );
	ULO(_("<verbosity {silent, verbose, trace}>"),	GETOPT_VERBOSITY );
	ULO(_("(use small tree window)"),		GETOPT_SMALLWINDOW );
	ULO(_("(don't restore extended file attributes)"),GETOPT_NOEXTATTR );
	ULO(_("(restore root dir owner/permissions)"),	GETOPT_ROOTPERM );
	ULO(_("(restore DMAPI event settings)"),	GETOPT_SETDM );
#ifdef REVEAL
	ULO(_("(check tape record checksums)"),		GETOPT_RECCHKSUM );
#endif /* REVEAL */
	ULO(_("(don't overwrite if changed)"),		GETOPT_CHANGED );
	ULO(_("(don't prompt)"),			GETOPT_FORCE );
	ULO(_("(display dump inventory)"),		GETOPT_INVPRINT );
	ULO(_("(inhibit inventory update)"),		GETOPT_NOINVUPDATE );
	ULO(_("(force use of format 2 generation numbers)"),GETOPT_FMT2COMPAT );
	ULO(_("<session label>"),			GETOPT_DUMPLABEL );
#ifdef REVEAL
	ULO(_("(timestamp messages)"),			GETOPT_TIMESTAMP );
#endif /* REVEAL */
	ULO(_("<options file>"),			GETOPT_OPTFILE );
#ifdef REVEAL
	ULO(_("(pin down I/O buffers)"),		GETOPT_RINGPIN );
#endif /* REVEAL */
	ULO(_("(force interrupted session completion)"),GETOPT_SESSCPLT );
	ULO(_("(resume)"),				GETOPT_RESUME );
	ULO(_("<session id>"),				GETOPT_SESSIONID );
	ULO(_("(don't timeout dialogs)"),		GETOPT_NOTIMEOUTS );
#ifdef REVEAL
	ULO(_("(unload media when change needed)"),	GETOPT_UNLOAD );
	ULO(_("(show subsystem in messages)"),		GETOPT_SHOWLOGSS );
	ULO(_("(show verbosity in messages)"),		GETOPT_SHOWLOGLEVEL );
#endif /* REVEAL */
	ULO(_("<excluded subtree> ..."),		GETOPT_NOSUBTREE );
	ULO(_("<I/O buffer ring length>"),		GETOPT_RINGLEN );
	ULN(_("- (stdin)") );
	ULN(_("<destination>") );
#endif /* RESTORE */

	/* anywhere usage is called we will exit shortly after...
	 * catch all of those cases below
	 */

	(void) mlog_exit(EXIT_ERROR, RV_OPT);
}

/* returns TRUE if preemption
 */
bool_t
preemptchk( int flg )
{
	bool_t preempt_requested;
	int i;
	int sigs[] = { SIGINT, SIGHUP, SIGTERM, SIGQUIT };
	int num_sigs = sizeof(sigs) / sizeof(sigs[0]);
	sigset_t pending_set, handle_set;

	/* see if a progress report needed
	 */
	if ( progrpt_enabledpr ) {
		time32_t now = time( 0 );
		bool_t need_progrptpr = BOOL_FALSE;
		while ( now >= progrpt_deadline ) {
			need_progrptpr = BOOL_TRUE;
			progrpt_deadline += progrpt_interval;
		}
		if ( need_progrptpr ) {
			size_t statlinecnt;
			char **statline;
			ix_t i;
			statlinecnt = content_statline( &statline );
			for ( i = 0 ; i < statlinecnt ; i++ ) {
				mlog( MLOG_NORMAL,
				      statline[ i ] );
			}
		}
	}

	/* Progress report only */
	if (flg == PREEMPT_PROGRESSONLY) {
		return BOOL_FALSE;
	}

	/* signals not caught if in a pipeline
	 */
	if ( pipeline ) {
		return BOOL_FALSE;
	}

	/* release signals momentarily to let any pending ones
	 * invoke signal handler and set flags
	 */
	sigpending( &pending_set );
	for ( i = 0; i < num_sigs; i++ ) {
		if ( sigismember( &pending_set, sigs[i] ) == 1 ) {
			sigfillset( &handle_set );
			sigdelset( &handle_set, sigs[i] );
			sigsuspend( &handle_set );
		}
	}

	preempt_requested = BOOL_FALSE;

	if ( sigint_received ) {
		mlog( MLOG_DEBUG | MLOG_PROC,
		      "SIGINT received (preempt)\n" );
		if ( dlog_allowed( )) {
			preempt_requested = sigint_dialog( );
		} else {
			preempt_requested = BOOL_TRUE;
		}
		/* important that this appear after dialog.
		 * allows dialog to be terminated with SIGINT,
		 * without infinite loop.
		 */
		sigint_received = BOOL_FALSE;
	}

	if ( sighup_received ) {
		mlog( MLOG_DEBUG | MLOG_PROC,
		      "SIGHUP received (prempt)\n" );
		preempt_requested = BOOL_TRUE;
		sighup_received = BOOL_FALSE;
	}

	if ( sigterm_received ) {
		mlog( MLOG_DEBUG | MLOG_PROC,
		      "SIGTERM received (prempt)\n" );
		preempt_requested = BOOL_TRUE;
		sigterm_received = BOOL_FALSE;
	}

	if ( sigquit_received ) {
		mlog( MLOG_DEBUG | MLOG_PROC,
		      "SIGQUIT received (preempt)\n" );
		preempt_requested = BOOL_TRUE;
		sigquit_received = BOOL_FALSE;
	}

	return preempt_requested;
}

/* definition of locally defined static functions ****************************/

static bool_t
loadoptfile( int *argcp, char ***argvp )
{
	char *optfilename;
	ix_t optfileix = 0;
	int fd;
	size_t sz;
	int i;
	struct stat64 stat;
	char *argbuf;
	char *p;
	size_t tokencnt;
	int nread;
	const char *sep = " \t\n\r";
	char **newargv;
	int c;
	int rval;

	/* see if option specified
	 */
	optind = 1;
	opterr = 0;
	optfilename =  0;
	while ( ( c = getopt( *argcp, *argvp, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
		case GETOPT_OPTFILE:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_NOLOCK,
				      _("-%c argument missing\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			if ( optfilename ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_NOLOCK,
				      _("-%c allowed only once\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			optfilename = optarg;
			assert( optind > 2 );
			optfileix = ( ix_t )optind - 2;
			break;
		}
	}
	if ( ! optfilename )  {
		return BOOL_TRUE;
	}

	/* attempt to open the option  file
	 */
	errno = 0;
	fd = open( optfilename, O_RDONLY );
	if ( fd  < 0 ) {
		mlog( MLOG_ERROR | MLOG_NOLOCK,
		      _("cannot open option file %s: %s (%d)\n"),
		      optfilename,
		      strerror( errno ),
		      errno );
		return BOOL_FALSE;
	}

	/* get file status
	 */
	rval = fstat64( fd, &stat );
	if ( rval ) {
		mlog( MLOG_ERROR | MLOG_NOLOCK,
		      _("cannot stat option file %s: %s (%d)\n"),
		      optfilename,
		      strerror( errno ),
		      errno );
		close( fd );
		return BOOL_FALSE;
	}

	/* ensure the file is ordinary
	 */
	if ( ( stat.st_mode & S_IFMT ) != S_IFREG ) {
		mlog( MLOG_ERROR | MLOG_NOLOCK,
		      _("given option file %s is not ordinary file\n"),
		      optfilename );
		close( fd );
		return BOOL_FALSE;
	}

	/* calculate the space required for the cmd line options.
	 * skip the GETOPT_OPTFILE option which put us here!
	 */
	sz = 0;
	for ( i =  0 ; i < *argcp ; i++ ) {
		if ( i == ( int )optfileix ) {
			i++; /* to skip option argument */
			continue;
		}
		sz += strlen( ( * argvp )[ i ] ) + 1;
	}

	/* add in the size of the option file (plus one byte in case
	 * option file ends without newline, and one NULL for safety)
	 */
	sz += ( size_t )stat.st_size + 2;

	/* allocate an argument buffer
	 */
	argbuf = ( char * )malloc( sz );
	assert( argbuf );

	/* copy arg0 (the executable's name ) in first
	 */
	p = argbuf;
	i = 0;
	sprintf( p, "%s ", ( * argvp )[ i ] );
	p += strlen( ( * argvp )[ i ] ) + 1;
	i++;

	/* copy the options file into the buffer after the given args
	 */
	nread = read( fd, ( void * )p, ( size_t )stat.st_size );
	if ( nread < 0 ) {
		mlog( MLOG_ERROR | MLOG_NOLOCK,
		      _("read of option file %s failed: %s (%d)\n"),
		      optfilename,
		      strerror( errno ),
		      errno );
		close( fd );
		return BOOL_FALSE;
	}
	assert( ( off64_t )nread == stat.st_size );
	p += ( size_t )stat.st_size;
	*p++ = ' ';

	/* copy the remaining command line args into the buffer
	 */
	for ( ; i < *argcp ; i++ ) {
		if ( i == ( int )optfileix ) {
			i++; /* to skip option argument */
			continue;
		}
		sprintf( p, "%s ", ( * argvp )[ i ] );
		p += strlen( ( * argvp )[ i ] ) + 1;
	}

	/* null-terminate the entire buffer
	 */
	*p++ = 0;
	assert( ( size_t )( p - argbuf ) <= sz );

	/* change newlines and carriage returns into spaces
	 */
	for ( p = argbuf ; *p ; p++ ) {
		if ( strchr( "\n\r", ( int )( *p ))) {
			*p = ' ';
		}
	}

	/* count the tokens in the buffer
	 */
	tokencnt = 0;
	p = argbuf;
	for ( ; ; ) {
		/* start at the first non-separator character
		 */
		while ( *p && strchr( sep, ( int )( *p ))) {
			p++;
		}

		/* done when NULL encountered
		 */
		if ( ! *p ) {
			break;
		}

		/* we have a token
		 */
		tokencnt++;

		/* find the end of the first token
		 */
		p = strpbrkquotes( p, sep );

		/* if no more separators, all tokens seen
		 */
		if ( ! p ) {
			break;
		}
	}

	/* if no arguments, can return now
	 */
	if ( ! tokencnt ) {
		close( fd );
		return BOOL_TRUE;
	}

	/* allocate a new argv array to hold the tokens
	 */
	newargv = ( char ** )calloc( tokencnt, sizeof( char * ));
	assert( newargv );

	/* null-terminate tokens and place in new argv, after
	 * extracting quotes and escapes
	 */
	p = argbuf;
	for ( i = 0 ; ; i++ ) {
		char *endp = 0;

		/* start at the first non-separator character
		 */
		while ( *p && strchr( sep, ( int )*p )) {
			p++;
		}

		/* done when NULL encountered
		 */
		if ( ! *p ) {
			break;
		}

		/* better not disagree with counting scan!
		 */
		assert( i < ( int )tokencnt );

		/* find the end of the first token
		 */
		endp = strpbrkquotes( p, sep );

		/* null-terminate if needed
		 */
		if ( endp ) {
			*endp = 0;
		}

		/* strip quotes and escapes
		 */
		p = stripquotes( p );

		/* stick result in new argv array
		 */
		newargv[ i ] = p;

		/* if no more separators, all tokens seen
		 */
		if ( ! endp ) {
			break;
		}

		p = endp + 1;
	}

	/* return new argc anr argv
	 */
	close( fd );
	*argcp = ( int )tokencnt;
	*argvp = newargv;
	return BOOL_TRUE;
}

/* parent and children share this handler. 
 */
static void
sighandler( int signo )
{
	/* dialog gets first crack at the signal
	 */
	if ( dlog_sighandler( signo ) )
		return;

	/* if in pipeline, don't do anything risky. just quit.
	 */
	if ( pipeline ) {
		int rval;

		mlog( MLOG_TRACE | MLOG_NOTE | MLOG_NOLOCK | MLOG_PROC,
		      _("received signal %d (%s): cleanup and exit\n"),
		      signo,
		      sig_numstring( signo ));

		if ( content_complete( )) {
			rval = EXIT_NORMAL;
		} else {
			rval = EXIT_INTERRUPT;
		}
		mlog_exit(rval, RV_NONE);
		exit( rval );
	}

	switch ( signo ) {
	case SIGHUP:
		/* immediately disable further dialogs
		*/
		dlog_desist( );
		sighup_received = BOOL_TRUE;
		break;
	case SIGTERM:
		/* immediately disable further dialogs
		*/
		dlog_desist( );
		sigterm_received = BOOL_TRUE;
		break;
	case SIGINT:
		sigint_received = BOOL_TRUE;
		break;
	case SIGQUIT:
		/* immediately disable further dialogs
		 */
		dlog_desist( );
		sigquit_received = BOOL_TRUE;
		break;
	case SIGALRM:
	case SIGUSR1:
		break;
	default:
		sigstray_received = signo;
		break;
	}
}

static int
childmain( void *arg1 )
{
	ix_t stix;
	int exitcode;
	drive_t *drivep;

	/* Determine which stream I am.
	 */
	stix = ( ix_t )arg1;

	/* tell the content manager to begin.
	 */
#ifdef DUMP
	exitcode = content_stream_dump( stix );
#endif /* DUMP */
#ifdef RESTORE
	exitcode = content_stream_restore( stix );
#endif /* RESTORE */

	/* let the drive manager shut down its slave thread
	 */
	drivep = drivepp[ stix ];
	( * drivep->d_opsp->do_quit )( drivep );

	return exitcode;
}


/* ARGSUSED */
static void
prompt_prog_cb( void *uctxp, dlog_pcbp_t pcb, void *pctxp )
{
	/* query: ask for a dump label
	 */
	( * pcb )( pctxp,
		   progrpt_enabledpr
		   ?
		   _("please enter seconds between progress reports, "
		   "or 0 to disable")
		   :
		   _("please enter seconds between progress reports") );
}

/* SIGINTR dialog
 *
 * side affect is to change verbosity level.
 * return code of BOOL_TRUE indicates a stop was requested.
 */
#define PREAMBLEMAX	( 7 + 2 * STREAM_SIMMAX )
#define QUERYMAX	3
#define CHOICEMAX	9
#define ACKMAX		7
#define POSTAMBLEMAX	3

static bool_t
sigint_dialog( void )
{
	fold_t fold;
	char **statline;
	ix_t i;
	size_t statlinecnt;
	char *preamblestr[ PREAMBLEMAX ];
	size_t preamblecnt;
	char *querystr[ QUERYMAX ];
	size_t querycnt;
	char *choicestr[ CHOICEMAX ];
	size_t choicecnt;
	char *ackstr[ ACKMAX ];
	size_t ackcnt;
	char *postamblestr[ POSTAMBLEMAX ];
	size_t postamblecnt;
	size_t interruptix;
	size_t verbosityix;
	size_t metricsix;
	size_t controlix;
	size_t ioix;
	size_t mediachangeix;
#ifdef RESTORE
	size_t piix;
	size_t roix;
#endif /* RESTORE */
	size_t progix;
	size_t mllevix;
	size_t mlssix;
	size_t mltsix;
	size_t continueix;
	size_t allix;
	size_t nochangeix;
	size_t responseix;
	int ssselected = 0;
	bool_t stop_requested = BOOL_FALSE;

	/* preamble: the content status line, indicate if interrupt happening
	 */
	fold_init( fold, _("status and control dialog"), '=' );
	statlinecnt = content_statline( &statline );
	preamblecnt = 0;
	preamblestr[ preamblecnt++ ] = "\n";
	preamblestr[ preamblecnt++ ] = fold;
	preamblestr[ preamblecnt++ ] = "\n";
	preamblestr[ preamblecnt++ ] = "\n";
	for ( i = 0 ; i < statlinecnt ; i++ ) {
		preamblestr[ preamblecnt++ ] = statline[ i ];
	}
	if ( stop_in_progress ) {
		preamblestr[ preamblecnt++ ] =
			_("\nsession interrupt in progress\n");
	}
	preamblestr[ preamblecnt++ ] = "\n";
	assert( preamblecnt <= PREAMBLEMAX );
	dlog_begin( preamblestr, preamblecnt );

	/* top-level query: a function of session interrupt status
	 */
	querycnt = 0;
	querystr[ querycnt++ ] = _("please select one of "
				 "the following operations\n");
	assert( querycnt <= QUERYMAX );
	choicecnt = 0;
	if ( ! stop_in_progress ) {
		interruptix = choicecnt;
		choicestr[ choicecnt++ ] = _("interrupt this session");
	} else {
		interruptix = SIZEMAX; /* never happen */
	}

	verbosityix = choicecnt;
	choicestr[ choicecnt++ ] = _("change verbosity");
	metricsix = choicecnt;
	choicestr[ choicecnt++ ] = _("display metrics");
	if ( content_media_change_needed ) {
		mediachangeix = choicecnt;
		choicestr[ choicecnt++ ] = _("confirm media change");
	} else {
		mediachangeix = SIZEMAX; /* never happen */
	}
	controlix = choicecnt;
	choicestr[ choicecnt++ ] = _("other controls");
	continueix = choicecnt;
	choicestr[ choicecnt++ ] = _("continue");
	assert( choicecnt <= CHOICEMAX );

	responseix = dlog_multi_query( querystr,
				       querycnt,
				       choicestr,
				       choicecnt,
				       0,		/* hilitestr */
				       IXMAX,		/* hiliteix */
				       0,		/* defaultstr */
				       continueix,	/* defaultix */
				       DLOG_TIMEOUT,	/* timeout */
				       continueix,	/* timeout ix */
				       continueix,	/* sigint ix */
				       continueix,	/* sighup ix */
				       continueix );	/* sigquit ix */
	if ( responseix == interruptix ) {
		ackcnt = 0;
		ackstr[ ackcnt++ ] = "\n";
		dlog_multi_ack( ackstr,
				ackcnt );
		querycnt = 0;
		querystr[ querycnt++ ] = _("please confirm\n");
		assert( querycnt <= QUERYMAX );
		choicecnt = 0;
		interruptix = choicecnt;
		choicestr[ choicecnt++ ] = _("interrupt this session");
		nochangeix = choicecnt;
		choicestr[ choicecnt++ ] = _("continue");
		assert( choicecnt <= CHOICEMAX );
		responseix = dlog_multi_query( querystr,
					       querycnt,
					       choicestr,
					       choicecnt,
					       0,	/* hilitestr */
					       IXMAX,	/* hiliteix */
					       0,       /* defaultstr */
					       nochangeix, /* defaultix */
					       DLOG_TIMEOUT,/* timeout */
					       nochangeix, /* timeout ix */
					       nochangeix, /* sigint ix */
					       nochangeix, /* sighup ix */
					       nochangeix);/* sigquit ix */
		ackcnt = 0;
		if ( responseix == nochangeix ) {
			ackstr[ ackcnt++ ] = _("continuing\n");
		} else {
			ackstr[ ackcnt++ ] = _("interrupt request accepted\n");
			stop_requested = BOOL_TRUE;
		}
		dlog_multi_ack( ackstr,
				ackcnt );
	} else if ( responseix == verbosityix ) {
		ackcnt = 0;
		ackstr[ ackcnt++ ] = "\n";
		dlog_multi_ack( ackstr,
				ackcnt );
		querycnt = 0;
		querystr[ querycnt++ ] = _("please select one of "
					 "the following subsystems\n");
		assert( querycnt <= QUERYMAX );
		choicecnt = 0;
		/* number of lines must match number of subsystems
		 */
		for ( choicecnt = 0 ; choicecnt < MLOG_SS_CNT ; choicecnt++ ) {
			choicestr[ choicecnt ] = mlog_ss_names[ choicecnt ];
		}
		allix = choicecnt;
		choicestr[ choicecnt++ ] = _("all of the above");
		nochangeix = choicecnt;
		choicestr[ choicecnt++ ] = _("no change");
		assert( choicecnt <= CHOICEMAX );
		responseix = dlog_multi_query( querystr,
					       querycnt,
					       choicestr,
					       choicecnt,
					       0,	/* hilitestr */
					       IXMAX,	/* hiliteix */
					       0,       /* defaultstr */
					       allix,   /* defaultix */
					       DLOG_TIMEOUT,/* timeout */
					       nochangeix, /* timeout ix */
					       nochangeix, /* sigint ix */
					       nochangeix, /* sighup ix */
					       nochangeix);/* sigquit ix */
		ackcnt = 0;
		if ( responseix == nochangeix ) {
			ackstr[ ackcnt++ ] = _("no change\n");
		} else if ( responseix == allix ) {
			ssselected = -1;
			ackstr[ ackcnt++ ] = _("all subsystems selected\n\n");
		} else {
			ssselected = ( int )responseix;
			ackstr[ ackcnt++ ] = "\n";
		}
		dlog_multi_ack( ackstr,
				ackcnt );
		if ( responseix != nochangeix ) {
			querycnt = 0;
			querystr[ querycnt++ ] = ("please select one of the "
						  "following verbosity levels\n");
			assert( querycnt <= QUERYMAX );
			choicecnt = 0;
			choicestr[ choicecnt++ ] = _("silent");
			choicestr[ choicecnt++ ] = _("verbose");
			choicestr[ choicecnt++ ] = _("trace");
			choicestr[ choicecnt++ ] = _("debug");
			choicestr[ choicecnt++ ] = _("nitty");
			choicestr[ choicecnt++ ] = _("nitty + 1");
			nochangeix = choicecnt;
			choicestr[ choicecnt++ ] = _("no change");
			assert( choicecnt <= CHOICEMAX );
			responseix = dlog_multi_query( querystr,
						       querycnt,
						       choicestr,
						       choicecnt,
						       ssselected == -1
						       ?
						       0
						       :
						    _(" (current)"),/* hilitestr */
						       ssselected == -1
						       ?
						       IXMAX
						       :
			     ( ix_t )mlog_level_ss[ ssselected ], /* hiliteix */
						       0,       /* defaultstr */
						      nochangeix,/* defaultix */
						    DLOG_TIMEOUT,/* timeout */
						    nochangeix, /* timeout ix */
						     nochangeix, /* sigint ix */
						     nochangeix, /* sighup ix */
						    nochangeix);/* sigquit ix */
			ackcnt = 0;
			if ( responseix == nochangeix
			     ||
			     ( ssselected >= 0
			       &&
			       responseix
			       ==
			       ( ix_t )mlog_level_ss[ ssselected ] )) {
				ackstr[ ackcnt++ ] = _("no change\n");
			} else {
				if ( ssselected < 0 ) {
					ix_t ssix;
					assert( ssselected == -1 );
					for ( ssix = 0
					      ;
					      ssix < MLOG_SS_CNT
					      ;
					      ssix++ ) {
						mlog_level_ss[ ssix ] =
							( int )responseix;
					}
				} else {
					mlog_level_ss[ ssselected ] =
							( int )responseix;
				}
				ackstr[ ackcnt++ ] = _("level changed\n");
			}
			dlog_multi_ack( ackstr,
					ackcnt );
		}
	} else if ( responseix == metricsix ) {
		ackcnt = 0;
		ackstr[ ackcnt++ ] = "\n";
		dlog_multi_ack( ackstr,
				ackcnt );
		querycnt = 0;
		querystr[ querycnt++ ] = _("please select one of "
					  "the following metrics\n");
		assert( querycnt <= QUERYMAX );
		choicecnt = 0;
		ioix = choicecnt;
		choicestr[ choicecnt++ ] = _("I/O");
#ifdef RESTORE
		piix = choicecnt;
		choicestr[ choicecnt++ ] = _("media inventory status");
		roix = choicecnt;
		choicestr[ choicecnt++ ] = _("needed media objects");
#endif /* RESTORE */
		nochangeix = choicecnt;
		choicestr[ choicecnt++ ] = _("continue");
		assert( choicecnt <= CHOICEMAX );
		responseix = dlog_multi_query( querystr,
					       querycnt,
					       choicestr,
					       choicecnt,
					       0,           /* hilitestr */
					       IXMAX,       /* hiliteix */
					       0,           /* defaultstr */
					       nochangeix,  /* defaultix */
					       DLOG_TIMEOUT,
					       nochangeix, /* timeout ix */
					       nochangeix, /* sigint ix */
					       nochangeix, /* sighup ix */
					       nochangeix);/* sigquit ix */
		if ( responseix != nochangeix ) {
			ackcnt = 0;
			ackstr[ ackcnt++ ] = "\n";
			dlog_multi_ack( ackstr,
					ackcnt );
		}
		if ( responseix == ioix ) {
			drive_display_metrics( );
#ifdef RESTORE
		} else if ( responseix == piix ) {
			content_showinv( );
		} else if ( responseix == roix ) {
			content_showremainingobjects( );
#endif /* RESTORE */
		}

		if ( responseix != nochangeix ) {
			querycnt = 0;
			querystr[ querycnt++ ] = "\n";
			assert( querycnt <= QUERYMAX );
			choicecnt = 0;
			nochangeix = choicecnt;
			choicestr[ choicecnt++ ] = _("continue");
			assert( choicecnt <= CHOICEMAX );
			responseix = dlog_multi_query( querystr,
						       querycnt,
						       choicestr,
						       choicecnt,
						       0,        /* hilitestr */
						       IXMAX,    /* hiliteix */
						       0,       /* defaultstr */
						       nochangeix,/* defaultix*/
						       DLOG_TIMEOUT,
						       nochangeix,/*timeout ix*/
						       nochangeix,/* sigint ix*/
						       nochangeix,/* sighup ix*/
						       nochangeix);/*sigquitix*/
		}
		ackcnt = 0;
		ackstr[ ackcnt++ ] = _("continuing\n");
		dlog_multi_ack( ackstr,
				ackcnt );
	} else if ( responseix == mediachangeix ) {
		ackcnt = 0;
		dlog_multi_ack( ackstr,
				ackcnt );
		ackcnt = 0;
		ackstr[ ackcnt++ ] = content_mediachange_query( );
		dlog_multi_ack( ackstr,
				ackcnt );
	} else if ( responseix == controlix ) {
		ackcnt = 0;
		ackstr[ ackcnt++ ] = "\n";
		dlog_multi_ack( ackstr,
				ackcnt );
		querycnt = 0;
		querystr[ querycnt++ ] = _("please select one of "
					   "the following controls\n");
		assert( querycnt <= QUERYMAX );
		choicecnt = 0;
		progix = choicecnt;
		if ( progrpt_enabledpr ) {
		    choicestr[ choicecnt++ ] = _("change interval of "
					         "or disable progress reports");
		} else {
		    choicestr[ choicecnt++ ] = _("enable progress reports");
		}
		mllevix = choicecnt;
		if ( mlog_showlevel ) {
			choicestr[ choicecnt++ ] = _("hide log message levels");
		} else {
			choicestr[ choicecnt++ ] = _("show log message levels");
		}
		mlssix = choicecnt;
		if ( mlog_showss ) {
			choicestr[ choicecnt++ ] = _("hide log message subsystems");
		} else {
			choicestr[ choicecnt++ ] = _("show log message subsystems");
		}
		mltsix = choicecnt;
		if ( mlog_timestamp ) {
			choicestr[ choicecnt++ ] = _("hide log message timestamps");
		} else {
			choicestr[ choicecnt++ ] = _("show log message timestamps");
		}
		nochangeix = choicecnt;
		choicestr[ choicecnt++ ] = _("continue");
		assert( choicecnt <= CHOICEMAX );
		responseix = dlog_multi_query( querystr,
					       querycnt,
					       choicestr,
					       choicecnt,
					       0,           /* hilitestr */
					       IXMAX,       /* hiliteix */
					       0,           /* defaultstr */
					       nochangeix,  /* defaultix */
					       DLOG_TIMEOUT,
					       nochangeix, /* timeout ix */
					       nochangeix, /* sigint ix */
					       nochangeix, /* sighup ix */
					       nochangeix);/* sigquit ix */
		ackcnt = 0;
		if ( responseix == progix ) {
			char buf[ 10 ];
			const size_t ncix = 1;
			const size_t okix = 2;

			ackstr[ ackcnt++ ] = "\n";
			dlog_multi_ack( ackstr,
					ackcnt );
			ackcnt = 0;
			responseix = dlog_string_query( prompt_prog_cb,
							0,
							buf,
							sizeof( buf ),
							DLOG_TIMEOUT,
							ncix,/* timeout ix */
							ncix, /* sigint ix */
							ncix,  /* sighup ix */
							ncix,  /* sigquit ix */
							okix );
			if ( responseix == okix ) {
				int newinterval;
				newinterval = atoi( buf );
				if ( ! strlen( buf )) {
					ackstr[ ackcnt++ ] = _("no change\n");
				} else if ( newinterval > 0 ) {
					time32_t newdeadline;
					char intervalbuf[ 64 ];
					newdeadline = time( 0 ) + ( time32_t )newinterval;
					if ( progrpt_enabledpr ) {
						if ( ( time32_t )newinterval == progrpt_interval ) {
							ackstr[ ackcnt++ ] = _("no change\n");
						} else {
							ackstr[ ackcnt++ ] = _("changing progress report interval to ");
							sprintf( intervalbuf,
								 _("%d seconds\n"),
								 newinterval );
							assert( strlen( intervalbuf )
								<
								sizeof( intervalbuf ));
							ackstr[ ackcnt++ ] = intervalbuf;
							if ( progrpt_deadline > newdeadline ) {
								progrpt_deadline = newdeadline;
							}
						}
					} else {
						ackstr[ ackcnt++ ] = _("enabling progress reports at ");
						sprintf( intervalbuf,
							 _("%d second intervals\n"),
							 newinterval );
						assert( strlen( intervalbuf )
							<
							sizeof( intervalbuf ));
						ackstr[ ackcnt++ ] = intervalbuf;
						progrpt_enabledpr = BOOL_TRUE;
						progrpt_deadline = newdeadline;
					}
					progrpt_interval = ( time32_t )newinterval;
				} else {
					if ( progrpt_enabledpr ) {
						ackstr[ ackcnt++ ] = _("disabling progress reports\n");
					} else {
						ackstr[ ackcnt++ ] = _("no change\n");
					}
					progrpt_enabledpr = BOOL_FALSE;
				}
			} else {
				ackstr[ ackcnt++ ] = _("no change\n");
			}
		} else if ( responseix == mllevix ) {
			mlog_showlevel = ! mlog_showlevel;
			if ( mlog_showlevel ) {
				ackstr[ ackcnt++ ] = _("showing log message levels\n");
			} else {
				ackstr[ ackcnt++ ] = _("hiding log message levels\n");
			}
		} else if ( responseix == mlssix ) {
			mlog_showss = ! mlog_showss;
			if ( mlog_showss ) {
				ackstr[ ackcnt++ ] = _("showing log message subsystems\n");
			} else {
				ackstr[ ackcnt++ ] = _("hiding log message subsystems\n");
			}
		} else if ( responseix == mltsix ) {
			mlog_timestamp = ! mlog_timestamp;
			if ( mlog_timestamp ) {
				ackstr[ ackcnt++ ] = _("showing log message timestamps\n");
			} else {
				ackstr[ ackcnt++ ] = _("hiding log message timestamps\n");
			}
		}
		dlog_multi_ack( ackstr,
				ackcnt );
	} else {
		ackcnt = 0;
		ackstr[ ackcnt++ ] = _("continuing\n");
		dlog_multi_ack( ackstr,
				ackcnt );
	}

	fold_init( fold, _("end dialog"), '-' );
	postamblecnt = 0;
	postamblestr[ postamblecnt++ ] = "\n";
	postamblestr[ postamblecnt++ ] = fold;
	postamblestr[ postamblecnt++ ] = "\n\n";
	assert( postamblecnt <= POSTAMBLEMAX );
	dlog_end( postamblestr,
		  postamblecnt );

	return stop_requested;
}

static char *
sigintstr( void )
{
	int ttyfd;
	static char buf[ 20 ];
	struct termios termios;
	cc_t intchr;
	int rval;

	ttyfd = dlog_fd( );
	if ( ttyfd == -1 ) {
		return 0;
	}

	rval = tcgetattr( ttyfd, &termios );
	if ( rval ) {
		mlog( MLOG_NITTY | MLOG_PROC,
		      "could not get controlling terminal information: %s\n",
		      strerror( errno ));
		return 0;
	}
	
	intchr = termios.c_cc[ VINTR ];
	mlog( MLOG_NITTY | MLOG_PROC,
	      "tty fd: %d; terminal interrupt character: %c (0%o)\n",
	      ttyfd,
	      intchr,
	      intchr );

	if ( intchr < ' ' ) {
		sprintf( buf, "^%c", intchr + '@' );
	} else if ( intchr == 0177 ) {
		sprintf( buf, "DEL" );
	} else {
		sprintf( buf, "%c", intchr );
	}
	assert( strlen( buf ) < sizeof( buf ));

	return buf;
}

#ifdef DUMP
static bool_t
set_rlimits( void )
#endif /* DUMP */
#ifdef RESTORE
static bool_t
set_rlimits( size64_t *vmszp )
#endif /* RESTORE */
{
	struct rlimit64 rlimit64;
#ifdef RESTORE
	size64_t vmsz;
#endif /* RESTORE */
	/* REFERENCED */
	int rval;

	assert( minstacksz <= maxstacksz );

	rval = getrlimit64( RLIMIT_AS, &rlimit64 );

	assert( ! rval );
	mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
	      "RLIMIT_AS org cur 0x%llx max 0x%llx\n",
	      rlimit64.rlim_cur,
	      rlimit64.rlim_max );
#ifdef RESTORE
	if (rlimit64.rlim_cur != RLIM64_INFINITY) {
		rlimit64.rlim_cur = rlimit64.rlim_max;
		( void )setrlimit64( RLIMIT_AS, &rlimit64 );
		rval = getrlimit64( RLIMIT_AS, &rlimit64 );
		assert( ! rval );
		mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
			"RLIMIT_VMEM now cur 0x%llx max 0x%llx\n",
			rlimit64.rlim_cur,
			rlimit64.rlim_max );
	}

	vmsz = ( size64_t )rlimit64.rlim_cur;
#endif /* RESTORE */
	
	assert( minstacksz <= maxstacksz );
	rval = getrlimit64( RLIMIT_STACK, &rlimit64 );
	assert( ! rval );
	mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
	      "RLIMIT_STACK org cur 0x%llx max 0x%llx\n",
	      rlimit64.rlim_cur,
	      rlimit64.rlim_max );
	if ( rlimit64.rlim_cur < minstacksz ) {
		if ( rlimit64.rlim_max < minstacksz ) {
			mlog( MLOG_DEBUG
			      |
			      MLOG_NOLOCK
			      |
			      MLOG_PROC,
			      "raising stack size hard limit "
			      "from 0x%llx to 0x%llx\n",
			      rlimit64.rlim_max,
			      minstacksz );
			rlimit64.rlim_cur = minstacksz;
			rlimit64.rlim_max = minstacksz;
			( void )setrlimit64( RLIMIT_STACK, &rlimit64 );
			rval = getrlimit64( RLIMIT_STACK, &rlimit64 );
			assert( ! rval );
			if ( rlimit64.rlim_cur < minstacksz ) {
				mlog( MLOG_NORMAL
				      |
				      MLOG_WARNING
				      |
				      MLOG_NOLOCK
				      |
				      MLOG_PROC,
				      _("unable to raise stack size hard limit "
				      "from 0x%llx to 0x%llx\n"),
				      rlimit64.rlim_max,
				      minstacksz );
			}
		} else {
			mlog( MLOG_DEBUG
			      |
			      MLOG_NOLOCK
			      |
			      MLOG_PROC,
			      "raising stack size soft limit "
			      "from 0x%llx to 0x%llx\n",
			      rlimit64.rlim_cur,
			      minstacksz );
			rlimit64.rlim_cur = minstacksz;
			( void )setrlimit64( RLIMIT_STACK, &rlimit64 );
			rval = getrlimit64( RLIMIT_STACK, &rlimit64 );
			assert( ! rval );
			if ( rlimit64.rlim_cur < minstacksz ) {
				mlog( MLOG_NORMAL
				      |
				      MLOG_WARNING
				      |
				      MLOG_NOLOCK
				      |
				      MLOG_PROC,
				      _("unable to raise stack size soft limit "
				      "from 0x%llx to 0x%llx\n"),
				      rlimit64.rlim_cur,
				      minstacksz );
			}
		}
	} else if ( rlimit64.rlim_cur > maxstacksz ) {
		mlog( MLOG_DEBUG
		      |
		      MLOG_NOLOCK
		      |
		      MLOG_PROC,
		      "lowering stack size soft limit "
		      "from 0x%llx to 0x%llx\n",
		      rlimit64.rlim_cur,
		      maxstacksz );
		rlimit64.rlim_cur = maxstacksz;
		( void )setrlimit64( RLIMIT_STACK, &rlimit64 );
		rval = getrlimit64( RLIMIT_STACK, &rlimit64 );
		assert( ! rval );
		if ( rlimit64.rlim_cur > maxstacksz ) {
			mlog( MLOG_NORMAL
			      |
			      MLOG_WARNING
			      |
			      MLOG_NOLOCK
			      |
			      MLOG_PROC,
			      _("unable to lower stack size soft limit "
			      "from 0x%llx to 0x%llx\n"),
			      rlimit64.rlim_cur,
			      maxstacksz );
		}
	}
	mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
	      "RLIMIT_STACK new cur 0x%llx max 0x%llx\n",
	      rlimit64.rlim_cur,
	      rlimit64.rlim_max );

	rval = getrlimit64( RLIMIT_DATA, &rlimit64 );
	assert( ! rval );
	mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
	      "RLIMIT_DATA org cur 0x%llx max 0x%llx\n",
	      rlimit64.rlim_cur,
	      rlimit64.rlim_max );
	
	rval = getrlimit64( RLIMIT_FSIZE, &rlimit64 );
	assert( ! rval );
	mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
	      "RLIMIT_FSIZE org cur 0x%llx max 0x%llx\n",
	      rlimit64.rlim_cur,
	      rlimit64.rlim_max );
	rlimit64.rlim_cur = rlimit64.rlim_max;
	( void )setrlimit64( RLIMIT_FSIZE, &rlimit64 );
	rlimit64.rlim_cur = RLIM64_INFINITY;
	( void )setrlimit64( RLIMIT_FSIZE, &rlimit64 );
	rval = getrlimit64( RLIMIT_FSIZE, &rlimit64 );
	assert( ! rval );
	mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
	      "RLIMIT_FSIZE now cur 0x%llx max 0x%llx\n",
	      rlimit64.rlim_cur,
	      rlimit64.rlim_max );
	
	rval = getrlimit64( RLIMIT_CPU, &rlimit64 );
	assert( ! rval );
	mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
	      "RLIMIT_CPU cur 0x%llx max 0x%llx\n",
	      rlimit64.rlim_cur,
	      rlimit64.rlim_max );
	rlimit64.rlim_cur = rlimit64.rlim_max;
	( void )setrlimit64( RLIMIT_CPU, &rlimit64 );
	rval = getrlimit64( RLIMIT_CPU, &rlimit64 );
	assert( ! rval );
	mlog( MLOG_NITTY | MLOG_NOLOCK | MLOG_PROC,
	      "RLIMIT_CPU now cur 0x%llx max 0x%llx\n",
	      rlimit64.rlim_cur,
	      rlimit64.rlim_max );

#ifdef RESTORE
	*vmszp = vmsz;
#endif /* RESTORE */
	return BOOL_TRUE;
}

struct sig_printmap {
	int num;
	char *string;
};

typedef struct sig_printmap sig_printmap_t;

static sig_printmap_t sig_printmap[ ] = {
	{SIGHUP,	"SIGHUP"},
	{SIGINT,	"SIGINT"},
	{SIGQUIT,	"SIGQUIT"},
	{SIGILL,	"SIGILL"},
	{SIGABRT,	"SIGABRT"},
	{SIGFPE,	"SIGFPE"},
	{SIGBUS,	"SIGBUS"},
	{SIGSEGV,	"SIGSEGV"},
#ifdef SIGSYS
	{SIGSYS,	"SIGSYS"},
#endif
	{SIGPIPE,	"SIGPIPE"},
	{SIGALRM,	"SIGALRM"},
	{SIGTERM,	"SIGTERM"},
	{SIGUSR1,	"SIGUSR1"},
	{SIGUSR2,	"SIGUSR2"},
	{SIGCHLD,	"SIGCHLD"},
	{SIGPWR,	"SIGPWR"},
	{SIGURG,	"SIGURG"},
	{SIGPOLL,	"SIGPOLL"},
	{SIGXCPU,	"SIGXCPU"},
	{SIGXFSZ,	"SIGXFSZ"},
#if HIDDEN
	{SIGRTMIN,	"SIGRTMIN"},
	{SIGRTMAX,	"SIGRTMAX"},
#endif
	{0,		"???"}
};

static char *
sig_numstring( int num )
{
	sig_printmap_t *p = sig_printmap;
	sig_printmap_t *endp = sig_printmap
			       +
			       ( sizeof( sig_printmap )
			         /
			         sizeof( sig_printmap[ 0 ] ));
	for ( ; p < endp ; p++ ) {
		if ( p->num == num ) {
			return p->string;
		}
	}

	return "???";
}

static char *
strpbrkquotes( char *p, const char *sep )
{
	bool_t prevcharwasbackslash = BOOL_FALSE;
	bool_t inquotes = BOOL_FALSE;

	for ( ; ; p++ ) {
		if ( *p == 0 ) {
			return 0;
		}

		if ( *p == '\\' ) {
			if ( ! prevcharwasbackslash ) {
				prevcharwasbackslash = BOOL_TRUE;
			} else {
				prevcharwasbackslash = BOOL_FALSE;
			}
			continue;
		}

		if ( *p == '"' ) {
			if ( prevcharwasbackslash ) {
				prevcharwasbackslash = BOOL_FALSE;
				continue;
			}
			if ( inquotes ) {
				inquotes = BOOL_FALSE;
			} else {
				inquotes = BOOL_TRUE;
			}
			continue;
		}

		if ( ! inquotes ) {
			if ( strchr( sep, ( int )( *p ))) {
				return p;
			}
		}

		prevcharwasbackslash = BOOL_FALSE;
	}
	/* NOTREACHED */
}

static char *
stripquotes( char *p )
{
	size_t len = strlen( p );
	char *endp;
	char *nextp;
	bool_t justremovedbackslash;

	if ( len > 2 && p[ 0 ] == '"' ) {
		p++;
		len--;
		if ( len && p[ len - 1 ] == '"' ) {
			p[ len - 1 ] = 0;
			len--;
		}
	}

	endp = p + len;
	justremovedbackslash = BOOL_FALSE;

	for ( nextp = p ; nextp < endp ; ) {
		if ( *nextp == '\\' && ! justremovedbackslash ) {
			shiftleftby1( nextp, endp );
			endp--;
			justremovedbackslash = BOOL_TRUE;
		} else {
			justremovedbackslash = BOOL_FALSE;
			nextp++;
		}
	}

	return p;
}

static void
shiftleftby1( char *p, char *endp )
{
	for ( ; p < endp ; p++ ) {
		*p = p[ 1 ];
	}
}
