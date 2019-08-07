/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

/*
 * Pass0 scans the used part of the partition. It creates two maps which will
 * be used on the pass 1. These are a map of nodes looking like leaves and
 * a map of "bad" unformatted nodes. After pass 0 we can detect unformatted
 * node pointers pointing to leaves.
 */

/* leaves */
reiserfs_bitmap_t *leaves_bitmap;
#define pass0_is_leaf(block) __is_marked (leaves, block)
#define pass0_mark_leaf(block) __mark (leaves, block)

/* nodes which are referred to from only one indirect item */
static reiserfs_bitmap_t *good_unfm_bitmap;
#define pass0_is_good_unfm(block) __is_marked (good_unfm, block)
#define pass0_mark_good_unfm(block) __mark (good_unfm, block)
#define pass0_unmark_good_unfm(block) __unmark (good_unfm, block)

/* nodes which are referred to from more than one indirect item */
static reiserfs_bitmap_t *bad_unfm_bitmap;
#define pass0_is_bad_unfm(block) __is_marked (bad_unfm, block)
#define pass0_mark_bad_unfm(block) __mark (bad_unfm, block)
#define pass0_unmark_bad_unfm(block) __unmark (bad_unfm, block)

static int correct_direct_item_offset(__u64 offset, int format)
{
	if (format == KEY_FORMAT_2) {
		return (offset && ((offset - 1) % 8 == 0));
	} else {
		return (offset);
	}
	return 0;
}

/* bitmaps which are built on pass 0 and are used on pass 1 */
static void make_aux_bitmaps(reiserfs_filsys_t fs)
{
	struct reiserfs_super_block *sb;

	sb = fs->fs_ondisk_sb;

	/* bitmap of leaves found on the device */
	leaves_bitmap = reiserfs_create_bitmap(get_sb_block_count(sb));

	good_unfm_bitmap = reiserfs_create_bitmap(get_sb_block_count(sb));

	bad_unfm_bitmap = reiserfs_create_bitmap(get_sb_block_count(sb));
}

void delete_aux_bitmaps(void)
{
	reiserfs_delete_bitmap(leaves_bitmap);
	reiserfs_delete_bitmap(good_unfm_bitmap);
	reiserfs_delete_bitmap(bad_unfm_bitmap);
}

/* register block some indirect item points to */
static void register_unfm(unsigned long block)
{
	if (!pass0_is_good_unfm(block) && !pass0_is_bad_unfm(block)) {
		/* this block was not pointed by other indirect items yet */
		pass0_mark_good_unfm(block);
		return;
	}

	if (pass0_is_good_unfm(block)) {
		/* block was pointed once already, unmark it in bitmap of good
		   unformatted nodes and mark in bitmap of bad pointers */
		pass0_unmark_good_unfm(block);
		pass0_mark_bad_unfm(block);
		return;
	}

	assert(pass0_is_bad_unfm(block));
}

/* 'upper' item is correct if 'upper + 2' exists and its key is greater than
   key of 'upper' */
static int upper_correct(struct buffer_head *bh, struct item_head *upper,
			 int upper_item_num)
{
	if (upper_item_num + 2 < B_NR_ITEMS(bh)) {
		if (comp_keys(&upper->ih_key, &(upper + 2)->ih_key) != -1)
			/* item-num's item is out of order of order */
			return 0;
		return 1;
	}

	/* there is no item above the "bad pair" */
	return 2;
}

/* 'lower' item is correct if 'lower - 2' exists and its key is smaller than
   key of 'lower' */
static int lower_correct(struct buffer_head *bh, struct item_head *lower,
			 int lower_item_num)
{
	if (lower_item_num - 2 >= 0) {
		if (comp_keys(&(lower - 2)->ih_key, &lower->ih_key) != -1)
			return 0;
		return 1;
	}
	return 2;
}

/* return 1 if something was changed */
static int correct_key_format(struct item_head *ih, int symlink)
{
	int dirty = 0;

	if (is_stat_data_ih(ih)) {
		/* for stat data we have no way to check whether key format in item
		   head matches to the key format found from the key directly */
		if (get_ih_item_len(ih) == SD_V1_SIZE) {
			if (get_ih_key_format(ih) != KEY_FORMAT_1) {
				/*fsck_log ("correct_key_format: ih_key_format of (%H) is set to format 1\n",
				   ih); */
				set_ih_key_format(ih, KEY_FORMAT_1);
				return 1;
			}
			return 0;
		}
		if (get_ih_item_len(ih) == SD_SIZE) {
			if (get_ih_key_format(ih) != KEY_FORMAT_2) {
				/*fsck_log ("correct_key_format: ih_key_format of (%H) is set to format 2\n",
				   ih); */
				set_ih_key_format(ih, KEY_FORMAT_2);
				return 1;
			}
			return 0;
		}

		die("stat_data item of the wrong length");
	}

	if (symlink && is_direct_ih(ih)
	    && (key_format(&ih->ih_key) != KEY_FORMAT_1)) {
		/* All symlinks are of 3.5 format */
		/*fsck_log ("correct_key_format: Symlink keys should be of 3.5 format. %k - fixed.\n", &ih->ih_key); */
		set_type_and_offset(KEY_FORMAT_1, &ih->ih_key,
				    get_offset(&ih->ih_key),
				    get_type(&ih->ih_key));
	}

	if (key_format(&ih->ih_key) != get_ih_key_format(ih)) {
		/*fsck_log ("correct_key_format: ih_key_format of (%H) is set to format found in the key\n",
		   ih); */
		set_ih_key_format(ih, key_format(&ih->ih_key));
		dirty = 1;
	}

	if (is_direct_ih(ih) && get_offset(&ih->ih_key) > fs->fs_blocksize * 4) {
		/*fsck_log ("correct_key_format: %H made of indirect type\n", ih); */
		set_type(key_format(&ih->ih_key), &ih->ih_key, TYPE_INDIRECT);
		if (get_offset(&ih->ih_key) % fs->fs_blocksize != 1)
			fsck_log
			    ("correct_key_format: Item header's key has the wrong offset %H\n",
			     ih);
		dirty = 1;
	}

	return dirty;
}

#if 0
/* fixme: we might try all available hashes */
static int prob_name(reiserfs_filsys_t fs,
		     char **name, int max_len, __u32 deh_offset)
{
	int start;		/* */
	int len;

	for (start = 0; start < max_len; start++) {
		for (len = 0; len < max_len - start; len++) {
			if (is_properly_hashed
			    (fs, *name + start, len + 1, deh_offset)) {
				*name = *name + start;
				return len + 1;
			}
		}
	}
	return 0;
}
#endif

static void hash_hits_init(reiserfs_filsys_t fs)
{
	fsck_data(fs)->rebuild.hash_amount = known_hashes();
	fsck_data(fs)->rebuild.hash_hits =
	    getmem(sizeof(unsigned long) * fsck_data(fs)->rebuild.hash_amount);
	return;
}

static void add_hash_hit(reiserfs_filsys_t fs, int hash_code)
{
	fsck_data(fs)->rebuild.hash_hits[hash_code]++;
}

/* deh_location look reasonable, try to find name length. return 0 if
   we failed */
static int try_to_get_name_length(struct item_head *ih,
				  struct reiserfs_de_head *deh, int i)
{
	int len;

	if (i == 0 || !de_bad_location(deh - 1)) {
		len = name_in_entry_length(ih, deh, i);
		return (len > 0) ? len : 0;
	}

	/* previous entry had bad location so we had no way to find
	   name length */
	return 0;
}

/* define this if you are using -t to debug recovering of corrupted directory
   item */
#define DEBUG_VERIFY_DENTRY
#undef DEBUG_VERIFY_DENTRY

