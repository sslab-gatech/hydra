/*
 * Copyright 2002-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#define _GNU_SOURCE

#include "tune.h"

#include <getopt.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "parse_time.h"

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
	message("Usage: %s [options] device [block-count]\n"
		"\n"
		"Options:\n\n"
		"  -j | --journal-device file\tcurrent journal device\n"
		"  --journal-new-device file\tnew journal device\n"
		"  -o | --journal-new-offset N\tnew journal offset in blocks\n"
		"  -s | --journal-new-size N\tnew journal size in blocks\n"
		"  -t | --trans-max-size N\tnew journal max transaction size in blocks\n"
		"  --no-journal-available\tcurrent journal is not available\n"
		"  --make-journal-standard\tnew journal to be standard\n"
		/*"\t-p | --keep-old-journal-param  (keep parametrs from old journal to new one)\n" */
		"  -b | --add-badblocks file\tadd to bad block list\n"
		"  -B | --badblocks file\t\tset the bad block list\n"
		"  -u | --uuid UUID|random\tset new UUID\n"
		"  -l | --label LABEL\t\tset new label\n"
		"  -f | --force\t\t\tforce tuning, less confirmations\n"
		"  -c | --check-interval\t\tset interval in days for fsck -a to check,\n"
		"                       \t\t\"disable\" to disable check,\n"
		"                       \t\tor \"default\" to restore default\n"
		"  -C | --time-last-checked\tset the time the filesystem was last checked\n"
		"                          \t(now or YYYYMMDD[HH[MM[SS]]])\n"
		"  -m | --max-mnt-count\t\tset maximum number of mounts before fsck -a\n"
		"                      \t\tchecks, \"disable\" to disable check,\n"
		"                      \t\tor \"default\" to restore default\n"
		"  -M | --mnt-count\t\tset the number of times the filesystem\n"
		"                  \t\thas been mounted\n"
		"  -h | --help\t\t\tprint help and exit\n"
		"  -V\t\t\t\tprint version and exit\n", program_name);
	exit(1);
}

static unsigned long Journal_size = 0;
static int Max_trans_size = JOURNAL_TRANS_MAX;
static unsigned short Max_mnt_count = 0;
static unsigned short Mnt_count = 0;
static unsigned int Check_interval = 0;
static time_t Time_last_checked = 0;
static int Offset = 0;
static __u16 Options = 0;
static int Force = 0;
static int Bads = 0;
static char *LABEL;
static unsigned char UUID[16];
static char *badblocks_file;

/* If specified paramenters defines the standard journal, make it standard. */
static int should_make_journal_standard(reiserfs_filsys_t fs,
					char *j_new_dev_name)
{
	if (!is_reiserfs_jr_magic_string(fs->fs_ondisk_sb))
		return 0;
/*
    if (!user_confirmed (stderr, "ATTENTION! Filesystem with non-standard journal "
			 "found. Continue? (y/n):", "y\n")) {
	exit(1);
    }
*/
	/* make sure journal is on main device, it has default size
	   and the file system has non-standard magic */

	if (j_new_dev_name) {
		/* new journal was specified - check if it is available */
		if (strcmp(j_new_dev_name, fs->fs_file_name))
			return 0;

		if (Journal_size && Journal_size !=
		    journal_default_size(fs->fs_super_bh->b_blocknr,
					 fs->fs_blocksize) + 1)
			return 0;

		if (Max_trans_size && (Max_trans_size != JOURNAL_TRANS_MAX))
			return 0;
	} else {
		/* new journal was not specified - check ondisk journal params */

		if (get_sb_reserved_for_journal(fs->fs_ondisk_sb) <
		    journal_default_size(fs->fs_super_bh->b_blocknr,
					 fs->fs_blocksize) + 1) {
			message
			    ("Can not create standard journal of the size %llu",
			     journal_default_size(fs->fs_super_bh->b_blocknr,
						  fs->fs_blocksize) + 1);
			return 0;
		}
	}

	return 1;
}

