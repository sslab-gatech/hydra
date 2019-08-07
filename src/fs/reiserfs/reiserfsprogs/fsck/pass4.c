/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

void pass_4_check_unaccessed_items(void)
{
	struct reiserfs_key key;
	struct reiserfs_path path;
	int i;
	struct buffer_head *bh;
	struct item_head *ih;
	unsigned long items;
	const struct reiserfs_key *rdkey;
	int ret;

	path.path_length = ILLEGAL_PATH_ELEMENT_OFFSET;
	key = root_dir_key;

	fsck_progress("Pass 4 - ");
	items = 0;

	while (reiserfs_search_by_key_4(fs, &key, &path) == ITEM_FOUND) {
		bh = PATH_PLAST_BUFFER(&path);

		/* print ~ how many leaves were scanned and how fast it was */
		if (!fsck_quiet(fs))
			print_how_fast(items++, 0, 50, 0);

		for (i = get_item_pos(&path), ih = tp_item_head(&path);
		     i < B_NR_ITEMS(bh); i++, ih++) {
			if (!is_item_reachable(ih)) {
				PATH_LAST_POSITION(&path) = i;
				rdkey = reiserfs_next_key(&path);
				if (rdkey)
					key = *rdkey;
				else
					memset(&key, 0xff, KEY_SIZE);

				pass_4_stat(fs)->deleted_items++;

				reiserfsck_delete_item(&path, 0);

				goto cont;
			}

			if (get_ih_flags(ih) != 0) {
				clean_ih_flags(ih);
				mark_buffer_dirty(bh);
			}

		}
		PATH_LAST_POSITION(&path) = i - 1;
		rdkey = reiserfs_next_key(&path);
		if (rdkey)
			key = *rdkey;
		else
			memset(&key, 0xff, KEY_SIZE);

		pathrelse(&path);

cont:
		/* to make gcc 3.2 do not sware here */ ;
	}

	pathrelse(&path);

	fsck_progress("finished\n");
	stage_report(4, fs);

	/* after pass 4 */

	/* put bitmap on place */
	reiserfs_bitmap_copy(fs->fs_bitmap2, fsck_new_bitmap(fs));

	/* update super block */
	set_sb_free_blocks(fs->fs_ondisk_sb,
			   reiserfs_bitmap_zeros(fsck_new_bitmap(fs)));
	mark_buffer_dirty(fs->fs_super_bh);

	/* write all dirty blocks */
	fsck_progress("Flushing..");
	fs->fs_dirt = 1;
	id_map_flush(proper_id_map(fs), fs);
	ret = reiserfs_flush_to_ondisk_bitmap(fs->fs_bitmap2, fs);
	if (ret < 0)
		reiserfs_exit(1, "Exiting after unrecoverable error.");
	reiserfs_flush(fs);
	fsck_progress("finished\n");

	return;
}
