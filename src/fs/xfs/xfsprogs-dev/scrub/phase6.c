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
#include <sys/statvfs.h>
#include "xfs.h"
#include "xfs_fs.h"
#include "handle.h"
#include "path.h"
#include "ptvar.h"
#include "workqueue.h"
#include "xfs_scrub.h"
#include "common.h"
#include "bitmap.h"
#include "disk.h"
#include "filemap.h"
#include "fscounters.h"
#include "inodes.h"
#include "read_verify.h"
#include "spacemap.h"
#include "vfs.h"

/*
 * Phase 6: Verify data file integrity.
 *
 * Identify potential data block extents with GETFSMAP, then feed those
 * extents to the read-verify pool to get the verify commands batched,
 * issued, and (if there are problems) reported back to us.  If there
 * are errors, we'll record the bad regions and (if available) use rmap
 * to tell us if metadata are now corrupt.  Otherwise, we'll scan the
 * whole directory tree looking for files that overlap the bad regions
 * and report the paths of the now corrupt files.
 */

/* Find the fd for a given device identifier. */
static struct disk *
xfs_dev_to_disk(
	struct scrub_ctx	*ctx,
	dev_t			dev)
{
	if (dev == ctx->fsinfo.fs_datadev)
		return ctx->datadev;
	else if (dev == ctx->fsinfo.fs_logdev)
		return ctx->logdev;
	else if (dev == ctx->fsinfo.fs_rtdev)
		return ctx->rtdev;
	abort();
}

/* Find the device major/minor for a given file descriptor. */
static dev_t
xfs_disk_to_dev(
	struct scrub_ctx	*ctx,
	struct disk		*disk)
{
	if (disk == ctx->datadev)
		return ctx->fsinfo.fs_datadev;
	else if (disk == ctx->logdev)
		return ctx->fsinfo.fs_logdev;
	else if (disk == ctx->rtdev)
		return ctx->fsinfo.fs_rtdev;
	abort();
}

struct owner_decode {
	uint64_t		owner;
	const char		*descr;
};

static const struct owner_decode special_owners[] = {
	{XFS_FMR_OWN_FREE,	"free space"},
	{XFS_FMR_OWN_UNKNOWN,	"unknown owner"},
	{XFS_FMR_OWN_FS,	"static FS metadata"},
	{XFS_FMR_OWN_LOG,	"journalling log"},
	{XFS_FMR_OWN_AG,	"per-AG metadata"},
	{XFS_FMR_OWN_INOBT,	"inode btree blocks"},
	{XFS_FMR_OWN_INODES,	"inodes"},
	{XFS_FMR_OWN_REFC,	"refcount btree"},
	{XFS_FMR_OWN_COW,	"CoW staging"},
	{XFS_FMR_OWN_DEFECTIVE,	"bad blocks"},
	{0, NULL},
};

/* Decode a special owner. */
static const char *
xfs_decode_special_owner(
	uint64_t			owner)
{
	const struct owner_decode	*od = special_owners;

	while (od->descr) {
		if (od->owner == owner)
			return od->descr;
		od++;
	}

	return NULL;
}

/* Routines to translate bad physical extents into file paths and offsets. */

struct xfs_verify_error_info {
	struct bitmap			*d_bad;		/* bytes */
	struct bitmap			*r_bad;		/* bytes */
};

/* Report if this extent overlaps a bad region. */
static bool
xfs_report_verify_inode_bmap(
	struct scrub_ctx		*ctx,
	const char			*descr,
	int				fd,
	int				whichfork,
	struct fsxattr			*fsx,
	struct xfs_bmap			*bmap,
	void				*arg)
{
	struct xfs_verify_error_info	*vei = arg;
	struct bitmap			*bmp;

	/* Only report errors for real extents. */
	if (bmap->bm_flags & (BMV_OF_PREALLOC | BMV_OF_DELALLOC))
		return true;

	if (fsx->fsx_xflags & FS_XFLAG_REALTIME)
		bmp = vei->r_bad;
	else
		bmp = vei->d_bad;

	if (!bitmap_test(bmp, bmap->bm_physical, bmap->bm_length))
		return true;

	str_error(ctx, descr,
_("offset %llu failed read verification."), bmap->bm_offset);
	return true;
}

