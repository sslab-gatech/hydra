/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

/* nothing abount reiserfs here */

#ifndef REISERFSPROGS_MISC_H
#define REISERFSPROGS_MISC_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

#include <linux/major.h>

#if defined(MAJOR_IN_MKDEV)
#   include <sys/mkdev.h>
#elif defined(MAJOR_IN_SYSMACROS)
#   include <sys/sysmacros.h>
#endif

#include "swab.h"

#define POSITION_FOUND          8
#define POSITION_NOT_FOUND      9

#define INVAL_PTR	(void *)-1

void check_memory_msg(void);
void die(const char *fmt, ...) __attribute__ ((format(printf, 1, 2)));
void *getmem(int size);
void *mem_alloc(int size);
void freemem(void *p);
void checkmem(const char *p, int size);
void *expandmem(void *p, int size, int by);
unsigned int get_mem_size(const char *p);
void check_and_free_mem(void);
char *kdevname(int dev);

typedef enum mount_flags {
	MF_NOT_MOUNTED = 0x0,
	MF_RO = 0x1,
	MF_RW = 0x2
} mount_flags_t;

typedef struct mount_hint {
	char *point;		/* Mount point. */
	__u32 psize;		/* Mount point buffer size. */
	__u32 flags;		/* Mount flags. */
} mount_hint_t;

struct mntent *misc_mntent(const char *device);
int misc_device_mounted(const char *device);

typedef struct dma_info {
	int fd;
	struct stat st;
	int support_type;
	int dma;
	__u64 speed;
} dma_info_t;

int prepare_dma_check(dma_info_t *dma_info);
int get_dma_info(dma_info_t *dma_info);
void clean_after_dma_check(int fd, dma_info_t *dma_info);

int valid_offset(int fd, loff_t offset);
unsigned long count_blocks(const char *filename, int blocksize);

void print_how_far(FILE * fp, unsigned long *passed, unsigned long total,
		   unsigned int inc, int quiet);
void print_how_fast(unsigned long total, unsigned long passed, int cursor_pos,
		    int reset_time);
__u32 get_random(void);

int user_confirmed(FILE * fp, const char *q, const char *yes);

/* Only le bitops operations are used. */
static inline int misc_set_bit(unsigned long long nr, void *addr)
{
	__u8 *p, mask;
	int retval;

	p = (__u8 *) addr;
	p += nr >> 3;
	mask = 1 << (nr & 0x7);
	/*cli(); */
	retval = (mask & *p) != 0;
	*p |= mask;
	/*sti(); */
	return retval;
}

static inline int misc_clear_bit(unsigned long long nr, void *addr)
{
	__u8 *p, mask;
	int retval;

	p = (__u8 *) addr;
	p += nr >> 3;
	mask = 1 << (nr & 0x7);
	/*cli(); */
	retval = (mask & *p) != 0;
	*p &= ~mask;
	/*sti(); */
	return retval;
}

static inline int misc_test_bit(unsigned long long nr, const void *addr)
{
	__u8 *p, mask;

	p = (__u8 *) addr;
	p += nr >> 3;
	mask = 1 << (nr & 0x7);
	return ((mask & *p) != 0);
}

static inline unsigned long long misc_find_first_zero_bit(const void *vaddr,
							  unsigned long long
							  size)
{
	const __u8 *p = vaddr, *addr = vaddr;
	unsigned long long res;

	if (!size)
		return 0;

	size = (size >> 3) + ((size & 0x7) > 0);
	while (*p++ == 255) {
		if (--size == 0)
			return (unsigned long long)(p - addr) << 3;
	}

	--p;
	for (res = 0; res < 8; res++)
		if (!misc_test_bit(res, p))
			break;
	return res + (p - addr) * 8;
}

static inline unsigned long long misc_find_next_zero_bit(const void *vaddr,
							 unsigned long long
							 size,
							 unsigned long long
							 offset)
{
	const __u8 *addr = vaddr;
	const __u8 *p = addr + (offset >> 3);
	int bit = offset & 7;
	unsigned long long res;

	if (offset >= size)
		return size;

	if (bit) {
		/* Look for zero in first char */
		for (res = bit; res < 8; res++)
			if (!misc_test_bit(res, p))
				return res + (p - addr) * 8;
		p++;
	}
	/* No zero yet, search remaining full bytes for a zero */
	res = misc_find_first_zero_bit(p, size - 8 * (p - addr));
	return res + (p - addr) * 8;
}

static inline unsigned long long misc_find_first_set_bit(const void *vaddr,
							 unsigned long long
							 size)
{
	const __u8 *p = vaddr, *addr = vaddr;
	unsigned long long res;

	if (!size)
		return 0;

	size = (size >> 3) + ((size & 0x7) > 0);
	while (*p++ == 0) {
		if (--size == 0)
			return (unsigned long long)(p - addr) << 3;
	}

	--p;
	for (res = 0; res < 8; res++)
		if (misc_test_bit(res, p))
			break;

	return res + (p - addr) * 8;
}

