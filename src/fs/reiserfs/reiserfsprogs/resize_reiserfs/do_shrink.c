/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "resize.h"
#include <time.h>

static unsigned long int_node_cnt = 0, int_moved_cnt = 0;
static unsigned long leaf_node_cnt = 0, leaf_moved_cnt = 0;
static unsigned long unfm_node_cnt = 0, unfm_moved_cnt = 0;
static unsigned long total_node_cnt = 0;
static unsigned long total_moved_cnt = 0;

static unsigned long unused_block;
static unsigned long blocks_used;
static int block_count_mismatch = 0;

static reiserfs_bitmap_t *bmp;
static struct reiserfs_super_block *ondisk_sb;

/* abnornal exit from block reallocation process */
static void quit_resizer(reiserfs_filsys_t fs)
{
	/* save changes to bitmap blocks */
	reiserfs_close(fs);
	/* leave fs in ERROR state */
	reiserfs_exit(1, "fs shrinking was not completed successfully, "
		      "run reiserfsck.");
}

/* block moving */
static unsigned long move_generic_block(reiserfs_filsys_t fs,
					unsigned long block,
					unsigned long bnd, int h)
{
	struct buffer_head *bh, *bh2;

	/* primitive fsck */
	if (block > get_sb_block_count(ondisk_sb)) {
		fprintf(stderr, "resize_reiserfs: invalid block number "
			"(%lu) found.\n", block);
		quit_resizer(fs);
	}

	/* progress bar, 3D style :) */
	if (opt_verbose)
		print_how_far(stderr, &total_node_cnt, blocks_used, 1, 0);
	else
		total_node_cnt++;

	/* infinite loop check */
	if (total_node_cnt > blocks_used && !block_count_mismatch) {
		fputs("resize_reiserfs: warning: block count exeeded\n",
		      stderr);
		block_count_mismatch = 1;
	}

	if (block < bnd)	/* block will not be moved */
		return 0;

	/* move wrong block */
	bh = bread(fs->fs_dev, block, fs->fs_blocksize);

	if (!bh)
		reiserfs_exit(1, "move_generic_block: bread failed.\n");

	reiserfs_bitmap_find_zero_bit(bmp, &unused_block);
	if (unused_block == 0 || unused_block >= bnd) {
		fputs("resize_reiserfs: can\'t find free block\n", stderr);
		quit_resizer(fs);
	}

	/* blocknr changing */
	bh2 = getblk(fs->fs_dev, unused_block, fs->fs_blocksize);
	memcpy(bh2->b_data, bh->b_data, bh2->b_size);
	reiserfs_bitmap_clear_bit(bmp, block);
	reiserfs_bitmap_set_bit(bmp, unused_block);

	brelse(bh);
	mark_buffer_uptodate(bh2, 1);
	mark_buffer_dirty(bh2);
	bwrite(bh2);
	brelse(bh2);

	total_moved_cnt++;
	return unused_block;
}

static unsigned long move_unformatted_block(reiserfs_filsys_t fs,
					    unsigned long block,
					    unsigned long bnd, int h)
{
	unsigned long b;
	unfm_node_cnt++;
	b = move_generic_block(fs, block, bnd, h);
	if (b)
		unfm_moved_cnt++;
	return b;
}

/* recursive function processing all tree nodes */
static unsigned long move_formatted_block(reiserfs_filsys_t fs,
					  unsigned long block,
					  unsigned long bnd, int h)
{
	struct buffer_head *bh;
	struct item_head *ih;
	unsigned long new_blocknr = 0;
	int node_is_internal = 0;
	unsigned int i, j;

	bh = bread(fs->fs_dev, block, fs->fs_blocksize);
	if (!bh)
		reiserfs_exit(1, "move_formatted_block: bread failed");

	if (is_leaf_node(bh)) {

		leaf_node_cnt++;

		for (i = 0; i < B_NR_ITEMS(bh); i++) {
			ih = item_head(bh, i);

			/* skip the bad blocks. */
			if (get_key_objectid(&ih->ih_key) == BADBLOCK_OBJID &&
			    get_key_dirid(&ih->ih_key) == BADBLOCK_DIRID)
				continue;

			if (is_indirect_ih(ih)) {
				__le32 *indirect;

				indirect = (__le32 *) ih_item_body(bh, ih);
				for (j = 0; j < I_UNFM_NUM(ih); j++) {
					unsigned long unfm_block;

					if (d32_get(indirect, j) == 0)	/* hole */
						continue;
					unfm_block =
					    move_unformatted_block(fs,
								   d32_get
								   (indirect,
								    j), bnd,
								   h + 1);
					if (unfm_block) {
						d32_put(indirect, j,
							unfm_block);
						mark_buffer_dirty(bh);
					}
				}
			}
		}
	} else if (is_internal_node(bh)) {	/* internal node */

		int_node_cnt++;
		node_is_internal = 1;

		for (i = 0; i <= B_NR_ITEMS(bh); i++) {
			unsigned long moved_block;
			moved_block =
			    move_formatted_block(fs,
						 get_dc_child_blocknr(B_N_CHILD
								      (bh, i)),
						 bnd, h + 1);
			if (moved_block) {
				set_dc_child_blocknr(B_N_CHILD(bh, i),
						     moved_block);
				mark_buffer_dirty(bh);
			}
		}
	} else {
		DIE("block (%lu) has invalid format\n", block);
	}

	if (buffer_dirty(bh)) {
		mark_buffer_uptodate(bh, 1);
		bwrite(bh);
	}

	brelse(bh);

	new_blocknr = move_generic_block(fs, block, bnd, h);
	if (new_blocknr) {
		if (node_is_internal)
			int_moved_cnt++;
		else
			leaf_moved_cnt++;
	}

	return new_blocknr;
}

