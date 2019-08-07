/*
 * Copyright 2002-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#define _GNU_SOURCE

#include "includes.h"
#include "progbar.h"

/* compares description block with commit block. returns 0 if they differ, 1
   if they match */
static int does_desc_match_commit(struct buffer_head *d_bh,
				  struct buffer_head *c_bh)
{
	return (get_commit_trans_id(c_bh) == get_desc_trans_id(d_bh) &&
		get_commit_trans_len(c_bh) == get_desc_trans_len(d_bh));
}

/* d_bh is descriptor, return number of block where commit block of this
   transaction is to be */
static unsigned long commit_expected(reiserfs_filsys_t fs,
				     struct buffer_head *d_bh)
{
	unsigned long offset;
	struct journal_params *sb_jp;

	sb_jp = sb_jp(fs->fs_ondisk_sb);
	//desc = (struct reiserfs_journal_desc *)d_bh->b_data;
	offset = d_bh->b_blocknr - get_jp_journal_1st_block(sb_jp);
	return get_jp_journal_1st_block(sb_jp) +
	    ((offset + get_desc_trans_len(d_bh) +
	      1) % get_jp_journal_size(sb_jp));
}

/* d_bh contains journal descriptor, returns number of block where descriptor
   block of next transaction should be */
static unsigned long next_desc_expected(reiserfs_filsys_t fs,
					struct buffer_head *d_bh)
{
	unsigned long offset;
	struct journal_params *sb_jp;

	sb_jp = sb_jp(fs->fs_ondisk_sb);
	//desc = (struct reiserfs_journal_desc *)d_bh->b_data;
	offset = d_bh->b_blocknr - get_jp_journal_1st_block(sb_jp);
	return get_jp_journal_1st_block(sb_jp) +
	    ((offset + get_desc_trans_len(d_bh) +
	      2) % get_jp_journal_size(sb_jp));
}

/* common checks for validness of a transaction */
static int transaction_check_content(reiserfs_filsys_t fs,
				     reiserfs_trans_t *trans)
{
	struct buffer_head *d_bh, *c_bh;
	struct reiserfs_journal_desc *desc;
	struct reiserfs_journal_commit *commit;
	unsigned long block;
	unsigned int trans_half, i;

	d_bh = bread(fs->fs_journal_dev, trans->desc_blocknr, fs->fs_blocksize);

	if (!d_bh || who_is_this(d_bh->b_data, d_bh->b_size) != THE_JDESC)
		goto error_desc_brelse;

	/* read expected commit block and compare with descriptor block */
	c_bh =
	    bread(fs->fs_journal_dev, commit_expected(fs, d_bh),
		  fs->fs_blocksize);
	if (!c_bh)
		goto error_desc_brelse;

	if (!does_desc_match_commit(d_bh, c_bh))
		goto error_commit_brelse;

	/* Check that all target blocks are journalable */
	desc = (struct reiserfs_journal_desc *)(d_bh->b_data);
	commit = (struct reiserfs_journal_commit *)(c_bh->b_data);

	trans_half = journal_trans_half(d_bh->b_size);
	for (i = 0; i < get_desc_trans_len(d_bh); i++) {
		if (i < trans_half)
			block = le32_to_cpu(desc->j2_realblock[i]);
		else
			block =
			    le32_to_cpu(commit->j3_realblock[i - trans_half]);

		if (not_journalable(fs, block))
			goto error_commit_brelse;
	}

	brelse(d_bh);
	brelse(c_bh);
	return 1;

error_commit_brelse:
	brelse(c_bh);
error_desc_brelse:
	brelse(d_bh);
	return 0;
}

/* common checks for validness of a transaction */
static int transaction_check_desc(reiserfs_filsys_t fs,
				  struct buffer_head *d_bh)
{
	struct buffer_head *c_bh;
	int ret = 1;

	if (!d_bh || who_is_this(d_bh->b_data, d_bh->b_size) != THE_JDESC)
		return 0;

