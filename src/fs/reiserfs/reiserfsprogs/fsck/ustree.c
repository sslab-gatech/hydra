/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"
#include "progbar.h"

struct tree_balance *cur_tb = 0;

void reiserfsck_paste_into_item(struct reiserfs_path *path, const char *body,
				int size)
{
	struct tree_balance tb;

	init_tb_struct(&tb, fs, path, size);
	if (fix_nodes( /*tb.transaction_handle, */ M_PASTE, &tb, 0 /*ih */ ) !=
	    CARRY_ON)
		//fix_nodes(options, tree_balance, ih_to_option, body_to_option)

		die("reiserfsck_paste_into_item: fix_nodes failed");

	do_balance( /*tb.transaction_handle, */ &tb, 0, body, M_PASTE,
		   0 /*zero num */ );
}

void reiserfsck_insert_item(struct reiserfs_path *path, struct item_head *ih,
			    const char *body)
{
	struct tree_balance tb;

	init_tb_struct(&tb, fs, path, IH_SIZE + get_ih_item_len(ih));
	if (fix_nodes
	    ( /*tb.transaction_handle, */ M_INSERT, &tb,
	     ih /*, body */ ) != CARRY_ON)
		die("reiserfsck_insert_item: fix_nodes failed");
	do_balance( /*tb.transaction_handle, */ &tb, ih, body, M_INSERT,
		   0 /*zero num */ );
}

static void free_unformatted_nodes(struct item_head *ih, struct buffer_head *bh)
{
	__le32 *punfm = (__le32 *) ih_item_body(bh, ih);
	unsigned int i;

	for (i = 0; i < I_UNFM_NUM(ih); i++) {
		__u32 unfm = d32_get(punfm, i);
		if (unfm != 0) {
			struct buffer_head *to_be_forgotten;

			to_be_forgotten =
			    find_buffer(fs->fs_dev, unfm, fs->fs_blocksize);
			if (to_be_forgotten) {
				//atomic_inc(&to_be_forgotten->b_count);
				to_be_forgotten->b_count++;
				bforget(to_be_forgotten);
			}

			reiserfs_free_block(fs, unfm);
		}
	}
}

void reiserfsck_delete_item(struct reiserfs_path *path, int temporary)
{
	struct tree_balance tb;
	struct item_head *ih = tp_item_head(path);

	if (is_indirect_ih(ih) && !temporary)
		free_unformatted_nodes(ih, PATH_PLAST_BUFFER(path));

	init_tb_struct(&tb, fs, path, -(IH_SIZE + get_ih_item_len(ih)));

	if (fix_nodes( /*tb.transaction_handle, */ M_DELETE, &tb, 0 /*ih */ ) !=
	    CARRY_ON)
		die("reiserfsck_delete_item: fix_nodes failed");

	do_balance( /*tb.transaction_handle, */ &tb, 0, 0, M_DELETE,
		   0 /*zero num */ );
}

void reiserfsck_cut_from_item(struct reiserfs_path *path, int cut_size)
{
	struct tree_balance tb;
	struct item_head *ih;

	if (cut_size >= 0)
		die("reiserfsck_cut_from_item: cut size == %d", cut_size);

	if (is_indirect_ih(ih = tp_item_head(path))) {
		struct buffer_head *bh = PATH_PLAST_BUFFER(path);
		__u32 unfm_ptr =
		    d32_get((__le32 *)ih_item_body(bh, ih), I_UNFM_NUM(ih) - 1);
		if (unfm_ptr != 0) {
			struct buffer_head *to_be_forgotten;

			to_be_forgotten =
			    find_buffer(fs->fs_dev, unfm_ptr, fs->fs_blocksize);
			if (to_be_forgotten) {
				//atomic_inc(&to_be_forgotten->b_count);
				to_be_forgotten->b_count++;
				bforget(to_be_forgotten);
			}
			reiserfs_free_block(fs, unfm_ptr);
		}
	}

	init_tb_struct(&tb, fs, path, cut_size);

	if (fix_nodes( /*tb.transaction_handle, */ M_CUT, &tb, 0) != CARRY_ON)
		die("reiserfsck_cut_from_item: fix_nodes failed");

	do_balance( /*tb.transaction_handle, */ &tb, 0, 0, M_CUT,
		   0 /*zero num */ );
}

