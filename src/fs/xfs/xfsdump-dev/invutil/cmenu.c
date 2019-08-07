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

#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <uuid/uuid.h>

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"
#include "getopt.h"

#include "invutil.h"
#include "cmenu.h"
#include "list.h"
#include "fstab.h"
#include "invidx.h"
#include "stobj.h"

WINDOW	*mainmenu;
WINDOW	*infowin;

menukey_t keyv[] = {
    {'+',	"| +:       Expand           |",	menu_expand	},
    {'-',	"| -:       Collapse         |",	menu_collapse	},
    {'*',	"| *:       Expand Tree      |",	menu_expandall	},
    {'%',	"| %:       Collapse Tree    |",	menu_collapseall},
    {'d',	"| d:       Delete           |",	menu_delete	},
    {'u',	"| u:       Undelete         |",	menu_undelete	},
    {'i',	"| i:       Import Inventory |",	menu_import	},
    {'x',	"| x:       Commit & Exit    |",	menu_commit	},
    {'q',	"| q:       Quit             |",	menu_quit	},
    {' ',	"| <space>: Select           |",	menu_select	},
    {KEY_ENTER,	NULL,					menu_select	},
};

/*ARGSUSED*/
void
signal_handler(int s)
{
    switch(s) {
    case SIGWINCH:
	signal(SIGWINCH, signal_handler);
	delwin(mainmenu);
	delwin(infowin);
	endwin();
	create_windows();
	redraw_screen = BOOL_TRUE;
	break;
    }
}

/*ARGSUSED*/
int
menu_quit(WINDOW *win, node_t *current, node_t *list)
{
    return BOOL_TRUE;
}

/*ARGSUSED*/
int
menu_commit(WINDOW *win, node_t *current, node_t *list)
{
    node_t *n;
    node_t *next;
    data_t *d;

    n = list;
    while(n != NULL && n->data != NULL) {
	d = n->data;
	if(d->ops != NULL && d->ops->op_commit != NULL && d->commited == BOOL_FALSE) {
	    d->ops->op_commit(win, n, list);
	    d->commited = BOOL_TRUE;
	}
	n = n->next;
    }

    n = list;
    while(n != NULL) {
	next = n->next;
	node_free(list_del(n));
	n = next;
    }

    return BOOL_TRUE;
}

node_t *
get_lastnode(node_t *node)
{
    while(node->next != NULL) {
	node = node->next;
    }

    return node;
}

/*ARGSUSED*/
int
menu_import(WINDOW *win, node_t *current, node_t *list)
{
    char inv_path[MAXPATHLEN];
    struct stat s;
    char *fstabname;
    data_t *d;

    for(;;) {
	inv_path[0] = '\0';

	if(get_string(win, "Path to inventory to be imported: ", inv_path, MAXPATHLEN) == ERR) {
	    put_error("Error: invalid input");
	    continue;
	}
	if(strlen(inv_path) == 0) {
	    clear_line(stdscr, LINES - 1);
	    return BOOL_FALSE;
	}
	if(stat(inv_path, &s) < 0 || !S_ISDIR(s.st_mode)) {
	    put_error("Error: invalid path");
	    continue;
	}
	clear_line(stdscr, LINES - 1);

	fstabname = GetFstabFullPath(inv_path);
	if(fstabname == NULL) {
	    put_footer("internal memory error: import inventory", ALIGN_LEFT);
	    exit(1);
	}

	while(current->next != NULL) {
	    current = current->next;
	}
	generate_fstab_menu(inv_path, current, 0, fstabname);
	free(fstabname);

	while(current->next != NULL) {
	    current = current->next;
	    d = current->data;
	    d->text[1] = 'I';
	    d->imported = BOOL_TRUE;
	}
	redraw_screen = BOOL_TRUE;
	break;
    }

    return BOOL_FALSE;
}

/*ARGSUSED*/
int
menu_unhighlight(WINDOW *win, node_t *current, node_t *list)
{
    wclear(infowin);
    put_info_header("");
    return BOOL_FALSE;
}

