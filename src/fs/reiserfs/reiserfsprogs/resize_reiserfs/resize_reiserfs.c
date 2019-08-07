/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

/*
 * Written by Alexander Zarochentcev.
 *
 * FS resize utility
 *
 */

#define _GNU_SOURCE

#include "resize.h"
#include <limits.h>

static int opt_banner = 0;
static int opt_skipj = 0;
int opt_force = 0;
int opt_verbose = 1;		/* now "verbose" option is default */
int opt_nowrite = 0;
int opt_safe = 0;

char *g_progname;

/* calculate the new fs size (in blocks) from old fs size and the string
   representation of new size */
static long long int calc_new_fs_size(unsigned long count,
				      unsigned int bs, char *bytes_str)
{
	long long int bytes;
	long long int blocks;
	char *end;
	int rel;

	end = bytes_str + strlen(bytes_str) - 1;
	rel = bytes_str[0] == '+' || bytes_str[0] == '-';
	bytes = strtoll(bytes_str, &bytes_str, 10);

	/* Some error occured while convertion or the specified
	   string is not valid. */
	if (bytes == LONG_LONG_MIN || bytes == LONG_LONG_MAX ||
	    (bytes_str != end && bytes_str != end + 1))
		return -EINVAL;

	switch (*end) {
	case 'G':
	case 'g':
		bytes *= 1024;
	case 'M':
	case 'm':
		bytes *= 1024;
	case 'K':
	case 'k':
		bytes *= 1024;
	}

	blocks = bytes / bs;

	return rel ? count + blocks : blocks;
}

/* print some fs parameters */
static void sb_report(struct reiserfs_super_block *sb1,
		      struct reiserfs_super_block *sb2)
{
	printf("ReiserFS report:\n"
	       "blocksize             %u\n"
	       "block count           %u (%u)\n"
	       "free blocks           %u (%u)\n"
	       "bitmap block count    %u (%u)\n",
	       get_sb_block_size(sb1),
	       get_sb_block_count(sb1), get_sb_block_count(sb2),
	       get_sb_free_blocks(sb1), get_sb_free_blocks(sb2),
	       get_sb_bmap_nr(sb1), get_sb_bmap_nr(sb2));
};

/* conditional bwrite */
static int bwrite_cond(struct buffer_head *bh)
{
	if (!opt_nowrite) {
		mark_buffer_uptodate(bh, 1);
		mark_buffer_dirty(bh);
		bwrite(bh);
	}
	return 0;
}

/* the first one of the most important functions */
static int expand_fs(reiserfs_filsys_t fs, long long int block_count_new)
{
	unsigned int bmap_nr_new, bmap_nr_old;
	struct reiserfs_super_block *sb;
	unsigned int i;

	reiserfs_reopen(fs, O_RDWR);
	if (reiserfs_open_ondisk_bitmap(fs))
		reiserfs_exit(1, "cannot open ondisk bitmap");

	sb = fs->fs_ondisk_sb;

	set_sb_fs_state(fs->fs_ondisk_sb, FS_ERROR);

	bwrite_cond(fs->fs_super_bh);

	if (reiserfs_expand_bitmap(fs->fs_bitmap2, block_count_new))
		reiserfs_exit(1, "cannot expand bitmap\n");

	/* count bitmap blocks in new fs */
	bmap_nr_new = (block_count_new - 1) / (fs->fs_blocksize * 8) + 1;
	bmap_nr_old = reiserfs_fs_bmap_nr(fs);

	/* update super block buffer */
	set_sb_free_blocks(sb, get_sb_free_blocks(sb) +
			   (block_count_new - get_sb_block_count(sb)) -
			   (bmap_nr_new - bmap_nr_old));
	set_sb_block_count(sb, block_count_new);

	set_sb_bmap_nr(fs->fs_ondisk_sb,
		       reiserfs_bmap_over(bmap_nr_new) ? 0 : bmap_nr_new);

	/* mark new bitmap blocks as used */
	for (i = bmap_nr_old; i < bmap_nr_new; i++)
		reiserfs_bitmap_set_bit(fs->fs_bitmap2,
					i * fs->fs_blocksize * 8);

	/* normally, this is done by reiserfs_bitmap_set_bit, but if we
	 ** haven't actually added any bitmap blocks, the bitmap won't be dirtied.
	 **
	 ** In memory, reiserfsprogs puts zeros for the bits past the end of
	 ** the old filesystem.  But, on disk that bitmap is full of ones.
	 ** we explicitly dirty the bitmap here to make sure the zeros get written
	 ** to disk
	 */
	fs->fs_bitmap2->bm_dirty = 1;

	return 0;
}

