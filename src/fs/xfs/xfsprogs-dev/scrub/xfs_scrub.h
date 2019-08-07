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
#ifndef XFS_SCRUB_XFS_SCRUB_H_
#define XFS_SCRUB_XFS_SCRUB_H_

#define _PATH_PROC_MOUNTS	"/proc/mounts"

extern unsigned int		nr_threads;
extern unsigned int		bg_mode;
extern unsigned int		debug;
extern int			nproc;
extern bool			verbose;
extern long			page_size;
extern bool			want_fstrim;
extern bool			stderr_isatty;
extern bool			stdout_isatty;

enum scrub_mode {
	SCRUB_MODE_DRY_RUN,
	SCRUB_MODE_REPAIR,
};

enum error_action {
	ERRORS_CONTINUE,
	ERRORS_SHUTDOWN,
};

struct scrub_ctx {
	/* Immutable scrub state. */

	/* Strings we need for presentation */
	char			*mntpoint;
	char			*blkdev;

	/* Mountpoint info */
	struct stat		mnt_sb;
	struct statvfs		mnt_sv;
	struct statfs		mnt_sf;

	/* Open block devices */
	struct disk		*datadev;
	struct disk		*logdev;
	struct disk		*rtdev;

	/* What does the user want us to do? */
	enum scrub_mode		mode;

	/* How does the user want us to react to errors? */
	enum error_action	error_action;

	/* fd to filesystem mount point */
	int			mnt_fd;

	/* Number of threads for metadata scrubbing */
	unsigned int		nr_io_threads;

	/* XFS specific geometry */
	struct xfs_fsop_geom	geo;
	struct fs_path		fsinfo;
	unsigned int		agblklog;
	unsigned int		blocklog;
	unsigned int		inodelog;
	unsigned int		inopblog;
	void			*fshandle;
	size_t			fshandle_len;

	/* Data block read verification buffer */
	void			*readbuf;

	/* Mutable scrub state; use lock. */
	pthread_mutex_t		lock;
	unsigned long long	max_errors;
	unsigned long long	runtime_errors;
	unsigned long long	errors_found;
	unsigned long long	warnings_found;
	unsigned long long	inodes_checked;
	unsigned long long	bytes_checked;
	unsigned long long	naming_warnings;
	unsigned long long	repairs;
	unsigned long long	preens;
	bool			need_repair;
	bool			preen_triggers[XFS_SCRUB_TYPE_NR];
};

/* Phase helper functions */
void xfs_shutdown_fs(struct scrub_ctx *ctx);
bool xfs_cleanup_fs(struct scrub_ctx *ctx);
bool xfs_setup_fs(struct scrub_ctx *ctx);
bool xfs_scan_metadata(struct scrub_ctx *ctx);
bool xfs_scan_inodes(struct scrub_ctx *ctx);
bool xfs_scan_connections(struct scrub_ctx *ctx);
bool xfs_scan_blocks(struct scrub_ctx *ctx);
bool xfs_scan_summary(struct scrub_ctx *ctx);
bool xfs_repair_fs(struct scrub_ctx *ctx);

/* Progress estimator functions */
uint64_t xfs_estimate_inodes(struct scrub_ctx *ctx);
unsigned int xfs_scrub_estimate_ag_work(struct scrub_ctx *ctx);
bool xfs_estimate_metadata_work(struct scrub_ctx *ctx, uint64_t *items,
				unsigned int *nr_threads, int *rshift);
bool xfs_estimate_inodes_work(struct scrub_ctx *ctx, uint64_t *items,
			      unsigned int *nr_threads, int *rshift);
bool xfs_estimate_repair_work(struct scrub_ctx *ctx, uint64_t *items,
			      unsigned int *nr_threads, int *rshift);
bool xfs_estimate_verify_work(struct scrub_ctx *ctx, uint64_t *items,
			      unsigned int *nr_threads, int *rshift);

#endif /* XFS_SCRUB_XFS_SCRUB_H_ */
