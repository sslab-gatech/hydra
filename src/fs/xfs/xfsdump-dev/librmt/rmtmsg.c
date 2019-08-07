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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "rmtlib.h"

/*
 * Provide functions for warning and debug messages.
 */

static int rmt_debug_code = 0;

static void
checkcode(void)
{
    if (rmt_debug_code != RMTWARN &&
	rmt_debug_code != RMTDBG) {
	rmt_debug_code = 0;
    }
}

void
_rmt_turnonmsgsbyenv(void)
{
    char *rmt_debug_str = getenv("RMTDEBUG");

    if (rmt_debug_str != NULL) {
        rmt_debug_code = atoi(rmt_debug_str);
	checkcode();
    }
}

void
rmt_turnonmsgs(int code)
{
    rmt_debug_code = code;
    checkcode();
}

int
_rmt_msgson(void)
{
    return rmt_debug_code;
}

#define RMT_MAX_MSG_STR 256

void
_rmt_msg(int level, const char *msg, ...)
{
    static char msg_str[RMT_MAX_MSG_STR];
    va_list     arg;

    if (rmt_debug_code >= level) {
	va_start(arg, msg);
	vsprintf(msg_str, msg, arg);
	va_end(arg);

	fprintf(stderr, "%s%s", level==RMTWARN ? _("WARNING: "):"", msg_str);
    }
}
