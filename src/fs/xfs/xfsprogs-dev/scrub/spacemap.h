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
#ifndef XFS_SCRUB_SPACEMAP_H_
#define XFS_SCRUB_SPACEMAP_H_

typedef bool (*xfs_fsmap_iter_fn)(struct scrub_ctx *ctx, const char *descr,
		struct fsmap *fsr, void *arg);

bool xfs_iterate_fsmap(struct scrub_ctx *ctx, const char *descr,
		struct fsmap *keys, xfs_fsmap_iter_fn fn, void *arg);
bool xfs_scan_all_spacemaps(struct scrub_ctx *ctx, xfs_fsmap_iter_fn fn,
		void *arg);

#endif /* XFS_SCRUB_SPACEMAP_H_ */
