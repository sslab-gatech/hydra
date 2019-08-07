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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <string.h>

#include "config.h"

#include "types.h"
#include "mlog.h"
#include "dlog.h"
#include "getopt.h"

static int dlog_ttyfd = -1;
static volatile bool_t dlog_allowed_flag = BOOL_FALSE;
static bool_t dlog_timeouts_flag = BOOL_FALSE;
static char *promptstr = " -> ";

static sigset_t dlog_registered_sigs;

static bool_t promptinput( char *buf,
			   size_t bufsz,
			   ix_t *exceptionixp,
			   time32_t timeout,
			   ix_t timeoutix,
			   ix_t sigintix,
			   ix_t sighupix,
			   ix_t sigquitix,
			   char *fmt, ... );
static void dlog_string_query_print( void *ctxp, char *fmt, ... );

bool_t
dlog_init( int argc, char *argv[ ] )
{
	int c;

	/* can only call once
	 */
	assert( dlog_ttyfd == -1 );

	/* initially allow dialog, use stdin fd
	 */
	dlog_ttyfd = 0; /* stdin */
	dlog_allowed_flag = BOOL_TRUE;
	dlog_timeouts_flag = BOOL_TRUE;

	sigemptyset( &dlog_registered_sigs );

	/* look for command line option claiming the operator knows
	 * what's up.
	 */
	optind = 1;
	opterr = 0;
	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
		case GETOPT_FORCE:
			dlog_ttyfd = -1;
			dlog_allowed_flag = BOOL_FALSE;
			break;
		case GETOPT_NOTIMEOUTS:
			dlog_timeouts_flag = BOOL_FALSE;
			break;
		}
	}
#ifdef RESTORE
	/* look to see if restore source coming in on
	 * stdin. If so , try to open /dev/tty for dialogs.
	 */
	if ( optind < argc && ! strcmp( argv[ optind ], "-" )) {
		dlog_ttyfd = open( "/dev/tty", O_RDWR );
		if ( dlog_ttyfd < 0 ) {
			perror("/dev/tty");
			dlog_ttyfd = -1;
			dlog_allowed_flag = BOOL_FALSE;
		}
	}
#endif /* RESTORE */

#ifdef CHKSTDIN
	/* if stdin is not a tty, don't allow dialogs
	 */
	if ( dlog_allowed_flag ) {
		struct stat statbuf;
		int rval;

		assert( dlog_ttyfd >= 0 );
		rval = fstat( dlog_ttyfd, &statbuf );
		if ( rval ) {
			mlog( MLOG_VERBOSE | MLOG_WARNING,
			      _("could not fstat stdin (fd %d): %s (%d)\n"),
			      dlog_ttyfd,
			      strerror( errno ),
			      errno );
		} else {
			mlog( MLOG_DEBUG,
			      "stdin mode 0x%x\n",
			      statbuf.st_mode );
		}
	}
#endif /* CHKSTDIN */

	return BOOL_TRUE;
}

bool_t
dlog_allowed( void )
{
	return dlog_allowed_flag;
}

void
dlog_desist( void )
{
	dlog_allowed_flag = BOOL_FALSE;
}

int
dlog_fd( void )
{
	return dlog_ttyfd;
}

void
dlog_begin( char *preamblestr[ ], size_t preamblecnt )
{
	size_t ix;

	mlog_lock( );
	for ( ix = 0 ; ix < preamblecnt ; ix++ ) {
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      preamblestr[ ix ] );
	}
}

void
dlog_end( char *postamblestr[ ], size_t postamblecnt )
{
	size_t ix;

	for ( ix = 0 ; ix < postamblecnt ; ix++ ) {
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      postamblestr[ ix ] );
	}
	mlog_unlock( );
}

