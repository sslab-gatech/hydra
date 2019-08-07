/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

static int do_items_have_the_same_type(const struct item_head *ih,
				       const struct reiserfs_key *key)
{
	return (get_type(&ih->ih_key) == get_type(key)) ? 1 : 0;
}

static int are_items_in_the_same_node(const struct reiserfs_path *path)
{
	return (PATH_LAST_POSITION(path) <
		B_NR_ITEMS(PATH_PLAST_BUFFER(path)) - 1) ? 1 : 0;
}

static void cut_last_unfm_pointer(struct reiserfs_path *path,
				  struct item_head *ih)
{
	set_ih_free_space(ih, 0);
	if (I_UNFM_NUM(ih) == 1)
		reiserfsck_delete_item(path, 0);
	else
		reiserfsck_cut_from_item(path, -((int)UNFM_P_SIZE));
}

/*
    if this is not a symlink - make it of_this_size;
    otherwise find a size and return it in symlink_size;
*/
static unsigned long indirect_to_direct(struct reiserfs_path *path, __u64 len,
					int symlink)
{
	struct buffer_head *bh = PATH_PLAST_BUFFER(path);
	struct item_head *ih = tp_item_head(path);
	__u32 unfm_ptr;
	struct buffer_head *unfm_bh = NULL;
	struct item_head ins_ih;
	char *buf;
	char bad_drct[fs->fs_blocksize];

	/* direct item to insert */
	memset(&ins_ih, 0, sizeof(ins_ih));
	if (symlink) {
		set_ih_key_format(&ins_ih, KEY_FORMAT_1);
	} else {
		set_ih_key_format(&ins_ih, get_ih_key_format(ih));
	}
	set_key_dirid(&ins_ih.ih_key, get_key_dirid(&ih->ih_key));
	set_key_objectid(&ins_ih.ih_key, get_key_objectid(&ih->ih_key));
	set_type_and_offset(get_ih_key_format(&ins_ih), &ins_ih.ih_key,
			    get_offset(&ih->ih_key) + (I_UNFM_NUM(ih) -
						       1) * bh->b_size,
			    TYPE_DIRECT);

	// we do not know what length this item should be
	unfm_ptr = d32_get((__le32 *) tp_item_body(path), I_UNFM_NUM(ih) - 1);
	if (unfm_ptr && (unfm_bh = bread(bh->b_dev, unfm_ptr, bh->b_size))) {
		/* we can read the block */
		buf = unfm_bh->b_data;

	} else {
		/* we cannot read the block */
		if (unfm_ptr)
			fsck_log
			    ("indirect_to_direct: Reading of the block (%lu), pointed to by the file %K, failed\n",
			     unfm_ptr, &ih->ih_key);
		memset(bad_drct, 0, fs->fs_blocksize);
		buf = bad_drct;
	}
/*
    if (len > MAX_DIRECT_ITEM_LEN (fs->fs_blocksize)) {
	fsck_log ("indirect_to_direct: canot create such a long item %d (%K), "
	    "Cutting it down to %d byte\n", len,  &ih->ih_key, MAX_DIRECT_ITEM_LEN (fs->fs_blocksize) - 8);
	len = MAX_DIRECT_ITEM_LEN (fs->fs_blocksize) - 8;
    }

    if (!len) {
	buf = bad_link;
	len = strlen (bad_link);
    }
*/
	set_ih_item_len(&ins_ih,
			(get_ih_key_format(ih) ==
			 KEY_FORMAT_2) ? ROUND_UP(len) : len);
	set_ih_free_space(&ins_ih, MAX_US_INT);

	mark_ih_become_tail(ih);
	mark_buffer_dirty(bh);
	// last last unformatted node pointer
	path->pos_in_item = I_UNFM_NUM(ih) - 1;
	cut_last_unfm_pointer(path, ih);

	/* insert direct item */
	if (reiserfs_search_by_key_4(fs, &(ins_ih.ih_key), path) == ITEM_FOUND)
		reiserfs_panic
		    ("indirect_to_direct: The direct item %k should not exist yet.",
		     &(ins_ih.ih_key));
	reiserfsck_insert_item(path, &ins_ih, (const char *)(buf));

	brelse(unfm_bh);

	/* put to stat data offset of first byte in direct item */
	return get_offset(&ins_ih.ih_key);	//offset;
}

/*  start_key is the key after which N items need to be deleted
    save_here is a pointer where deleted items need to be saved if save is set.
    start_key is the first undeleted item.
    return whether we are sure there is nothing else of this file
 */
int delete_N_items_after_key(struct reiserfs_key *start_key,
			     struct si **save_here, int skip_dir_items,
			     int n_to_delete)
{
	struct reiserfs_path path;
//    struct reiserfs_key key = *start_key;
	struct reiserfs_key *rkey;
	int count = 0;

	while (1) {
		reiserfs_search_by_key_4(fs, start_key, &path);

		if (get_item_pos(&path) == B_NR_ITEMS(get_bh(&path))) {
			rkey = uget_rkey(&path);
			if (rkey && !not_of_one_file(start_key, rkey)) {
				/* file continues in the right neighbor */
				copy_key(start_key, rkey);
				pathrelse(&path);
				continue;
			}
			/* there is no more items with this key */
			pathrelse(&path);
			return 1;
		}

		if (is_stat_data_ih(tp_item_head(&path)))
			fix_obviously_wrong_sd_mode(&path);

		rkey = &(tp_item_head(&path))->ih_key;
		if (not_of_one_file(start_key, rkey)) {
			/* there are no more item with this key */
			pathrelse(&path);
			return 1;
		}

		copy_key(start_key, rkey);
		/* ok, item found, but make sure that it is not a directory one */
		if ((is_stat_data_key(rkey)
		     && !not_a_directory(tp_item_body(&path)))
		    || (is_direntry_key(rkey))) {
			if (skip_dir_items) {
				/* item of directory found. Leave it in the tree */
				set_offset(KEY_FORMAT_1, start_key,
					   get_offset(start_key) + 1);
				pathrelse(&path);
				continue;
			} else {
				reiserfs_panic
				    ("delete_N_items_after_key: No directory item of %K are "
				     "expected", start_key);
			}
		}
		if (save_here != NULL)
			*save_here =
			    save_and_delete_file_item(*save_here, &path);
		else
			reiserfsck_delete_item(&path, 0);

		count++;

		if (count == n_to_delete)
			break;

		pathrelse(&path);
	}

	return 0;
}

