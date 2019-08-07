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
#include "workqueue.h"
#include "path.h"
#include "xfs.h"
#include "xfs_fs.h"
#include "xfs_scrub.h"
#include "common.h"
#include "counter.h"
#include "disk.h"
#include "read_verify.h"
#include "progress.h"

/*
 * Read Verify Pool
 *
 * Manages the data block read verification phase.  The caller schedules
 * verification requests, which are then scheduled to be run by a thread
 * pool worker.  Adjacent (or nearly adjacent) requests can be combined
 * to reduce overhead when free space fragmentation is high.  The thread
 * pool takes care of issuing multiple IOs to the device, if possible.
 */

/*
 * Perform all IO in 32M chunks.  This cannot exceed 65536 sectors
 * because that's the biggest SCSI VERIFY(16) we dare to send.
 */
#define RVP_IO_MAX_SIZE		(33554432)
#define RVP_IO_MAX_SECTORS	(RVP_IO_MAX_SIZE >> BBSHIFT)

/* Tolerate 64k holes in adjacent read verify requests. */
#define RVP_IO_BATCH_LOCALITY	(65536)

struct read_verify_pool {
	struct workqueue	wq;		/* thread pool */
	struct scrub_ctx	*ctx;		/* scrub context */
	void			*readbuf;	/* read buffer */
	struct ptcounter	*verified_bytes;
	read_verify_ioerr_fn_t	ioerr_fn;	/* io error callback */
	size_t			miniosz;	/* minimum io size, bytes */
};

/* Create a thread pool to run read verifiers. */
struct read_verify_pool *
read_verify_pool_init(
	struct scrub_ctx		*ctx,
	size_t				miniosz,
	read_verify_ioerr_fn_t		ioerr_fn,
	unsigned int			nproc)
{
	struct read_verify_pool		*rvp;
	bool				ret;
	int				error;

	rvp = calloc(1, sizeof(struct read_verify_pool));
	if (!rvp)
		return NULL;

	error = posix_memalign((void **)&rvp->readbuf, page_size,
			RVP_IO_MAX_SIZE);
	if (error || !rvp->readbuf)
		goto out_free;
	rvp->verified_bytes = ptcounter_init(nproc);
	if (!rvp->verified_bytes)
		goto out_buf;
	rvp->miniosz = miniosz;
	rvp->ctx = ctx;
	rvp->ioerr_fn = ioerr_fn;
	/* Run in the main thread if we only want one thread. */
	if (nproc == 1)
		nproc = 0;
	ret = workqueue_create(&rvp->wq, (struct xfs_mount *)rvp, nproc);
	if (ret)
		goto out_counter;
	return rvp;

out_counter:
	ptcounter_free(rvp->verified_bytes);
out_buf:
	free(rvp->readbuf);
out_free:
	free(rvp);
	return NULL;
}

/* Finish up any read verification work. */
void
read_verify_pool_flush(
	struct read_verify_pool		*rvp)
{
	workqueue_destroy(&rvp->wq);
}

/* Finish up any read verification work and tear it down. */
void
read_verify_pool_destroy(
	struct read_verify_pool		*rvp)
{
	ptcounter_free(rvp->verified_bytes);
	free(rvp->readbuf);
	free(rvp);
}

/*
 * Issue a read-verify IO in big batches.
 */
static void
read_verify(
	struct workqueue		*wq,
	xfs_agnumber_t			agno,
	void				*arg)
{
	struct read_verify		*rv = arg;
	struct read_verify_pool		*rvp;
	unsigned long long		verified = 0;
	ssize_t				sz;
	ssize_t				len;

	rvp = (struct read_verify_pool *)wq->wq_ctx;
	while (rv->io_length > 0) {
		len = min(rv->io_length, RVP_IO_MAX_SIZE);
		dbg_printf("diskverify %d %"PRIu64" %zu\n", rv->io_disk->d_fd,
				rv->io_start, len);
		sz = disk_read_verify(rv->io_disk, rvp->readbuf,
				rv->io_start, len);
		if (sz < 0) {
			dbg_printf("IOERR %d %"PRIu64" %zu\n",
					rv->io_disk->d_fd,
					rv->io_start, len);
			/* IO error, so try the next logical block. */
			len = rvp->miniosz;
			rvp->ioerr_fn(rvp->ctx, rv->io_disk, rv->io_start, len,
					errno, rv->io_end_arg);
		}

		progress_add(len);
		verified += len;
		rv->io_start += len;
		rv->io_length -= len;
	}

	free(rv);
	ptcounter_add(rvp->verified_bytes, verified);
}

/* Queue a read verify request. */
static bool
read_verify_queue(
	struct read_verify_pool		*rvp,
	struct read_verify		*rv)
{
	struct read_verify		*tmp;
	bool				ret;

	dbg_printf("verify fd %d start %"PRIu64" len %"PRIu64"\n",
			rv->io_disk->d_fd, rv->io_start, rv->io_length);

	tmp = malloc(sizeof(struct read_verify));
	if (!tmp) {
		rvp->ioerr_fn(rvp->ctx, rv->io_disk, rv->io_start,
				rv->io_length, errno, rv->io_end_arg);
		return true;
	}
	memcpy(tmp, rv, sizeof(*tmp));

	ret = workqueue_add(&rvp->wq, read_verify, 0, tmp);
	if (ret) {
		str_info(rvp->ctx, rvp->ctx->mntpoint,
_("Could not queue read-verify work."));
		free(tmp);
		return false;
	}
	rv->io_length = 0;
	return true;
}

/*
 * Issue an IO request.  We'll batch subsequent requests if they're
 * within 64k of each other
 */
bool
read_verify_schedule_io(
	struct read_verify_pool		*rvp,
	struct read_verify		*rv,
	struct disk			*disk,
	uint64_t			start,
	uint64_t			length,
	void				*end_arg)
{
	uint64_t			req_end;
	uint64_t			rv_end;

	assert(rvp->readbuf);
	req_end = start + length;
	rv_end = rv->io_start + rv->io_length;

	/*
	 * If we have a stashed IO, we haven't changed fds, the error
	 * reporting is the same, and the two extents are close,
	 * we can combine them.
	 */
	if (rv->io_length > 0 && disk == rv->io_disk &&
	    end_arg == rv->io_end_arg &&
	    ((start >= rv->io_start && start <= rv_end + RVP_IO_BATCH_LOCALITY) ||
	     (rv->io_start >= start &&
	      rv->io_start <= req_end + RVP_IO_BATCH_LOCALITY))) {
		rv->io_start = min(rv->io_start, start);
		rv->io_length = max(req_end, rv_end) - rv->io_start;
	} else  {
		/* Otherwise, issue the stashed IO (if there is one) */
		if (rv->io_length > 0)
			return read_verify_queue(rvp, rv);

		/* Stash the new IO. */
		rv->io_disk = disk;
		rv->io_start = start;
		rv->io_length = length;
		rv->io_end_arg = end_arg;
	}

	return true;
}

/* Force any stashed IOs into the verifier. */
bool
read_verify_force_io(
	struct read_verify_pool		*rvp,
	struct read_verify		*rv)
{
	bool				moveon;

	assert(rvp->readbuf);
	if (rv->io_length == 0)
		return true;

	moveon = read_verify_queue(rvp, rv);
	if (moveon)
		rv->io_length = 0;
	return moveon;
}

/* How many bytes has this process verified? */
uint64_t
read_verify_bytes(
	struct read_verify_pool		*rvp)
{
	return ptcounter_value(rvp->verified_bytes);
}