static int set_standard_journal_params(reiserfs_filsys_t fs)
{
	struct buffer_head *bh;

	/* ondisk superblock update */

	if (get_sb_version(fs->fs_ondisk_sb) == 0)
		memcpy(fs->fs_ondisk_sb->s_v1.s_magic,
		       REISERFS_3_5_SUPER_MAGIC_STRING,
		       strlen(REISERFS_3_5_SUPER_MAGIC_STRING));
	else if (get_sb_version(fs->fs_ondisk_sb) == 2)
		memcpy(fs->fs_ondisk_sb->s_v1.s_magic,
		       REISERFS_3_6_SUPER_MAGIC_STRING,
		       strlen(REISERFS_3_6_SUPER_MAGIC_STRING));
	else {
		message
		    ("Can not set standard reiserfs magic: unknown format found %u,"
		     " try reiserfsck first", get_sb_version(fs->fs_ondisk_sb));
		return 0;
	}

	set_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb),
				 get_journal_start_must(fs));
	set_jp_journal_dev(sb_jp(fs->fs_ondisk_sb), 0);
	set_jp_journal_size(sb_jp(fs->fs_ondisk_sb),
			    journal_default_size(fs->fs_super_bh->b_blocknr,
						 fs->fs_blocksize));

	if (get_jp_journal_max_trans_len(sb_jp(fs->fs_ondisk_sb)) !=
	    JOURNAL_TRANS_MAX)
		set_jp_journal_max_trans_len(sb_jp(fs->fs_ondisk_sb),
					     JOURNAL_TRANS_MAX);
	if (get_jp_journal_max_batch(sb_jp(fs->fs_ondisk_sb)) !=
	    JOURNAL_MAX_BATCH)
		set_jp_journal_max_batch(sb_jp(fs->fs_ondisk_sb),
					 JOURNAL_MAX_BATCH);
	if (get_jp_journal_max_commit_age(sb_jp(fs->fs_ondisk_sb)) !=
	    JOURNAL_MAX_COMMIT_AGE)
		set_jp_journal_max_commit_age(sb_jp(fs->fs_ondisk_sb),
					      JOURNAL_MAX_COMMIT_AGE);
	if (get_jp_journal_max_trans_age(sb_jp(fs->fs_ondisk_sb)) !=
	    JOURNAL_MAX_TRANS_AGE)
		set_jp_journal_max_trans_age(sb_jp(fs->fs_ondisk_sb),
					     JOURNAL_MAX_TRANS_AGE);
	set_sb_reserved_for_journal(fs->fs_ondisk_sb, 0);

	/* journal_header update */
	bh = getblk(fs->fs_journal_dev,
		    get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb)) +
		    get_jp_journal_size(sb_jp(fs->fs_ondisk_sb)),
		    fs->fs_blocksize);

	if (!bh) {
		message
		    ("Cannot get the journal header block. getblk failed.\n");
		return 0;
	}
	((struct reiserfs_journal_header *)(bh->b_data))->jh_journal =
	    *(sb_jp(fs->fs_ondisk_sb));
	mark_buffer_uptodate(bh, 1);
	mark_buffer_dirty(bh);
	bwrite(bh);
	brelse(bh);

	return 1;
}

static void zero_journal(reiserfs_filsys_t fs)
{
	unsigned int i;
	struct buffer_head *bh;
	unsigned long done;
	unsigned long start, len;

	fprintf(stderr, "Initializing journal - ");

	start = get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb));
	len = get_jp_journal_size(sb_jp(fs->fs_ondisk_sb));
	done = 0;
	for (i = 0; i < len; i++) {
		print_how_far(stderr, &done, len, 1, 1 /*be quiet */ );
		bh = getblk(fs->fs_journal_dev, start + i, fs->fs_blocksize);
		if (!bh)
			die("zero_journal: getblk failed");
		memset(bh->b_data, 0, bh->b_size);
		mark_buffer_dirty(bh);
		mark_buffer_uptodate(bh, 1);
		bwrite(bh);
		brelse(bh);
	}

	fprintf(stderr, "\n");
	fflush(stderr);
}

static int str2int(char *str)
{
	int val;
	char *tmp;

	val = (int)strtol(str, &tmp, 0);
	if (*tmp)
		die("%s: strtol is unable to make an integer of %s\n",
		    program_name, str);
	return val;
}

static void set_transaction_max_size(char *str)
{
	Max_trans_size = str2int(str);
}

/* journal must fit into number of blocks pointed by first bitmap */
static void set_journal_device_size(char *str)
{
	Journal_size = str2int(str);
}