/* check directory item and try to recover something */
static int verify_directory_item(reiserfs_filsys_t fs, struct buffer_head *bh,
				 int item_num)
{
	struct item_head *ih;
	struct item_head tmp;
	char *item;
	struct reiserfs_de_head *deh;
	char *name;
	int name_len;
	int bad, lost_found;
	int i, j;
	char buf[4096];
	int hash_code;
	int min_entry_size = 1;

#ifdef DEBUG_VERIFY_DENTRY
	char *direntries;
#endif

	ih = item_head(bh, item_num);
	item = ih_item_body(bh, ih);
	deh = (struct reiserfs_de_head *)item;

	if ((get_ih_entry_count(ih) >
	     (get_ih_item_len(ih) / (DEH_SIZE + min_entry_size)))
	    || (get_ih_entry_count(ih) == 0)) {
		set_ih_entry_count(ih,
				   (int)get_ih_item_len(ih) / (DEH_SIZE +
							       min_entry_size));
		mark_buffer_dirty(bh);
	}

	if (get_ih_entry_count(ih) == 0) {
		delete_item(fs, bh, item_num);
		return -1;
	}

	/* check deh_location */
	for (i = 0; i < get_ih_entry_count(ih); i++) {
		/* silently fix deh_state */
		if (get_deh_state(deh + i) != (1 << DEH_Visible2)) {
			set_deh_state(deh + i, (1 << DEH_Visible2));
			mark_buffer_dirty(bh);
		}
		if (dir_entry_bad_location(deh + i, ih, !i))
			mark_de_bad_location(deh + i);
	}

#ifdef DEBUG_VERIFY_DENTRY
	direntries = getmem(ih_entry_count(ih) * sizeof(int));

	printf("Entries with bad locations within the directory: ");
	for (i = 0; i < ih_entry_count(ih); i++) {
		if (de_bad_location(deh + i))
			printf("%d ", i);
	}
	printf("\n");
#endif /* DEBUG_VERIFY_DENTRY */

	/* find entries names in which have mismatching deh_offset */
	for (i = get_ih_entry_count(ih) - 1; i >= 0; i--) {
		if (de_bad(deh + i))
			/* bad location */
			continue;

		if (i) {
			if (get_deh_location(deh + i - 1) <
			    get_deh_location(deh + i))
				mark_de_bad_location(deh + i - 1);
		}

		name = name_in_entry(deh + i, i);
		/* Although we found a name, we not always can get its length as
		   it depends on deh_location of previous entry. */
		name_len = try_to_get_name_length(ih, deh + i, i);

#ifdef DEBUG_VERIFY_DENTRY
		if (name_len == 0)
			printf
			    ("Trying to find the name length for %d-th entry\n",
			     i);
#endif /* DEBUG_VERIFY_DENTRY */
		if (is_dot(name, name_len)) {
			if (i != 0)
				fsck_log
				    ("block %lu: item %d: \".\" must be the first entry, but it is the %d-th entry\n",
				     bh->b_blocknr, item_num, i);
			/* check and fix "." */

			if (get_deh_offset(deh + i) != DOT_OFFSET) {
				set_deh_offset(deh + i, DOT_OFFSET);
				mark_buffer_dirty(bh);
			}
			/* "." must point to the directory it is in */
/*
	    if (not_of_one_file (&(deh[i].deh2_dir_id), &(ih->ih_key))) {
		fsck_log ("verify_direntry: block %lu, item %H has entry \".\" "
			  "pointing to (%K) instead of (%K)\n",
			  bh->b_blocknr, ih,
			  &(deh[i].deh2_dir_id), &(ih->ih_key));
		set_deh_dirid (deh + i, get_key_dirid (&ih->ih_key));
		set_deh_objectid (deh + i, get_key_objectid (&ih->ih_key));
		mark_buffer_dirty (bh);
	    }
*/
		} else if (is_dot_dot(name, name_len)) {
			if (i != 1)
				fsck_log
				    ("block %lu: item %d: \"..\" is %d-th entry\n",
				     bh->b_blocknr, item_num, i);

			/* check and fix ".." */
			if (get_deh_offset(deh + i) != DOT_DOT_OFFSET) {
				set_deh_offset(deh + i, DOT_DOT_OFFSET);
				mark_buffer_dirty(bh);
			}
		} else {
			int min_length, max_length;

			/* check other name */

			if (name_len == 0) {
				/* we do not know the length of name - we will try to find it */
				min_length = 1;
				max_length = item + get_ih_item_len(ih) - name;
			} else
				/* we kow name length, so we will try only one name length */
				min_length = max_length = name_len;

			hash_code = 0;

			for (j = min_length; j <= max_length; j++) {
				hash_code =
				    find_hash_in_use(name, j,
						     get_deh_offset(deh + i),
						     get_sb_hash_code(fs->
								      fs_ondisk_sb));
/*		add_hash_hit (fs, hash_code);*/
				if (code2func(hash_code) != NULL) {
					/* deh_offset matches to some hash of the name */
					if (fsck_hash_defined(fs) &&
					    hash_code !=
					    func2code(fs->fs_hash_function)) {
						/* wrong hash selected - so we can skip this leaf */
						return 1;
					}

					if (!name_len) {
						fsck_log
						    ("%s: block %lu, item %H: Found a name \"%.*s\" for %d-th entry "
						     "matching to the hash %u.\n",
						     __FUNCTION__,
						     bh->b_blocknr, ih, j, name,
						     i,
						     get_deh_offset(deh + i));
						/* FIXME: if next byte is 0 we think that the name is aligned to 8 byte boundary */
						if (i) {
							set_deh_location(&deh
									 [i -
									  1],
									 get_deh_location
									 (deh +
									  i) +
									 ((name
									   [j]
									   ||
									   fs->
									   fs_format
									   ==
									   REISERFS_FORMAT_3_5)
									  ? j :
									  ROUND_UP
									  (j)));
							mark_de_good_location
							    (deh + i - 1);
							mark_buffer_dirty(bh);
						}
					}
					break;
				}
			}

			add_hash_hit(fs, hash_code);

			if (j == max_length + 1) {
				/* deh_offset does not match to anything. it will be
				   deleted for now, but maybe we could just fix a
				   deh_offset if it is in ordeer */
				mark_de_bad_offset(deh + i);
			}
		}
	}			/* for */

#ifdef DEBUG_VERIFY_DENTRY
	printf("Entries with mismatching hash: ");
	for (i = 0; i < ih_entry_count(ih); i++) {
		if (de_bad_offset(deh + i))
			printf("%d ", i);
	}
	printf("\n");
#endif /* DEBUG_VERIFY_DENTRY */

	/* correct deh_locations such that code cutting entries will not get
	   screwed up */
	{
		int prev_loc;
#ifdef DEBUG_VERIFY_DENTRY
		int loc_fixed;
#endif

		prev_loc = get_ih_item_len(ih);
		for (i = 0; i < get_ih_entry_count(ih); i++) {
#ifdef DEBUG_VERIFY_DENTRY
			loc_fixed = 0;
#endif
			if (de_bad_location(deh + i)) {
				set_deh_location(deh + i, prev_loc /* - 1 */ );
				mark_buffer_dirty(bh);
#ifdef DEBUG_VERIFY_DENTRY
				loc_fixed = 1;
#endif
			} else {
				if (get_deh_location(deh + i) >= prev_loc) {
					set_deh_location(deh + i,
							 prev_loc /* - 1 */ );
					mark_buffer_dirty(bh);
#ifdef DEBUG_VERIFY_DENTRY
					loc_fixed = 1;
#endif
				}
			}

			prev_loc = get_deh_location(deh + i);

			if (i == get_ih_entry_count(ih) - 1) {
				/* last entry starts right after an array of dir entry headers */
				if (!de_bad(deh + i) &&
				    get_deh_location(deh + i) !=
				    (DEH_SIZE * get_ih_entry_count(ih))) {
					/* free space in the directory item */
					fsck_log
					    ("%s: block %lu, item %H: Directory item has a free space - deleting\n",
					     __FUNCTION__, bh->b_blocknr, ih);
					cut_entry(fs, bh, item_num,
						  get_ih_entry_count(ih), 0);
				}
				if (get_deh_location(deh + i) !=
				    (DEH_SIZE * get_ih_entry_count(ih))) {
					set_deh_location(&deh[i],
							 (DEH_SIZE *
							  get_ih_entry_count
							  (ih)));
#ifdef DEBUG_VERIFY_DENTRY
					loc_fixed = 1;
#endif
					mark_buffer_dirty(bh);
				}
			}
#ifdef DEBUG_VERIFY_DENTRY
			if (loc_fixed)
				direntries[i] = 1;
#endif
		}		/* for */

#ifdef DEBUG_VERIFY_DENTRY
		printf("Entries with fixed deh_locations: ");
		for (i = 0; i < ih_entry_count(ih); i++) {
			if (direntries[i])
				printf("%d ", i);
		}
		printf("\n");
#endif /* DEBUG_VERIFY_DENTRY */

	}

#ifdef DEBUG_VERIFY_DENTRY
	printf(" N  location name\n");
	for (i = 0; i < ih_entry_count(ih); i++) {
		if (de_bad(deh + i) || (i && de_bad(deh + i - 1)) ||	/* previous entry marked bad */
		    (i < ih_entry_count(ih) - 1 && de_bad(deh + i + 1))) {	/* next entry is marked bad */
			/* print only entries to be deleted and their nearest neighbors */
			printf("%3d: %8d ", i, deh_location(deh + i));
			if (de_bad(deh + i))
				printf("will be deleted\n");
			else
				printf("\"%.*s\"\n",
				       name_in_entry_length(ih, deh + i, i),
				       name_in_entry(deh + i, i));
		}
	}
#endif

	bad = lost_found = 0;
	tmp = *ih;

	/* mark enries of /lost+found as bad */
	deh = B_I_DEH(bh, ih);
	for (i = 0; i < get_ih_entry_count(ih); i++, deh++) {
		unsigned int dirid, objectid;

		if (de_bad(deh))
			continue;

		sprintf(buf, "%.*s", name_in_entry_length(ih, deh, i),
			name_in_entry(deh, i));
		if (sscanf(buf, "%d_%d", &dirid, &objectid) != 2)
			continue;

		if (get_deh_dirid(deh) != dirid
		    || get_deh_objectid(deh) != objectid)
			continue;

		/* entry in lost+found */
//      printf ("%s - will be deleted\n", buf);
		lost_found++;
		mark_de_bad_offset(deh);
	}

	/* delete entries which are marked bad */
	for (i = 0; i < get_ih_entry_count(ih); i++) {
		deh = B_I_DEH(bh, ih) + i;
		if (de_bad(deh)) {
			bad++;
			if (get_ih_entry_count(ih) == 1) {
				delete_item(fs, bh, item_num);
				break;
			} else {
				cut_entry(fs, bh, item_num, i, 1);
			}
			i--;
		}
	}

	if (bad == get_ih_entry_count(&tmp)) {
		if (lost_found != bad) {
			fsck_log
			    ("%s: block %lu, item %H: All entries were deleted from the directory\n",
			     __FUNCTION__, bh->b_blocknr, &tmp);
		}
		return -1;
	}

	deh = B_I_DEH(bh, ih);
	if (get_offset(&ih->ih_key) != get_deh_offset(deh)) {
		fsck_log
		    ("verify_direntry: block %lu, item %H: Key's offset %k must match the directory's hash (%u) - changed.\n",
		     bh->b_blocknr, ih, &ih->ih_key, get_deh_offset(deh));
		set_offset(key_format(&ih->ih_key), &ih->ih_key,
			   get_deh_offset(deh));
		mark_buffer_dirty(bh);
	}

	if (bad > lost_found)
		fsck_log("pass0: block %lu, item %H: %d entries were deleted\n",
			 bh->b_blocknr, &tmp, bad - lost_found);

	return 0;

}

static __inline__ int does_it_fit_into_dev(__u64 offset, __u64 fs_size)
{
/*
   Count of unformatted pointers - offset / blocksize
   Count of blocks to store them - UNFM_P_SIZE * offset / blocksize / MAX_ITEM_LEN
   Size to store it              - blocksize * UNFM_P_SIZE * offset / blocksize / MAX_ITEM_LEN
*/
	return (UNFM_P_SIZE * offset / MAX_ITEM_LEN(fs->fs_blocksize) <
		fs_size) ? 1 : 0;
}

static int is_wrong_short_key(const struct reiserfs_key *key)
{
	if (get_key_dirid(key) == 0 || get_key_objectid(key) == 0
	    || get_key_objectid(key) == 1 || get_key_dirid(key) == ~(__u32) 0
	    || get_key_objectid(key) == ~(__u32) 0
	    || get_key_dirid(key) == get_key_objectid(key) ||
	    /* the alloc=packing_groups used to allow dirid = 1
	       (get_key_dirid (key) == 1 && get_key_objectid (key) != 2) || */
	    (get_key_dirid(key) != 1 && get_key_objectid(key) == 2))
		return 1;

	return 0;
}

