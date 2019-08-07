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
#ifndef XFS_SCRUB_BITMAP_H_
#define XFS_SCRUB_BITMAP_H_

struct bitmap {
	pthread_mutex_t		bt_lock;
	struct avl64tree_desc	*bt_tree;
};

bool bitmap_init(struct bitmap **bmap);
void bitmap_free(struct bitmap **bmap);
bool bitmap_set(struct bitmap *bmap, uint64_t start, uint64_t length);
bool bitmap_iterate(struct bitmap *bmap,
		bool (*fn)(uint64_t, uint64_t, void *), void *arg);
bool bitmap_test(struct bitmap *bmap, uint64_t start,
		uint64_t len);
bool bitmap_empty(struct bitmap *bmap);
void bitmap_dump(struct bitmap *bmap);

#endif /* XFS_SCRUB_BITMAP_H_ */