static void set_offset_in_journal_device(char *str)
{
	Offset = str2int(str);
}

static void set_max_mnt_count(char *str)
{
	if (!strcmp(str, "disable"))
		Max_mnt_count = USHRT_MAX;
	else if (!strcmp(str, "default"))
		Max_mnt_count = DEFAULT_MAX_MNT_COUNT;
	else
		Max_mnt_count = str2int(str);
}

static void set_mnt_count(char *str)
{
	Mnt_count = str2int(str);
}

static void set_check_interval(char *str)
{
	if (!strcmp(str, "disable"))
		Check_interval = UINT_MAX;
	else if (!strcmp(str, "default"))
		Check_interval = DEFAULT_CHECK_INTERVAL;
	else
		Check_interval = str2int(str) * 60 * 60 * 24;
}

static void set_time_last_checked(char *str)
{
	if (!strcmp(str, "now"))
		Time_last_checked = time(NULL);
	else
		Time_last_checked = parse_time(str);

	if (Time_last_checked == 0)
		print_usage_and_exit();
}

static void callback_new_badblocks(reiserfs_filsys_t fs,
				   struct reiserfs_path *badblock_path,
				   void *data)
{
	struct item_head *tmp_ih;
	__le32 *ind_item;
	__u32 i;

	tmp_ih = tp_item_head(badblock_path);
	ind_item = (__le32 *) tp_item_body(badblock_path);

	for (i = 0; i < I_UNFM_NUM(tmp_ih); i++) {
		if (reiserfs_bitmap_test_bit(fs->fs_badblocks_bm,
					     d32_get(ind_item, i))) {
			message("Block %u is marked as bad already.",
				d32_get(ind_item, i));

			reiserfs_bitmap_clear_bit(fs->fs_badblocks_bm,
						  d32_get(ind_item, i));
		}
	}

	pathrelse(badblock_path);
}

static void callback_clear_badblocks(reiserfs_filsys_t fs,
				     struct reiserfs_path *badblock_path,
				     void *data)
{
	struct item_head *tmp_ih;
	__le32 *ind_item;
	__u32 i;

	tmp_ih = tp_item_head(badblock_path);
	ind_item = (__le32 *) tp_item_body(badblock_path);

	for (i = 0; i < I_UNFM_NUM(tmp_ih); i++) {
		reiserfs_bitmap_clear_bit(fs->fs_bitmap2, d32_get(ind_item, i));
	}

	pathrelse(badblock_path);
}

static void add_badblocks(reiserfs_filsys_t fs)
{
	unsigned long i, marked = 0;

	if (reiserfs_open_ondisk_bitmap(fs) < 0) {
		message("Failed to open reiserfs ondisk bitmap.\n");
		reiserfs_close(fs);
		exit(1);
	}

	if (create_badblock_bitmap(fs, badblocks_file)) {
		message("Failed to initialize the bad block bitmap.\n");
		reiserfs_close(fs);
		exit(1);
	}

	if (Bads == 1)
		badblock_list(fs, callback_new_badblocks, NULL);
	else
		badblock_list(fs, callback_clear_badblocks, NULL);

	for (i = 0; i < get_sb_block_count(fs->fs_ondisk_sb); i++) {
		if (reiserfs_bitmap_test_bit(fs->fs_badblocks_bm, i)) {
			if (!reiserfs_bitmap_test_bit(fs->fs_bitmap2, i)) {
				reiserfs_bitmap_set_bit(fs->fs_bitmap2, i);
				marked++;
			} else {
				/* Check that this is a block  */
				message
				    ("Bad block %lu is used already in reiserfs tree. "
				     "To mark it as a bad block use reiserfsck\n"
				     "--fix-fixable with -B option.", i);

				reiserfs_bitmap_clear_bit(fs->fs_badblocks_bm,
							  i);
			}
		}
	}

	if (marked) {
		set_sb_free_blocks(fs->fs_ondisk_sb,
				   get_sb_free_blocks(fs->fs_ondisk_sb) -
				   fs->fs_badblocks_bm->bm_set_bits);
		mark_buffer_dirty(fs->fs_super_bh);
	}

	if (Bads == 1) {
		/* fs->fs_badblocks_bm contains blocks which are not in the bad
		   block list yet. Merge it with what is in the tree already. */
		badblock_list(fs, mark_badblock, NULL);
	}

	if (marked) {
		add_badblock_list(fs, 1);
	}

	message("%lu blocks were marked as bad.", marked);
}

