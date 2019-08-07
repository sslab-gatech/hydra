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
#include <pthread.h>
#include <stdbool.h>
#include <sys/statvfs.h>
#include "platform_defs.h"
#include "xfs.h"
#include "xfs_fs.h"
#include "path.h"
#include "xfs_scrub.h"
#include "common.h"
#include "progress.h"

/*
 * Reporting Status to the Console
 *
 * We aim for a roughly standard reporting format -- the severity of the
 * status being reported, a textual description of the object being
 * reported, and whatever the status happens to be.
 *
 * Errors are the most severe and reflect filesystem corruption.
 * Warnings indicate that something is amiss and needs the attention of
 * the administrator, but does not constitute a corruption.  Information
 * is merely advisory.
 */

/* Too many errors? Bail out. */
bool
xfs_scrub_excessive_errors(
	struct scrub_ctx	*ctx)
{
	bool			ret;

	pthread_mutex_lock(&ctx->lock);
	ret = ctx->max_errors > 0 && ctx->errors_found >= ctx->max_errors;
	pthread_mutex_unlock(&ctx->lock);

	return ret;
}

static const char *err_str[] = {
	[S_ERROR]	= "Error",
	[S_WARN]	= "Warning",
	[S_REPAIR]	= "Repaired",
	[S_INFO]	= "Info",
	[S_PREEN]	= "Optimized",
};

/* If stream is a tty, clear to end of line to clean up progress bar. */
static inline const char *stream_start(FILE *stream)
{
	if (stream == stderr)
		return stderr_isatty ? CLEAR_EOL : "";
	return stdout_isatty ? CLEAR_EOL : "";
}

/* Print a warning string and some warning text. */
void
__str_out(
	struct scrub_ctx	*ctx,
	const char		*descr,
	enum error_level	level,
	int			error,
	const char		*file,
	int			line,
	const char		*format,
	...)
{
	FILE			*stream = stderr;
	va_list			args;
	char			buf[DESCR_BUFSZ];

	/* print strerror or format of choice but not both */
	assert(!(error && format));

	if (level >= S_INFO)
		stream = stdout;

	pthread_mutex_lock(&ctx->lock);

	/* We only want to hear about optimizing when in debug/verbose mode. */
	if (level == S_PREEN && !debug && !verbose)
		goto out_record;

	fprintf(stream, "%s%s: %s: ", stream_start(stream), _(err_str[level]),
			descr);
	if (error) {
		fprintf(stream, _("%s."), strerror_r(error, buf, DESCR_BUFSZ));
	} else {
		va_start(args, format);
		vfprintf(stream, format, args);
		va_end(args);
	}

	if (debug)
		fprintf(stream, _(" (%s line %d)"), file, line);
	fprintf(stream, "\n");
	if (stream == stdout)
		fflush(stream);

out_record:
	if (error)      /* A syscall failed */
		ctx->runtime_errors++;
	else if (level == S_ERROR)
		ctx->errors_found++;
	else if (level == S_WARN)
		ctx->warnings_found++;
	else if (level == S_REPAIR)
		ctx->repairs++;
	else if (level == S_PREEN)
		ctx->preens++;

	pthread_mutex_unlock(&ctx->lock);
}

double
timeval_subtract(
	struct timeval		*tv1,
	struct timeval		*tv2)
{
	return ((tv1->tv_sec - tv2->tv_sec) +
		((float) (tv1->tv_usec - tv2->tv_usec)) / 1000000);
}

/* Produce human readable disk space output. */
double
auto_space_units(
	unsigned long long	bytes,
	char			**units)
{
	if (debug > 1)
		goto no_prefix;
	if (bytes > (1ULL << 40)) {
		*units = "TiB";
		return (double)bytes / (1ULL << 40);
	} else if (bytes > (1ULL << 30)) {
		*units = "GiB";
		return (double)bytes / (1ULL << 30);
	} else if (bytes > (1ULL << 20)) {
		*units = "MiB";
		return (double)bytes / (1ULL << 20);
	} else if (bytes > (1ULL << 10)) {
		*units = "KiB";
		return (double)bytes / (1ULL << 10);
	}

no_prefix:
	*units = "B";
	return bytes;
}

/* Produce human readable discrete number output. */
double
auto_units(
	unsigned long long	number,
	char			**units)
{
	if (debug > 1)
		goto no_prefix;
	if (number > 1000000000000ULL) {
		*units = "T";
		return number / 1000000000000.0;
	} else if (number > 1000000000ULL) {
		*units = "G";
		return number / 1000000000.0;
	} else if (number > 1000000ULL) {
		*units = "M";
		return number / 1000000.0;
	} else if (number > 1000ULL) {
		*units = "K";
		return number / 1000.0;
	}

no_prefix:
	*units = "";
	return number;
}

/* How many threads to kick off? */
unsigned int
scrub_nproc(
	struct scrub_ctx	*ctx)
{
	if (nr_threads)
		return nr_threads;
	return ctx->nr_io_threads;
}

