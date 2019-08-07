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
#ifndef FSTAB_H
#define FSTAB_H

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"
#include "list.h"
#include "cmenu.h"

/* fstab.c */
node_t *generate_fstab_menu(char *inv_path, node_t *startnode, int level, char *fstabname);
int open_fstab(char *fstabname);
int close_all_fstab(void);
void *remmap_fstab(int fidx, int num);
int find_matching_fstab(int fidx, invt_fstab_t *fstabentry);
int fstab_select(WINDOW *win, node_t *current, node_t *list);
int fstab_highlight(WINDOW *win, node_t *current, node_t *list);
int fstab_commit(WINDOW *win, node_t *current, node_t *list);
int fstab_prune(char *mountpt, uuid_t *uuidp, time32_t prunetime, node_t *node, node_t *list);

#endif /* FSTAB_H */
