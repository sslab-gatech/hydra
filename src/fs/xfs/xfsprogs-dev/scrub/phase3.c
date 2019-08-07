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
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include "xfs.h"
#include "list.h"
#include "path.h"
#include "workqueue.h"
#include "xfs_scrub.h"
#include "common.h"
#include "counter.h"
#include "inodes.h"
#include "progress.h"
#include "scrub.h"

/* Phase 3: Scan all inodes. */

/*
 * Run a per-file metadata scanner.  We use the ino/gen interface to
 * ensure that the inode we're checking matches what the inode scan
 * told us to look at.
 */
static bool
xfs_scrub_fd(
	struct scrub_ctx	*ctx,
	bool			(*fn)(struct scrub_ctx *, uint64_t,
				      uint32_t, int),
	struct xfs_bstat	*bs)
{
	return fn(ctx, bs->bs_ino, bs->bs_gen, ctx->mnt_fd);
}

struct scrub_inode_ctx {
	struct ptcounter	*icount;
	bool			moveon;
};

/* Verify the contents, xattrs, and extent maps of an inode. */
static int
xfs_scrub_inode(
	struct scrub_ctx	*ctx,
	struct xfs_handle	*handle,
	struct xfs_bstat	*bstat,
	void			*arg)
{
	struct scrub_inode_ctx	*ictx = arg;
	struct ptcounter	*icount = ictx->icount;
	bool			moveon = true;
	int			fd = -1;

	background_sleep();

	/* Try to open the inode to pin it. */
	if (S_ISREG(bstat->bs_mode)) {
		fd = xfs_open_handle(handle);
		/* Stale inode means we scan the whole cluster again. */
		if (fd < 0 && errno == ESTALE)
			return ESTALE;
	}

	/* Scrub the inode. */
	moveon = xfs_scrub_fd(ctx, xfs_scrub_inode_fields, bstat);
	if (!moveon)
		goto out;

	/* Scrub all block mappings. */
	moveon = xfs_scrub_fd(ctx, xfs_scrub_data_fork, bstat);
	if (!moveon)
		goto out;
	moveon = xfs_scrub_fd(ctx, xfs_scrub_attr_fork, bstat);
	if (!moveon)
		goto out;
	moveon = xfs_scrub_fd(ctx, xfs_scrub_cow_fork, bstat);
	if (!moveon)
		goto out;

	if (S_ISLNK(bstat->bs_mode)) {
		/* Check symlink contents. */
		moveon = xfs_scrub_symlink(ctx, bstat->bs_ino,
				bstat->bs_gen, ctx->mnt_fd);
	} else if (S_ISDIR(bstat->bs_mode)) {
		/* Check the directory entries. */
		moveon = xfs_scrub_fd(ctx, xfs_scrub_dir, bstat);
	}
	if (!moveon)
		goto out;

	/* Check all the extended attributes. */
	moveon = xfs_scrub_fd(ctx, xfs_scrub_attr, bstat);
	if (!moveon)
		goto out;

	/* Check parent pointers. */
	moveon = xfs_scrub_fd(ctx, xfs_scrub_parent, bstat);
	if (!moveon)
		goto out;

out:
	ptcounter_add(icount, 1);
	progress_add(1);
	if (fd >= 0)
		close(fd);
	if (!moveon)
		ictx->moveon = false;
	return ictx->moveon ? 0 : XFS_ITERATE_INODES_ABORT;
}

/* Verify all the inodes in a filesystem. */
bool
xfs_scan_inodes(
	struct scrub_ctx	*ctx)
{
	struct scrub_inode_ctx	ictx;
	bool			ret;

	ictx.moveon = true;
	ictx.icount = ptcounter_init(scrub_nproc(ctx));
	if (!ictx.icount) {
		str_info(ctx, ctx->mntpoint, _("Could not create counter."));
		return false;
	}

	ret = xfs_scan_all_inodes(ctx, xfs_scrub_inode, &ictx);
	if (!ret)
		ictx.moveon = false;
	if (!ictx.moveon)
		goto free;
	xfs_scrub_report_preen_triggers(ctx);
	ctx->inodes_checked = ptcounter_value(ictx.icount);

free:
	ptcounter_free(ictx.icount);
	return ictx.moveon;
}

/* Estimate how much work we're going to do. */
bool
xfs_estimate_inodes_work(
	struct scrub_ctx	*ctx,
	uint64_t		*items,
	unsigned int		*nr_threads,
	int			*rshift)
{
	*items = ctx->mnt_sv.f_files - ctx->mnt_sv.f_ffree;
	*nr_threads = scrub_nproc(ctx);
	*rshift = 0;
	return true;
}
