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
#ifndef XFS_SCRUB_READ_VERIFY_H_
#define XFS_SCRUB_READ_VERIFY_H_

struct scrub_ctx;
struct read_verify_pool;

/* Function called when an IO error happens. */
typedef void (*read_verify_ioerr_fn_t)(struct scrub_ctx *ctx,
		struct disk *disk, uint64_t start, uint64_t length,
		int error, void *arg);

struct read_verify_pool *read_verify_pool_init(struct scrub_ctx *ctx,
		size_t miniosz, read_verify_ioerr_fn_t ioerr_fn,
		unsigned int nproc);
void read_verify_pool_flush(struct read_verify_pool *rvp);
void read_verify_pool_destroy(struct read_verify_pool *rvp);

struct read_verify {
	void			*io_end_arg;
	struct disk		*io_disk;
	uint64_t		io_start;	/* bytes */
	uint64_t		io_length;	/* bytes */
};

bool read_verify_schedule_io(struct read_verify_pool *rvp,
		struct read_verify *rv, struct disk *disk, uint64_t start,
		uint64_t length, void *end_arg);
bool read_verify_force_io(struct read_verify_pool *rvp, struct read_verify *rv);
uint64_t read_verify_bytes(struct read_verify_pool *rvp);

#endif /* XFS_SCRUB_READ_VERIFY_H_ */
