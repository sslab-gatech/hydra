/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by reiserfsprogs/README
 */

#include "fsck.h"

static struct reiserfs_key *trunc_links = NULL;
static __u32 links_num = 0;

int wrong_mode(const struct reiserfs_key *key, __u16 * mode, __u64 real_size,
	       int symlink);
int wrong_st_blocks(const struct reiserfs_key *key, __u32 * blocks,
		    __u32 sd_blocks, __u16 mode, int new_format);
int wrong_st_size(const struct reiserfs_key *key,
		  unsigned long long max_file_size, int blocksize,
		  __u64 * size, __u64 sd_size, int type);
int wrong_first_direct_byte(const struct reiserfs_key *key, int blocksize,
			    __u32 * first_direct_byte,
			    __u32 sd_first_direct_byte, __u32 size);
void get_object_key(struct reiserfs_de_head *deh, struct reiserfs_key *key,
		    struct reiserfs_key *entry_key, struct item_head *ih);
void print_name(char *name, int len);
void erase_name(int len);

struct reiserfs_path_key {
	struct short_key {
		__u32 k_dir_id;
		__u32 k_objectid;
	} key;
	struct reiserfs_path_key *next, *prev;
};

static struct reiserfs_path_key *head_key = NULL;
static struct reiserfs_path_key *tail_key = NULL;

static int check_path_key(const struct reiserfs_key *key)
{
	struct reiserfs_path_key *cur = head_key;

	while (cur != NULL) {
		if (!comp_short_keys(&cur->key, key)) {
			fsck_log
			    ("\nsemantic check: The directory %k has 2 names.",
			     key);
			return LOOP_FOUND;
		}
		cur = cur->next;
	}
	return 0;
}

static int add_path_key(const struct reiserfs_key *key)
{
	if (check_path_key(key))
		return LOOP_FOUND;

	if (tail_key == NULL) {
		tail_key = getmem(sizeof(struct reiserfs_path_key));
		head_key = tail_key;
		tail_key->prev = NULL;
	} else {
		tail_key->next = getmem(sizeof(struct reiserfs_path_key));
		tail_key->next->prev = tail_key;
		tail_key = tail_key->next;
	}
	copy_short_key(&tail_key->key, key);
	tail_key->next = NULL;

	return 0;
}

static void del_path_key(void)
{
	if (tail_key == NULL)
		die("Wrong path_key structure");

	if (tail_key->prev == NULL) {
		freemem(tail_key);
		tail_key = head_key = NULL;
	} else {
		tail_key = tail_key->prev;
		freemem(tail_key->next);
		tail_key->next = NULL;
	}
}

/* path is path to stat data. If file will be relocated - new_ih will contain
   a key file was relocated with */
static int check_check_regular_file(struct reiserfs_path *path, void *sd,
				    struct item_head *new_ih)
{
	int is_new_file;
//    struct reiserfs_key key, sd_key;
	__u16 mode;
	__u32 nlink;
	__u64 real_size, sd_size;
	__u32 blocks, sd_blocks;	/* proper values and value in stat data */
	__u32 first_direct_byte, sd_first_direct_byte;

	struct item_head *ih, sd_ih;
	int fix_sd;
	int symlnk = 0;
	int retval = OK;
	__u32 tmp_position;

	ih = tp_item_head(path);

	if (new_ih) {
		/* this objectid is used already */
		*new_ih = *ih;
		pathrelse(path);
		rewrite_file(new_ih, 1, 1);
		linked_already(&new_ih->ih_key);
		one_less_corruption(fs, FIXABLE);
		sem_pass_stat(fs)->oid_sharing_files_relocated++;
		retval = RELOCATED;
		if (reiserfs_search_by_key_4(fs, &(new_ih->ih_key), path) ==
		    ITEM_NOT_FOUND)
			reiserfs_panic
			    ("check_check_regular_file: Could not find a StatData of "
			     "the relocated file %K", &new_ih->ih_key);
		/* stat data is marked unreachable again due to relocation, fix that */
		ih = tp_item_head(path);
		sd = tp_item_body(path);
	}

