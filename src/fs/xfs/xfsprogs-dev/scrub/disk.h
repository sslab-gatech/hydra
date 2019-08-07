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
#ifndef XFS_SCRUB_DISK_H_
#define XFS_SCRUB_DISK_H_

#define DISK_FLAG_SCSI_VERIFY	0x1
struct disk {
	struct stat	d_sb;
	int		d_fd;
	int		d_lbalog;
	unsigned int	d_flags;
	unsigned int	d_blksize;	/* bytes */
	uint64_t	d_size;		/* bytes */
	uint64_t	d_start;	/* bytes */
};

unsigned int disk_heads(struct disk *disk);
struct disk *disk_open(const char *pathname);
int disk_close(struct disk *disk);
ssize_t disk_read_verify(struct disk *disk, void *buf, uint64_t startblock,
		uint64_t blockcount);

#endif /* XFS_SCRUB_DISK_H_ */
