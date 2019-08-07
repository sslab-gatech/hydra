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
#ifndef XFS_SCRUB_FILEMAP_H_
#define XFS_SCRUB_FILEMAP_H_

/* inode fork block mapping */
struct xfs_bmap {
	uint64_t	bm_offset;	/* file offset of segment in bytes */
	uint64_t	bm_physical;	/* physical starting byte  */
	uint64_t	bm_length;	/* length of segment, bytes */
	uint32_t	bm_flags;	/* output flags */
};

typedef bool (*xfs_bmap_iter_fn)(struct scrub_ctx *ctx, const char *descr,
		int fd, int whichfork, struct fsxattr *fsx,
		struct xfs_bmap *bmap, void *arg);

bool xfs_iterate_filemaps(struct scrub_ctx *ctx, const char *descr, int fd,
		int whichfork, struct xfs_bmap *key, xfs_bmap_iter_fn fn,
		void *arg);

#endif /* XFS_SCRUB_FILEMAP_H_ */