/* returns 1 when file looks correct, -1 if directory items appeared
   there, 0 - only holes in the file found */
/* when it returns, key->k_offset is offset of the last item of file */
/* sd_size is zero if we do not need to convert any indirect to direct */
int are_file_items_correct(struct item_head *sd_ih, void *sd, __u64 * size,
			   __u32 * blocks, int mark_passed_items, int *symlink)
{
	struct reiserfs_path path;
	struct item_head *ih;
	struct reiserfs_key *next_key, *key;
	__u32 sd_first_direct_byte = 0;
	__u64 sd_size;
	unsigned int i;
	int retval, was_tail = 0;
	int had_direct = 0;
	int key_version = get_ih_key_format(sd_ih);
	int next_is_another_object = 0;
	__u64 last_unfm_offset = 0;
	int will_convert = 0;
	int should_convert;

	should_convert = (fsck_mode(fs) != FSCK_REBUILD) || mark_passed_items;
	key = &sd_ih->ih_key;
	get_sd_size(sd_ih, sd, &sd_size);

	if (key_version == KEY_FORMAT_1)
		get_sd_first_direct_byte(sd_ih, sd, &sd_first_direct_byte);

	set_offset(key_version, key, 1);
	set_type(key_version, key, TYPE_DIRECT);

	/* correct size and st_blocks */
	*size = 0;
	*blocks = 0;

	path.path_length = ILLEGAL_PATH_ELEMENT_OFFSET;

	do {
		retval = reiserfs_search_by_position(fs, key, key_version, &path);
		if (retval == POSITION_FOUND && path.pos_in_item != 0)
			die("are_file_items_correct: All bytes we look for must be first items byte (position 0).");

		switch (retval) {
		case POSITION_FOUND:
			/**/ ih = tp_item_head(&path);

			if (ih_was_tail(ih)) {
				was_tail = 1;
			}

			set_type(key_version, key, get_type(&ih->ih_key));

			if (mark_passed_items == 1) {
				mark_item_reachable(ih,
						    PATH_PLAST_BUFFER(&path));
			}
			// does not change path
			next_key = reiserfs_next_key(&path);

			if (next_key == 0 || not_of_one_file(key, next_key) ||
			    (!is_indirect_key(next_key)
			     && !is_direct_key(next_key))) {
				next_is_another_object = 1;
				will_convert = (is_indirect_ih(ih) && sd_size
						&& (I_UNFM_NUM(ih) > 0));
				if (will_convert) {
					last_unfm_offset =
					    get_offset(key) +
					    fs->fs_blocksize * (I_UNFM_NUM(ih) -
								1);
					/* if symlink or
					   [ 1. sd_size points somewhere into last unfm block
					   2. one item of the file was direct before for 3_6 || FDB points to the tail correctly for 3_5
					   3. we can have a tail in the file of a such size ] */
					will_convert = will_convert
					    && (sd_size >= last_unfm_offset)
					    && (sd_size <
						last_unfm_offset +
						fs->fs_blocksize)
					    && !STORE_TAIL_IN_UNFM(sd_size,
								   sd_size -
								   last_unfm_offset
								   + 1,
								   fs->
								   fs_blocksize);

					will_convert = will_convert && (*symlink
									||
									((key_version == KEY_FORMAT_1) && (sd_first_direct_byte == last_unfm_offset)) || ((key_version == KEY_FORMAT_2) && was_tail));
				}
			}

			if (should_convert) {
				*symlink = *symlink && (will_convert
							|| is_direct_key(&ih->
									 ih_key));

				if (!(*symlink)
				    && key_version != get_ih_key_format(ih)) {
					if (fsck_mode(fs) == FSCK_CHECK) {
						fsck_log
						    ("are_file_items_correct: vpf-10250: block %lu, item (%d): The item format (%H)"
						     " is not equal to SD format (%d)\n",
						     get_bh(&path)->b_blocknr,
						     PATH_LAST_POSITION(&path),
						     ih, key_version);
						one_more_corruption(fs,
								    FIXABLE);
					} else {
						fsck_log
						    ("are_file_items_correct: vpf-10280: block %lu, item (%d): The item format (%H)"
						     " is not equal to SD format (%d) - fixed.\n",
						     get_bh(&path)->b_blocknr,
						     PATH_LAST_POSITION(&path),
						     ih, key_version);

						set_type_and_offset(key_version,
								    &ih->ih_key,
								    get_offset
								    (&ih->
								     ih_key),
								    get_type
								    (&ih->
								     ih_key));
						set_ih_key_format(ih,
								  key_version);
						mark_buffer_dirty(get_bh
								  (&path));
					}
				}

				if (*symlink && is_direct_key(&ih->ih_key)) {
					/* symlink. Check that it is of KEY_FORMAT_1 */
					if (fsck_mode(fs) == FSCK_CHECK) {
						if ((get_ih_key_format(ih) !=
						     KEY_FORMAT_1)
						    || (key_format(&ih->ih_key)
							!= KEY_FORMAT_1)) {
							fsck_log
							    ("are_file_items_correct: vpf-10732: block %lu, item (%d): The symlink format (%H)"
							     " is not equal to 3.5 format (%d)\n",
							     get_bh(&path)->
							     b_blocknr,
							     PATH_LAST_POSITION
							     (&path), ih,
							     KEY_FORMAT_1);
							one_more_corruption(fs,
									    FIXABLE);
						}
					} else {
						if ((get_ih_key_format(ih) !=
						     KEY_FORMAT_1)
						    || (key_format(&ih->ih_key)
							!= KEY_FORMAT_1)) {
							fsck_log
							    ("are_file_items_correct: vpf-10732: block %lu, item (%d): The symlink format (%H)"
							     " is not equal to 3.5 format (%d)\n",
							     get_bh(&path)->
							     b_blocknr,
							     PATH_LAST_POSITION
							     (&path), ih,
							     KEY_FORMAT_1);
							set_type_and_offset
							    (KEY_FORMAT_1,
							     &ih->ih_key,
							     get_offset(&ih->
									ih_key),
							     get_type(&ih->
								      ih_key));
							set_ih_key_format(ih,
									  KEY_FORMAT_1);
							mark_buffer_dirty(get_bh
									  (&path));
						}

					}
				}

				if (will_convert)
					*size = sd_size;
				else
					*size =
					    get_offset(&ih->ih_key) +
					    get_bytes_number(ih,
							     fs->fs_blocksize) -
					    1;

				if (get_type(&ih->ih_key) == TYPE_INDIRECT) {
					if (*symlink)	/* symlinks must be calculated as dirs */
						*blocks =
						    dir_size2st_blocks(*size);
					else
						for (i = 0; i < I_UNFM_NUM(ih);
						     i++) {
							__le32 *ind =
							    (__le32 *)
							    tp_item_body(&path);

							if (d32_get(ind, i) !=
							    0)
								*blocks +=
								    (fs->
								     fs_blocksize
								     >> 9);
						}
				} else if (get_type(&ih->ih_key) == TYPE_DIRECT) {
					if (*symlink)	/* symlinks must be calculated as dirs */
						*blocks =
						    dir_size2st_blocks(*size);
					else if (!had_direct)
						*blocks +=
						    (fs->fs_blocksize >> 9);

					/* calculate only the first direct byte */
					had_direct++;
				}
			}

			if (next_is_another_object) {
				/* next item does not exists or is of another object,
				   therefore all items of file are correct */
				if (will_convert) {
					if (fsck_mode(fs) == FSCK_CHECK) {
						/* here it can be symlink only */
						fsck_log
						    ("are_file_items_correct: The indirect item should be converted back"
						     " to direct %K\n",
						     &ih->ih_key);
						one_more_corruption(fs,
								    FIXABLE);
						pathrelse(&path);
					} else {
						__le32 *ind =
						    (__le32 *) tp_item_body(&path);
						/*   DEBUG message.
						   fsck_log ("are_file_items_correct: The indirect item is converted back to direct %K\n", &ih->ih_key);
						 */
						if (d32_get
						    (ind,
						     I_UNFM_NUM(ih) - 1) == 0)
							*blocks +=
							    (fs->
							     fs_blocksize >> 9);

						/* path is released here. */
						sd_first_direct_byte =
						    indirect_to_direct(&path,
								       sd_size -
								       last_unfm_offset
								       + 1,
								       *symlink);
						/* last item of the file is direct item */
						set_offset(key_version, key,
							   sd_first_direct_byte);
						set_type(key_version, key,
							 TYPE_DIRECT);
					}
				} else
					pathrelse(&path);

				return 1;
			}

			/* next item is item of this file */
			if (get_offset(&ih->ih_key) +
			    get_bytes_number(ih,
					     fs->fs_blocksize) !=
			    get_offset(next_key)) {
				/* next item has incorrect offset (hole or overlapping) */
				pathrelse(&path);
				return 0;
			}
			if (do_items_have_the_same_type(ih, next_key) == 1
			    && are_items_in_the_same_node(&path) == 1) {
				/* two indirect items or two direct items in the same leaf. FIXME: Second will be deleted */
				pathrelse(&path);
				return 0;
			}

			/* items are of different types or are in different nodes */
/*
	    if (get_offset (&ih->ih_key) + get_bytes_number (ih, fs->fs_blocksize) != get_offset (next_key))
            {
		// indirect item free space is not set properly
		if (!is_indirect_ih (ih) ) //|| get_ih_free_space(ih) == 0)
		    fsck_log ("are_file_items_correct: "
			      "item must be indirect and must have invalid free space (%H)", ih);
	
                if (fsck_mode (fs) == FSCK_REBUILD)
                {		
                    set_ih_free_space(ih, 0);
                    mark_buffer_dirty (PATH_PLAST_BUFFER (&path));
        	}
	    }
*/
			/* next item exists */
			set_type_and_offset(key_version, key,
					    get_offset(next_key),
					    get_type(next_key));

			if (comp_keys(key, next_key))
				reiserfs_panic
				    ("are_file_items_correct: Internal tree is in inconsistent state, "
				     "the current item key %K and the next key %K must match",
				     key, next_key);
			pathrelse(&path);
			break;

		case POSITION_NOT_FOUND:
			/* We always must have next key found. Exception is first byte. It does not have to exist */
			if (get_offset(key) != 1)
				reiserfs_panic
				    ("are_file_items_correct: Position (offset == %llu) in the middle of"
				     "the file %K was not found.",
				     get_offset(key), key);
			pathrelse(&path);
			return 0;

		case FILE_NOT_FOUND:
			if (get_offset(key) != 1)
				reiserfs_panic
				    ("are_file_items_correct: File %K must be found as we found its StatData.",
				     key);
			pathrelse(&path);
			return 1;

		case DIRECTORY_FOUND:
			pathrelse(&path);
			return -1;
		}
	} while (1);

	die("are_file_items_correct: Cannot reach here");
	return 0;
}

