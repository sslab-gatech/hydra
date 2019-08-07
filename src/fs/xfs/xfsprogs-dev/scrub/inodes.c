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
#include <pthread.h>
#include <sys/statvfs.h>
#include "platform_defs.h"
#include "xfs.h"
#include "xfs_arch.h"
#include "xfs_format.h"
#include "handle.h"
#include "path.h"
#include "workqueue.h"
#include "xfs_scrub.h"
#include "common.h"
#include "inodes.h"

/*
 * Iterate a range of inodes.
 *
 * This is a little more involved than repeatedly asking BULKSTAT for a
 * buffer's worth of stat data for some number of inodes.  We want to scan as
 * many of the inodes that the inobt thinks there are, including the ones that
 * are broken, but if we ask for n inodes starting at x, it'll skip the bad
 * ones and fill from beyond the range (x + n).
 *
 * Therefore, we ask INUMBERS to return one inobt chunk's worth of inode
 * bitmap information.  Then we try to BULKSTAT only the inodes that were
 * present in that chunk, and compare what we got against what INUMBERS said
 * was there.  If there's a mismatch, we know that we have an inode that fails
 * the verifiers but we can inject the bulkstat information to force the scrub
 * code to deal with the broken inodes.
 *
 * If the iteration function returns ESTALE, that means that the inode has
 * been deleted and possibly recreated since the BULKSTAT call.  We wil
 * refresh the stat information and try again up to 30 times before reporting
 * the staleness as an error.
 */

/*
 * Did we get exactly the inodes we expected?  If not, load them one at a
 * time (or fake it) into the bulkstat data.
 */
static void
xfs_iterate_inodes_range_check(
	struct scrub_ctx	*ctx,
	struct xfs_inogrp	*inogrp,
	struct xfs_bstat	*bstat)
{
	struct xfs_fsop_bulkreq	onereq = {0};
	struct xfs_bstat	*bs;
	__u64			oneino;
	__s32			onelen = 0;
	int			i;
	int			error;

	onereq.lastip  = &oneino;
	onereq.icount  = 1;
	onereq.ocount  = &onelen;

	for (i = 0, bs = bstat; i < XFS_INODES_PER_CHUNK; i++) {
		if (!(inogrp->xi_allocmask & (1ULL << i)))
			continue;
		if (bs->bs_ino == inogrp->xi_startino + i) {
			bs++;
			continue;
		}

		/* Load the one inode. */
		oneino = inogrp->xi_startino + i;
		onereq.ubuffer = bs;
		error = ioctl(ctx->mnt_fd, XFS_IOC_FSBULKSTAT_SINGLE,
				&onereq);
		if (error || bs->bs_ino != inogrp->xi_startino + i) {
			memset(bs, 0, sizeof(struct xfs_bstat));
			bs->bs_ino = inogrp->xi_startino + i;
			bs->bs_blksize = ctx->mnt_sv.f_frsize;
		}
		bs++;
	}
}

/*
 * Call into the filesystem for inode/bulkstat information and call our
 * iterator function.  We'll try to fill the bulkstat information in batches,
 * but we also can detect iget failures.
 */
