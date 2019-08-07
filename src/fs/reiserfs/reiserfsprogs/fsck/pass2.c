/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

/* on pass2 we take leaves which could not be inserted into tree
   during pass1 and insert each item separately. It is possible that
   items of different objects with the same key can be found. We treat
   that in the following way: we put it into tree with new key and
   link it into /lost+found directory with name made of dir,oid. When
   coming item is a directory - we delete object from the tree, put it
   back with different key, link it to /lost+found directory and
   insert directory as it is */

/* relocation rules: we have an item (it is taken from "non-insertable"
   leaf). It has original key yet. We check to see if object with this
   key is remapped. Object can be only remapped if it is not a piece
   of directory */

/* in list of this structures we store what has been relocated. */
struct relocated {
	unsigned long old_dir_id;
	unsigned long old_objectid;

	unsigned long new_objectid;

	struct relocated *next;
};

/* all relocated files will be linked into lost+found directory at the
   beginning of semantic pass */
static struct relocated *relocated_list = NULL;

static __u32 get_relocated_objectid_from_list(const struct reiserfs_key *key)
{
	struct relocated *cur = relocated_list;

	while (cur) {
		if (cur->old_dir_id == get_key_dirid(key) &&
		    cur->old_objectid == get_key_objectid(key))
			/* object is relocated already */
			return cur->new_objectid;
		cur = cur->next;
	}
	return 0;
}

/* return objectid the object has to be remapped with */
__u32 objectid_for_relocation(const struct reiserfs_key * key)
{
	struct relocated *cur;
	__u32 cur_id;

	if ((cur_id = get_relocated_objectid_from_list(key)) != 0)
		return cur_id;

	cur = getmem(sizeof(struct relocated));
	cur->old_dir_id = get_key_dirid(key);
	cur->old_objectid = get_key_objectid(key);
	cur->new_objectid = id_map_alloc(proper_id_map(fs));
	cur->next = relocated_list;
	relocated_list = cur;
/*    fsck_log ("relocation: (%K) is relocated to (%lu, %lu)\n",
	      key, get_key_dirid (key), cur->new_objectid);*/
	return cur->new_objectid;
}

/* relocated files get added into lost+found with slightly different names */
static __u64 link_one(struct relocated *file)
{
	char *name;
	struct reiserfs_key obj_key;
	__u64 len = 0;

	asprintf(&name, "%lu,%lu", file->old_dir_id, file->new_objectid);
	set_key_dirid(&obj_key, file->old_dir_id);
	set_key_objectid(&obj_key, file->new_objectid);

	/* 0 for fsck_need does not mean too much - it would make effect if there
	 * were no this directory yet. But /lost_found is there already */
	len = reiserfs_add_entry(fs, &lost_found_dir_key, name,
				 name_length(name, lost_found_dir_format),
				 &obj_key, 0 /*fsck_need */ );
	pass_2_stat(fs)->relocated++;
	free(name);

	return len;
}

void linked_already(const struct reiserfs_key *new_key /*, link_func_t link_func */ )
{
	struct relocated *cur = relocated_list;
	struct relocated *prev = NULL;

	while (cur) {
		if (cur->old_dir_id == get_key_dirid(new_key) &&
		    cur->new_objectid == get_key_objectid(new_key))
			break;

		prev = cur;
		cur = cur->next;
	}

	if (cur) {
		/* len = link_func(cur); */

		if (prev)
			prev->next = cur->next;
		else
			relocated_list = cur->next;

		freemem(cur);
	}
}

void link_relocated_files(void)
{
	struct relocated *tmp;
	int count;

	count = 0;
	while (relocated_list) {
		link_one(relocated_list);
		tmp = relocated_list;
		relocated_list = relocated_list->next;
		freemem(tmp);
		count++;
	}
}

/* this item is in tree. All unformatted pointer are correct. Do not
   check them */
void save_item(struct si **head, struct item_head *ih, char *item,
	       __u32 blocknr)
{
	struct si *si, *cur;

	si = getmem(sizeof(*si));
	si->si_dnm_data = getmem(get_ih_item_len(ih));
	/*si->si_blocknr = blocknr; */
	memcpy(&(si->si_ih), ih, IH_SIZE);
	memcpy(si->si_dnm_data, item, get_ih_item_len(ih));

	if (*head == NULL)
		*head = si;
	else {
		cur = *head;
		while (cur->si_next)
			cur = cur->si_next;
		cur->si_next = si;
	}
	return;
}