	if (get_ih_item_len(ih) == SD_SIZE)
		is_new_file = 1;
	else
		is_new_file = 0;

	get_sd_nlink(ih, sd, &nlink);
	get_sd_mode(ih, sd, &mode);
	get_sd_size(ih, sd, &sd_size);
	get_sd_blocks(ih, sd, &sd_blocks);

/*	
    if (fsck_mode (fs) == FSCK_FIX_FIXABLE) {
    	// check and set nlink first
    	nlink ++;
    	set_sd_nlink (ih, sd, &nlink);
    	mark_buffer_dirty (bh);

    	if (nlink > 1)
	    return OK;
    }
*/

	if (!is_new_file)
		get_sd_first_direct_byte(ih, sd, &sd_first_direct_byte);

	if (S_ISLNK(mode))
		symlnk = 1;

	sd_ih = *ih;
//    sd_key = sd_ih.ih_key;
	pathrelse(path);

	if (are_file_items_correct
	    (&sd_ih, sd, &real_size, &blocks, 0 /* do not mark reachable */ ,
	     &symlnk) != 1) {
		one_more_corruption(fs, FATAL);
		fsck_log
		    ("check_regular_file: The file %K with the corrupted structure found\n",
		     &sd_ih.ih_key);
	} else {
		fix_sd = 0;

		fix_sd += wrong_mode(&sd_ih.ih_key, &mode, real_size, symlnk);
		if (!is_new_file)
			fix_sd +=
			    wrong_first_direct_byte(&sd_ih.ih_key,
						    fs->fs_blocksize,
						    &first_direct_byte,
						    sd_first_direct_byte,
						    real_size);

		if (reiserfs_bin_search
		    (&sd_ih.ih_key, trunc_links, links_num,
		     sizeof(sd_ih.ih_key), &tmp_position,
		     comp_short_keys) != POSITION_FOUND) {
			fix_sd +=
			    wrong_st_size(&sd_ih.ih_key,
					  is_new_file ? MAX_FILE_SIZE_V2 :
					  MAX_FILE_SIZE_V1, fs->fs_blocksize,
					  &real_size, sd_size,
					  symlnk ? TYPE_SYMLINK : 0);
		} else {
			real_size = sd_size;
		}

		fix_sd +=
		    wrong_st_blocks(&sd_ih.ih_key, &blocks, sd_blocks, mode,
				    is_new_file);

		if (fix_sd) {
			if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
				struct buffer_head *bh;
				/* find stat data and correct it */
				set_type_and_offset(KEY_FORMAT_1, &sd_ih.ih_key,
						    SD_OFFSET, TYPE_STAT_DATA);
				if (reiserfs_search_by_key_4
				    (fs, &sd_ih.ih_key, path) != ITEM_FOUND) {
					fsck_log
					    ("check_regular_file: A StatData of the file %K cannot be "
					     "found\n", &sd_ih.ih_key);
					one_more_corruption(fs, FATAL);
					return STAT_DATA_NOT_FOUND;
				}

				bh = get_bh(path);
				ih = tp_item_head(path);
				sd = tp_item_body(path);
				set_sd_size(ih, sd, &real_size);
				set_sd_blocks(ih, sd, &blocks);
				set_sd_mode(ih, sd, &mode);
				if (!is_new_file)
					set_sd_first_direct_byte(ih, sd,
								 &first_direct_byte);
				mark_buffer_dirty(bh);
			} else {
				fsck_check_stat(fs)->fixable_corruptions +=
				    fix_sd;
			}
		}
	}
	return retval;
}

