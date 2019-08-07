/* * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

/* mkreiserfs is very simple. It skips first 64k of device, and then
   writes the super block, the needed amount of bitmap blocks (this
   amount is calculated based on file system size), and root block.
   Bitmap policy is primitive: it assumes, that device does not have
   unreadable blocks, and it occupies first blocks for super, bitmap
   and root blocks. bitmap blocks are interleaved across the disk,
   mainly to make resizing faster. */

#define _GNU_SOURCE

#ifndef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "io.h"
#include "misc.h"
#include "reiserfs_lib.h"

#include "../version.h"

#include <getopt.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/utsname.h>

#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
#  include <uuid/uuid.h>
#endif

static char *program_name;

static void message(const char *fmt, ...)
    __attribute__ ((format(printf, 1, 2)));

static void message(const char *fmt, ...)
{
	char *buf;
	va_list args;

	buf = NULL;
	va_start(args, fmt);
	vasprintf(&buf, fmt, args);
	va_end(args);

	if (buf) {
		fprintf(stderr, "%s: %s\n", program_name, buf);
		free(buf);
	}
}

static void print_usage_and_exit(void)
{
	fprintf(stderr, "Usage: %s [options] "
		" device [block-count]\n"
		"\n"
		"Options:\n\n"
		"  -b | --block-size N              size of file-system block, in bytes\n"
		"  -j | --journal-device FILE       path to separate device to hold journal\n"
		"  -s | --journal-size N            size of the journal in blocks\n"
		"  -o | --journal-offset N          offset of the journal from the start of\n"
		"                                   the separate device, in blocks\n"
		"  -t | --transaction-max-size N    maximal size of transaction, in blocks\n"
		"  -B | --badblocks file            store all bad blocks given in file on the fs\n"
		"  -h | --hash rupasov|tea|r5       hash function to use by default\n"
		"  -u | --uuid UUID                 store UUID in the superblock\n"
		"  -l | --label LABEL               store LABEL in the superblock\n"
		"  --format 3.5|3.6                 old 3.5 format or newer 3.6\n"
		"  -f | --force                     specified once, make mkreiserfs the whole\n"
		"                                   disk, not block device or mounted partition;\n"
		"                                   specified twice, do not ask for confirmation\n"
		"  -q | --quiet                     quiet work without messages, progress and\n"
		"                                   questions. Useful if run in a script. For use\n"
		"                                   by end users only.\n"
		"  -d | --debug                     print debugging information during mkreiser\n"
		"  -V                               print version and exit\n",
		program_name);
	exit(1);
}

static int Create_default_journal = 1;
static int Block_size = 4096;

/* size of journal + 1 block for journal header */
static unsigned long Journal_size = 0;
static int Max_trans_size = 0;		//JOURNAL_TRANS_MAX;
static int Hash = DEFAULT_HASH;
static int Offset = 0;
static char *Format;
static unsigned char UUID[16];
static char *LABEL = NULL;
static char *badblocks_file;

enum mkfs_mode {
	DEBUG_MODE = 1 << 0,
	QUIET_MODE = 1 << 1,
	DO_NOTHING = 1 << 2
};

static int mode;

/* form super block (old one) */
static void make_super_block(reiserfs_filsys_t fs)
{
	set_sb_umount_state(fs->fs_ondisk_sb, FS_CLEANLY_UMOUNTED);
	set_sb_tree_height(fs->fs_ondisk_sb, 2);
	set_sb_hash_code(fs->fs_ondisk_sb, Hash);
	if (fs->fs_format == REISERFS_FORMAT_3_6) {
#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
		if (uuid_is_null(UUID))
			uuid_generate(UUID);

		memcpy(fs->fs_ondisk_sb->s_uuid, UUID, 16);
#endif
		if (LABEL != NULL) {
			if (strlen(LABEL) > 16)
				reiserfs_warning(stderr,
						 "\nSpecified LABEL is longer then 16 "
						 "characters, will be truncated\n\n");
			strncpy(fs->fs_ondisk_sb->s_label, LABEL, 16);
		}
		set_sb_v2_flag(fs->fs_ondisk_sb, reiserfs_attrs_cleared);
	}

	if (!is_reiserfs_jr_magic_string(fs->fs_ondisk_sb) ||
	    strcmp(fs->fs_file_name, fs->fs_j_file_name))
		/* either standard journal (and we leave all new fields to be 0) or
		   journal is created on separate device so there is no space on data
		   device which can be used as a journal */
		set_sb_reserved_for_journal(fs->fs_ondisk_sb, 0);
	else
		set_sb_reserved_for_journal(fs->fs_ondisk_sb,
					    get_jp_journal_size(sb_jp
								(fs->
								 fs_ondisk_sb))
					    + 1);

	if (fs->fs_badblocks_bm)
		set_sb_free_blocks(fs->fs_ondisk_sb,
				   get_sb_free_blocks(fs->fs_ondisk_sb) -
				   fs->fs_badblocks_bm->bm_set_bits);
}

