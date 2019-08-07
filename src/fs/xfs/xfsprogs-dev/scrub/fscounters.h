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
#ifndef XFS_SCRUB_FSCOUNTERS_H_
#define XFS_SCRUB_FSCOUNTERS_H_

bool xfs_scan_estimate_blocks(struct scrub_ctx *ctx,
		unsigned long long *d_blocks, unsigned long long *d_bfree,
		unsigned long long *r_blocks, unsigned long long *r_bfree,
		unsigned long long *f_files, unsigned long long *f_free);
bool xfs_count_all_inodes(struct scrub_ctx *ctx, uint64_t *count);

#endif /* XFS_SCRUB_FSCOUNTERS_H_ */