	/* read expected commit block and compare with descriptor block */
	c_bh =
	    bread(fs->fs_journal_dev, commit_expected(fs, d_bh),
		  fs->fs_blocksize);
	if (!c_bh)
		return 0;

	if (!does_desc_match_commit(d_bh, c_bh))
		ret = 0;

	brelse(c_bh);
	return ret;
}

/* read the journal and find the oldest and newest transactions, return number
   of transactions found */
int get_boundary_transactions(reiserfs_filsys_t fs,
			      reiserfs_trans_t *oldest,
			      reiserfs_trans_t *newest)
{
	struct reiserfs_super_block *sb;
	unsigned long j_cur;
	unsigned long j_start;
	unsigned long j_size;
	struct buffer_head *d_bh;
	__u32 newest_trans_id, oldest_trans_id, trans_id;
	int trans_nr;

	sb = fs->fs_ondisk_sb;

	j_start = get_jp_journal_1st_block(sb_jp(sb));
	j_size = get_jp_journal_size(sb_jp(sb));

	oldest_trans_id = 0xffffffff;
	newest_trans_id = 0;

	trans_nr = 0;
	for (j_cur = 0; j_cur < j_size; j_cur++) {
		d_bh =
		    bread(fs->fs_journal_dev, j_start + j_cur,
			  fs->fs_blocksize);
		if (!transaction_check_desc(fs, d_bh)) {
			brelse(d_bh);
			continue;
		}

		trans_nr++;

		trans_id = get_desc_trans_id(d_bh);
		if (trans_id < oldest_trans_id) {
			oldest_trans_id = trans_id;

			oldest->mount_id = get_desc_mount_id(d_bh);
			oldest->trans_id = get_desc_trans_id(d_bh);
			oldest->desc_blocknr = d_bh->b_blocknr;
			oldest->trans_len = get_desc_trans_len(d_bh);
			oldest->commit_blocknr = commit_expected(fs, d_bh);
			oldest->next_trans_offset =
			    next_desc_expected(fs, d_bh) - j_start;
		}

		if (trans_id > newest_trans_id) {
			newest_trans_id = trans_id;

			newest->mount_id = get_desc_mount_id(d_bh);
			newest->trans_id = get_desc_trans_id(d_bh);
			newest->desc_blocknr = d_bh->b_blocknr;
			newest->trans_len = get_desc_trans_len(d_bh);
			newest->commit_blocknr = commit_expected(fs, d_bh);
			newest->next_trans_offset =
			    next_desc_expected(fs, d_bh) - j_start;
		}

		j_cur += get_desc_trans_len(d_bh) + 1;
		brelse(d_bh);
	}

	return trans_nr;
}

#define TRANS_FOUND     1
#define TRANS_NOT_FOUND 0

/* trans is a valid transaction. Look for valid transaction with smallest
   trans id which is greater than the id of the current one */
int next_transaction(reiserfs_filsys_t fs, reiserfs_trans_t *trans,
		     reiserfs_trans_t break_trans)
{
	struct buffer_head *d_bh, *next_d_bh;
	int found;
	unsigned long j_start;
	unsigned long j_offset;
	unsigned long block;

	j_start = get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb));

	found = TRANS_NOT_FOUND;

	if (trans->trans_id == break_trans.trans_id)
		return found;

	/* make sure that 'trans' is a valid transaction */
	d_bh = bread(fs->fs_journal_dev, trans->desc_blocknr, fs->fs_blocksize);
	if (!transaction_check_desc(fs, d_bh))
		die("next_transaction: valid transaction is expected");

	block = next_desc_expected(fs, d_bh);
	j_offset = block - j_start;

	while (1) {
		next_d_bh = bread(fs->fs_journal_dev, block, fs->fs_blocksize);
		if (transaction_check_desc(fs, next_d_bh))
			break;

		brelse(next_d_bh);
		j_offset++;
		block =
		    j_start +
		    (j_offset % get_jp_journal_size(sb_jp(fs->fs_ondisk_sb)));
	}

	//next_desc = (struct reiserfs_journal_desc *)next_d_bh->b_data;

	if (break_trans.trans_id >= get_desc_trans_id(next_d_bh)) {
		/* found transaction is newer */
		trans->mount_id = get_desc_mount_id(next_d_bh);
		trans->trans_id = get_desc_trans_id(next_d_bh);
		trans->desc_blocknr = next_d_bh->b_blocknr;
		trans->trans_len = get_desc_trans_len(next_d_bh);
		trans->commit_blocknr = commit_expected(fs, next_d_bh);
		trans->next_trans_offset =
		    next_desc_expected(fs, next_d_bh) - j_start;
		found = TRANS_FOUND;
	}

	brelse(d_bh);
	brelse(next_d_bh);
	return found;
}