/* wipe out first 64 k of a device and both possible reiserfs super block */
static void invalidate_other_formats(int dev)
{
	struct buffer_head *bh;

	bh = bread(dev, 0, 64 * 1024);
	if (!bh) {
		reiserfs_exit(1, "Unable to read first blocks of the device");
	}
#if defined(__sparc__) || defined(__sparc_v9__)
	memset(bh->b_data + 1024, 0, bh->b_size - 1024);
#else
	memset(bh->b_data, 0, bh->b_size);
#endif
	mark_buffer_uptodate(bh, 1);
	mark_buffer_dirty(bh);
	bwrite(bh);
	brelse(bh);
}

static void zero_journal(reiserfs_filsys_t fs)
{
	unsigned long start, len, done;
	struct buffer_head *bh;
	unsigned int i;

	fprintf(stdout, "Initializing journal - ");

	start = get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb));
	len = get_jp_journal_size(sb_jp(fs->fs_ondisk_sb));

	done = 0;
	for (i = 0; i < len; i++) {
		print_how_far(stdout, &done, len, 1, 1 /*be quiet */ );
		bh = getblk(fs->fs_journal_dev, start + i, fs->fs_blocksize);
		if (!bh) {
			reiserfs_exit(1, "zero_journal: getblk failed");
		}
		memset(bh->b_data, 0, bh->b_size);
		mark_buffer_dirty(bh);
		mark_buffer_uptodate(bh, 1);
		bwrite(bh);
		brelse(bh);
	}

	fprintf(stdout, "\n");
	fflush(stdout);
}

/* this only sets few first bits in bitmap block. Fills not initialized fields
   of super block (root block and bitmap block numbers) */
static void make_bitmap(reiserfs_filsys_t fs)
{
	struct reiserfs_super_block *sb = fs->fs_ondisk_sb;
	unsigned int i;
	unsigned long block;
	int marked;

	marked = 0;

	/* mark skipped area and super block */
	for (i = 0; i <= fs->fs_super_bh->b_blocknr; i++) {
		reiserfs_bitmap_set_bit(fs->fs_bitmap2, i);
		marked++;
	}

	if (fs->fs_badblocks_bm) {
		for (i = 0; i < get_sb_block_count(sb); i++) {
			if (reiserfs_bitmap_test_bit(fs->fs_badblocks_bm, i)) {
				reiserfs_bitmap_set_bit(fs->fs_bitmap2, i);
				marked++;
			}
		}
	}

	/* mark bitmaps as used */
	block = fs->fs_super_bh->b_blocknr + 1;

	for (i = 0; i < reiserfs_fs_bmap_nr(fs); i++) {
		reiserfs_bitmap_set_bit(fs->fs_bitmap2, block);
		marked++;
		if (spread_bitmaps(fs))
			block = (block / (fs->fs_blocksize * 8) + 1) *
			    (fs->fs_blocksize * 8);
		else
			block++;
	}

	if (!get_size_of_journal_or_reserved_area(fs->fs_ondisk_sb))
		/* root block follows directly super block and first bitmap */
		block = fs->fs_super_bh->b_blocknr + 1 + 1;
	else {
		/* makr journal blocks as used */
		for (i = 0; i <= get_jp_journal_size(sb_jp(sb)); i++) {
			reiserfs_bitmap_set_bit(fs->fs_bitmap2,
						i +
						get_jp_journal_1st_block(sb_jp
									 (sb)));
			marked++;
		}
		block = get_jp_journal_1st_block(sb_jp(sb)) + i;
	}

	/*get correct block - not journal nor bitmap */
	while (block_of_journal(fs, block) || block_of_bitmap(fs, block)) {
		block++;
	}

	while ((block < get_sb_block_count(sb)) &&
	       reiserfs_bitmap_test_bit(fs->fs_bitmap2, block)) {
		block++;
	}

	if (block >= get_sb_block_count(sb))
		reiserfs_exit(1, "mkreiserfs: too many bad blocks");

	reiserfs_bitmap_set_bit(fs->fs_bitmap2, block);
	marked++;

	set_sb_root_block(sb, block);
	set_sb_free_blocks(sb, get_sb_block_count(sb) - marked);
}