/* returns buffer, containing found directory item.*/
static char *get_next_directory_item(struct reiserfs_key *key,	/* on return this will contain key of next item in the tree */
				     const struct reiserfs_key *parent,
				     struct item_head *ih, __u32 * pos_in_item,
				     int dir_format)
{
	INITIALIZE_REISERFS_PATH(path);
	char *dir_item;
	struct reiserfs_key *rdkey;
	struct buffer_head *bh;
	struct reiserfs_de_head *deh;
	int i;
	int retval;

start_again:

	retval = reiserfs_search_by_entry_key(fs, key, &path);

	if (retval != POSITION_FOUND && get_offset(key) != DOT_OFFSET)
		reiserfs_panic
		    ("get_next_directory_item: The current directory %k cannot be found",
		     key);

	/* leaf containing directory item */
	bh = PATH_PLAST_BUFFER(&path);
	*pos_in_item = path.pos_in_item;
	*ih = *tp_item_head(&path);
	deh = B_I_DEH(bh, ih);

	/* position was not found for '.' or there is no '..' */
	if (retval != POSITION_FOUND || ((get_offset(key) == DOT_OFFSET) &&
					 (get_ih_entry_count(ih) < 2
					  || name_in_entry_length(ih, deh + 1,
								  1) != 2
					  || strncmp(name_in_entry(deh + 1, 1),
						     "..", 2)))) {

		fsck_log
		    ("get_next_directory_item: The %s %k cannot be found in %k",
		     (retval == POSITION_NOT_FOUND) ? "entry" : "directory",
		     key, &ih->ih_key);

		if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
			/* add "." and ".." exist */
			pathrelse(&path);
			reiserfs_add_entry(fs, key, ".",
					   name_length(".", dir_format), key,
					   0);
			reiserfs_add_entry(fs, key, "..",
					   name_length("..", dir_format),
					   parent, 0);
			fsck_log(" - entry was added\n");
			goto start_again;
		} else {
			one_more_corruption(fs, FIXABLE);
			fsck_log("\n");
			if (retval == DIRECTORY_NOT_FOUND)
				return NULL;
		}
	}

	/* make sure, that ".." exists as well */
/*
    if (get_offset (key) == DOT_OFFSET) {
	if (get_ih_entry_count (ih) < 2 ||
	    name_in_entry_length (ih, deh + 1, 1) != 2 ||
	    strncmp (name_in_entry (deh + 1, 1), "..", 2))
	{
	    fsck_log ("get_next_directory_item: \"..\" not found in %H\n", ih);
	    pathrelse (&path);
	    return 0;
	}
    }
*/
	/* mark hidden entries as visible, set "." and ".." correctly */
	deh += *pos_in_item;
	for (i = *pos_in_item; i < get_ih_entry_count(ih); i++, deh++) {
/*	int namelen;
	char * name;

	name = name_in_entry (deh, i);
	namelen = name_in_entry_length (ih, deh, i);
	if (de_hidden (deh)) // handled in check_tree
	    reiserfs_panic ("get_next_directory_item: item %k: hidden entry %d \'%.*s\'\n",
			    key, i, namelen, name);
*/

		if (get_deh_offset(deh) == DOT_OFFSET) {
			if (not_of_one_file(&(deh->deh2_dir_id), key)) {
				/* "." must point to the directory it is in */

				//deh->deh_objectid != REISERFS_ROOT_PARENT_OBJECTID)/*????*/ {
				fsck_log
				    ("get_next_directory_item: The entry \".\" of the directory %K "
				     "pointes to %K, instead of %K", key,
				     (struct reiserfs_key
				      *)(&(deh->deh2_dir_id)), key);
				if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
					set_deh_dirid(deh, get_key_dirid(key));
					set_deh_objectid(deh,
							 get_key_objectid(key));
					mark_buffer_dirty(bh);
					fsck_log(" - corrected\n");
				} else {
					one_more_corruption(fs, FIXABLE);
					fsck_log("\n");
				}
			}
		}

		if (get_deh_offset(deh) == DOT_DOT_OFFSET) {
			/* set ".." so that it points to the correct parent directory */
			if (comp_short_keys(&(deh->deh2_dir_id), parent)) {
				fsck_log
				    ("get_next_directory_item: The entry \"..\" of the directory %K "
				     "pointes to %K, instead of %K", key,
				     (struct reiserfs_key
				      *)(&(deh->deh2_dir_id)));
				if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
					set_deh_dirid(deh,
						      get_key_dirid(parent));
					set_deh_objectid(deh,
							 get_key_objectid
							 (parent));
					mark_buffer_dirty(bh);
					fsck_log(" - corrected\n");
				} else {
					one_more_corruption(fs, FIXABLE);
					fsck_log("\n");
				}
			}
		}
	}

	/* copy directory item to the temporary buffer */
	dir_item = getmem(get_ih_item_len(ih));
	memcpy(dir_item, ih_item_body(bh, ih), get_ih_item_len(ih));

	/* next item key */
	if (PATH_LAST_POSITION(&path) == (B_NR_ITEMS(bh) - 1) &&
	    (rdkey = uget_rkey(&path)))
		copy_key(key, rdkey);
	else {
		set_key_dirid(key, 0);
		set_key_objectid(key, 0);
	}

	if (fsck_mode(fs) == FSCK_REBUILD)
		mark_item_reachable(tp_item_head(&path), bh);
	pathrelse(&path);

	return dir_item;
}

