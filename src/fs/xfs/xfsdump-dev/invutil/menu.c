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

#include <errno.h>
#include <ncurses.h>
#include <ctype.h>
#include <uuid/uuid.h>

#include "types.h"
#include "cmenu.h"
#include "list.h"
#include "invutil.h"

int
put_all_options(WINDOW *win, node_t *list, node_t *current)
{
    int i;
    int attr;
    data_t *d;
    node_t *node;

    i = 0;
    node = list;
    while(node != NULL) {
	d = ((data_t *)(node->data));
	if(d->hidden == BOOL_TRUE) {
	    node = node->next;
	    continue;
	}
	if(node == current) {
	    attr = A_REVERSE;
        }
        else {
            attr = 0;
        }
        put_option(win, i, d->text, attr);
	node = node->next;
	i++;
    }

    return 0;
}

void
put_helpscreen(int keyc, menukey_t *keyv)
{
    int k;
    int l;
    int winline;
    WINDOW *helpscreen;

    winline = LINES - (keyc + 5);
    if(winline < 0)
	winline = 0;

    helpscreen = newwin(keyc + 4, 29, winline, COLS - 30);
    keypad(helpscreen, TRUE);
    wattron(helpscreen, A_REVERSE);
    wmove(helpscreen, 0, 0);
    waddstr(helpscreen, " --------------------------- ");
    wmove(helpscreen, 1, 0);
    waddstr(helpscreen, "| Menu Help                 |");
    wmove(helpscreen, 2, 0);
    waddstr(helpscreen, "|                           |");
    l = 3;
    for(k = 0; k < keyc + 1; k++) {
	if(keyv[k].text == NULL)
	    continue;
	wmove(helpscreen, l, 0);
	waddstr(helpscreen, keyv[k].text);
	l++;
    }
    wmove(helpscreen, l, 0);
    waddstr(helpscreen, " --------------------------- ");
    wrefresh(helpscreen);
    wattroff(helpscreen, A_REVERSE);
    hitanykey();
    wclear(helpscreen);
    wrefresh(helpscreen);
    delwin(helpscreen);
}

int
menu(WINDOW *win, int line, int col, node_t *list, int keyc, menukey_t *keyv)
{
    int c;
    int k;
    int quit;
    char txt[256];
    node_t *current;
    node_t *last;
    data_t *d;
    int current_line;
    int last_line;
    int size;

    current = list; 
    last = current;
    current_line = 0;
    last_line = current_line;

    redraw_screen = BOOL_TRUE;

    quit = BOOL_FALSE;
    while(quit != BOOL_TRUE) {
	if(current == NULL && current->data == NULL) {
	    current = list;
	}
	d = (data_t *)(current->data);

	if(redraw_screen == BOOL_TRUE) {
	    wclear(win);
	    snprintf(txt, sizeof(txt), "%s: %s", g_programName, g_programVersion);
	    put_header(txt, ALIGN_LEFT);
	    if(d->hidden == BOOL_TRUE) {
		current = list;
		current_line = 0;
		d = (data_t *)(current->data);
	    }
	    put_all_options(win, list, current);
	    redraw_screen = BOOL_FALSE;

	    if(d->ops != NULL && d->ops->op_highlight != NULL) {
		quit = d->ops->op_highlight(win, current, list);
	    }
	}
	else if (last != current || redraw_options) {
	    put_option(win, last_line, ((data_t *)(last->data))->text, 0);
	    put_option(win, current_line, d->text, A_REVERSE);

	    redraw_options = BOOL_FALSE;

	    if(((data_t *)(last->data))->ops != NULL
	       && ((data_t *)(last->data))->ops->op_unhighlight != NULL) {
		quit = ((data_t *)(last->data))->ops->op_unhighlight(win, current, list);
	    }
	    if(d->ops != NULL && d->ops->op_highlight != NULL) {
		quit = d->ops->op_highlight(win, current, list);
	    }
	}

	last = current;
	last_line = current_line;

	wrefresh(stdscr);
	wrefresh(infowin);
	size = (LINES - 2) - INFO_SIZE;
	prefresh(win,
		 current_line - (size - 1), 0,
		 line, col,
		 size, COLS - 1);

	if(quit == BOOL_TRUE)
	    continue;

	c=wgetch(win);
	switch(c) {
        case '?':
	case KEY_F(1):
	    /* online help */
	    put_helpscreen(keyc, keyv);
	    redraw_screen = BOOL_TRUE;
	    break;
	case KEY_UP:
	case 'k':
	    /* go to previous option */
	    {
		data_t *d;
		node_t *n = current->prev;

		while(n != NULL) {
		    d = ((data_t *)(n->data)); 
		    if(d == NULL) {
			n = n->prev;
			continue;
		    }
		    if(d->hidden == BOOL_FALSE) {
			break;
		    }
		    n = n->prev;
		}
		if(n != NULL) {
		    current = n;
		    current_line--;
		}
	    }
	    break;
	case KEY_DOWN:
	case 'j':
	    /* go to next option */
	    {
		data_t *d;
		node_t *n = current->next;

		while(n != NULL) {
		    d = ((data_t *)(n->data)); 
		    if(d == NULL) {
			n = n->next;
			continue;
		    }
		    if(d->hidden == BOOL_FALSE) {
			break;
		    }
		    n = n->next;
		}
		if(n != NULL) {
		    current = n;
		    current_line++;
		}
	    }
	    break;
	case KEY_RIGHT:
	case 'l':
	    /* go to next child */
	    {
		node_t *n = current->next;
		int l;

		if(current != NULL && ((data_t *)(current->data)) == NULL) {
		    break;
		}
		l = ((data_t *)(current->data))->level;
		if(n != NULL && ((data_t *)(n->data)) != NULL && ((data_t *)(n->data))->level > l) {
		    if(((data_t *)(n->data))->hidden == BOOL_TRUE) {
			menu_expand(win, current, list);
		    }
		    current = n;
		    current_line++;
		}
	    }
	    break;
	case KEY_LEFT:
	case 'h':
	    /* go to parent */
	    {
		data_t *d;
		node_t *n = current->prev;
		int l = 0;
		int i;

		if(current != NULL && ((data_t *)(current->data)) != NULL) {
		    l = ((data_t *)(current->data))->level;
		}
		i = current_line - 1;
		while(n != NULL) {
		    d = ((data_t *)(n->data)); 
		    if(d == NULL) {
			n = n->prev;
			continue;
		    }
		    if(d->hidden == BOOL_TRUE) {
			n = n->prev;
			continue;
		    }
		    if(d->level < l) {
			break;
		    }
		    n = n->prev;
		    i--;
		}
		if(n != NULL) {
		    current = n;
		    current_line = i;;
		}
	    }
	    break;
	case ERR:
	    if(errno == EINTR) {
		/* mainmenu window has been recreated - use new one */
		win = mainmenu;
	    }
	    break;
	default:
	    for(k = 0; k < keyc; k++) {
		if(c == keyv[k].key || tolower(c) == keyv[k].key) {
		    if (keyv[k].fn != NULL) {
			quit = keyv[k].fn(win, current, list);
			break;
		    }
		    else {
			put_footer("internal error: can't execute function", ALIGN_CENTRE);
			break;
		    }
		}
	    }
	    break;
	}
    }

    return 0;
}
