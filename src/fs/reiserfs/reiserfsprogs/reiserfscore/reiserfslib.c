/*
 *  Copyright 2000-2004 by Hans Reiser, licensing governed by
 *  reiserfsprogs/README
 */

#define _GNU_SOURCE

#include "includes.h"
#include <linux/kdev_t.h>
#include <time.h>

struct reiserfs_key root_dir_key = { 0, 0, {{0, 0},} };
struct reiserfs_key parent_root_dir_key = { 0, 0, {{0, 0},} };
struct reiserfs_key lost_found_dir_key = { 0, 0, {{0, 0},} };
static struct reiserfs_key badblock_key =
    { constant_cpu_to_le32(BADBLOCK_DIRID),
      constant_cpu_to_le32(BADBLOCK_OBJID),
      {{constant_cpu_to_le32(0), constant_cpu_to_le32(0)},} };

__u16 root_dir_format = 0;
__u16 lost_found_dir_format = 0;

static void make_const_keys(void)
{
	set_key_dirid(&root_dir_key, REISERFS_ROOT_PARENT_OBJECTID);
	set_key_objectid(&root_dir_key, REISERFS_ROOT_OBJECTID);

	set_key_dirid(&parent_root_dir_key, 0);
	set_key_objectid(&parent_root_dir_key, REISERFS_ROOT_PARENT_OBJECTID);
}

/* reiserfs needs at least: enough blocks for journal, 64 k at the beginning,
   one block for super block, bitmap block and root block. Note that first
   bitmap block must point to all of them */
int is_block_count_correct(unsigned long journal_offset,
			   unsigned int block_size, unsigned long block_count,
			   unsigned long journal_size)
{
	unsigned long blocks;

	/* RESERVED, MD RAID SBs, super block, bitmap, root, journal size with journal header */
	blocks = journal_offset + journal_size;

	/* we have a limit: skipped area, super block, journal and root block
	   all have to be addressed by one first bitmap */
	if (blocks > block_size * 8)
		return 0;

	if (blocks > block_count)
		return 0;

	return 1;
}

/* read super block. fixme: only 4k blocks, pre-journaled format
   is refused. Journal and bitmap are to be opened separately.
   skip_check is set to 1 if checks of openned SB should be omitted.*/
reiserfs_filsys_t reiserfs_open(const char *filename, int flags,
				 long *error, void *vp, int check)
{
	reiserfs_filsys_t fs;
	struct buffer_head *bh;
	struct reiserfs_super_block *sb;
	int fd;
	unsigned int i;

	/* convert root dir key and parent root dir key to little endian format */
	make_const_keys();

	*error = 0;

	fd = open(filename, flags
#if defined(O_LARGEFILE)
		  | O_LARGEFILE
#endif
	    );
	if (fd == -1) {
		*error = errno;
		return NULL;
	}

	fs = getmem(sizeof(*fs));
	fs->fs_dev = fd;
	fs->fs_vp = vp;
	asprintf(&fs->fs_file_name, "%s", filename);

	/* reiserfs super block is either in 16-th or in 2-nd 4k block of the
	   device */
	for (i = 2; i < 17; i += 14) {
		bh = bread(fd, i, 4096);
		if (!bh) {
			*error = REISERFS_ET_BREAD_FAILED;
		} else {
			sb = (struct reiserfs_super_block *)bh->b_data;

			if (is_any_reiserfs_magic_string(sb))
				goto found;

			/* reiserfs signature is not found at the i-th 4k block */
			brelse(bh);
		}
	}

	*error = REISERFS_ET_BAD_MAGIC;

	freemem(fs);
	close(fd);
	fs = NULL;
	return fs;

found:

	if (!is_blocksize_correct(get_sb_block_size(sb))) {
		*error = REISERFS_ET_BAD_SUPER;
		freemem(fs);
		close(fd);
		brelse(bh);
		return NULL;
	}

	if (check) {
		/* A few checks of found super block. */
		struct buffer_head *tmp_bh;

		tmp_bh =
		    bread(fd, get_sb_block_count(sb) - 1,
			  get_sb_block_size(sb));

		if (!tmp_bh) {
			*error = REISERFS_ET_SMALL_PARTITION;
			freemem(fs);
			close(fd);
			brelse(bh);
			return NULL;
		}

		brelse(tmp_bh);
	}

	fs->fs_blocksize = get_sb_block_size(sb);

	/* check block size on the filesystem */
	if (fs->fs_blocksize != 4096) {
		i = bh->b_blocknr * 4096 / fs->fs_blocksize;
		brelse(bh);
		bh = bread(fd, i, fs->fs_blocksize);
		if (!bh) {
			*error = REISERFS_ET_BREAD_FAILED;
			freemem(fs);
			return NULL;
		}
		sb = (struct reiserfs_super_block *)bh->b_data;
	}

	fs->fs_hash_function = code2func(get_sb_hash_code(sb));
	fs->fs_super_bh = bh;
	fs->fs_ondisk_sb = sb;
	fs->fs_flags = flags;	/* O_RDONLY or O_RDWR */

	fs->fs_format = get_reiserfs_format(sb);

	/*reiserfs_read_bitmap_blocks(fs); */
	if (flags & O_RDWR)
		fs->fs_dirt = 1;
	else
		fs->fs_dirt = 0;

	return fs;
}

/* creates buffer for super block and fills it up with fields which are
   constant for given size and version of a filesystem */