static inline unsigned long long misc_find_next_set_bit(const void *vaddr,
							unsigned long long size,
							unsigned long long
							offset)
{
	const __u8 *addr = vaddr;
	const __u8 *p = addr + (offset >> 3);
	int bit = offset & 7;
	unsigned long long res;

	if (offset >= size)
		return size;

	if (bit) {
		/* Look for zero in first char */
		for (res = bit; res < 8; res++)
			if (misc_test_bit(res, p))
				return res + (p - addr) * 8;
		p++;
	}
	/* No set bit yet, search remaining full bytes for a 1 */
	res = misc_find_first_set_bit(p, size - 8 * (p - addr));
	return res + (p - addr) * 8;
}

#define STAT_FIELD(Field, Type)						\
static inline Type misc_device_##Field(const char *device)		\
{									\
	struct stat st;							\
									\
	if (stat(device, &st) == 0)					\
		return st.st_##Field;					\
									\
	fprintf(stderr, "Stat of the device '%s' failed.", device);	\
	exit(8);							\
}

STAT_FIELD(mode, mode_t);
STAT_FIELD(rdev, dev_t);
STAT_FIELD(size, off_t);
STAT_FIELD(blocks, blkcnt_t);

__u16 mask16(int from, int count);
__u32 mask32(int from, int count);
__u64 mask64(int from, int count);

int reiserfs_bin_search(const void *key, void *base, __u32 num, int width,
			__u32 * ppos, __compar_fn_t comp_func);

struct block_handler {
	__u32 blocknr;
	dev_t device;
};

int blocklist__is_block_saved(struct block_handler **base, __u32 * count,
			      __u32 blocknr, dev_t device, __u32 * position);
void blocklist__insert_in_position(void *block_h, void **base, __u32 * count,
				   int elem_size, __u32 * position);
int blockdev_list_compare(const void *block1, const void *block2);

#define set_bit_field_XX(XX,vp,val,from,count) \
{\
    __le##XX * p;\
    __u##XX tmp;\
\
    /* make sure that given value can be put in 'count' bits */\
    if (val > (1 << count))\
	die ("set_bit_field: val %d is too big for %d bits", val, count);\
\
    p = (__le##XX *)vp;\
    tmp = le##XX##_to_cpu (*p);\
\
    /* clear 'count' bits starting from 'from'-th one */\
    tmp &= ~mask##XX (from, count);\
\
    /* put given value in proper bits */\
    tmp |= (val << from);\
\
    *p = cpu_to_le##XX (tmp);\
}

#define get_bit_field_XX(XX,vp,from,count) \
\
    __le##XX * p;\
    __u##XX tmp;\
\
    p = (__le##XX *)vp;\
    tmp = le##XX##_to_cpu (*p);\
\
    /* clear all bits but 'count' bits starting from 'from'-th one */\
    tmp &= mask##XX (from, count);\
\
    /* get value written in specified bits */\
    tmp >>= from;\
    return tmp;

#ifndef major
#define major(rdev)      ((rdev)>>8)
#define minor(rdev)      ((rdev) & 0xff)
#endif /* major */

#ifndef SCSI_DISK_MAJOR
#define SCSI_DISK_MAJOR(maj) ((maj) == SCSI_DISK0_MAJOR || \
			     ((maj) >= SCSI_DISK1_MAJOR && (maj) <= SCSI_DISK7_MAJOR))
#endif /* SCSI_DISK_MAJOR */

#ifndef SCSI_BLK_MAJOR
#define SCSI_BLK_MAJOR(maj)  (SCSI_DISK_MAJOR(maj) || (maj) == SCSI_CDROM_MAJOR)
#endif /* SCSI_BLK_MAJOR */

#ifndef IDE_DISK_MAJOR
#ifdef IDE9_MAJOR
#define IDE_DISK_MAJOR(maj) ((maj) == IDE0_MAJOR || (maj) == IDE1_MAJOR || \
			     (maj) == IDE2_MAJOR || (maj) == IDE3_MAJOR || \
			     (maj) == IDE4_MAJOR || (maj) == IDE5_MAJOR || \
			     (maj) == IDE6_MAJOR || (maj) == IDE7_MAJOR || \
			     (maj) == IDE8_MAJOR || (maj) == IDE9_MAJOR)
#else
#define IDE_DISK_MAJOR(maj) ((maj) == IDE0_MAJOR || (maj) == IDE1_MAJOR || \
			     (maj) == IDE2_MAJOR || (maj) == IDE3_MAJOR || \
			     (maj) == IDE4_MAJOR || (maj) == IDE5_MAJOR)
#endif /* IDE9_MAJOR */
#endif /* IDE_DISK_MAJOR */

#endif /* REISERFS_MISC_H */