/* delete all items and put them back (after that file should have
   correct sequence of items.
   if should_relocate is specified then it relocates items to a new id.
   if should_change_ih is specified then the key in ih is changed also. */
void rewrite_file(struct item_head *ih, int should_relocate,
		  int should_change_ih)
{
	struct reiserfs_key key;
	struct si *si;
	__u32 new_objectid = 0;
	int moved_items;
	struct reiserfs_key old, new;
	int nothing_else = 0;

	/* starting with the leftmost one - look for all items of file,
	   store and delete and  */
	key = ih->ih_key;
	set_type_and_offset(KEY_FORMAT_1, &key, SD_OFFSET, TYPE_STAT_DATA);

	si = 0;
	nothing_else = 0;

	nothing_else =
	    delete_N_items_after_key(&key, &si,
				     should_relocate /* do not skip dir items */
				     , 10);

	if (should_relocate && (si || should_change_ih)) {
		/* get new objectid for relocation or get objectid with which file
		   was relocated already */
		new_objectid = objectid_for_relocation(&ih->ih_key);
		if (should_change_ih)
			set_key_objectid(&ih->ih_key, new_objectid);

		if (si) {
			old = si->si_ih.ih_key;
			set_key_objectid(&(si->si_ih.ih_key), new_objectid);
			new = si->si_ih.ih_key;
		}
	}

	moved_items = 0;

	while (si) {
		while (si && (nothing_else || si->si_next)) {
			if (should_relocate) {
				set_key_objectid(&(si->si_ih.ih_key),
						 new_objectid);
				moved_items++;
			}

			insert_item_separately(&(si->si_ih), si->si_dnm_data,
					       1 /*was in tree */ );
			si = remove_saved_item(si);
		}
		if (!nothing_else)
			nothing_else =
			    delete_N_items_after_key(&key, &si,
						     should_relocate
						     /* do not skip dir items */
						     , 10);
	}

	if (moved_items)
		fsck_log("rewrite_file: %d items of file %K moved to %K\n",
			 moved_items, &old, &new);

}