reiserfs_filsys_t reiserfs_create(const char *filename,
				   int version,
				   unsigned long block_count,
				   int block_size,
				   int default_journal, int new_format,
				   long *error)
{
	reiserfs_filsys_t fs;
	time_t now;
	unsigned int bmap_nr = reiserfs_bmap_nr(block_count, block_size);;

	*error = 0;

	/* convert root dir key and parent root dir key to little endian format */
	make_const_keys();

	if (count_blocks(filename, block_size) < block_count) {
		*error = REISERFS_ET_NOT_ENOUGH_BLOCKS;
		return NULL;
	}

	if (!is_block_count_correct(REISERFS_DISK_OFFSET_IN_BYTES / block_size,
				    block_size, block_count, 0)) {
		*error = REISERFS_ET_TOO_SMALL;
		return NULL;
	}

	fs = getmem(sizeof(*fs));
	if (!fs) {
		*error = errno;
		return NULL;
	}

	fs->fs_dev = open(filename, O_RDWR | O_EXCL
#if defined(O_LARGEFILE)
			  | O_LARGEFILE
#endif
	    );
	if (fs->fs_dev == -1) {
		*error = errno;
		freemem(fs);
		return NULL;
	}

	fs->fs_blocksize = block_size;
	asprintf(&fs->fs_file_name, "%s", filename);
	fs->fs_format = version;

	if (new_format)
		fs->fs_super_bh = getblk(fs->fs_dev,
					 REISERFS_DISK_OFFSET_IN_BYTES /
					 block_size, block_size);
	else
		fs->fs_super_bh = getblk(fs->fs_dev,
					 REISERFS_OLD_DISK_OFFSET_IN_BYTES /
					 block_size, block_size);

	if (!fs->fs_super_bh) {
		*error = REISERFS_ET_GETBLK_FAILED;
		return NULL;
	}

	mark_buffer_uptodate(fs->fs_super_bh, 1);

	fs->fs_ondisk_sb =
	    (struct reiserfs_super_block *)fs->fs_super_bh->b_data;
	memset(fs->fs_ondisk_sb, 0, block_size);

	/* fill super block fields which are constant for given version and block count */
	set_sb_block_count(fs->fs_ondisk_sb, block_count);
	/* sb_free_blocks */
	/* sb_root_block */
	/* sb_journal_1st_block */
	/* sb_journal_dev */
	/* sb_orig_journal_size */
	/* sb_joural_magic */
	/* sb_journal magic_F */
	/* sb_mount_id */
	/* sb_not_used0 */
	/* sb_generation_number */
	set_sb_block_size(fs->fs_ondisk_sb, block_size);
	switch (version) {
	case REISERFS_FORMAT_3_5:
		set_sb_oid_maxsize(fs->fs_ondisk_sb,
				   (block_size -
				    SB_SIZE_V1) / sizeof(__u32) / 2 * 2);
		/* sb_oid_cursize */
		/* sb_state */
		memcpy(fs->fs_ondisk_sb->s_v1.s_magic,
		       REISERFS_3_5_SUPER_MAGIC_STRING,
		       strlen(REISERFS_3_5_SUPER_MAGIC_STRING));
		break;

	case REISERFS_FORMAT_3_6:
		set_sb_oid_maxsize(fs->fs_ondisk_sb,
				   (block_size -
				    SB_SIZE) / sizeof(__u32) / 2 * 2);
		/* sb_oid_cursize */
		/* sb_state */
		memcpy(fs->fs_ondisk_sb->s_v1.s_magic,
		       REISERFS_3_6_SUPER_MAGIC_STRING,
		       strlen(REISERFS_3_6_SUPER_MAGIC_STRING));
		break;
	}
	if (!default_journal)
		memcpy(fs->fs_ondisk_sb->s_v1.s_magic,
		       REISERFS_JR_SUPER_MAGIC_STRING,
		       strlen(REISERFS_JR_SUPER_MAGIC_STRING));

	/* sb_fsck_state */
	/* sb_hash_function_code */
	/* sb_tree_height */

	set_sb_bmap_nr(fs->fs_ondisk_sb,
		       reiserfs_bmap_over(bmap_nr) ? 0 : bmap_nr);

	set_sb_version(fs->fs_ondisk_sb, version);
	set_sb_v2_lastcheck(fs->fs_ondisk_sb, time(&now));
	set_sb_v2_check_interval(fs->fs_ondisk_sb, DEFAULT_CHECK_INTERVAL);
	set_sb_v2_mnt_count(fs->fs_ondisk_sb, 1);
	set_sb_v2_max_mnt_count(fs->fs_ondisk_sb, DEFAULT_MAX_MNT_COUNT);

	/* sb_not_used1 */

	mark_buffer_dirty(fs->fs_super_bh);
	fs->fs_dirt = 1;

	return fs;
}

int no_reiserfs_found(reiserfs_filsys_t fs)
{
	return (fs == NULL || fs->fs_blocksize == 0) ? 1 : 0;
}

int spread_bitmaps(reiserfs_filsys_t fs)
{
	return fs->fs_super_bh->b_blocknr != 2;
}

/* 0 means: do not guarantee that fs is consistent */
int reiserfs_is_fs_consistent(reiserfs_filsys_t fs)
{
	if (get_sb_umount_state(fs->fs_ondisk_sb) == FS_CLEANLY_UMOUNTED &&
	    get_sb_fs_state(fs->fs_ondisk_sb) == FS_CONSISTENT)
		return 1;
	return 0;
}

/* flush bitmap, brelse super block, flush all dirty buffers, close and open
   again the device, read super block */
static void reiserfs_only_reopen(reiserfs_filsys_t fs, int flag)
{
	unsigned long super_block;

	/*  reiserfs_flush_to_ondisk_bitmap (fs->fs_bitmap2, fs); */
	super_block = fs->fs_super_bh->b_blocknr;
	brelse(fs->fs_super_bh);
	flush_buffers(fs->fs_dev);

	invalidate_buffers(fs->fs_dev);
	if (close(fs->fs_dev))
		die("reiserfs_reopen: closed failed: %s", strerror(errno));

	fs->fs_dev = open(fs->fs_file_name, flag
#if defined(O_LARGEFILE)
			  | O_LARGEFILE
#endif
	    );
	if (fs->fs_dev == -1)
		die("reiserfs_reopen: could not reopen device: %s",
		    strerror(errno));

	fs->fs_super_bh = bread(fs->fs_dev, super_block, fs->fs_blocksize);
	if (!fs->fs_super_bh)
		die("reiserfs_reopen: reading super block failed");
	fs->fs_ondisk_sb =
	    (struct reiserfs_super_block *)fs->fs_super_bh->b_data;
	fs->fs_flags = flag;	/* O_RDONLY or O_RDWR */

	if (flag & O_RDWR)
		fs->fs_dirt = 1;
	else
		fs->fs_dirt = 0;
}

void reiserfs_reopen(reiserfs_filsys_t fs, int flag)
{
	reiserfs_only_reopen(fs, flag);
	reiserfs_reopen_journal(fs, flag);
}

int is_opened_rw(reiserfs_filsys_t fs)
{
	if ((fs->fs_flags) & O_RDWR)
		return 1;
	return 0;
}

/* flush all changes made on a filesystem */
void reiserfs_flush(reiserfs_filsys_t fs)
{
	if (fs->fs_dirt) {
		reiserfs_flush_journal(fs);
		flush_buffers(fs->fs_dev);
	}
	fs->fs_dirt = 0;
}

/* free all memory involved into manipulating with filesystem */
void reiserfs_free(reiserfs_filsys_t fs)
{
	reiserfs_free_journal(fs);
	reiserfs_free_ondisk_bitmap(fs);

	/* release super block and memory used by filesystem handler */
	brelse(fs->fs_super_bh);
	fs->fs_super_bh = NULL;

	free_buffers();

	free(fs->fs_file_name);
	fs->fs_file_name = NULL;
	freemem(fs);
}

/* this closes everything: journal. bitmap and the fs itself */
void reiserfs_close(reiserfs_filsys_t fs)
{
	reiserfs_close_journal(fs);
	reiserfs_close_ondisk_bitmap(fs);

	reiserfs_flush(fs);
	reiserfs_free(fs);
	fsync(fs->fs_dev);
}

int reiserfs_new_blocknrs(reiserfs_filsys_t fs,
			  unsigned long *free_blocknrs,
			  unsigned long start, int amount_needed)
{
	if (fs->block_allocator)
		return fs->block_allocator(fs, free_blocknrs, start,
					   amount_needed);
	die("block allocator is not defined\n");
	return 0;
}

