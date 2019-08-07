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
#include <string.h>
#include <pthread.h>
#include <sys/statvfs.h>
#include "workqueue.h"
#include "xfs.h"
#include "xfs_fs.h"
#include "path.h"
#include "xfs_scrub.h"
#include "common.h"
#include "spacemap.h"

/*
 * Filesystem space map iterators.
 *
 * Logically, we call GETFSMAP to fetch a set of space map records and
 * call a function to iterate over the records.  However, that's not
 * what actually happens -- the work is split into separate items, with
 * each AG, the realtime device, and the log device getting their own
 * work items.  For an XFS with a realtime device and an external log,
 * this means that we can have up to ($agcount + 2) threads running at
 * once.
 *
 * This comes into play if we want to have per-workitem memory.  Maybe.
 * XXX: do we really need all that ?
 */

#define FSMAP_NR	65536

/* Iterate all the fs block mappings between the two keys. */
bool
xfs_iterate_fsmap(
	struct scrub_ctx	*ctx,
	const char		*descr,
	struct fsmap		*keys,
	xfs_fsmap_iter_fn	fn,
	void			*arg)
{
	struct fsmap_head	*head;
	struct fsmap		*p;
	bool			moveon = true;
	int			i;
	int			error;

	head = malloc(fsmap_sizeof(FSMAP_NR));
	if (!head) {
		str_errno(ctx, descr);
		return false;
	}

	memset(head, 0, sizeof(*head));
	memcpy(head->fmh_keys, keys, sizeof(struct fsmap) * 2);
	head->fmh_count = FSMAP_NR;

	while ((error = ioctl(ctx->mnt_fd, FS_IOC_GETFSMAP, head)) == 0) {
		for (i = 0, p = head->fmh_recs;
		     i < head->fmh_entries;
		     i++, p++) {
			moveon = fn(ctx, descr, p, arg);
			if (!moveon)
				goto out;
			if (xfs_scrub_excessive_errors(ctx)) {
				moveon = false;
				goto out;
			}
		}

		if (head->fmh_entries == 0)
			break;
		p = &head->fmh_recs[head->fmh_entries - 1];
		if (p->fmr_flags & FMR_OF_LAST)
			break;
		fsmap_advance(head);
	}

	if (error) {
		str_errno(ctx, descr);
		moveon = false;
	}
out:
	free(head);
	return moveon;
}

/* GETFSMAP wrappers routines. */
struct xfs_scan_blocks {
	xfs_fsmap_iter_fn	fn;
	void			*arg;
	bool			moveon;
};

/* Iterate all the reverse mappings of an AG. */
static void
xfs_scan_ag_blocks(
	struct workqueue	*wq,
	xfs_agnumber_t		agno,
	void			*arg)
{
	struct scrub_ctx	*ctx = (struct scrub_ctx *)wq->wq_ctx;
	struct xfs_scan_blocks	*sbx = arg;
	char			descr[DESCR_BUFSZ];
	struct fsmap		keys[2];
	off64_t			bperag;
	bool			moveon;

	bperag = (off64_t)ctx->geo.agblocks *
		 (off64_t)ctx->geo.blocksize;

	snprintf(descr, DESCR_BUFSZ, _("dev %d:%d AG %u fsmap"),
				major(ctx->fsinfo.fs_datadev),
				minor(ctx->fsinfo.fs_datadev),
				agno);

	memset(keys, 0, sizeof(struct fsmap) * 2);
	keys->fmr_device = ctx->fsinfo.fs_datadev;
	keys->fmr_physical = agno * bperag;
	(keys + 1)->fmr_device = ctx->fsinfo.fs_datadev;
	(keys + 1)->fmr_physical = ((agno + 1) * bperag) - 1;
	(keys + 1)->fmr_owner = ULLONG_MAX;
	(keys + 1)->fmr_offset = ULLONG_MAX;
	(keys + 1)->fmr_flags = UINT_MAX;

	moveon = xfs_iterate_fsmap(ctx, descr, keys, sbx->fn, sbx->arg);
	if (!moveon)
		sbx->moveon = false;
}