/* Iterate the extent mappings of a file to report errors. */
static bool
xfs_report_verify_fd(
	struct scrub_ctx		*ctx,
	const char			*descr,
	int				fd,
	void				*arg)
{
	struct xfs_bmap			key = {0};
	bool				moveon;

	/* data fork */
	moveon = xfs_iterate_filemaps(ctx, descr, fd, XFS_DATA_FORK, &key,
			xfs_report_verify_inode_bmap, arg);
	if (!moveon)
		return false;

	/* attr fork */
	moveon = xfs_iterate_filemaps(ctx, descr, fd, XFS_ATTR_FORK, &key,
			xfs_report_verify_inode_bmap, arg);
	if (!moveon)
		return false;
	return true;
}

/* Report read verify errors in unlinked (but still open) files. */
static int
xfs_report_verify_inode(
	struct scrub_ctx		*ctx,
	struct xfs_handle		*handle,
	struct xfs_bstat		*bstat,
	void				*arg)
{
	char				descr[DESCR_BUFSZ];
	bool				moveon;
	int				fd;
	int				error;

	snprintf(descr, DESCR_BUFSZ, _("inode %"PRIu64" (unlinked)"),
			(uint64_t)bstat->bs_ino);

	/* Ignore linked files and things we can't open. */
	if (bstat->bs_nlink != 0)
		return 0;
	if (!S_ISREG(bstat->bs_mode) && !S_ISDIR(bstat->bs_mode))
		return 0;

	/* Try to open the inode. */
	fd = xfs_open_handle(handle);
	if (fd < 0) {
		error = errno;
		if (error == ESTALE)
			return error;

		str_info(ctx, descr,
_("Disappeared during read error reporting."));
		return error;
	}

	/* Go find the badness. */
	moveon = xfs_report_verify_fd(ctx, descr, fd, arg);
	close(fd);

	return moveon ? 0 : XFS_ITERATE_INODES_ABORT;
}

/* Scan a directory for matches in the read verify error list. */
static bool
xfs_report_verify_dir(
	struct scrub_ctx	*ctx,
	const char		*path,
	int			dir_fd,
	void			*arg)
{
	return xfs_report_verify_fd(ctx, path, dir_fd, arg);
}

/*
 * Scan the inode associated with a directory entry for matches with
 * the read verify error list.
 */
static bool
xfs_report_verify_dirent(
	struct scrub_ctx	*ctx,
	const char		*path,
	int			dir_fd,
	struct dirent		*dirent,
	struct stat		*sb,
	void			*arg)
{
	bool			moveon;
	int			fd;

	/* Ignore things we can't open. */
	if (!S_ISREG(sb->st_mode) && !S_ISDIR(sb->st_mode))
		return true;

	/* Ignore . and .. */
	if (!strcmp(".", dirent->d_name) || !strcmp("..", dirent->d_name))
		return true;

	/*
	 * If we were given a dirent, open the associated file under
	 * dir_fd for badblocks scanning.  If dirent is NULL, then it's
	 * the directory itself we want to scan.
	 */
	fd = openat(dir_fd, dirent->d_name,
			O_RDONLY | O_NOATIME | O_NOFOLLOW | O_NOCTTY);
	if (fd < 0)
		return true;

	/* Go find the badness. */
	moveon = xfs_report_verify_fd(ctx, path, fd, arg);
	if (moveon)
		goto out;

out:
	close(fd);

	return moveon;
}

/* Given bad extent lists for the data & rtdev, find bad files. */
static bool
xfs_report_verify_errors(
	struct scrub_ctx		*ctx,
	struct bitmap			*d_bad,
	struct bitmap			*r_bad)
{
	struct xfs_verify_error_info	vei;
	bool				moveon;

	vei.d_bad = d_bad;
	vei.r_bad = r_bad;

	/* Scan the directory tree to get file paths. */
	moveon = scan_fs_tree(ctx, xfs_report_verify_dir,
			xfs_report_verify_dirent, &vei);
	if (!moveon)
		return false;

	/* Scan for unlinked files. */
	return xfs_scan_all_inodes(ctx, xfs_report_verify_inode, &vei);
}

/* Verify disk blocks with GETFSMAP */