/* semantic pass of --check */
static int check_semantic_pass(struct reiserfs_key *key,
			       const struct reiserfs_key *parent, int dot_dot,
			       struct item_head *new_ih)
{
	struct reiserfs_path path;
	void *sd;
	__u32 nlink;
	int is_new_dir;
	struct buffer_head *bh;
	struct item_head *ih;
	int retval;
	char *dir_item;
	__u32 pos_in_item;
	struct item_head tmp_ih;
	struct reiserfs_key next_item_key, entry_key, object_key;
	__u64 dir_size = 0;
	__u32 blocks;
	__u64 sd_size;
	__u32 sd_blocks;
	int fix_sd;
	/*int relocate; */
	int dir_format = 0;
	__u16 mode;

	retval = OK;

	/* start_again: when directory was relocated */

	if (!KEY_IS_STAT_DATA_KEY(key)) {
		fsck_log
		    ("check_semantic_pass: The key %k must be key of a StatData\n",
		     key);
		one_more_corruption(fs, FATAL);
		return STAT_DATA_NOT_FOUND;
	}

	/* look for stat data of an object */
	if (reiserfs_search_by_key_4(fs, key, &path) == ITEM_NOT_FOUND) {
		pathrelse(&path);
		return STAT_DATA_NOT_FOUND;
	}

	/* stat data has been found */
	ih = tp_item_head(&path);
	sd = tp_item_body(&path);

	get_sd_nlink(ih, sd, &nlink);

	/* It seems quite difficult to relocate objects on fix-fixable -
	 * rewrite_file calls reiserfs_file_write which can convert tails
	 * to unfm, plus unreachable, was_tail flags, etc. */
#if 0
	if (( /* relocate = */ should_be_relocated(&ih->ih_key))) {
		/*
		   if (fsck_mode(fs) == FSCK_CHECK)
		   relocate = 0;
		 */
		one_more_corruption(fs, FATAL);
	}
#endif

	if (fix_obviously_wrong_sd_mode(&path)) {
		one_more_corruption(fs, FIXABLE);
		pathrelse(&path);
		return OK;
	}

	if (nlink == 0) {
		fsck_log
		    ("%s: block %lu: The StatData %k has bad nlink number (%u)\n",
		     __FUNCTION__, get_bh(&path)->b_blocknr, &ih->ih_key,
		     nlink);
		one_more_corruption(fs, FATAL);
	}