static bool
xfs_iterate_inodes_range(
	struct scrub_ctx	*ctx,
	const char		*descr,
	void			*fshandle,
	uint64_t		first_ino,
	uint64_t		last_ino,
	xfs_inode_iter_fn	fn,
	void			*arg)
{
	struct xfs_fsop_bulkreq	igrpreq = {0};
	struct xfs_fsop_bulkreq	bulkreq = {0};
	struct xfs_handle	handle;
	struct xfs_inogrp	inogrp;
	struct xfs_bstat	bstat[XFS_INODES_PER_CHUNK];
	char			idescr[DESCR_BUFSZ];
	char			buf[DESCR_BUFSZ];
	struct xfs_bstat	*bs;
	__u64			igrp_ino;
	__u64			ino;
	__s32			bulklen = 0;
	__s32			igrplen = 0;
	bool			moveon = true;
	int			i;
	int			error;
	int			stale_count = 0;


	memset(bstat, 0, XFS_INODES_PER_CHUNK * sizeof(struct xfs_bstat));
	bulkreq.lastip  = &ino;
	bulkreq.icount  = XFS_INODES_PER_CHUNK;
	bulkreq.ubuffer = &bstat;
	bulkreq.ocount  = &bulklen;

	igrpreq.lastip  = &igrp_ino;
	igrpreq.icount  = 1;
	igrpreq.ubuffer = &inogrp;
	igrpreq.ocount  = &igrplen;

	memcpy(&handle.ha_fsid, fshandle, sizeof(handle.ha_fsid));
	handle.ha_fid.fid_len = sizeof(xfs_fid_t) -
			sizeof(handle.ha_fid.fid_len);
	handle.ha_fid.fid_pad = 0;

	/* Find the inode chunk & alloc mask */
	igrp_ino = first_ino;
	error = ioctl(ctx->mnt_fd, XFS_IOC_FSINUMBERS, &igrpreq);
	while (!error && igrplen) {
		/* Load the inodes. */
		ino = inogrp.xi_startino - 1;
		bulkreq.icount = inogrp.xi_alloccount;
		error = ioctl(ctx->mnt_fd, XFS_IOC_FSBULKSTAT, &bulkreq);
		if (error)
			str_info(ctx, descr, "%s", strerror_r(errno,
						buf, DESCR_BUFSZ));

		xfs_iterate_inodes_range_check(ctx, &inogrp, bstat);

		/* Iterate all the inodes. */
		for (i = 0, bs = bstat; i < inogrp.xi_alloccount; i++, bs++) {
			if (bs->bs_ino > last_ino)
				goto out;

			handle.ha_fid.fid_ino = bs->bs_ino;
			handle.ha_fid.fid_gen = bs->bs_gen;
			error = fn(ctx, &handle, bs, arg);
			switch (error) {
			case 0:
				break;
			case ESTALE:
				stale_count++;
				if (stale_count < 30) {
					igrp_ino = inogrp.xi_startino;
					goto igrp_retry;
				}
				snprintf(idescr, DESCR_BUFSZ, "inode %"PRIu64,
						(uint64_t)bs->bs_ino);
				str_info(ctx, idescr,
_("Changed too many times during scan; giving up."));
				break;
			case XFS_ITERATE_INODES_ABORT:
				error = 0;
				/* fall thru */
			default:
				moveon = false;
				errno = error;
				goto err;
			}
			if (xfs_scrub_excessive_errors(ctx)) {
				moveon = false;
				goto out;
			}
		}

		stale_count = 0;
igrp_retry:
		error = ioctl(ctx->mnt_fd, XFS_IOC_FSINUMBERS, &igrpreq);
	}

err:
	if (error) {
		str_errno(ctx, descr);
		moveon = false;
	}
out:
	return moveon;
}

/* BULKSTAT wrapper routines. */
struct xfs_scan_inodes {
	xfs_inode_iter_fn	fn;
	void			*arg;
	bool			moveon;
};

/* Scan all the inodes in an AG. */
static void
xfs_scan_ag_inodes(
	struct workqueue	*wq,
	xfs_agnumber_t		agno,
	void			*arg)
{
	struct xfs_scan_inodes	*si = arg;
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

	moveon = xfs_iterate_inodes_range(ctx, descr, ctx->fshandle, ag_ino,
			next_ag_ino - 1, si->fn, si->arg);
	if (!moveon)
		si->moveon = false;
}

/* Scan all the inodes in a filesystem. */
bool
xfs_scan_all_inodes(
	struct scrub_ctx	*ctx,
	xfs_inode_iter_fn	fn,
	void			*arg)
{
	struct xfs_scan_inodes	si;
	xfs_agnumber_t		agno;
	struct workqueue	wq;
	int			ret;

	si.moveon = true;
	si.fn = fn;
	si.arg = arg;

	ret = workqueue_create(&wq, (struct xfs_mount *)ctx,
			scrub_nproc_workqueue(ctx));
	if (ret) {
		str_info(ctx, ctx->mntpoint, _("Could not create workqueue."));
		return false;
	}

	for (agno = 0; agno < ctx->geo.agcount; agno++) {
		ret = workqueue_add(&wq, xfs_scan_ag_inodes, agno, &si);
		if (ret) {
			si.moveon = false;
			str_info(ctx, ctx->mntpoint,
_("Could not queue AG %u bulkstat work."), agno);
			break;
		}
	}

	workqueue_destroy(&wq);

	return si.moveon;
}

/*
 * Open a file by handle, or return a negative error code.
 */
int
xfs_open_handle(
	struct xfs_handle	*handle)
{
	return open_by_fshandle(handle, sizeof(*handle),
			O_RDONLY | O_NOATIME | O_NOFOLLOW | O_NOCTTY);
}
