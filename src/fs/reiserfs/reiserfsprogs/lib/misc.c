/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#define _GNU_SOURCE

#include "misc.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <time.h>
#include <utime.h>
#include <ctype.h>
#include <linux/hdreg.h>
#include <dirent.h>
#include <assert.h>

#include <sys/ioctl.h>
#include <signal.h>

/* Debian modifications by Ed Boraas <ed@debian.org> */
#include <sys/mount.h>
/* End Debian mods */

void die(const char *fmt, ...)
{
	static char buf[1024];
	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	fprintf(stderr, "\n%s\n", buf);
	abort();
}

#define MEM_BEGIN "_mem_begin_"
#define MEM_END "mem_end"
#define MEM_FREED "__free_"
#define CONTROL_SIZE (strlen (MEM_BEGIN) + 1 + sizeof (int) + strlen (MEM_END) + 1)

unsigned int get_mem_size(const char *p)
{
	const char *begin;

	begin = p - strlen(MEM_BEGIN) - 1 - sizeof(int);
	return *(int *)(begin + strlen(MEM_BEGIN) + 1);
}

void checkmem(const char *p, int size)
{
	const char *begin;
	const char *end;

	begin = p - strlen(MEM_BEGIN) - 1 - sizeof(int);
	if (strcmp(begin, MEM_BEGIN))
		die("checkmem: memory corrupted - invalid head sign");

	if (*(int *)(begin + strlen(MEM_BEGIN) + 1) != size)
		die("checkmem: memory corrupted - invalid size");

	end = begin + size + CONTROL_SIZE - strlen(MEM_END) - 1;
	if (strcmp(end, MEM_END))
		die("checkmem: memory corrupted - invalid end sign");
}

void *getmem(int size)
{
	char *mem;

	if ((mem = mem_alloc(size)) == NULL)
		die("getmem: no more memory (%d)", size);

	memset(mem, 0, size);
//    checkmem (mem, size);

	return mem;
}

void *mem_alloc(int size)
{
	char *p;
	char *mem;

	p = (char *)malloc(CONTROL_SIZE + size);
	if (!p)
		die("getmem: no more memory (%d)", size);

	/* Write the MEM_BEGIN magic in the beginning of allocated memory. */
	strcpy(p, MEM_BEGIN);
	p += strlen(MEM_BEGIN) + 1;
	/* Write the size after the magic. */
	*(int *)p = size;
	p += sizeof(int);
	mem = p;
	p += size;
	strcpy(p, MEM_END);

	return mem;
}

void *expandmem(void *vp, int size, int by)
{
	int allocated;
	char *mem, *p = vp;
	int expand_by = by;

	if (p) {
		checkmem(p, size);
		allocated = CONTROL_SIZE + size;
		p -= (strlen(MEM_BEGIN) + 1 + sizeof(int));
	} else {
		allocated = 0;
		/* add control bytes to the new allocated area */
		expand_by += CONTROL_SIZE;
	}
	p = realloc(p, allocated + expand_by);
	if (!p)
		die("expandmem: no more memory (%d)", size);
	if (!vp) {
		strcpy(p, MEM_BEGIN);
	}
	mem = p + strlen(MEM_BEGIN) + 1 + sizeof(int);

	*(int *)(p + strlen(MEM_BEGIN) + 1) = size + by;
	/* fill new allocated area by 0s */
	if (by > 0)
		memset(mem + size, 0, by);
	strcpy(mem + size + by, MEM_END);
//    checkmem (mem, size + by);

	return mem;
}

void freemem(void *vp)
{
	char *p = vp;
	int size;

	if (!p)
		return;
	size = get_mem_size(vp);
	checkmem(p, size);

	p -= (strlen(MEM_BEGIN) + 1 + sizeof(int));
	strcpy(p, MEM_FREED);
	strcpy(p + size + CONTROL_SIZE - strlen(MEM_END) - 1, MEM_FREED);
	free(p);
}

typedef int (*func_t) (const char *);

/* Lookup the @file in the @mntfile. @file is mntent.mnt_fsname if @fsname
   is set; mntent.mnt_dir otherwise. Return the mnt entry from the @mntfile.

   Warning: if the root fs is mounted RO, the content of /etc/mtab may be
   not correct. */