int
menu_select(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return BOOL_FALSE;
    }
    d = (data_t *)(current->data);

    if(d->ops != NULL && d->ops->op_select != NULL) {
	return d->ops->op_select(win, current, list);
    }

    return BOOL_FALSE;
}


node_t *
expand_node(node_t *node)
{
    int i;
    data_t *d;

    if(node == NULL || node->data == NULL) {
	return NULL;
    }
    d = node->data;

    d->expanded = BOOL_TRUE;

    for(i = 0; i < d->nbr_children; i++) {
	((data_t *)(d->children[i]->data))->hidden = BOOL_FALSE;
    }

    return node;
}

int
menu_expand(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return BOOL_FALSE;
    }
    d = current->data;

    if(d->ops != NULL && d->ops->op_expand != NULL) {
	return d->ops->op_expand(win, current, list);
    }

    expand_node(current);

    redraw_screen = BOOL_TRUE;

    return BOOL_FALSE;
}

node_t *
expand_tree(node_t *node)
{
    int i;
    data_t *d;

    if(node == NULL || node->data == NULL) {
	return NULL;
    }
    d = node->data;

    d->hidden = BOOL_FALSE;
    d->expanded = BOOL_TRUE;

    for(i = 0; i < d->nbr_children; i++) {
	expand_tree(d->children[i]);
    }

    return node;
}

int
menu_expandall(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return BOOL_FALSE;
    }
    d = (data_t *)(current->data);

    if(d->ops != NULL && d->ops->op_expandall != NULL) {
	return d->ops->op_expandall(win, current, list);
    }

    expand_tree(current);

    redraw_screen = BOOL_TRUE;

    return BOOL_FALSE;
}

node_t *
collapse_node(node_t *node)
{
    int i;
    data_t *d;

    if(node == NULL || node->data == NULL) {
	return NULL;
    }
    d = node->data;

    if(d->expanded == BOOL_FALSE)
	return node;

    d->expanded = BOOL_FALSE;

    for(i = 0; i < d->nbr_children; i++) {
	((data_t *)(d->children[i]->data))->hidden = BOOL_TRUE;
	collapse_node(d->children[i]);
    }

    return node;
}

int
menu_collapse(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return BOOL_FALSE;
    }
    d = current->data;

    if(d->ops != NULL && d->ops->op_collapse != NULL) {
	return d->ops->op_collapse(win, current, list);
    }

    collapse_node(current);

    redraw_screen = BOOL_TRUE;

    return BOOL_FALSE;
}

int
menu_collapseall(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;
    node_t *n;

    if(current == NULL || current->data == NULL) {
	return BOOL_FALSE;
    }
    d = current->data;

    if(d->ops != NULL && d->ops->op_collapseall != NULL) {
	return d->ops->op_collapseall(win, current, list);
    }

    n = current;
    while(d->parent != NULL) {
	n = d->parent;
	d = n->data;
    }

    collapse_node(n);

    redraw_screen = BOOL_TRUE;

    return BOOL_FALSE;
}

int
menu_delete(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return BOOL_FALSE;
    }
    d = (data_t *)(current->data);

    if(d->ops != NULL && d->ops->op_delete != NULL) {
	return d->ops->op_delete(win, current, list);
    }

    list_delete(current, list);
    redraw_screen = BOOL_TRUE;

    return BOOL_FALSE;
}

int
menu_undelete(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return BOOL_FALSE;
    }
    d = (data_t *)(current->data);

    if(d->ops != NULL && d->ops->op_undelete != NULL) {
	return d->ops->op_undelete(win, current, list);
    }

    list_undelete(current, list);

    redraw_screen = BOOL_TRUE;

    return BOOL_FALSE;
}

int
menu_saveexit(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return BOOL_TRUE;
    }
    d = (data_t *)(current->data);

    if(d->ops != NULL && d->ops->op_saveexit != NULL) {
	return d->ops->op_saveexit(win, current, list);
    }

    return BOOL_TRUE;
}