static void read_journal_write_in_place(reiserfs_filsys_t fs,
					reiserfs_trans_t *trans,
					unsigned int index,
					unsigned long in_journal,
					unsigned long in_place)
{
	struct buffer_head *j_bh, *bh;

	j_bh = bread(fs->fs_journal_dev, in_journal, fs->fs_blocksize);
	if (!j_bh) {
		fprintf(stderr,
			"replay_one_transaction: transaction %lu: reading %lu block failed\n",
			trans->trans_id, in_journal);
		return;
	}
	if (not_journalable(fs, in_place)) {
		fprintf(stderr,
			"replay_one_transaction: transaction %lu: block %ld should not be journalled (%lu)\n",
			trans->trans_id, in_journal, in_place);
		brelse(j_bh);
		return;
	}

	bh = getblk(fs->fs_dev, in_place, fs->fs_blocksize);

	memcpy(bh->b_data, j_bh->b_data, bh->b_size);
	mark_buffer_dirty(bh);
	mark_buffer_uptodate(bh, 1);
	bwrite(bh);
	brelse(bh);
	brelse(j_bh);

}

/* go through all blocks of transaction and call 'action' each of them */
void for_each_block(reiserfs_filsys_t fs, reiserfs_trans_t *trans,
		    action_on_block_t action)
{
	struct buffer_head *d_bh, *c_bh;
	struct reiserfs_journal_desc *desc;
	struct reiserfs_journal_commit *commit;
	unsigned long j_start, j_offset, j_size;
	unsigned int i, trans_half;
	unsigned long block;

	d_bh = bread(fs->fs_journal_dev, trans->desc_blocknr, fs->fs_blocksize);
	if (!d_bh) {
		reiserfs_warning(stdout,
				 "reading descriptor block %lu failed\n",
				 trans->desc_blocknr);
		return;
	}

	c_bh =
	    bread(fs->fs_journal_dev, trans->commit_blocknr, fs->fs_blocksize);
	if (!c_bh) {
		reiserfs_warning(stdout, "reading commit block %lu failed\n",
				 trans->commit_blocknr);
		brelse(d_bh);
		return;
	}

	desc = (struct reiserfs_journal_desc *)(d_bh->b_data);
	commit = (struct reiserfs_journal_commit *)(c_bh->b_data);

	/* first block of journal and size of journal */
	j_start = get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb));
	j_size = get_jp_journal_size(sb_jp(fs->fs_ondisk_sb));

	/* offset in the journal where the transaction starts */
	j_offset = trans->desc_blocknr - j_start + 1;

	trans_half = journal_trans_half(d_bh->b_size);
	for (i = 0; i < trans->trans_len; i++, j_offset++) {
		if (i < trans_half)
			block = le32_to_cpu(desc->j2_realblock[i]);
		else
			block =
			    le32_to_cpu(commit->j3_realblock[i - trans_half]);
		action(fs, trans, i, j_start + (j_offset % j_size), block);
	}

	brelse(d_bh);
	brelse(c_bh);
}

/* transaction is supposed to be valid */
int replay_one_transaction(reiserfs_filsys_t fs, reiserfs_trans_t *trans)
{
	for_each_block(fs, trans, read_journal_write_in_place);
	fsync(fs->fs_dev);
	return 0;
}