static void set_root_dir_nlink(struct item_head *ih, void *sd)
{
	__u32 nlink;

	nlink = 3;
	set_sd_nlink(ih, sd, &nlink);
}

/* form the root block of the tree (the block head, the item head, the
   root directory) */
static void make_root_block(reiserfs_filsys_t fs)
{
	struct reiserfs_super_block *sb;
	struct buffer_head *bh;

	sb = fs->fs_ondisk_sb;
	/* get memory for root block */
	bh = getblk(fs->fs_dev, get_sb_root_block(sb), get_sb_block_size(sb));

	if (!bh) {
		reiserfs_exit(1, "getblk failed");
	}

	mark_buffer_uptodate(bh, 1);

	make_empty_leaf(bh);
	make_sure_root_dir_exists(fs, set_root_dir_nlink, 0);
	brelse(bh);

	mark_objectid_used(fs, REISERFS_ROOT_PARENT_OBJECTID);
	mark_objectid_used(fs, REISERFS_ROOT_OBJECTID);
}

static void report(reiserfs_filsys_t fs, char *j_filename)
{
//    print_block (stdout, fs, fs->fs_super_bh);
	struct reiserfs_super_block *sb =
	    (struct reiserfs_super_block *)(fs->fs_super_bh->b_data);

	struct stat st;
	dev_t rdev;

	if (!is_any_reiserfs_magic_string(sb))
		return;

	if (fstat(fs->fs_super_bh->b_dev, &st) == -1) {
		/*reiserfs_warning (stderr, "fstat failed: %s\n", strerror(errno)); */
		rdev = 0;
	} else
		rdev = st.st_rdev;

	if (mode & DEBUG_MODE) {
		reiserfs_warning(stdout,
				 "Block %lu (0x%x) contains super block. ",
				 fs->fs_super_bh->b_blocknr, rdev);
	}
	switch (get_reiserfs_format(sb)) {
	case REISERFS_FORMAT_3_5:
		reiserfs_warning(stdout, " Format 3.5 with ");
		break;
	case REISERFS_FORMAT_3_6:
		reiserfs_warning(stdout, "Format 3.6 with ");
		break;
	}
	if (is_reiserfs_jr_magic_string(sb))
		reiserfs_warning(stdout, "non-");
	reiserfs_warning(stdout, "standard journal\n");
	reiserfs_warning(stdout, "Count of blocks on the device: %u\n",
			 get_sb_block_count(sb));
	reiserfs_warning(stdout, "Number of blocks consumed by mkreiserfs "
			 "formatting process: %u\n", get_sb_block_count(sb)
			 - get_sb_free_blocks(sb));
	if (mode & DEBUG_MODE)
		reiserfs_warning(stdout, "Free blocks: %u\n",
				 get_sb_free_blocks(sb));
	reiserfs_warning(stdout, "Blocksize: %d\n", get_sb_block_size(sb));
	reiserfs_warning(stdout, "Hash function used to sort names: %s\n",
			 code2name(get_sb_hash_code(sb)));
	if (mode & DEBUG_MODE) {
		reiserfs_warning(stdout, "Number of bitmaps: %u",
				 get_sb_bmap_nr(sb));
		if (get_sb_bmap_nr(sb) != reiserfs_fs_bmap_nr(fs))
			reiserfs_warning(stdout, " (really uses %u)",
					 reiserfs_fs_bmap_nr(fs));
		reiserfs_warning(stdout, "\nRoot block: %u\n",
				 get_sb_root_block(sb));
		reiserfs_warning(stdout, "Tree height: %d\n",
				 get_sb_tree_height(sb));
		reiserfs_warning(stdout, "Objectid map size %d, max %d\n",
				 get_sb_oid_cursize(sb),
				 get_sb_oid_maxsize(sb));
		reiserfs_warning(stdout, "Journal parameters:\n");
		print_journal_params(stdout, sb_jp(sb));
	} else {
		if (j_filename && strcmp(j_filename, fs->fs_file_name))
			reiserfs_warning(stdout, "Journal Device [0x%x]\n",
					 get_jp_journal_dev(sb_jp(sb)));

		reiserfs_warning(stdout,
				 "Journal Size %u blocks (first block %u)\n",
				 get_jp_journal_size(sb_jp(sb)) + 1,
				 get_jp_journal_1st_block(sb_jp(sb)));
		reiserfs_warning(stdout, "Journal Max transaction length %u\n",
				 get_jp_journal_max_trans_len(sb_jp(sb)));
	}

	if (j_filename && strcmp(j_filename, fs->fs_file_name)) {
		reiserfs_warning(stdout,
				 "Space on this device reserved by journal: "
				 "%u\n", get_sb_reserved_for_journal(sb));
	}

	if (mode & DEBUG_MODE) {
		reiserfs_warning(stdout, "Filesystem state 0x%x\n",
				 get_sb_fs_state(sb));
		reiserfs_warning(stdout, "sb_version %u\n", get_sb_version(sb));
	}

	if (get_reiserfs_format(sb) == REISERFS_FORMAT_3_6) {
		reiserfs_warning(stdout, "inode generation number: %u\n",
				 get_sb_v2_inode_generation(sb));
		reiserfs_warning(stdout, "UUID: %U\n", sb->s_uuid);
		if (strcmp(sb->s_label, ""))
			reiserfs_warning(stdout, "LABEL: %s\n", sb->s_label);
	}

	return;
}

