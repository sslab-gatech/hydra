/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

void erase_name(int len);
void print_name(char *name, int len);

void modify_item(struct item_head *ih, void *item);

/* fixme: search_by_key is not needed after any add_entry */
static __u64 _look_for_lost(reiserfs_filsys_t fs, int link_lost_dirs)
{
	struct reiserfs_key key, prev_key, *rdkey;
	INITIALIZE_REISERFS_PATH(path);
	int item_pos;
	struct buffer_head *bh;
	struct item_head *ih;
	unsigned long leaves;
	int is_it_dir;
	static int lost_files = 0;	/* looking for lost dirs we calculate amount of
					   lost files, so that when we will look for
					   lost files we will be able to stop when
					   there are no lost files anymore */
	int retval;
	__u64 size;

	key = root_dir_key;

	if (!link_lost_dirs && !lost_files) {
		/* we have to look for lost files but we know already that there are
		   no any */
		return 0;
	}

	fsck_progress("Looking for lost %s:\n",
		      link_lost_dirs ? "directories" : "files");
	leaves = 0;

	/* total size of added entries */
	size = 0;
	while (1) {
		retval = reiserfs_search_by_key_4(fs, &key, &path);
		/* fixme: we assume path ends up with a leaf */
		bh = get_bh(&path);
		item_pos = get_item_pos(&path);
		if (retval != ITEM_FOUND) {
			if (item_pos == get_blkh_nr_items(B_BLK_HEAD(bh))) {
				rdkey = uget_rkey(&path);
				if (!rdkey) {
					pathrelse(&path);
					break;
				}
				key = *rdkey;
				pathrelse(&path);
				continue;
			}
			/* we are on the item in the buffer */
		}

		/* print ~ how many leaves were scanned and how fast it was */
		if (!fsck_quiet(fs))
			print_how_fast(leaves++, 0, 50, 0);

		for (ih = tp_item_head(&path);
		     item_pos < get_blkh_nr_items(B_BLK_HEAD(bh));
		     item_pos++, ih++, PATH_LAST_POSITION(&path)++) {
			if (is_item_reachable(ih))
				continue;

			/* found item which can not be reached */
			if (!is_direntry_ih(ih) && !is_stat_data_ih(ih)) {
				continue;
			}

			if (is_direntry_ih(ih)) {
				/* if this directory has no stat data - try to recover it */
				struct reiserfs_key sd;
				struct reiserfs_path tmp;

				sd = ih->ih_key;
				set_type_and_offset(KEY_FORMAT_1, &sd,
						    SD_OFFSET, TYPE_STAT_DATA);
				if (reiserfs_search_by_key_4(fs, &sd, &tmp) ==
				    ITEM_FOUND) {
					/* should not happen - because if there were a stat data -
					   we would have done with the whole directory */
					pathrelse(&tmp);
					continue;
				}
				lost_found_pass_stat(fs)->dir_recovered++;
				create_dir_sd(fs, &tmp, &sd, modify_item);
				id_map_mark(proper_id_map(fs),
					    get_key_objectid(&sd));
				key = sd;
				pathrelse(&path);
				goto cont;
			}

			/* stat data marked "not having name" found */
			if (is_stat_data_ih(tp_item_head(&path)))
				fix_obviously_wrong_sd_mode(&path);

			is_it_dir =
			    ((not_a_directory(ih_item_body(bh, ih))) ? 0 : 1);

			if (is_it_dir) {
				struct reiserfs_key tmp_key;
				INITIALIZE_REISERFS_PATH(tmp_path);
				struct item_head *tmp_ih;

				/* there is no need to link empty lost directories into /lost+found */
				tmp_key = ih->ih_key;
				set_type_and_offset(KEY_FORMAT_1, &tmp_key,
						    0xffffffff, TYPE_DIRENTRY);
				reiserfs_search_by_key_4(fs, &tmp_key,
							 &tmp_path);
				tmp_ih = tp_item_head(&tmp_path);
				tmp_ih--;
				if (not_of_one_file(&tmp_key, tmp_ih))
					reiserfs_panic("not directory found");
				if (!is_direntry_ih(tmp_ih) ||
				    (get_deh_offset
				     (B_I_DEH(get_bh(&tmp_path), tmp_ih) +
				      get_ih_entry_count(tmp_ih) - 1) ==
				     DOT_DOT_OFFSET)) {
					/* last directory item is either stat data or empty
					   directory item - do not link this dir into lost+found */
					lost_found_pass_stat(fs)->
					    empty_lost_dirs++;
					pathrelse(&tmp_path);
					continue;
				}
				pathrelse(&tmp_path);
			}

			if (link_lost_dirs && !is_it_dir) {
				/* we are looking for directories and it is not a dir */
				lost_files++;
				continue;
			}

			lost_found_pass_stat(fs)->lost_found++;

			{
				struct reiserfs_key obj_key =
				    { 0, 0, {{0, 0},} };
				char *lost_name;
				struct item_head tmp_ih;

				/* key to continue */
				key = ih->ih_key;
				set_key_objectid(&key,
						 get_key_objectid(&key) + 1);

				tmp_ih = *ih;
				if (id_map_test
				    (semantic_id_map(fs),
				     get_key_objectid(&ih->ih_key))) {
					/* objectid is used, relocate an object */
					lost_found_pass_stat(fs)->oid_sharing++;

					if (is_it_dir) {
						relocate_dir(&tmp_ih, 1);
						lost_found_pass_stat(fs)->
						    oid_sharing_dirs_relocated++;
					} else {
						rewrite_file(&tmp_ih, 1, 1);
						lost_found_pass_stat(fs)->
						    oid_sharing_files_relocated++;
					}

					linked_already(&tmp_ih.ih_key);
				} else {
					if (!is_it_dir)
						id_map_mark(semantic_id_map(fs),
							    get_key_objectid
							    (&ih->ih_key));
				}

				asprintf(&lost_name, "%u_%u",
					 get_key_dirid(&tmp_ih.ih_key),
					 get_key_objectid(&tmp_ih.ih_key));

				/* entry in lost+found directory will point to this key */
				set_key_dirid(&obj_key,
					      get_key_dirid(&tmp_ih.ih_key));
				set_key_objectid(&obj_key,
						 get_key_objectid(&tmp_ih.
								  ih_key));

				pathrelse(&path);

				/* 0 does not mean anyting - item with "." and ".." already
				   exists and reached, so only name will be added */
				size +=
				    reiserfs_add_entry(fs, &lost_found_dir_key,
						       lost_name,
						       name_length(lost_name,
								   lost_found_dir_format),
						       &obj_key,
						       0 /*fsck_need */ );

				if (is_it_dir) {
					/* fixme: we hope that if we will try to pull all the
					   directory right now - then there will be less
					   lost_found things */
					print_name(lost_name,
						   strlen(lost_name));
					/*fsck_progress ("\tChecking lost dir \"%s\":", lost_name); */
					rebuild_semantic_pass(&obj_key,
							      &lost_found_dir_key,
							      /*dot_dot */ 0,
							      /*reloc_ih */ NULL);
					erase_name(strlen(lost_name));
					/*fsck_progress ("finished\n"); */

					lost_found_pass_stat(fs)->
					    lost_found_dirs++;
				} else {
					if (reiserfs_search_by_key_4
					    (fs, &obj_key, &path) != ITEM_FOUND)
						reiserfs_panic
						    ("look_for_lost: lost file stat data %K not found",
						     &obj_key);

					/* check_regular_file does not mark stat data reachable */
					mark_item_reachable(tp_item_head(&path),
							    get_bh(&path));

					rebuild_check_regular_file(&path,
								   tp_item_body
								   (&path),
								   NULL
								   /*reloc_ih */
								   );
					pathrelse(&path);

					lost_found_pass_stat(fs)->
					    lost_found_files++;
					lost_files--;
				}

				free(lost_name);
				goto cont;
			}
		}		/* for */

		prev_key = key;

		PATH_LAST_POSITION(&path) = item_pos - 1;
		rdkey = reiserfs_next_key(&path);
		if (rdkey)
			key = *rdkey;
		else
			break;

		if (comp_keys(&prev_key, &key) != -1)
			reiserfs_panic
			    ("pass_3a: key must grow 2: prev=%k next=%k",
			     &prev_key, &key);
		pathrelse(&path);

cont:
		if (!link_lost_dirs && !lost_files) {
			break;
		}
	}

	pathrelse(&path);

#if 0
	/* check names added we just have added to/lost+found. Those names are
	   marked DEH_Lost_found flag */
	fsck_progress("Checking lost+found directory..");
	fflush(stdout);
	check_semantic_tree(&lost_found_dir_key, &root_dir_key, 0,
			    1 /* lost+found */ );
	fsck_progress("finished\n");
#endif

	return size;

}