void for_each_transaction(reiserfs_filsys_t fs, action_on_trans_t action)
{
	reiserfs_trans_t oldest, newest;
	int ret = 0;

	if (!get_boundary_transactions(fs, &oldest, &newest))
		return;

	while (1) {
		action(fs, &oldest);
		if ((ret =
		     next_transaction(fs, &oldest, newest)) == TRANS_NOT_FOUND)
			break;
	}
}

unsigned long get_size_of_journal_or_reserved_area(struct reiserfs_super_block
						   *sb)
{
	if (is_reiserfs_jr_magic_string(sb))
		return get_sb_reserved_for_journal(sb);

	/* with standard journal */
	return get_jp_journal_size(sb_jp(sb)) + 1;
}

__u32 advise_journal_max_trans_len(__u32 desired,
				   __u32 journal_size /* no j_header */ ,
				   int blocksize, int verbose)
{
	__u32 saved;
	__u32 ratio = 1;

	if (blocksize < 4096)
		ratio = 4096 / blocksize;

	saved = desired;
	if (!desired)
		desired = JOURNAL_TRANS_MAX / ratio;

	if (journal_size / desired < JOURNAL_MIN_RATIO)
		desired = journal_size / JOURNAL_MIN_RATIO;

	if (desired > JOURNAL_TRANS_MAX / ratio)
		desired = JOURNAL_TRANS_MAX / ratio;

	if (desired < JOURNAL_TRANS_MIN / ratio)
		desired = JOURNAL_TRANS_MIN / ratio;

	if (verbose) {
		if (saved && saved != desired)
			reiserfs_warning(stderr,
					 "WARNING: wrong transaction max size (%u). Changed to %u\n",
					 saved, desired);
	}

	return desired;
}

__u32 advise_journal_max_batch(unsigned long journal_trans_max)
{
	return journal_trans_max * JOURNAL_MAX_BATCH / JOURNAL_TRANS_MAX;
}

__u32 advise_journal_max_commit_age(void)
{
	return JOURNAL_MAX_COMMIT_AGE;
}

__u32 advise_journal_max_trans_age(void)
{
	return JOURNAL_MAX_TRANS_AGE;
}

int reiserfs_journal_params_check(reiserfs_filsys_t fs)
{
	struct reiserfs_journal_header *j_head;
	struct reiserfs_super_block *sb = fs->fs_ondisk_sb;

	j_head = (struct reiserfs_journal_header *)(fs->fs_jh_bh->b_data);

	/* Check the superblock's journal parameters. */
	if (!is_reiserfs_jr_magic_string(sb)) {
		if (get_jp_journal_dev(sb_jp(sb)) != 0 ||
		    get_jp_journal_1st_block(sb_jp(sb)) !=
		    get_journal_start_must(fs)
		    || get_jp_journal_size(sb_jp(sb)) !=
		    journal_default_size(fs->fs_super_bh->b_blocknr,
					 fs->fs_blocksize)) {
			reiserfs_warning(stderr,
					 "\nreiserfs_open_journal: wrong journal parameters found in the "
					 "super block. \nYou should run reiserfsck with --rebuild-sb to "
					 "check your superblock consistency.\n\n");

			return 1;
		}
	}

	if (memcmp
	    (&j_head->jh_journal, sb_jp(sb), sizeof(struct journal_params))) {
		if (!is_reiserfs_jr_magic_string(sb)) {
			reiserfs_warning(stderr,
					 "\nreiserfs_open_journal: journal parameters from the superblock "
					 "does not match \nto the journal headers ones. It looks like that "
					 "you created your fs with old\nreiserfsprogs. Journal header is "
					 "fixed.\n\n", fs->fs_j_file_name);

			memcpy(&j_head->jh_journal, sb_jp(sb),
			       sizeof(struct journal_params));
			mark_buffer_dirty(fs->fs_jh_bh);
			bwrite(fs->fs_jh_bh);
		} else {
			reiserfs_warning(stderr,
					 "\nreiserfs_open_journal: journal parameters from the super block "
					 "does not match \nto journal parameters from the journal. You should "
					 "run  reiserfsck with --rebuild-sb to check your superblock consistency.\n\n");

			return 1;
		}
	}

	return 0;
}

