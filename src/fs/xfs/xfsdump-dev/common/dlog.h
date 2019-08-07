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
#ifndef DLOG_H
#define DLOG_H

/* dlog.[hc] - operator dialog abstraction
 *
 * abstracts dialogs with the operator
 */

extern bool_t dlog_init( int argc, char *argv[ ] );


/* tells if dialogs are allowed;
 * will be false if invoked to right of unnamed pipe,
 * or if pipe to left breaks.
 */
extern bool_t dlog_allowed( void );


/* allows signal handler to notify dlog of broken write pipe
 */
extern void dlog_desist( void );

/* returns the dialog tty file descriptor. returns -1 if none
 */
extern int dlog_fd( void );

/* returns BOOL_TRUE if a dialog consumed the given signal
 */
extern bool_t dlog_sighandler( int signo );

/* bracket a dialog session
 */
extern void dlog_begin( char *preamblestr[ ], size_t preamblecnt );
extern void dlog_end( char *postamblestr[ ], size_t postamblecnt );


/* multiple choice question abstraction. if any exception event index
 * set to IXMAX, that event will be ignored. returns index of selected
 * choice, or exception index if exception occured.
 */
extern ix_t dlog_multi_query( char *querystr[ ],	/* pre-choices output */
			      size_t querycnt,		/* length of above */
			      char *choicestr[ ],	/* choices */
			      size_t choicecnt,		/* length of above */
			      char *hilitestr,		/* to distinguish */
			      ix_t hiliteix,		/* highlighted choice */
			      char *defaultstr,		/* to distinguish */
			      ix_t defaultix,		/* return if cr */
			      time32_t timeout,		/* secs b4 giving up */
			      ix_t timeoutix,		/* return if timeout */
			      ix_t sigintix,		/* return if SIGINT */
			      ix_t sighupix,		/* return if SIGHUP */
			      ix_t sigquitix );		/* return if SIGQUIT */
extern void dlog_multi_ack( char *ackstr[ ], size_t ackcnt );

/* call the caller's callback to display whatever, using provided print
 * function, then prompt for and return an arbitrary string. two types
 * defined: pointer to the caller's function to print, and pointer to
 * function used by that function to do output. returns okix if successful,
 * timeoutix if times out, sigintix if SIGINT recieved, sighupix if SIGHUP
 * received, sigquitix if SIGQUIT received. if any of the exception indices
 * are set to IXMAX by the caller, those events will be ignored.
 */
typedef void ( * dlog_pcbp_t )( void *pctxp, char *s, ... );
typedef void ( * dlog_ucbp_t )( void *uctxp, dlog_pcbp_t pcb, void *pctxp );
extern ix_t dlog_string_query( dlog_ucbp_t ucb, /* user's print func */
			       void *uctxp,	/* user's context for above */
			       char *bufp,	/* typed string returned in */
			       size_t bufsz,	/* buffer size */
			       time32_t timeout,  /* secs b4 giving up */
			       ix_t timeoutix,  /* return if timeout */
			       ix_t sigintix,	/* return if SIGINT */
			       ix_t sighupix,	/* return if SIGHUP */
			       ix_t sigquitix,  /* return if SIGQUIT */
			       ix_t okix );     /* return if successful */
extern void dlog_string_ack( char *ackstr[ ], size_t ackcnt );

#endif /* DLOG_H */