	if (not_a_directory(sd)) {
		fsck_check_stat(fs)->files++;

		retval =
		    check_check_regular_file(&path, sd,
					     /* relocate ? new_ih : */ NULL);
		pathrelse(&path);
		return retval;
	}

/*
    if (relocate) {
	if (!new_ih)
	    reiserfs_panic ("check_semantic_pass: Memory is not prepared for relocation of "
		"%K", &ih->ih_key);
	*new_ih = *ih;
	pathrelse (&path);
	sem_pass_stat (fs)->oid_sharing_dirs_relocated ++;
	relocate_dir (new_ih, 1);
	linked_already(&new_ih->ih_key);
	one_less_corruption (fs, FIXABLE);
	*key = new_ih->ih_key;
	retval = RELOCATED;
	goto start_again;
    }
*/

/*
    if (fsck_mode (fs) == FSCK_FIX_FIXABLE) {
    	// it looks like stat data of a directory found
    	if (nlink) {
	    // we saw this directory already
	    if (!dot_dot) {
	    	// this name is not ".."  - and hard links are not allowed on directories
	    	pathrelse (&path);
	    	return STAT_DATA_NOT_FOUND;
	    } else {
	    	// ".." found
	    	nlink ++;
	    	set_sd_nlink (ih, sd, &nlink);
	    	mark_buffer_dirty (get_bh (&path));
	    	pathrelse (&path);
	    	return OK;
	    }
    	} // do not run it for dot_dot on check at all
    	
     	nlink = 2;
    	if (get_key_objectid (key) == REISERFS_ROOT_OBJECTID)
	    nlink ++;
    	set_sd_nlink (ih, sd, &nlink);
    	mark_buffer_dirty (get_bh (&path));
    }
*/

	/* directory stat data found */
	if (get_ih_item_len(ih) == SD_SIZE)
		is_new_dir = 1;
	else
		is_new_dir = 0;

	/* save stat data's size and st_blocks */
	get_sd_size(ih, sd, &sd_size);
	get_sd_blocks(ih, sd, &sd_blocks);
	get_sd_mode(ih, sd, &mode);

	dir_format =
	    (get_ih_item_len(tp_item_head(&path)) ==
	     SD_SIZE) ? KEY_FORMAT_2 : KEY_FORMAT_1;

	/* release path pointing to stat data */
	pathrelse(&path);

	fsck_check_stat(fs)->dirs++;

	set_key_dirid(&next_item_key, get_key_dirid(key));
	set_key_objectid(&next_item_key, get_key_objectid(key));
	set_key_offset_v1(&next_item_key, DOT_OFFSET);
	set_key_uniqueness(&next_item_key, DIRENTRY_UNIQUENESS);