/* read journal header and make sure that it matches with the filesystem
   opened */
int reiserfs_open_journal(reiserfs_filsys_t fs, const char *j_filename,
			  int flags)
{
	struct reiserfs_super_block *sb;
	__u64 count;

	sb = fs->fs_ondisk_sb;

	if (!j_filename) {
		/*
		   if (is_reiserfs_jr_magic_string (sb)) {
		   // create a special file to access journal
		   return 1;
		   } */

		j_filename = fs->fs_file_name;
	} else if (!is_reiserfs_jr_magic_string(sb)) {
		/* make sure that name specified is a correct name */
		if (strcmp(j_filename, fs->fs_file_name)) {
			reiserfs_warning(stderr,
					 "Filesystem with standard journal found, "
					 "wrong name of specified journal device %s \n",
					 j_filename);
			return 2;
		}
	}

	fs->fs_journal_dev = open(j_filename, flags
#if defined(O_LARGEFILE)
				  | O_LARGEFILE
#endif
	    );
	if (fs->fs_journal_dev == -1)
		return -1;

	asprintf(&fs->fs_j_file_name, "%s", j_filename);

	if (get_jp_journal_size(sb_jp(sb)) < JOURNAL_MIN_SIZE) {
		reiserfs_warning(stderr, "Journal of (%lu) block size found on "
				 "specified journal device %s.\nMust be not less than (%lu).\n",
				 get_jp_journal_size(sb_jp(sb)) + 1, j_filename,
				 JOURNAL_MIN_SIZE + 1);
		close(fs->fs_journal_dev);
		return 1;
	}

	if (!(count = count_blocks(j_filename, fs->fs_blocksize))) {
		close(fs->fs_journal_dev);
		return -1;
	}

	if (get_jp_journal_1st_block(sb_jp(sb)) +
	    get_jp_journal_size(sb_jp(sb)) + 1 > count) {
		reiserfs_warning(stderr,
				 "Detected journal on specified device %s "
				 "does not fit to the device.\nStart block (%lu) + "
				 "size (%lu) less than device size (%lu).\n",
				 j_filename,
				 get_jp_journal_1st_block(sb_jp(sb)),
				 get_jp_journal_size(sb_jp(sb)) + 1, count);
		close(fs->fs_journal_dev);
		return 1;
	}

	/* read journal header */
	fs->fs_jh_bh = bread(fs->fs_journal_dev,
			     get_jp_journal_1st_block(sb_jp(sb)) +
			     get_jp_journal_size(sb_jp(sb)), fs->fs_blocksize);

	if (!fs->fs_jh_bh) {
		reiserfs_warning(stderr, "reiserfs_open_journal: bread failed "
				 "reading journal  header.\n");
		close(fs->fs_journal_dev);
		return -1;
	}

	return 0;
}

/* initialize super block's journal related fields and journal header fields.
 * If len is 0 - make journal of default size */
