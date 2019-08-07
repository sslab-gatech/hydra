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
#ifndef XFS_SCRUB_SCRUB_H_
#define XFS_SCRUB_SCRUB_H_

/* Online scrub and repair. */
enum check_outcome {
	CHECK_DONE,	/* no further processing needed */
	CHECK_REPAIR,	/* schedule this for repairs */
	CHECK_ABORT,	/* end program */
	CHECK_RETRY,	/* repair failed, try again later */
};

void xfs_scrub_report_preen_triggers(struct scrub_ctx *ctx);
bool xfs_scrub_primary_super(struct scrub_ctx *ctx);
bool xfs_scrub_ag_headers(struct scrub_ctx *ctx, xfs_agnumber_t agno);
bool xfs_scrub_ag_metadata(struct scrub_ctx *ctx, xfs_agnumber_t agno);
bool xfs_scrub_fs_metadata(struct scrub_ctx *ctx);

bool xfs_can_scrub_fs_metadata(struct scrub_ctx *ctx);
bool xfs_can_scrub_inode(struct scrub_ctx *ctx);
bool xfs_can_scrub_bmap(struct scrub_ctx *ctx);
bool xfs_can_scrub_dir(struct scrub_ctx *ctx);
bool xfs_can_scrub_attr(struct scrub_ctx *ctx);
bool xfs_can_scrub_symlink(struct scrub_ctx *ctx);
bool xfs_can_scrub_parent(struct scrub_ctx *ctx);
bool xfs_can_repair(struct scrub_ctx *ctx);

bool xfs_scrub_inode_fields(struct scrub_ctx *ctx, uint64_t ino, uint32_t gen,
		int fd);
bool xfs_scrub_data_fork(struct scrub_ctx *ctx, uint64_t ino, uint32_t gen,
		int fd);
bool xfs_scrub_attr_fork(struct scrub_ctx *ctx, uint64_t ino, uint32_t gen,
		int fd);
bool xfs_scrub_cow_fork(struct scrub_ctx *ctx, uint64_t ino, uint32_t gen,
		int fd);
bool xfs_scrub_dir(struct scrub_ctx *ctx, uint64_t ino, uint32_t gen,
		int fd);
bool xfs_scrub_attr(struct scrub_ctx *ctx, uint64_t ino, uint32_t gen,
		int fd);
bool xfs_scrub_symlink(struct scrub_ctx *ctx, uint64_t ino, uint32_t gen,
		int fd);
bool xfs_scrub_parent(struct scrub_ctx *ctx, uint64_t ino, uint32_t gen,
		int fd);

/* Repair parameters are the scrub inputs and retry count. */
struct repair_item {
	struct list_head	list;
	__u64			ino;
	__u32			type;
	__u32			flags;
	__u32			gen;
	__u32			agno;
};

/* Only perform repairs; leave optimization-only actions for later. */
#define XRM_REPAIR_ONLY		(1U << 0)

/* Complain if still broken even after fix. */
#define XRM_NOFIX_COMPLAIN	(1U << 1)

enum check_outcome xfs_repair_metadata(struct scrub_ctx *ctx, int fd,
		struct repair_item *ri, unsigned int repair_flags);

#endif /* XFS_SCRUB_SCRUB_H_ */