	dir_size = 0;
	while ((dir_item =
		get_next_directory_item(&next_item_key, parent, &tmp_ih,
					&pos_in_item, dir_format)) != NULL) {
		/* dir_item is copy of the item in separately allocated memory,
		   item_key is a key of next item in the tree */
		int i;
		char *name = NULL;
		int namelen, entry_len;
		struct reiserfs_de_head *deh =
		    (struct reiserfs_de_head *)dir_item + pos_in_item;

		for (i = pos_in_item; i < get_ih_entry_count(&tmp_ih);
		     i++, deh++) {
			struct item_head relocated_ih;
			int ret = OK;

			if (name) {
				free(name);
				name = NULL;
			}

			namelen = name_in_entry_length(&tmp_ih, deh, i);
			asprintf(&name, "%.*s", namelen, name_in_entry(deh, i));
			entry_len = entry_length(&tmp_ih, deh, i);

			get_object_key(deh, &object_key, &entry_key, &tmp_ih);

			if ((dir_format == KEY_FORMAT_2)
			    && (entry_len % 8 != 0)) {
				/* not alighed directory of new format - delete it */
				if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
					fsck_log
					    ("Entry %K (\"%.*s\") in the directory %K is not formated "
					     "properly - deleted\n",
					     (struct reiserfs_key *)&(deh->
								      deh2_dir_id),
					     namelen, name, &tmp_ih.ih_key);
					reiserfs_remove_entry(fs, &entry_key);
					entry_len =
					    name_length(name, dir_format);
					reiserfs_add_entry(fs, key, name,
							   entry_len,
							   (struct reiserfs_key
							    *)&(deh->
								deh2_dir_id),
							   0);
				} else {
					fsck_log
					    ("Entry %K (\"%.*s\") in the directory %K is not formated "
					     "properly.\n",
					     (struct reiserfs_key *)&(deh->
								      deh2_dir_id),
					     namelen, name, &tmp_ih.ih_key);
					one_more_corruption(fs, FIXABLE);
				}
			}

			print_name(name, namelen);

			if (!is_properly_hashed
			    (fs, name, namelen, get_deh_offset(deh))) {
				one_more_corruption(fs, FATAL);
				fsck_log
				    ("check_semantic_pass: Hash mismatch detected for (%.*s) in "
				     "directory %K\n", namelen, name,
				     &tmp_ih.ih_key);
			}

			if (is_dot(name, namelen)
			    || (is_dot_dot(name, namelen))) {
				/* do not go through "." and ".." */
				ret = OK;
			} else {
				if ((ret = add_path_key(&object_key)) == 0) {
					ret =
					    check_semantic_pass(&object_key,
								key,
								is_dot_dot(name,
									   namelen),
								&relocated_ih);
					del_path_key();
				}
			}

			erase_name(namelen);

			/* check what check_semantic_tree returned */
			switch (ret) {
			case OK:
				dir_size += DEH_SIZE + entry_len;
				break;

			case STAT_DATA_NOT_FOUND:
				fsck_log
				    ("check_semantic_pass: Name \"%.*s\" in directory %K points to "
				     "nowhere\n", namelen, name,
				     &tmp_ih.ih_key);
			case LOOP_FOUND:
				if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
					reiserfs_remove_entry(fs, &entry_key);
					fsck_log(" - removed");
				} else {
					one_more_corruption(fs, FIXABLE);
				}
				fsck_log("\n");
				break;

			case DIRECTORY_HAS_NO_ITEMS:
				fsck_log
				    ("check_semantic_pass: Name \"%.*s\" in directory %K points a "
				     "directory without body\n", namelen, name,
				     &tmp_ih.ih_key);

				/* fixme: stat data should be deleted as well */
				/*
				   if (fsck_fix_fixable (fs)) {
				   reiserfs_remove_entry (fs, &entry_key);
				   fsck_data(fs)->deleted_entries ++;
				   fsck_log (" - removed");
				   }
				   fsck_log ("\n"); */
				break;

			case RELOCATED:
				/* file was relocated, update key in corresponding directory entry */
				if (reiserfs_search_by_entry_key
				    (fs, &entry_key, &path) != POSITION_FOUND) {
					fsck_log
					    ("Cannot find a name of the relocated file %K in the directory "
					     "%K\n", &entry_key,
					     &tmp_ih.ih_key);
				} else {
					/* update key dir entry points to */
					struct reiserfs_de_head *tmp_deh;

					tmp_deh =
					    B_I_DEH(get_bh(&path),
						    tp_item_head(&path)) +
					    path.pos_in_item;
					fsck_log
					    ("The directory %K pointing to %K (\"%.*s\") updated to point "
					     "to ", &tmp_ih.ih_key,
					     &tmp_deh->deh2_dir_id, namelen,
					     name);
					set_deh_dirid(tmp_deh,
						      get_key_dirid
						      (&relocated_ih.ih_key));
					set_deh_objectid(tmp_deh,
							 get_key_objectid
							 (&relocated_ih.
							  ih_key));

					fsck_log("%K (\"%.*s\")\n",
						 &tmp_deh->deh2_dir_id, namelen,
						 name);
					mark_buffer_dirty(get_bh(&path));
				}
				dir_size += DEH_SIZE + entry_len;
				pathrelse(&path);
				break;
			}
		}		/* for */

		freemem(dir_item);
		free(name);
		name = NULL;

		if (not_of_one_file(&next_item_key, key))
			/* next key is not of this directory */
			break;

	}			/* while (dir_item) */

	if (dir_size == 0)
		/* FIXME: is it possible? */
		return DIRECTORY_HAS_NO_ITEMS;

	/* calc correct value of sd_blocks field of stat data */
	blocks = dir_size2st_blocks(dir_size);

	fix_sd = 0;
	fix_sd += wrong_st_blocks(key, &blocks, sd_blocks, mode, is_new_dir);
	fix_sd +=
	    wrong_st_size(key, is_new_dir ? MAX_FILE_SIZE_V2 : MAX_FILE_SIZE_V1,
			  fs->fs_blocksize, &dir_size, sd_size, TYPE_DIRENTRY);

	if (fix_sd) {
		if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
			/* we have to fix either sd_size or sd_blocks, so look for stat data again */
			if (reiserfs_search_by_key_4(fs, key, &path) !=
			    ITEM_FOUND) {
				fsck_log
				    ("check_semantic_tree: The StatData of the file %K was not found\n",
				     key);
				one_more_corruption(fs, FATAL);
				return STAT_DATA_NOT_FOUND;
			}

			bh = get_bh(&path);
			ih = tp_item_head(&path);
			sd = tp_item_body(&path);

			set_sd_size(ih, sd, &dir_size);
			set_sd_blocks(ih, sd, &blocks);
			mark_buffer_dirty(bh);
			pathrelse(&path);
		} else {
			fsck_check_stat(fs)->fixable_corruptions += fix_sd;
		}
	}

	return retval;
}