/* 1 if some of fields in the block head of bh look bad */
int leaf_structure_check(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	struct block_head *blkh;
	int free_space, counted;

	blkh = B_BLK_HEAD(bh);

	if (!is_leaf_block_head(bh->b_data)) {
		/* We should not get here on rebuild. */
		fsck_log("block %lu: The block does not look like a leaf.\n",
			 bh->b_blocknr);
		one_more_corruption(fs, FATAL);
		return 1;
	}

	if (get_blkh_nr_items(blkh) == 0)
		return 0;

	counted = leaf_count_ih(bh->b_data, bh->b_size);

	if (counted < get_blkh_nr_items(blkh)) {
		fsck_log
		    ("block %lu: The number of items (%lu) is incorrect, should be (%lu)",
		     bh->b_blocknr, get_blkh_nr_items(blkh), counted);
		if (fsck_mode(fs) == FSCK_REBUILD) {
			set_blkh_nr_items(blkh, counted);
			fsck_log(" - corrected\n");
			mark_buffer_dirty(bh);
		} else {
			fsck_log("\n");
			one_more_corruption(fs, FATAL);
			return 1;
		}
	}

	free_space = leaf_free_space_estimate(bh->b_data, bh->b_size);
	if (get_blkh_free_space(blkh) != free_space) {
		fsck_log
		    ("block %lu: The free space (%lu) is incorrect, should be (%lu)",
		     bh->b_blocknr, get_blkh_free_space(blkh), free_space);

		if (fsck_mode(fs) != FSCK_CHECK && fsck_mode(fs) != FSCK_AUTO) {
			set_blkh_free_space(blkh, free_space);
			fsck_log(" - corrected\n");
			mark_buffer_dirty(bh);
		} else {
			fsck_log("\n");
			one_more_corruption(fs, FIXABLE);
			return 1;
		}
	}

	return 0;
}

/* do this on pass 0 with every leaf marked used */

/* FIXME: we can improve fixing of broken keys: we can ssfe direct items which
   go after stat data and have broken keys */
