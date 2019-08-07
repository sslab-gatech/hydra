/*
 * Copyright (c) 2000-2001 Silicon Graphics, Inc.; provided copyright in
 * certain portions may be held by third parties as indicated herein.
 * All Rights Reserved.
 *
 * The code in this source file represents an aggregation of work from
 * Georgia Tech, Fred Fish, Jeff Lee, Arnold Robbins and other Silicon
 * Graphics engineers over the period 1985-2000.
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "rmtlib.h"

#define RMT_DEBUG_FILE "/tmp/librmt_debug" /* file for debug output on server */

static int _rmt_open(char *, int, int);

int _rmt_Ctp[MAXUNIT][2] = { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} };
int _rmt_Ptc[MAXUNIT][2] = { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} };
int _rmt_host[MAXUNIT] = { -1, -1, -1, -1};

struct uname_table
{
    int id;
    char *name;
};

struct uname_table uname_table[] =
{ {UNAME_LINUX, "Linux"}, {UNAME_IRIX, "IRIX"}, {0,0} };


/*
 *	Open a local or remote file.  Looks just like open(2) to
 *	caller.
 */

int rmtopen (path, oflag, mode)
char *path;
int oflag;
int mode;
{
	if (strchr (path, ':') != NULL)
	{
		return (_rmt_open (path, oflag, mode) | REM_BIAS);
	}
	else
	{
		return (open (path, oflag, mode));
	}
}

/*
 *	_rmt_open --- open a magtape device (or file) on system specified,
 *                    as given user
 *
 *	file name has the form system[.user]:????
 */

#define MAXHOSTLEN	257
#define MAXDBGPATH	100

/* ARGSUSED */
static int _rmt_open (char *path, int oflag, int mode)
{
	int i, rc;
	char buffer[BUFMAGIC];
	char system[MAXHOSTLEN];
	char device[BUFMAGIC];
	char login[BUFMAGIC];
	char *sys, *dev, *user;
        char *rsh_path;
        char *rmt_path;
	char rmt_cmd[MAXDBGPATH];


	sys = system;
	dev = device;
	user = login;

	if ((rsh_path = getenv("RSH")) == NULL) {
	    rsh_path = RSH_PATH;
	}

	if ((rmt_path = getenv("RMT")) == NULL) {
	    rmt_path = RMT_PATH;
	}


/*
 *	first, find an open pair of file descriptors
 */

	for (i = 0; i < MAXUNIT; i++)
		if (READ(i) == -1 && WRITE(i) == -1)
			break;

	if (i == MAXUNIT)
	{
		setoserror( EMFILE );
		return(-1);
	}

/*
 *	pull apart system and device, and optional user
 *	don't munge original string
 */
	while (*path != '@' && *path != ':') {
		*user++ = *path++;
	}
	*user = '\0';
	path++;

	if (*(path - 1) == '@')
	{
		while (*path != ':') {
			*sys++ = *path++;
		}
		*sys = '\0';
		path++;
	}
	else
	{
		for ( user = login; (*sys = *user); user++, sys++ )
			;
		user = login;
	}

	while (*path) {
		*dev++ = *path++;
	}
	*dev = '\0';

	_rmt_turnonmsgsbyenv();

        /* try to find out the uname of the remote host */
	{
#define MAX_UNAMECMD MAXHOSTLEN+40
#define MAX_UNAME 20
	    FILE *rmt_f;
	    char cmd[MAX_UNAMECMD];
	    char uname[MAX_UNAME];
            struct uname_table *p;

	    if (user != login) {
		snprintf(cmd, sizeof(cmd), "%s -l %s %s uname", rsh_path, login, system);
	    }
	    else {
		snprintf(cmd, sizeof(cmd), "%s %s uname", rsh_path, system);
	    }

	    rmt_f = popen(cmd, "r");
	    if (rmt_f == NULL) {
		_rmt_msg(RMTWARN, _(
		"rmtopen: failed to detect remote host type using \"%s\"\n"),
			cmd);
		RMTHOST(i) = UNAME_UNDEFINED;
		goto do_rmt;
	    }
	    else {
		int len;
		char *c  = fgets(uname, sizeof(uname), rmt_f);
	        pclose(rmt_f);

		if (c == NULL) {
		    _rmt_msg(RMTWARN, _(
		"rmtopen: failed to detect remote host type reading \"%s\"\n"),
			cmd);
		    RMTHOST(i) = UNAME_UNDEFINED;
		    goto do_rmt;
		}
		len = strlen(uname);
		if (len > 0 && uname[len-1] == '\n')
		    uname[len-1] = '\0'; /* chomp the '\n' */
	    }

	    for(p = &uname_table[0]; p->name != 0; p++) {
		if (strncmp(p->name, uname, strlen(p->name)) == 0)
		    break;
	    }
	    if (p->name == 0) {
		RMTHOST(i) = UNAME_UNKNOWN;
		_rmt_msg(RMTWARN, _(
		"rmtopen: remote host type, \"%s\", is unknown to librmt\n"),
			uname);
	    }
	    else {
		RMTHOST(i) = p->id;
		_rmt_msg(RMTDBG, "rmtopen: RMTHOST(%d) = %s\n", i, p->name);
	    }
	}


/*
 *	setup the pipes for the 'rsh' command and fork
 */
do_rmt:
	if (pipe(_rmt_Ptc[i]) == -1 || pipe(_rmt_Ctp[i]) == -1)
		return(-1);

	if ((rc = fork()) == -1)
		return(-1);

	if (rc == 0)
	{
		close(0);
		dup(_rmt_Ptc[i][0]);
		close(_rmt_Ptc[i][0]); close(_rmt_Ptc[i][1]);
		close(1);
		dup(_rmt_Ctp[i][1]);
		close(_rmt_Ctp[i][0]); close(_rmt_Ctp[i][1]);
		(void) setuid (getuid ());
		(void) setgid (getgid ());
		if (_rmt_msgson() == RMTDBG) {
		    snprintf(rmt_cmd, sizeof(rmt_cmd), "%s %s.%d",
				rmt_path, RMT_DEBUG_FILE, getpid());
		}
		else {
		    strncpy(rmt_cmd, rmt_path, sizeof(rmt_cmd));
		}
		if (user != login)
		{
			execl(rsh_path, "rsh", system, "-l", login,
				rmt_cmd, (char *) 0);
		}
		else
		{
			execl(rsh_path, "rsh", system,
				rmt_cmd, (char *) 0);
		}

/*
 *	bad problems if we get here
 */
		fprintf(stderr,
		    "librmt: problem finding either RSH(%s) or RMT(%s): %s\n",
		    rsh_path, rmt_path, strerror(errno));
		exit(1);
	}

	close(_rmt_Ptc[i][0]); close(_rmt_Ctp[i][1]);

/*
 *	now attempt to open the tape device
 */

	sprintf(buffer, "O%s\n%d\n", device, oflag);
	if (_rmt_command(i, buffer) == -1 || _rmt_status(i) == -1)
		return(-1);

	return(i);
}