static void save_lost_found_result(reiserfs_filsys_t fs)
{
	FILE *file;
	int retval;

	/* save bitmaps with which we will be able start reiserfs from
	   pass 1 */
	file = open_file("temp_fsck_file.deleteme", "w+");
	if (!file)
		return;

	reiserfs_begin_stage_info_save(file, LOST_FOUND_DONE);
	reiserfs_end_stage_info_save(file);
	close_file(file);

	retval = rename("temp_fsck_file.deleteme", state_dump_file(fs));
	if (retval != 0)
		fsck_progress
		    ("pass 0: Could not rename the temporary file temp_fsck_file.deleteme to %s",
		     state_dump_file(fs));
}

/* we have nothing to load from a state file, but we have to fetch
   on-disk bitmap, copy it to allocable bitmap, and fetch objectid
   map */
void load_lost_found_result(reiserfs_filsys_t fs)
{
	fsck_new_bitmap(fs) =
	    reiserfs_create_bitmap(get_sb_block_count(fs->fs_ondisk_sb));
	reiserfs_bitmap_copy(fsck_new_bitmap(fs), fs->fs_bitmap2);

	fsck_allocable_bitmap(fs) =
	    reiserfs_create_bitmap(get_sb_block_count(fs->fs_ondisk_sb));
	reiserfs_bitmap_copy(fsck_allocable_bitmap(fs), fs->fs_bitmap2);

	fs->block_allocator = reiserfsck_reiserfs_new_blocknrs;
	fs->block_deallocator = reiserfsck_reiserfs_free_block;

	/* we need objectid map on semantic pass to be able to relocate files */
	proper_id_map(fs) = id_map_init();
	/* Not implemented yet.
	   fetch_objectid_map (proper_id_map (fs), fs);
	 */
}