/* Iterate all the reverse mappings of a standalone device. */
static void
xfs_scan_dev_blocks(
	struct scrub_ctx	*ctx,
	int			idx,
	dev_t			dev,
	struct xfs_scan_blocks	*sbx)
{
	struct fsmap		keys[2];
	char			descr[DESCR_BUFSZ];
	bool			moveon;

	snprintf(descr, DESCR_BUFSZ, _("dev %d:%d fsmap"),
			major(dev), minor(dev));

	memset(keys, 0, sizeof(struct fsmap) * 2);
	keys->fmr_device = dev;
	(keys + 1)->fmr_device = dev;
	(keys + 1)->fmr_physical = ULLONG_MAX;
	(keys + 1)->fmr_owner = ULLONG_MAX;
	(keys + 1)->fmr_offset = ULLONG_MAX;
	(keys + 1)->fmr_flags = UINT_MAX;

	moveon = xfs_iterate_fsmap(ctx, descr, keys, sbx->fn, sbx->arg);
	if (!moveon)
		sbx->moveon = false;
}

/* Iterate all the reverse mappings of the realtime device. */
static void
xfs_scan_rt_blocks(
	struct workqueue	*wq,
	xfs_agnumber_t		agno,
	void			*arg)
{
	struct scrub_ctx	*ctx = (struct scrub_ctx *)wq->wq_ctx;

	xfs_scan_dev_blocks(ctx, agno, ctx->fsinfo.fs_rtdev, arg);
}

/* Iterate all the reverse mappings of the log device. */
static void
xfs_scan_log_blocks(
	struct workqueue	*wq,
	xfs_agnumber_t		agno,
	void			*arg)
{
	struct scrub_ctx	*ctx = (struct scrub_ctx *)wq->wq_ctx;

	xfs_scan_dev_blocks(ctx, agno, ctx->fsinfo.fs_logdev, arg);
}

/* Scan all the blocks in a filesystem. */
bool
xfs_scan_all_spacemaps(
	struct scrub_ctx	*ctx,
	xfs_fsmap_iter_fn	fn,
	void			*arg)
{
	struct workqueue	wq;
	struct xfs_scan_blocks	sbx;
	xfs_agnumber_t		agno;
	int			ret;

	sbx.moveon = true;
	sbx.fn = fn;
	sbx.arg = arg;

	ret = workqueue_create(&wq, (struct xfs_mount *)ctx,
			scrub_nproc_workqueue(ctx));
	if (ret) {
		str_info(ctx, ctx->mntpoint, _("Could not create workqueue."));
		return false;
	}
	if (ctx->fsinfo.fs_rt) {
		ret = workqueue_add(&wq, xfs_scan_rt_blocks,
				ctx->geo.agcount + 1, &sbx);
		if (ret) {
			sbx.moveon = false;
			str_info(ctx, ctx->mntpoint,
_("Could not queue rtdev fsmap work."));
			goto out;
		}
	}
	if (ctx->fsinfo.fs_log) {
		ret = workqueue_add(&wq, xfs_scan_log_blocks,
				ctx->geo.agcount + 2, &sbx);
		if (ret) {
			sbx.moveon = false;
			str_info(ctx, ctx->mntpoint,
_("Could not queue logdev fsmap work."));
			goto out;
		}
	}
	for (agno = 0; agno < ctx->geo.agcount; agno++) {
		ret = workqueue_add(&wq, xfs_scan_ag_blocks, agno, &sbx);
		if (ret) {
			sbx.moveon = false;
			str_info(ctx, ctx->mntpoint,
_("Could not queue AG %u fsmap work."), agno);
			break;
		}
	}
out:
	workqueue_destroy(&wq);

	return sbx.moveon;
}
