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
#ifdef HAVE_LIBATTR
# include <attr/attributes.h>
#endif
#include "xfs.h"
#include "handle.h"
#include "list.h"
#include "path.h"
#include "workqueue.h"
#include "xfs_scrub.h"
#include "common.h"
#include "inodes.h"
#include "progress.h"
#include "scrub.h"
#include "unicrash.h"

/* Phase 5: Check directory connectivity. */

/*
 * Warn about problematic bytes in a directory/attribute name.  That means
 * terminal control characters and escape sequences, since that could be used
 * to do something naughty to the user's computer and/or break scripts.  XFS
 * doesn't consider any byte sequence invalid, so don't flag these as errors.
 */
static bool
xfs_scrub_check_name(
	struct scrub_ctx	*ctx,
	const char		*descr,
	const char		*namedescr,
	const char		*name)
{
	const char		*p;
	bool			bad = false;
	char			*errname;

	/* Complain about zero length names. */
	if (*name == '\0' && should_warn_about_name(ctx)) {
		str_warn(ctx, descr, _("Zero length name found."));
		return true;
	}

	/* control characters */
	for (p = name; *p; p++) {
		if ((*p >= 1 && *p <= 31) || *p == 127) {
			bad = true;
			break;
		}
	}

	if (bad && should_warn_about_name(ctx)) {
		errname = string_escape(name);
		if (!errname) {
			str_errno(ctx, descr);
			return false;
		}
		str_info(ctx, descr,
_("Control character found in %s name \"%s\"."),
				namedescr, errname);
		free(errname);
	}

	return true;
}

/*
 * Iterate a directory looking for filenames with problematic
 * characters.
 */
static bool
xfs_scrub_scan_dirents(
	struct scrub_ctx	*ctx,
	const char		*descr,
	int			*fd,
	struct xfs_bstat	*bstat)
{
	struct unicrash		*uc = NULL;
	DIR			*dir;
	struct dirent		*dentry;
	bool			moveon = true;

	dir = fdopendir(*fd);
	if (!dir) {
		str_errno(ctx, descr);
		goto out;
	}
	*fd = -1; /* closedir will close *fd for us */

	moveon = unicrash_dir_init(&uc, ctx, bstat);
	if (!moveon)
		goto out_unicrash;

	dentry = readdir(dir);
	while (dentry) {
		moveon = xfs_scrub_check_name(ctx, descr, _("directory"),
				dentry->d_name);
		if (!moveon)
			break;
		moveon = unicrash_check_dir_name(uc, descr, dentry);
		if (!moveon)
			break;
		dentry = readdir(dir);
	}
	unicrash_free(uc);

out_unicrash:
	closedir(dir);
out:
	return moveon;
}

#ifdef HAVE_LIBATTR
/* Routines to scan all of an inode's xattrs for name problems. */
struct xfs_attr_ns {
	int			flags;
	const char		*name;
};

static const struct xfs_attr_ns attr_ns[] = {
	{0,			"user"},
	{ATTR_ROOT,		"system"},
	{ATTR_SECURE,		"secure"},
	{0, NULL},
};

/*
 * Check all the xattr names in a particular namespace of a file handle
 * for Unicode normalization problems or collisions.
 */
static bool
xfs_scrub_scan_fhandle_namespace_xattrs(
	struct scrub_ctx		*ctx,
	const char			*descr,
	struct xfs_handle		*handle,
	struct xfs_bstat		*bstat,
	const struct xfs_attr_ns	*attr_ns)
{
	struct attrlist_cursor		cur;
	char				attrbuf[XFS_XATTR_LIST_MAX];
	char				keybuf[NAME_MAX + 1];
	struct attrlist			*attrlist = (struct attrlist *)attrbuf;
	struct attrlist_ent		*ent;
	struct unicrash			*uc;
	bool				moveon = true;
	int				i;
	int				error;

	moveon = unicrash_xattr_init(&uc, ctx, bstat);
	if (!moveon)
		return false;

	memset(attrbuf, 0, XFS_XATTR_LIST_MAX);
	memset(&cur, 0, sizeof(cur));
	memset(keybuf, 0, NAME_MAX + 1);
	error = attr_list_by_handle(handle, sizeof(*handle), attrbuf,
			XFS_XATTR_LIST_MAX, attr_ns->flags, &cur);
	while (!error) {
		/* Examine the xattrs. */
		for (i = 0; i < attrlist->al_count; i++) {
			ent = ATTR_ENTRY(attrlist, i);
			snprintf(keybuf, NAME_MAX, "%s.%s", attr_ns->name,
					ent->a_name);
			moveon = xfs_scrub_check_name(ctx, descr,
					_("extended attribute"), keybuf);
			if (!moveon)
				goto out;
			moveon = unicrash_check_xattr_name(uc, descr, keybuf);
			if (!moveon)
				goto out;
		}

		if (!attrlist->al_more)
			break;
		error = attr_list_by_handle(handle, sizeof(*handle), attrbuf,
				XFS_XATTR_LIST_MAX, attr_ns->flags, &cur);
	}
	if (error && errno != ESTALE)
		str_errno(ctx, descr);
out:
	unicrash_free(uc);
	return moveon;
}