struct si *save_and_delete_file_item(struct si *si, struct reiserfs_path *path)
{
	struct buffer_head *bh = PATH_PLAST_BUFFER(path);
	struct item_head *ih = tp_item_head(path);

	save_item(&si, ih, ih_item_body(bh, ih), bh->b_blocknr);

	/* delete item temporary - do not free unformatted nodes */
	reiserfsck_delete_item(path, 1 /*temporary */ );
	return si;
}

/* check whether there are any directory items with this key */
int should_relocate(struct item_head *ih)
{
	struct reiserfs_key key;
	const struct reiserfs_key *rkey;
	struct reiserfs_path path;
	struct item_head *path_ih;

	/* starting with the leftmost item with this key */
	key = ih->ih_key;
	set_type_and_offset(KEY_FORMAT_1, &key, SD_OFFSET, TYPE_STAT_DATA);

	while (1) {
		reiserfs_search_by_key_4(fs, &key, &path);

		if (get_item_pos(&path) == B_NR_ITEMS(get_bh(&path))) {
			rkey = uget_rkey(&path);
			if (rkey && !not_of_one_file(&key, rkey)) {
				/* file continues in the right neighbor */
				key = *rkey;
				pathrelse(&path);
				continue;
			}
			/* there is no more items with this key */
			pathrelse(&path);
			break;
		}

		path_ih = tp_item_head(&path);
		if (not_of_one_file(&key, &(path_ih->ih_key))) {
			/* there are no more item with this key */
			pathrelse(&path);
			break;
		}

		if (is_stat_data_ih(path_ih)) {
			fix_obviously_wrong_sd_mode(&path);
			if (ih_checked(path_ih)) {
				/* we have checked it already */
				pathrelse(&path);

				if (get_relocated_objectid_from_list
				    (&path_ih->ih_key))
					return 1;	/* it was relocated */
				break;
			} else {
				mark_ih_checked(path_ih);
				mark_buffer_dirty(get_bh(&path));
			}
		}

		/* ok, item found, but make sure that it is not a directory one */
		if ((is_stat_data_ih(path_ih)
		     && !not_a_directory(tp_item_body(&path)))
		    || (is_direntry_ih(path_ih))) {
			/* item of directory found. so, we have to relocate the file */
			pathrelse(&path);
			return 1;
		}
		key = path_ih->ih_key;
		set_offset(KEY_FORMAT_1, &key, get_offset(&key) + 1);
		pathrelse(&path);
	}
	return 0;
}

/* this works for both new and old stat data */
#define st_mode(sd) le16_to_cpu(((struct stat_data *)(sd))->sd_mode)

#define st_mtime_v1(sd) le32_to_cpu(((struct stat_data_v1 *)(sd))->sd_mtime)
#define st_mtime_v2(sd) le32_to_cpu(((struct stat_data *)(sd))->sd_mtime)

/* either both sd-s are new of both are old */
static void overwrite_stat_data(struct item_head *new_ih,
				void *new_item, struct reiserfs_path *path)
{
	__u16 new_mode, old_mode;

	get_sd_mode(new_ih, new_item, &new_mode);
	get_sd_mode(tp_item_head(path), tp_item_body(path), &old_mode);

	if (S_ISREG(new_mode) && !S_ISREG(old_mode)) {
		/* in tree we have not regular file - overwrite its stat data
		   with stat data of regular file */
		memcpy(tp_item_body(path), new_item, get_ih_item_len(tp_item_head(path)));
		mark_buffer_dirty(get_bh(path));
		return;
	}

	if (S_ISREG(old_mode) && !S_ISREG(new_mode)) {
		/* new stat data is not a stat data of regular file, keep
		   regular file stat data in tree */
		return;
	}

	/* if coming stat data has newer mtime - use that */
	if (stat_data_v1(new_ih)) {
		if (st_mtime_v1(new_item) > st_mtime_v1(tp_item_body(path))) {
			memcpy(tp_item_body(path), new_item, SD_V1_SIZE);
			mark_buffer_dirty(get_bh(path));
		}
	} else {
		if (st_mtime_v2(new_item) > st_mtime_v2(tp_item_body(path))) {
			memcpy(tp_item_body(path), new_item, SD_SIZE);
			mark_buffer_dirty(get_bh(path));
		}
	}
	return;
}