int reiserfs_free_block(reiserfs_filsys_t fs, unsigned long block)
{
	if (fs->block_deallocator)
		return fs->block_deallocator(fs, block);
	die("block deallocator is not defined\n");
	return 0;
}

static int reiserfs_search_by_key_x(reiserfs_filsys_t fs,
				    const struct reiserfs_key *key,
				    struct reiserfs_path *path, int key_length)
{
	struct buffer_head *bh;
	unsigned long block;
	struct reiserfs_path_element *curr;
	int retval;

	block = get_sb_root_block(fs->fs_ondisk_sb);
	if (not_data_block(fs, block))
		return IO_ERROR;

	path->path_length = ILLEGAL_PATH_ELEMENT_OFFSET;
	while (1) {
		curr = PATH_OFFSET_PELEMENT(path, ++path->path_length);
		bh = curr->pe_buffer =
		    bread(fs->fs_dev, block, fs->fs_blocksize);
		if (bh == NULL) {
			path->path_length--;
			pathrelse(path);
			return ITEM_NOT_FOUND;
		}
		retval =
		    reiserfs_bin_search(key, leaf_key(bh, 0), B_NR_ITEMS(bh),
					is_leaf_node(bh) ? IH_SIZE : KEY_SIZE,
					&curr->pe_position,
					key_length ==
					4 ? comp_keys : comp_keys_3);
		if (retval == POSITION_FOUND) {
			/* key found, return if this is leaf level */
			if (is_leaf_node(bh)) {
				path->pos_in_item = 0;
				return ITEM_FOUND;
			}
			curr->pe_position++;
		} else {
			/* key not found in the node */
			if (is_leaf_node(bh))
				return ITEM_NOT_FOUND;
		}
		block = get_dc_child_blocknr(B_N_CHILD(bh, curr->pe_position));
		if (not_data_block(fs, block))
			return IO_ERROR;
	}
	printf("search_by_key: you can not get here\n");
	return ITEM_NOT_FOUND;
}

int reiserfs_search_by_key_3(reiserfs_filsys_t fs, const struct reiserfs_key *key,
			     struct reiserfs_path *path)
{
	return reiserfs_search_by_key_x(fs, key, path, 3);
}

int reiserfs_search_by_key_4(reiserfs_filsys_t fs, const struct reiserfs_key *key,
			     struct reiserfs_path *path)
{
	return reiserfs_search_by_key_x(fs, key, path, 4);
}

/* key is key of byte in the regular file. This searches in tree
   through items and in the found item as well */
int reiserfs_search_by_position(reiserfs_filsys_t s, struct reiserfs_key *key,
				int version, struct reiserfs_path *path)
{
	struct buffer_head *bh;
	struct item_head *ih;
	struct reiserfs_key *next_key;

	if (reiserfs_search_by_key_3(s, key, path) == ITEM_FOUND) {
		ih = tp_item_head(path);

		if (!is_direct_ih(ih) && !is_indirect_ih(ih))
			return DIRECTORY_FOUND;

		path->pos_in_item = 0;
		return POSITION_FOUND;
	}

	bh = get_bh(path);
	ih = tp_item_head(path);

	if (PATH_LAST_POSITION(path) == 0) {
		/* previous item does not exist, that means we are in leftmost leaf of
		 * the tree */
		if (!not_of_one_file(&ih->ih_key, key)) {
			if (!is_direct_ih(ih) && !is_indirect_ih(ih))
				return DIRECTORY_FOUND;
			return POSITION_NOT_FOUND;
		}
		return FILE_NOT_FOUND;
	}

	/* take previous item */
	PATH_LAST_POSITION(path)--;
	ih--;

	if (not_of_one_file(&ih->ih_key, key) || is_stat_data_ih(ih)) {
		/* previous item belongs to another object or is a stat data, check
		 * next item */
		PATH_LAST_POSITION(path)++;
		if (PATH_LAST_POSITION(path) < B_NR_ITEMS(bh))
			/* next key is in the same node */
			next_key = leaf_key(bh, PATH_LAST_POSITION(path));
		else
			next_key = uget_rkey(path);
		if (next_key == NULL || not_of_one_file(next_key, key)) {
			/* there is no any part of such file in the tree */
			path->pos_in_item = 0;
			return FILE_NOT_FOUND;
		}

		if (is_direntry_key(next_key)) {
			reiserfs_warning(stderr,
					 "%s: looking for %k found a directory with the same key\n",
					 __func__, next_key);
			return DIRECTORY_FOUND;
		}

		/* next item is the part of this file */
		path->pos_in_item = 0;
		return POSITION_NOT_FOUND;
	}

	if (is_direntry_ih(ih)) {
		return DIRECTORY_FOUND;
	}

	if (is_stat_data_ih(ih)) {
		PATH_LAST_POSITION(path)++;
		return FILE_NOT_FOUND;
	}

	/* previous item is part of desired file */
	if (I_K_KEY_IN_ITEM(ih, key, bh->b_size)) {
		path->pos_in_item = get_offset(key) - get_offset(&ih->ih_key);
		if (is_indirect_ih(ih))
			path->pos_in_item /= bh->b_size;
		return POSITION_FOUND;
	}

	path->pos_in_item =
	    is_indirect_ih(ih) ? I_UNFM_NUM(ih) : get_ih_item_len(ih);
	return POSITION_NOT_FOUND;
}

static int comp_dir_entries(const void *p1, const void *p2)
{
	__u32 off1, off2;

	off1 = d32_get((__le32 *) p1, 0);
	off2 = *(__u32 *) p2;

	if (off1 < off2)
		return -1;
	if (off1 > off2)
		return 1;
	return 0;
}

struct reiserfs_key *uget_lkey(const struct reiserfs_path *path)
{
	int pos, offset = path->path_length;
	const struct buffer_head *bh;

	if (offset < FIRST_PATH_ELEMENT_OFFSET)
		die("uget_lkey: illegal offset in the path (%d)", offset);

	/* While not higher in path than first element. */
	while (offset-- > FIRST_PATH_ELEMENT_OFFSET) {
		if (!buffer_uptodate(PATH_OFFSET_PBUFFER(path, offset)))
			die("uget_lkey: parent is not uptodate");

		/* Parent at the path is not in the tree now. */
		if (!B_IS_IN_TREE(bh = PATH_OFFSET_PBUFFER(path, offset)))
			die("uget_lkey: buffer on the path is not in tree");

		/* Check whether position in the parent is correct. */
		if ((pos = PATH_OFFSET_POSITION(path, offset)) > B_NR_ITEMS(bh))
			die("uget_lkey: invalid position (%d) in the path",
			    pos);

		/* Check whether parent at the path really points to the child. */
		if (get_dc_child_blocknr(B_N_CHILD(bh, pos)) !=
		    PATH_OFFSET_PBUFFER(path, offset + 1)->b_blocknr)
			die("uget_lkey: invalid block number (%d). Must be %ld",
			    get_dc_child_blocknr(B_N_CHILD(bh, pos)),
			    PATH_OFFSET_PBUFFER(path, offset + 1)->b_blocknr);

		/* Return delimiting key if position in the parent is not equal to zero. */
		if (pos)
			return internal_key(bh, pos - 1);
	}