node_t *
delete_node(node_t *node)
{
    int i;
    data_t *d;

    if(node == NULL || node->data == NULL) {
	return NULL;
    }
    d = node->data;

    d->deleted = BOOL_TRUE;
    d->text[0] = 'D';

    for(i = 0; i < d->nbr_children; i++) {
	delete_node(d->children[i]);
    }

    return node;
}

/*ARGSUSED*/
int
list_delete(node_t *current, node_t *list)
{
    if(current == NULL && current->data == NULL) {
	return 0;
    }

    delete_node(current);

    return 0;
}

node_t *
undelete_node(node_t *node)
{
    data_t *d;

    if(node == NULL || node->data == NULL) {
	return NULL;
    }
    d = node->data;

    d->deleted = BOOL_FALSE;
    d->text[0] = ' ';

    if(d->parent != NULL) {
	undelete_node(d->parent);
    }

    return node;
}

/*ARGSUSED*/
void
list_undelete(node_t *current, node_t *list)
{
    if(current == NULL || current->data == NULL) {
	return;
    }

    undelete_node(current);
}

int
list_prune(node_t *menulist, char *mountpt, uuid_t *uuidp, time32_t prunetime)
{
    node_t *n;
    data_t *d;

    n = menulist;
    while(n != NULL) {
	d = (data_t *)(n->data);

	if(d != NULL && d->ops != NULL && d->ops->op_prune != NULL) {
	    if(d->ops->op_prune(mountpt, uuidp, prunetime, n, menulist) == BOOL_TRUE) {
		if(d->ops->op_delete == NULL) {
		    list_delete(n, menulist);
		}
		else {
		    d->ops->op_delete(NULL, n, menulist);
		}
	    }
	    else {
		if(d->ops->op_undelete == NULL) {
		    list_undelete(n, menulist);
		}
		else {
		    d->ops->op_undelete(NULL, n, menulist);
		}
	    }
	}
	n = n->next;
    }

    return 0;
}

node_t *
generate_menu(char *inv_path)
{
    char *fstabname;
    node_t *list;

    fstabname = GetFstabFullPath(inv_path);
    if(fstabname == NULL) {
	fprintf(stderr, "%s: internal memory error: general_menu\n",
		g_programName);
	exit(1);
    }
    list = generate_fstab_menu(inv_path, NULL, 0, fstabname);
    free(fstabname);

    return list;
}

int
create_windows()
{
    int		menusize;
    int		infosize;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    signal(SIGWINCH, signal_handler);

    if(LINES < 7) {
	endwin();
	fprintf(stderr, "%s: window too small for curses interactive mode: LINES = %d\n",
		g_programName, LINES);
	exit(1);
    }

    mainmenu = newpad(100, COLS);
    keypad(mainmenu, TRUE);
    notimeout(mainmenu, TRUE);

    menusize = (LINES - 2) - INFO_SIZE;
    if(menusize <= 0)
	menusize = 1;

    prefresh(mainmenu,
	     0, 0,
	     1, 0,
	     menusize, COLS - 1);

    infosize = INFO_SIZE;
    if(infosize <= 0)
	infosize = 1;

    infowin = newwin(infosize, COLS, menusize + 1, 0);
    keypad(infowin, TRUE);
    wrefresh(infowin);

    return menusize;
}

int
invutil_interactive(char *inv_path, char *mountpt, uuid_t *uuidp, time32_t timeSecs)
{
    int		keyc;
    node_t	*menulist;

    menulist = generate_menu(inv_path);
    if(menulist == NULL) {
	fprintf(stderr, "%s: abnormal termination\n", g_programName);
	exit(1);
    }

    if(timeSecs > 0) {
	list_prune(menulist, mountpt, uuidp, timeSecs);
    }

    keyc = sizeof(keyv) / sizeof(keyv[0]);

    create_windows();

    menu(mainmenu, 1, 0, menulist, keyc, keyv);

    endwin();

    close_all_stobj();
    close_all_invidx();
    close_all_fstab();

    return 0;
}
