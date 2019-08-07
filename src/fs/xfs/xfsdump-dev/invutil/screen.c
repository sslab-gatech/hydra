/*
 * Copyright (c) 2002 Silicon Graphics, Inc.
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

#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <uuid/uuid.h>

#include "cmenu.h"

int
put_line(WINDOW *win, int line, char *msg, int attr, alignment_t alignment)
{
    int c;
    int cols;
    int msglen;
    static char text[256];

    cols = getmaxx(win);
    cols--;
    if(cols > 255) {
	cols = 255;
    }
    msglen = strlen(msg);
    if(msglen > cols)
	msglen = cols;

    switch(alignment) {
    case ALIGN_CENTRE:
	c = (cols - msglen) / 2;
	break;
    case ALIGN_RIGHT:
	c = (cols - msglen);
	break;
    case ALIGN_LEFT:
    default:
	c = 0;
	break;
    }

    snprintf(text, sizeof(text), "%*s%.*s%*s", c, "", msglen, msg, cols - msglen - c, "");

    wmove(win, line, 0);
    wattron(win, attr);
    waddstr(win, text);
    wattroff(win, attr);

    return 0;
}

void
hitanykey()
{
    put_footer("Hit any key to continue...", ALIGN_CENTRE);
    wgetch(stdscr);
    clear_line(stdscr, LINES - 1);
}

/*ARGSUSED*/
int
get_string(WINDOW *win, char *prompt, char *response, int len)
{
    put_line(stdscr, LINES - 1, prompt, 0, ALIGN_LEFT);
    wmove(stdscr, LINES - 1, strlen(prompt));
    echo();
    wgetnstr(stdscr, response, len);
    noecho();
    
    return 0;
}
