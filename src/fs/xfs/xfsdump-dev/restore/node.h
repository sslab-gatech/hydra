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
#ifndef NODE_H
#define NODE_H

/* node.[ch] - abstract pool of nodes
 *
 * operators alloc, free, map, and unmap nodes.
 */

typedef size32_t nh_t;
#define NH_NULL	SIZE32MAX

/* node_init - creates a new node abstraction.
 * user reserves one byte per node for use by the node abstraction
 */
extern bool_t node_init( int fd,		/* backing store */
		         off64_t off,		/* offset into backing store */
		         size_t nodesz,		/* node size */
		         ix_t nodehkix,		/* my housekeeping byte */
		         size_t alignsz,	/* node alignment requirement */
		         size64_t vmsz,		/* abstractions's share of VM */
		         size64_t dir_nondir);	/* num of dirs + nondirs */

/* node_sync - syncs up with existing node abstraction persistent state
 */
extern bool_t node_sync( int fd, off64_t off );

/* node_alloc - allocates a node, returning a handle.
 * returns NULL handle if no space left.
 */
extern nh_t node_alloc( void );

/* node_map - returns a pointer to the node identified by the node handle.
 * pointer remains valid until node_unmap called.
 */
extern void *node_map( nh_t node_handle );

/* node_unmap - unmaps the node.
 * SIDE-EFFECT: sets *nodepp to 0.
 */
extern void node_unmap( nh_t node_handle, void **nodepp );

/* node_free - frees a previously allocated node.
 * SIDE-EFFECT: sets *node_handlep to NODE_HANDLE_NULL.
 */
extern void node_free( nh_t *node_handlep );

#endif /* NODE_H */
