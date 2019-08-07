/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

/* g_disk_bitmap initially contains copy of disk bitmaps
   (cautious version of it);

   g_new_bitmap initially has marked only super block, bitmap blocks
   and bits after the end of bitmap

   in pass 1 we go through g_disk_bitmap.

   If block does not look like formatted node, we skip it.

   If block contains internal node, put 0 in g_disk_bitmap if block is
   not used in new tree yet.

   If block contains leaf and is used already (by an indirect item
   handled already to this time) save all items. They will be inserted
   into tree after pass 1.

   If block looking like leaf is not used in the new tree, try to
   insert in into tree. If it is not possible, mark block in
   g_uninsertable_leaf_bitmap. Blocks marked in this bitmap will be inserted into tree in pass 2. They can not be

  This means, that in pass 1 when we have
   found block containing the internal nodes we mark it in
   g_disk_bitmap as free (reiserfs_free_internal_block). When block
   gets into new tree it is marked in g_new_bitmap (mark_block_used)
   When collecting resources for do_balance, we mark new blocks with
   mark_block_used. After do_balance we unmark unused new blocks in
   g_new_bitmap (bitmap.c:/reiserfs_free_block)

   Allocating of new blocks: look for 0 bit in g_disk_bitmap
   (find_zero_bit_in_bitmap), make sure, that g_new_bitmap contains 0
   at the corresponding bit (is_block_used).

 */

/* is blocks used (marked by 1 in new bitmap) in the tree which is being built
   (as leaf, internal, bitmap, or unformatted node) */
int is_block_used(unsigned long block)
{
	return reiserfs_bitmap_test_bit(fsck_new_bitmap(fs), block);
}

void mark_block_used(unsigned long block, int check_hardware)
{
	if (!block)
		return;
	if (is_block_used(block)) {
		if (check_hardware)
			check_memory_msg();
		die("mark_block_used: (%lu) used already", block);
	}

	reiserfs_bitmap_set_bit(fsck_new_bitmap(fs), block);
}

static void mark_block_free(unsigned long block)
{
	if (!is_block_used(block))
		die("mark_block_free: (%lu) is free already", block);

	reiserfs_bitmap_clear_bit(fsck_new_bitmap(fs), block);
}

int is_block_uninsertable(unsigned long block)
{
	return !reiserfs_bitmap_test_bit(fsck_uninsertables(fs), block);
}

/* uninsertable block is marked by bit clearing */
void mark_block_uninsertable(unsigned long block)
{
	if (is_block_uninsertable(block))
		die("mark_block_uninsertable: (%lu) is uninsertable already",
		    block);

	reiserfs_bitmap_clear_bit(fsck_uninsertables(fs), block);
	/* we do not need thsi actually */
	pass_1_stat(fs)->uninsertable_leaves++;
}

/* FIXME: should be able to work around no disk space */
int reiserfsck_reiserfs_new_blocknrs(reiserfs_filsys_t fs,
				     unsigned long *free_blocknrs,
				     unsigned long start, int amount_needed)
{
	int i;

	if (!are_there_allocable_blocks(amount_needed))
		die("out of disk space");
	for (i = 0; i < amount_needed; i++) {
		free_blocknrs[i] = alloc_block();
		if (!free_blocknrs[i])
			die("reiserfs_new_blocknrs: 0 is allocated");
		mark_block_used(free_blocknrs[i], 0);
	}
	return CARRY_ON;
}

// FIXME: do you check readability of a block? If f_read fails - you
// free block in bitmap or if you mark bad blocks used to avoid their
// allocation in future you should have bad block counter in a super
// block. Another minor issue: users of _get_new_buffer expect buffer
// to be filled with 0s
struct buffer_head *reiserfsck_get_new_buffer(unsigned long start)
{
	unsigned long blocknr = 0;
	struct buffer_head *bh = NULL;

	reiserfs_new_blocknrs(fs, &blocknr, start, 1);
	bh = getblk(fs->fs_dev, blocknr, fs->fs_blocksize);
	return bh;
}

/* free block in new bitmap */
int reiserfsck_reiserfs_free_block(reiserfs_filsys_t fs, unsigned long block)
{
	mark_block_free(block);

	/* put it back to pool of blocks for allocation */
	make_allocable(block);
	return 0;
}
