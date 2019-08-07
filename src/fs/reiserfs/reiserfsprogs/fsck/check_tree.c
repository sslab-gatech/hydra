/*
 * Copyright 1999-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

#if 0
struct check_relocated {
	__u32 old_dir_id;
	__u32 old_objectid;
	/*mode_t mode; */
	struct check_relocated *next;
};

static struct check_relocated *relocated_list;

void to_be_relocated(struct reiserfs_key *key)
{
	struct check_relocated *cur, *prev, *new_relocated;

	cur = relocated_list;
	prev = 0;

	while (cur && comp_short_keys(key, (struct reiserfs_key *)cur) != 1) {
		if (comp_short_keys(key, (struct reiserfs_key *)cur) == 0)
			return;
		prev = cur;
		cur = cur->next;
	}

	new_relocated = getmem(sizeof(struct check_relocated));
	copy_short_key((struct reiserfs_key *)new_relocated, key);

	if (prev) {
		new_relocated->next = prev->next;
		prev->next = new_relocated;
	} else {
		new_relocated->next = relocated_list;
		relocated_list = new_relocated;
	}
}

int should_be_relocated(struct reiserfs_key *key)
{
	struct check_relocated *cur, *prev;
	int ret = 0;

	if (!key)
		return 0;

	cur = relocated_list;

	prev = NULL;
	while (cur && comp_short_keys(key, (struct reiserfs_key *)cur) != 1) {
		if (comp_short_keys(key, (struct reiserfs_key *)cur) == 0) {
			ret = 1;
			break;
		}
		prev = cur;
		cur = cur->next;
	}

	if (ret) {
		/* cur is found */
		if (prev)	/* not the first */
			prev->next = cur->next;
		else		/* first */
			relocated_list = cur->next;
		freemem(cur);
	}

	return ret;
}

void clear_relocated_list()
{
	struct check_relocated *next;

	while (relocated_list) {
		next = relocated_list->next;
		freemem(relocated_list);
		relocated_list = next;
	}
}

#endif

//
//
//
// check_fs_tree stops and recommends to run fsck --rebuild-tree when:
// 1. read fails
// 2. node of wrong level found in the tree
// 3. something in the tree points to wrong block number
//      out of filesystem boundary is pointed by tree
//      to block marked as free in bitmap
//      the same block is pointed from more than one place
//      not data blocks (journal area, super block, bitmaps)
// 4. bad formatted node found
// 5. delimiting keys are incorrect
//

/* mark every block we see in the tree in control bitmap, so, when to make
   sure, that no blocks are pointed to from more than one place we use
   additional bitmap (control_bitmap). If we see pointer to a block we set
   corresponding bit to 1. If it is set already - run fsck with --rebuild-tree */
static reiserfs_bitmap_t *control_bitmap;
static reiserfs_bitmap_t *source_bitmap;

static int tree_scanning_failed = 0;

/* 1 if block is not marked as used in the bitmap */
static int is_block_free(reiserfs_filsys_t fs, unsigned long block)
{
	return !reiserfs_bitmap_test_bit(source_bitmap, block);
}

/*static unsigned int hits = 0;*/

/* we have seen this block in the tree, mark corresponding bit in the
   control bitmap */
static void we_met_it(unsigned long block)
{
	reiserfs_bitmap_set_bit(control_bitmap, block);
	/*hits ++; */
}

/* have we seen this block somewhere in the tree before? */
static int did_we_meet_it(unsigned long block)
{
	return reiserfs_bitmap_test_bit(control_bitmap, block);
}

static void init_control_bitmap(reiserfs_filsys_t fs)
{
	unsigned int i;
	unsigned long block;
	unsigned long reserved;
	unsigned int blocks = get_sb_block_count(fs->fs_ondisk_sb);

	control_bitmap = reiserfs_create_bitmap(blocks);
	if (!control_bitmap)
		die("init_control_bitmap: Failed to allocate a control bitmap.");

	/*printf ("Initially number of zeros in control bitmap %d\n",
	   reiserfs_bitmap_zeros (control_bitmap)); */

	/* skipped and super block */
	for (i = 0; i <= fs->fs_super_bh->b_blocknr; i++)
		we_met_it(i);

	/*printf ("SKIPPED: %u blocks marked used (%d)\n", hits,
	   reiserfs_bitmap_zeros (control_bitmap));
	   hits = 0; */

	/* bitmaps */
	block = fs->fs_super_bh->b_blocknr + 1;
	for (i = 0; i < reiserfs_fs_bmap_nr(fs); i++) {
		we_met_it(block);

		if (spread_bitmaps(fs))
			block = (block / (fs->fs_blocksize * 8) + 1) *
			    (fs->fs_blocksize * 8);
		else
			block++;
	}

	/*printf ("BITMAPS: %u blocks marked used (%d)\n", hits,
	   reiserfs_bitmap_zeros (control_bitmap));

	   hits = 0; */

	/* mark as used area of the main device either containing a journal or
	   reserved to hold it */

	reserved = get_size_of_journal_or_reserved_area(fs->fs_ondisk_sb);

	/* where does journal area (or reserved journal area) start from */

	if (!is_new_sb_location(fs->fs_super_bh->b_blocknr, fs->fs_blocksize) &&
	    !is_old_sb_location(fs->fs_super_bh->b_blocknr, fs->fs_blocksize))
		die("init_control_bitmap: Wrong super block location. You must run " "--rebuild-sb.");

	block = get_journal_start_must(fs);

	for (i = block; i < reserved + block; i++)
		we_met_it(i);

	if (fs->fs_badblocks_bm)
		for (i = 0; i < get_sb_block_count(fs->fs_ondisk_sb); i++) {
			if (reiserfs_bitmap_test_bit(fs->fs_badblocks_bm, i))
				we_met_it(i);
		}
}