int reiserfs_create_journal(reiserfs_filsys_t fs, const char *j_device,	/* journal device name */
			    unsigned long offset,	/* journal offset on the j_device */
			    unsigned long len,	/* including journal header */
			    int transaction_max_size, int force)
{
	struct stat st;
	struct buffer_head *bh;
	struct reiserfs_journal_header *jh;
	struct reiserfs_super_block *sb;
	unsigned long blocks;

	sb = fs->fs_ondisk_sb;

	if (!j_device || !strcmp(j_device, fs->fs_file_name)) {
		/* Journal is to be on the host device, check the amount space for the
		 * journal on it. */
		len =
		    len ? len : journal_default_size(fs->fs_super_bh->b_blocknr,
						     fs->fs_blocksize) + 1;

		offset = offset ? offset : get_journal_start_must(fs);

		if (offset < get_journal_start_must(fs)) {
			reiserfs_warning(stderr,
					 "reiserfs_create_journal: offset is "
					 "%lu, but it cannot be less then %llu on the device %s\n",
					 offset, get_journal_start_must(fs),
					 j_device);
			return 0;
		}

		if (!is_block_count_correct(offset, fs->fs_blocksize,
					    get_sb_block_count(sb), len)) {
			/* host device does not contain enough blocks */
			reiserfs_warning(stderr,
					 "reiserfs_create_journal: cannot create "
					 "a journal of %lu blocks with %lu offset on %u blocks\n",
					 len, offset, get_sb_block_count(sb));
			return 0;
		}

		j_device = fs->fs_file_name;

		st.st_rdev = 0;
	} else {
		/* journal is to be on separate device */
		if (!(blocks = count_blocks(j_device, fs->fs_blocksize)))
			return 0;

		if (!len) {
			/* default size of a journal on a separate device is whole device */
			if (blocks < offset) {
				reiserfs_warning(stderr,
						 "reiserfs_create_journal: offset is "
						 "%lu, blocks on device %lu\n",
						 offset, blocks);
				return 0;
			}
			/* XXX jdm: This can end up being huge and could result
			 * in an unmountable file system:
			 * len = blocks - offset; */
			len = journal_default_size(fs->fs_super_bh->b_blocknr,
						   fs->fs_blocksize) + 1;

		}

		if (!force
		    && len > journal_default_size(fs->fs_super_bh->b_blocknr,
						  fs->fs_blocksize) + 1) {
			unsigned long journal_max =
			    journal_max_size(fs->fs_super_bh->b_blocknr,
					     fs->fs_blocksize);
			fflush(stderr);

			reiserfs_warning(stdout,
					 "\n*** You've specified a journal "
					 "size larger than the default size of "
					 "%lu\n*** blocks. This may slow down "
					 "journal initialization and mounting "
					 "of\n*** the file system.%s",
					 journal_default_size(fs->fs_super_bh->
							      b_blocknr,
							      fs->
							      fs_blocksize) + 1,
					 len > journal_max ? " " : "\n");
			if (len > journal_max)
				reiserfs_warning(stdout,
						 "On 32-bit systems, and on "
						 "64-bit systems with\n*** limited "
						 "memory, this may also cause the file "
						 "system to be unmountable.\n*** Please "
						 "consider using a journal size "
						 "<= %lu blocks.\n\nFile system creation "
						 "failed. You may override this behavior "
						 "with the -f option.\n",
						 journal_max);
			return 0;
		}

		if (len < blocks)
			reiserfs_warning(stdout,
					 "\n\n*** Your journal device is %lu "
					 "blocks, but your journal is only %lu "
					 "blocks.\n*** You may want to consider "
					 "resizing the journal device to avoid "
					 "wasting space.\n\n", blocks, len);

		if (blocks < offset + len) {
			reiserfs_warning(stderr,
					 "reiserfs_create_journal: no enough "
					 "blocks on device %lu, needed %lu\n",
					 blocks, offset + len);
			return 0;
		}

		if (stat(j_device, &st) == -1) {
			reiserfs_warning(stderr,
					 "reiserfs_create_journal: stat %s failed"
					 ": %s\n", j_device, strerror(errno));
			return 0;
		}
/*
	if (!S_ISBLK (st.st_mode)) {
		reiserfs_warning (stderr, "reiserfs_create_journal: "
		"%s is not a block device (%x)\n", j_device, st.st_rdev);
		return 0;
	}
*/
	}

	fs->fs_journal_dev = open(j_device, O_RDWR
#if defined(O_LARGEFILE)
				  | O_LARGEFILE
#endif
	    );
	if (fs->fs_journal_dev == -1) {
		reiserfs_warning(stderr,
				 "reiserfs_create_journal: could not open "
				 "%s: %s\n", j_device, strerror(errno));
		return 0;
	}

	asprintf(&fs->fs_j_file_name, "%s", j_device);

	if (len < JOURNAL_MIN_SIZE + 1) {
		reiserfs_warning(stderr,
				 "WARNING: Journal size (%u) is less, than "
				 "minimal supported journal size (%u).\n", len,
				 JOURNAL_MIN_SIZE + 1);
		return 0;
	}
	/* get journal header */
	bh = getblk(fs->fs_journal_dev, offset + len - 1, fs->fs_blocksize);
	if (!bh) {
		reiserfs_warning(stderr,
				 "reiserfs_create_journal: getblk failed\n");
		return 0;
	}

	/* fill journal header */
	jh = (struct reiserfs_journal_header *)bh->b_data;
	set_jp_journal_1st_block(&jh->jh_journal, offset);
	set_jp_journal_dev(&jh->jh_journal, st.st_rdev);
	set_jp_journal_magic(&jh->jh_journal, get_random());

	set_jp_journal_size(&jh->jh_journal, len - 1);
	set_jp_journal_max_trans_len(&jh->jh_journal,
				     advise_journal_max_trans_len
				     (transaction_max_size, len - 1,
				      fs->fs_blocksize, 1));
	set_jp_journal_max_batch(&jh->jh_journal,
				 advise_journal_max_batch
				 (get_jp_journal_max_trans_len
				  (&jh->jh_journal)));
	set_jp_journal_max_commit_age(&jh->jh_journal,
				      advise_journal_max_commit_age());
	set_jp_journal_max_trans_age(&jh->jh_journal,
				     advise_journal_max_trans_age());

	mark_buffer_uptodate(bh, 1);
	mark_buffer_dirty(bh);

	fs->fs_jh_bh = bh;

	/* make a copy of journal header in the super block */
	memcpy(sb_jp(sb), &jh->jh_journal, sizeof(struct journal_params));
	mark_buffer_dirty(fs->fs_super_bh);

	return 1;
}