static void pass0_correct_leaf(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	int file_format = KEY_FORMAT_UNDEFINED;
	struct item_head *ih;

	__le32 *ind_item;
	__u64 fs_size;
	__u64 offset;
	int symlnk = 0;

	unsigned long unfm_ptr;
//    unsigned int nr_items;
	int i, j, nr_items;
	int dirty = 0;

	leaf_structure_check(fs, bh);

	/* Delete all safe links. */
	for (i = get_blkh_nr_items(B_BLK_HEAD(bh)) - 1; i >= 0; i--) {
		if (get_key_dirid(&item_head(bh, i)->ih_key) == ~(__u32) 0) {
			delete_item(fs, bh, i);
		}
		if (get_key_dirid(&item_head(bh, i)->ih_key) ==
		    BADBLOCK_DIRID
		    && get_key_objectid(&item_head(bh, i)->ih_key) ==
		    BADBLOCK_OBJID) {
			delete_item(fs, bh, i);
		}
	}

	fs_size =
	    (__u64) fs->fs_blocksize * get_sb_block_count(fs->fs_ondisk_sb);

start_again:

	ih = item_head(bh, 0);
	for (i = 0; i < (nr_items = get_blkh_nr_items(B_BLK_HEAD(bh)));
	     i++, ih++) {

		if (is_indirect_ih(ih) && (get_ih_item_len(ih) % 4 != 0)) {
			set_type(get_ih_key_format(ih), &ih->ih_key,
				 TYPE_UNKNOWN);
			dirty = 1;
		}

		if (type_unknown(&ih->ih_key)) {
			if ((get_ih_item_len(ih) == SD_SIZE)
			    || (get_ih_item_len(ih) == SD_V1_SIZE)) {
				set_type_and_offset(KEY_FORMAT_1, &ih->ih_key,
						    SD_OFFSET, TYPE_STAT_DATA);
				dirty = 1;
				fsck_log
				    ("pass0: vpf-10100: block %lu, item (%d): Unknown item type of StatData size,"
				     " type set to StatData %k\n",
				     bh->b_blocknr, i, &ih->ih_key);
			} else {
				fsck_log
				    ("pass0: vpf-10110: block %lu, item (%d): Unknown item type found %k - deleted\n",
				     bh->b_blocknr, i, &ih->ih_key);
				delete_item(fs, bh, i);
				goto start_again;
			}
		}

		if (is_wrong_short_key(&ih->ih_key)) {
			/* sometimes stat datas get k_objectid==0 or k_dir_id==0 */

/*	    if (i == (nr_items - 1)) {
		
		if (i == 0) {
		    fsck_log ("block %lu: item %d: (%H) id equals 0\n",
			      bh->b_blocknr, i, ih);
		    return;
		}
		// delete last item
                fsck_log ("block %lu: item %d: (%H) id equals 0\n",
			      bh->b_blocknr, i, ih);
		delete_item (fs, bh, i - 1);
		return;
	    }
*/

			/* FIXME:
			   18842 19034    0x1 IND  (2)
			   19035 19035    0x0 SD   (0)
			   18842     1    0x1 IND  (1)
			   18842     1 0x1001 DRCT (2)
			 */
			/* there is next item: if it is not stat data - take its k_dir_id
			   and k_objectid. if key order will be still wrong - the changed
			   item will be deleted */

			if (i && !is_stat_data_ih(ih)) {
				/* previous item has a wrong short_key */
				fsck_log
				    ("pass0: vpf-10120: block %lu: item %d: Wrong key %k, corrected to ",
				     bh->b_blocknr, i, &ih->ih_key);
				set_key_dirid(&ih->ih_key,
					      get_key_dirid(&(ih - 1)->ih_key));
				set_key_objectid(&ih->ih_key,
						 get_key_objectid(&(ih - 1)->
								  ih_key));
				fsck_log("%k\n", &ih->ih_key);
				dirty = 1;
			} else if ((i < nr_items - 1)
				   && !is_stat_data_ih(ih + 1)) {
				if (!is_wrong_short_key(&(ih + 1)->ih_key)) {
					fsck_log
					    ("pass0: vpf-10130: block %lu: item %d: Wrong key %k, corrected to ",
					     bh->b_blocknr, i, &ih->ih_key);
					set_key_dirid(&ih->ih_key,
						      get_key_dirid(&(ih + 1)->
								    ih_key));
					set_key_objectid(&ih->ih_key,
							 get_key_objectid(&
									  (ih +
									   1)->
									  ih_key));
/*		    set_offset (KEY_FORMAT_1, &ih->ih_key, 0);
		    set_type (KEY_FORMAT_1, &ih->ih_key, TYPE_STAT_DATA);*/
					fsck_log("%k\n", &ih->ih_key);
					dirty = 1;
					goto start_again;
				} else {
					fsck_log
					    ("pass0: vpf-10140: block %lu: items %d and %d have bad short keys %k, %k, both deleted\n",
					     bh->b_blocknr, i, i + 1,
					     &ih->ih_key, &(ih + 1)->ih_key);
					delete_item(fs, bh, i);
					delete_item(fs, bh, i);
					goto start_again;
				}
			} else {
				fsck_log
				    ("pass0: vpf-10150: block %lu: item %d: Wrong key %k, deleted\n",
				     bh->b_blocknr, i, &ih->ih_key);
				delete_item(fs, bh, i);
				goto start_again;
			}
		}

		if (i && i + 1 < nr_items) {
			if (is_stat_data_ih(ih - 1) && !is_stat_data_ih(ih) &&
			    !is_direct_ih(ih + 1) && !is_stat_data_ih(ih + 1)) {
				/* i or i+1 item should be SD or i+1 should be direct item */
				if ((get_ih_item_len(ih) == SD_SIZE)
				    || (get_ih_item_len(ih) == SD_V1_SIZE)) {
					/* make i as SD */
					fsck_log
					    ("pass0: vpf-10400: block %lu, item %d: Wrong order of items - "
					     "change the type of the key %k to StatData\n",
					     bh->b_blocknr, i, &ih->ih_key);
					set_type_and_offset(KEY_FORMAT_1,
							    &ih->ih_key,
							    SD_OFFSET,
							    TYPE_STAT_DATA);
					dirty = 1;
				} else if ((get_ih_item_len(ih + 1) == SD_SIZE)
					   || (get_ih_item_len(ih + 1) ==
					       SD_V1_SIZE)) {
					/* make i+1 as SD */
					fsck_log
					    ("pass0: vpf-10410: block %lu, item %d: Wrong order of items - "
					     "change the type of the key %k to StatData\n",
					     bh->b_blocknr, i + 1,
					     &(ih + 1)->ih_key);
					set_type_and_offset(KEY_FORMAT_1,
							    &(ih + 1)->ih_key,
							    SD_OFFSET,
							    TYPE_STAT_DATA);
					dirty = 1;
				} else if (is_indirect_ih(ih)) {
					fsck_log
					    ("pass0: vpf-10420: block %lu, item %d: Wrong order of items - "
					     "change the type of the key %k to Direct\n",
					     bh->b_blocknr, i + 1,
					     &(ih + 1)->ih_key);
					set_type(get_ih_key_format(ih + 1),
						 &(ih + 1)->ih_key,
						 TYPE_DIRECT);
					dirty = 1;
				}
			}
		}

		if (i && ((is_stat_data_ih(ih - 1) &&
			   !is_stat_data_ih(ih) &&
			   (get_key_dirid(&(ih - 1)->ih_key) !=
			    get_key_dirid(&ih->ih_key))) || (is_stat_data_ih(ih)
							     &&
							     (get_key_dirid
							      (&(ih - 1)->
							       ih_key) >
							      get_key_dirid
							      (&ih->
							       ih_key))))) {
			/* not the same dir_id of the same file or not increasing dir_id of different files */
			if ((i > 1)
			    && get_key_dirid(&(ih - 2)->ih_key) ==
			    get_key_dirid(&(ih - 1)->ih_key)) {
				/* fix i-th */
				if (!is_stat_data_ih(ih)) {
					fsck_log
					    ("pass0: vpf-10430: block %lu, item %d: Wrong order of items - "
					     "change the dir_id of the key %k to %lu\n",
					     bh->b_blocknr, i, &ih->ih_key,
					     get_key_dirid(&(ih - 1)->ih_key));
					set_key_dirid(&ih->ih_key,
						      get_key_dirid(&(ih - 1)->
								    ih_key));
					dirty = 1;
				} else if (i + 1 < nr_items) {
					fsck_log
					    ("pass0: vpf-10440: block %lu, item %d: Wrong order of items - "
					     "change the dir_id of the key %k to %lu\n",
					     bh->b_blocknr, i, &ih->ih_key,
					     get_key_dirid(&(ih + 1)->ih_key));
					set_key_dirid(&ih->ih_key,
						      get_key_dirid(&(ih + 1)->
								    ih_key));
					dirty = 1;
				}
			} else
			    if ((i + 1 < nr_items)
				&& get_key_dirid(&ih->ih_key) ==
				get_key_dirid(&(ih + 1)->ih_key)) {
				fsck_log
				    ("pass0: vpf-10450: block %lu, item %d: Wrong order of items - "
				     "change the dir_id of the key %k to %lu\n",
				     bh->b_blocknr, i - 1, &(ih - 1)->ih_key,
				     get_key_dirid(&ih->ih_key));
				/* fix (i - 1)-th */
				set_key_dirid(&(ih - 1)->ih_key,
					      get_key_dirid(&ih->ih_key));
				dirty = 1;
			}
		}

		if (i && i + 1 < nr_items) {
			/* there is a previous and a next items */
			if ((get_key_dirid(&(ih - 1)->ih_key) ==
			     get_key_dirid(&(ih + 1)->ih_key))
			    && (get_key_dirid(&(ih - 1)->ih_key) !=
				get_key_dirid(&ih->ih_key))) {
				fsck_log
				    ("pass0: vpf-10460: block %lu, item %d: Wrong order of items - "
				     "change the dir_id of the key %k to %lu\n",
				     bh->b_blocknr, i, &ih->ih_key,
				     get_key_dirid(&(ih - 1)->ih_key));
				set_key_dirid(&ih->ih_key,
					      get_key_dirid(&(ih - 1)->ih_key));
				dirty = 1;
			}
			if (is_stat_data_ih(ih - 1) && is_indirect_ih(ih)
			    && is_direct_ih(ih + 1)) {
				if ((get_key_objectid(&(ih - 1)->ih_key) ==
				     get_key_objectid(&(ih + 1)->ih_key))
				    && (get_key_objectid(&(ih - 1)->ih_key) !=
					get_key_objectid(&ih->ih_key))) {
					fsck_log
					    ("pass0: vpf-10470: block %lu, item %d: Wrong order of items - "
					     "change the object_id of the key %k to %lu\n",
					     bh->b_blocknr, i, &ih->ih_key,
					     get_key_objectid(&(ih - 1)->
							      ih_key));
					set_key_objectid(&ih->ih_key,
							 get_key_objectid(&
									  (ih -
									   1)->
									  ih_key));
					dirty = 1;
				}
				if ((get_key_objectid(&(ih - 1)->ih_key) ==
				     get_key_objectid(&ih->ih_key))
				    && (get_key_objectid(&(ih - 1)->ih_key) !=
					get_key_objectid(&(ih + 1)->ih_key))) {
					fsck_log
					    ("pass0: vpf-10480: block %lu, item %d: Wrong order of items - "
					     "change the object_id of the key %k to %lu\n",
					     bh->b_blocknr, i + 1,
					     &(ih + 1)->ih_key,
					     get_key_objectid(&(ih - 1)->
							      ih_key));
					set_key_objectid(&(ih + 1)->ih_key,
							 get_key_objectid(&
									  (ih -
									   1)->
									  ih_key));
					dirty = 1;
				}
				if ((get_key_objectid(&ih->ih_key) ==
				     get_key_objectid(&(ih + 1)->ih_key))
				    && (get_key_objectid(&(ih - 1)->ih_key) !=
					get_key_objectid(&(ih + 1)->ih_key))) {
					fsck_log
					    ("pass0: vpf-10490: block %lu, item %d: Wrong order of items - "
					     "change the object_id of the key %k to %lu\n",
					     bh->b_blocknr, i - 1,
					     &(ih - 1)->ih_key,
					     get_key_objectid(&(ih + 1)->
							      ih_key));
					set_key_objectid(&(ih - 1)->ih_key,
							 get_key_objectid(&
									  (ih +
									   1)->
									  ih_key));
					dirty = 1;
				}
				if ((get_key_dirid(&(ih - 1)->ih_key) ==
				     get_key_dirid(&ih->ih_key))
				    && (get_key_dirid(&(ih - 1)->ih_key) !=
					get_key_dirid(&(ih + 1)->ih_key))) {
					fsck_log
					    ("pass0: vpf-10500: block %lu, item %d: Wrong order of items - "
					     "change the dir_id of the key %k to %lu\n",
					     bh->b_blocknr, i + 1,
					     &(ih + 1)->ih_key,
					     get_key_dirid(&(ih - 1)->ih_key));
					set_key_dirid(&(ih + 1)->ih_key,
						      get_key_dirid(&(ih - 1)->
								    ih_key));
					dirty = 1;
				}
				if ((get_key_dirid(&ih->ih_key) ==
				     get_key_dirid(&(ih + 1)->ih_key))
				    && (get_key_dirid(&(ih - 1)->ih_key) !=
					get_key_dirid(&(ih + 1)->ih_key))) {
					fsck_log
					    ("pass0: vpf-10510: block %lu, item %d: Wrong order of items - "
					     "change the dir_id of the key %k to %lu\n",
					     bh->b_blocknr, i - 1,
					     &(ih - 1)->ih_key,
					     get_key_dirid(&(ih + 1)->ih_key));
					set_key_dirid(&(ih - 1)->ih_key,
						      get_key_dirid(&(ih + 1)->
								    ih_key));
					dirty = 1;
				}
			}
		}

		if (i && is_stat_data_ih(ih) &&
		    get_key_dirid(&(ih - 1)->ih_key) ==
		    get_key_dirid(&ih->ih_key)
		    && get_key_objectid(&(ih - 1)->ih_key) >=
		    get_key_objectid(&ih->ih_key)) {
			if ((i + 1 < nr_items) && !is_stat_data_ih(ih + 1)) {
				if (get_key_objectid(&(ih - 1)->ih_key) <
				    get_key_objectid(&(ih + 1)->ih_key)) {
					fsck_log
					    ("pass0: vpf-10520: block %lu, item %d: Wrong order of items - "
					     "change the object_id of the key %k to %lu\n",
					     bh->b_blocknr, i - 1,
					     &(ih - 1)->ih_key,
					     get_key_objectid(&(ih + 1)->
							      ih_key));
					set_key_objectid(&ih->ih_key,
							 get_key_objectid(&
									  (ih +
									   1)->
									  ih_key));
					dirty = 1;
				}
			}
		}

		if (i && is_stat_data_ih(ih - 1) && !is_stat_data_ih(ih) &&
		    (get_key_objectid(&(ih - 1)->ih_key) !=
		     get_key_objectid(&ih->ih_key))) {
			int err = 0;
			if (i > 1) {
				if (comp_short_keys
				    (&(ih - 2)->ih_key,
				     &(ih - 1)->ih_key) != -1)
					misc_set_bit(1, &err);
				if (comp_short_keys
				    (&(ih - 2)->ih_key, &ih->ih_key) != -1)
					misc_set_bit(2, &err);
			}
			if (i + 1 < nr_items) {
				if (comp_short_keys
				    (&(ih - 1)->ih_key,
				     &(ih + 1)->ih_key) != -1)
					misc_set_bit(3, &err);
				if (comp_short_keys
				    (&ih->ih_key, &(ih + 1)->ih_key) != -1)
					misc_set_bit(4, &err);
			}
/*
            if ((test_bit (1, err) || test_bit (3, err)) &&
		(test_bit (2, err) || test_bit (4, err))) {
      		// thera are no problem-free keys, delete them both
                delete_item (fs, bh, i - 1);
                delete_item (fs, bh, i - 1);
                goto start_again;
      	    }
*/
			if (!misc_test_bit(1, &err) && !misc_test_bit(3, &err)
			    && !misc_test_bit(2, &err)
			    && !misc_test_bit(4, &err)) {
				if (i <= 1) {
					/* take bigger */
					if (comp_short_keys
					    (&(ih - 1)->ih_key,
					     &ih->ih_key) == 1) {
						fsck_log
						    ("pass0: vpf-10530: block %lu, item %d: Wrong order of items - "
						     "change the object_id of the key %k to %lu\n",
						     bh->b_blocknr, i,
						     &ih->ih_key,
						     get_key_objectid(&
								      (ih -
								       1)->
								      ih_key));
						set_key_objectid(&ih->ih_key,
								 get_key_objectid
								 (&(ih - 1)->
								  ih_key));
					} else {
						fsck_log
						    ("pass0: vpf-10540: block %lu, item %d: Wrong order of items - "
						     "change the object_id of the key %k to %lu\n",
						     bh->b_blocknr, i - 1,
						     &(ih - 1)->ih_key,
						     get_key_objectid(&ih->
								      ih_key));
						set_key_objectid(&(ih - 1)->
								 ih_key,
								 get_key_objectid
								 (&ih->ih_key));
					}
				} else {
					/* take smaller */
					if (comp_short_keys
					    (&(ih - 1)->ih_key,
					     &ih->ih_key) == 1) {
						fsck_log
						    ("pass0: vpf-10550: block %lu, item %d: Wrong order of items - "
						     "change the object_id of the key %k to %lu\n",
						     bh->b_blocknr, i - 1,
						     &(ih - 1)->ih_key,
						     get_key_objectid(&ih->
								      ih_key));
						set_key_objectid(&(ih - 1)->
								 ih_key,
								 get_key_objectid
								 (&ih->ih_key));
					} else {
						fsck_log
						    ("pass0: vpf-10560: block %lu, item %d: Wrong order of items - "
						     "change the object_id of the key %k to %lu\n",
						     bh->b_blocknr, i,
						     &ih->ih_key,
						     get_key_objectid(&
								      (ih -
								       1)->
								      ih_key));
						set_key_objectid(&ih->ih_key,
								 get_key_objectid
								 (&(ih - 1)->
								  ih_key));
					}
				}
				dirty = 1;
			} else if (!misc_test_bit(1, &err)
				   && !misc_test_bit(3, &err)) {
				/* take i - 1 */
				fsck_log
				    ("pass0: vpf-10590: block %lu, item %d: Wrong order of items - "
				     "change the object_id of the key %k to %lu\n",
				     bh->b_blocknr, i, &ih->ih_key,
				     get_key_objectid(&(ih - 1)->ih_key));
				set_key_objectid(&ih->ih_key,
						 get_key_objectid(&(ih - 1)->
								  ih_key));
				dirty = 1;
			} else if (!misc_test_bit(2, &err)
				   && !misc_test_bit(4, &err)) {
				/* take i */
				fsck_log
				    ("pass0: vpf-10600: block %lu, item %d: Wrong order of items - "
				     "change the object_id of the key %k to %lu\n",
				     bh->b_blocknr, i - 1, &(ih - 1)->ih_key,
				     get_key_objectid(&ih->ih_key));
				set_key_objectid(&(ih - 1)->ih_key,
						 get_key_objectid(&ih->ih_key));
				dirty = 1;
			}
		}

		/* this recovers corruptions like the below:
		   1774 1732 0 0
		   116262638 1732 1 3
		   1774 1736 0 0 */
		if (i && is_stat_data_ih(ih - 1) && !is_stat_data_ih(ih)) {
			if (get_key_objectid(&ih->ih_key) !=
			    get_key_objectid(&(ih - 1)->ih_key)
			    || get_key_dirid(&ih->ih_key) !=
			    get_key_dirid(&(ih - 1)->ih_key)
			    || get_offset(&ih->ih_key) != 1) {
				if (is_direntry_ih(ih)) {
					fsck_log
					    ("pass0: vpf-10160: block %lu: item %d: No \".\" entry found in "
					     "the first item of a directory\n",
					     bh->b_blocknr, i);
					set_key_dirid(&ih->ih_key,
						      get_key_dirid(&(ih - 1)->
								    ih_key));
					set_key_objectid(&ih->ih_key,
							 get_key_objectid(&
									  (ih -
									   1)->
									  ih_key));
					dirty = 1;
				} else {
					fsck_log
					    ("pass0: vpf-10170: block %lu: item %d: Wrong order of items - "
					     "the item \n\t%H fixed to ",
					     bh->b_blocknr, i, ih);

					set_key_dirid(&ih->ih_key,
						      get_key_dirid(&(ih - 1)->
								    ih_key));
					set_key_objectid(&ih->ih_key,
							 get_key_objectid(&
									  (ih -
									   1)->
									  ih_key));

					if (get_ih_item_len(ih - 1) == SD_SIZE) {
						/* stat data is new, therefore this item is new too */
						set_offset(KEY_FORMAT_2,
							   &(ih->ih_key), 1);
						if ((get_ih_entry_count(ih) !=
						     0xffff)
						    && (get_ih_item_len(ih) %
							4 == 0))
							set_type(KEY_FORMAT_2,
								 &(ih->ih_key),
								 TYPE_INDIRECT);
						else
							set_type(KEY_FORMAT_2,
								 &(ih->ih_key),
								 TYPE_DIRECT);
						set_ih_key_format(ih,
								  KEY_FORMAT_2);
					} else {
						/* stat data is old, therefore this item is old too */
						set_offset(KEY_FORMAT_1,
							   &(ih->ih_key), 1);
						if ((get_ih_entry_count(ih) !=
						     0xffff)
						    && (get_ih_item_len(ih) %
							4 == 0))
							set_type(KEY_FORMAT_1,
								 &(ih->ih_key),
								 TYPE_INDIRECT);
						else {
							set_type(KEY_FORMAT_1,
								 &(ih->ih_key),
								 TYPE_DIRECT);
							set_ih_free_space(ih,
									  0xffff);
						}
						set_ih_key_format(ih,
								  KEY_FORMAT_1);
					}
					fsck_log("\n\t%H\n", ih);
					dirty = 1;
				}
			}
		}

		/* FIXME: corruptions like:
		   56702 66802 1 2
		   56702 65536 0 0
		   56702 66803 1 2
		   do not get recovered (both last items will be deleted) */
		/* delete item if it is not in correct order of object items */
		if (i && not_of_one_file(&ih->ih_key, &(ih - 1)->ih_key) &&
		    !is_stat_data_ih(ih)) {
			fsck_log
			    ("pass0: vpf-10180: block %lu: item %d: The item %k, which follows non StatData item %k, is deleted\n",
			     bh->b_blocknr, i, &ih->ih_key, &(ih - 1)->ih_key);
			delete_item(fs, bh, i);
			goto start_again;
		}

		if (is_stat_data_ih(ih)) {
			if (get_offset(&ih->ih_key) != 0) {
				set_offset(KEY_FORMAT_1, &ih->ih_key, 0);
				dirty = 1;
			}
		} else if (!is_direntry_ih(ih)) {
			/* not SD, not direntry */
			if (i
			    && comp_short_keys(&(ih - 1)->ih_key,
					       &ih->ih_key) == 0) {
				if (is_stat_data_ih(ih - 1)) {
					if (get_offset(&ih->ih_key) != 1) {
						set_offset(key_format
							   (&ih->ih_key),
							   &ih->ih_key, 1);
						dirty = 1;
					}
				} else if (is_indirect_ih(ih - 1)
					   && is_direct_ih(ih)) {
					if (get_offset(&ih->ih_key) !=
					    get_offset(&(ih - 1)->ih_key) +
					    I_UNFM_NUM(ih -
						       1) * fs->fs_blocksize) {
						set_offset(key_format
							   (&ih->ih_key),
							   &ih->ih_key,
							   get_offset(&
								      (ih -
								       1)->
								      ih_key) +
							   I_UNFM_NUM(ih -
								      1) *
							   fs->fs_blocksize);
						dirty = 1;
					}
				} else {
					/* if indirect item or not the first direct item in the leaf */
					fsck_log
					    ("pass0: vpf-10250: block %lu, item %d: The item %k with wrong type is deleted\n",
					     bh->b_blocknr, i, &ih->ih_key);
					delete_item(fs, bh, i);
					goto start_again;
				}

				/* Check the lenght of the direct item; offset should be ok already. */
				if (is_direct_ih(ih)) {
					if (STORE_TAIL_IN_UNFM
					    (get_offset(&ih->ih_key) +
					     get_ih_item_len(ih) - 1,
					     get_ih_item_len(ih), bh->b_size)) {
						fsck_log
						    ("pass0: vpf-10700: block %lu, item %d: The item with wrong offset"
						     " or length found %k, len % lu - deleted\n",
						     bh->b_blocknr, i,
						     &ih->ih_key,
						     get_ih_item_len(ih));
						delete_item(fs, bh, i);
						goto start_again;
					}
				}
			} else {
				/*first item in the node or first item of the file */

				if (i) {
					/* first item of the file, but not SD - delete */
					fsck_log
					    ("pass0: vpf-10190: block %lu, item %d: The item %k, which follows non StatData"
					     " item %k, is deleted\n",
					     bh->b_blocknr, i, &ih->ih_key,
					     &(ih - 1)->ih_key);
					delete_item(fs, bh, i);
					goto start_again;
				}

				/* indirect or direct is the first in the leaf */
				offset = (__u64) get_offset(&ih->ih_key);
				if (is_indirect_ih(ih)) {
					if (offset % fs->fs_blocksize != 1) {
						fsck_log
						    ("pass0: vpf-10200: block %lu, item %d: The item %k with wrong offset is deleted\n",
						     bh->b_blocknr, i,
						     &ih->ih_key);
						delete_item(fs, bh, i);
						goto start_again;
					}
				} else if (is_direct_ih(ih)) {
					if (!correct_direct_item_offset
					    (get_offset(&ih->ih_key),
					     key_format(&ih->ih_key))
					    ||
					    ((get_offset(&ih->ih_key) %
					      bh->b_size - 1) +
					     get_ih_item_len(ih) > bh->b_size)
					    || STORE_TAIL_IN_UNFM(offset +
								  get_ih_item_len
								  (ih) - 1,
								  get_ih_item_len
								  (ih),
								  bh->b_size)) {
						fsck_log
						    ("pass0: vpf-10210: block %lu, item %d: The item with wrong offset ",
						     bh->b_blocknr, i);
						fsck_log
						    ("or length found %k, len % lu - deleted\n",
						     &ih->ih_key,
						     get_ih_item_len(ih));
						delete_item(fs, bh, i);
						goto start_again;
					}
				}

				offset +=
				    get_bytes_number(ih, fs->fs_blocksize);
				if (!does_it_fit_into_dev(offset, fs_size)) {
					fsck_log
					    ("pass0: vpf-10230: block %lu, item %d: The item offset is is too big %k - deleted\n",
					     bh->b_blocknr, i, &ih->ih_key);
					delete_item(fs, bh, i);
					goto start_again;
				}

			}
		}

		if (i && comp_keys(&(ih - 1)->ih_key, &ih->ih_key) != -1) {
			/* previous item has key not smaller than the key of currect item */
			if (is_stat_data_ih(ih - 1) && !is_stat_data_ih(ih)) {
				/* fix key of stat data such as if it was stat data of that item */
				fsck_log
				    ("pass0: block %lu, items %d, %d: Wrong order of items - make the StatData item %k of the file %k\n",
				     bh->b_blocknr, i - 1, i, &(ih - 1)->ih_key,
				     &ih->ih_key);
				set_key_dirid(&(ih - 1)->ih_key,
					      get_key_dirid(&ih->ih_key));
				set_key_objectid(&(ih - 1)->ih_key,
						 get_key_objectid(&ih->ih_key));
				set_offset(KEY_FORMAT_1, &(ih - 1)->ih_key, 0);
				set_type(KEY_FORMAT_1, &(ih - 1)->ih_key,
					 TYPE_STAT_DATA);
				dirty = 1;
			} else {
				/* ok, we have to delete one of these two - decide which one */
				int retval;

				/* something will be deleted */
				dirty = 1;
				retval = upper_correct(bh, ih - 1, i - 1);
				switch (retval) {
				case 0:
					/* delete upper item */
					fsck_log
					    ("pass0: block %lu, item %d (upper): Item %k is out of order - deleted\n",
					     bh->b_blocknr, i - 1,
					     &(ih - 1)->ih_key);
					delete_item(fs, bh, i - 1);
					goto start_again;

				case 1:
					/* delete lower item */
					fsck_log
					    ("pass0: block %lu, item %d (lower): Item %k is out of order - deleted\n",
					     bh->b_blocknr, i, &ih->ih_key);
					delete_item(fs, bh, i);
					goto start_again;

				default:
					/* upper item was the first item of a node */
					/* to make gcc 3.2 do not sware here */
					    ;
				}

				retval = lower_correct(bh, ih, i);
				switch (retval) {
				case 0:
					/* delete lower item */
					fsck_log
					    ("pass0: block %lu, item %d (lower): Item %k is out of order - deleted\n",
					     bh->b_blocknr, i, &ih->ih_key);
					delete_item(fs, bh, i);
					goto start_again;

				case 1:
					/* delete upper item */
					fsck_log
					    ("pass0: block %lu, %d (upper): Item %k is out of order - deleted\n",
					     bh->b_blocknr, i - 1,
					     &(ih - 1)->ih_key);
					delete_item(fs, bh, i - 1);
					goto start_again;

				default:
					/* only 2 items in the node, how to decide what to delete, go and ask user */
					/* to make gcc 3.2 do not sware here */
					    ;
				}
				fsck_log
				    ("pass0: block %lu, items %d and %d: Which of these items looks better (the other will be deleted.)?\n"
				     "%k\n%k\n", bh->b_blocknr, i - 1, i,
				     &(ih - 1)->ih_key, &ih->ih_key);
				if (fsck_user_confirmed
				    (fs, "1 or 2?", "1\n", 1))
					delete_item(fs, bh, i - 1);
				else
					delete_item(fs, bh, i);
				goto start_again;
			}
		}

		if (is_stat_data_ih(ih) && (get_ih_item_len(ih) != SD_SIZE &&
					    get_ih_item_len(ih) !=
					    SD_V1_SIZE)) {
			fsck_log
			    ("pass0: block %lu, item %d: StatData item of wrong length found %H - deleted\n",
			     bh->b_blocknr, i, ih);
			delete_item(fs, bh, i);
			goto start_again;
		}

		dirty += correct_key_format(ih, symlnk);

		if (is_stat_data_ih(ih)) {
			__u16 mode;

			file_format = get_ih_key_format(ih);

			get_sd_mode(ih, item_body(bh, i), &mode);
			symlnk = (S_ISLNK(mode) ? 1 : 0);
			;	/*correct_stat_data (fs, bh, i); */
		} else if (!is_direntry_ih(ih) &&
			   !(symlnk && is_direct_ih(ih)) &&
			   (file_format != KEY_FORMAT_UNDEFINED) &&
			   (file_format != get_ih_key_format(ih))) {
			fsck_log
			    ("pass0: vpf-10240: block %lu, item (%d): Item %k, which format (%d) is not equal to StatData "
			     "format (%d), is deleted\n", bh->b_blocknr, i,
			     &ih->ih_key, get_ih_key_format(ih), file_format);
			delete_item(fs, bh, i);
			goto start_again;
		} else {
			file_format = KEY_FORMAT_UNDEFINED;
			symlnk = 0;
		}

		if (i && is_stat_data_ih(ih - 1)
		    && !not_of_one_file(&ih->ih_key, &(ih - 1)->ih_key)) {
			__u16 mode;

			get_sd_mode(ih - 1, ih_item_body(bh, ih - 1), &mode);
			if (not_a_directory(ih_item_body(bh, ih - 1))
			    && is_direntry_ih(ih)) {
				/* make SD mode SD of dir */
				fsck_log
				    ("pass0: block %lu, item %d: The directory %K has the wrong mode (%M), corrected to ",
				     bh->b_blocknr, i, &ih->ih_key, mode);
				mode &= ~S_IFMT;
				mode |= S_IFDIR;
				fsck_log("(%M)\n", mode);
				set_sd_mode(ih - 1, ih_item_body(bh, ih - 1),
					    &mode);
				dirty = 1;
			} else if (!not_a_directory(ih_item_body(bh, ih - 1))
				   && !is_direntry_ih(ih)) {
				/* make SD mode SD of regular file */
				fsck_log
				    ("pass0: block %lu, item %d: Not the directory %K has the wrong mode (%M), corrected to ",
				     bh->b_blocknr, i, &ih->ih_key, mode);
				mode &= ~S_IFMT;
				mode |= S_IFREG;
				fsck_log("(%M)\n", mode);
				set_sd_mode(ih - 1, ih_item_body(bh, ih - 1),
					    &mode);
				dirty = 1;
			}
			if (not_a_regfile(ih_item_body(bh, ih - 1))
			    && is_indirect_ih(ih)) {
				fsck_log
				    ("pass0: block %lu, item %d: The file %K has the wrong mode (%M), corrected to ",
				     bh->b_blocknr, i, &ih->ih_key, mode);
				mode &= ~S_IFMT;
				mode |= S_IFREG;
				fsck_log("(%M)\n", mode);
				set_sd_mode(ih - 1, ih_item_body(bh, ih - 1),
					    &mode);
				dirty = 1;
			}
		}

		if (is_direntry_ih(ih)) {
			j = verify_directory_item(fs, bh, i);

			if (j == 1) {
				/* wrong hash, skip the leaf */
				pass_0_stat(fs)->too_old_leaves++;
				mark_buffer_clean(bh);
				return;
			} else if (j == -1) {
				/* item was deleted */
				goto start_again;
			}
			continue;
		}

		/*DEBUG*/
		    if (!is_stat_data_ih(ih) && get_offset(&ih->ih_key) == 0)
			reiserfs_panic
			    ("block %lu, item %d: Zero offset can have StatData items only, but found %k\n",
			     bh->b_blocknr, i, &ih->ih_key);

		if (!is_indirect_ih(ih))
			continue;

		/* Darko Palic's filesystem had item: [20079 17328 0xfffb1001 IND, len 4048], format old */
		{
			struct reiserfs_key tmp_key;

			tmp_key = ih->ih_key;
			set_offset(key_format(&tmp_key), &tmp_key,
				   get_offset(&tmp_key) + get_bytes_number(ih,
									   bh->
									   b_size)
				   - 1);

			if (get_offset(&tmp_key) < get_offset(&ih->ih_key)) {
				fsck_log
				    ("pass0: block %lu, item %d: The item, which has wrong offset %k, is deleted\n",
				     bh->b_blocknr, i, &ih->ih_key);
				delete_item(fs, bh, i);
				goto start_again;
			}
		}

		/* temporary ugly fix */
/*
	if (i && is_stat_data_ih (ih - 1) && !not_of_one_file (&ih->ih_key, &(ih - 1)->ih_key)) {
	    __u16 mode;

	    get_sd_mode (ih - 1, B_I_PITEM (bh, ih - 1), &mode);
	    if (!S_ISREG (mode)) {
		fsck_log ("pass0: block %lu: mode (%M) of file %K having indirect item is fixed to ",
			  bh->b_blocknr, mode, &ih->ih_key);
		mode &= ~S_IFMT;
		mode |= S_IFREG;
		set_sd_mode (ih - 1, B_I_PITEM (bh, ih - 1), &mode);
		fsck_log ("(%M)\n", mode);
		dirty = 1;
	    }
	}
*/
		ind_item = (__le32 *) ih_item_body(bh, ih);
		for (j = 0; j < (int)I_UNFM_NUM(ih); j++) {
			unfm_ptr = d32_get(ind_item, j);
			if (!unfm_ptr)
				continue;
#if 0
			if (fsck_mode(fs) == FSCK_ZERO_FILES) {
				/* FIXME: this is temporary mode of fsck */
				ind_item[j] = 0;
				reiserfs_bitmap_clear_bit(fsck_new_bitmap(fs),
							  unfm_ptr);
				tmp_zeroed++;
				dirty = 1;
				continue;
			}
#endif

			if (not_data_block(fs, unfm_ptr) ||	/* journal area or bitmap or super block */
			    unfm_ptr >= get_sb_block_count(fs->fs_ondisk_sb) ||	/* garbage in pointer */
			    (fs->fs_badblocks_bm &&	/* bad block */
			     reiserfs_bitmap_test_bit(fs->fs_badblocks_bm,
						      unfm_ptr))) {

				pass_0_stat(fs)->wrong_pointers++;
				/*
				   fsck_log ("pass0: %d-th pointer (%lu) in item %k (leaf block %lu) is wrong\n",
				   j, unfm_ptr, &ih->ih_key, bh->b_blocknr);
				 */
				d32_put(ind_item, j, 0);
				dirty = 1;
				continue;
			}
			/* mark block in bitmaps of unformatted nodes */
			register_unfm(unfm_ptr);
		}
	}

	/* mark all objectids in use */
	ih = item_head(bh, 0);
	for (i = 0; i < get_blkh_nr_items(B_BLK_HEAD(bh)); i++, ih++) {
		struct reiserfs_de_head *deh;

		id_map_mark(proper_id_map(fs), get_key_dirid(&ih->ih_key));
		id_map_mark(proper_id_map(fs), get_key_objectid(&ih->ih_key));
		if (is_direntry_ih(ih)) {
			for (j = 0, deh = B_I_DEH(bh, ih);
			     j < get_ih_entry_count(ih); j++, deh++)
				id_map_mark(proper_id_map(fs),
					    get_deh_objectid(deh));
		}
	}

	if (get_blkh_nr_items(B_BLK_HEAD(bh)) < 1) {
		/* pass 1 will skip this */
		pass_0_stat(fs)->all_contents_removed++;
//      fsck_log ("pass0: block %lu: All items were deleted.\n", bh->b_blocknr);
		dirty = 0;
		mark_buffer_clean(bh);
	} else {
		/* pass1 will use this bitmap */
		pass0_mark_leaf(bh->b_blocknr);
		/*fsck_data (fs)->rebuild.leaves ++; */
	}

	if (dirty) {
		pass_0_stat(fs)->leaves_corrected++;
		mark_buffer_dirty(bh);
	}
}