/* file must have correct sequence of items and tail must be stored in
   unformatted pointer;
   pos in bh is a position of SD */
static int make_file_writeable(struct buffer_head *bh, int pos)
/* struct item_head * sd_ih, void * sd */
{
	struct item_head *sd_ih = item_head(bh, pos);
	struct item_head sd_ih_copy;
	struct stat_data *sd = (struct stat_data *)item_body(bh, pos);
	struct stat_data sd_copy;
	__u64 size;
	__u32 blocks;
	__u16 mode;
	int retval, symlink;

	sd_ih_copy = *sd_ih;
	memcpy(&sd_copy, sd, get_ih_item_len(sd_ih));
	get_sd_mode(sd_ih, sd, &mode);
	symlink = S_ISLNK(mode);

	if (ih_writable(sd_ih))
		return 1;
	else {
		mark_ih_writable(sd_ih);
		mark_buffer_dirty(bh);
	}

	retval =
	    are_file_items_correct(&sd_ih_copy, &sd_copy, &size, &blocks,
				   0 /*do not mark accessed */ , &symlink);
	if (retval == 1)
		/* file looks correct */
		return 1;

	rewrite_file(sd_ih, 0, 0);
	/*fsck_data (fs)->rebuild.rewritten ++; */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

	size = 0;

	if (are_file_items_correct
	    (&sd_ih_copy, &sd_copy, &size, &blocks,
	     0 /*do not mark accessed */ , &symlink) == 0) {
		fsck_log
		    ("WARNING: file was rewritten but still is not correct %K\n",
		     &sd_ih->ih_key);
		return -1;
	}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

	return 1;
}

/* this inserts __first__ indirect item (having k_offset == 1 and only
   one unfm pointer) into tree */
static int create_first_item_of_file(struct item_head *ih, char *item,
				     struct reiserfs_path *path,
				     int was_in_tree)
{
	__le32 unfm_ptr;
	__le32 *ni = 0;
	struct buffer_head *unbh;
	struct item_head indih;
	unsigned int i;
	int retval;

	mark_item_unreachable(&indih);

	copy_key(&(indih.ih_key), &(ih->ih_key));
	set_ih_item_len(&indih, UNFM_P_SIZE);

	set_ih_free_space(&indih, 0);
	if (get_offset(&ih->ih_key) > fs->fs_blocksize) {
		/* insert indirect item containing 0 unfm pointer */
		unfm_ptr = 0;
		retval = 0;
	} else {
		if (is_direct_ih(ih)) {
			/* copy direct item to new unformatted node. Save information about it */
			int len = get_bytes_number(ih, fs->fs_blocksize);

			unbh =
			    reiserfsck_get_new_buffer(PATH_PLAST_BUFFER(path)->
						      b_blocknr);
			memset(unbh->b_data, 0, unbh->b_size);
			unfm_ptr = cpu_to_le32(unbh->b_blocknr);
			memcpy(unbh->b_data + get_offset(&ih->ih_key) - 1, item,
			       len);

			set_ih_free_space(&indih, fs->fs_blocksize -
					  len - (get_offset(&ih->ih_key) - 1));

			mark_ih_was_tail(&indih);
			mark_buffer_dirty(unbh);
			mark_buffer_uptodate(unbh, 1);
			brelse(unbh);

			retval = len;
		} else {
			/* take first unformatted pointer from an indirect item */
			//free_sp = ih_get_free_space(0, ih, item);

			set_ih_item_len(&indih, get_ih_item_len(ih));
			ni = getmem(get_ih_item_len(ih));
			memcpy(ni, (item), get_ih_item_len(ih));

			if (!was_in_tree) {
				for (i = 0; i < I_UNFM_NUM(ih); i++) {
					if (still_bad_unfm_ptr_2
					    (d32_get(ni, i)))
						reiserfs_panic
						    ("create_first_item_of_file: The file %K has a pointer to the bad block (%u)",
						     &ih->ih_key, d32_get(ni,
									  i));
					mark_block_used(d32_get(ni, i), 0);
				}
			}

			retval =
			    fs->fs_blocksize * get_ih_item_len(ih) /
			    UNFM_P_SIZE;
		}
	}

	set_ih_key_format(&indih, get_ih_key_format(ih));
	//ih_version(&indih) = ih_version(ih);
	set_offset(key_format(&(ih->ih_key)), &indih.ih_key, 1);
	set_type(key_format(&(ih->ih_key)), &indih.ih_key, TYPE_INDIRECT);

	if (ni) {
		reiserfsck_insert_item(path, &indih, (const char *)ni);
		freemem(ni);
	} else {
		reiserfsck_insert_item(path, &indih, (const char *)&unfm_ptr);
	}

	return retval;
}

/* path points to first part of tail. Function copies file tail into unformatted node and returns
   its block number. */
/* we convert direct item that is on the path to indirect. we need a number of free block for
   unformatted node. reiserfs_new_blocknrs will start from block number returned by this function */
static unsigned long block_to_start(struct reiserfs_path *path)
{
	struct buffer_head *bh;
	struct item_head *ih;
	__u32 blk;

	bh = PATH_PLAST_BUFFER(path);
	ih = tp_item_head(path);
	if (get_offset(&ih->ih_key) == 1 || PATH_LAST_POSITION(path) == 0)
		return bh->b_blocknr;

	ih--;
	blk = d32_get((__le32 *) ih_item_body(bh, ih), I_UNFM_NUM(ih) - 1);
	return (blk ? blk : bh->b_blocknr);
}