static int check_safe_links(void)
{
	struct reiserfs_path safe_link_path, path;
	struct reiserfs_key safe_link_key = { cpu_to_le32(-1), 0, {{0, 0}} };
	struct reiserfs_key key = { 0, 0, {{0, 0}} };
	struct reiserfs_key *rkey;
	struct item_head *tmp_ih;

	while (1) {
		if (get_key_dirid(&safe_link_key) == 0)
			break;

		reiserfs_search_by_key_4(fs, &safe_link_key, &safe_link_path);

		if (get_blkh_nr_items(B_BLK_HEAD(get_bh(&safe_link_path))) <=
		    PATH_LAST_POSITION(&safe_link_path)) {
			pathrelse(&safe_link_path);
			break;
		}

		tmp_ih = tp_item_head(&safe_link_path);

		if (get_key_dirid(&tmp_ih->ih_key) != (__u32) - 1 ||
		    get_key_objectid(&tmp_ih->ih_key) == (__u32) - 1) {
			pathrelse(&safe_link_path);
			break;
		}

		if (get_ih_item_len(tmp_ih) != 4)
			reiserfs_panic("Safe Link %k cannot be of the size %d",
				       &tmp_ih->ih_key,
				       get_ih_item_len(tmp_ih));

		set_key_dirid(&key,
			      d32_get((__le32 *) tp_item_body(&safe_link_path), 0));
		set_key_objectid(&key, get_key_objectid(&tmp_ih->ih_key));
		if ((rkey = reiserfs_next_key(&safe_link_path)) == NULL)
			set_key_dirid(&safe_link_key, 0);
		else
			safe_link_key = *rkey;

		if (reiserfs_search_by_key_4(fs, &key, &path) == ITEM_NOT_FOUND) {
			/*sware on check, delete on fix-fixable */
			if (fsck_mode(fs) == FSCK_CHECK) {
				fsck_log
				    ("Invalid safe link %k: cannot find the pointed object (%K)\n",
				     &tmp_ih->ih_key, &key);
				one_more_corruption(fs, FIXABLE);
			} else if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
				fsck_log
				    ("Invalid safe link %k: cannot find the pointed object (%K) - "
				     "safe link was deleted\n", &tmp_ih->ih_key,
				     &key);
				d32_put((__le32 *) tp_item_body(&safe_link_path), 0,
					0);
				pathrelse(&path);
				reiserfsck_delete_item(&safe_link_path, 0);
				continue;
			}
		} else if (get_offset(&tmp_ih->ih_key) == 0x1) {
			/* Truncate */
			if (!not_a_directory(tp_item_body(&path))) {
				/*truncate on directory should not happen */
				/*sware on check, delete on fix-fixable */
				if (fsck_mode(fs) == FSCK_CHECK) {
					fsck_log
					    ("Invalid 'truncate' safe link %k, cannot happen for "
					     "directory (%K)\n",
					     &tmp_ih->ih_key, &key);
					one_more_corruption(fs, FIXABLE);
				} else if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
					fsck_log
					    ("Invalid 'truncate' safe link %k, cannot happen for "
					     "a directory (%K) - safe link was deleted\n",
					     &tmp_ih->ih_key, &key);
					d32_put((__le32 *)
						tp_item_body(&safe_link_path), 0,
						0);
					pathrelse(&path);
					reiserfsck_delete_item(&safe_link_path,
							       0);
					continue;
				}
			} else {
				/* save 'safe truncate links' to avoid swaring on wrong sizes. */
				__u32 position;

				if (reiserfs_bin_search
				    (&key, trunc_links, links_num, sizeof(key),
				     &position,
				     comp_short_keys) != POSITION_FOUND) {
					blocklist__insert_in_position(&key,
								      (void *)
								      &trunc_links,
								      &links_num,
								      sizeof
								      (key),
								      &position);
				}
			}
		}
		pathrelse(&path);
		pathrelse(&safe_link_path);
	}

	return OK;
}