/* if we managed to complete tree scanning and if control bitmap and/or proper
   amount of free blocks mismatch with bitmap on disk and super block's
   s_free_blocks - we can fix that */
static void handle_bitmaps(reiserfs_filsys_t fs)
{
	int problem = 0;

	if (tree_scanning_failed) {
		fsck_progress
		    ("Could not scan the internal tree. --rebuild-tree "
		     "is required\n");
		return;
	}

	fsck_progress("Comparing bitmaps..");

	/* check free block counter */
	if (get_sb_free_blocks(fs->fs_ondisk_sb) !=
	    reiserfs_bitmap_zeros(control_bitmap)) {
/*	fsck_log ("vpf-10630: The count of free blocks in the on-disk bitmap "
		  "(%lu) mismatches with the correct one (%lu).\n",
		  get_sb_free_blocks (fs->fs_ondisk_sb),
		  reiserfs_bitmap_zeros (control_bitmap));
*/
		problem++;
	}

	if (reiserfs_bitmap_compare(source_bitmap, control_bitmap))
		problem++;

	if (problem) {
		if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
			fsck_log
			    ("vpf-10630: The on-disk and the correct bitmaps differs. "
			     "Will be fixed later.\n");
//            fsck_progress ("Trying to fix bitmap ..\n");
			/* mark blocks as used in source bitmap if they are used in
			   control bitmap */
			reiserfs_bitmap_disjunction(source_bitmap,
						    control_bitmap);
			/* change used blocks count accordinly source bitmap, copy bitmap
			   changes to on_disk bitmap */
			set_sb_free_blocks(fs->fs_ondisk_sb,
					   reiserfs_bitmap_zeros
					   (source_bitmap));
			reiserfs_bitmap_copy(fs->fs_bitmap2, source_bitmap);
			mark_buffer_dirty(fs->fs_super_bh);
/*
            // check again
            if ((diff = reiserfs_bitmap_compare(source_bitmap,
						control_bitmap)) != 0)
	    {
                // do not mark them as fatal or fixable because one can live
		// with leaked space. So this is not a fatal corruption, and
		// fix-fixable cannot fix it
                fsck_progress (" bitmaps were not recovered.\n\tYou can either "
		"run rebuild-tree or live with %d leaked blocks\n", diff);
            } else {
                fsck_progress ("finished\n");
            }
*/
		} else if (problem) {
			fsck_log
			    ("vpf-10640: The on-disk and the correct bitmaps differs.\n");
			while (problem) {
				/* fixable corruptions because we can try to recover them
				   without rebuilding the tree */
				one_more_corruption(fs, FIXABLE);
				problem--;
			}
		}
	} else
		fsck_progress("finished\n");

	return;
}

static int auto_handle_bitmaps(reiserfs_filsys_t fs)
{
	unsigned long i;

	if (source_bitmap->bm_byte_size != control_bitmap->bm_byte_size)
		return -1;

	for (i = 0; i < source_bitmap->bm_byte_size; i++) {
		if (control_bitmap->bm_map[i] & ~source_bitmap->bm_map[i]) {
			return 1;
		}
	}

	return 0;
}

/* is this block legal to be pointed to by some place of the tree? */
static int bad_block_number(reiserfs_filsys_t fs, unsigned long block)
{
	if (block >= get_sb_block_count(fs->fs_ondisk_sb) ||
	    not_data_block(fs, block)) {
		/* block has value which can not be used as a pointer in a tree */

		return 1;
/*
	if (is_unfm_pointer) {
	    // unformatted node pointer will be zeroed
	    one_more_corruption (fs, fixable);
	    return 1;
	}

	// internal nodes can not have wrong pointer
	one_more_corruption (fs, fatal);
	return 1;
*/

	}

	if (is_block_free(fs, block)) {
		/* block is marked free - bitmap problems will be handled later */
		//one_more_corruption (fs, fixable);
	}

	return 0;
}

static int got_already(reiserfs_filsys_t fs, unsigned long block)
{
	if (did_we_meet_it(block)) {
		/* block is in tree at least twice */
		return 1;
	}
	we_met_it(block);
	return 0;
}

/* 1 if it does not look like reasonable stat data */
static int bad_stat_data(reiserfs_filsys_t fs,
			 struct buffer_head *bh, struct item_head *ih)
{
	unsigned long objectid;
//    __u32 links;