static void direct2indirect2(unsigned long unfm, struct reiserfs_path *path)
{
	struct item_head *ih;
	struct reiserfs_key key;
	struct buffer_head *unbh;
	__le32 ni;
	int copied = 0;
	int file_format;

	ih = tp_item_head(path);
	copy_key(&key, &(ih->ih_key));

	file_format = key_format(&key);

	if (get_offset(&key) % fs->fs_blocksize != 1) {
		/* look for first part of tail */
		pathrelse(path);
		set_offset(file_format, &key,
			   (get_offset(&key) & ~(fs->fs_blocksize - 1)) + 1);
		if (reiserfs_search_by_key_4(fs, &key, path) != ITEM_FOUND)
			die("direct2indirect: can not find first part of tail");
	}

	unbh = reiserfsck_get_new_buffer(unfm ? unfm : block_to_start(path));
	memset(unbh->b_data, 0, unbh->b_size);

	/* delete parts of tail coping their contents to new buffer */
	do {
		__u64 len;

		ih = tp_item_head(path);

		len = get_bytes_number(ih, fs->fs_blocksize);

		memcpy(unbh->b_data + copied,
		       ih_item_body(PATH_PLAST_BUFFER(path), ih), len);

//      save_unfm_overwriting (unbh->b_blocknr, ih);
		copied += len;
		set_offset(file_format, &key, get_offset(&key) + len);

		reiserfsck_delete_item(path, 0);

	} while (reiserfs_search_by_key_4(fs, &key, path) == ITEM_FOUND);

	pathrelse(path);

	/* paste or insert pointer to the unformatted node */
	set_offset(file_format, &key, get_offset(&key) - copied);
//    set_offset (ih_key_format (ih), &key, get_offset (&key) - copied);
//  key.k_offset -= copied;
	ni = cpu_to_le32(unbh->b_blocknr);

	if (reiserfs_search_by_position(fs, &key, file_format, path) == FILE_NOT_FOUND) {
		struct item_head insih;

		copy_key(&(insih.ih_key), &key);
		set_ih_key_format(&insih, file_format);
		set_type(get_ih_key_format(&insih), &insih.ih_key,
			 TYPE_INDIRECT);
		set_ih_free_space(&insih, 0);
		mark_item_unreachable(&insih);
		set_ih_item_len(&insih, UNFM_P_SIZE);
		mark_ih_was_tail(&insih);
		reiserfsck_insert_item(path, &insih, (const char *)&(ni));
	} else {
		ih = tp_item_head(path);

		if (!is_indirect_ih(ih))
			reiserfs_panic
			    ("%s: The item, which is supposed to be deleted, found in the tree - %k",
			     __FUNCTION__, &ih->ih_key);
		if (get_bytes_number(ih, fs->fs_blocksize) +
		    get_offset(&ih->ih_key) != get_offset(&key))
			reiserfs_panic
			    ("%s: The item %k with the wrong offset found in the block %lu block",
			     __FUNCTION__, &ih->ih_key,
			     PATH_PLAST_BUFFER(path)->b_blocknr);

		mark_ih_was_tail(ih);
		mark_buffer_dirty(get_bh(path));
		reiserfsck_paste_into_item(path, (const char *)&ni,
					   UNFM_P_SIZE);
	}

	mark_buffer_dirty(unbh);
	mark_buffer_uptodate(unbh, 1);
	brelse(unbh);

	if (reiserfs_search_by_position(fs, &key, file_format, path) != POSITION_FOUND
	    || !is_indirect_ih(tp_item_head(path)))
		reiserfs_panic
		    ("direct2indirect: The data %k, which are supposed to be converted, are not found",
		     &key);
	return;
}

static int append_to_unformatted_node(struct item_head *comingih,
				      struct item_head *ih, char *item,
				      struct reiserfs_path *path,
				      unsigned int coming_len)
{
	struct buffer_head *bh, *unbh = NULL;
	__u64 end_of_data, free_space;
	__u16 offset = get_offset(&comingih->ih_key) % fs->fs_blocksize - 1;
	__u32 unfm_ptr;
	int zero_number;

	bh = PATH_PLAST_BUFFER(path);
	unfm_ptr = d32_get((__le32 *) ih_item_body(bh, ih), I_UNFM_NUM(ih) - 1);

	/* append to free space of the last unformatted node of indirect item ih */
	free_space =
	    get_offset(&ih->ih_key) + fs->fs_blocksize * I_UNFM_NUM(ih) -
	    get_offset(&comingih->ih_key);

	if (free_space < coming_len)
		reiserfs_panic
		    ("%s: block %lu: The unformatted node %u, pointed by the file %k, does not have enough free space"
		     "for appending %llu bytes", __FUNCTION__, bh->b_blocknr,
		     unfm_ptr, &ih->ih_key, coming_len);

	if (fs->fs_blocksize < free_space)
		reiserfs_panic
		    ("%s: block %lu: The unformatted node %u, pointed by the file %k, does not have enough free space"
		     "for appending %llu bytes", __FUNCTION__, bh->b_blocknr,
		     &ih->ih_key, unfm_ptr, &ih->ih_key, coming_len);

	end_of_data = fs->fs_blocksize - free_space;
	zero_number = offset - end_of_data;

	if (unfm_ptr && !not_data_block(fs, unfm_ptr)) {
		unbh = bread(fs->fs_dev, unfm_ptr, fs->fs_blocksize);
		if (!is_block_used(unfm_ptr))
			reiserfs_panic
			    ("%s: block %lu: The unformatted node %u, pointed by the file %k, is marked as unused",
			     __FUNCTION__, bh->b_blocknr, unfm_ptr,
			     &ih->ih_key);

		if (unbh == 0)
			unfm_ptr = 0;
	}

	if (!unfm_ptr || not_data_block(fs, unfm_ptr)) {
		/* indirect item points to block which can not be pointed or to 0, in
		   any case we have to allocate new node */
		/*if (unfm_ptr == 0 || unfm_ptr >= SB_BLOCK_COUNT (fs)) { */
		unbh = reiserfsck_get_new_buffer(bh->b_blocknr);
		memset(unbh->b_data, 0, unbh->b_size);
		d32_put((__le32 *) ih_item_body(bh, ih), I_UNFM_NUM(ih) - 1,
			unbh->b_blocknr);
		/*mark_block_unformatted (unbh->b_blocknr); */
		mark_buffer_dirty(bh);
	}
	memset(unbh->b_data + end_of_data, 0, zero_number);
	memcpy(unbh->b_data + offset, item, coming_len);

//    save_unfm_overwriting (unbh->b_blocknr, comingih);

	free_space -= (zero_number + coming_len);
	set_ih_free_space(ih,
			  get_ih_free_space(ih) - (zero_number + coming_len));
	memset(unbh->b_data + offset + coming_len, 0, free_space);
//  mark_buffer_uptodate (unbh, 0);
	mark_buffer_uptodate(unbh, 1);
	mark_buffer_dirty(unbh);
	brelse(unbh);
	pathrelse(path);
	return coming_len;
}