static void after_lost_found(reiserfs_filsys_t fs)
{
	int ret;
	/* update super block: objectid map, fsck state */
	set_sb_fs_state(fs->fs_ondisk_sb, LOST_FOUND_DONE);
	mark_buffer_dirty(fs->fs_super_bh);

	/* write all dirty blocks */
	fsck_progress("Flushing..");
	id_map_flush(proper_id_map(fs), fs);
	fs->fs_dirt = 1;
	ret = reiserfs_flush_to_ondisk_bitmap(fsck_new_bitmap(fs), fs);
	if (ret < 0)
		reiserfs_exit(1, "Exiting after unrecoverable error.");
	reiserfs_flush(fs);
	fsck_progress("finished\n");

	stage_report(0x3a, fs);

	if (!fsck_run_one_step(fs)) {
		if (fsck_user_confirmed(fs, "Continue? (Yes):", "Yes\n", 1))
			/* reiserfsck continues */
			return;
	}

	save_lost_found_result(fs);

	id_map_free(proper_id_map(fs));
	proper_id_map(fs) = NULL;

	fs->fs_dirt = 1;
	reiserfs_close(fs);
	exit(EXIT_OK);
}

void pass_3a_look_for_lost(reiserfs_filsys_t fs)
{
	INITIALIZE_REISERFS_PATH(path);
	struct item_head *ih;
	void *sd;
	__u64 size, sd_size;
	__u32 blocks;
	__u16 mode;
	__u32 objectid;
	unsigned int gen_counter;
	fsck_progress("Pass 3a (looking for lost dir/files):\n");

	/* when warnings go not to stderr - separate them in the log */
	if (fsck_log_file(fs) != stderr)
		fsck_log("####### Pass 3a (lost+found pass) #########\n");

	/* look for lost dirs first */
	size = _look_for_lost(fs, 1);

	/* link files which are still lost */
	size += _look_for_lost(fs, 0);

	/* update /lost+found sd_size and sd_blocks (nlink is correct already) */

	objectid = reiserfs_find_entry(fs, &root_dir_key, "lost+found",
				       &gen_counter, &lost_found_dir_key);

	if (!objectid) {
		reiserfs_panic
		    ("look_for_lost: The entry 'lost+found' could not be found in the root directory.");
	}

	if (reiserfs_search_by_key_4(fs, &lost_found_dir_key, &path) !=
	    ITEM_FOUND)
		reiserfs_panic
		    ("look_for_lost: The StatData of the 'lost+found' directory %K could not be found",
		     &lost_found_dir_key);
	ih = tp_item_head(&path);
	sd = tp_item_body(&path);
	get_sd_size(ih, sd, &sd_size);
	size += sd_size;
	blocks = dir_size2st_blocks(size);

	set_sd_size(ih, sd, &size);
	set_sd_blocks(ih, sd, &blocks);

	/* make lost+found to be drwx------ */
	mode = S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR;
	set_sd_mode(ih, sd, &mode);

	mark_buffer_dirty(get_bh(&path));
	pathrelse(&path);

	after_lost_found(fs);
}