static void set_hash_function(char *str)
{
	if (!strcmp(str, "tea"))
		Hash = TEA_HASH;
	else if (!strcmp(str, "rupasov"))
		Hash = YURA_HASH;
	else if (!strcmp(str, "r5"))
		Hash = R5_HASH;
	else
		message("wrong hash type specified. Using default");
}

static void set_reiserfs_version(char *str)
{
	if (!strcmp(str, "3.5"))
		Format = "3.5";
	else {
		Format = "3.6";
		if (strcmp(str, "3.6"))
			message("wrong reiserfs version specified. "
				"Using default 3.6 format");
	}
}

static int str2int(char *str)
{
	int val;
	char *tmp;

	val = (int)strtol(str, &tmp, 0);

	if (*tmp) {
		reiserfs_exit(1,
			      "%s: strtol is unable to make an integer of %s\n",
			      program_name, str);
	}

	return val;
}

static __u64 str2u64(char *str)
{
	__u64 val;
	char *tmp;

	val = (__u64) strtoll(str, &tmp, 0);

	if (*tmp) {
		reiserfs_exit(1,
			      "%s: strtoll is unable to make an integer of %s\n",
			      program_name, str);
	}

	return val;
}

static void set_block_size(char *str, int *b_size)
{
	*b_size = str2int(str);

	if (!is_blocksize_correct(*b_size))
		reiserfs_exit(1, "%s: wrong blocksize %s specified, "
			      "only power of 2 from 512-8192 interval "
			      "are supported", program_name, str);
}

static void set_transaction_max_size(char *str)
{
	Max_trans_size = str2int(str);
}

/* reiserfs_create_journal will check this */
static void set_journal_device_size(char *str)
{
	Journal_size = str2int(str);
/*
    if (Journal_size < JOURNAL_MIN_SIZE)
		die ("%s: wrong journal size specified: %lu. Should be at least %u",
			 program_name,
			 Journal_size + 1, JOURNAL_MIN_SIZE + 1);
*/
}

/* reiserfs_create_journal will check this */
static void set_offset_in_journal_device(char *str)
{
	Offset = str2int(str);
}

static int is_journal_default(char *name, char *jname, int blocksize)
{
	if (jname && strcmp(name, jname))
		return 0;

	if (Journal_size && Journal_size !=
	    journal_default_size(REISERFS_DISK_OFFSET_IN_BYTES / blocksize,
				 blocksize) + 1)
		/* journal size is set and it is not default size */
		return 0;

	if (Max_trans_size && Max_trans_size != JOURNAL_TRANS_MAX)
		return 0;

	return 1;
}

/* if running kernel is 2.2 - mkreiserfs creates 3.5 format, if 2.4 - 3.6,
   otherwise - mkreiserfs fails */