	objectid = get_key_objectid(&ih->ih_key);
	if (!is_objectid_used(fs, objectid)) {
		/* FIXME: this could be cured right here */
		fsck_log
		    ("bad_stat_data: The objectid (%lu) is marked free, but used "
		     "by an object %k\n", objectid, &ih->ih_key);

		/* if it is FIX_FIXABLE we flush objectid map at the end
		   no way to call one_less_corruption later
		 */
		if (fsck_mode(fs) != FSCK_FIX_FIXABLE)
			one_more_corruption(fs, FIXABLE);
	}

	if (id_map_mark(proper_id_map(fs), objectid)) {
		fsck_log
		    ("bad_stat_data: The objectid (%lu) is shared by at least two "
		     "files. Can be fixed with --rebuild-tree only.\n",
		     objectid);
#if 0
		to_be_relocated(&ih->ih_key);
//      one_more_corruption (fs, FIXABLE);
#endif
	}

	return 0;
/* Check this on semantic_check pass.

    sd = (struct stat_data *)ih_item_body(bh,ih);
    get_sd_nlink (ih, sd, &links);
    if (S_ISDIR(sd->sd_mode)) {
        if (links < 2) {
            fsck_log ("%s: block %lu: The directory StatData %k has bad nlink number (%u)\n",
                __FUNCTION__, bh->b_blocknr, &ih->ih_key, links);
            one_more_corruption (fs, fatal);
        }
    } else {
        if (links == 0) {
            fsck_log ("%s: block %lu: The StatData %k has bad nlink number (%u)\n",
                __FUNCTION__, bh->b_blocknr, &ih->ih_key);
            one_more_corruption (fs, fatal);
        }
    }
*/
}

/* it looks like we can check item length only */
static int bad_direct_item(reiserfs_filsys_t fs,
			   struct buffer_head *bh, struct item_head *ih)
{
	return 0;
}

static inline void handle_one_pointer(reiserfs_filsys_t fs,
			       struct buffer_head *bh, __le32 * item,
			       int offset)
{
	if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
		fsck_log(" - zeroed");
		d32_put(item, offset, 0);
		mark_buffer_dirty(bh);
	} else {
		one_more_corruption(fs, FIXABLE);
	}
}

static int bad_badblocks_item(reiserfs_filsys_t fs, struct buffer_head *bh,
			      struct item_head *ih)
{
	__u32 i;
	__le32 *ind = (__le32 *) ih_item_body(bh, ih);

	if (get_ih_item_len(ih) % 4) {
		fsck_log("%s: block %lu: item (%H) has bad length\n",
			 __FUNCTION__, bh->b_blocknr, ih);
		one_more_corruption(fs, FATAL);
		return 1;
	}

	/* All valid badblocks are given in badblock bitmap. Nothing to check anymore. */
	if (fs->fs_badblocks_bm)
		return 0;

	for (i = 0; i < I_UNFM_NUM(ih); i++) {
		if (!d32_get(ind, i)) {
/*	    fsck_log ("%s: block %lu: badblocks item (%H) has zero pointer.",
		      __FUNCTION__, bh->b_blocknr, ih);
	
	    if (fsck_mode(fs) != FSCK_FIX_FIXABLE) {
		    fsck_log("Not an error, but could be deleted with --fix-fixable\n");
	    } else {
		    fsck_log("Will be deleted later.\n");
	    }*/

			continue;
		}

		/* check list of badblocks pointers */
		if (d32_get(ind, i) >= get_sb_block_count(fs->fs_ondisk_sb)) {
			fsck_log
			    ("%s: badblock pointer (block %lu) points out of disk spase (%lu)",
			     __FUNCTION__, bh->b_blocknr, d32_get(ind, i));
			handle_one_pointer(fs, bh, ind, i);
			fsck_log("\n");
		}

		if (did_we_meet_it(d32_get(ind, i))) {
			/* it can be
			   1. not_data_block
			   delete pointer
			   2. ind [i] or internal/leaf
			   advice to run fix-fixable if there is no fatal errors
			   with list of badblocks, say that it could fix it. */

			if (not_data_block(fs, d32_get(ind, i))) {
				fsck_log
				    ("%s: badblock pointer (block %lu) points on fs metadata (%lu)",
				     __FUNCTION__, bh->b_blocknr, d32_get(ind,
									  i));
				handle_one_pointer(fs, bh, ind, i);
				fsck_log("\n");
			} else {
				one_more_corruption(fs, FIXABLE);
				fsck_log
				    ("%s: badblock pointer (block %lu) points to a block (%lu) "
				     "which is in the tree already. Use badblock option (-B) to"
				     " fix the problem\n", __FUNCTION__,
				     bh->b_blocknr, d32_get(ind, i));
			}

			continue;
		}

		we_met_it(d32_get(ind, i));
	}

	return 0;
}

/* for each unformatted node pointer: make sure it points to data area and
   that it is not in the tree yet */