/* brelse journal header, flush all dirty buffers, close device, open, read
   journal header */
void reiserfs_reopen_journal(reiserfs_filsys_t fs, int flag)
{
	unsigned long jh_block;

	if (!reiserfs_journal_opened(fs))
		return;

	jh_block = fs->fs_jh_bh->b_blocknr;
	brelse(fs->fs_jh_bh);
	flush_buffers(fs->fs_journal_dev);
	invalidate_buffers(fs->fs_journal_dev);
	if (close(fs->fs_journal_dev))
		die("reiserfs_reopen_journal: closed failed: %s",
		    strerror(errno));

	fs->fs_journal_dev = open(fs->fs_j_file_name, flag
#if defined(O_LARGEFILE)
				  | O_LARGEFILE
#endif
	    );
	if (fs->fs_journal_dev == -1)
		die("reiserfs_reopen_journal: could not reopen journal device");

	fs->fs_jh_bh = bread(fs->fs_journal_dev, jh_block, fs->fs_blocksize);
	if (!fs->fs_jh_bh)
		die("reiserfs_reopen_journal: reading journal header failed");
}

int reiserfs_journal_opened(reiserfs_filsys_t fs)
{
	return fs->fs_jh_bh ? 1 : 0;
}

void reiserfs_flush_journal(reiserfs_filsys_t fs)
{
	if (!reiserfs_journal_opened(fs))
		return;
	flush_buffers(fs->fs_journal_dev);
}

void reiserfs_free_journal(reiserfs_filsys_t fs)
{
	if (!reiserfs_journal_opened(fs))
		return;
	brelse(fs->fs_jh_bh);
	fs->fs_jh_bh = NULL;
	free(fs->fs_j_file_name);
	fs->fs_j_file_name = NULL;
}

void reiserfs_close_journal(reiserfs_filsys_t fs)
{
	reiserfs_flush_journal(fs);
	reiserfs_free_journal(fs);

}