struct xfs_verify_extent {
	struct read_verify_pool	*readverify;
	struct ptvar		*rvstate;
	struct bitmap		*d_bad;		/* bytes */
	struct bitmap		*r_bad;		/* bytes */
};

/* Report an IO error resulting from read-verify based off getfsmap. */
static bool
xfs_check_rmap_error_report(
	struct scrub_ctx	*ctx,
	const char		*descr,
	struct fsmap		*map,
	void			*arg)
{
	const char		*type;
	char			buf[32];
	uint64_t		err_physical = *(uint64_t *)arg;
	uint64_t		err_off;

	if (err_physical > map->fmr_physical)
		err_off = err_physical - map->fmr_physical;
	else
		err_off = 0;

	snprintf(buf, 32, _("disk offset %"PRIu64),
			(uint64_t)BTOBB(map->fmr_physical + err_off));

	if (map->fmr_flags & FMR_OF_SPECIAL_OWNER) {
		type = xfs_decode_special_owner(map->fmr_owner);
		str_error(ctx, buf,
_("%s failed read verification."),
				type);
	}

	/*
	 * XXX: If we had a getparent() call we could report IO errors
	 * efficiently.  Until then, we'll have to scan the dir tree
	 * to find the bad file's pathname.
	 */

	return true;
}

/*
 * Remember a read error for later, and see if rmap will tell us about the
 * owner ahead of time.
 */
static void
xfs_check_rmap_ioerr(
	struct scrub_ctx		*ctx,
	struct disk			*disk,
	uint64_t			start,
	uint64_t			length,
	int				error,
	void				*arg)
{
	struct fsmap			keys[2];
	char				descr[DESCR_BUFSZ];
	struct xfs_verify_extent	*ve = arg;
	struct bitmap			*tree;
	dev_t				dev;
	bool				moveon;

	dev = xfs_disk_to_dev(ctx, disk);

	/*
	 * If we don't have parent pointers, save the bad extent for
	 * later rescanning.
	 */
	if (dev == ctx->fsinfo.fs_datadev)
		tree = ve->d_bad;
	else if (dev == ctx->fsinfo.fs_rtdev)
		tree = ve->r_bad;
	else
		tree = NULL;
	if (tree) {
		moveon = bitmap_set(tree, start, length);
		if (!moveon)
			str_errno(ctx, ctx->mntpoint);
	}

	snprintf(descr, DESCR_BUFSZ, _("dev %d:%d ioerr @ %"PRIu64":%"PRIu64" "),
			major(dev), minor(dev), start, length);

	/* Go figure out which blocks are bad from the fsmap. */
	memset(keys, 0, sizeof(struct fsmap) * 2);
	keys->fmr_device = dev;
	keys->fmr_physical = start;
	(keys + 1)->fmr_device = dev;
	(keys + 1)->fmr_physical = start + length - 1;
	(keys + 1)->fmr_owner = ULLONG_MAX;
	(keys + 1)->fmr_offset = ULLONG_MAX;
	(keys + 1)->fmr_flags = UINT_MAX;
	xfs_iterate_fsmap(ctx, descr, keys, xfs_check_rmap_error_report,
			&start);
}

/* Schedule a read-verify of a (data block) extent. */
static bool
xfs_check_rmap(
	struct scrub_ctx		*ctx,
	const char			*descr,
	struct fsmap			*map,
	void				*arg)
{
	struct xfs_verify_extent	*ve = arg;
	struct disk			*disk;

	dbg_printf("rmap dev %d:%d phys %"PRIu64" owner %"PRId64
			" offset %"PRIu64" len %"PRIu64" flags 0x%x\n",
			major(map->fmr_device), minor(map->fmr_device),
			(uint64_t)map->fmr_physical, (int64_t)map->fmr_owner,
			(uint64_t)map->fmr_offset, (uint64_t)map->fmr_length,
			map->fmr_flags);

	/* "Unknown" extents should be verified; they could be data. */
	if ((map->fmr_flags & FMR_OF_SPECIAL_OWNER) &&
			map->fmr_owner == XFS_FMR_OWN_UNKNOWN)
		map->fmr_flags &= ~FMR_OF_SPECIAL_OWNER;

	/*
	 * We only care about read-verifying data extents that have been
	 * written to disk.  This means we can skip "special" owners
	 * (metadata), xattr blocks, unwritten extents, and extent maps.
	 * These should all get checked elsewhere in the scrubber.
	 */
	if (map->fmr_flags & (FMR_OF_PREALLOC | FMR_OF_ATTR_FORK |
			      FMR_OF_EXTENT_MAP | FMR_OF_SPECIAL_OWNER))
		goto out;

	/* XXX: Filter out directory data blocks. */

	/* Schedule the read verify command for (eventual) running. */
	disk = xfs_dev_to_disk(ctx, map->fmr_device);

	read_verify_schedule_io(ve->readverify, ptvar_get(ve->rvstate), disk,
			map->fmr_physical, map->fmr_length, ve);

out:
	/* Is this the last extent?  Fire off the read. */
	if (map->fmr_flags & FMR_OF_LAST)
		read_verify_force_io(ve->readverify, ptvar_get(ve->rvstate));

	return true;
}