static int bad_indirect_item(reiserfs_filsys_t fs, struct buffer_head *bh,
			     struct item_head *ih)
{
	__le32 *ind = (__le32 *) ih_item_body(bh, ih);
	unsigned int i;

	if (get_ih_item_len(ih) % 4) {
		fsck_log
		    ("%s: block %lu: The item (%H) has the bad length (%u)\n",
		     __FUNCTION__, bh->b_blocknr, ih, get_ih_item_len(ih));
		one_more_corruption(fs, FATAL);
		return 1;
	}

	for (i = 0; i < I_UNFM_NUM(ih); i++) {

		fsck_check_stat(fs)->unfm_pointers++;
		if (!d32_get(ind, i)) {
			fsck_check_stat(fs)->zero_unfm_pointers++;
			continue;
		}

		/* check unformatted node pointer and mark it used in the
		   control bitmap */
		if (bad_block_number(fs, d32_get(ind, i))) {
			fsck_log
			    ("%s: block %lu: The item %k has the bad pointer (%d) to "
			     "the block (%lu)", __FUNCTION__, bh->b_blocknr,
			     &ih->ih_key, i, d32_get(ind, i));

			handle_one_pointer(fs, bh, ind, i);
			fsck_log("\n");
			continue;
		}

		if (got_already(fs, d32_get(ind, i))) {
			fsck_log
			    ("%s: block %lu: The item (%H) has the bad pointer (%d) "
			     "to the block (%lu), which is in tree already",
			     __FUNCTION__, bh->b_blocknr, ih, i, d32_get(ind,
									 i));

			handle_one_pointer(fs, bh, ind, i);
			fsck_log("\n");
			continue;
		}
	}

#if 0
	/* delete this check for 3.6 */
	if (get_ih_free_space(ih) > fs->fs_blocksize - 1) {
		if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
			/*FIXME: fix it if needed */
		} else {
			fsck_log
			    ("bad_indirect_item: %H has wrong ih_free_space\n",
			     ih);
			one_more_corruption(fs, fixable);
		}
	}
#endif

	return 0;
}

/* FIXME: this was is_bad_directory from pass0.c */
static int bad_directory_item(reiserfs_filsys_t fs,
			      struct buffer_head *bh, struct item_head *ih)
{
	char *name, *prev_name;
	__u32 off, prev_off;
	unsigned int count, i;
	struct reiserfs_de_head *deh = B_I_DEH(bh, ih);
	int min_entry_size = 1;	/* We have no way to understand whether the
				   filesystem was created in 3.6 format or
				   converted to it. So, we assume that minimal name
				   length is 1 */
	__u16 state;
	int namelen;

	count = get_ih_entry_count(ih);

	if (count == 0) {
		one_more_corruption(fs, FATAL);
		return 1;
	}

	/* make sure item looks like a directory */
	if (get_ih_item_len(ih) / (DEH_SIZE + min_entry_size) < count) {
		/* entry count can not be that big */
		fsck_log
		    ("%s: block %lu: The directory item %k has the exsessively "
		     "big entry count (%u)\n", __FUNCTION__, bh->b_blocknr,
		     &ih->ih_key, count);

		one_more_corruption(fs, FATAL);
		return 1;
	}

	if (get_deh_location(&deh[count - 1]) != DEH_SIZE * count) {
		/* last entry should start right after array of dir entry headers */
		fsck_log
		    ("%s: block %lu: The directory item %k has the corrupted "
		     "entry structure\n", __FUNCTION__, bh->b_blocknr,
		     &ih->ih_key);

		one_more_corruption(fs, FATAL);
		return 1;
	}

	/* check name hashing */
	prev_name = ih_item_body(bh, ih) + get_ih_item_len(ih);
	prev_off = 0;

	for (i = 0; i < count; i++, deh++) {
		namelen = name_in_entry_length(ih, deh, i);
		name = name_in_entry(deh, i);
		off = get_deh_offset(deh);

		if (namelen > (int)REISERFS_MAX_NAME_LEN(fs->fs_blocksize) ||
		    name >= prev_name || off <= prev_off) {
			fsck_log
			    ("%s: block %lu: The directory item %k has a broken entry "
			     "(%d)\n", __FUNCTION__, bh->b_blocknr, &ih->ih_key,
			     i);
			one_more_corruption(fs, FATAL);
			return 1;
		}

		if (!is_properly_hashed(fs, name, namelen, off)) {
			fsck_log
			    ("%s: block %lu: The directory item %k has a not properly "
			     "hashed entry (%d)\n", __FUNCTION__, bh->b_blocknr,
			     &ih->ih_key, i);

			one_more_corruption(fs, FATAL);
			return 1;
		}

		prev_name = name;
		prev_off = off;
	}