	/* there is no left delimiting key */
	return NULL;
}

struct reiserfs_key *uget_rkey(const struct reiserfs_path *path)
{
	int pos, offset = path->path_length;
	struct buffer_head *bh;

	if (offset < FIRST_PATH_ELEMENT_OFFSET)
		die("uget_rkey: illegal offset in the path (%d)", offset);

	while (offset-- > FIRST_PATH_ELEMENT_OFFSET) {
		if (!buffer_uptodate(PATH_OFFSET_PBUFFER(path, offset)))
			die("uget_rkey: parent is not uptodate");

		/* Parent at the path is not in the tree now. */
		if (!B_IS_IN_TREE(bh = PATH_OFFSET_PBUFFER(path, offset)))
			die("uget_rkey: buffer on the path is not in tree");

		/* Check whether position in the parrent is correct. */
		if ((pos = PATH_OFFSET_POSITION(path, offset)) > B_NR_ITEMS(bh))
			die("uget_rkey: invalid position (%d) in the path",
			    pos);

		/* Check whether parent at the path really points to the child. */
		if (get_dc_child_blocknr(B_N_CHILD(bh, pos)) !=
		    PATH_OFFSET_PBUFFER(path, offset + 1)->b_blocknr)
			die("uget_rkey: invalid block number (%d). Must be %ld",
			    get_dc_child_blocknr(B_N_CHILD(bh, pos)),
			    PATH_OFFSET_PBUFFER(path, offset + 1)->b_blocknr);

		/* Return delimiting key if position in the parent is not the last one. */
		if (pos != B_NR_ITEMS(bh))
			return internal_key(bh, pos);
	}

	/* there is no right delimiting key */
	return NULL;
}

struct reiserfs_key *reiserfs_next_key(const struct reiserfs_path *path)
{
	if (get_item_pos(path) < B_NR_ITEMS(get_bh(path)) - 1)
		return leaf_key(get_bh(path), get_item_pos(path) + 1);

	return uget_rkey(path);
}

/* NOTE: this only should be used to look for keys who exists */
int reiserfs_search_by_entry_key(reiserfs_filsys_t fs,
				 const struct reiserfs_key *key,
				 struct reiserfs_path *path)
{
	struct buffer_head *bh;
	int item_pos;
	struct item_head *ih;
	struct reiserfs_key tmpkey;
	__u32 offset;

	if (reiserfs_search_by_key_4(fs, key, path) == ITEM_FOUND) {
		path->pos_in_item = 0;
		return POSITION_FOUND;
	}

	bh = get_bh(path);
	item_pos = get_item_pos(path);
	ih = tp_item_head(path);

	if (item_pos == 0) {
		/* key is less than the smallest key in the tree */
		if (not_of_one_file(&(ih->ih_key), key))
			/* there are no items of that directory */
			return DIRECTORY_NOT_FOUND;

		if (!is_direntry_ih(ih)) {
			reiserfs_panic
			    ("reiserfs_search_by_entry_key: found item "
			     "is not of directory type %H", ih);
		}

		/* key we looked for should be here */
		path->pos_in_item = 0;
		return POSITION_NOT_FOUND;
	}

	/* take previous item */
	item_pos--;
	ih--;
	PATH_LAST_POSITION(path)--;

	if (not_of_one_file(&(ih->ih_key), key) || !is_direntry_ih(ih)) {
		/* previous item belongs to another object or is stat data, check next
		   item */

		item_pos++;
		PATH_LAST_POSITION(path)++;

		if (item_pos < B_NR_ITEMS(bh)) {
			/* next item is in the same node */
			ih++;
			if (not_of_one_file(&(ih->ih_key), key)) {
				/* there are no items of that directory */
				path->pos_in_item = 0;
				return DIRECTORY_NOT_FOUND;
			}

			if (!is_direntry_ih(ih))
				reiserfs_panic
				    ("_search_by_entry_key: %k is not a directory",
				     key);
		} else {
			/* next item is in right neighboring node */
			struct reiserfs_key *next_key = uget_rkey(path);

			if (next_key == NULL ||
			    not_of_one_file(next_key, key)) {
				/* there are no items of that directory */
				path->pos_in_item = 0;
				return DIRECTORY_NOT_FOUND;
			}

			if (!is_direntry_key(next_key))
				reiserfs_panic
				    ("_search_by_entry_key: %k is not a directory",
				     key);

			/* we got right delimiting key - search for it - the entry will be
			   pasted in position 0 */
			copy_key(&tmpkey, next_key);
			pathrelse(path);
			if (reiserfs_search_by_key_4(fs, &tmpkey, path) !=
			    ITEM_FOUND || PATH_LAST_POSITION(path) != 0)
				reiserfs_panic
				    ("_search_by_entry_key: item corresponding to delimiting key %k not found",
				     &tmpkey);
		}

		/* next item is the part of this directory */
		path->pos_in_item = 0;
		return POSITION_NOT_FOUND;
	}

	/* previous item is part of desired directory */
	offset = get_key_offset_v1(key);
	if (reiserfs_bin_search
	    (&offset, B_I_DEH(bh, ih), get_ih_entry_count(ih), DEH_SIZE,
	     &(path->pos_in_item), comp_dir_entries) == POSITION_FOUND)
		return POSITION_FOUND;

	return POSITION_NOT_FOUND;
}

void init_tb_struct(struct tree_balance *tb, reiserfs_filsys_t fs,
		    struct reiserfs_path *path, int size)
{
	memset(tb, '\0', sizeof(struct tree_balance));
	tb->tb_fs = fs;
	tb->tb_path = path;

	PATH_OFFSET_PBUFFER(path, ILLEGAL_PATH_ELEMENT_OFFSET) = NULL;
	PATH_OFFSET_POSITION(path, ILLEGAL_PATH_ELEMENT_OFFSET) = 0;
	tb->insert_size[0] = size;
}

int reiserfs_remove_entry(reiserfs_filsys_t fs, const struct reiserfs_key *key)
{
	struct reiserfs_path path;
	struct tree_balance tb;
	struct item_head *ih;
	struct reiserfs_de_head *deh;

	if (reiserfs_search_by_entry_key(fs, key, &path) != POSITION_FOUND) {
		pathrelse(&path);
		return 1;
	}

	ih = tp_item_head(&path);
	if (get_ih_entry_count(ih) == 1) {
		init_tb_struct(&tb, fs, &path,
			       -(IH_SIZE + get_ih_item_len(ih)));
		if (fix_nodes(M_DELETE, &tb, NULL) != CARRY_ON) {
			unfix_nodes(&tb);
			return 1;
		}
		do_balance(&tb, NULL, NULL, M_DELETE, 0);
		return 0;
	}

	deh = B_I_DEH(get_bh(&path), ih) + path.pos_in_item;
	init_tb_struct(&tb, fs, &path,
		       -(DEH_SIZE + entry_length(ih, deh, path.pos_in_item)));
	if (fix_nodes(M_CUT, &tb, NULL) != CARRY_ON) {
		unfix_nodes(&tb);
		return 1;
	}
	do_balance(&tb, NULL, NULL, M_CUT, 0);
	return 0;
}

