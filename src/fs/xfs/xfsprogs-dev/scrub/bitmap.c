/*
 * Copyright (C) 2018 Oracle.  All Rights Reserved.
 *
 * Author: Darrick J. Wong <darrick.wong@oracle.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include "platform_defs.h"
#include "avl64.h"
#include "list.h"
#include "bitmap.h"

/*
 * Space Efficient Bitmap
 *
 * Implements a space-efficient bitmap.  We use an AVL tree to manage
 * extent records that tell us which ranges are set; the bitmap key is
 * an arbitrary uint64_t.  The usual bitmap operations (set, clear,
 * test, test and set) are supported, plus we can iterate set ranges.
 */

#define avl_for_each_range_safe(pos, n, l, first, last) \
	for (pos = (first), n = pos->avl_nextino, l = (last)->avl_nextino; pos != (l); \
			pos = n, n = pos ? pos->avl_nextino : NULL)

#define avl_for_each_safe(tree, pos, n) \
	for (pos = (tree)->avl_firstino, n = pos ? pos->avl_nextino : NULL; \
			pos != NULL; \
			pos = n, n = pos ? pos->avl_nextino : NULL)

#define avl_for_each(tree, pos) \
	for (pos = (tree)->avl_firstino; pos != NULL; pos = pos->avl_nextino)

struct bitmap_node {
	struct avl64node	btn_node;
	uint64_t		btn_start;
	uint64_t		btn_length;
};

static uint64_t
extent_start(
	struct avl64node	*node)
{
	struct bitmap_node	*btn;

	btn = container_of(node, struct bitmap_node, btn_node);
	return btn->btn_start;
}

static uint64_t
extent_end(
	struct avl64node	*node)
{
	struct bitmap_node	*btn;

	btn = container_of(node, struct bitmap_node, btn_node);
	return btn->btn_start + btn->btn_length;
}

static struct avl64ops bitmap_ops = {
	extent_start,
	extent_end,
};

/* Initialize a bitmap. */
bool
bitmap_init(
	struct bitmap		**bmapp)
{
	struct bitmap		*bmap;

	bmap = calloc(1, sizeof(struct bitmap));
	if (!bmap)
		return false;
	bmap->bt_tree = malloc(sizeof(struct avl64tree_desc));
	if (!bmap->bt_tree) {
		free(bmap);
		return false;
	}

	pthread_mutex_init(&bmap->bt_lock, NULL);
	avl64_init_tree(bmap->bt_tree, &bitmap_ops);
	*bmapp = bmap;

	return true;
}

/* Free a bitmap. */
void
bitmap_free(
	struct bitmap		**bmapp)
{
	struct bitmap		*bmap;
	struct avl64node	*node;
	struct avl64node	*n;
	struct bitmap_node	*ext;

	bmap = *bmapp;
	avl_for_each_safe(bmap->bt_tree, node, n) {
		ext = container_of(node, struct bitmap_node, btn_node);
		free(ext);
	}
	free(bmap->bt_tree);
	*bmapp = NULL;
}

/* Create a new bitmap extent node. */
static struct bitmap_node *
bitmap_node_init(
	uint64_t		start,
	uint64_t		len)
{
	struct bitmap_node	*ext;

	ext = malloc(sizeof(struct bitmap_node));
	if (!ext)
		return NULL;

	ext->btn_node.avl_nextino = NULL;
	ext->btn_start = start;
	ext->btn_length = len;

	return ext;
}

/* Set a region of bits (locked). */
static bool
__bitmap_set(
	struct bitmap		*bmap,
	uint64_t		start,
	uint64_t		length)
{
	struct avl64node	*firstn;
	struct avl64node	*lastn;
	struct avl64node	*pos;
	struct avl64node	*n;
	struct avl64node	*l;
	struct bitmap_node	*ext;
	uint64_t		new_start;
	uint64_t		new_length;
	struct avl64node	*node;
	bool			res = true;

	/* Find any existing nodes adjacent or within that range. */
	avl64_findranges(bmap->bt_tree, start - 1, start + length + 1,
			&firstn, &lastn);

	/* Nothing, just insert a new extent. */
	if (firstn == NULL && lastn == NULL) {
		ext = bitmap_node_init(start, length);
		if (!ext)
			return false;

		node = avl64_insert(bmap->bt_tree, &ext->btn_node);
		if (node == NULL) {
			free(ext);
			errno = EEXIST;
			return false;
		}

		return true;
	}

	assert(firstn != NULL && lastn != NULL);
	new_start = start;
	new_length = length;

	avl_for_each_range_safe(pos, n, l, firstn, lastn) {
		ext = container_of(pos, struct bitmap_node, btn_node);

		/* Bail if the new extent is contained within an old one. */
		if (ext->btn_start <= start &&
		    ext->btn_start + ext->btn_length >= start + length)
			return res;

		/* Check for overlapping and adjacent extents. */
		if (ext->btn_start + ext->btn_length >= start ||
		    ext->btn_start <= start + length) {
			if (ext->btn_start < start) {
				new_start = ext->btn_start;
				new_length += ext->btn_length;
			}

			if (ext->btn_start + ext->btn_length >
			    new_start + new_length)
				new_length = ext->btn_start + ext->btn_length -
						new_start;

			avl64_delete(bmap->bt_tree, pos);
			free(ext);
		}
	}

	ext = bitmap_node_init(new_start, new_length);
	if (!ext)
		return false;

	node = avl64_insert(bmap->bt_tree, &ext->btn_node);
	if (node == NULL) {
		free(ext);
		errno = EEXIST;
		return false;
	}

	return res;
}