	deh = B_I_DEH(bh, ih);
	state = (1 << DEH_Visible2);
	/* ok, items looks like a directory */
	for (i = 0; i < count; i++, deh++) {
		if (get_deh_state(deh) != state) {
			fsck_log
			    ("bad_directory_item: block %lu: The directory item %k "
			     "has the entry (%d) \"%.*s\" with a not legal state "
			     "(%o), (%o) expected", bh->b_blocknr, &ih->ih_key,
			     i, name_in_entry_length(ih, deh, i),
			     name_in_entry(deh, i), get_deh_state(deh), state);

			if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
				set_deh_state(deh, 1 << DEH_Visible2);
				mark_buffer_dirty(bh);
				fsck_log(" - corrected\n");
			} else
				one_more_corruption(fs, FIXABLE);

			fsck_log("\n");
		}
	}

	return 0;
}

static int bad_item(reiserfs_filsys_t fs, struct buffer_head *bh, int num)
{
	struct item_head *ih;
	int format;

	ih = item_head(bh, num);

	if ((get_ih_flags(ih)) != 0) {
		if (fsck_mode(fs) != FSCK_FIX_FIXABLE) {
			one_more_corruption(fs, FIXABLE);
			fsck_log
			    ("%s: vpf-10570: block %lu: The item header (%d) has not "
			     "cleaned flags.\n", __FUNCTION__, bh->b_blocknr,
			     num);
		} else {
			fsck_log
			    ("%s: vpf-10580: block %lu: Flags in the item header "
			     "(%d) were cleaned\n", __FUNCTION__, bh->b_blocknr,
			     num);

			clean_ih_flags(ih);
			mark_buffer_dirty(bh);
		}
	}

	if (is_stat_data_ih(ih) && get_ih_item_len(ih) == SD_SIZE)
		format = KEY_FORMAT_2;
	else if (is_stat_data_ih(ih) && get_ih_item_len(ih) == SD_V1_SIZE)
		format = KEY_FORMAT_1;
	else
		format = key_format(&ih->ih_key);

	if (format != get_ih_key_format(ih)) {
		if (fsck_mode(fs) != FSCK_FIX_FIXABLE) {
			one_more_corruption(fs, FIXABLE);
			fsck_log
			    ("%s: vpf-10710: block %lu: The format (%d) specified "
			     "in the item header (%d) differs from the key format "
			     "(%d).\n", __FUNCTION__, bh->b_blocknr,
			     get_ih_key_format(ih), num, format);
		} else {
			fsck_log
			    ("%s: vpf-10720: block %lu: The format (%d) specified in "
			     "the item header (%d) was fixed to the key format (%d).\n",
			     __FUNCTION__, bh->b_blocknr, get_ih_key_format(ih),
			     num, format);

			set_ih_key_format(ih, format);
			mark_buffer_dirty(bh);
		}
	}

	if (get_key_objectid(&ih->ih_key) == BADBLOCK_OBJID) {
		if (get_key_dirid(&ih->ih_key) == BADBLOCK_DIRID
		    && is_indirect_ih(ih)) {
			/* Bad Block support. */
			return bad_badblocks_item(fs, bh, ih);
		}

		goto error;
	} else {
		if (get_key_dirid(&ih->ih_key) == (__u32) - 1) {
			/*  Safe Link support. Allowable safe links are:
			   -1 object_id           0x1       INDIRECT (truncate) or
			   -1 object_id   blocksize+1       DIRECT   (unlink) */
			if (is_direct_ih(ih)
			    && get_offset(&ih->ih_key) ==
			    fs->fs_blocksize + 1) {
				if (get_ih_item_len(ih) == 4) {
					/*fsck_log("vpf-00010: safe link found %k\n", &ih->ih_key); */
					fsck_check_stat(fs)->safe++;
					return 0;
				}
			}

			if (is_indirect_ih(ih)
			    && get_offset(&ih->ih_key) == 0x1) {
				if (get_ih_item_len(ih) == 4) {
					/*fsck_log("vpf-00020: safe link found %k\n", &ih->ih_key); */
					fsck_check_stat(fs)->safe++;
					return 0;
				}
			}

			/* it does not look like safe link */
			goto error;
		}
	}

	if (is_stat_data_ih(ih))
		return bad_stat_data(fs, bh, ih);

	if (is_direct_ih(ih))
		return bad_direct_item(fs, bh, ih);

	if (is_indirect_ih(ih))
		return bad_indirect_item(fs, bh, ih);

	return bad_directory_item(fs, bh, ih);

error:
	one_more_corruption(fs, FATAL);
	fsck_log("%s: vpf-10310: block %lu, item %d: The item has a wrong "
		 "key %k\n", __FUNCTION__, num, bh->b_blocknr, &ih->ih_key);
	return 1;
}