static struct mntent *misc_mntent_lookup(const char *mntfile, const char *file,
					 int path)
{
	struct mntent *mnt;
	int name_match = 0;
	struct stat st;
	dev_t rdev = 0;
	dev_t dev = 0;
	ino_t ino = 0;
	char *name;
	FILE *fp;

	assert(mntfile != NULL);
	assert(file != NULL);

	if (stat(file, &st) == 0) {
		/* Devices is stated. */
		if (S_ISBLK(st.st_mode)) {
			rdev = st.st_rdev;
		} else {
			dev = st.st_dev;
			ino = st.st_ino;
		}
	}

	if ((fp = setmntent(mntfile, "r")) == NULL)
		return INVAL_PTR;

	while ((mnt = getmntent(fp)) != NULL) {
		/* Check if names match. */
		name = path ? mnt->mnt_dir : mnt->mnt_fsname;

		if (strcmp(file, name) == 0)
			name_match = 1;

		if (stat(name, &st))
			continue;

		/* If names do not match, check if stats match. */
		if (!name_match) {
			if (rdev && S_ISBLK(st.st_mode)) {
				if (rdev != st.st_rdev)
					continue;
			} else if (dev && !S_ISBLK(st.st_mode)) {
				if (dev != st.st_dev || ino != st.st_ino)
					continue;
			} else {
				continue;
			}
		}

		/* If not path and not block device do not check anything more. */
		if (!path && !rdev)
			break;

		if (path) {
			/* Either names or stats match. Make sure the st_dev of
			   the path is same as @mnt_fsname device rdev. */
			if (stat(mnt->mnt_fsname, &st) == 0 &&
			    dev == st.st_rdev)
				break;
		} else {
			/* Either names or stats match. Make sure the st_dev of
			   the mount entry is same as the given device rdev. */
			if (stat(mnt->mnt_dir, &st) == 0 && rdev == st.st_dev)
				break;
		}
	}

	endmntent(fp);
	return mnt;
}

static int misc_root_mounted(const char *device)
{
	struct stat rootst, devst;

	assert(device != NULL);

	if (stat("/", &rootst) != 0)
		return -1;

	if (stat(device, &devst) != 0)
		return -1;

	if (!S_ISBLK(devst.st_mode) || devst.st_rdev != rootst.st_dev)
		return 0;

	return 1;
}

static int misc_file_ro(const char *file)
{
	if (utime(file, NULL) == -1) {
		if (errno == EROFS)
			return 1;
	}

	return 0;
}

struct mntent *misc_mntent(const char *device)
{
	int proc = 0, path = 0, root = 0;

	struct mntent *mnt;
	struct statfs stfs;

	assert(device != NULL);

	/* Check if the root. */
	if (misc_root_mounted(device) == 1)
		root = 1;

#ifdef __linux__
	/* Check if /proc is procfs. */
	if (statfs("/proc", &stfs) == 0 && stfs.f_type == 0x9fa0) {
		proc = 1;

		if (root) {
			/* Lookup the "/" entry in /proc/mounts. Special
			   case as root entry can present as:
			   rootfs / rootfs rw 0 0
			   Look up the mount point in this case. */
			mnt = misc_mntent_lookup("/proc/mounts", "/", 1);
		} else {
			/* Lookup the @device /proc/mounts */
			mnt = misc_mntent_lookup("/proc/mounts", device, 0);
		}

		if (mnt == INVAL_PTR)
			proc = 0;
		else if (mnt)
			return mnt;
	}
#endif /* __linux__ */

#if defined(MOUNTED) || defined(_PATH_MOUNTED)

#ifndef MOUNTED
#define MOUNTED _PATH_MOUNTED
#endif
	/* Check in MOUNTED (/etc/mtab) if RW. */
	if (!misc_file_ro(MOUNTED)) {
		path = 1;

		if (root) {
			mnt = misc_mntent_lookup(MOUNTED, "/", 1);
		} else {
			mnt = misc_mntent_lookup(MOUNTED, device, 0);
		}

		if (mnt == INVAL_PTR)
			path = 0;
		else if (mnt)
			return mnt;
	}
#endif /* defined(MOUNTED) || defined(_PATH_MOUNTED) */

	/* If has not been checked in neither /proc/mounts nor /etc/mtab (or
	   errors have occured), return INVAL_PTR, NULL otherwise. */
	return (!proc && !path) ? INVAL_PTR : NULL;
}