void reiserfs_paste_into_item(reiserfs_filsys_t fs,
			      struct reiserfs_path *path, const void *body,
			      int size)
{
	struct tree_balance tb;

	init_tb_struct(&tb, fs, path, size);

	if (fix_nodes(M_PASTE, &tb, NULL) != CARRY_ON)
		reiserfs_panic("reiserfs_paste_into_item: fix_nodes failed");

	do_balance(&tb, NULL, body, M_PASTE, 0 /* zero num */ );
}

void reiserfs_insert_item(reiserfs_filsys_t fs, struct reiserfs_path *path,
			  struct item_head *ih, const void *body)
{
	struct tree_balance tb;

	init_tb_struct(&tb, fs, path, IH_SIZE + get_ih_item_len(ih));
	if (fix_nodes(M_INSERT, &tb, ih) != CARRY_ON)
		die("reiserfs_insert_item: fix_nodes failed");

	do_balance(&tb, ih, body, M_INSERT, 0 /*zero num */ );
}

/*===========================================================================*/

__u32 hash_value(hashf_t func, const char *name, int namelen)
{
	__u32 res;

	res = func(name, namelen);
	res = GET_HASH_VALUE(res);
	if (res == 0)
		res = 128;

	return res;
}

/* if name is found in a directory - return 1 and set path to the name,
   otherwise return 0 and pathrelse path */
int reiserfs_locate_entry(reiserfs_filsys_t fs, struct reiserfs_key *dir,
			  const char *name, struct reiserfs_path *path)
{
	struct reiserfs_key entry_key;
	struct item_head *ih;
	struct reiserfs_de_head *deh;
	int i, retval;
	struct reiserfs_key *rdkey;

	set_key_dirid(&entry_key, get_key_dirid(dir));
	set_key_objectid(&entry_key, get_key_objectid(dir));
	set_key_offset_v1(&entry_key, 0);
	set_key_uniqueness(&entry_key, DIRENTRY_UNIQUENESS);

	if (reiserfs_search_by_entry_key(fs, &entry_key, path) ==
	    DIRECTORY_NOT_FOUND) {
		pathrelse(path);
		return 0;
	}

	do {
		ih = tp_item_head(path);
		deh = B_I_DEH(get_bh(path), ih) + path->pos_in_item;
		for (i = path->pos_in_item; i < get_ih_entry_count(ih);
		     i++, deh++) {
			/* the name in directory has the same hash as the given name */
			if ((name_in_entry_length(ih, deh, i) ==
			     (int)strlen(name))
			    && !memcmp(name_in_entry(deh, i), name,
				       strlen(name))) {
				path->pos_in_item = i;
				return 1;
			}
		}

		rdkey = uget_rkey(path);
		if (!rdkey || not_of_one_file(rdkey, dir)) {
			pathrelse(path);
			return 0;
		}

		if (!is_direntry_key(rdkey))
			reiserfs_panic
			    ("reiserfs_locate_entry: can not find name in broken directory yet");

		/* first name of that item may be a name we are looking for */
		entry_key = *rdkey;
		pathrelse(path);
		retval = reiserfs_search_by_entry_key(fs, &entry_key, path);
		if (retval != POSITION_FOUND)
			reiserfs_panic
			    ("reiserfs_locate_entry: wrong delimiting key in the tree");

	} while (1);

	return 0;

}

/* returns 0 if name is not found in a directory and 1 if name is
   found. Stores key found in the entry in 'key'. Returns minimal not used
   generation counter in 'min_gen_counter'. dies if found object is not a
   directory. */
int reiserfs_find_entry(reiserfs_filsys_t fs, const struct reiserfs_key *dir,
			const char *name, unsigned int *min_gen_counter,
			struct reiserfs_key *key)
{
	struct reiserfs_key entry_key;
	int retval;
	int i;
	INITIALIZE_REISERFS_PATH(path);
	struct item_head *ih;
	struct reiserfs_de_head *deh;
	struct reiserfs_key *rdkey;
	__u32 hash;

	set_key_dirid(&entry_key, get_key_dirid(dir));
	set_key_objectid(&entry_key, get_key_objectid(dir));
	if (!strcmp(name, "."))
		hash = DOT_OFFSET;
	else if (!strcmp(name, ".."))
		hash = DOT_DOT_OFFSET;
	else
		hash = hash_value(reiserfs_hash(fs), name, strlen(name));
	set_key_offset_v1(&entry_key, hash);
	set_key_uniqueness(&entry_key, DIRENTRY_UNIQUENESS);

	*min_gen_counter = 0;

	if (reiserfs_search_by_entry_key(fs, &entry_key, &path) ==
	    DIRECTORY_NOT_FOUND) {
		pathrelse(&path);
		return 0;
	}

	do {
		ih = tp_item_head(&path);
		deh = B_I_DEH(get_bh(&path), ih) + path.pos_in_item;
		for (i = path.pos_in_item; i < get_ih_entry_count(ih);
		     i++, deh++) {
			if (GET_HASH_VALUE(get_deh_offset(deh)) !=
			    GET_HASH_VALUE(hash)) {
				/* all entries having the same hash were scanned */
				pathrelse(&path);
				return 0;
			}

			if (GET_GENERATION_NUMBER(get_deh_offset(deh)) ==
			    *min_gen_counter)
				(*min_gen_counter)++;

			if ((name_in_entry_length(ih, deh, i) ==
			     (int)strlen(name))
			    &&
			    (!memcmp
			     (name_in_entry(deh, i), name, strlen(name)))) {
				/* entry found in the directory */
				if (key) {
					memset(key, 0,
					       sizeof(struct reiserfs_key));
					set_key_dirid(key, get_deh_dirid(deh));
					set_key_objectid(key,
							 get_deh_objectid(deh));
				}
				pathrelse(&path);
				return 1;	//get_deh_objectid (deh) ? get_deh_objectid (deh) : 1;
			}
		}

		rdkey = uget_rkey(&path);
		if (!rdkey || not_of_one_file(rdkey, dir)) {
			pathrelse(&path);
			return 0;
		}

		if (!is_direntry_key(rdkey))
			reiserfs_panic
			    ("reiserfs_find_entry: can not find name in broken directory yet");

		/* next item is the item of the directory we are looking name in */
		if (GET_HASH_VALUE(get_offset(rdkey)) != hash) {
			/* but there is no names with given hash */
			pathrelse(&path);
			return 0;
		}

		/* first name of that item may be a name we are looking for */
		entry_key = *rdkey;
		pathrelse(&path);
		retval = reiserfs_search_by_entry_key(fs, &entry_key, &path);
		if (retval != POSITION_FOUND)
			reiserfs_panic
			    ("reiserfs_find_entry: wrong delimiting key in the tree");

	} while (1);