/* insert sd item if it does not exist, overwrite it otherwise */
static void put_sd_into_tree(struct item_head *new_ih, char *new_item)
{
	struct reiserfs_path path;

	if (!not_a_directory(new_item)) {
		/* new item is a stat data of a directory. So we have to
		   relocate all items which have the same short key and are of
		   not a directory */
		rewrite_file(new_ih, 1, 0 /*do not change new_ih */ );
	} else {
		/* new item is a stat data of something else but directory. If
		   there are items of directory - we have to relocate the file */
		if (should_relocate(new_ih))
			rewrite_file(new_ih, 1, 1 /*change new_ih */ );
	}

	/* if we will have to insert item into tree - it is ready */
	zero_nlink(new_ih, new_item);
	mark_item_unreachable(new_ih);

	/* we are sure now that if we are inserting stat data of a
	   directory - there are no items with the same key which are not
	   items of a directory, and that if we are inserting stat data is
	   of not a directory - it either has new key already or there are
	   no items with this key which are items of a directory */
	if (reiserfs_search_by_key_4(fs, &(new_ih->ih_key), &path) ==
	    ITEM_FOUND) {
		/* this stat data is found */
		if (get_ih_key_format(tp_item_head(&path)) !=
		    get_ih_key_format(new_ih)) {
			/* in tree stat data and a new one are of different
			   formats */
			fsck_log
			    ("put_sd_into_tree: Inserting the StatData %K, mode (%M)...",
			     &(new_ih->ih_key), st_mode(new_item));
			if (stat_data_v1(new_ih)) {
				/* sd to be inserted is of V1, where as sd in the tree
				   is of V2 */
				fsck_log
				    ("found newer in the tree, mode (%M), insersion was skipped.\n",
				     st_mode(tp_item_body(&path)));
				pathrelse(&path);
			} else {
				/* the stat data in the tree is sd_v1 */
				fsck_log
				    ("older sd, mode (%M), is replaced with it.\n",
				     st_mode(tp_item_body(&path)));
				reiserfsck_delete_item(&path,
						       0 /*not temporary */ );

				reiserfs_search_by_key_4(fs, &new_ih->ih_key,
							 &path);
				reiserfsck_insert_item(&path, new_ih, new_item);
			}
		} else {
			/* both stat data are of the same version */
			overwrite_stat_data(new_ih, new_item, &path);
			pathrelse(&path);
		}
		return;
	}

	/* item not found, insert a new one */
	reiserfsck_insert_item(&path, new_ih, new_item);
}

/* this tries to put each item entry to the tree, if there is no items
   of the directory, insert item containing 1 entry */
static void put_directory_item_into_tree(struct item_head *comingih, char *item)
{
	struct reiserfs_de_head *deh;
	int i;
	char *buf;
	char *name;
	int namelen;

	/* if there are anything with this key but a directory - move it
	   somewhere else */
	rewrite_file(comingih, 1, 0 /* do not change ih */ );

	deh = (struct reiserfs_de_head *)item;

	for (i = 0; i < get_ih_entry_count(comingih); i++, deh++) {
		name = name_in_entry(deh, i);
		namelen = name_in_entry_length(comingih, deh, i);

		if (!is_properly_hashed(fs, name, namelen, get_deh_offset(deh)))
			reiserfs_panic
			    ("put_directory_item_into_tree: The entry (%d) \"%.*s\" of the directory %k has"
			     " badly hashed entry", i, namelen, name,
			     &comingih->ih_key);

		asprintf(&buf, "%.*s", namelen, name);
		/* 1 for fsck is important: if there is no any items of this
		   directory in the tree yet - new item will be inserted
		   marked not reached */
		reiserfs_add_entry(fs, &(comingih->ih_key), buf,
				   entry_length(comingih, deh, i),
				   (struct reiserfs_key *)&(deh->deh2_dir_id),
				   1 << IH_Unreachable);
		free(buf);
	}
}