/* 1 if i-th and (i-1)-th items can not be neighbors in a leaf */
int bad_pair(reiserfs_filsys_t fs, struct buffer_head *bh, int pos)
{
	struct item_head *ih;

	ih = item_head(bh, pos);

	if (comp_keys(&((ih - 1)->ih_key), &ih->ih_key) != -1) {
		if (fsck_mode(fs) != FSCK_REBUILD)
			one_more_corruption(fs, FATAL);
		return 1;
	}

	if (is_stat_data_ih(ih))
		/* left item must be of another object */
		if (comp_short_keys(&((ih - 1)->ih_key), &ih->ih_key) != -1) {
			if (fsck_mode(fs) != FSCK_REBUILD)
				one_more_corruption(fs, FATAL);
			return 1;
		}

	if (get_key_objectid(&ih->ih_key) == BADBLOCK_OBJID) {
		/* BAD BLOCK LIST SUPPORT. */
		if (get_key_dirid(&ih->ih_key) == BADBLOCK_DIRID &&
		    is_indirect_ih(ih) && comp_short_keys(&((ih - 1)->ih_key),
							  &ih->ih_key))
			return 0;
	} else {
		/* Safe link support. */
		if (get_key_dirid(&ih->ih_key) == (__u32) - 1) {
			if (comp_short_keys(&((ih - 1)->ih_key), &ih->ih_key) ==
			    0) {
				if (is_indirect_ih(ih - 1) && is_direct_ih(ih))
					return 0;	/* safe link */
				/* they do not look like safe links */
			} else {
				if (is_indirect_ih(ih) || is_direct_ih(ih))
					return 0;	/* safe link */
				/* it does not look like safe link */
			}
		}
	}

	if (is_direct_ih(ih)) {
		/* left item must be indirect or stat data item of the same file */
		if (not_of_one_file(&((ih - 1)->ih_key), &ih->ih_key)) {
			if (fsck_mode(fs) != FSCK_REBUILD)
				one_more_corruption(fs, FATAL);

			return 1;
		}

		if (!
		    ((is_stat_data_ih(ih - 1) && get_offset(&ih->ih_key) == 1)
		     || (is_indirect_ih(ih - 1)
			 && get_offset(&(ih - 1)->ih_key) +
			 get_bytes_number(ih - 1,
					  bh->b_size) ==
			 get_offset(&ih->ih_key)))) {
			if (fsck_mode(fs) != FSCK_REBUILD)
				one_more_corruption(fs, FATAL);

			return 1;
		}
	}

	if (is_indirect_ih(ih) || is_direntry_ih(ih)) {
		/* left item must be stat data of the same object */
		if (not_of_one_file(&((ih - 1)->ih_key), &ih->ih_key) ||
		    !is_stat_data_ih(ih - 1)) {
			if (fsck_mode(fs) != FSCK_REBUILD)
				one_more_corruption(fs, FATAL);
			return 1;
		}
	}

	return 0;
}

/* 1 if block head or any of items is bad */
static int bad_leaf(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	int i;

	if (leaf_structure_check(fs, bh))
		return 1;

	for (i = 0; i < B_NR_ITEMS(bh); i++) {
		if (bad_item(fs, bh, i)) {
			fsck_log
			    ("bad_leaf: block %lu, item %d: The corrupted item found "
			     "(%H)\n", bh->b_blocknr, i, item_head(bh, i));
		}

		if (i && bad_pair(fs, bh, i)) {
			fsck_log
			    ("bad_leaf: block %lu, items %d and %d: The wrong order "
			     "of items: %k, %k\n", bh->b_blocknr, i - 1, i,
			     &item_head(bh, i - 1)->ih_key,
			     &item_head(bh, i)->ih_key);
		}
	}
	return 0;
}

/* 1 if bh does not look like internal node */
static int bad_internal(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	int i;

	for (i = 0; i <= B_NR_ITEMS(bh); i++) {
		if (i != B_NR_ITEMS(bh) && i != B_NR_ITEMS(bh) - 1)
			/* make sure that keys are in increasing order */
			if (comp_keys(internal_key(bh, i),
				      internal_key(bh, i + 1)) != -1) {
				fsck_log
				    ("%s: vpf-10320: block %lu, items %d and %d: The "
				     "wrong order of items: %k, %k\n",
				     __FUNCTION__, bh->b_blocknr, i, i + 1,
				     internal_key(bh, i), internal_key(bh,
									   i +
									   1));

				one_more_corruption(fs, FATAL);
				return 1;
			}

		/* make sure that the child is correct */
		if (bad_block_number
		    (fs, get_dc_child_blocknr(B_N_CHILD(bh, i)))) {
			fsck_log
			    ("%s: vpf-10330: block %lu, item %d: The internal item "
			     "points to the not legal block (%lu)\n",
			     __FUNCTION__, bh->b_blocknr, i,
			     get_dc_child_blocknr(B_N_CHILD(bh, i)));

			one_more_corruption(fs, FATAL);
			return 1;
		}
	}
	return 0;
}

/* h == 0 for root level. block head's level == 1 for leaf level  */
static inline int h_to_level(reiserfs_filsys_t fs, int h)
{
	return get_sb_tree_height(fs->fs_ondisk_sb) - h - 1;
}

#if 0
int dc_fix(struct buffer_head *bh, int pos, __u32 block)
{
	if (!bh || !bh->b_data)
		return -1;

	if (B_NR_ITEMS(bh) < pos)
		return -1;

	set_dc_child_blocknr(B_N_CHILD(bh, pos), block);

	mark_buffer_dirty(bh);
	bwrite(bh);

	return 0;
}