int misc_device_mounted(const char *device)
{
	struct mntent *mnt;

	/* Check for the "/" first to avoid any possible problem with
	   reflecting the root fs info in mtab files. */
	if (misc_root_mounted(device) == 1) {
		return misc_file_ro("/") ? MF_RO : MF_RW;
	}

	/* Lookup the mount entry. */
	if ((mnt = misc_mntent(device)) == NULL) {
		return MF_NOT_MOUNTED;
	} else if (mnt == INVAL_PTR) {
		return 0;
	}

	return hasmntopt(mnt, MNTOPT_RO) ? MF_RO : MF_RW;
}

static char buf1[100];
static char buf2[100];

void print_how_fast(unsigned long passed, unsigned long total,
		    int cursor_pos, int reset_time)
{
	static time_t t0 = 0, t1 = 0, t2 = 0;
	int speed;
	int indent;

	if (reset_time)
		time(&t0);

	time(&t1);
	if (t1 != t0) {
		speed = passed / (t1 - t0);
		if (total - passed) {
			if (t1 - t2 < 1)
				return;
			t2 = t1;
		}
	} else
		speed = 0;

	/* what has to be written */
	if (total)
		sprintf(buf1, "left %lu, %d /sec", total - passed, speed);
	else {
		/*(*passed) ++; */
		sprintf(buf1, "done %lu, %d /sec", passed, speed);
	}

	/* make indent */
	indent = 79 - cursor_pos - strlen(buf1);
	memset(buf2, ' ', indent);
	buf2[indent] = 0;
	fprintf(stderr, "%s%s", buf2, buf1);

	memset(buf2, '\b', indent + strlen(buf1));
	buf2[indent + strlen(buf1)] = 0;
	fprintf(stderr, "%s", buf2);
	fflush(stderr);
}

static const char *strs[] =
    { "0%", ".", ".", ".", ".", "20%", ".", ".", ".", ".", "40%", ".", ".", ".",
".", "60%", ".", ".", ".", ".", "80%", ".", ".", ".", ".", "100%" };

static char progress_to_be[1024];
static char current_progress[1024];

static void str_to_be(char *buf, int prosents)
{
	int i;
	prosents -= prosents % 4;
	buf[0] = 0;
	for (i = 0; i <= prosents / 4; i++)
		strcat(buf, strs[i]);
}

void print_how_far(FILE * fp,
		   unsigned long *passed, unsigned long total,
		   unsigned int inc, int quiet)
{
	int percent;

	if (*passed == 0)
		current_progress[0] = 0;

	(*passed) += inc;
	if (*passed > total) {
/*	fprintf (fp, "\nprint_how_far: total %lu has been reached already. cur=%lu\n",
	total, *passed);*/
		return;
	}

	percent = ((*passed) * 100) / total;

	str_to_be(progress_to_be, percent);

	if (strlen(current_progress) != strlen(progress_to_be)) {
		fprintf(fp, "%s", progress_to_be + strlen(current_progress));
	}

	strcat(current_progress, progress_to_be + strlen(current_progress));

	if (!quiet) {
		print_how_fast(*passed /* - inc */ , total,
			       strlen(progress_to_be),
			       (*passed == inc) ? 1 : 0);
	}

	fflush(fp);
}

#if defined(__linux__) && defined(_IOR) && !defined(BLKGETSIZE64)
/* Note! Despite this call being called with *64, it must be encoded to
 * return only sizeof(size_t), since in earlier kernel versions it was
 * declared _IOR(0x12, 114, sizeof(u64)), making it use sizeof(sizeof(u64)).
 *
 * However, the call itself does always return 64bit!
 */
#   define BLKGETSIZE64 _IOR(0x12, 114, size_t)
#endif

/* To not have problem with last sectors on the block device when switching
   to smaller one. */
#define MAX_BS (64 * 1024)

int valid_offset(int fd, loff_t offset)
{
	char ch;
	loff_t res;

	/*res = reiserfs_llseek (fd, offset, 0); */
	res = lseek(fd, offset, SEEK_SET);
	if (res < 0)
		return 0;

	/* if (read (fd, &ch, 1) < 0) does not wirk on files */
	if (read(fd, &ch, 1) < 1)
		return 0;

	return 1;
}

/* calculates number of blocks in a file. Returns 0 for "sparse"
   regular files and files other than regular files and block devices */