/* this appends file with one unformatted node pointer (since balancing
   algorithm limitation). This pointer can be 0, or new allocated block or
   pointer from indirect item that is being inserted into tree */
int reiserfsck_append_file(struct item_head *comingih, char *item, int pos,
			   struct reiserfs_path *path, int was_in_tree)
{
	__le32 *ni;
	struct buffer_head *unbh;
	int retval;
	struct item_head *ih = tp_item_head(path);
	__u32 bytes_number;
	int i, count = 0;

	if (!is_indirect_ih(ih))
		reiserfs_panic
		    ("reiserfsck_append_file: Operation is not allowed for non-indirect item %k",
		     &ih->ih_key);

	if (is_direct_ih(comingih)) {
		unsigned int coming_len =
		    get_bytes_number(comingih, fs->fs_blocksize);

		if (get_offset(&comingih->ih_key) <
		    get_offset(&ih->ih_key) +
		    fs->fs_blocksize * I_UNFM_NUM(ih)) {
			/* direct item fits to free space of indirect item */
			return append_to_unformatted_node(comingih, ih, item,
							  path, coming_len);
		}

		unbh =
		    reiserfsck_get_new_buffer(PATH_PLAST_BUFFER(path)->
					      b_blocknr);
		memset(unbh->b_data, 0, unbh->b_size);
		memcpy(unbh->b_data +
		       get_offset(&comingih->ih_key) % unbh->b_size - 1, item,
		       coming_len);

		mark_buffer_dirty(unbh);
		mark_buffer_uptodate(unbh, 1);

		ni = getmem(UNFM_P_SIZE);
		d32_put(ni, 0, unbh->b_blocknr);
		count = 1;

		brelse(unbh);
		retval = coming_len;
	} else {
		/* coming item is indirect item */

		bytes_number = get_bytes_number(ih, fs->fs_blocksize);
		if (get_offset(&comingih->ih_key) + pos * fs->fs_blocksize !=
		    get_offset(&ih->ih_key) + bytes_number)
			reiserfs_panic
			    ("reiserfsck_append_file: file %K: Cannot append indirect pointers of the offset "
			     "(%LLu) at the position %LLu\n", &comingih->ih_key,
			     get_offset(&comingih->ih_key) +
			     pos * fs->fs_blocksize,
			     get_offset(&ih->ih_key) + bytes_number);

		/* take unformatted pointer from an indirect item */
		count = I_UNFM_NUM(comingih) - pos;
		ni = getmem(count * UNFM_P_SIZE);
		memcpy(ni, (item + pos * UNFM_P_SIZE), count * UNFM_P_SIZE);

		if (!was_in_tree) {
			for (i = 0; i < count; i++) {
				if (still_bad_unfm_ptr_2(d32_get(ni, i)))
					die("reiserfsck_append_file: Trying to insert a pointer to illegal block (%u)", d32_get(ni, i));
				mark_block_used(d32_get(ni, i), 0);
			}
		}

		retval = fs->fs_blocksize * count;
	}

	reiserfsck_paste_into_item(path, (const char *)ni, count * UNFM_P_SIZE);
	freemem(ni);
	return retval;
}

long long int must_there_be_a_hole(struct item_head *comingih,
				   struct reiserfs_path *path)
{
	struct item_head *ih = tp_item_head(path);

	if (is_direct_ih(ih)) {
		direct2indirect2(0, path);
		ih = tp_item_head(path);
	}

	path->pos_in_item = I_UNFM_NUM(ih);

	return (get_offset(&comingih->ih_key) -
		get_offset(&ih->ih_key)) / fs->fs_blocksize - I_UNFM_NUM(ih);
/*
    if (get_offset (&ih->ih_key) + (I_UNFM_NUM (ih) + 1) * fs->fs_blocksize <= get_offset (&comingih->ih_key))
	return 1;

    return 0;*/
}

int reiserfs_append_zero_unfm_ptr(struct reiserfs_path *path,
				  long long int p_count)
{
	__le32 *ni;
	long long int count;

	if (is_direct_ih(tp_item_head(path)))
		/* convert direct item to indirect */
		direct2indirect2(0, path);

	count = MAX_INDIRECT_ITEM_LEN(fs->fs_blocksize) / UNFM_P_SIZE;

	if (p_count <= count)
		count = p_count;

	ni = getmem(count * UNFM_P_SIZE);
	memset(ni, 0, count * UNFM_P_SIZE);

	reiserfsck_paste_into_item(path, (const char *)ni, count * UNFM_P_SIZE);
	freemem(ni);
	return 0;
}

/* write direct item to unformatted node */
/* coming item is direct */
static int overwrite_by_direct_item(struct item_head *comingih, char *item,
				    struct reiserfs_path *path)
{
	__u32 unfm_ptr;
	struct buffer_head *unbh, *bh;
	struct item_head *ih;
	int offset;
	__u64 coming_len = get_bytes_number(comingih, fs->fs_blocksize);

	bh = PATH_PLAST_BUFFER(path);
	ih = tp_item_head(path);

	unfm_ptr = d32_get((__le32 *) ih_item_body(bh, ih), path->pos_in_item);
	unbh = 0;

	if (unfm_ptr != 0 && unfm_ptr < get_sb_block_count(fs->fs_ondisk_sb)) {
		 /**/ unbh = bread(fs->fs_dev, unfm_ptr, bh->b_size);
		if (!is_block_used(unfm_ptr))
			die("overwrite_by_direct_item: block %lu, item %d, pointer %d: The pointed block" "(%u) being overwritten is marked as unused.", bh->b_blocknr, PATH_LAST_POSITION(path), path->pos_in_item, unfm_ptr);
		if (unbh == 0)
			unfm_ptr = 0;
	}
	if (unfm_ptr == 0 || unfm_ptr >= get_sb_block_count(fs->fs_ondisk_sb)) {
		if ((unbh = reiserfsck_get_new_buffer(bh->b_blocknr)) != NULL) {
			memset(unbh->b_data, 0, unbh->b_size);
			d32_put((__le32 *) ih_item_body(bh, ih), path->pos_in_item,
				unbh->b_blocknr);
			mark_buffer_dirty(bh);
		} else {
			die("overwrite_by_direct_item: Could not allocate a new block for new data");
		}
	}

	offset = (get_offset(&comingih->ih_key) % bh->b_size) - 1;
	if (offset + coming_len > bh->b_size)
		die("overwrite_by_direct_item: The length of the file after insertion (offset=%lu, length=%u)" "will exceed the maximal possible length.", (long unsigned)get_offset(&comingih->ih_key), (unsigned)coming_len);