/*
 * Check all the xattr names in all the xattr namespaces for problematic
 * characters.
 */
static bool
xfs_scrub_scan_fhandle_xattrs(
	struct scrub_ctx		*ctx,
	const char			*descr,
	struct xfs_handle		*handle,
	struct xfs_bstat		*bstat)
{
	const struct xfs_attr_ns	*ns;
	bool				moveon = true;

	for (ns = attr_ns; ns->name; ns++) {
		moveon = xfs_scrub_scan_fhandle_namespace_xattrs(ctx, descr,
				handle, bstat, ns);
		if (!moveon)
			break;
	}
	return moveon;
}
#else
# define xfs_scrub_scan_fhandle_xattrs(c, d, h, b)	(true)
#endif /* HAVE_LIBATTR */

/*
 * Verify the connectivity of the directory tree.
 * We know that the kernel's open-by-handle function will try to reconnect
 * parents of an opened directory, so we'll accept that as sufficient.
 *
 * Check for potential Unicode collisions in names.
 */
static int
xfs_scrub_connections(
	struct scrub_ctx	*ctx,
	struct xfs_handle	*handle,
	struct xfs_bstat	*bstat,
	void			*arg)
{
	bool			*pmoveon = arg;
	char			descr[DESCR_BUFSZ];
	bool			moveon;
	xfs_agnumber_t		agno;
	xfs_agino_t		agino;
	int			fd = -1;

	agno = bstat->bs_ino / (1ULL << (ctx->inopblog + ctx->agblklog));
	agino = bstat->bs_ino % (1ULL << (ctx->inopblog + ctx->agblklog));
	snprintf(descr, DESCR_BUFSZ, _("inode %"PRIu64" (%u/%u)"),
			(uint64_t)bstat->bs_ino, agno, agino);
	background_sleep();

	/* Warn about naming problems in xattrs. */
	moveon = xfs_scrub_scan_fhandle_xattrs(ctx, descr, handle, bstat);
	if (!moveon)
		goto out;

	/* Open the dir, let the kernel try to reconnect it to the root. */
	if (S_ISDIR(bstat->bs_mode)) {
		fd = xfs_open_handle(handle);
		if (fd < 0) {
			if (errno == ESTALE)
				return ESTALE;
			str_errno(ctx, descr);
			goto out;
		}
	}

	/* Warn about naming problems in the directory entries. */
	if (fd >= 0 && S_ISDIR(bstat->bs_mode)) {
		moveon = xfs_scrub_scan_dirents(ctx, descr, &fd, bstat);
		if (!moveon)
			goto out;
	}

out:
	progress_add(1);
	if (fd >= 0)
		close(fd);
	if (!moveon)
		*pmoveon = false;
	return *pmoveon ? 0 : XFS_ITERATE_INODES_ABORT;
}

/* Check directory connectivity. */
bool
xfs_scan_connections(
	struct scrub_ctx	*ctx)
{
	bool			moveon = true;
	bool			ret;

	if (ctx->errors_found) {
		str_info(ctx, ctx->mntpoint,
_("Filesystem has errors, skipping connectivity checks."));
		return true;
	}

	ret = xfs_scan_all_inodes(ctx, xfs_scrub_connections, &moveon);
	if (!ret)
		moveon = false;
	if (!moveon)
		return false;
	xfs_scrub_report_preen_triggers(ctx);
	return true;
}