/*
 * Read verify all the file data blocks in a filesystem.  Since XFS doesn't
 * do data checksums, we trust that the underlying storage will pass back
 * an IO error if it can't retrieve whatever we previously stored there.
 * If we hit an IO error, we'll record the bad blocks in a bitmap and then
 * scan the extent maps of the entire fs tree to figure (and the unlinked
 * inodes) out which files are now broken.
 */
bool
xfs_scan_blocks(
	struct scrub_ctx		*ctx)
{
	struct xfs_verify_extent	ve;
	bool				moveon;

	ve.rvstate = ptvar_init(scrub_nproc(ctx), sizeof(struct read_verify));
	if (!ve.rvstate) {
		str_errno(ctx, ctx->mntpoint);
		return false;
	}

	moveon = bitmap_init(&ve.d_bad);
	if (!moveon) {
		str_errno(ctx, ctx->mntpoint);
		goto out_ve;
	}

	moveon = bitmap_init(&ve.r_bad);
	if (!moveon) {
		str_errno(ctx, ctx->mntpoint);
		goto out_dbad;
	}

	ve.readverify = read_verify_pool_init(ctx, ctx->geo.blocksize,
			xfs_check_rmap_ioerr, disk_heads(ctx->datadev));
	if (!ve.readverify) {
		moveon = false;
		str_info(ctx, ctx->mntpoint,
_("Could not create media verifier."));
		goto out_rbad;
	}
	moveon = xfs_scan_all_spacemaps(ctx, xfs_check_rmap, &ve);
	if (!moveon)
		goto out_pool;
	read_verify_pool_flush(ve.readverify);
	ctx->bytes_checked += read_verify_bytes(ve.readverify);
	read_verify_pool_destroy(ve.readverify);

	/* Scan the whole dir tree to see what matches the bad extents. */
	if (!bitmap_empty(ve.d_bad) || !bitmap_empty(ve.r_bad))
		moveon = xfs_report_verify_errors(ctx, ve.d_bad, ve.r_bad);

	bitmap_free(&ve.r_bad);
	bitmap_free(&ve.d_bad);
	ptvar_free(ve.rvstate);
	return moveon;

out_pool:
	read_verify_pool_destroy(ve.readverify);
out_rbad:
	bitmap_free(&ve.r_bad);
out_dbad:
	bitmap_free(&ve.d_bad);
out_ve:
	ptvar_free(ve.rvstate);
	return moveon;
}

/* Estimate how much work we're going to do. */
bool
xfs_estimate_verify_work(
	struct scrub_ctx	*ctx,
	uint64_t		*items,
	unsigned int		*nr_threads,
	int			*rshift)
{
	unsigned long long	d_blocks;
	unsigned long long	d_bfree;
	unsigned long long	r_blocks;
	unsigned long long	r_bfree;
	unsigned long long	f_files;
	unsigned long long	f_free;
	bool			moveon;

	moveon = xfs_scan_estimate_blocks(ctx, &d_blocks, &d_bfree,
				&r_blocks, &r_bfree, &f_files, &f_free);
	if (!moveon)
		return moveon;

	*items = ((d_blocks - d_bfree) + (r_blocks - r_bfree)) << ctx->blocklog;
	*nr_threads = disk_heads(ctx->datadev);
	*rshift = 20;
	return moveon;
}