	return 0;
}

/* compose directory entry: dir entry head and name itself */
static char *make_entry(char *entry, const char *name,
			const struct reiserfs_key *key, __u32 offset)
{
	struct reiserfs_de_head *deh;
	__u16 state;

	if (!entry)
		entry = getmem(DEH_SIZE + ROUND_UP(strlen(name)));

	memset(entry, 0, DEH_SIZE + ROUND_UP(strlen(name)));
	deh = (struct reiserfs_de_head *)entry;

	set_deh_location(deh, 0);
	set_deh_offset(deh, offset);
	state = (1 << DEH_Visible2);
	set_deh_state(deh, state);

	/* key of object entry will point to */
	set_deh_dirid(deh, get_key_dirid(key));
	set_deh_objectid(deh, get_key_objectid(key));

	memcpy((char *)(deh + 1), name, strlen(name));
	return entry;
}

/* add new name into a directory. If it exists in a directory - do
   nothing */
int reiserfs_add_entry(reiserfs_filsys_t fs, const struct reiserfs_key *dir,
		       const char *name, int name_len,
		       const struct reiserfs_key *key, __u16 fsck_need)
{
	struct item_head entry_ih = { {0,}, };
	char *entry;
	int retval;
	INITIALIZE_REISERFS_PATH(path);
	unsigned int gen_counter;
	int item_len;
	__u32 hash;

	if (reiserfs_find_entry(fs, dir, name, &gen_counter, NULL))
		/* entry is in the directory already or directory was not found */
		return 0;

	/* compose entry key to look for its place in the tree */
	set_key_dirid(&(entry_ih.ih_key), get_key_dirid(dir));
	set_key_objectid(&(entry_ih.ih_key), get_key_objectid(dir));
	if (!strcmp(name, "."))
		hash = DOT_OFFSET;
	else if (!strcmp(name, ".."))
		hash = DOT_DOT_OFFSET;
	else
		hash =
		    hash_value(reiserfs_hash(fs), name,
			       strlen(name)) + gen_counter;
	set_key_offset_v1(&(entry_ih.ih_key), hash);
	set_key_uniqueness(&(entry_ih.ih_key), DIRENTRY_UNIQUENESS);

	set_ih_key_format(&entry_ih, KEY_FORMAT_1);
	set_ih_entry_count(&entry_ih, 1);

	item_len = DEH_SIZE + name_len;
/*
    if (get_reiserfs_format (fs->fs_ondisk_sb) == REISERFS_FORMAT_3_5)
	item_len = DEH_SIZE + strlen (name);
    else if (get_reiserfs_format (fs->fs_ondisk_sb) == REISERFS_FORMAT_3_6)
	item_len = DEH_SIZE + ROUND_UP (strlen (name));
    else
	reiserfs_panic ("unknown fs format");
*/

	set_ih_item_len(&entry_ih, item_len);

	/* fsck may need to insert item which was not reached yet */
	set_ih_flags(&entry_ih, fsck_need);

	entry = make_entry(NULL, name, key, get_offset(&(entry_ih.ih_key)));

	retval = reiserfs_search_by_entry_key(fs, &(entry_ih.ih_key), &path);
	switch (retval) {
	case POSITION_NOT_FOUND:
		reiserfs_paste_into_item(fs, &path, entry, item_len);
		break;

	case DIRECTORY_NOT_FOUND:
		set_deh_location((struct reiserfs_de_head *)entry, DEH_SIZE);
		reiserfs_insert_item(fs, &path, &entry_ih, entry);
		break;

	default:
		reiserfs_panic
		    ("reiserfs_add_entry: looking for %k (inserting name \"%s\") "
		     "search_by_entry_key returned %d", &(entry_ih.ih_key),
		     name, retval);
	}

	freemem(entry);
	return item_len;
}

void copy_key(void *to, const void *from)
{
	memcpy(to, from, KEY_SIZE);
}

void copy_short_key(void *to, const void *from)
{
	memcpy(to, from, SHORT_KEY_SIZE);
}

/* inserts new or old stat data of a directory (unreachable, nlinks == 0) */
int create_dir_sd(reiserfs_filsys_t fs,
		  struct reiserfs_path *path, const struct reiserfs_key *key,
		  void (*modify_item) (struct item_head *, void *))
{
	struct item_head ih;
	struct stat_data sd;
	int key_format;

	if (fs->fs_format == REISERFS_FORMAT_3_5)
		key_format = KEY_FORMAT_1;
	else
		key_format = KEY_FORMAT_2;

	memset(&sd, 0, sizeof(sd));
	make_dir_stat_data(fs->fs_blocksize, key_format, get_key_dirid(key),
			   get_key_objectid(key), &ih, &sd);

	/* if calling user is not root set the owner of the root entry
	 * to the calling user */
	if (getuid()) {
		if (key_format == KEY_FORMAT_1) {
			struct stat_data_v1 *sd_v1 = (struct stat_data_v1 *)&sd;
			set_sd_v1_uid(sd_v1, getuid());
			set_sd_v1_gid(sd_v1, getgid());
		} else {
			set_sd_v2_uid(&sd, getuid());
			set_sd_v2_gid(&sd, getgid());
		}
	}

	if (modify_item)
		modify_item(&ih, &sd);
#if 0
	/* set nlink count to 0 and make the item unreachable */
	zero_nlink(&ih, &sd, 0);
	mark_item_unreachable(&ih);
#endif
	reiserfs_insert_item(fs, path, &ih, &sd);
	return key_format;
}

void make_sure_root_dir_exists(reiserfs_filsys_t fs,
			       void (*modify_item) (struct item_head *, void *),
			       int ih_flags)
{
	INITIALIZE_REISERFS_PATH(path);

	/* is there root's stat data */
	if (reiserfs_search_by_key_4(fs, &root_dir_key, &path) ==
	    ITEM_NOT_FOUND) {
		root_dir_format =
		    create_dir_sd(fs, &path, &root_dir_key, modify_item);
	} else {
		struct item_head *ih = tp_item_head(&path);

		if (!is_stat_data_ih(ih))
			reiserfs_panic("It must be root's stat data %k\n",
				       &ih->ih_key);

		root_dir_format =
		    (get_ih_item_len(tp_item_head(&path)) ==
		     SD_SIZE) ? KEY_FORMAT_2 : KEY_FORMAT_1;
		pathrelse(&path);
	}

	/* add "." and ".." if any of them do not exist. Last two
	   parameters say: 0 - entry is not added on lost_found pass and 1
	   - mark item unreachable */

	reiserfs_add_entry(fs, &root_dir_key, ".",
			   name_length(".", root_dir_format), &root_dir_key,
			   ih_flags);
	reiserfs_add_entry(fs, &root_dir_key, "..",
			   name_length("..", root_dir_format),
			   &parent_root_dir_key, ih_flags);
}