/* Removes @N-th key and @(N+1) pointer. */
int internal_remove(struct buffer_head *bh, int pos)
{
	char *delete;
	__u32 nr;

	if (!bh || !bh->b_data)
		return -1;

	if (B_NR_ITEMS(bh) < pos)
		return -1;

	delete = (char *)B_N_CHILD(bh, pos + 2);
	memmove(delete - DC_SIZE, delete, bh->b_size - (delete - bh->b_data));

	delete = (char *)internal_key(bh, pos + 1);
	memmove(delete - KEY_SIZE, delete, bh->b_size - (delete - bh->b_data));

	nr = B_NR_ITEMS(bh) - 1;

	set_blkh_nr_items(B_BLK_HEAD(bh), nr);
	set_blkh_free_space(B_BLK_HEAD(bh), bh->b_size -
			    (BLKH_SIZE + KEY_SIZE * nr + DC_SIZE * (nr + 1)));

	mark_buffer_dirty(bh);
	bwrite(bh);

	return 0;
}

int leaf_fix_key_oid(struct buffer_head *bh, int pos, __u32 oid)
{
	struct item_head *ih;

	if (!bh || !bh->b_data)
		return -1;

	if (B_NR_ITEMS(bh) < pos)
		return -1;

	ih = item_head(bh, pos);
	set_key_objectid(&ih->ih_key, oid);

	mark_buffer_dirty(bh);
	bwrite(bh);

	return 0;
}
#endif

/* bh must be formatted node. blk_level must be tree_height - h + 1 */
static int bad_node(reiserfs_filsys_t fs, struct buffer_head **path, int h)
{
	struct buffer_head **pbh = &path[h];

	if (B_LEVEL(*pbh) != h_to_level(fs, h)) {
		fsck_log
		    ("block %lu: The level of the node (%d) is not correct, "
		     "(%d) expected\n", (*pbh)->b_blocknr, B_LEVEL(*pbh),
		     h_to_level(fs, h));

		one_more_corruption(fs, FATAL);
		return 1;
	}

	if (bad_block_number(fs, (*pbh)->b_blocknr)) {
		one_more_corruption(fs, FATAL);
		fsck_log
		    ("%s: vpf-10340: The node in the wrong block number (%lu) "
		     "found in the tree\n", __FUNCTION__, (*pbh)->b_blocknr);
		return 1;
	}

	if (got_already(fs, (*pbh)->b_blocknr)) {
		fsck_log
		    ("%s: vpf-10350: The block (%lu) is used more than once "
		     "in the tree.\n", __FUNCTION__, (*pbh)->b_blocknr);
		one_more_corruption(fs, FATAL);
		return 1;
	}

	if (is_leaf_node(*pbh)) {
		fsck_check_stat(fs)->leaves++;
		return bad_leaf(fs, *pbh);
	}

	fsck_check_stat(fs)->internals++;
	return bad_internal(fs, *pbh);
}

/* internal node bh must point to block */
static int get_pos(const struct buffer_head *bh, unsigned long block)
{
	int i;

	for (i = 0; i <= B_NR_ITEMS(bh); i++) {
		if (get_dc_child_blocknr(B_N_CHILD(bh, i)) == block)
			return i;
	}
	die("An internal pointer to the block (%lu) cannot be found in the node (%lu)", block, bh->b_blocknr);
	return 0;
}

/* path[h] - leaf node */
static struct reiserfs_key *lkey(struct buffer_head **path, int h)
{
	int pos;

	while (h > 0) {
		pos = get_pos(path[h - 1], path[h]->b_blocknr);
		if (pos)
			return internal_key(path[h - 1], pos - 1);
		h--;
	}
	return NULL;
}

/* path[h] - leaf node */
static struct reiserfs_key *rkey(struct buffer_head **path, int h)
{
	int pos;

	while (h > 0) {
		pos = get_pos(path[h - 1], path[h]->b_blocknr);
		if (pos != B_NR_ITEMS(path[h - 1]))
			return internal_key(path[h - 1], pos);
		h--;
	}
	return NULL;
}