ix_t
dlog_multi_query( char *querystr[ ],
		  size_t querycnt,
		  char *choicestr[ ],
		  size_t choicecnt,
		  char *hilitestr,
		  size_t hiliteix,
		  char *defaultstr,
		  ix_t defaultix,
		  time32_t timeout,
		  ix_t timeoutix,
		  ix_t sigintix,
		  ix_t sighupix,
		  ix_t sigquitix )
{
	size_t ix;
	char buf[ 100 ];
	char *prepromptstr;

	/* sanity
	 */
	assert( dlog_allowed_flag );

	/* display query description strings
	 */
	for ( ix = 0 ; ix < querycnt ; ix++ ) {
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      querystr[ ix ] );
	}

	/* display the choices: NOTE: display is 1-based, code intfs 0-based!
	 */
	for ( ix = 0 ; ix < choicecnt ; ix++ ) {
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      "%u: %s",
		      ix + 1,
		      choicestr[ ix ] );
		if ( ix == hiliteix ) {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      "%s",
			      hilitestr ?  hilitestr : " *" );
		}
		if ( ix == defaultix ) {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      "%s",
			      defaultstr ?  defaultstr : _(" (default)") );
		}
		if ( dlog_timeouts_flag
		     &&
		     timeoutix != IXMAX
		     &&
		     ix == timeoutix ) {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      _(" (timeout in %u sec)"),
			      timeout );
		}
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      "\n" );
	}

	/* read the tty until we get a proper answer or are interrupted
	 */
	prepromptstr = "";
	for ( ; ; ) {
		ix_t exceptionix;
		bool_t ok;

		/* prompt and accept input
		 */
		ok = promptinput( buf,
				  sizeof( buf ),
				  &exceptionix,
				  timeout,
				  timeoutix,
				  sigintix,
				  sighupix,
				  sigquitix,
				  prepromptstr,
				  choicecnt );
		if ( ok ) {
			long int val;
			char *end = buf;

			if ( ! strlen( buf )) {
				return defaultix;
			}

			val = strtol( buf, &end, 10 );
			if ( *end != '\0' || val < 1 || val > choicecnt ) {
				prepromptstr = _(
				      "please enter a value "
				      "between 1 and %d inclusive ");
				continue;
			}
			return val - 1; // return value is a 0-based index
		} else {
			return exceptionix;
		}
	}
	/* NOTREACHED */
}

void
dlog_multi_ack( char *ackstr[ ], size_t ackcnt )
{
	size_t ix;

	for ( ix = 0 ; ix < ackcnt ; ix++ ) {
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      ackstr[ ix ] );
	}
}

ix_t
dlog_string_query( dlog_ucbp_t ucb, /* user's print func */
		   void *uctxp,	  /* user's context for above */
		   char *bufp,	  /* typed string returned in */
		   size_t bufsz,	  /* buffer size */
		   time32_t timeout,  /* secs b4 giving up */
		   ix_t timeoutix,
		   ix_t sigintix,
		   ix_t sighupix,
		   ix_t sigquitix,
		   ix_t okix )
{
	ix_t exceptionix;
	bool_t ok;

	/* sanity
	 */
	assert( dlog_allowed_flag );

	/* call the caller's callback with his context, print context, and
	 * print operator
	 */
	( * ucb )( uctxp, dlog_string_query_print, 0 );

	/* if called for, print the timeout and a newline.
	 * if not, print just a newline
	 */
	if ( dlog_timeouts_flag && timeoutix != IXMAX ) {
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      _(" (timeout in %u sec)\n"),
		      timeout );
	} else {
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      "\n" );
	}

	/* prompt and accept input
	 */
	ok = promptinput( bufp,
			  bufsz,
			  &exceptionix,
			  timeout,
			  timeoutix,
			  sigintix,
			  sighupix,
			  sigquitix,
			  "" );
	if ( ok ) {
		return okix;
	} else {
		return exceptionix;
	}
}

void
dlog_string_ack( char *ackstr[ ], size_t ackcnt )
{
	dlog_multi_ack( ackstr, ackcnt );
}

/* ok that this is a static, since used under mutual exclusion lock
 */
static volatile int dlog_signo_received;

bool_t
dlog_sighandler( int signo )
{
	if ( sigismember( &dlog_registered_sigs, signo ) < 1 )
		return BOOL_FALSE;
	// only process the first signal
	sigemptyset( &dlog_registered_sigs );
	dlog_signo_received = signo;
	return BOOL_TRUE;
}

/* ARGSUSED */
static void
dlog_string_query_print( void *ctxp, char *fmt, ... )
{
	va_list args;

	assert( ! ctxp );

	va_start( args, fmt );
	mlog_va( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE, fmt, args );
	va_end( args );
}

