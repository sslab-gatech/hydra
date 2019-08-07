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

#include <stdlib.h>
#include <ncurses.h>
#include <uuid/uuid.h>

#include "list.h"
#include "cmenu.h"

node_t *
parent_add_child(node_t *parent, node_t *child)
{
    data_t *d;

    if(parent == NULL || child == NULL)
	return parent;

    d = parent->data;

    d->nbr_children++;
    d->children = realloc(d->children, d->nbr_children * sizeof(*child));
    d->children[d->nbr_children - 1] = child;

    return parent;
}

node_t *
node_create(int hidden, int expanded, int level, int deleted, int file_idx, char *text,
	    menu_ops_t *ops, node_t *parent, node_t **children, int nbr_children, int data_idx)
{
    node_t *newnode;
    data_t *newdata;

    newnode = malloc(sizeof(*newnode));
    if(newnode == NULL)
	return NULL;

    newdata = malloc(sizeof(*newdata));
    if(newdata == NULL)
	return NULL;

    newdata->hidden = hidden;
    newdata->expanded = expanded;
    newdata->level = level;
    newdata->deleted = deleted;
    newdata->file_idx = file_idx;
    newdata->text = text;
    newdata->ops = ops;
    newdata->parent = parent;
    newdata->children = children;
    newdata->nbr_children = nbr_children;
    newdata->data_idx = data_idx;
    newdata->commited = BOOL_FALSE;
    newdata->imported = BOOL_FALSE;

    if(deleted == BOOL_TRUE) {
	newdata->text[0] = 'D';
    }

    newnode->data = newdata;
    newnode->next = NULL;
    newnode->prev = NULL;

    return newnode;
}

void
node_free(node_t *node)
{
    if(node != NULL) {
	if(node->data != NULL) {
	    if(((data_t *)(node->data))->text != NULL) {
	        free(((data_t *)(node->data))->text);
	        free(((data_t *)(node->data))->children);
	    }
	    free(node->data);
	}
	free(node);
    }
}

node_t *
list_add(node_t *prevnode, node_t *newnode)
{
    if(newnode == NULL)
	return NULL;

    if(prevnode != NULL) {
	newnode->next = prevnode->next;
	prevnode->next = newnode;

	newnode->prev = prevnode;
	if(newnode->next != NULL) {
	    newnode->next->prev = newnode;
	}
    }
    else {
	newnode->next = NULL;
	newnode->prev = NULL;
    }

    if(((data_t *)(newnode->data))->parent != NULL) {
	parent_add_child(((data_t *)(newnode->data))->parent, newnode);
    }

    return newnode;
}

node_t *
list_del(node_t *node)
{
    if(node->prev != NULL) {
	node->prev->next = node->next;
    }
    if(node->next != NULL) {
	node->next->prev = node->prev;
    }
    node->next = NULL;
    node->prev = NULL;

    return node;
}

node_t *
free_all_children(node_t *node)
{
    int i;
    data_t *d;

    if(node == NULL || node->data == NULL)
	return NULL;

    d = (data_t *)(node->data);

    for(i = 0; i < d->nbr_children; i++) {
	node_free(list_del(free_all_children(d->children[i])));
    }
    d->children = NULL;
    d->nbr_children = 0;

    return node;
}

/* Used when a parent controls the commit for child nodes */
node_t *
mark_all_children_commited(node_t *node)
{
    int i;
    data_t *d;

    if(node == NULL || node->data == NULL)
	return NULL;

    d = (data_t *)(node->data);

    for(i = 0; i < d->nbr_children; i++) {
	mark_all_children_commited(d->children[i]);
    }
    d->commited = BOOL_TRUE;

    return node;
}