/* are all delimiting keys correct */
static int bad_path(reiserfs_filsys_t fs, struct buffer_head **path, int h1)
{
	int h = 0;
	const struct reiserfs_key *dk;
	int pos = 0;

	while (path[h])
		h++;

	h--;

	// path[h] is leaf
	if (h != h1)
		die("bad_path: The leaf is expected as the last element in the path");

	if (h)
		pos = get_pos(path[h - 1], path[h]->b_blocknr);

	dk = lkey(path, h);
	if (dk && comp_keys(dk, leaf_key(path[h], 0))) {
		/* left delimiting key must be equal to the key of 0-th item in the
		   node */
		fsck_log
		    ("bad_path: The left delimiting key %k of the node (%lu) must "
		     "be equal to the first element's key %k within the node.\n",
		     dk, path[h]->b_blocknr, leaf_key(path[h], 0));
		one_more_corruption(fs, FATAL);
		return 1;
	}

	dk = rkey(path, h);
	if (dk && comp_keys(dk, leaf_key(path[h],
					 get_blkh_nr_items(B_BLK_HEAD(path[h]))
					 - 1)) != 1) {
		/* right delimiting key must be gt the key of the last item in the node */
		fsck_log
		    ("bad_path: The right delimiting key %k of the node (%lu) must "
		     "be greater than the last (%d) element's key %k within the node.\n",
		     dk, path[h]->b_blocknr,
		     get_blkh_nr_items(B_BLK_HEAD(path[h])) - 1,
		     leaf_key(path[h],
			      get_blkh_nr_items(B_BLK_HEAD(path[h])) - 1));
		one_more_corruption(fs, FATAL);
		return 1;
	}

	if (h && (get_dc_child_size(B_N_CHILD(path[h - 1], pos)) +
		  get_blkh_free_space((struct block_head *)path[h]->b_data) +
		  BLKH_SIZE != path[h]->b_size)) {
		/* wrong dc_size */
		fsck_log
		    ("bad_path: block %lu, pointer %d: The used space (%d) of the "
		     "child block (%lu)", path[h - 1]->b_blocknr, pos,
		     get_dc_child_size(B_N_CHILD(path[h - 1], pos)),
		     path[h]->b_blocknr);
		fsck_log
		    (" is not equal to the (blocksize (4096) - free space (%d) - "
		     "header size (%u))",
		     get_blkh_free_space((struct block_head *)path[h]->b_data),
		     BLKH_SIZE);

		if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
			set_dc_child_size(B_N_CHILD(path[h - 1], pos),
					  path[h]->b_size -
					  get_blkh_free_space((struct block_head
							       *)path[h]->
							      b_data)
					  - BLKH_SIZE);
			fsck_log(" - corrected to (%lu)\n",
				 get_dc_child_size(B_N_CHILD
						   (path[h - 1], pos)));

			mark_buffer_dirty(path[h - 1]);
		} else {
			one_more_corruption(fs, FIXABLE);
			fsck_log("\n");
			return 1;
		}
	}

	return 0;
}

static void before_check_fs_tree(reiserfs_filsys_t fs)
{
	init_control_bitmap(fs);

	source_bitmap =
	    reiserfs_create_bitmap(get_sb_block_count(fs->fs_ondisk_sb));
	reiserfs_bitmap_copy(source_bitmap, fs->fs_bitmap2);

	proper_id_map(fs) = id_map_init();
}

static void after_check_fs_tree(reiserfs_filsys_t fs)
{
	if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
		int ret = reiserfs_flush_to_ondisk_bitmap(fs->fs_bitmap2, fs);
		if (ret < 0)
			reiserfs_exit(1, "Exiting after unrecoverable error.");
		reiserfs_flush(fs);
		fs->fs_dirt = 1;
		reiserfs_bitmap_delta(source_bitmap, control_bitmap);
		fsck_deallocate_bitmap(fs) = source_bitmap;
	} else
		reiserfs_delete_bitmap(source_bitmap);

	reiserfs_delete_bitmap(control_bitmap);
	flush_buffers(fs->fs_dev);
}

/* pass internal tree of filesystem */
void check_fs_tree(reiserfs_filsys_t fs)
{
	before_check_fs_tree(fs);

	fsck_progress("Checking internal tree..  ");
	pass_through_tree(fs, bad_node, bad_path,
			  fsck_mode(fs) == FSCK_AUTO ? 2 : -1);
	/* internal tree is correct (including all objects have correct
	   sequences of items) */
	fsck_progress("finished\n");

	/* compare created bitmap with the original */
	if (fsck_mode(fs) == FSCK_AUTO) {
		if (auto_handle_bitmaps(fs)) {
			fprintf(stderr, "The on-disk bitmap looks corrupted.");
			one_more_corruption(fs, FIXABLE);
		}
		id_map_free(proper_id_map(fs));
	} else
		handle_bitmaps(fs);

	after_check_fs_tree(fs);
}

static int clean_attributes_handler(reiserfs_filsys_t fs,
				    struct buffer_head **path, int h)
{
	struct buffer_head *bh = path[h];
	int i;

	if (B_LEVEL(bh) != h_to_level(fs, h)) {
		reiserfs_panic
		    ("The node (%lu) with wrong level (%d) found in the "
		     "tree, (%d) expected\n", bh->b_blocknr, B_LEVEL(bh),
		     h_to_level(fs, h));
	}

	if (!is_leaf_node(bh))
		return 0;

	for (i = 0; i < B_NR_ITEMS(bh); i++) {
		if (is_stat_data_ih(item_head(bh, i)) &&
		    get_ih_item_len(item_head(bh, i)) == SD_SIZE) {
			set_sd_v2_sd_attrs((struct stat_data *)item_body(bh, i),
					   0);
			mark_buffer_dirty(bh);
		}
	}

	return 0;
}

void do_clean_attributes(reiserfs_filsys_t fs)
{
	pass_through_tree(fs, clean_attributes_handler, NULL, -1);
	set_sb_v2_flag(fs->fs_ondisk_sb, reiserfs_attrs_cleared);
	mark_buffer_dirty(fs->fs_super_bh);
}