	memcpy(unbh->b_data + offset, item, coming_len);

//    save_unfm_overwriting (unbh->b_blocknr, comingih);

	if ((path->pos_in_item == (I_UNFM_NUM(ih) - 1)) &&
	    (bh->b_size - 0 /*ih_free_space (ih) */ ) < (offset + coming_len)) {
		set_ih_free_space(ih, bh->b_size - (offset + coming_len));
		mark_buffer_dirty(bh);
	}
	mark_buffer_dirty(unbh);
//  mark_buffer_uptodate (unbh, 0);
	mark_buffer_uptodate(unbh, 1);
	brelse(unbh);
	return coming_len;
}

#if 0

void overwrite_unfm_by_unfm(unsigned long unfm_in_tree,
			    unsigned long coming_unfm, int bytes_in_unfm)
{
	struct overwritten_unfm_segment *unfm_os_list;	/* list of overwritten segments of the unformatted node */
	struct overwritten_unfm_segment unoverwritten_segment;
	struct buffer_head *bh_in_tree, *coming_bh;

	if (!test_bit
	    (coming_unfm % (fs->fs_blocksize * 8),
	     SB_AP_BITMAP(fs)[coming_unfm / (fs->fs_blocksize * 8)]->b_data))
		/* block (pointed by indirect item) is free, we do not have to keep its contents */
		return;

	/* coming block is marked as used in disk bitmap. Put its contents to
	   block in tree preserving everything, what has been overwritten there by
	   direct items */
	unfm_os_list =
	    find_overwritten_unfm(unfm_in_tree, bytes_in_unfm,
				  &unoverwritten_segment);
	if (unfm_os_list) {
		/*    add_event (UNFM_OVERWRITING_UNFM); */
		bh_in_tree = bread(fs->fs_dev, unfm_in_tree, fs->fs_blocksize);
		coming_bh = bread(fs->fs_dev, coming_unfm, fs->fs_blocksize);
		if (bh_in_tree == 0 || coming_bh == 0)
			return;

		while (get_unoverwritten_segment
		       (unfm_os_list, &unoverwritten_segment)) {
			if (unoverwritten_segment.ous_begin < 0
			    || unoverwritten_segment.ous_end > bytes_in_unfm - 1
			    || unoverwritten_segment.ous_begin >
			    unoverwritten_segment.ous_end)
				die("overwrite_unfm_by_unfm: invalid segment found (%d %d)", unoverwritten_segment.ous_begin, unoverwritten_segment.ous_end);

			memcpy(bh_in_tree->b_data +
			       unoverwritten_segment.ous_begin,
			       coming_bh->b_data +
			       unoverwritten_segment.ous_begin,
			       unoverwritten_segment.ous_end -
			       unoverwritten_segment.ous_begin + 1);
			mark_buffer_dirty(bh_in_tree);
		}

		brelse(bh_in_tree);
		brelse(coming_bh);
	}
}
#endif

/* put unformatted node pointers from incoming item over the in-tree ones */
static int overwrite_by_indirect_item(struct item_head *comingih,
				      __le32 * coming_item,
				      struct reiserfs_path *path,
				      int *pos_in_coming_item)
{
	struct buffer_head *bh = PATH_PLAST_BUFFER(path);
	struct item_head *ih = tp_item_head(path);
	int written;
	__le32 *item_in_tree;
	int src_unfm_ptrs, dest_unfm_ptrs, to_copy, i, dirty = 0;

	item_in_tree = (__le32 *) ih_item_body(bh, ih) + path->pos_in_item;
	coming_item += *pos_in_coming_item;

	dest_unfm_ptrs = I_UNFM_NUM(ih) - path->pos_in_item;
	src_unfm_ptrs = I_UNFM_NUM(comingih) - *pos_in_coming_item;

	if (dest_unfm_ptrs >= src_unfm_ptrs) {
		/* whole coming item (comingih) fits into item in tree (ih) starting with path->pos_in_item */

		//free_sp = ih_get_free_space(0, comingih, (char *)coming_item);

		written = get_bytes_number(comingih, fs->fs_blocksize) -
		    /* free_sp - */ *pos_in_coming_item * fs->fs_blocksize;
		*pos_in_coming_item = I_UNFM_NUM(comingih);
		to_copy = src_unfm_ptrs;
		if (dest_unfm_ptrs == src_unfm_ptrs)
			set_ih_free_space(ih, 0 /* free_sp */ );
	} else {
		/* only part of coming item overlaps item in the tree */
		*pos_in_coming_item += dest_unfm_ptrs;
		written = dest_unfm_ptrs * fs->fs_blocksize;
		to_copy = dest_unfm_ptrs;
		set_ih_free_space(ih, 0);
	}

	for (i = 0; i < to_copy; i++) {
		if (d32_get(coming_item, i) != 0
		    && d32_get(item_in_tree, i) == 0) {
			/* overwrite holes only by correct a pointer in the coming item
			   which must be correct */
			d32_put(item_in_tree, i, d32_get(coming_item, i));
			mark_block_used(d32_get(coming_item, i), 0);
			dirty++;
		}
	}
	if (dirty)
		mark_buffer_dirty(bh);
	return written;
}

static int reiserfsck_overwrite_file(struct item_head *comingih, char *item,
				     struct reiserfs_path *path,
				     int *pos_in_coming_item, int was_in_tree)
{
	__u32 unfm_ptr;
	int written = 0;
	struct item_head *ih = tp_item_head(path);

	if (not_of_one_file(ih, &(comingih->ih_key)))
		reiserfs_panic
		    ("reiserfsck_overwrite_file: The file to be overwritten %K must be of"
		     " the same as the new data %K", &ih->ih_key,
		     &comingih->ih_key);

	if (is_direct_ih(ih)) {
		unfm_ptr = 0;
		if (is_indirect_ih(comingih)) {
			if (get_offset(&ih->ih_key) % fs->fs_blocksize != 1)
				reiserfs_panic
				    ("reiserfsck_overwrite_file: The second part of the tail %k can not"
				     " be overwritten by indirect item %k",
				     &ih->ih_key, &comingih->ih_key);
			/* use pointer from coming indirect item */
			unfm_ptr = d32_get((__le32 *) item, *pos_in_coming_item);
			if (!was_in_tree) {
				if (still_bad_unfm_ptr_2(unfm_ptr))
					die("reiserfsck_overwrite_file: The pointer to the unformatted block (%u)" " points to the bad area.", unfm_ptr);
			}
		}
		/* */
		direct2indirect2(unfm_ptr, path);
	}
	if (is_direct_ih(comingih)) {
		written = overwrite_by_direct_item(comingih, item, path);
	} else {
		if (was_in_tree)
			reiserfs_panic
			    ("reiserfsck_overwrite_file: Item %k we are going to overwrite with"
			     " %k cannot not be in the tree yet", &ih->ih_key,
			     &comingih->ih_key);
		written =
		    overwrite_by_indirect_item(comingih, (__le32 *) item, path,
					       pos_in_coming_item);
	}

	return written;
}

