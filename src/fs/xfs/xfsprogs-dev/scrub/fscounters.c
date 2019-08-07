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
#include <sys/statvfs.h>
#include "platform_defs.h"
#include "xfs.h"
#include "xfs_arch.h"
#include "xfs_fs.h"
#include "xfs_format.h"
#include "path.h"
#include "workqueue.h"
#include "xfs_scrub.h"
#include "common.h"
#include "fscounters.h"

/*
 * Filesystem counter collection routines.  We can count the number of
 * inodes in the filesystem, and we can estimate the block counters.
 */

/* Count the number of inodes in the filesystem. */

/* INUMBERS wrapper routines. */
struct xfs_count_inodes {
	bool			moveon;
	uint64_t		counters[0];
};

/*
 * Count the number of inodes.  Use INUMBERS to figure out how many inodes
 * exist in the filesystem, assuming we've already scrubbed that.
 */
static bool
xfs_count_inodes_range(
	struct scrub_ctx	*ctx,
	const char		*descr,
	uint64_t		first_ino,
	uint64_t		last_ino,
	uint64_t		*count)
{
	struct xfs_fsop_bulkreq	igrpreq = {0};
	struct xfs_inogrp	inogrp;
	__u64			igrp_ino;
	uint64_t		nr = 0;
	__s32			igrplen = 0;
	int			error;

	ASSERT(!(first_ino & (XFS_INODES_PER_CHUNK - 1)));
	ASSERT((last_ino & (XFS_INODES_PER_CHUNK - 1)));

	igrpreq.lastip  = &igrp_ino;
	igrpreq.icount  = 1;
	igrpreq.ubuffer = &inogrp;
	igrpreq.ocount  = &igrplen;

	igrp_ino = first_ino;
	error = ioctl(ctx->mnt_fd, XFS_IOC_FSINUMBERS, &igrpreq);
	while (!error && igrplen && inogrp.xi_startino < last_ino) {
		nr += inogrp.xi_alloccount;
		error = ioctl(ctx->mnt_fd, XFS_IOC_FSINUMBERS, &igrpreq);
	}

	if (error) {
		str_errno(ctx, descr);
		return false;
	}

	*count = nr;
	return true;
}

/* Scan all the inodes in an AG. */
static void
xfs_count_ag_inodes(
	struct workqueue	*wq,
	xfs_agnumber_t		agno,
	void			*arg)
{
	struct xfs_count_inodes	*ci = arg;
	struct scrub_ctx	*ctx = (struct scrub_ctx *)wq->wq_ctx;
	char			descr[DESCR_BUFSZ];
	uint64_t		ag_ino;
	uint64_t		next_ag_ino;
	bool			moveon;

	snprintf(descr, DESCR_BUFSZ, _("dev %d:%d AG %u inodes"),
				major(ctx->fsinfo.fs_datadev),
				minor(ctx->fsinfo.fs_datadev),
				agno);

	ag_ino = (__u64)agno << (ctx->inopblog + ctx->agblklog);
	next_ag_ino = (__u64)(agno + 1) << (ctx->inopblog + ctx->agblklog);

	moveon = xfs_count_inodes_range(ctx, descr, ag_ino, next_ag_ino - 1,
			&ci->counters[agno]);
	if (!moveon)
		ci->moveon = false;
}

/* Count all the inodes in a filesystem. */
bool
xfs_count_all_inodes(
	struct scrub_ctx	*ctx,
	uint64_t		*count)
{
	struct xfs_count_inodes	*ci;
	xfs_agnumber_t		agno;
	struct workqueue	wq;
	bool			moveon;
	int			ret;

	ci = calloc(1, sizeof(struct xfs_count_inodes) +
			(ctx->geo.agcount * sizeof(uint64_t)));
	if (!ci)
		return false;
	ci->moveon = true;

	ret = workqueue_create(&wq, (struct xfs_mount *)ctx,
			scrub_nproc_workqueue(ctx));
	if (ret) {
		moveon = false;
		str_info(ctx, ctx->mntpoint, _("Could not create workqueue."));
		goto out_free;
	}
	for (agno = 0; agno < ctx->geo.agcount; agno++) {
		ret = workqueue_add(&wq, xfs_count_ag_inodes, agno, ci);
		if (ret) {
			moveon = false;
			str_info(ctx, ctx->mntpoint,
_("Could not queue AG %u icount work."), agno);
			break;
		}
	}
	workqueue_destroy(&wq);

	for (agno = 0; agno < ctx->geo.agcount; agno++)
		*count += ci->counters[agno];
	moveon = ci->moveon;

out_free:
	free(ci);
	return moveon;
}

/* Estimate the number of blocks and inodes in the filesystem. */
bool
xfs_scan_estimate_blocks(
	struct scrub_ctx		*ctx,
	unsigned long long		*d_blocks,
	unsigned long long		*d_bfree,
	unsigned long long		*r_blocks,
	unsigned long long		*r_bfree,
	unsigned long long		*f_files,
	unsigned long long		*f_free)
{
	struct xfs_fsop_counts		fc;
	struct xfs_fsop_resblks		rb;
	struct statvfs			sfs;
	int				error;

	/* Grab the fstatvfs counters, since it has to report accurately. */
	error = fstatvfs(ctx->mnt_fd, &sfs);
	if (error) {
		str_errno(ctx, ctx->mntpoint);
		return false;
	}

	/* Fetch the filesystem counters. */
	error = ioctl(ctx->mnt_fd, XFS_IOC_FSCOUNTS, &fc);
	if (error) {
		str_errno(ctx, ctx->mntpoint);
		return false;
	}

	/*
	 * XFS reserves some blocks to prevent hard ENOSPC, so add those
	 * blocks back to the free data counts.
	 */
	error = ioctl(ctx->mnt_fd, XFS_IOC_GET_RESBLKS, &rb);
	if (error)
		str_errno(ctx, ctx->mntpoint);
	sfs.f_bfree += rb.resblks_avail;

	*d_blocks = sfs.f_blocks + (ctx->geo.logstart ? ctx->geo.logblocks : 0);
	*d_bfree = sfs.f_bfree;
	*r_blocks = ctx->geo.rtblocks;
	*r_bfree = fc.freertx;
	*f_files = sfs.f_files;
	*f_free = sfs.f_ffree;

	return true;
}