static int select_format(void)
{
	struct utsname sysinfo;

	if (Format) {
		if (!strcmp(Format, "3.5"))
			return REISERFS_FORMAT_3_5;

		if (strcmp(Format, "3.6")) {
			message("Unknown fromat %s specified\n", Format);
			exit(1);
		}
		return REISERFS_FORMAT_3_6;
	}

	reiserfs_warning(stdout, "Guessing about desired format.. ");

	if (uname(&sysinfo) == -1) {
		message("could not get system info: %s", strerror(errno));
		exit(1);
	}

	reiserfs_warning(stdout, "Kernel %s is running.\n", sysinfo.release);

	if (strncmp(sysinfo.release, "2.4", 3) >= 0)
		return REISERFS_FORMAT_3_6;

	if (strncmp(sysinfo.release, "2.2", 3)) {
		message("You should run either 2.2 or 2.4 or higher to be able "
			"to create reiserfs filesystem or specify desired format with --format");
		exit(1);
	}

	reiserfs_warning(stdout, "Creating filesystem of format 3.5\n");
	return REISERFS_FORMAT_3_5;
}

static int block_size_ok(int blocksize, int force)
{
	int pagesize = getpagesize();
	if (blocksize > 4096) {
		reiserfs_warning(stderr, "Block sizes larger than 4k are not "
				 "supported on all architectures.\n");
		if (blocksize > pagesize)
			reiserfs_warning(stderr,
					 "The newly created filesystem will not "
					 "be mountable on this system.\n");
		else
			reiserfs_warning(stderr,
					 "The newly created filesystem may not "
					 "be mountable on other systems.\n");
		check_forcing_ask_confirmation(force);
	} else if (blocksize < 4096) {
		reiserfs_warning(stderr, "Block sizes smaller than 4k "
				 "are not supported.\n");
		return 0;
	}

	return 1;
}