/*
 */
int reiserfsck_file_write(struct item_head *ih, char *item, int was_in_tree)
{
	struct reiserfs_path path;
	int count, pos_in_coming_item;
	long long int retval, written;
	struct reiserfs_key key;
	int file_format = KEY_FORMAT_UNDEFINED;
	int relocated = 0;

	if (!was_in_tree) {
		__u16 mode;

		/* We already inserted all SD items. If we cannot find SD of this item - skip it */
		memset(&key, 0, sizeof(key));

check_again:
		copy_short_key(&key, &(ih->ih_key));

		if (reiserfs_search_by_key_4(fs, &key, &path) != ITEM_FOUND) {
			fsck_log
			    ("vpf-10260: The file we are inserting the new item (%H) into has no"
			     " StatData, insertion was skipped\n", ih);
			pathrelse(&path);
			return 0;
		}

		/*SD found */
		file_format = get_ih_key_format(tp_item_head(&path));
		get_sd_mode(tp_item_head(&path), tp_item_body(&path), &mode);

		if (file_format != get_ih_key_format(ih)) {
			/* Not for symlinks and not for items which should be relocted. */
			if (((S_ISDIR(mode) && is_direntry_ih(ih)) ||
			     (!S_ISDIR(mode) && !is_direntry_ih(ih))) &&
			    !S_ISLNK(mode)) {
				set_type_and_offset(file_format, &ih->ih_key,
						    get_offset(&ih->ih_key),
						    get_type(&ih->ih_key));
				set_ih_key_format(ih, file_format);
			}
		}

		if (!relocated && should_relocate(ih)) {
			rewrite_file(ih, 1, 1 /*change new_ih */ );
			pathrelse(&path);
			relocated = 1;

			/* object has been relocated but we should not mark it as used in semantic map,
			   as it does not exist at pass2 and we do not get here for relocation as
			   was_in_tree == 1 */

			goto check_again;
		}

		if (make_file_writeable(get_bh(&path), get_item_pos(&path)) ==
		    -1) {
			/* write was not completed. Skip that item. Maybe it should be saved to lost_found */
			fsck_log
			    ("reiserfsck_file_write: WARNING: The file we are inserting the new item %k into was"
			     " not recovered and is still in inconsistent state, insertion was skipped\n",
			     &ih->ih_key);
			pathrelse(&path);
			return 0;
		}

		pathrelse(&path);
	}

	count = get_bytes_number(ih, fs->fs_blocksize);
	pos_in_coming_item = 0;

	copy_key(&key, &(ih->ih_key));

	while (count) {

		retval = reiserfs_search_by_position(fs, &key, key_format(&key), &path);

		/*  if there are items of bigger offset than we are looking for and
		   there is no item between wamted offset and SD, insert first item */
		if (retval == POSITION_NOT_FOUND &&
		    (PATH_LAST_POSITION(&path) >= B_NR_ITEMS(get_bh(&path)) ||
		     get_offset(&tp_item_head(&path)->ih_key) > get_offset(&key)))
			retval = FILE_NOT_FOUND;

		if (retval == DIRECTORY_FOUND)
			reiserfs_panic
			    ("The directory was found at the place of the file we are going to insert"
			     " the item %k into", key);

		if (retval == POSITION_FOUND) {
			written =
			    reiserfsck_overwrite_file(ih, item, &path,
						      &pos_in_coming_item,
						      was_in_tree);
			count -= written;
			set_offset(key_format(&key), &key,
				   get_offset(&key) + written);
		}
		if (retval == FILE_NOT_FOUND) {
			written =
			    create_first_item_of_file(ih, item, &path,
						      was_in_tree);
			count -= written;

			set_offset(key_format(&key), &key,
				   get_offset(&key) + written);
		}
		if (retval == POSITION_NOT_FOUND) {

			if (is_direct_ih(ih)) {
				mark_ih_was_tail(tp_item_head(&path));
				mark_buffer_dirty(get_bh(&path));
			}

			if ((written = must_there_be_a_hole(ih, &path)) > 0) {
				reiserfs_append_zero_unfm_ptr(&path, written);
			} else {
				written =
				    reiserfsck_append_file(ih, item,
							   pos_in_coming_item,
							   &path, was_in_tree);
				count -= written;
				set_offset(key_format(&key), &key,
					   get_offset(&key) + written);
				pos_in_coming_item +=
				    written / fs->fs_blocksize;
			}
		}
		if (count < 0)
			reiserfs_panic
			    ("reiserfsck_file_write: We wrote into the file %K more bytes than needed - count (%d) < 0.",
			     &key, count);
		pathrelse(&path);
	}

	/* This is a test for writing into the file. If not sure that file data are consistent after
	   reiserfsck_file_write - uncomment this clause: */

/*    if (!was_in_tree && are_file_items_correct (&ih->ih_key,
    	(file_format == KEY_FORMAT_UNDEFINED) ?	get_ih_key_format (ih) : file_format,
    	&size, &blocks, 0, symlink, 0) == 0)
        reiserfs_panic ("reiserfsck_file_write: item was not inserted properly\n");*/

	return get_bytes_number(ih, fs->fs_blocksize);
}

void one_more_corruption(reiserfs_filsys_t fs, int kind)
{
	if (kind == FATAL)
		fsck_check_stat(fs)->fatal_corruptions++;
	else if (kind == FIXABLE)
		fsck_check_stat(fs)->fixable_corruptions++;
}

void one_less_corruption(reiserfs_filsys_t fs, int kind)
{
	if (kind == FATAL)
		fsck_check_stat(fs)->fatal_corruptions--;
	else if (kind == FIXABLE)
		fsck_check_stat(fs)->fixable_corruptions--;
}