static bool_t
promptinput( char *buf,
	     size_t bufsz,
	     ix_t *exceptionixp,
	     time32_t timeout,
	     ix_t timeoutix,
	     ix_t sigintix,
	     ix_t sighupix,
	     ix_t sigquitix,
	     char *fmt,
	     ... )
{
	va_list args;
	time32_t now = time( NULL );
	int nread = -1;
	sigset_t orig_set;
	char *bufp = buf;

	/* display the pre-prompt
	 */
	va_start( args, fmt );
	mlog_va( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE, fmt, args );
	va_end( args );

	/* display standard prompt
	 */
#ifdef NOTYET
	if ( dlog_timeouts_flag && timeoutix != IXMAX ) {
		mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
		      _("(timeout in %d sec)"),
		      timeout );
	}
#endif /* NOTYET */
	mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE, promptstr );

	/* set up timeout
	 */
	if ( dlog_timeouts_flag && timeoutix != IXMAX ) {
		timeout += now;
	} else {
		timeout = TIMEMAX;
	}

	/* set up signal handling
	 * the mlog lock is held for the life of the dialog (see dlog_begin)
	 * and it's possible the main thread, which normally does the signal
	 * handling, is now waiting on the mlog lock trying to log a message.
	 * so unblock the relevant signals for this thread. note this means
	 * the current thread or the main thread might handle one of these
	 * signals.
	 */
	dlog_signo_received = -1;
	sigemptyset( &dlog_registered_sigs );
	if ( sigintix != IXMAX ) {
		sigaddset( &dlog_registered_sigs, SIGINT );
	}
	if ( sighupix != IXMAX ) {
		sigaddset( &dlog_registered_sigs, SIGHUP );
		sigaddset( &dlog_registered_sigs, SIGTERM );
	}
	if ( sigquitix != IXMAX ) {
		sigaddset( &dlog_registered_sigs, SIGQUIT );
	}

	pthread_sigmask( SIG_UNBLOCK, &dlog_registered_sigs, &orig_set );

	/* wait for input, timeout, or interrupt.
	 * note we come out of the select() frequently in order to
	 * check for a signal. the signal may have been handled by the
	 * the main thread, so we can't rely on the signal waking us
	 * up from the select().
	 */
	while ( now < timeout && dlog_signo_received == -1 && dlog_allowed_flag ) {
		int rc;
		fd_set rfds;
		struct timeval tv = { 0, 100000 }; // 100 ms

		FD_ZERO( &rfds );
		FD_SET( dlog_ttyfd, &rfds );

		rc = select( dlog_ttyfd + 1, &rfds, NULL, NULL, &tv );
		if ( rc > 0 && FD_ISSET( dlog_ttyfd, &rfds ) ) {
			nread = read( dlog_ttyfd, bufp, bufsz );
			if ( nread < 0 ) {
				break; // error handled below
			} else if ( nread == 0 && buf == bufp ) {
				mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE, "\n" );
				*bufp = 0;
				break; // no input, return an empty string
			} else if ( nread > 0 && bufp[nread-1] == '\n' ) {
				// received a full line, chomp the newline
				bufp[nread-1] = 0;
				break;
			} else if ( nread == bufsz ) {
				// no more room, truncate and return
				bufp[nread-1] = 0;
				break;
			}

			// keep waiting for a full line of input
			bufp += nread;
			bufsz -= nread;
			nread = -1;
		}
		now = time( NULL );
	}

	/* restore signal handling
	 */
	pthread_sigmask( SIG_SETMASK, &orig_set, NULL );
	sigemptyset( &dlog_registered_sigs );
	
	/* check for timeout or interrupt
	 */
	if ( nread < 0 ) {
		if ( now >= timeout ) {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      _("timeout\n") );
			*exceptionixp = timeoutix;
		} else if ( dlog_signo_received == SIGINT ) {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      _("keyboard interrupt\n") );
			mlog_exit_hint(RV_KBD_INTR);
			*exceptionixp = sigintix;
		} else if ( dlog_signo_received == SIGHUP ) {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      _("hangup\n") );
			*exceptionixp = sighupix;
		} else if ( dlog_signo_received == SIGTERM ) {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      _("terminate\n") );
			*exceptionixp = sighupix;
		} else if ( dlog_signo_received == SIGQUIT ) {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      _("keyboard quit\n") );
			mlog_exit_hint(RV_KBD_INTR);
			*exceptionixp = sigquitix;
		} else {
			mlog( MLOG_NORMAL | MLOG_NOLOCK | MLOG_BARE,
			      _("abnormal dialog termination\n"));
			*exceptionixp = sigquitix;
		}
		return BOOL_FALSE;
	} else {
		assert( dlog_signo_received == -1 );
		*exceptionixp = 0;
		return BOOL_TRUE;
	}
}