static int is_bad_sd(struct item_head *ih, char *item)
{
	struct stat_data *sd = (struct stat_data *)item;

	if (get_key_offset_v1(&ih->ih_key) || get_key_uniqueness(&ih->ih_key)) {
		fsck_log
		    ("vpf-10610: StatData item %k has non zero offset found.\n",
		     &ih->ih_key);
		return 1;
	}

	if (get_ih_item_len(ih) == SD_V1_SIZE) {
		/* looks like old stat data */
		if (get_ih_key_format(ih) != KEY_FORMAT_1)
			fsck_log
			    ("vpf-10620: StatData item %k has wrong format.\n",
			     &ih->ih_key);
		return 0;
	}

	if (!S_ISDIR(sd_v2_mode(sd)) && !S_ISREG(sd_v2_mode(sd)) &&
	    !S_ISCHR(sd_v2_mode(sd)) && !S_ISBLK(sd_v2_mode(sd)) &&
	    !S_ISLNK(sd_v2_mode(sd)) && !S_ISFIFO(sd_v2_mode(sd)) &&
	    !S_ISSOCK(sd_v2_mode(sd))) {
		/*fsck_log ("file %k unexpected mode encountered 0%o\n", &ih->ih_key, sd_v2_mode(sd)) */
		    ;
	}
	return 0;
}

