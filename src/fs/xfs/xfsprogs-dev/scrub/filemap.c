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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include "xfs.h"
#include "xfs_fs.h"
#include "path.h"
#include "xfs_scrub.h"
#include "common.h"
#include "filemap.h"

/*
 * These routines provide a simple interface to query the block
 * mappings of the fork of a given inode via GETBMAPX and call a
 * function to iterate each mapping result.
 */

#define BMAP_NR		2048

/* Iterate all the extent block mappings between the key and fork end. */
bool
xfs_iterate_filemaps(
	struct scrub_ctx	*ctx,
	const char		*descr,
	int			fd,
	int			whichfork,
	struct xfs_bmap		*key,
	xfs_bmap_iter_fn	fn,
	void			*arg)
{
	struct fsxattr		fsx;
	struct getbmapx		*map;
	struct getbmapx		*p;
	struct xfs_bmap		bmap;
	char			bmap_descr[DESCR_BUFSZ];
	bool			moveon = true;
	xfs_off_t		new_off;
	int			getxattr_type;
	int			i;
	int			error;

	switch (whichfork) {
	case XFS_ATTR_FORK:
		snprintf(bmap_descr, DESCR_BUFSZ, _("%s attr"), descr);
		break;
	case XFS_COW_FORK:
		snprintf(bmap_descr, DESCR_BUFSZ, _("%s CoW"), descr);
		break;
	case XFS_DATA_FORK:
		snprintf(bmap_descr, DESCR_BUFSZ, _("%s data"), descr);
		break;
	default:
		abort();
	}

	map = calloc(BMAP_NR, sizeof(struct getbmapx));
	if (!map) {
		str_errno(ctx, bmap_descr);
		return false;
	}

	map->bmv_offset = BTOBB(key->bm_offset);
	map->bmv_block = BTOBB(key->bm_physical);
	if (key->bm_length == 0)
		map->bmv_length = ULLONG_MAX;
	else
		map->bmv_length = BTOBB(key->bm_length);
	map->bmv_count = BMAP_NR;
	map->bmv_iflags = BMV_IF_NO_DMAPI_READ | BMV_IF_PREALLOC |
			  BMV_IF_NO_HOLES;
	switch (whichfork) {
	case XFS_ATTR_FORK:
		getxattr_type = XFS_IOC_FSGETXATTRA;
		map->bmv_iflags |= BMV_IF_ATTRFORK;
		break;
	case XFS_COW_FORK:
		map->bmv_iflags |= BMV_IF_COWFORK;
		getxattr_type = FS_IOC_FSGETXATTR;
		break;
	case XFS_DATA_FORK:
		getxattr_type = FS_IOC_FSGETXATTR;
		break;
	default:
		abort();
	}

	error = ioctl(fd, getxattr_type, &fsx);
	if (error < 0) {
		str_errno(ctx, bmap_descr);
		moveon = false;
		goto out;
	}

	while ((error = ioctl(fd, XFS_IOC_GETBMAPX, map)) == 0) {
		for (i = 0, p = &map[i + 1]; i < map->bmv_entries; i++, p++) {
			bmap.bm_offset = BBTOB(p->bmv_offset);
			bmap.bm_physical = BBTOB(p->bmv_block);
			bmap.bm_length = BBTOB(p->bmv_length);
			bmap.bm_flags = p->bmv_oflags;
			moveon = fn(ctx, bmap_descr, fd, whichfork, &fsx,
					&bmap, arg);
			if (!moveon)
				goto out;
			if (xfs_scrub_excessive_errors(ctx)) {
				moveon = false;
				goto out;
			}
		}

		if (map->bmv_entries == 0)
			break;
		p = map + map->bmv_entries;
		if (p->bmv_oflags & BMV_OF_LAST)
			break;

		new_off = p->bmv_offset + p->bmv_length;
		map->bmv_length -= new_off - map->bmv_offset;
		map->bmv_offset = new_off;
	}

	/*
	 * Pre-reflink filesystems don't know about CoW forks, so don't
	 * be too surprised if it fails.
	 */
	if (whichfork == XFS_COW_FORK && error && errno == EINVAL)
		error = 0;

	if (error)
		str_errno(ctx, bmap_descr);
out:
	free(map);
	return moveon;
}
