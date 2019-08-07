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
#ifndef XFS_SCRUB_UNICRASH_H_
#define XFS_SCRUB_UNICRASH_H_

struct unicrash;

/* Unicode name collision detection. */
#ifdef HAVE_U8NORMALIZE

struct dirent;

bool unicrash_dir_init(struct unicrash **ucp, struct scrub_ctx *ctx,
		struct xfs_bstat *bstat);
bool unicrash_xattr_init(struct unicrash **ucp, struct scrub_ctx *ctx,
		struct xfs_bstat *bstat);
void unicrash_free(struct unicrash *uc);
bool unicrash_check_dir_name(struct unicrash *uc, const char *descr,
		struct dirent *dirent);
bool unicrash_check_xattr_name(struct unicrash *uc, const char *descr,
		const char *attrname);
#else
# define unicrash_dir_init(u, c, b)		(true)
# define unicrash_xattr_init(u, c, b)		(true)
# define unicrash_free(u)			do {(u) = (u);} while (0)
# define unicrash_check_dir_name(u, d, n)	(true)
# define unicrash_check_xattr_name(u, d, n)	(true)
#endif /* HAVE_U8NORMALIZE */

#endif /* XFS_SCRUB_UNICRASH_H_ */