/* uget_rkey is utils clone of stree.c/get_rkey */
/*
struct reiserfs_key *uget_rkey (struct reiserfs_path *path)
{
    int pos, offset = path->path_length;
    struct buffer_head * bh;

    if (offset < FIRST_PATH_ELEMENT_OFFSET)
	die ("uget_rkey: illegal offset in the path (%d)", offset);

    while (offset-- > FIRST_PATH_ELEMENT_OFFSET) {
	if (! buffer_uptodate (PATH_OFFSET_PBUFFER (path, offset)))
	    die ("uget_rkey: parent is not uptodate");

	// Parent at the path is not in the tree now.
	if (! B_IS_IN_TREE (bh = PATH_OFFSET_PBUFFER (path, offset)))
	    die ("uget_rkey: buffer on the path is not in tree");

	// Check whether position in the parrent is correct.
	if ((pos = PATH_OFFSET_POSITION (path, offset)) > B_NR_ITEMS (bh))
	    die ("uget_rkey: invalid position (%d) in the path", pos);

	// Check whether parent at the path really points to the child.
	if (get_dc_child_blocknr (B_N_CHILD (bh, pos)) != PATH_OFFSET_PBUFFER (path, offset + 1)->b_blocknr)
	    die ("uget_rkey: invalid block number (%d). Must be %ld",
		 get_dc_child_blocknr (B_N_CHILD (bh, pos)), PATH_OFFSET_PBUFFER (path, offset + 1)->b_blocknr);
	
	// Return delimiting key if position in the parent is not the last one.
	if (pos != B_NR_ITEMS (bh))
	    return internal_key(bh, pos);
    }

    // there is no right delimiting key
    return 0;
}
*/

static unsigned long first_child(struct buffer_head *bh)
{
	return get_dc_child_blocknr(B_N_CHILD(bh, 0));
}

#if 0
static unsigned long last_child(struct buffer_head *bh)
{
	return child_block_number(bh, node_item_number(bh));
}
#endif

static unsigned long get_child(int pos, struct buffer_head *parent)
{
	if (pos == -1)
		return -1;

	if (pos > B_NR_ITEMS(parent))
		die("get_child: no child found, should not happen: %d of %d",
		    pos, B_NR_ITEMS(parent));
	return get_dc_child_blocknr(B_N_CHILD(parent, pos));

}

static void print(int cur, int total)
{
	if (fsck_quiet(fs))
		return;
	printf("/%3d (of %3d)", cur, total);
	fflush(stdout);
}

/* erase /XXX(of XXX) */
static void erase(void)
{
	if (fsck_quiet(fs))
		return;
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b");
	printf("             ");
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b");
	fflush(stdout);
}

void pass_through_tree(reiserfs_filsys_t fs, do_after_read_t action1,
		       do_on_full_path_t action2, int depth)
{
	struct buffer_head *path[MAX_HEIGHT] = { 0, };
	int total[MAX_HEIGHT] = { 0, };
	int cur[MAX_HEIGHT] = { 0, };
	int h = 0;
	unsigned long block = get_sb_root_block(fs->fs_ondisk_sb);
	int problem;
	struct spinner spinner;

	spinner_init(&spinner, fsck_progress_file(fs));

	if (block >= get_sb_block_count(fs->fs_ondisk_sb)
	    || not_data_block(fs, block)) {
		die("\nBad root block %lu. (--rebuild-tree did not complete)\n",
		    block);
	}

	while (1) {
		problem = 0;

		if (path[h])
			die("pass_through_tree: empty slot expected");
		if (h)
			print(cur[h - 1], total[h - 1]);

		if (fs->fs_badblocks_bm
		    && reiserfs_bitmap_test_bit(fs->fs_badblocks_bm, block)) {
			fsck_log
			    ("%s: block %lu specified in badblock list found in tree, whole subtree skipped\n",
			     __FUNCTION__, block);
			fsck_data(fs)->check.bad_nodes++;
			one_more_corruption(fs, FATAL);

			if (h == 0) {
				brelse(path[h]);
				path[h] = 0;
				break;
			}
			problem = 1;
		} else {

			spinner_touch(&spinner);
			path[h] = bread(fs->fs_dev, block, fs->fs_blocksize);
			if (path[h] == 0)
				/* FIXME: handle case when read failed */
				die("pass_through_tree: unable to read %lu block on device 0x%x\n", block, fs->fs_dev);

			if (action1)
				if ((problem = action1(fs, path, h))) {
					fsck_log
					    (" the problem in the internal node occured (%lu), whole subtree is skipped\n",
					     path[h]->b_blocknr);
					fsck_data(fs)->check.bad_nodes++;

					if (h == 0) {
						brelse(path[h]);
						path[h] = 0;
						break;
					}
				}
		}

		/* Time to stop. */
		if (h == depth)
			problem++;

		if (problem || is_leaf_node(path[h])) {
			if (!problem && action2)
				action2(fs, path, h);

			brelse(path[h]);
			if (h)
				erase();

			while (h && (cur[h - 1] == total[h - 1] || problem)) {
				problem = 0;
				path[h] = 0;
				h--;
				brelse(path[h]);
				if (h)
					erase();
			}

			if (h == 0) {
				path[h] = 0;
				break;
			}

			block = get_child(cur[h - 1], path[h - 1]);
			cur[h - 1]++;
			path[h] = 0;
			continue;
		}
		total[h] = B_NR_ITEMS(path[h]) + 1;
		cur[h] = 1;
		block = first_child(path[h]);
		h++;
	}
	spinner_clear(&spinner);
}