int shrink_fs(reiserfs_filsys_t fs, long long int blocks)
{
	unsigned long n_root_block;
	unsigned int bmap_nr_new;
	unsigned long bad_count;

	ondisk_sb = fs->fs_ondisk_sb;

	bmap_nr_new = (blocks - 1) / (8 * fs->fs_blocksize) + 1;

	/* is shrinking possible ? */
	if (get_sb_block_count(ondisk_sb) - blocks >
	    get_sb_free_blocks(ondisk_sb) + reiserfs_fs_bmap_nr(fs) -
	    bmap_nr_new) {
		fprintf(stderr,
			"resize_reiserfs: can\'t shrink fs; too many "
			"blocks already allocated\n");
		return -1;
	}

	/* warn about alpha version */
	{
		int c;

		printf("You are running BETA version of reiserfs shrinker.\n"
		       "This version is only for testing or VERY CAREFUL use.\n"
		       "Backup of you data is recommended.\n\n"
		       "Do you want to continue? [y/N]:");
		fflush(stdout);
		c = getchar();
		if (c != 'y' && c != 'Y')
			exit(1);
	}

	reiserfs_reopen(fs, O_RDWR);
	if (reiserfs_open_ondisk_bitmap(fs))
		reiserfs_exit(1, "cannot open ondisk bitmap");
	bmp = fs->fs_bitmap2;
	ondisk_sb = fs->fs_ondisk_sb;

	set_sb_fs_state(fs->fs_ondisk_sb, FS_ERROR);
	mark_buffer_uptodate(fs->fs_super_bh, 1);
	mark_buffer_dirty(fs->fs_super_bh);
	bwrite(fs->fs_super_bh);

	/* calculate number of data blocks */
	blocks_used = get_sb_block_count(fs->fs_ondisk_sb)
	    - get_sb_free_blocks(fs->fs_ondisk_sb)
	    - reiserfs_fs_bmap_nr(fs)
	    - get_jp_journal_size(sb_jp(fs->fs_ondisk_sb))
	    - REISERFS_DISK_OFFSET_IN_BYTES / fs->fs_blocksize - 2;	/* superblock itself and 1 descriptor after the journal */

	unused_block = 1;

	if (opt_verbose) {
		printf("Processing the tree: ");
		fflush(stdout);
	}

	n_root_block = move_formatted_block(fs, get_sb_root_block(ondisk_sb),
					    blocks, 0);

	if (n_root_block)
		set_sb_root_block(ondisk_sb, n_root_block);

	if (opt_verbose)
		printf("\n\nnodes processed (moved):\n"
		       "int        %lu (%lu),\n"
		       "leaves     %lu (%lu),\n"
		       "unfm       %lu (%lu),\n"
		       "total      %lu (%lu).\n\n",
		       int_node_cnt, int_moved_cnt,
		       leaf_node_cnt, leaf_moved_cnt,
		       unfm_node_cnt, unfm_moved_cnt,
		       (unsigned long)total_node_cnt, total_moved_cnt);

	if (block_count_mismatch) {
		fprintf(stderr, "resize_reiserfs: data block count %lu"
			" doesn\'t match data block count %lu from super block\n",
			(unsigned long)total_node_cnt, blocks_used);
	}

	{
		unsigned long l;

		/* make sure that none of truncated block are in use */
		printf("check for used blocks in truncated region\n");
		for (l = blocks; l < fs->fs_bitmap2->bm_bit_size; l++) {
			if ((l % (fs->fs_blocksize * 8)) == 0)
				continue;
			if (reiserfs_bitmap_test_bit(fs->fs_bitmap2, l))
				printf("<%lu>", l);
		}
		printf("\n");
	}

	badblock_list(fs, mark_badblock, NULL);

	if (fs->fs_badblocks_bm) {
		bad_count = reiserfs_bitmap_ones(fs->fs_badblocks_bm);
		reiserfs_shrink_bitmap(fs->fs_badblocks_bm, blocks);
		add_badblock_list(fs, 1);
		bad_count -= reiserfs_bitmap_ones(fs->fs_badblocks_bm);
	} else
		bad_count = 0;

	reiserfs_shrink_bitmap(fs->fs_bitmap2, blocks);

	set_sb_free_blocks(ondisk_sb, get_sb_free_blocks(ondisk_sb)
			   - (get_sb_block_count(ondisk_sb) - blocks)
			   + (reiserfs_fs_bmap_nr(fs) - bmap_nr_new)
			   + bad_count);
	set_sb_block_count(ondisk_sb, blocks);
	set_sb_bmap_nr(ondisk_sb, bmap_nr_new);

	set_sb_bmap_nr(fs->fs_ondisk_sb,
		       reiserfs_bmap_over(bmap_nr_new) ? 0 : bmap_nr_new);

	return 0;
}
