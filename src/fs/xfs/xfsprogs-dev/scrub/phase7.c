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
#include <stdlib.h>
#include <sys/statvfs.h>
#include "xfs.h"
#include "xfs_fs.h"
#include "path.h"
#include "ptvar.h"
#include "xfs_scrub.h"
#include "common.h"
#include "fscounters.h"
#include "spacemap.h"

/* Phase 7: Check summary counters. */

struct xfs_summary_counts {
	unsigned long long	dbytes;		/* data dev bytes */
	unsigned long long	rbytes;		/* rt dev bytes */
	unsigned long long	next_phys;	/* next phys bytes we see? */
	unsigned long long	agbytes;	/* freespace bytes */
};

/* Record block usage. */
static bool
xfs_record_block_summary(
	struct scrub_ctx		*ctx,
	const char			*descr,
	struct fsmap			*fsmap,
	void				*arg)
{
	struct xfs_summary_counts	*counts;
	unsigned long long		len;

	counts = ptvar_get((struct ptvar *)arg);
	if (fsmap->fmr_device == ctx->fsinfo.fs_logdev)
		return true;
	if ((fsmap->fmr_flags & FMR_OF_SPECIAL_OWNER) &&
	    fsmap->fmr_owner == XFS_FMR_OWN_FREE)
		return true;

	len = fsmap->fmr_length;

	/* freesp btrees live in free space, need to adjust counters later. */
	if ((fsmap->fmr_flags & FMR_OF_SPECIAL_OWNER) &&
	    fsmap->fmr_owner == XFS_FMR_OWN_AG) {
		counts->agbytes += fsmap->fmr_length;
	}
	if (fsmap->fmr_device == ctx->fsinfo.fs_rtdev) {
		/* Count realtime extents. */
		counts->rbytes += len;
	} else {
		/* Count datadev extents. */
		if (counts->next_phys >= fsmap->fmr_physical + len)
			return true;
		else if (counts->next_phys > fsmap->fmr_physical)
			len = counts->next_phys - fsmap->fmr_physical;
		counts->dbytes += len;
		counts->next_phys = fsmap->fmr_physical + fsmap->fmr_length;
	}

	return true;
}

/* Add all the summaries in the per-thread counter */
static bool
xfs_add_summaries(
	struct ptvar			*ptv,
	void				*data,
	void				*arg)
{
	struct xfs_summary_counts	*total = arg;
	struct xfs_summary_counts	*item = data;

	total->dbytes += item->dbytes;
	total->rbytes += item->rbytes;
	total->agbytes += item->agbytes;
	return true;
}

/*
 * Count all inodes and blocks in the filesystem as told by GETFSMAP and
 * BULKSTAT, and compare that to summary counters.  Since this is a live
 * filesystem we'll be content if the summary counts are within 10% of
 * what we observed.
 */
bool
xfs_scan_summary(
	struct scrub_ctx		*ctx)
{
	struct xfs_summary_counts	totalcount = {0};
	struct ptvar			*ptvar;
	unsigned long long		used_data;
	unsigned long long		used_rt;
	unsigned long long		used_files;
	unsigned long long		stat_data;
	unsigned long long		stat_rt;
	uint64_t			counted_inodes = 0;
	unsigned long long		absdiff;
	unsigned long long		d_blocks;
	unsigned long long		d_bfree;
	unsigned long long		r_blocks;
	unsigned long long		r_bfree;
	unsigned long long		f_files;
	unsigned long long		f_free;
	bool				moveon;
	bool				complain;
	int				error;

	/* Flush everything out to disk before we start counting. */
	error = syncfs(ctx->mnt_fd);
	if (error) {
		str_errno(ctx, ctx->mntpoint);
		return false;
	}

	ptvar = ptvar_init(scrub_nproc(ctx), sizeof(struct xfs_summary_counts));
	if (!ptvar) {
		str_errno(ctx, ctx->mntpoint);
		return false;
	}

	/* Use fsmap to count blocks. */
	moveon = xfs_scan_all_spacemaps(ctx, xfs_record_block_summary, ptvar);
	if (!moveon)
		goto out_free;
	moveon = ptvar_foreach(ptvar, xfs_add_summaries, &totalcount);
	if (!moveon)
		goto out_free;
	ptvar_free(ptvar);