static int resizer_check_fs_size(reiserfs_filsys_t fs, long long int new_size)
{
	if (new_size < 0) {
		reiserfs_warning(stderr, "\nresizer_reiserfs: the new size "
				 "value is wrong.\n\n");
		return new_size;
	}

	if (new_size == get_sb_block_count(fs->fs_ondisk_sb)) {
		reiserfs_warning(stderr, "%s already is of the needed size. "
				 "Nothing to be done\n\n", fs->fs_file_name);
		return 1;
	}

	if (new_size < get_sb_block_count(fs->fs_ondisk_sb)) {
		if (misc_device_mounted(fs->fs_file_name) > 0) {
			reiserfs_warning(stderr,
					 "Can't shrink filesystem on-line.\n\n");
			return 1;
		}
	}

	if (new_size >= get_sb_block_count(fs->fs_ondisk_sb)) {
		loff_t offset = (loff_t) new_size * fs->fs_blocksize - 1;

		if (!valid_offset(fs->fs_dev, offset)) {
			reiserfs_warning(stderr,
					 "%s is of %lu blocks size only with "
					 "reiserfs of %u blocks\nsize on it. You are "
					 "trying to expand reiserfs up to %lu blocks "
					 "size.\nYou probably forgot to expand your "
					 "partition size.\n\n",
					 fs->fs_file_name,
					 count_blocks(fs->fs_file_name,
						      fs->fs_blocksize),
					 get_sb_block_count(fs->fs_ondisk_sb),
					 new_size);
			return 1;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	char *bytes_count_str = NULL;
	char *devname;
	char *jdevice_name = NULL;
	reiserfs_filsys_t fs;
	struct reiserfs_super_block *sb;

	int c;
	long error;

	struct reiserfs_super_block *sb_old;

	long long int block_count_new;

	g_progname = basename(argv[0]);

	if (argc < 2)
		print_usage_and_exit();

	while ((c = getopt(argc, argv, "fvcqks:j:V")) != EOF) {
		switch (c) {
		case 's':
			if (!optarg)
				reiserfs_exit(1,
					      "Missing argument to -s option");
			bytes_count_str = optarg;
			break;
		case 'j':
			if (!optarg)
				reiserfs_exit(1,
					      "Missing argument to -j option");
			jdevice_name = optarg;
		case 'f':
			opt_force = 1;
			break;
		case 'v':
			opt_verbose++;
			break;
		case 'n':
			/* no nowrite option at this moment */
			/* opt_nowrite = 1; */
			break;
		case 'c':
			opt_safe = 1;
			break;
		case 'q':
			opt_verbose = 0;
			break;
		case 'k':
			opt_skipj = 1;
			break;
		case 'V':
			opt_banner++;
			break;
		default:
			print_usage_and_exit();
		}
	}

	print_banner(g_progname);

	if (opt_banner)
		exit(0);

	devname = argv[optind];

	fs = reiserfs_open(devname, O_RDONLY, &error, NULL, 1);
	if (!fs) {
		if (error) {
			reiserfs_exit(1, "cannot open '%s': %s",
				      devname, error_message(error));
		} else {
			exit(1);
		}
	}

	if (reiserfs_open_journal(fs, jdevice_name, O_RDWR | O_LARGEFILE)) {
		reiserfs_exit(1, "Failed to open the journal device (%s).",
			      jdevice_name);
	}

	if (reiserfs_journal_params_check(fs)) {
		if (!opt_skipj) {
			reiserfs_exit(1,
				      "Wrong journal parameters detected on (%s)",
				      jdevice_name);
		} else {
			reiserfs_close_journal(fs);
		}
	}

	/* forced to continue without journal available/specified */

	if (no_reiserfs_found(fs)) {
		reiserfs_exit(1, "no reiserfs found on the device.");
	}

	if (!spread_bitmaps(fs)) {
		reiserfs_exit(1, "cannot resize reiserfs in old (not spread "
			      "bitmap) format.");
	}

	sb = fs->fs_ondisk_sb;

	/* If size change was specified by user, calculate it,
	   otherwise take the whole device. */
	block_count_new = bytes_count_str ?
	    calc_new_fs_size(get_sb_block_count(sb),
			     fs->fs_blocksize, bytes_count_str) :
	    count_blocks(devname, fs->fs_blocksize);

	if (resizer_check_fs_size(fs, block_count_new))
		return 1;

	if (misc_device_mounted(devname) > 0) {
		reiserfs_close(fs);
		error = resize_fs_online(devname, block_count_new);
		reiserfs_warning(stderr,
				 "\n\nresize_reiserfs: On-line resizing %s.\n\n",
				 error ? "failed" : "finished successfully");
		return error;
	}

	if (!reiserfs_is_fs_consistent(fs)) {
		reiserfs_warning(stderr,
				 "\n\nresize_reiserfs: run reiserfsck --check "
				 "first\n\n");
		reiserfs_close(fs);
		return 1;
	}

	if (get_sb_umount_state(sb) != FS_CLEANLY_UMOUNTED)
		/* fixme: shouldn't we check for something like: fsck guarantees: fs is ok */
		reiserfs_exit(1, "the file system isn't in valid state.");

	/* Needed to keep idiot compiler from issuing false warning */
	sb_old = NULL;

	/* save SB for reporting */
	if (opt_verbose) {
		sb_old = getmem(SB_SIZE);
		memcpy(sb_old, fs->fs_ondisk_sb, SB_SIZE);
	}

	error = (block_count_new > get_sb_block_count(fs->fs_ondisk_sb)) ?
	    expand_fs(fs, block_count_new) : shrink_fs(fs, block_count_new);
	if (error) {
		reiserfs_warning(stderr,
				 "\n\nresize_reiserfs: Resizing failed.\n\n ");
		return error;
	}

	if (opt_verbose) {
		sb_report(fs->fs_ondisk_sb, sb_old);
		freemem(sb_old);
	}

	set_sb_fs_state(fs->fs_ondisk_sb, FS_CONSISTENT);
	bwrite_cond(fs->fs_super_bh);

	if (opt_verbose) {
		printf("\nSyncing..");
		fflush(stdout);
	}
	reiserfs_close(fs);
	if (opt_verbose)
		printf("done\n");

	reiserfs_warning(stderr, "\n\nresize_reiserfs: Resizing finished "
			 "successfully.\n\n ");

	return 0;
}
