/*
 * Copyright 2002-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "debugreiserfs.h"
#include <search.h>
#include <obstack.h>

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

/* try to find blocks which contain only items which are */

/* read blocks marked in debug_bitmap and collect statistic of it:
 number of stat data */
struct {
	unsigned long all;
	unsigned long items[5];
	unsigned long unique_keys;	/* keys of stat datas */
	unsigned long unique_entry_keys;	/* keys which appear in directory entries */
	unsigned long names;	/* dir entries but "." and ".." */
	unsigned long dir_blocks;	/* block containing only one directory item */
	unsigned long unique_items;
	unsigned long leaves;
	unsigned long blocks_to_skip;
} fs_stat;

/* store unique item heads */
struct obstack items;

/* tree sorting item heades by comp_items_1 */
void *items_tree;

static int comp_items_1(const void *p1, const void *p2)
{
	int retval;
	struct item_head *ih1, *ih2;

	/*
	   if (*(int *)p1 != *(int *)p2)
	   retval = 1;
	   else
	   retval = 0;
	 */
	retval = comp_keys(p1, p2);
	/*retval = comp_short_keys (p1, p2); */
	if (retval)
		return retval;

	ih1 = (struct item_head *)p1;
	ih2 = (struct item_head *)p2;

	if (get_ih_item_len(ih1) < get_ih_item_len(ih2))
		return -1;
	if (get_ih_item_len(ih1) > get_ih_item_len(ih2))
		return 1;
	if (get_ih_entry_count(ih1) < get_ih_entry_count(ih2))
		return -1;
	if (get_ih_entry_count(ih1) > get_ih_entry_count(ih2))
		return 1;

	return 0;
}

/*
static void print_node (const void *nodep, VISIT value, int level)
{
    int i;

    if (value == leaf) {
	for (i = 0; i < level; i ++)
	    reiserfs_warning (stdout, "\t");
	reiserfs_warning (stdout, "%H\n", *(struct item_head **)nodep);
	return;
    }
    if (value == postorder) {
	for (i = 0; i < level; i ++)
	    reiserfs_warning (stdout, "\t");
	reiserfs_warning (stdout, "%H\n", *(struct item_head **)nodep);
    }
}
*/

static int is_unique_item(struct obstack *ostack, void **tree, void *ih)
{
	void *res;
	void *key1;

	key1 = obstack_copy(ostack, ih, IH_SIZE);
	res = tsearch(key1, tree, comp_items_1);
	if (!res)
		reiserfs_panic("Too many keys found");

/*    twalk (*tree, print_node);*/
/*    reiserfs_warning (stderr, "\n\n");*/
	if (*(void **)res != key1) {
		/* key is in tree already, remove it from obstack */
		/*reiserfs_warning (stdout, "%H is skipped\n", ih);fflush (stdout); */
		obstack_free(ostack, key1);
		return 0;
	}

	/*reiserfs_warning (stdout, "%k is added\n", ih);fflush (stdout); */
	return 1;
}

static void stat1_the_leaf(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	int i, i_num;
	struct item_head *ih;
	int is_there_unique_item;

	ih = item_head(bh, 0);
	is_there_unique_item = 0;
	i_num = leaf_item_number_estimate(bh);
	for (i = 0; i < i_num; i++, ih++) {
		/* count all items */
		fs_stat.all++;

		if (is_unique_item(&items, &items_tree, ih)) {
			/* this is item we have not seen yet */
			fs_stat.unique_items++;
			is_there_unique_item++;
		}
	}

	if (!is_there_unique_item) {
		/* the node contains only items we have seen already. so we will skip
		   it */
		fs_stat.blocks_to_skip++;
		reiserfs_bitmap_clear_bit(input_bitmap(fs), bh->b_blocknr);
	} else {
		ih = item_head(bh, 0);
		/* node contains at least one unique item. We will put it in, count items of each type */
		for (i = 0; i < i_num; i++, ih++) {
			fs_stat.items[get_type(&ih->ih_key)]++;
		}
	}
}