void insert_item_separately(struct item_head *ih, char *item, int was_in_tree)
{
	if (get_key_dirid(&ih->ih_key) == get_key_objectid(&ih->ih_key))
		reiserfs_panic
		    ("insert_item_separately: The item being inserted has the bad key %H",
		     ih);

	if (is_stat_data_ih(ih)) {
		put_sd_into_tree(ih, item);
	} else if (is_direntry_ih(ih)) {
		put_directory_item_into_tree(ih, item);
	} else {
		reiserfsck_file_write(ih, item, was_in_tree);
	}
}

static void put_stat_data_items(struct buffer_head *bh)
{
	int i;
	struct item_head *ih;

	ih = item_head(bh, 0);
	for (i = 0; i < B_NR_ITEMS(bh); i++, ih++) {

		/* this check instead of saved_items */
		if (!is_stat_data_ih(ih)
		    || is_bad_item(bh, ih, ih_item_body(bh, ih))) {
			continue;
		}
		insert_item_separately(ih, ih_item_body(bh, ih),
				       0 /*was in tree */ );
	}
}

static void put_not_stat_data_items(struct buffer_head *bh)
{
	int i;
	struct item_head *ih;

	ih = item_head(bh, 0);
	for (i = 0; i < B_NR_ITEMS(bh); i++, ih++) {

		if (is_stat_data_ih(ih)
		    || is_bad_item(bh, ih, ih_item_body(bh, ih))) {
			continue;
		}
		insert_item_separately(ih, ih_item_body(bh, ih),
				       0 /*was in tree */ );
	}
}

static void before_pass_2(reiserfs_filsys_t fs)
{
	/* anything specific for pass 2 ? */
}

static void save_pass_2_result(reiserfs_filsys_t fs)
{
	FILE *file;
	int retval;

	file = open_file("temp_fsck_file.deleteme", "w+");
	if (!file)
		return;

	/* to be able to restart from semantic we do not need to save
	   anything here, but two magic values */
	reiserfs_begin_stage_info_save(file, TREE_IS_BUILT);
	reiserfs_end_stage_info_save(file);
	close_file(file);
	retval = rename("temp_fsck_file.deleteme", state_dump_file(fs));
	if (retval != 0)
		fsck_progress
		    ("%s: Could not rename the temporary file temp_fsck_file.deleteme to %s",
		     __FUNCTION__, state_dump_file(fs));
}

/* we have nothing to load from a state file, but we have to fetch
   on-disk bitmap, copy it to allocable bitmap, and fetch objectid
   map */
void load_pass_2_result(reiserfs_filsys_t fs)
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

/* uninsertable blocks are marked by 0s in uninsertable_leaf_bitmap
   during the pass 1. They must be not in the tree */
static void do_pass_2(reiserfs_filsys_t fs)
{

	struct buffer_head *bh;
	unsigned long j;
	int i, what_node;
	unsigned long done = 0, total;

	total = reiserfs_bitmap_zeros(fsck_uninsertables(fs)) * 2;
	if (!total)
		return;

	fsck_progress("\nPass 2:\n");

	for (i = 0; i < 2; i++) {
		j = 0;
		while ((j < fsck_uninsertables(fs)->bm_bit_size) &&
		       reiserfs_bitmap_find_zero_bit(fsck_uninsertables(fs),
						     &j) == 0) {
			bh = bread(fs->fs_dev, j, fs->fs_blocksize);
			if (bh == NULL) {
				fsck_log
				    ("pass_2: Reading of the block (%lu) failed on the device 0x%x\n",
				     j, fs->fs_dev);
				goto cont;
			}

			if (is_block_used(bh->b_blocknr)
			    && !(block_of_journal(fs, bh->b_blocknr)
				 && fsck_data(fs)->rebuild.use_journal_area)) {
				fsck_log
				    ("%s: The block (%lu) is in the tree already. Should not happen.\n",
				     __FUNCTION__, bh->b_blocknr);
				goto cont;
			}
			/* this must be leaf */
			what_node = who_is_this(bh->b_data, bh->b_size);
			if (what_node != THE_LEAF) {	// || B_IS_KEYS_LEVEL(bh)) {
				fsck_log
				    ("%s: The block (%b), marked as a leaf on the first two passes, "
				     "is not a leaf! Will be skipped.\n",
				     __FUNCTION__, bh);
				goto cont;
			}
/*	
	    fsck_log ("block %lu is being inserted\n", bh->b_blocknr);
	    check_buffers_mem(fsck_log_file (fs));
	    fflush(fsck_log_file (fs));
*/
			if (i) {
				/* insert all not SD items */
				put_not_stat_data_items(bh);
				pass_2_stat(fs)->leaves++;
				make_allocable(j);
			} else
				/* insert SD items only */
				put_stat_data_items(bh);

			print_how_far(fsck_progress_file(fs), &done, total, 1,
				      fsck_quiet(fs));
cont:
			brelse(bh);
			j++;
		}
	}

	fsck_progress("\n");
}