	/* Scan the whole fs. */
	moveon = xfs_count_all_inodes(ctx, &counted_inodes);
	if (!moveon)
		goto out;

	moveon = xfs_scan_estimate_blocks(ctx, &d_blocks, &d_bfree, &r_blocks,
			&r_bfree, &f_files, &f_free);
	if (!moveon)
		return moveon;

	/*
	 * If we counted blocks with fsmap, then dblocks includes
	 * blocks for the AGFL and the freespace/rmap btrees.  The
	 * filesystem treats them as "free", but since we scanned
	 * them, we'll consider them used.
	 */
	d_bfree -= totalcount.agbytes >> ctx->blocklog;

	/* Report on what we found. */
	used_data = (d_blocks - d_bfree) << ctx->blocklog;
	used_rt = (r_blocks - r_bfree) << ctx->blocklog;
	used_files = f_files - f_free;
	stat_data = totalcount.dbytes;
	stat_rt = totalcount.rbytes;

	/*
	 * Complain if the counts are off by more than 10% unless
	 * the inaccuracy is less than 32MB worth of blocks or 100 inodes.
	 */
	absdiff = 1ULL << 25;
	complain = verbose;
	complain |= !within_range(ctx, stat_data, used_data, absdiff, 1, 10,
			_("data blocks"));
	complain |= !within_range(ctx, stat_rt, used_rt, absdiff, 1, 10,
			_("realtime blocks"));
	complain |= !within_range(ctx, counted_inodes, used_files, 100, 1, 10,
			_("inodes"));

	if (complain) {
		double		d, r, i;
		char		*du, *ru, *iu;

		if (used_rt || stat_rt) {
			d = auto_space_units(used_data, &du);
			r = auto_space_units(used_rt, &ru);
			i = auto_units(used_files, &iu);
			fprintf(stdout,
_("%.1f%s data used;  %.1f%s realtime data used;  %.2f%s inodes used.\n"),
					d, du, r, ru, i, iu);
			d = auto_space_units(stat_data, &du);
			r = auto_space_units(stat_rt, &ru);
			i = auto_units(counted_inodes, &iu);
			fprintf(stdout,
_("%.1f%s data found; %.1f%s realtime data found; %.2f%s inodes found.\n"),
					d, du, r, ru, i, iu);
		} else {
			d = auto_space_units(used_data, &du);
			i = auto_units(used_files, &iu);
			fprintf(stdout,
_("%.1f%s data used;  %.1f%s inodes used.\n"),
					d, du, i, iu);
			d = auto_space_units(stat_data, &du);
			i = auto_units(counted_inodes, &iu);
			fprintf(stdout,
_("%.1f%s data found; %.1f%s inodes found.\n"),
					d, du, i, iu);
		}
		fflush(stdout);
	}

	/*
	 * Complain if the checked inode counts are off, which
	 * implies an incomplete check.
	 */
	if (verbose ||
	    !within_range(ctx, counted_inodes, ctx->inodes_checked, 100, 1, 10,
			_("checked inodes"))) {
		double		i1, i2;
		char		*i1u, *i2u;

		i1 = auto_units(counted_inodes, &i1u);
		i2 = auto_units(ctx->inodes_checked, &i2u);
		fprintf(stdout,
_("%.1f%s inodes counted; %.1f%s inodes checked.\n"),
				i1, i1u, i2, i2u);
		fflush(stdout);
	}

	/*
	 * Complain if the checked block counts are off, which
	 * implies an incomplete check.
	 */
	if (ctx->bytes_checked &&
	    (verbose ||
	     !within_range(ctx, used_data + used_rt,
			ctx->bytes_checked, absdiff, 1, 10,
			_("verified blocks")))) {
		double		b1, b2;
		char		*b1u, *b2u;

		b1 = auto_space_units(used_data + used_rt, &b1u);
		b2 = auto_space_units(ctx->bytes_checked, &b2u);
		fprintf(stdout,
_("%.1f%s data counted; %.1f%s data verified.\n"),
				b1, b1u, b2, b2u);
		fflush(stdout);
	}

	moveon = true;

out:
	return moveon;
out_free:
	ptvar_free(ptvar);
	return moveon;
}
