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
#ifndef CMENU_H
#define CMENU_H

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"
#include "list.h"

/* number of lines for info window */
#define INFO_SIZE 4

#define put_header(m,a)		put_line(stdscr, 0, m, A_REVERSE | A_BOLD, a)
#define put_footer(m,a)		put_line(stdscr, LINES - 1, m, A_REVERSE | A_BOLD, a)
#define put_error(m)		put_line(stdscr, LINES - 1, m, A_REVERSE | A_BOLD, ALIGN_LEFT);\
					beep();\
					wrefresh(stdscr);\
					sleep(2)
#define put_info_header(m)	put_line(infowin, 0, m, A_REVERSE|A_BOLD, ALIGN_LEFT)
#define put_info_line(l,m)	put_line(infowin, l, m, 0, ALIGN_LEFT)
#define put_option(w,l,t,a)	put_line(w, l, t, a, ALIGN_LEFT)
#define clear_line(w,l)		wmove(w, l, 0); wclrtoeol(w);

typedef enum {
    ALIGN_LEFT,
    ALIGN_CENTRE,
    ALIGN_RIGHT
} alignment_t;

typedef struct menu_ops_s {
    int (* op_delete		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_undelete		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_saveexit		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_select		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_collapse		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_expand		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_collapseall	) (WINDOW *win, node_t *current, node_t *list);
    int (* op_expandall		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_highlight		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_unhighlight	) (WINDOW *win, node_t *current, node_t *list);
    int (* op_commit		) (WINDOW *win, node_t *current, node_t *list);
    int (* op_prune		) (char *mountpt, uuid_t *uuidp, time32_t prunetime, node_t *node, node_t *list);
} menu_ops_t;

typedef struct {
    int key;
    char *text;
    int (*fn)(WINDOW *win, node_t *current, node_t *list);
} menukey_t;

typedef struct {
    invt_seshdr_t *header;
    invt_session_t *session;
} stobjsess_t;

typedef struct {
    char		*name;
    int			fd;
    int			nEntries;
    char		*mapaddr;
    invt_counter_t	*counter;
    invt_entry_t	**data;
    int			numrecords;
} invidx_fileinfo_t;

typedef struct {
    char		*name;
    int			fd;
    off_t		size;
    char		*mapaddr;
    invt_sescounter_t	*counter;
    void		**data;
    int			numrecords;
} stobj_fileinfo_t;

typedef struct {
    char		*name;
    int			fd;
    int			nEntries;
    char		*mapaddr;
    invt_counter_t	*counter;
    invt_fstab_t	**data;
    int			numrecords;
} fstab_fileinfo_t;


extern int redraw_screen;
extern int redraw_options;
extern WINDOW *mainmenu;
extern WINDOW *infowin;


/* cmenu.c */
void hitanykey(void);
int put_line(WINDOW *win, int line, char *msg, int attr, alignment_t alignment);
int create_windows(void);
int get_string(WINDOW *win, char *prompt, char *response, int len);

void list_undelete(node_t *current, node_t *list);
int list_delete(node_t *current, node_t *list);

int menu_import(WINDOW *win, node_t *current, node_t *list);
int menu_unhighlight(WINDOW *win, node_t *current, node_t *list);
int menu_expand(WINDOW *win, node_t *current, node_t *list);
int menu_expandall(WINDOW *win, node_t *current, node_t *list);
int menu_collapse(WINDOW *win, node_t *current, node_t *list);
int menu_collapseall(WINDOW *win, node_t *current, node_t *list);
int menu_select(WINDOW *win, node_t *current, node_t *list);
int menu_delete(WINDOW *win, node_t *current, node_t *list);
int menu_undelete(WINDOW *win, node_t *current, node_t *list);
int menu_saveexit(WINDOW *win, node_t *current, node_t *list);
int menu_commit(WINDOW *win, node_t *current, node_t *list);
int menu_quit(WINDOW *win, node_t *current, node_t *list);

/* menu.c */
int menu(WINDOW *win, int line, int col, node_t *list, int keyc, menukey_t *keyv);
node_t *generate_menu(char *inv_path);

#endif /* CMENU_H */