/* Set a region of bits. */
bool
bitmap_set(
	struct bitmap		*bmap,
	uint64_t		start,
	uint64_t		length)
{
	bool			res;

	pthread_mutex_lock(&bmap->bt_lock);
	res = __bitmap_set(bmap, start, length);
	pthread_mutex_unlock(&bmap->bt_lock);

	return res;
}

#if 0	/* Unused, provided for completeness. */
/* Clear a region of bits. */
bool
bitmap_clear(
	struct bitmap		*bmap,
	uint64_t		start,
	uint64_t		len)
{
	struct avl64node	*firstn;
	struct avl64node	*lastn;
	struct avl64node	*pos;
	struct avl64node	*n;
	struct avl64node	*l;
	struct bitmap_node	*ext;
	uint64_t		new_start;
	uint64_t		new_length;
	struct avl64node	*node;
	int			stat;

	pthread_mutex_lock(&bmap->bt_lock);
	/* Find any existing nodes over that range. */
	avl64_findranges(bmap->bt_tree, start, start + len, &firstn, &lastn);

	/* Nothing, we're done. */
	if (firstn == NULL && lastn == NULL) {
		pthread_mutex_unlock(&bmap->bt_lock);
		return true;
	}

	assert(firstn != NULL && lastn != NULL);

	/* Delete or truncate everything in sight. */
	avl_for_each_range_safe(pos, n, l, firstn, lastn) {
		ext = container_of(pos, struct bitmap_node, btn_node);

		stat = 0;
		if (ext->btn_start < start)
			stat |= 1;
		if (ext->btn_start + ext->btn_length > start + len)
			stat |= 2;
		switch (stat) {
		case 0:
			/* Extent totally within range; delete. */
			avl64_delete(bmap->bt_tree, pos);
			free(ext);
			break;
		case 1:
			/* Extent is left-adjacent; truncate. */
			ext->btn_length = start - ext->btn_start;
			break;
		case 2:
			/* Extent is right-adjacent; move it. */
			ext->btn_length = ext->btn_start + ext->btn_length -
					(start + len);
			ext->btn_start = start + len;
			break;
		case 3:
			/* Extent overlaps both ends. */
			ext->btn_length = start - ext->btn_start;
			new_start = start + len;
			new_length = ext->btn_start + ext->btn_length -
					new_start;

			ext = bitmap_node_init(new_start, new_length);
			if (!ext)
				return false;

			node = avl64_insert(bmap->bt_tree, &ext->btn_node);
			if (node == NULL) {
				errno = EEXIST;
				return false;
			}
			break;
		}
	}

	pthread_mutex_unlock(&bmap->bt_lock);
	return true;
}
#endif

#ifdef DEBUG
/* Iterate the set regions of this bitmap. */
bool
bitmap_iterate(
	struct bitmap		*bmap,
	bool			(*fn)(uint64_t, uint64_t, void *),
	void			*arg)
{
	struct avl64node	*node;
	struct bitmap_node	*ext;
	bool			moveon = true;

	pthread_mutex_lock(&bmap->bt_lock);
	avl_for_each(bmap->bt_tree, node) {
		ext = container_of(node, struct bitmap_node, btn_node);
		moveon = fn(ext->btn_start, ext->btn_length, arg);
		if (!moveon)
			break;
	}
	pthread_mutex_unlock(&bmap->bt_lock);

	return moveon;
}
#endif

/* Do any bitmap extents overlap the given one?  (locked) */
static bool
__bitmap_test(
	struct bitmap		*bmap,
	uint64_t		start,
	uint64_t		len)
{
	struct avl64node	*firstn;
	struct avl64node	*lastn;

	/* Find any existing nodes over that range. */
	avl64_findranges(bmap->bt_tree, start, start + len, &firstn, &lastn);

	return firstn != NULL && lastn != NULL;
}

/* Is any part of this range set? */
bool
bitmap_test(
	struct bitmap		*bmap,
	uint64_t		start,
	uint64_t		len)
{
	bool			res;

	pthread_mutex_lock(&bmap->bt_lock);
	res = __bitmap_test(bmap, start, len);
	pthread_mutex_unlock(&bmap->bt_lock);

	return res;
}

/* Are none of the bits set? */
bool
bitmap_empty(
	struct bitmap		*bmap)
{
	return bmap->bt_tree->avl_firstino == NULL;
}

#ifdef DEBUG
static bool
bitmap_dump_fn(
	uint64_t		startblock,
	uint64_t		blockcount,
	void			*arg)
{
	printf("%"PRIu64":%"PRIu64"\n", startblock, blockcount);
	return true;
}

/* Dump bitmap. */
void
bitmap_dump(
	struct bitmap		*bmap)
{
	printf("BITMAP DUMP %p\n", bmap);
	bitmap_iterate(bmap, bitmap_dump_fn, NULL);
	printf("BITMAP DUMP DONE\n");
}
#endif