/* we only can use a file for filesystem or journal if it is either not
   mounted block device or regular file and we are forced to use it */
int can_we_format_it(const char *device_name, int force)
{
	mode_t mode;
	dev_t rdev;

	if (misc_device_mounted(device_name) > 0) {
		/* device looks mounted */
		reiserfs_warning(stderr, "'%s' looks mounted.", device_name);
		check_forcing_ask_confirmation(force);
	}

	mode = misc_device_mode(device_name);
	rdev = misc_device_rdev(device_name);

	if (!S_ISBLK(mode)) {
		/* file is not a block device */
		reiserfs_warning(stderr, "%s is not a block special device\n",
				 device_name);
		check_forcing_ask_confirmation(force);
	} else {
		if ((IDE_DISK_MAJOR(major(rdev)) && minor(rdev) % 64 == 0) ||
		    (SCSI_BLK_MAJOR(major(rdev)) && minor(rdev) % 16 == 0)) {
			/* /dev/hda or similar */
			reiserfs_warning(stderr,
					 "%s is entire device, not just one partition!\n",
					 device_name);
			check_forcing_ask_confirmation(force);
		}
	}

	return 1;
}

int create_badblock_bitmap(reiserfs_filsys_t fs, const char *badblocks_file)
{
	FILE *fd;
	char buf[128];
	__u32 blocknr;
	int count;

	fs->fs_badblocks_bm =
	    reiserfs_create_bitmap(get_sb_block_count(fs->fs_ondisk_sb));
	reiserfs_bitmap_zero(fs->fs_badblocks_bm);

	if (!badblocks_file)
		return 0;

	fd = fopen(badblocks_file, "r");

	if (fd == NULL) {
		fprintf(stderr,
			"%s: Failed to open the given badblock file '%s'.\n\n",
			__FUNCTION__, badblocks_file);
		return 1;
	}

	while (!feof(fd)) {
		if (fgets(buf, sizeof(buf), fd) == NULL)
			break;
		count = sscanf(buf, "%u", &blocknr);

		if (count <= 0)
			continue;

		if (blocknr >= get_sb_block_count(fs->fs_ondisk_sb)) {
			fprintf(stderr,
				"%s: block number (%u) points out of fs size "
				"(%u).\n", __FUNCTION__, blocknr,
				get_sb_block_count(fs->fs_ondisk_sb));
		} else if (not_data_block(fs, blocknr)) {
			fprintf(stderr,
				"%s: block number (%u) belongs to system "
				"reiserfs area. It cannot be relocated.\n",
				__FUNCTION__, blocknr);
			return 1;
		} else {
			reiserfs_bitmap_set_bit(fs->fs_badblocks_bm, blocknr);
		}
	}

	fclose(fd);

	return 0;
}

void badblock_list(reiserfs_filsys_t fs, badblock_func_t action, void *data)
{
	struct reiserfs_path badblock_path;
	struct reiserfs_key rd_key = badblock_key;
	struct reiserfs_key *key;

	badblock_path.path_length = ILLEGAL_PATH_ELEMENT_OFFSET;
	set_type_and_offset(KEY_FORMAT_2, &badblock_key, 1, TYPE_INDIRECT);

	while (1) {
		if (reiserfs_search_by_key_4(fs, &rd_key, &badblock_path) ==
		    IO_ERROR) {
			fprintf(stderr,
				"%s: Some problems while searching by the key "
				"occured. Probably due to tree corruptions.\n",
				__FUNCTION__);
			pathrelse(&badblock_path);
			break;
		}

		if (get_blkh_nr_items(B_BLK_HEAD(get_bh(&badblock_path))) <=
		    PATH_LAST_POSITION(&badblock_path)) {
			pathrelse(&badblock_path);
			break;
		}

		rd_key = tp_item_head(&badblock_path)->ih_key;

		if (get_key_dirid(&rd_key) != BADBLOCK_DIRID ||
		    get_key_objectid(&rd_key) != BADBLOCK_OBJID ||
		    !KEY_IS_INDIRECT_KEY(&rd_key)) {
			pathrelse(&badblock_path);
			break;
		}

		if ((key = reiserfs_next_key(&badblock_path)))
			rd_key = *key;
		else
			memset(&rd_key, 0, sizeof(rd_key));

		action(fs, &badblock_path, data);

		if (get_key_dirid(&rd_key) == 0)
			break;
	}
}

static void callback_badblock_rm(reiserfs_filsys_t fs,
				 struct reiserfs_path *badblock_path,
				 void *data)
{
	struct tree_balance tb;
	struct item_head *tmp_ih;

	tmp_ih = tp_item_head(badblock_path);
	memset(tp_item_body(badblock_path), 0, get_ih_item_len(tmp_ih));

	init_tb_struct(&tb, fs, badblock_path,
		       -(IH_SIZE +
			 get_ih_item_len(tp_item_head(badblock_path))));

	if (fix_nodes(M_DELETE, &tb, NULL) != CARRY_ON)
		die("%s: fix_nodes failed", __FUNCTION__);

	do_balance(&tb, NULL, NULL, M_DELETE, 0 /* zero num */);
}

void mark_badblock(reiserfs_filsys_t fs,
		   struct reiserfs_path *badblock_path, void *data)
{
	struct item_head *tmp_ih;
	__le32 *ind_item;
	__u32 i;

	if (!fs->fs_badblocks_bm)
		create_badblock_bitmap(fs, NULL);

	tmp_ih = tp_item_head(badblock_path);
	ind_item = (__le32 *) tp_item_body(badblock_path);

	for (i = 0; i < I_UNFM_NUM(tmp_ih); i++) {
		reiserfs_bitmap_set_bit(fs->fs_badblocks_bm,
					d32_get(ind_item, i));
	}

	pathrelse(badblock_path);
}