/* update journal header */
static void update_journal_header(reiserfs_filsys_t fs,
				  struct buffer_head *bh_jh,
				  reiserfs_trans_t *trans)
{
	struct reiserfs_journal_header *j_head;

	j_head = (struct reiserfs_journal_header *)(bh_jh->b_data);

	/* update journal header */
	set_jh_last_flushed(j_head, trans->trans_id);
	set_jh_mount_id(j_head, trans->mount_id);
	set_jh_replay_start_offset(j_head, trans->next_trans_offset);
	mark_buffer_dirty(bh_jh);
	bwrite(bh_jh);
	fsync(fs->fs_journal_dev);
}

/* fixme: what should be done when not all transactions can be replayed in proper order? */
int reiserfs_replay_journal(reiserfs_filsys_t fs)
{
	struct buffer_head *bh;
	struct reiserfs_journal_header *j_head;
	reiserfs_trans_t cur, newest, control;
	int replayed, ret;
	struct progbar progbar;
	int trans_count;

	if (!reiserfs_journal_opened(fs))
		reiserfs_panic("replay_journal: journal is not opened");

	if (!is_opened_rw(fs))
		reiserfs_panic
		    ("replay_journal: fs is not opened with write perms");

	reiserfs_warning(stderr, "Replaying journal: ");
	bh = fs->fs_jh_bh;

	j_head = (struct reiserfs_journal_header *)(bh->b_data);
	control.mount_id = get_jh_mount_id(j_head);
	control.trans_id = get_jh_last_flushed(j_head);
	control.desc_blocknr = get_jh_replay_start_offset(j_head);

	trans_count = get_boundary_transactions(fs, &cur, &newest);
	if (!trans_count) {
		reiserfs_warning(stderr, "No transactions found\n");
		return 0;
	}

	/*  Smth strange with journal header or journal. We cannot say for sure what was the last
	   replaied transaction, but relying on JH data is preferable. */

	replayed = 0;
	ret = TRANS_FOUND;

	progbar_init(&progbar, " trans", stderr);
	/* Looking to the first valid not replayed transaction. */
	while (1) {
		if (cur.mount_id == control.mount_id &&
		    cur.trans_id > control.trans_id)
			break;

		if ((ret = next_transaction(fs, &cur, newest)) != TRANS_FOUND)
			break;
	}

	while (ret == TRANS_FOUND) {
		/* If not the next transaction to be replayed, break out here. */
		if ((cur.mount_id != control.mount_id) ||
		    (cur.trans_id != control.trans_id + 1 && control.trans_id))
			break;

		if (!transaction_check_content(fs, &cur)) {
			progbar_clear(&progbar);
			reiserfs_warning(stderr,
					 "Trans broken: mountid %lu, transid %lu, desc %lu, "
					 "len %lu, commit %lu, next trans offset %lu\n",
					 cur.mount_id, cur.trans_id,
					 cur.desc_blocknr, cur.trans_len,
					 cur.commit_blocknr,
					 cur.next_trans_offset);
			break;
		}

		reiserfs_warning(stderr,
				 "Trans replayed: mountid %lu, transid %lu, desc %lu, "
				 "len %lu, commit %lu, next trans offset %lu\n",
				 cur.mount_id, cur.trans_id, cur.desc_blocknr,
				 cur.trans_len, cur.commit_blocknr,
				 cur.next_trans_offset);
		replay_one_transaction(fs, &cur);
		update_journal_header(fs, bh, &cur);
		control = cur;
		replayed++;

		progbar_update(&progbar, "Replaying journal", replayed,
			       trans_count, replayed);

		ret = next_transaction(fs, &cur, newest);
	}
	progbar_clear(&progbar);

	reiserfs_warning(stderr,
			 "\rReplaying journal: Done.\nReiserfs journal '%s' in blocks [%u..%u]: %d "
			 "transactions replayed\n", fs->fs_j_file_name,
			 get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb)),
			 get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb)) +
			 get_jp_journal_size(sb_jp(fs->fs_ondisk_sb)) + 1,
			 replayed);

	mark_buffer_dirty(fs->fs_super_bh);
	bwrite(fs->fs_super_bh);

	update_journal_header(fs, bh, &newest);

	return 0;
}