static void after_pass_2(reiserfs_filsys_t fs)
{
	time_t t;
	int ret;

	/* we can now flush new_bitmap on disk as tree is built and
	   contains all data, which were found on dik at start in
	   used bitmaps */
	reiserfs_bitmap_copy(fs->fs_bitmap2, fsck_new_bitmap(fs));

	/* we should copy new_bitmap to allocable bitmap, becuase evth what is used
	   for now (marked as used in new_bitmap) should not be allocablel;
	   and what is not in tree for now should be allocable.
	   these bitmaps differ because on pass2 we skip those blocks, whose SD's
	   are not in the tree, and therefore indirect items of such bad leaves points
	   to not used and not allocable blocks.
	 */

	/* DEBUG only */
/*    if (reiserfs_bitmap_compare (fsck_allocable_bitmap (fs), fsck_new_bitmap(fs))) {
        fsck_log ("Allocable bitmap differs from the new bitmap after pass2\n");
	reiserfs_bitmap_copy (fsck_allocable_bitmap(fs), fsck_new_bitmap (fs));
    }
*/

	/* update super block: objectid map, fsck state */
	set_sb_fs_state(fs->fs_ondisk_sb, TREE_IS_BUILT);
	mark_buffer_dirty(fs->fs_super_bh);

	/* write all dirty blocks */
	fsck_progress("Flushing..");
	id_map_flush(proper_id_map(fs), fs);
	fs->fs_dirt = 1;
	ret = reiserfs_flush_to_ondisk_bitmap(fs->fs_bitmap2, fs);
	if (ret < 0)
		reiserfs_exit(1, "Exiting after unrecoverable error.");
	reiserfs_flush(fs);
	fsck_progress("finished\n");

	/* fixme: should be optional */
/*    fsck_progress ("Tree is built. Checking it - ");
    reiserfsck_check_pass1 ();
    fsck_progress ("finished\n");*/

	stage_report(2, fs);

	/* free what we do not need anymore */
	reiserfs_delete_bitmap(fsck_uninsertables(fs));

	if (!fsck_run_one_step(fs)) {
		if (fsck_user_confirmed(fs, "Continue? (Yes):", "Yes\n", 1))
			/* reiserfsck continues */
			return;
	} else
		save_pass_2_result(fs);

	id_map_free(proper_id_map(fs));
	proper_id_map(fs) = NULL;

	reiserfs_delete_bitmap(fsck_new_bitmap(fs));
	reiserfs_delete_bitmap(fsck_allocable_bitmap(fs));

	time(&t);
	fsck_progress("###########\n"
		      "reiserfsck finished pass 2 at %s"
		      "###########\n", ctime(&t));
	fs->fs_dirt = 1;
	reiserfs_close(fs);
	exit(0);
}

void pass_2(reiserfs_filsys_t fs)
{
	if (fsck_log_file(fs) != stderr)
		fsck_log("####### Pass 2 #######\n");

	before_pass_2(fs);

	/* take blocks which were not inserted into tree yet and put each
	   item separately */
	do_pass_2(fs);

	after_pass_2(fs);

	if (get_sb_root_block(fs->fs_ondisk_sb) == ~(__u32) 0 ||
	    get_sb_root_block(fs->fs_ondisk_sb) == 0)
		die("\nNo reiserfs metadata found.  If you are sure that you had the reiserfs\n" "on this partition,  then the start  of the partition  might be changed\n" "or all data were wiped out. The start of the partition may get changed\n" "by a partitioner  if you have used one.  Then you probably rebuilt the\n" "superblock as there was no one.  Zero the block at 64K offset from the\n" "start of the partition (a new super block you have just built) and try\n" "to move the start of the partition a few cylinders aside  and check if\n" "debugreiserfs /dev/xxx detects a reiserfs super block. If it does this\n" "is likely to be the right super block version.                        \n");
}