/*
 * How many threads to kick off for a workqueue?  If we only want one
 * thread, save ourselves the overhead and just run it in the main thread.
 */
unsigned int
scrub_nproc_workqueue(
	struct scrub_ctx	*ctx)
{
	unsigned int		x;

	x = scrub_nproc(ctx);
	if (x == 1)
		x = 0;
	return x;
}

/*
 * Check if the argument is either the device name or mountpoint of a mounted
 * filesystem.
 */
#define MNTTYPE_XFS	"xfs"
static bool
find_mountpoint_check(
	struct stat		*sb,
	struct mntent		*t)
{
	struct stat		ms;

	if (S_ISDIR(sb->st_mode)) {		/* mount point */
		if (stat(t->mnt_dir, &ms) < 0)
			return false;
		if (sb->st_ino != ms.st_ino)
			return false;
		if (sb->st_dev != ms.st_dev)
			return false;
		if (strcmp(t->mnt_type, MNTTYPE_XFS) != 0)
			return NULL;
	} else {				/* device */
		if (stat(t->mnt_fsname, &ms) < 0)
			return false;
		if (sb->st_rdev != ms.st_rdev)
			return false;
		if (strcmp(t->mnt_type, MNTTYPE_XFS) != 0)
			return NULL;
		/*
		 * Make sure the mountpoint given by mtab is accessible
		 * before using it.
		 */
		if (stat(t->mnt_dir, &ms) < 0)
			return false;
	}

	return true;
}

/* Check that our alleged mountpoint is in mtab */
bool
find_mountpoint(
	char			*mtab,
	struct scrub_ctx	*ctx)
{
	struct mntent_cursor	cursor;
	struct mntent		*t = NULL;
	bool			found = false;

	if (platform_mntent_open(&cursor, mtab) != 0) {
		fprintf(stderr, "Error: can't get mntent entries.\n");
		exit(1);
	}

	while ((t = platform_mntent_next(&cursor)) != NULL) {
		/*
		 * Keep jotting down matching mount details; newer mounts are
		 * towards the end of the file (hopefully).
		 */
		if (find_mountpoint_check(&ctx->mnt_sb, t)) {
			ctx->mntpoint = strdup(t->mnt_dir);
			ctx->blkdev = strdup(t->mnt_fsname);
			found = true;
		}
	}
	platform_mntent_close(&cursor);
	return found;
}

/*
 * Sleep for 100ms * however many -b we got past the initial one.
 * This is an (albeit clumsy) way to throttle scrub activity.
 */
void
background_sleep(void)
{
	unsigned long long	time;
	struct timespec		tv;

	if (bg_mode < 2)
		return;

	time = 100000ULL * (bg_mode - 1);
	tv.tv_sec = time / 1000000;
	tv.tv_nsec = time % 1000000;
	nanosleep(&tv, NULL);
}

/*
 * Return the input string with non-printing bytes escaped.
 * Caller must free the buffer.
 */
char *
string_escape(
	const char		*in)
{
	char			*str;
	const char		*p;
	char			*q;
	int			x;

	str = malloc(strlen(in) * 4);
	if (!str)
		return NULL;
	for (p = in, q = str; *p != '\0'; p++) {
		if (isprint(*p)) {
			*q = *p;
			q++;
		} else {
			x = sprintf(q, "\\x%02x", *p);
			q += x;
		}
	}
	*q = '\0';
	return str;
}

/*
 * Record another naming warning, and decide if it's worth
 * complaining about.
 */
bool
should_warn_about_name(
	struct scrub_ctx	*ctx)
{
	bool			whine;
	bool			res;

	pthread_mutex_lock(&ctx->lock);
	ctx->naming_warnings++;
	whine = ctx->naming_warnings == TOO_MANY_NAME_WARNINGS;
	res = ctx->naming_warnings < TOO_MANY_NAME_WARNINGS;
	pthread_mutex_unlock(&ctx->lock);

	if (whine && !(debug || verbose))
		str_info(ctx, ctx->mntpoint,
_("More than %u naming warnings, shutting up."),
				TOO_MANY_NAME_WARNINGS);

	return debug || verbose || res;
}

/* Decide if a value is within +/- (n/d) of a desired value. */
bool
within_range(
	struct scrub_ctx	*ctx,
	unsigned long long	value,
	unsigned long long	desired,
	unsigned long long	abs_threshold,
	unsigned int		n,
	unsigned int		d,
	const char		*descr)
{
	assert(n < d);

	/* Don't complain if difference does not exceed an absolute value. */
	if (value < desired && desired - value < abs_threshold)
		return true;
	if (value > desired && value - desired < abs_threshold)
		return true;

	/* Complain if the difference exceeds a certain percentage. */
	if (value < desired * (d - n) / d)
		return false;
	if (value > desired * (d + n) / d)
		return false;

	return true;
}
