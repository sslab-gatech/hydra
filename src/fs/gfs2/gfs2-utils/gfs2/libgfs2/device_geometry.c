#include "clusterautoconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/fs.h>

#include "libgfs2.h"
#include "config.h"

#ifndef BLKSSZGET
#define BLKSSZGET _IO(0x12,104)   /* logical_block_size */
#endif

#ifndef BLKIOMIN
#define BLKIOMIN _IO(0x12,120)    /* minimum_io_size */
#endif

#ifndef BLKIOOPT
#define BLKIOOPT _IO(0x12,121)    /* optimal_io_size */
#endif

#ifndef BLKALIGNOFF
#define BLKALIGNOFF _IO(0x12,122) /* alignment_offset */
#endif

#ifndef BLKPBSZGET
#define BLKPBSZGET _IO(0x12,123)  /* physical_block_size */
#endif

int lgfs2_get_dev_info(int fd, struct lgfs2_dev_info *i)
{
	int ret;
	int ro = 0;
	off_t off;

	memset(i, 0, sizeof(*i));

	ret = fstat(fd, &i->stat);
	if (ret < 0)
		return ret;

	switch (i->stat.st_mode & S_IFMT) {
	case S_IFREG:
		i->size = i->stat.st_size;
		ret = fcntl(fd, F_GETFL, 0);
		if ((ret & O_ACCMODE) == O_RDONLY)
			i->readonly = 1;
		i->io_optimal_size = i->stat.st_blksize;
		goto size_check;
	case S_IFBLK:
		break;
	default:
		errno = ENOTBLK;
		return -1;
	}

	ioctl(fd, BLKRAGET, &i->ra_pages);
	ioctl(fd, BLKBSZGET, &i->soft_block_size);
	ioctl(fd, BLKSSZGET, &i->logical_block_size);
	ioctl(fd, BLKIOMIN, &i->io_min_size);
	ioctl(fd, BLKIOOPT, &i->io_optimal_size);
	ioctl(fd, BLKPBSZGET, &i->physical_block_size);
	ioctl(fd, BLKALIGNOFF, &i->io_align_offset);
	ioctl(fd, BLKROGET, &ro);
	if (ro)
		i->readonly = 1;
	off = lseek(fd, 0, SEEK_END);
	if (off < 0)
		return -1;
	i->size = off;

size_check:
	if (i->size < (1 << 20)) {
		errno = ENOSPC;
		return -1;
	}

	return 0;
}

/**
 * fix_device_geometry - round off address and lengths and convert to FS blocks
 * @sdp: The super block
 *
 */

void fix_device_geometry(struct gfs2_sbd *sdp)
{
	struct device *device = &sdp->device;

	device->length = sdp->dinfo.size / sdp->bsize;

	if (cfg_debug) {
		printf("\nDevice Geometry:  (in FS blocks)\n");
		printf("  length = %"PRIu64"\n", device->length);
		printf("\nDevice Size: %"PRIu64"\n", sdp->dinfo.size);
	}
}