int is_bad_directory(struct item_head *ih, char *item, int dev, int blocksize)
{
	int i;
	char *name;
	int namelen, entrylen;
	struct reiserfs_de_head *deh = (struct reiserfs_de_head *)item;
	__u32 prev_offset = 0;
	__u16 prev_location = get_ih_item_len(ih);
	int min_entry_size = 1;	/* we have no way to understand whether the
				   filesystem wes created in 3.6 format or
				   converted to it. So, we assume that minimal name
				   length is 1 */

	if (get_ih_item_len(ih) / (DEH_SIZE + min_entry_size) <
	    get_ih_entry_count(ih))
		/* entry count is too big */
		return 1;

	for (i = 0; i < get_ih_entry_count(ih); i++, deh++) {
		entrylen = entry_length(ih, deh, i);
		if (entrylen > (int)REISERFS_MAX_NAME_LEN(blocksize))
			return 1;

		if (get_deh_offset(deh) <= prev_offset)
			return 1;

		prev_offset = get_deh_offset(deh);

		if (get_deh_location(deh) + entrylen != prev_location)
			return 1;

		prev_location = get_deh_location(deh);

		namelen = name_in_entry_length(ih, deh, i);
		name = name_in_entry(deh, i);
		if (!is_properly_hashed(fs, name, namelen, get_deh_offset(deh)))
			return 1;
	}
	return 0;
}