unsigned long count_blocks(const char *filename, int blocksize)
{
	loff_t high, low;
	unsigned long sz;
	__u64 size;
	int fd;

	if (!S_ISBLK(misc_device_mode(filename)) &&
	    !S_ISREG(misc_device_mode(filename)))
		return 0;

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open '%s': %s.\n", filename,
			strerror(errno));
		return 0;
	}
#ifdef BLKGETSIZE64
	{
		if (ioctl(fd, BLKGETSIZE64, &size) >= 0) {
			size = (size / MAX_BS) * MAX_BS / blocksize;
			sz = size;
			if ((__u64) sz != size)
				die("count_blocks: block device too large");

			close(fd);
			return sz;
		}
	}
#endif

#ifdef BLKGETSIZE
	{
		if (ioctl(fd, BLKGETSIZE, &sz) >= 0) {
			size = sz;

			close(fd);
			return (size * 512 / MAX_BS) * MAX_BS / blocksize;
		}
	}
#endif

	low = 0;
	for (high = 1; valid_offset(fd, high); high *= 2)
		low = high;
	while (low < high - 1) {
		const loff_t mid = (low + high) / 2;

		if (valid_offset(fd, mid))
			low = mid;
		else
			high = mid;
	}

	valid_offset(fd, 0);

	close(fd);
	return (low + 1) * MAX_BS / MAX_BS / blocksize;
}

/* there are masks for certain bits  */
__u16 mask16(int from, int count)
{
	__u16 mask;

	mask = (0xffff >> from);
	mask <<= from;
	mask <<= (16 - from - count);
	mask >>= (16 - from - count);
	return mask;
}

__u32 mask32(int from, int count)
{
	__u32 mask;

	mask = (0xffffffff >> from);
	mask <<= from;
	mask <<= (32 - from - count);
	mask >>= (32 - from - count);
	return mask;
}

__u64 mask64(int from, int count)
{
	__u64 mask;

	mask = (0xffffffffffffffffLL >> from);
	mask <<= from;
	mask <<= (64 - from - count);
	mask >>= (64 - from - count);
	return mask;
}

__u32 get_random(void)
{
	srandom(time(NULL));
	return random();
}

/* this implements binary search in the array 'base' among 'num' elements each
   of those is 'width' bytes long. 'comp_func' is used to compare keys */
int reiserfs_bin_search(const void *key, void *base, __u32 num, int width,
			__u32 * ppos, comparison_fn_t comp_func)
{
	__u32 rbound, lbound, j;
	int ret;

	if (num == 0 || base == NULL) {
		/* objectid map may be 0 elements long */
		*ppos = 0;
		return POSITION_NOT_FOUND;
	}

	lbound = 0;
	rbound = num - 1;

	for (j = (rbound + lbound) / 2; lbound <= rbound;
	     j = (rbound + lbound) / 2) {
		ret = comp_func((void *)((char *)base + j * width), key);
		if (ret < 0) {	/* second is greater */
			lbound = j + 1;
			continue;

		} else if (ret > 0) {	/* first is greater */
			if (j == 0)
				break;
			rbound = j - 1;
			continue;
		} else {	/* equal */
			*ppos = j;
			return POSITION_FOUND;
		}
	}

	*ppos = lbound;
	return POSITION_NOT_FOUND;
}

#define BLOCKLIST__ELEMENT_NUMBER 10

/*element is block number and device*/
/* XXX ENDIAN CHECK */
int blockdev_list_compare(const void *block1, const void *block2)
{
	if (*(__u32 *) block1 < *(__u32 *) block2)
		return -1;
	if (*(__u32 *) block1 > *(__u32 *) block2)
		return 1;

	if (*((__u32 *) block1 + 1) < *((__u32 *) block2 + 1))
		return -1;
	if (*((__u32 *) block1 + 1) > *((__u32 *) block2 + 1))
		return 1;

	return 0;
}

void blocklist__insert_in_position(void *elem, void **base, __u32 * count,
				   int elem_size, __u32 * position)
{
	if (elem_size == 0)
		return;

	if (*base == NULL)
		*base = getmem(BLOCKLIST__ELEMENT_NUMBER * elem_size);

	if (*count == get_mem_size((void *)*base) / elem_size)
		*base = expandmem(*base, get_mem_size((void *)*base),
				  BLOCKLIST__ELEMENT_NUMBER * elem_size);

	if (*position < *count) {
		memmove(*base + (*position + 1),
			*base + (*position), (*count - *position) * elem_size);
	}

	memcpy(*base + (char)*position * elem_size, elem, elem_size);
	*count += 1;
}