void add_badblock_list(reiserfs_filsys_t fs, int replace)
{
	struct tree_balance tb;
	struct reiserfs_path badblock_path;
	struct item_head badblock_ih;
	__le32 ni;

	__u64 offset;
	__u32 i, j;

	if (fs->fs_badblocks_bm == NULL)
		return;

	/* delete all items with badblock_key */
	if (replace)
		badblock_list(fs, callback_badblock_rm, NULL);

	memset(&badblock_ih, 0, sizeof(badblock_ih));
	set_ih_key_format(&badblock_ih, KEY_FORMAT_2);
	set_ih_item_len(&badblock_ih, UNFM_P_SIZE);
	set_ih_free_space(&badblock_ih, 0);
	set_ih_location(&badblock_ih, 0);
	set_key_dirid(&badblock_ih.ih_key, BADBLOCK_DIRID);
	set_key_objectid(&badblock_ih.ih_key, BADBLOCK_OBJID);
	set_type(KEY_FORMAT_2, &badblock_ih.ih_key, TYPE_INDIRECT);

	j = 0;

	/* insert all badblock pointers */
	for (i = 0; i < fs->fs_badblocks_bm->bm_bit_size; i++) {
		int retval;

		if (!reiserfs_bitmap_test_bit(fs->fs_badblocks_bm, i))
			continue;

		offset = j * fs->fs_blocksize + 1;
		set_offset(KEY_FORMAT_2, &badblock_ih.ih_key, offset);
		ni = cpu_to_le32(i);

		retval = reiserfs_search_by_position(fs, &badblock_ih.ih_key,
						key_format(&badblock_ih.ih_key),
						&badblock_path);

		switch (retval) {
		case (FILE_NOT_FOUND):
			init_tb_struct(&tb, fs, &badblock_path,
				       IH_SIZE + get_ih_item_len(&badblock_ih));

			if (fix_nodes(M_INSERT, &tb, &badblock_ih) != CARRY_ON)
				die("reiserfsck_insert_item: fix_nodes failed");

			do_balance(&tb, &badblock_ih, (void *)&ni, M_INSERT, 0);

			break;

		case (POSITION_NOT_FOUND):
		case (POSITION_FOUND):
			/* Insert the new item to the found position. */

			init_tb_struct(&tb, fs, &badblock_path, UNFM_P_SIZE);

			if (fix_nodes(M_PASTE, &tb, NULL) != CARRY_ON)
				die("reiserfsck_paste_into_item: fix_nodes failed");

			do_balance(&tb, NULL, (const char *)&ni, M_PASTE, 0);
			break;
		}

		j++;
	}
}

int reiserfs_iterate_file_data(reiserfs_filsys_t fs,
			       const struct reiserfs_key const *short_key,
			       reiserfs_file_iterate_indirect_fn indirect_fn,
			       reiserfs_file_iterate_direct_fn direct_fn,
			       void *data)
{
	INITIALIZE_REISERFS_PATH(path);
	struct reiserfs_key key = {
		.k2_dir_id = short_key->k2_dir_id,
		.k2_objectid = short_key->k2_objectid,
	};
	struct item_head *ih;
	__u64 size;
	__u64 position = 0;
	int ret;

	set_key_type_v2(&key, TYPE_STAT_DATA);
	set_key_offset_v2(&key, 0);

	ret = reiserfs_search_by_key_3(fs, &key, &path);
	if (ret != ITEM_FOUND) {
		ret = -ENOENT;
		goto fail;
	}

	ih = tp_item_head(&path);
	if (!is_stat_data_ih(ih)) {
		ret = -EINVAL;
		goto fail;
	}

	if (get_ih_key_format(ih) == KEY_FORMAT_1) {
		struct stat_data_v1 *sd = tp_item_body(&path);
		size = sd_v1_size(sd);
	} else {
		struct stat_data *sd = tp_item_body(&path);
		size = sd_v2_size(sd);
	}

	pathrelse(&path);

	set_key_offset_v2(&key, 1);
	set_key_type_v2(&key, TYPE_DIRECT);

	while (position < size) {
		struct item_head *ih = tp_item_head(&path);
		__u64 offset;
		ret = reiserfs_search_by_position(fs, &key, 0, &path);
		ih = tp_item_head(&path);
		if (ret != POSITION_FOUND) {
			reiserfs_warning(stderr,
				"found %k instead of %k [%d] (%lu, %lu)\n",
				&ih->ih_key, &key, ret, position, size);
			if (ret != ITEM_NOT_FOUND)
				ret = -EIO;
			goto fail;
		}

		offset = get_offset(&ih->ih_key);

		position = offset - 1;

		if (is_indirect_key(&ih->ih_key)) {
			int num_ptrs = get_ih_item_len(ih) / 4;
			__u32 *ptrs = tp_item_body(&path);
			if (!num_ptrs) {
				reiserfs_warning(stderr,
					 "indirect item %k contained 0 block pointers\n",
					 &ih->ih_key);
				ret = -EIO;
				goto fail;
			}
			ret = indirect_fn(fs, position, size, num_ptrs,
					  ptrs, data);
			if (ret)
				goto fail;
			position += num_ptrs * fs->fs_blocksize;
		} else if (is_direct_key(&ih->ih_key)) {
			int len = get_ih_item_len(ih);
			ret = direct_fn(fs, position, size, tp_item_body(&path),
					len, data);
			if (ret)
				goto fail;
			position += len;
		} else
			break;
		pathrelse(&path);
		set_key_offset_v2(&key, position + 1);
	}

	ret = 0;

fail:
	pathrelse(&path);
	return ret;
}

int reiserfs_iterate_dir(reiserfs_filsys_t fs,
			 const struct reiserfs_key const *dir_short_key,
			 const reiserfs_iterate_dir_fn callback, void *data)
{
	INITIALIZE_REISERFS_PATH(path);
	struct reiserfs_key *next_key;
	struct reiserfs_key min_key = {};
	struct reiserfs_de_head *deh;
	struct reiserfs_key next_item_key;
	int ret;
	__u64 next_pos = DOT_OFFSET;

	set_key_dirid(&next_item_key, get_key_dirid(dir_short_key));
	set_key_objectid(&next_item_key, get_key_objectid(dir_short_key));
	set_key_offset_v1(&next_item_key, DOT_OFFSET);
	set_key_uniqueness(&next_item_key, DIRENTRY_UNIQUENESS);

	while (1) {
		__u32 pos;
		int i;
		struct item_head *ih;
		ret = reiserfs_search_by_entry_key(fs, &next_item_key, &path);
		if (ret != POSITION_FOUND) {
			reiserfs_warning(stderr,
				"search by entry key for %k: %d\n",
				&next_item_key, ret);
			break;
	       }

		ret = 0;

		ih = tp_item_head(&path);
		deh = tp_item_body(&path);
		pos = path.pos_in_item;

		deh += pos;
		for (i = pos; i < get_ih_entry_count(ih); i++, deh++) {
			const char *name;
			size_t name_len;
			if (get_deh_offset(deh) == DOT_OFFSET ||
			    get_deh_offset(deh) == DOT_DOT_OFFSET)
				continue;

			name = tp_item_body(&path) + get_deh_location(deh);
			name_len = entry_length(ih, deh, i);
			if (!name[name_len - 1])
				name_len = strlen(name);

			ret = callback(fs, dir_short_key, name, name_len,
				       get_deh_dirid(deh),
				       get_deh_objectid(deh), data);
			if (ret)
				goto fail;

			next_pos = get_deh_offset(deh) + 1;
		}

		next_key = uget_rkey(&path);
		if (!next_key)
			break;
		if (!comp_keys(next_key, &min_key)) {
			set_key_offset_v2(&next_item_key, next_pos);
			pathrelse(&path);
			continue;
		}

		if (comp_short_keys(next_key, &next_item_key))
			break;

		next_item_key = *next_key;
		pathrelse(&path);
	}
fail:
	pathrelse(&path);
	return ret;
}

void __attribute__ ((constructor)) init(void)
{
	initialize_reiserfs_error_table();
}
