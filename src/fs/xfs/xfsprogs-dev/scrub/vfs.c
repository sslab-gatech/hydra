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
#include <sys/statvfs.h>
#include "xfs.h"
#include "handle.h"
#include "path.h"
#include "workqueue.h"
#include "xfs_scrub.h"
#include "common.h"
#include "vfs.h"

#ifndef AT_NO_AUTOMOUNT
# define AT_NO_AUTOMOUNT	0x800
#endif

/*
 * Helper functions to assist in traversing a directory tree using regular
 * VFS calls.
 */

/* Scan a filesystem tree. */
struct scan_fs_tree {
	unsigned int		nr_dirs;
	pthread_mutex_t		lock;
	pthread_cond_t		wakeup;
	struct stat		root_sb;
	bool			moveon;
	scan_fs_tree_dir_fn	dir_fn;
	scan_fs_tree_dirent_fn	dirent_fn;
	void			*arg;
};

/* Per-work-item scan context. */
struct scan_fs_tree_dir {
	char			*path;
	struct scan_fs_tree	*sft;
	bool			rootdir;
};

/* Scan a directory sub tree. */
static void
scan_fs_dir(
	struct workqueue	*wq,
	xfs_agnumber_t		agno,
	void			*arg)
{
	struct scrub_ctx	*ctx = (struct scrub_ctx *)wq->wq_ctx;
	struct scan_fs_tree_dir	*sftd = arg;
	struct scan_fs_tree	*sft = sftd->sft;
	DIR			*dir;
	struct dirent		*dirent;
	char			newpath[PATH_MAX];
	struct scan_fs_tree_dir	*new_sftd;
	struct stat		sb;
	int			dir_fd;
	int			error;

	/* Open the directory. */
	dir_fd = open(sftd->path, O_RDONLY | O_NOATIME | O_NOFOLLOW | O_NOCTTY);
	if (dir_fd < 0) {
		if (errno != ENOENT)
			str_errno(ctx, sftd->path);
		goto out;
	}

	/* Caller-specific directory checks. */
	if (!sft->dir_fn(ctx, sftd->path, dir_fd, sft->arg)) {
		sft->moveon = false;
		close(dir_fd);
		goto out;
	}

	/* Iterate the directory entries. */
	dir = fdopendir(dir_fd);
	if (!dir) {
		str_errno(ctx, sftd->path);
		close(dir_fd);
		goto out;
	}
	rewinddir(dir);
	for (dirent = readdir(dir); dirent != NULL; dirent = readdir(dir)) {
		snprintf(newpath, PATH_MAX, "%s/%s", sftd->path,
				dirent->d_name);

		/* Get the stat info for this directory entry. */
		error = fstatat(dir_fd, dirent->d_name, &sb,
				AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW);
		if (error) {
			str_errno(ctx, newpath);
			continue;
		}

		/* Ignore files on other filesystems. */
		if (sb.st_dev != sft->root_sb.st_dev)
			continue;

		/* Caller-specific directory entry function. */
		if (!sft->dirent_fn(ctx, newpath, dir_fd, dirent, &sb,
				sft->arg)) {
			sft->moveon = false;
			break;
		}

		if (xfs_scrub_excessive_errors(ctx)) {
			sft->moveon = false;
			break;
		}

		/* If directory, call ourselves recursively. */
		if (S_ISDIR(sb.st_mode) && strcmp(".", dirent->d_name) &&
		    strcmp("..", dirent->d_name)) {
			new_sftd = malloc(sizeof(struct scan_fs_tree_dir));
			if (!new_sftd) {
				str_errno(ctx, newpath);
				sft->moveon = false;
				break;
			}
			new_sftd->path = strdup(newpath);
			new_sftd->sft = sft;
			new_sftd->rootdir = false;
			pthread_mutex_lock(&sft->lock);
			sft->nr_dirs++;
			pthread_mutex_unlock(&sft->lock);
			error = workqueue_add(wq, scan_fs_dir, 0, new_sftd);
			if (error) {
				str_info(ctx, ctx->mntpoint,
_("Could not queue subdirectory scan work."));
				sft->moveon = false;
				break;
			}
		}
	}

	/* Close dir, go away. */
	error = closedir(dir);
	if (error)
		str_errno(ctx, sftd->path);

out:
	pthread_mutex_lock(&sft->lock);
	sft->nr_dirs--;
	if (sft->nr_dirs == 0)
		pthread_cond_signal(&sft->wakeup);
	pthread_mutex_unlock(&sft->lock);

	free(sftd->path);
	free(sftd);
}

/* Scan the entire filesystem. */
bool
scan_fs_tree(
	struct scrub_ctx	*ctx,
	scan_fs_tree_dir_fn	dir_fn,
	scan_fs_tree_dirent_fn	dirent_fn,
	void			*arg)
{
	struct workqueue	wq;
	struct scan_fs_tree	sft;
	struct scan_fs_tree_dir	*sftd;
	int			ret;

	sft.moveon = true;
	sft.nr_dirs = 1;
	sft.root_sb = ctx->mnt_sb;
	sft.dir_fn = dir_fn;
	sft.dirent_fn = dirent_fn;
	sft.arg = arg;
	pthread_mutex_init(&sft.lock, NULL);
	pthread_cond_init(&sft.wakeup, NULL);

	sftd = malloc(sizeof(struct scan_fs_tree_dir));
	if (!sftd) {
		str_errno(ctx, ctx->mntpoint);
		return false;
	}
	sftd->path = strdup(ctx->mntpoint);
	sftd->sft = &sft;
	sftd->rootdir = true;

	ret = workqueue_create(&wq, (struct xfs_mount *)ctx,
			scrub_nproc_workqueue(ctx));
	if (ret) {
		str_info(ctx, ctx->mntpoint, _("Could not create workqueue."));
		goto out_free;
	}
	ret = workqueue_add(&wq, scan_fs_dir, 0, sftd);
	if (ret) {
		str_info(ctx, ctx->mntpoint,
_("Could not queue directory scan work."));
		goto out_free;
	}

	pthread_mutex_lock(&sft.lock);
	pthread_cond_wait(&sft.wakeup, &sft.lock);
	assert(sft.nr_dirs == 0);
	pthread_mutex_unlock(&sft.lock);
	workqueue_destroy(&wq);

	return sft.moveon;
out_free:
	free(sftd->path);
	free(sftd);
	return false;
}

#ifndef FITRIM
struct fstrim_range {
	__u64 start;
	__u64 len;
	__u64 minlen;
};
#define FITRIM		_IOWR('X', 121, struct fstrim_range)	/* Trim */
#endif

/* Call FITRIM to trim all the unused space in a filesystem. */
void
fstrim(
	struct scrub_ctx	*ctx)
{
	struct fstrim_range	range = {0};
	int			error;

	range.len = ULLONG_MAX;
	error = ioctl(ctx->mnt_fd, FITRIM, &range);
	if (error && errno != EOPNOTSUPP && errno != ENOTTY)
		perror(_("fstrim"));
}
