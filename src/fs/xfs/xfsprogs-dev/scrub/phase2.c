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
#include "scrub.h"

/* Phase 2: Check internal metadata. */

/* Scrub each AG's metadata btrees. */
static void
xfs_scan_ag_metadata(
	struct workqueue		*wq,
	xfs_agnumber_t			agno,
	void				*arg)
{
	struct scrub_ctx		*ctx = (struct scrub_ctx *)wq->wq_ctx;
	bool				*pmoveon = arg;
	bool				moveon;
	char				descr[DESCR_BUFSZ];

	snprintf(descr, DESCR_BUFSZ, _("AG %u"), agno);

	/*
	 * First we scrub and fix the AG headers, because we need
	 * them to work well enough to check the AG btrees.
	 */
	moveon = xfs_scrub_ag_headers(ctx, agno);
	if (!moveon)
		goto err;

	/* Now scrub the AG btrees. */
	moveon = xfs_scrub_ag_metadata(ctx, agno);
	if (!moveon)
		goto err;

	return;
err:
	*pmoveon = false;
}

/* Scrub whole-FS metadata btrees. */
static void
xfs_scan_fs_metadata(
	struct workqueue		*wq,
	xfs_agnumber_t			agno,
	void				*arg)
{
	struct scrub_ctx		*ctx = (struct scrub_ctx *)wq->wq_ctx;
	bool				*pmoveon = arg;
	bool				moveon;

	moveon = xfs_scrub_fs_metadata(ctx);
	if (!moveon)
		*pmoveon = false;
}

/* Scan all filesystem metadata. */
bool
xfs_scan_metadata(
	struct scrub_ctx	*ctx)
{
	struct workqueue	wq;
	xfs_agnumber_t		agno;
	bool			moveon = true;
	int			ret;

	ret = workqueue_create(&wq, (struct xfs_mount *)ctx,
			scrub_nproc_workqueue(ctx));
	if (ret) {
		str_info(ctx, ctx->mntpoint, _("Could not create workqueue."));
		return false;
	}

	/*
	 * In case we ever use the primary super scrubber to perform fs
	 * upgrades (followed by a full scrub), do that before we launch
	 * anything else.
	 */
	moveon = xfs_scrub_primary_super(ctx);
	if (!moveon)
		return moveon;

	for (agno = 0; moveon && agno < ctx->geo.agcount; agno++) {
		ret = workqueue_add(&wq, xfs_scan_ag_metadata, agno, &moveon);
		if (ret) {
			moveon = false;
			str_info(ctx, ctx->mntpoint,
_("Could not queue AG %u scrub work."), agno);
			goto out;
		}
	}

	if (!moveon)
		goto out;

	ret = workqueue_add(&wq, xfs_scan_fs_metadata, 0, &moveon);
	if (ret) {
		moveon = false;
		str_info(ctx, ctx->mntpoint,
_("Could not queue filesystem scrub work."));
		goto out;
	}

out:
	workqueue_destroy(&wq);
	return moveon;
}

/* Estimate how much work we're going to do. */
bool
xfs_estimate_metadata_work(
	struct scrub_ctx	*ctx,
	uint64_t		*items,
	unsigned int		*nr_threads,
	int			*rshift)
{
	*items = xfs_scrub_estimate_ag_work(ctx);
	*nr_threads = scrub_nproc(ctx);
	*rshift = 0;
	return true;
}