int main(int argc, char **argv)
{
	reiserfs_filsys_t fs;
	int force = 0;
	char *device_name = NULL;
	char *jdevice_name = NULL;
	__u64 fs_size = 0;
	int c;
	static int flag;
	long error;

	program_name = strrchr(argv[0], '/');

	if (program_name)
		program_name++;
	else
		program_name = argv[0];

	if (argc < 2)
		print_usage_and_exit();

	memset(UUID, 0, 16);

	while (1) {
		static struct option options[] = {
			{"block-size", required_argument, NULL, 'b'},
			{"journal-device", required_argument, NULL, 'j'},
			{"journal-size", required_argument, NULL, 's'},
			{"transaction-max-size", required_argument, NULL, 't'},
			{"journal-offset", required_argument, NULL, 'o'},
			{"badblocks", required_argument, NULL, 'B'},
			{"hash", required_argument, NULL, 'h'},
			{"uuid", required_argument, NULL, 'u'},
			{"label", required_argument, NULL, 'l'},
			{"format", required_argument, &flag, 1},
			{}
		};
		int option_index;

		c = getopt_long(argc, argv, "b:j:s:t:o:h:u:l:VfdB:q",
				options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			if (flag) {
				Format = optarg;
				flag = 0;
			}
			break;
		case 'b':	/* --block-size */
			set_block_size(optarg, &Block_size);
			break;

		case 'j':	/* --journal-device */
			Create_default_journal = 0;
			jdevice_name = optarg;
			break;

		case 's':	/* --journal-size */
			Create_default_journal = 0;
			set_journal_device_size(optarg);
			break;

		case 't':	/* --transaction-max-size */
			Create_default_journal = 0;
			set_transaction_max_size(optarg);
			break;

		case 'o':	/* --offset */
			Create_default_journal = 0;
			set_offset_in_journal_device(optarg);
			break;

		case 'B':	/* --badblock-list */
			asprintf(&badblocks_file, "%s", optarg);
			break;

		case 'h':	/* --hash */
			set_hash_function(optarg);
			break;

		case 'v':	/* --format */
			set_reiserfs_version(optarg);
			break;

		case 'V':
			mode = DO_NOTHING;
			break;

		case 'f':
			force++;
			break;

		case 'd':
			mode |= DEBUG_MODE;
			break;

		case 'u':
#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
			if (uuid_parse(optarg, UUID) < 0) {
				reiserfs_warning(stderr, "Invalid UUID '%s' is "
						 "specified\n", optarg);
				return 1;
			}
#else
			message("Cannot set up the UUID, uuidlib was not "
				"found by configure.\n");
			return 1;
#endif
			break;

		case 'l':
			LABEL = optarg;
			break;
		case 'q':
			mode |= QUIET_MODE;
			break;
		default:
			print_usage_and_exit();
		}
	}

	print_banner(program_name);

	if (mode & QUIET_MODE)
		fclose(stdout);

	if (mode == DO_NOTHING)
		exit(0);

	/* device to be formatted */
	device_name = argv[optind];

	if (optind == argc - 2) {
		/* number of blocks for filesystem is specified */
		fs_size = str2u64(argv[optind + 1]);
	} else if (optind == argc - 1) {
		/* number of blocks is not specified */
		if (!(fs_size = count_blocks(device_name, Block_size)))
			exit(1);
	} else {
		print_usage_and_exit();
	}

	if (fs_size >= UINT_MAX) {
		fprintf(stderr,
			">>> ReiserFS supports file systems of up to %u "
			"blocks.\n>>> The maximum size with a block size of %u bytes "
			"is about %Lu MiB.\n>>> This file system would occupy %Lu "
			"blocks. ", UINT_MAX, Block_size,
			((__u64) UINT_MAX * Block_size) / (1024 * 1024),
			fs_size);

		if (optind == argc - 1) {
			if (!force &&
			    !user_confirmed(stderr, "Truncate? (y/N): ",
					    "y\n")) {
				fprintf(stderr, "\nExiting.\n\n");
				exit(1);
			}
			fprintf(stderr, "Truncating.\n\n");
			fs_size = UINT_MAX;
		} else {
			fprintf(stderr, "Exiting.\n\n");
			exit(1);
		}
	}

	if (is_journal_default(device_name, jdevice_name, Block_size))
		Create_default_journal = 1;

	if (!(mode & QUIET_MODE) && !can_we_format_it(device_name, force))
		return 1;

	if (!(mode & QUIET_MODE) && !block_size_ok(Block_size, force))
		return 1;

	if (jdevice_name)
		if (!(mode & QUIET_MODE)
		    && !can_we_format_it(jdevice_name, force))
			return 1;

	fs = reiserfs_create(device_name, select_format(), fs_size, Block_size,
			     Create_default_journal, 1, &error);

	if (!fs) {
		reiserfs_warning(stderr, "reiserfs_create failed: %s %ld\n",
				 error_message(error), error);
		return 1;
	}

	if (!reiserfs_create_journal(fs, jdevice_name, Offset, Journal_size,
				     Max_trans_size, force)) {
		return 1;
	}

	if (!reiserfs_create_ondisk_bitmap(fs)) {
		return 1;
	}

	/* these fill buffers (super block, first bitmap, root block) with
	   reiserfs structures */
#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
	if (!uuid_is_null(UUID) && fs->fs_format != REISERFS_FORMAT_3_6) {
		reiserfs_warning(stderr,
				 "UUID can be specified only with 3.6 format\n");
		return 1;
	}
#endif

	if (badblocks_file) {
		if (create_badblock_bitmap(fs, badblocks_file))
			exit(1);
	}

	make_super_block(fs);
	make_bitmap(fs);
	make_root_block(fs);
	add_badblock_list(fs, 1);

	report(fs, jdevice_name);

	if (!force && !(mode & QUIET_MODE)) {
		fprintf(stderr, "ATTENTION: YOU SHOULD REBOOT AFTER FDISK!\n"
			"\tALL DATA WILL BE LOST ON '%s'", device_name);
		if (jdevice_name && strcmp(jdevice_name, device_name))
			fprintf(stderr, " AND ON JOURNAL DEVICE '%s'",
				jdevice_name);

		if (!user_confirmed(stderr, "!\nContinue (y/n):", "y\n"))
			return 1;
	}

	invalidate_other_formats(fs->fs_dev);
	zero_journal(fs);

	reiserfs_close(fs);

	printf("Syncing..");
	fflush(stdout);
	sync();
	printf("ok\n");

	if (mode & DEBUG_MODE)
		return 0;

	printf("ReiserFS is successfully created on %s.\n", device_name);

	return 0;
}

/*
 * Use BSD fomatting.
 * Local variables:
 * c-indentation-style: "bsd"
 * mode-name: "BSDC"
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 */