/*
static void stat2_the_leaf (struct buffer_head * bh)
{
    int i;
    struct item_head * ih;

    ih = B_N_PITEM_HEAD (bh, 0);
    for (i = 0; i < node_item_number (bh); i ++, ih ++) {
	
    }
}
*/

void do_stat(reiserfs_filsys_t fs)
{
	unsigned long i;
	unsigned long done, total;
	struct buffer_head *bh;
	int type;
	FILE *fp;

	obstack_init(&items);
	items_tree = 0;

/*
    bh = bread (fs->s_dev, 8211, fs->s_blocksize);
    stat1_the_leaf (fs, bh);

    return;
*/

	/* pass 0 of stating */
	total = reiserfs_bitmap_ones(input_bitmap(fs));
	done = 0;
	for (i = 0; i < get_sb_block_count(fs->fs_ondisk_sb); i++) {
		if (!reiserfs_bitmap_test_bit(input_bitmap(fs), i))
			continue;

		print_how_far(stderr, &done, total, 1, be_quiet(fs));

		bh = bread(fs->fs_dev, i, fs->fs_blocksize);
		if (!bh) {
			printf("could not read block %lu\n", i);
			continue;
		}
		type = who_is_this(bh->b_data, bh->b_size);
		if (type != THE_LEAF && type != HAS_IH_ARRAY) {
			reiserfs_bitmap_clear_bit(input_bitmap(fs), i);
			brelse(bh);
			continue;
		}
		fs_stat.leaves++;
		stat1_the_leaf(fs, bh);
		brelse(bh);
	}

	reiserfs_warning(stderr, "\nThere were found on the '%s' device:\n"
			 "\tleaves %lu\n"
			 "\ttotal number of items %lu\n"
			 "\tblocks containing at least one unique item %lu\n"
			 "\tblocks which can be skipped %lu\n"
			 "\t\tstat data %lu\n"
			 "\t\tindirect %lu\n"
			 "\t\tdirect %lu\n"
			 "\t\tdirectory items %lu\n" "\tunique items %lu\n",
			 /*
			    "\tnames there (but \".\" and \"..\") %lu\n"
			    "\tpointing to unique keys %lu\n"
			    "other items %lu\n"
			    "blocks containing only 1 dir item %lu\n",
			  */
			 fs->fs_file_name,
			 fs_stat.leaves,
			 fs_stat.all,
			 fs_stat.leaves - fs_stat.blocks_to_skip,
			 fs_stat.blocks_to_skip,
			 fs_stat.items[TYPE_STAT_DATA],
			 fs_stat.items[TYPE_INDIRECT],
			 fs_stat.items[TYPE_DIRECT],
			 fs_stat.items[TYPE_DIRENTRY], fs_stat.unique_items);
/*
		      fs_stat.names,
		      fs_stat.unique_keys,
		      fs_stat.items [4],
		      fs_stat.dir_blocks);
*/
	if (!input_bitmap_file_name(fs))
		return;

	fp = fopen(input_bitmap_file_name(fs), "w");
	if (!fp) {
		reiserfs_exit(1, "could not open %s to save bitmap: %m\n",
			      input_bitmap_file_name(fs));
	}
	reiserfs_warning(stderr, "Updated bitmap contains %u blocks marked\n",
			 reiserfs_bitmap_ones(input_bitmap(fs)));

	reiserfs_bitmap_save(fp, input_bitmap(fs));
	fclose(fp);
	return;

	/* pass 2 of stating */
	reiserfs_warning(stderr,
			 "Looking for blocks containing only keys not pointed by any of entries\n");
	total = reiserfs_bitmap_ones(input_bitmap(fs));
	done = 0;
	for (i = 0; i < get_sb_block_count(fs->fs_ondisk_sb); i++) {
		if (!reiserfs_bitmap_test_bit(input_bitmap(fs), i))
			continue;

		print_how_far(stderr, &done, total, 1, be_quiet(fs));

		bh = bread(fs->fs_dev, i, fs->fs_blocksize);
		if (!bh) {
			printf("could not read block %lu\n", i);
			continue;
		}
		/*stat2_the_leaf (bh); */
	}

}
