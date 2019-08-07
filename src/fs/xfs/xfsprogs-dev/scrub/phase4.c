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
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include "xfs.h"
#include "xfs_fs.h"
#include "list.h"
#include "path.h"
#include "workqueue.h"
#include "xfs_scrub.h"
#include "common.h"
#include "progress.h"
#include "scrub.h"
#include "vfs.h"

/* Phase 4: Repair filesystem. */

/* Process all the action items. */
static bool
xfs_process_action_items(
	struct scrub_ctx		*ctx)
{
	bool				moveon = true;

	pthread_mutex_lock(&ctx->lock);
	if (moveon && ctx->errors_found == 0 && want_fstrim) {
		fstrim(ctx);
		progress_add(1);
	}
	pthread_mutex_unlock(&ctx->lock);

	return moveon;
}

/* Fix everything that needs fixing. */
bool
xfs_repair_fs(
	struct scrub_ctx		*ctx)
{
	return xfs_process_action_items(ctx);
}

/* Estimate how much work we're going to do. */
bool
xfs_estimate_repair_work(
	struct scrub_ctx	*ctx,
	uint64_t		*items,
	unsigned int		*nr_threads,
	int			*rshift)
{
	*items = 1;
	*nr_threads = 1;
	*rshift = 0;
	return true;
}