/* change incorrect block adresses by 0. Do not consider such item as incorrect */
static int is_bad_indirect(struct item_head *ih, char *item, int dev,
			   int blocksize)
{
	unsigned long blocks;
	unsigned int i;
	int bad = 0;

	if (get_ih_item_len(ih) % UNFM_P_SIZE) {
		fsck_log
		    ("is_bad_indirect: indirect item of %H of invalid length\n",
		     ih);
		return 1;
	}

	blocks = get_sb_block_count(fs->fs_ondisk_sb);

	for (i = 0; i < I_UNFM_NUM(ih); i++) {
		__le32 *ind = (__le32 *) item;

		if (d32_get(ind, i) >= blocks) {
			bad++;
			fsck_log
			    ("is_bad_indirect: %d-th pointer of item %H looks bad (%lu)\n",
			     i, ih, d32_get(ind, i));
			continue;
		}
	}
	return bad;
}

/* this is used by pass1.c:save_item and check.c:is_leaf_bad */
int is_bad_item(struct buffer_head *bh, struct item_head *ih, char *item)
{
	int blocksize, dev;

	blocksize = bh->b_size;
	dev = bh->b_dev;

	// FIXME: refuse transparently bad items
	if (get_key_dirid(&ih->ih_key) == get_key_objectid(&ih->ih_key))
		return 1;
	if (!get_key_dirid(&ih->ih_key) || !get_key_objectid(&ih->ih_key))
		return 1;

	if (is_stat_data_ih(ih))
		return is_bad_sd(ih, item);

	if (is_direntry_ih(ih))
		return is_bad_directory(ih, item, dev, blocksize);

	if (is_indirect_ih(ih))
		return is_bad_indirect(ih, item, dev, blocksize);

	if (is_direct_ih(ih))
		return 0;

	return 1;
}

int is_leaf_bad(struct buffer_head *bh)
{
	int i;
	struct item_head *ih;
	int bad = 0;

	assert(is_leaf_node(bh));

	for (i = 0, ih = item_head(bh, 0); i < B_NR_ITEMS(bh); i++, ih++) {
		if (is_bad_item(bh, ih, ih_item_body(bh, ih))) {
			fsck_log
			    ("is_leaf_bad: block %lu, item %d: The corrupted item found (%H)\n",
			     bh->b_blocknr, i, ih);
			bad = 1;
			continue;
		}

		if (i && bad_pair(fs, bh, i)) {
			fsck_log
			    ("is_leaf_bad: block %lu items %d and %d: Wrong order of items:"
			     "\n\t%H\n\t%H\n", bh->b_blocknr, i - 1, i, ih - 1,
			     ih);
			bad = 1;
		}
	}

	return bad;
}

static int is_to_be_read(reiserfs_filsys_t fs, unsigned long block)
{
	return reiserfs_bitmap_test_bit(fsck_source_bitmap(fs), block);
}

static void do_pass_0(reiserfs_filsys_t fs)
{
	struct buffer_head *bh;
	unsigned long i;
	int what_node;
	unsigned long done = 0, total;

	if (fsck_mode(fs) == DO_TEST) {
		/* just to test pass0_correct_leaf */
		bh = bread(fs->fs_dev, fsck_data(fs)->rebuild.test,
			   fs->fs_blocksize);

		if (!bh) {
			/* we were reading one block at time, and failed, so mark block bad */
			fsck_progress("%s: Reading of the block %lu failed\n",
				      __FUNCTION__,
				      fsck_data(fs)->rebuild.test);
			reiserfs_free(fs);
			exit(0);
		}

		if (is_leaf_bad(bh)) {
			fsck_progress
			    ("###############  bad #################\n");
		}

		pass0_correct_leaf(fs, bh);

		print_block(stdout, fs, bh, 3, -1, -1);

		if (is_leaf_bad(bh)) {
			fsck_progress
			    ("############### still bad #################\n");
		}
		brelse(bh);
		reiserfs_free(fs);
		exit(0);
	}

	total = reiserfs_bitmap_ones(fsck_source_bitmap(fs));

	for (i = 0; i < get_sb_block_count(fs->fs_ondisk_sb); i++) {
		if (!is_to_be_read(fs, i))
			continue;

		print_how_far(fsck_progress_file(fs), &done, total, 1,
			      fsck_quiet(fs));

		bh = bread(fs->fs_dev, i, fs->fs_blocksize);
		if (!bh) {
			/* we were reading one block at time, and failed, so mark block bad */
			fsck_progress("%s: Reading of the block %lu failed\n",
				      __FUNCTION__, i);
			continue;
		}

		if (fs->fs_badblocks_bm
		    && reiserfs_bitmap_test_bit(fs->fs_badblocks_bm, i))
			reiserfs_panic
			    ("The block (%lu), specified in badblock list, was read.",
			     i);

		if (not_data_block(fs, i)) {
			/* block which could not be pointed by indirect item */
			if (!
			    (block_of_journal(fs, i)
			     && fsck_data(fs)->rebuild.use_journal_area))
				reiserfs_panic
				    ("The block (%lu) from non data area was read.",
				     i);
		}

		pass_0_stat(fs)->dealt_with++;
		what_node = who_is_this(bh->b_data, bh->b_size);
		if (what_node != THE_LEAF && what_node != HAS_IH_ARRAY) {
			brelse(bh);
			continue;
		}

		pass_0_stat(fs)->leaves++;
		pass0_correct_leaf(fs, bh);
		brelse(bh);
	}
	fsck_progress("\n");

	/* just in case */
	id_map_mark(proper_id_map(fs), REISERFS_ROOT_OBJECTID);

}

int is_used_leaf(unsigned long block)
{
	return pass0_is_leaf(block);
}

/*
int how_many_leaves_were_there (void)
{
    return fsck_data (fs)->rebuild.leaves;
}
*/

/* these are used to correct uformatted node pointers */
int is_bad_unformatted(unsigned long block)
{
	return pass0_is_bad_unfm(block);
}

/* these are used to correct uformatted node pointers */
int is_good_unformatted(unsigned long block)
{
	return pass0_is_good_unfm(block);
}

/* this is for check only. With this we make sure that all pointers we
   put into tree on pass 1 do not point to leaves (FIXME), do not
   point to journal, bitmap, etc, do not point out of fs boundary (and
   are marked used in on-disk bitmap - this condition skipped for now). */
int still_bad_unfm_ptr_1(unsigned long block)
{
	if (!block)
		return 0;
	if (pass0_is_leaf(block))
		return 1;
	if (pass0_is_bad_unfm(block) && !is_bad_unfm_in_tree_once(block))
		return 2;
	if (not_data_block(fs, block))
		return 3;
	/*
	   if (!was_block_used (block))
	   return 4;
	 */
	if (block >= get_sb_block_count(fs->fs_ondisk_sb))
		return 5;
	return 0;

}

/* pointers to data block which get into tree are checked with this */
int still_bad_unfm_ptr_2(unsigned long block)
{
	if (!block)
		return 0;
	if (is_block_used(block))
		return 1;
	if (block >= get_sb_block_count(fs->fs_ondisk_sb))
		return 1;
	return 0;
}

/* these are used to allocate blocks for tree building */
int are_there_allocable_blocks(unsigned int amout_needed)
{
	if (reiserfs_bitmap_zeros(fsck_allocable_bitmap(fs)) < amout_needed) {
		unsigned int zeros = 0, i;

		fsck_progress
		    ("Not enough allocable blocks, checking bitmap...");
		for (i = 0; i < fsck_allocable_bitmap(fs)->bm_bit_size; i++) {
			if (!reiserfs_bitmap_test_bit
			    (fsck_allocable_bitmap(fs), i))
				zeros++;
		}
		fsck_progress("there are %u allocable blocks, btw\n", zeros);
		return 0;
	}
	return 1;
}

unsigned long alloc_block(void)
{
	unsigned long block = 0;	/* FIXME: start point could be used */

	if (reiserfs_bitmap_find_zero_bit(fsck_allocable_bitmap(fs), &block)) {
		die("alloc_block: Allocable blocks counter is wrong");
		return 0;
	}
	reiserfs_bitmap_set_bit(fsck_allocable_bitmap(fs), block);
	return block;
}

void make_allocable(unsigned long block)
{
	reiserfs_bitmap_clear_bit(fsck_allocable_bitmap(fs), block);
}

static void choose_hash_function(reiserfs_filsys_t fs)
{
	unsigned long max;
	unsigned int hash_code;
	int i;

	if (fsck_hash_defined(fs))
		return;

	max = 0;
	hash_code = func2code(NULL);

	for (i = 0; i < fsck_data(fs)->rebuild.hash_amount; i++) {
		/* remember hash whihc got more hits */
		if (fsck_data(fs)->rebuild.hash_hits[i] > max) {
			hash_code = i;
			max = fsck_data(fs)->rebuild.hash_hits[i];
		}

		if (fsck_data(fs)->rebuild.hash_hits[i])
			fsck_log
			    ("%lu directory entries were hashed with %s hash.\n",
			     fsck_data(fs)->rebuild.hash_hits[i], code2name(i));
	}

	if (max == 0 || hash_code == 0) {
		/* no names were found. take either super block value or
		   default */
		hash_code = get_sb_hash_code(fs->fs_ondisk_sb);
		if (!hash_code)
			hash_code = DEFAULT_HASH;
		fsck_log("Could not find a hash in use. Using %s\n",
			 code2name(hash_code));
	}
	/* compare the most appropriate hash with the hash set in super block */
	if (hash_code != get_sb_hash_code(fs->fs_ondisk_sb)) {
		fsck_progress
		    ("Selected hash (%s) does not match to the hash set in the super block (%s).\n",
		     code2name(hash_code),
		     code2name(get_sb_hash_code(fs->fs_ondisk_sb)));
		set_sb_hash_code(fs->fs_ondisk_sb, hash_code);
	}
	fsck_progress("\t%s hash is selected\n", code2name(hash_code));

	reiserfs_hash(fs) = code2func(hash_code);
}