static void release_safe_links(void)
{
	freemem(trunc_links);
}

/* called when --check is given */
void semantic_check(void)
{
	if (fsck_data(fs)->check.bad_nodes) {
		fsck_progress("Bad nodes were found, Semantic pass skipped\n");
		goto clean;
	}

	if (fsck_data(fs)->check.fatal_corruptions) {
		fsck_progress
		    ("Fatal corruptions were found, Semantic pass skipped\n");
		goto clean;
	}

	fsck_progress("Checking Semantic tree:\n");

	if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
		/*create new_bitmap, and initialize new_bitmap & allocable bitmap */
		fsck_new_bitmap(fs) =
		    reiserfs_create_bitmap(get_sb_block_count
					   (fs->fs_ondisk_sb));
		reiserfs_bitmap_copy(fsck_new_bitmap(fs), fs->fs_bitmap2);
		fsck_allocable_bitmap(fs) =
		    reiserfs_create_bitmap(get_sb_block_count
					   (fs->fs_ondisk_sb));
		reiserfs_bitmap_copy(fsck_allocable_bitmap(fs), fs->fs_bitmap2);
		fs->block_allocator = reiserfsck_reiserfs_new_blocknrs;
		fs->block_deallocator = reiserfsck_reiserfs_free_block;
	}

	check_safe_links();

	if (check_semantic_pass(&root_dir_key, &parent_root_dir_key, 0, NULL) !=
	    OK) {
		fsck_log("check_semantic_tree: No root directory found");
		one_more_corruption(fs, FATAL);
	}

	release_safe_links();

	if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
		reiserfs_delete_bitmap(fs->fs_bitmap2);
		reiserfs_bitmap_delta(fsck_new_bitmap(fs),
				      fsck_deallocate_bitmap(fs));
		fs->fs_bitmap2 = fsck_new_bitmap(fs);
		reiserfs_delete_bitmap(fsck_allocable_bitmap(fs));
		fsck_allocable_bitmap(fs) = NULL;
		set_sb_free_blocks(fs->fs_ondisk_sb,
				   reiserfs_bitmap_zeros(fs->fs_bitmap2));
		mark_buffer_dirty(fs->fs_super_bh);
		add_badblock_list(fs, 1);
	}

	fsck_progress("finished\n");

clean:
	if (fsck_deallocate_bitmap(fs))
		reiserfs_delete_bitmap(fsck_deallocate_bitmap(fs));
}
