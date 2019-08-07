/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#ifndef REISERFSPROGS_IO_H
#define REISERFSPROGS_IO_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "misc.h"

struct buffer_head {
	unsigned long b_blocknr;
	int b_dev;
	unsigned long b_size;
	char *b_data;
	unsigned long b_state;
	unsigned int b_count;
	unsigned int b_list;
	void (*b_start_io) (unsigned long);
	void (*b_end_io) (struct buffer_head * bh, int uptodate);

	struct buffer_head *b_next;
	struct buffer_head *b_prev;
	struct buffer_head *b_hash_next;
	struct buffer_head *b_hash_prev;
};

#define BH_Uptodate	0
#define BH_Dirty	1
#define BH_Lock		2
#define BH_Do_not_flush 3

#define buffer_uptodate(bh) misc_test_bit(BH_Uptodate, &(bh)->b_state)
#define buffer_dirty(bh) misc_test_bit(BH_Dirty, &(bh)->b_state)
#define buffer_locked(bh) misc_test_bit(BH_Lock, &(bh)->b_state)
#define buffer_clean(bh) !misc_test_bit(BH_Dirty, &(bh)->b_state)
#define buffer_do_not_flush(bh) misc_test_bit(BH_Do_not_flush, &(bh)->b_state)
#define mark_buffer_dirty(bh) misc_set_bit(BH_Dirty, &(bh)->b_state)
#define mark_buffer_uptodate(bh,i) misc_set_bit(BH_Uptodate, &(bh)->b_state)
#define mark_buffer_clean(bh) misc_clear_bit(BH_Dirty, &(bh)->b_state)
#define mark_buffer_do_not_flush(bh) misc_set_bit(BH_Do_not_flush, &(bh)->b_state)
#define clear_buffer_do_not_flush(bh) misc_clear_bit(BH_Do_not_flush, &(bh)->b_state)

/*
printf ("%s:%s:%u %p %p %p\n",
__FILE__, __FUNCTION__, __LINE__,
	__builtin_return_address (0),
	__builtin_return_address (1),
	__builtin_return_address (2));
*/

void __wait_on_buffer(struct buffer_head *bh);
struct buffer_head *getblk(int dev, unsigned long block, int size);
struct buffer_head *reiserfs_getblk(int dev, unsigned long block, int size,
				    int *repeat);

struct buffer_head *find_buffer(int dev, unsigned long block,
				unsigned long size);
struct buffer_head *get_hash_table(dev_t dev, unsigned long block, int size);
struct buffer_head *bread(int dev, unsigned long block, size_t size);
struct buffer_head *reiserfs_bread(int dev, unsigned long block, int size,
				   int *repeat);
int bwrite(struct buffer_head *bh);
void brelse(struct buffer_head *bh);
void bforget(struct buffer_head *bh);
void init_rollback_file(char *rollback_file, unsigned int *blocksize,
			FILE * log);
int open_rollback_file(char *rollback_file, FILE * log);
void close_rollback_file();
void do_fsck_rollback(int fd_device, int fd_journal_device, FILE * log);

void flush_buffers(int);
void free_buffers(void);
void invalidate_buffers(int);

#endif