/* create bitmap of blocks the tree is to be built off */
/* debugreiserfs and pass0 should share this code -s should show
the same as we could recover - test: zero first 32M */
static void init_source_bitmap(reiserfs_filsys_t fs)
{
	FILE *fp;
	unsigned long block_count = get_sb_block_count(fs->fs_ondisk_sb);
	unsigned long i, j;
	unsigned long tmp;
	unsigned long block, reserved, bits_amount;

	switch (fsck_data(fs)->rebuild.scan_area) {
	case ALL_BLOCKS:
		fsck_source_bitmap(fs) = reiserfs_create_bitmap(block_count);
		reiserfs_bitmap_fill(fsck_source_bitmap(fs));
		fsck_progress
		    ("The whole partition (%u blocks) is to be scanned\n",
		     reiserfs_bitmap_ones(fsck_source_bitmap(fs)));
		break;

	case USED_BLOCKS:
		fsck_progress("Loading on-disk bitmap .. ");
		fsck_source_bitmap(fs) = reiserfs_create_bitmap(block_count);
		reiserfs_bitmap_copy(fsck_source_bitmap(fs), fs->fs_bitmap2);

		fsck_progress("ok, %u blocks marked used\n",
			      reiserfs_bitmap_ones(fsck_source_bitmap(fs)));
		break;

	case EXTERN_BITMAP:
		fp = fopen(fsck_data(fs)->rebuild.bitmap_file_name, "r");

		if (!fp) {
			reiserfs_exit(EXIT_OPER, "Could not load bitmap: %s\n",
				      strerror(errno));
		}

		fsck_source_bitmap(fs) = reiserfs_bitmap_load(fp);

		if (!fsck_source_bitmap(fs)) {
			reiserfs_exit(EXIT_OPER,
				      "Could not load fitmap from \"%s\"",
				      fsck_data(fs)->rebuild.bitmap_file_name);
		}

		fsck_progress("%u blocks marked used in extern bitmap\n",
			      reiserfs_bitmap_ones(fsck_source_bitmap(fs)));
		fclose(fp);
		break;

	default:
		reiserfs_panic("No area to scan specified");
	}

	tmp = 0;

	/* unmark bitmaps */
	block = fs->fs_super_bh->b_blocknr + 1;

	reserved = fsck_source_bitmap(fs)->bm_bit_size;
	for (i = 0; i < reiserfs_fs_bmap_nr(fs); i++) {
		if (!reiserfs_bitmap_test_bit(fsck_source_bitmap(fs), block)) {
			/* bitmap is definitely broken, mark all blocks of this bitmap block as used */

			bits_amount =
			    (reserved <
			     fs->fs_blocksize *
			     8) ? reserved : fs->fs_blocksize * 8;
			fsck_log
			    ("%s: Bitmap %lu (of %lu bits) is wrong - mark all blocks [%lu - %lu] as used\n",
			     __FUNCTION__, i, bits_amount,
			     i * fs->fs_blocksize * 8,
			     fs->fs_blocksize * 8 * i + bits_amount);

			for (j = i * fs->fs_blocksize * 8;
			     j < i * fs->fs_blocksize * 8 + bits_amount; j++) {
				if (!reiserfs_bitmap_test_bit
				    (fsck_source_bitmap(fs), j))
					reiserfs_bitmap_set_bit
					    (fsck_source_bitmap(fs), j);
			}
		}
		reiserfs_bitmap_clear_bit(fsck_source_bitmap(fs), block);

		reserved -= fs->fs_blocksize * 8;
		tmp++;
		/* next block fo bitmap */
		if (spread_bitmaps(fs))
			block =
			    (block / (fs->fs_blocksize * 8) +
			     1) * (fs->fs_blocksize * 8);
		else
			block++;
	}

	/* pass 0 will skip super block and journal areas and bitmap blocks, find
	   how many blocks have to be read */
	for (i = 0; i <= fs->fs_super_bh->b_blocknr; i++) {
		if (!reiserfs_bitmap_test_bit(fsck_source_bitmap(fs), i))
			continue;
		reiserfs_bitmap_clear_bit(fsck_source_bitmap(fs), i);
		tmp++;
	}

	/* unmark journal area as used if journal is standard or it is non standard
	   and initialy has been created on a main device */

	reserved = get_size_of_journal_or_reserved_area(fs->fs_ondisk_sb);
	/* where does journal area (or reserved journal area) start from */

	if (!is_new_sb_location(fs->fs_super_bh->b_blocknr, fs->fs_blocksize) &&
	    !is_old_sb_location(fs->fs_super_bh->b_blocknr, fs->fs_blocksize))
		die("init_source_bitmap: Wrong super block location, you must run --rebuild-sb.");

	block = get_journal_start_must(fs);

	for (i = block; i < reserved + block; i++) {
		if (!reiserfs_bitmap_test_bit(fsck_source_bitmap(fs), i))
			continue;
		reiserfs_bitmap_clear_bit(fsck_source_bitmap(fs), i);
		tmp++;
	}

	if (fs->fs_badblocks_bm)
		for (i = 0; i < get_sb_block_count(fs->fs_ondisk_sb); i++) {
			if (reiserfs_bitmap_test_bit(fs->fs_badblocks_bm, i))
				reiserfs_bitmap_clear_bit(fsck_source_bitmap
							  (fs), i);
		}

	fsck_source_bitmap(fs)->bm_set_bits =
	    reiserfs_bitmap_ones(fsck_source_bitmap(fs));

	fsck_progress("Skipping %u blocks (super block, journal, "
		      "bitmaps) %u blocks will be read\n", tmp,
		      fsck_source_bitmap(fs)->bm_set_bits);

}

static void before_pass_0(reiserfs_filsys_t fs)
{
	/* bitmap of blocks to be read */
	init_source_bitmap(fs);

	/* bitmap of leaves, good and bad unformatted */
	make_aux_bitmaps(fs);

	/* on pass0 all objectids will be marked here as used */
	proper_id_map(fs) = id_map_init();

	/* pass0 gathers statistics about hash hits */
	hash_hits_init(fs);
}

static void save_pass_0_result(reiserfs_filsys_t fs)
{
	FILE *file;
	int retval;

	/* save bitmaps with which we will be able start reiserfs from
	   pass 1 */
	file = open_file("temp_fsck_file.deleteme", "w+");
	if (!file)
		return;

	reiserfs_begin_stage_info_save(file, PASS_0_DONE);
	reiserfs_bitmap_save(file, leaves_bitmap);
	reiserfs_bitmap_save(file, good_unfm_bitmap);
	reiserfs_bitmap_save(file, bad_unfm_bitmap);
	reiserfs_end_stage_info_save(file);
	close_file(file);
	retval = rename("temp_fsck_file.deleteme", state_dump_file(fs));
	if (retval != 0)
		fsck_progress
		    ("%s: Could not rename the temporary file temp_fsck_file.deleteme to %s",
		     __FUNCTION__, state_dump_file(fs));
}

/* file 'fp' must contain 3 bitmaps saved during last pass 0: bitmap
   of leaves, bitmaps of good and bad unfms*/
void load_pass_0_result(FILE * fp, reiserfs_filsys_t fs)
{
	leaves_bitmap = reiserfs_bitmap_load(fp);
	good_unfm_bitmap = reiserfs_bitmap_load(fp);
	bad_unfm_bitmap = reiserfs_bitmap_load(fp);
	if (!leaves_bitmap || !good_unfm_bitmap || !bad_unfm_bitmap)
		fsck_exit("State dump file seems corrupted. Run without -d");

	fsck_source_bitmap(fs) = leaves_bitmap;

	/* on pass 1 we do not need proper objectid map */

	fsck_progress
	    ("Pass 0 result loaded. %d leaves, %d/%d good/bad data blocks\n",
	     reiserfs_bitmap_ones(leaves_bitmap),
	     reiserfs_bitmap_ones(good_unfm_bitmap),
	     reiserfs_bitmap_ones(bad_unfm_bitmap));
}

static void after_pass_0(reiserfs_filsys_t fs)
{
	time_t t;

	/* update super block: hash, objectid map, fsck state */
	choose_hash_function(fs);
	id_map_flush(proper_id_map(fs), fs);
	set_sb_fs_state(fs->fs_ondisk_sb, PASS_0_DONE);
	mark_buffer_dirty(fs->fs_super_bh);

	/* write all dirty blocks */
	fsck_progress("Flushing..");
	fs->fs_dirt = 1;
	reiserfs_flush(fs);
	fsck_progress("finished\n");

	stage_report(0, fs);

	/* free what we do not need anymore */
	reiserfs_delete_bitmap(fsck_source_bitmap(fs));
	fsck_source_bitmap(fs) = NULL;

	if (!fsck_run_one_step(fs)) {
		if (fsck_user_confirmed(fs, "Continue? (Yes):", "Yes\n", 1)) {
			/* reiserfsck continues */
			fsck_source_bitmap(fs) = leaves_bitmap;
			return;
		}
	} else
		save_pass_0_result(fs);

	id_map_free(proper_id_map(fs));
	proper_id_map(fs) = NULL;

	time(&t);
	fsck_progress("###########\n"
		      "reiserfsck finished pass 0 at %s"
		      "###########\n", ctime(&t));
	fs->fs_dirt = 1;
	reiserfs_close(fs);
	exit(0);
}

void pass_0(reiserfs_filsys_t fs)
{
	if (get_reiserfs_format(fs->fs_ondisk_sb) != fs->fs_format ||
	    get_reiserfs_format(fs->fs_ondisk_sb) == REISERFS_FORMAT_UNKNOWN) {
		reiserfs_exit(EXIT_OPER,
			      "pass 0: ReiserFS format version mismatch "
			      "found, you should run --rebuild-sb");
	}

	fsck_progress("\nPass 0:\n");
	if (fsck_log_file(fs) != stderr)
		/* this is just to separate warnings in the log file */
		fsck_log("####### Pass 0 #######\n");

	before_pass_0(fs);

	/* scan the partition, find leaves and correct them */
	do_pass_0(fs);

	after_pass_0(fs);
}