int main(int argc, char **argv)
{
	reiserfs_filsys_t fs;
	char *device_name;
	char *jdevice_name;
	char *j_new_device_name;
	int c;
	static int flag;
	struct reiserfs_journal_header *j_head;
	reiserfs_trans_t old, new;
	int Is_journal_or_maxtrans_size_specified = 0;
	long error;

	program_name = strrchr(argv[0], '/');

	if (program_name)
		program_name++;
	else
		program_name = argv[0];

	if (argc < 2)
		print_usage_and_exit();

	device_name = NULL;
	jdevice_name = NULL;
	j_new_device_name = NULL;

	memset(UUID, 0, 16);

	while (1) {
		static struct option options[] = {
			{"help", no_argument, NULL, 'h'},
			{"journal-device", required_argument, NULL, 'j'},
			{"journal-new-device", required_argument, &flag,
			 OPT_NEW_J},
			{"journal-new-size", required_argument, NULL, 's'},
			{"trans-max-size", required_argument, NULL, 't'},
			{"journal-new-offset", required_argument, NULL, 'o'},
			{"no-journal-available", no_argument, &flag,
			 OPT_SKIP_J},
			/*{"keep-old-journal-param", no_argument, NULL, 'p'}, */
			{"uuid", required_argument, NULL, 'u'},
			{"label", required_argument, NULL, 'l'},
			{"add-badblocks", required_argument, NULL, 'b'},
			{"badblocks", required_argument, NULL, 'B'},
			{"force", no_argument, NULL, 'f'},
			{"make-journal-standard", no_argument, &flag,
			 OPT_STANDARD},
			{"check-interval", required_argument, NULL, 'c'},
			{"time-last-checked", required_argument, NULL, 'C'},
			{"max-mount-count", required_argument, NULL, 'm'},
			{"mount-count", required_argument, NULL, 'M'},
			{}
		};
		int option_index;

		c = getopt_long(argc, argv, "hj:s:t:o:fu:l:b:B:Vc:C:m:M:",
				options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			/* long-only optins */
			if (flag == OPT_NEW_J) {
				Options |= OPT_NEW_J;
				j_new_device_name = optarg;
			}
			if (flag == OPT_SKIP_J) {
				Options |= OPT_SKIP_J;
			}
			if (flag == OPT_STANDARD) {
				Options |= OPT_STANDARD;
			}
			break;
		case 'j':	/* --journal-device */
			jdevice_name = optarg;
			break;

		case 's':	/* --journal-new-size */
			set_journal_device_size(optarg);
			Is_journal_or_maxtrans_size_specified = 1;
			break;

		case 't':	/* --trans-max-size */
			set_transaction_max_size(optarg);
			Is_journal_or_maxtrans_size_specified = 1;
			break;

		case 'o':	/* --offset */
			set_offset_in_journal_device(optarg);
			break;

		case 'f':
			/* forces replacing standard journal with non-standard
			   one. Specified more than once - allows to avoid asking for
			   confirmation */
			Force++;
			break;
		case 'b':	/* --add-badblocks */
			asprintf(&badblocks_file, "%s", optarg);
			Bads = 1;
			break;
		case 'B':	/* --badblocks */
			asprintf(&badblocks_file, "%s", optarg);
			Bads = 2;
			break;
		case 'u':
			/* UUID */
#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
			if (!strcmp(optarg, "random")) {
				uuid_generate(UUID);
			} else {
				if (uuid_parse(optarg, UUID) < 0) {
					message
					    ("Invalid UUID '%s' was specified\n",
					     optarg);
					return 1;
				}
			}
#else
			message("Cannot set the UUID, uuidlib was not found "
				"by configure.\n");
			return 1;
#endif
			break;
		case 'l':
			/* LABEL */
			LABEL = optarg;
			break;
		case 'V':
			print_banner("reiserfstune");
			exit(0);
		case 'h':
			print_usage_and_exit();
			break;
		case 'c':
			set_check_interval(optarg);
			break;
		case 'C':
			set_time_last_checked(optarg);
			break;
		case 'm':
			set_max_mnt_count(optarg);
			break;
		case 'M':
			set_mnt_count(optarg);
			break;

#if 0
		case 'J':	/* --journal-new-device */
			Options |= OPT_NEW_J;
			j_new_device_name = optarg;
			break;

		case 'u':	/* --no-journal-available */
			Options |= OPT_SKIPJ;
			break;

		case 'p':	/* --keep-old-journal-param */
			Options |= OPT_KEEPO;
			break;
#endif
		default:
			print_usage_and_exit();
		}
	}

	if (optind != argc - 1)
		print_usage_and_exit();

	/* device to be formatted */
	device_name = argv[optind];

	fs = reiserfs_open(device_name, O_RDONLY, &error, NULL, 1);
	if (no_reiserfs_found(fs)) {
		message("Cannot open reiserfs on %s: %s", device_name,
			error_message(error));
		return 1;
	}

	/* journal was opened or it wasn't opened but the option
	   --no-journal-available has been specified by user */

	/* make sure filesystem is not mounted */
	if (misc_device_mounted(fs->fs_file_name) > 0) {
		/* fixme: it can not be mounted, btw */
		message
		    ("Reiserfstune is not allowed to be run on mounted filesystem.");
		reiserfs_close(fs);
		return 1;
	}

	if (!reiserfs_is_fs_consistent(fs)) {
		message
		    ("Filesystem looks not cleanly umounted, check the consistency first.\n");
		reiserfs_close(fs);
		return 1;
	}

	reiserfs_reopen(fs, O_RDWR);

	if (badblocks_file) {
		add_badblocks(fs);

		reiserfs_close(fs);
		exit(0);
	}

	if (!jdevice_name && !(Options & OPT_SKIP_J)) {
		message
		    ("Journal device has not been specified. Assuming journal is on the main "
		     "device (%s).\n", device_name);
		jdevice_name = device_name;
	}

	if (jdevice_name && (Options & OPT_SKIP_J)) {
		message("Either specify journal device, "
			"or choose the option --no-journal-available");
		return 1;
	}

	if (j_new_device_name && (Options & OPT_STANDARD)) {
		/* New device was specified and --make-journal-standard was also. */
		message("Either specify new journal device, "
			"or choose the option --make-journal-standard");
		return 1;
	}

	/* now we try to open journal, it makes sence if there is no the flag
	   NEED_TUNE  in ondisk superblock and --no-journal available is not
	   specified. */
	if (get_jp_journal_magic(sb_jp(fs->fs_ondisk_sb)) != NEED_TUNE &&
	    !(Options & OPT_SKIP_J)) {
		if (reiserfs_open_journal(fs, jdevice_name, O_RDWR
#if defined(O_LARGEFILE)
					  | O_LARGEFILE
#endif
		    )) {
			message("Failed to open the journal device (%s).",
				jdevice_name);
			return 1;
		}

		if (reiserfs_journal_params_check(fs)) {
			message
			    ("Unable to open old journal. Wrong journal parameters.");
			reiserfs_close(fs);
			return 1;
		}
	}

	/* in spite of journal was opened, the file system can be non-consistent or
	   there are non-replayed transaction in journal,
	   make sure it isn't (if there is no the flag NEED_TUNE in ondisk superblock */
	if (get_jp_journal_magic(sb_jp(fs->fs_ondisk_sb)) != NEED_TUNE &&
	    reiserfs_journal_opened(fs)) {
		j_head =
		    (struct reiserfs_journal_header *)(fs->fs_jh_bh->b_data);
		if (get_boundary_transactions(fs, &old, &new)) {
			if (new.trans_id != get_jh_last_flushed(j_head)) {
				message
				    ("There are non-replayed transaction in old journal,"
				     " check filesystem consistency first");
				reiserfs_close(fs);
				return 1;
			}
		}
		if (!reiserfs_is_fs_consistent(fs)) {
			message("Check filesystem consistency first");
			reiserfs_close(fs);
			return 1;
		}
	}

	/* set UUID and LABEL if specified */
	if (fs->fs_format == REISERFS_FORMAT_3_6) {
		int need_dirty = 0;
#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
		if (!uuid_is_null(UUID)) {
			memcpy(fs->fs_ondisk_sb->s_uuid, UUID, 16);
			need_dirty = 1;
		}
#endif
		if (LABEL != NULL) {
			if (strlen(LABEL) > 16)
				message
				    ("Specified LABEL is longer then 16 characters, will be truncated\n");
			strncpy(fs->fs_ondisk_sb->s_label, LABEL, 16);
			need_dirty = 1;
		}
		if (Max_mnt_count &&
		    Max_mnt_count !=
		    get_sb_v2_max_mnt_count(fs->fs_ondisk_sb)) {
			if (Max_mnt_count <= 0)
				reiserfs_exit(1, "max-mnt-count must be > 0\n");
			set_sb_v2_max_mnt_count(fs->fs_ondisk_sb,
						Max_mnt_count);
			need_dirty = 1;
		}

		if (Mnt_count &&
		    Mnt_count != get_sb_v2_mnt_count(fs->fs_ondisk_sb)) {
			if (Max_mnt_count <= 0)
				reiserfs_exit(1, "max-mnt-count must be > 0\n");
			set_sb_v2_mnt_count(fs->fs_ondisk_sb, Mnt_count);
			need_dirty = 1;
		}

		if (Check_interval &&
		    Check_interval !=
		    get_sb_v2_check_interval(fs->fs_ondisk_sb)) {
			if (Check_interval <= 0)
				reiserfs_exit(1,
					      "check-interval must be > 0\n");
			set_sb_v2_check_interval(fs->fs_ondisk_sb,
						 Check_interval);
			need_dirty = 1;
		}
		if (Time_last_checked &&
		    Time_last_checked !=
		    get_sb_v2_lastcheck(fs->fs_ondisk_sb)) {
			set_sb_v2_lastcheck(fs->fs_ondisk_sb,
					    Time_last_checked);
			need_dirty = 1;
		}

		if (need_dirty) {
			mark_buffer_dirty(fs->fs_super_bh);
			fs->fs_dirt = 1;
		}
	} else {
#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
		if (!uuid_is_null(UUID))
			reiserfs_exit(1,
				      "UUID cannot be specified for 3.5 format\n");
#endif
		if (LABEL)
			reiserfs_exit(1,
				      "LABEL cannot be specified for 3.5 format\n");

		if (Max_mnt_count)
			reiserfs_exit(1,
				      "max-mnt-count cannot be specified for 3.5 format\n");
		if (Check_interval)
			reiserfs_exit(1,
				      "check-interval cannot be specified for 3.5 format\n");
	}

	if (!j_new_device_name) {

		/* new journal device hasn't been specified */
		printf("Current parameters:\n");
		print_filesystem_state(stdout, fs);
		print_block(stdout, fs, fs->fs_super_bh);

		if ((Options & OPT_STANDARD)
		    && should_make_journal_standard(fs, j_new_device_name)) {
			if (!user_confirmed
			    (stderr,
			     "ATTENTION! Filesystem with "
			     "non-standard journal found. Continue? (y/n):",
			     "y\n")) {
				exit(1);
			}

			fs->fs_journal_dev = fs->fs_dev;
			if (set_standard_journal_params(fs)) {
				printf("\nNew parameters:\n");
				print_filesystem_state(stdout, fs);
				print_block(stdout, fs, fs->fs_super_bh);
				printf("New journal parameters:\n");
				print_journal_params(stdout,
						     sb_jp(fs->fs_ondisk_sb));
				mark_buffer_dirty(fs->fs_super_bh);
				mark_buffer_uptodate(fs->fs_super_bh, 1);
				reiserfs_close(fs);
				printf("Syncing..");
				fflush(stdout);
				sync();
				printf("ok\n");
				return 0;
			}
		}

		if (Is_journal_or_maxtrans_size_specified) {
			/* new journal device hasn't been specified, but
			   journal size or max transaction size have been, so we suppose
			   that journal device remains the same */
			if (!reiserfs_journal_opened(fs)) {
				message
				    ("Cannot set up new paramenters for not specified journal.");
				return 1;
			}

			j_new_device_name = jdevice_name;
		} else {
			/* the only parameter has been specified is device_name, so
			   there is nothing to do */
			reiserfs_close(fs);
			return 0;
		}
	}

	/* new journal device has been specified */
	/* make sure new journal device is block device file */
	if (!can_we_format_it(j_new_device_name, Force)) {
		reiserfs_close(fs);
		return 1;
	}

	if (!strcmp(device_name, j_new_device_name)) {
		unsigned long reserved, journal_size;
		/* we have to put journal on main device. It is only possible if there
		   is enough space reserved by mkreiserfs */

		if (!is_reiserfs_jr_magic_string(fs->fs_ondisk_sb))
			/* standard journal */
			reserved =
			    get_jp_journal_size(sb_jp(fs->fs_ondisk_sb)) + 1;
		else
			/* non-standard journal */
			reserved =
			    get_sb_reserved_for_journal(fs->fs_ondisk_sb);

		journal_size = Journal_size;

		if (!journal_size) {
			journal_size =
			    journal_default_size(fs->fs_super_bh->b_blocknr,
						 fs->fs_blocksize) + 1;
			message
			    ("Journal size has not been specified. Assuming it is the default size (%lu)",
			     journal_size);
		}

/*	journal_size = (Journal_size ? Journal_size : // specified
			(fs->fs_blocksize == 1024 ? (fs->fs_blocksize) * 8 - 3 -
			 REISERFS_DISK_OFFSET_IN_BYTES / fs->fs_blocksize :
			 JOURNAL_DEFAULT_SIZE + 1));	// default
*/
		if (journal_size + Offset >
		    get_journal_start_must(fs) + reserved) {
			message
			    ("There is no enough space reserved for journal on main "
			     "device (journal_size=%lu, reserved=%lu)\n",
			     journal_size, reserved);
			reiserfs_close(fs);
			return 1;
		}
	}

	message("Current journal parameters:");
	print_journal_params(stdout, sb_jp(fs->fs_ondisk_sb));

	if (!is_reiserfs_jr_magic_string(fs->fs_ondisk_sb)) {
		/* we have standard journal, so check if we can convert it
		   to non-standard one */

		/*
		   if (!should_make_journal_non_standard (Force)) {
		   reiserfs_close (fs);
		   return 1;
		   }
		 */

		if (is_reiserfs_3_6_magic_string(fs->fs_ondisk_sb))
			set_sb_version(fs->fs_ondisk_sb, REISERFS_FORMAT_3_6);
		else if (is_reiserfs_3_5_magic_string(fs->fs_ondisk_sb))
			set_sb_version(fs->fs_ondisk_sb, REISERFS_FORMAT_3_5);
		else {
			message
			    ("Could not convert from unknown version, try reiserfsck first");
			reiserfs_close(fs);
			return 1;
		}

		memcpy(fs->fs_ondisk_sb->s_v1.s_magic,
		       REISERFS_JR_SUPER_MAGIC_STRING,
		       strlen(REISERFS_JR_SUPER_MAGIC_STRING));
		set_sb_reserved_for_journal(fs->fs_ondisk_sb,
					    get_jp_journal_size(sb_jp
								(fs->
								 fs_ondisk_sb))
					    + 1);
	}

	/* now we are going to close old journal and to create a new one */
	reiserfs_close_journal(fs);

	if (!reiserfs_create_journal(fs, j_new_device_name, Offset,
				     Journal_size, Max_trans_size, Force)) {
		message("Could not create new journal");
		reiserfs_close(fs);
		return 1;
	}

	if (should_make_journal_standard(fs, j_new_device_name))
		set_standard_journal_params(fs);

	message("New journal parameters:");
	print_journal_params(stdout, sb_jp(fs->fs_ondisk_sb));

	print_block(stdout, fs, fs->fs_super_bh);

	if (Force < 2) {
		message
		    ("ATTENTION: YOU ARE ABOUT TO SETUP THE NEW JOURNAL FOR THE \"%s\"!\n"
		     "AREA OF \"%s\" DEDICATED FOR JOURNAL WILL BE ZEROED!",
		     device_name, j_new_device_name);

		if (!user_confirmed(stderr, "Continue (y/n):", "y\n")) {
			return 1;
		}
	}

	zero_journal(fs);
	reiserfs_close(fs);

	printf("Syncing..");
	fflush(stdout);
	sync();
	printf("ok\n");

	return 0;
}