/* 0 - dma is not supported, scsi or regular file */
/* 1 - xt drive                                   */
/* 2 - ide drive */
static void get_dma_support(dma_info_t *dma_info)
{
	if (S_ISREG(dma_info->st.st_mode))
		dma_info->st.st_rdev = dma_info->st.st_dev;

	if (IDE_DISK_MAJOR(major(dma_info->st.st_rdev))) {
		dma_info->support_type = 2;
		return;
	}
#ifdef XT_DISK_MAJOR
	if (major(dma_info->st.st_rdev) == XT_DISK_MAJOR) {
		dma_info->support_type = 1;
		return;
	}
#endif
	dma_info->support_type = 0;
}

/*
 * Return values:
 * 0 - ok;
 * 1 - preparation cannot be done
 * -1 - preparation failed
 */

int prepare_dma_check(dma_info_t *dma_info)
{
	DIR *dir;
	struct dirent *dirent;
	struct stat st;
	dev_t rdev;
	int rem;
	char buf[256];

#ifndef HDIO_GET_DMA
	return -1;
#endif

	if (fstat(dma_info->fd, &dma_info->st))
		die("stat on device failed\n");

	get_dma_support(dma_info);

	/* dma should be supported */
	if (dma_info->support_type == 0)
		return 1;

	if (dma_info->support_type == 2) {
		rdev = dma_info->st.st_rdev;

		if ((rem = (minor(rdev) % 64)) != 0) {
			rdev -= rem;
			if (!(dir = opendir("/dev/"))) {
				dma_info->support_type = 1;
				return 0;
			}

			while ((dirent = readdir(dir)) != NULL) {
				if (strncmp(dirent->d_name, ".", 1) == 0
				    || strncmp(dirent->d_name, "..", 2) == 0)
					continue;
				memset(buf, 0, 256);
				strncat(buf, "/dev/", 5);
				strncat(buf, dirent->d_name,
					strlen(dirent->d_name));

				if (stat(buf, &st))
					break;

				if (S_ISBLK(st.st_mode) && st.st_rdev == rdev) {
					dma_info->st = st;
					dma_info->fd = open(buf, O_RDONLY
#if defined(O_LARGEFILE)
							    | O_LARGEFILE
#endif
					    );
					closedir(dir);
					return 0;
				}
			}
			closedir(dir);
			dma_info->support_type = 1;
			return 1;
		}
	}

	return 0;
}

static int is_dma_on(int fd)
{
#ifdef HDIO_GET_DMA
	static long parm;
	if (ioctl(fd, HDIO_GET_DMA, &parm))
		return -1;
	else
		return parm;
#endif
	return 0;
}

static __u64 dma_speed(int fd, int support_type)
{
	static struct hd_driveid id;
	__u64 speed = 0;

	if (support_type != 2)
		return 0;

#ifdef HDIO_OBSOLETE_IDENTITY
	if (!ioctl(fd, HDIO_GET_IDENTITY, &id) ||
	    !ioctl(fd, HDIO_OBSOLETE_IDENTITY)) {
#else
	if (!ioctl(fd, HDIO_GET_IDENTITY, &id)) {
#endif
		speed |= (__u64) id.dma_1word & ~(__u64) 0xff;
		speed |= ((__u64) id.dma_mword & ~(__u64) 0xff) << 16;
		speed |= ((__u64) id.dma_ultra & ~(__u64) 0xff) << 32;
	} else if (errno == -ENOMSG)
		return -1;
	else
		return -1;

	return speed;
}

int get_dma_info(dma_info_t *dma_info)
{
	if ((dma_info->dma = is_dma_on(dma_info->fd)) == -1)
		return -1;
	if ((dma_info->speed =
	     dma_speed(dma_info->fd, dma_info->support_type)) == (__u64) - 1)
		return -1;
	return 0;
}

void clean_after_dma_check(int fd, dma_info_t *dma_info)
{
	signal(SIGALRM, SIG_IGN);
	if (dma_info->fd && fd != dma_info->fd)
		close(dma_info->fd);
}

int user_confirmed(FILE * fp, const char *q, const char *yes)
{
	char *answer = NULL;
	size_t n = 0;

	fprintf(fp, "%s", q);
	if (getline(&answer, &n, stdin) != (ssize_t) strlen(yes)
	    || strcmp(yes, answer))
		return 0;

	return 1;
}
