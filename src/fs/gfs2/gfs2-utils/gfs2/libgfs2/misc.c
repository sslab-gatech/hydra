#include "clusterautoconfig.h"

#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "libgfs2.h"

#define PAGE_SIZE (4096)
#define DIV_RU(x, y) (((x) + (y) - 1) / (y))

int compute_heightsize(unsigned bsize, uint64_t *heightsize,
	uint32_t *maxheight, uint32_t bsize1, int diptrs, int inptrs)
{
	heightsize[0] = bsize - sizeof(struct gfs2_dinode);
	heightsize[1] = (uint64_t)bsize1 * diptrs;
	for (*maxheight = 2;; (*maxheight)++) {
		uint64_t space, d;
		uint32_t m;

		space = heightsize[*maxheight - 1] * inptrs;
		m = space % inptrs;
		d = space / inptrs;

		if (d != heightsize[*maxheight - 1] || m)
			break;
		heightsize[*maxheight] = space;
	}
	if (*maxheight > GFS2_MAX_META_HEIGHT) {
		errno = EINVAL;
		return -1;
	}
	return 0;
}

int compute_constants(struct gfs2_sbd *sdp)
{
	uint32_t hash_blocks, ind_blocks, leaf_blocks;
	uint32_t tmp_blocks;

	sdp->md.next_inum = 1;

	sdp->sd_sb.sb_bsize_shift = ffs(sdp->bsize) - 1;
	sdp->sd_fsb2bb_shift = sdp->sd_sb.sb_bsize_shift -
		GFS2_BASIC_BLOCK_SHIFT;
	sdp->sd_fsb2bb = 1 << sdp->sd_fsb2bb_shift;
	sdp->sd_diptrs = (sdp->bsize - sizeof(struct gfs2_dinode)) /
		sizeof(uint64_t);
	sdp->sd_inptrs = (sdp->bsize - sizeof(struct gfs2_meta_header)) /
		sizeof(uint64_t);
	sdp->sd_jbsize = sdp->bsize - sizeof(struct gfs2_meta_header);
	sdp->sd_hash_bsize = sdp->bsize / 2;
	sdp->sd_hash_bsize_shift = sdp->sd_sb.sb_bsize_shift - 1;
	sdp->sd_hash_ptrs = sdp->sd_hash_bsize / sizeof(uint64_t);
	sdp->sd_blocks_per_bitmap = (sdp->sd_sb.sb_bsize - sizeof(struct gfs2_meta_header))
	                             * GFS2_NBBY;

	/*  Compute maximum reservation required to add a entry to a directory  */

	hash_blocks = DIV_RU(sizeof(uint64_t) * (1 << GFS2_DIR_MAX_DEPTH),
			     sdp->sd_jbsize);

	ind_blocks = 0;
	for (tmp_blocks = hash_blocks; tmp_blocks > sdp->sd_diptrs;) {
		tmp_blocks = DIV_RU(tmp_blocks, sdp->sd_inptrs);
		ind_blocks += tmp_blocks;
	}

	leaf_blocks = 2 + GFS2_DIR_MAX_DEPTH;

	sdp->sd_max_dirres = hash_blocks + ind_blocks + leaf_blocks;

	if (compute_heightsize(sdp->bsize, sdp->sd_heightsize, &sdp->sd_max_height,
				sdp->bsize, sdp->sd_diptrs, sdp->sd_inptrs)) {
		return -1;
	}
	if (compute_heightsize(sdp->bsize, sdp->sd_jheightsize, &sdp->sd_max_jheight,
				sdp->sd_jbsize, sdp->sd_diptrs, sdp->sd_inptrs)) {
		return -1;
	}
	return 0;
}

/* Returns 0 if fd1 and fd2 refer to the same device/file, 1 otherwise, or -1 on error */
static int fdcmp(int fd1, int fd2)
{
	struct stat st1, st2;

	if (fd1 < 0 || fd2 < 0)
		return -1;
	if ((fstat(fd1, &st1) != 0) || (fstat(fd2, &st2) != 0))
		return -1;
	if (S_ISBLK(st1.st_mode) && S_ISBLK(st2.st_mode)) {
		if (st1.st_rdev == st2.st_rdev) {
			return 0;
		}
	} else if ((st1.st_dev == st2.st_dev) && (st1.st_ino == st2.st_ino)) {
			return 0;
	}
	return 1;
}

int lgfs2_open_mnt(const char *path, int dirflags, int *dirfd, int devflags, int *devfd, struct mntent **mnt)
{
	FILE *fp = setmntent("/proc/mounts", "r");
	if (fp == NULL) {
		perror("open: /proc/mounts");
		return 1;
	}
	/* Assume path is mount point until we know better. */
	*dirfd = open(path, dirflags);
	if (*dirfd < 0)
		return 1;

	while ((*mnt = getmntent(fp)) != NULL) {
		int fd;
		if (strcmp((*mnt)->mnt_type, "gfs2") != 0)
			continue;
		*devfd = open((*mnt)->mnt_fsname, devflags);
		/* Defer checking *devfd until later: whether it's ok to ignore
		 * the error depends on whether we find the mount point. */

		if (strcmp(path, (*mnt)->mnt_dir) == 0)
			break;
		if (strcmp(path, (*mnt)->mnt_fsname) == 0 || fdcmp(*dirfd, *devfd) == 0) {
			/* We have a match but our above assumption was
			   incorrect and *dirfd is actually the device. */
			close(*dirfd);
			*dirfd = open((*mnt)->mnt_dir, dirflags);
			break;
		}

		fd = open((*mnt)->mnt_dir, dirflags);
		if (fd >= 0) {
			int diff = fdcmp(*dirfd, fd);
			close(fd);
			if (diff == 0)
				break;
		}
		if (*devfd >= 0)
			close(*devfd);
	}
	endmntent(fp);
	if (*mnt == NULL) {
		close(*dirfd);
		return 0; /* Success. Answer is no. Both fds closed. */
	}
	if (*dirfd < 0) {
		close(*devfd);
		return 1;
	}
	if (*devfd < 0) {
		close(*dirfd);
		return 1;
	}
	return 0; /* Success. Answer is yes. Both fds open. */
}

int lgfs2_open_mnt_dev(const char *path, int flags, struct mntent **mnt)
{
	int dirfd = -1;
	int devfd = -1;
	if (lgfs2_open_mnt(path, O_RDONLY, &dirfd, flags, &devfd, mnt) != 0)
		return -1;
	if (*mnt != NULL)
		close(dirfd);
	return devfd;
}

int lgfs2_open_mnt_dir(const char *path, int flags, struct mntent **mnt)
{
	int dirfd = -1;
	int devfd = -1;
	if (lgfs2_open_mnt(path, flags, &dirfd, O_RDONLY, &devfd, mnt) != 0)
		return -1;
	if (*mnt != NULL)
		close(devfd);
	return dirfd;
}
