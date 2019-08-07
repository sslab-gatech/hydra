/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"
#include <getopt.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <signal.h>
#include <limits.h>

extern int screen_width;
extern int screen_savebuffer_len;
extern char *screen_savebuffer;

reiserfs_filsys_t fs;
static char *badblocks_file;

#define print_usage_and_exit() {						\
fsck_progress ("Usage: %s [mode] [options] "					\
" device\n"									\
"\n"										\
"Modes:\n"									\
"  --check\t\t\tconsistency checking (default)\n"				\
"  --fix-fixable\t\t\tfix corruptions which can be fixed without \n"		\
"  \t\t\t\t--rebuild-tree\n"							\
"  --rebuild-sb\t\t\tsuper block checking and rebuilding if needed\n"		\
"  \t\t\t\t(may require --rebuild-tree afterwards)\n"				\
"  --rebuild-tree\t\tforce fsck to rebuild filesystem from scratch\n"		\
"  \t\t\t\t(takes a long time)\n"						\
"  --clean-attributes\t\tclean garbage in reserved fields in StatDatas \n"	\
"Options:\n"									\
"  -j | --journal device\t\tspecify journal if relocated\n"			\
"  -B | --badblocks file\t\tfile with list of all bad blocks on the fs\n"			\
"  -l | --logfile file\t\tmake fsck to complain to specifed file\n"		\
"  -n | --nolog\t\t\tmake fsck to not complain\n"				\
"  -z | --adjust-size\t\tfix file sizes to real size\n"				\
"  -q | --quiet\t\t\tno speed info\n"						\
"  -y | --yes\t\t\tno confirmations\n"						\
"  -f | --force\t\tforce checking even if the file system is marked clean\n"\
"  -V\t\t\t\tprints version and exits\n"					\
"  -a and -p\t\t\tsome light-weight auto checks for bootup\n"			\
"  -r\t\t\tignored\n"							\
"Expert options:\n"								\
"  --no-journal-available\tdo not open nor replay journal\n"			\
"  -S | --scan-whole-partition\tbuild tree of all blocks of the device\n\n",	\
  argv[0]);									\
										\
  exit(EXIT_OK);								\
}

/*
   -B works with --fix-fixable
        fixes indirect pointers pointed to
	badblocks, adds badblocks to badblock list in fs.
    and with --rebuild
        builds the tree without pointers to badblocks (internal,
	indirect), adds badblocks to badblock list in fs.
*/

/*
Hidden usage:
Modes:
"  --rollback-fsck-changes\n\t\t\trollback all changes made by fsck\n"\
Options:
"  -b | --scan-marked-in-bitmap file\n"\
"  \t\t\tbuild tree of blocks marked in the bitmapfile\n"\
"  -R | --rollback-data file\n"\
"  \t\t\tback up all changes to this file or rollback from this file\n"\
"  \t\t\tpreviously backed up changes with --rollback-fsck-changes\n"\
"  -d dumpfile\n"\
"  \t\t\tto test fsck pass by pass - dump into dumpfile all needed\n"\
"  \t\t\tinfo for the next pass and load on the start of the next pass\n"\
"  -i | --interactive\tmake fsck to stop after every stage\n"\
"  -h | --hash hashname\n"\
"  -g | --background\n"\
"  -t \t\tdo test\n"\
*/

/* fsck is called with one non-optional argument - file name of device
   containing reiserfs. This function parses other options, sets flags
   based on parsing and returns non-optional argument */
static char *parse_options(struct fsck_data *data, int argc, char *argv[])
{
	int c;
	static int mode = FSCK_CHECK;
	static int flag;

	data->rebuild.scan_area = USED_BLOCKS;
	while (1) {
		static struct option options[] = {
			/* modes */
			{"check", no_argument, &mode, FSCK_CHECK},
			{"fix-fixable", no_argument, &mode, FSCK_FIX_FIXABLE},
			{"rebuild-sb", no_argument, &mode, FSCK_SB},
			{"rebuild-tree", no_argument, &mode, FSCK_REBUILD},
			{"rollback-fsck-changes", no_argument, &mode,
			 FSCK_ROLLBACK_CHANGES},
			{"clean-attributes", no_argument, &mode,
			 FSCK_CLEAN_ATTRIBUTES},
			/* options */
			{"logfile", required_argument, NULL, 'l'},
			{"badblocks", required_argument, NULL, 'B'},
			{"interactive", no_argument, NULL, 'i'},
			{"adjust-size", no_argument, NULL, 'z'},
			{"quiet", no_argument, NULL, 'q'},
			{"yes", no_argument, NULL, 'y'},
			{"force", no_argument, NULL, 'f'},
			{"nolog", no_argument, NULL, 'n'},

			/* if file exists ad reiserfs can be load of it - only
			   blocks marked used in that bitmap will be read */
			{"scan-marked-in-bitmap", required_argument, NULL, 'b'},

			{"create-passes-dump", required_argument, NULL, 'd'},

			/* all blocks will be read */
			{"scan-whole-partition", no_argument, NULL, 'S'},
			/* useful for -S */
			{"hash", required_argument, NULL, 'h'},

			/* preparing rollback data */
			{"rollback-data", required_argument, NULL, 'R'},

			{"journal", required_argument, NULL, 'j'},
			{"no-journal-available", no_argument, &flag,
			 OPT_SKIP_JOURNAL},

			{"bad-block-file", required_argument, NULL, 'B'},

			/* start reiserfsck in background and exit */
			{"background", no_argument, NULL, 'g'},

			{}
		};
		int option_index;

		c = getopt_long(argc, argv, "iql:nb:Szd:R:h:j:gafVrpyt:B:",
				options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			/* long-only options */
			if (flag == OPT_SKIP_JOURNAL) {
				/* no journal available */
				data->options |= OPT_SKIP_JOURNAL;
				flag = 0;
			}
			break;

		case 'i':	/* --interactive */
			data->options |= OPT_INTERACTIVE;
			break;

		case 'q':	/* --quiet */
			data->options |= OPT_QUIET;
			break;

		case 'y':	/* --quiet */
			data->options |= OPT_YES;
			break;

		case 'l':	/* --logfile */
			data->log_file_name = optarg;
			/*asprintf (&data->log_file_name, "%s", optarg); */
			data->log = fopen(optarg, "w");
			if (!data->log)
				fprintf(stderr,
					"reiserfsck: Cannot not open \'%s\': %s",
					optarg, strerror(errno));
			break;

		case 'n':	/* --nolog */
			data->options |= OPT_SILENT;
			break;

		case 'b':	/* --scan-marked-in-bitmap */
			/* will try to load a bitmap from a file and read only
			   blocks marked in it. That bitmap could be created by
			   previous run of reiserfsck with -c */
			data->rebuild.bitmap_file_name = optarg;
			/*asprintf (&data->rebuild.bitmap_file_name, "%s", optarg); */
			data->rebuild.scan_area = EXTERN_BITMAP;
			break;

		case 'S':	/* --scan-whole-partition */
			data->rebuild.scan_area = ALL_BLOCKS;
			break;

#if 0
		case 'J':	/* take all blocks which are leaves in journal area and put
				   them into tree item by item (DO NOT USE IT UNTIL YOU KNOW
				   WHAT ARE YOU DOING) */
			data->rebuild.use_journal_area = 1;
			break;
#endif
		case 'd':	/* --create-passes-dump */
			asprintf(&data->rebuild.passes_dump_file_name, "%s",
				 optarg);
			data->options |= OPT_SAVE_PASSES_DUMP;
			break;

		case 'z':	/* --adjust-file-size */
			data->options |= OPT_ADJUST_FILE_SIZE;
			break;

		case 'h':	/* --hash: suppose that this hash was used on a filesystem */
			asprintf(&data->rebuild.defined_hash, "%s", optarg);
			if (name2func(data->rebuild.defined_hash) == NULL)
				reiserfs_panic
				    ("reiserfsck: Unknown hash is specified: %s",
				     data->rebuild.defined_hash);
			data->options |= OPT_HASH_DEFINED;
			break;

		case 'j':	/* specified relocated journal device */
			data->journal_dev_name = optarg;
			break;

		case 'R':	/* preparing rollback data */
			asprintf(&data->rebuild.rollback_file, "%s", optarg);
			data->options |= OPT_SAVE_ROLLBACK;
			break;

		case 'B':	/* list of phisically corrupted blocks */
			asprintf(&badblocks_file, "%s", optarg);
			data->options |= BADBLOCKS_FILE;
			break;

		case 'g':	/* --background */
			data->options |= OPT_BACKGROUND;
			break;

		case 'a':
		case 'p':
			data->options |= OPT_QUIET;
			mode = FSCK_AUTO;
			break;

		case 'f':
			data->options |= OPT_FORCE;
			break;
		case 'r':	/* ignored */
			break;

		case 'V':	/* cause fsck to do nothing */
			mode = DO_NOTHING;
			break;

		case 't':
			mode = DO_TEST;
			data->rebuild.test = atoi(optarg);
			break;

		default:
			print_usage_and_exit();
		}
	}

	if (optind != argc - 1 && mode != DO_NOTHING)
		/* only one non-option argument is permitted */
		print_usage_and_exit();

	if (mode != FSCK_REBUILD &&
	    (data->rebuild.scan_area == EXTERN_BITMAP ||
	     data->rebuild.scan_area == ALL_BLOCKS ||
	     data->options & OPT_SAVE_PASSES_DUMP))
		/* wrong options for this mode */
		print_usage_and_exit();

/*
    if (data->options & OPT_ADJUST_FILE_SIZE) {
        if ((mode != FSCK_REBUILD) && (mode != FSCK_FIX_FIXABLE))
            print_usage_and_exit();
    }
*/

	if (data->options & OPT_SAVE_ROLLBACK) {
		if (mode == FSCK_SB)
			print_usage_and_exit();
	}

	if (mode == FSCK_ROLLBACK_CHANGES) {
		if ((data->options & OPT_SAVE_ROLLBACK) == 0)
			print_usage_and_exit();
	}

	if ((data->options & BADBLOCKS_FILE) && mode != FSCK_REBUILD &&
	    mode != FSCK_FIX_FIXABLE) {
		fprintf(stderr,
			"Badblocks can be specified with --fix-fixable or "
			"--rebuild-tree only.\n");
		print_usage_and_exit();
	}

	if ((mode == FSCK_REBUILD) && (data->options & OPT_YES))
		data->options &= ~OPT_YES;

	data->mode = mode;
	if (!data->log)
		data->log = stdout;

	return argv[optind];
}

#define REBUILD_WARNING \
"*************************************************************\n\
** Do not  run  the  program  with  --rebuild-tree  unless **\n\
** something is broken and MAKE A BACKUP  before using it. **\n\
** If you have bad sectors on a drive  it is usually a bad **\n\
** idea to continue using it. Then you probably should get **\n\
** a working hard drive, copy the file system from the bad **\n\
** drive  to the good one -- dd_rescue is  a good tool for **\n\
** that -- and only then run this program.                 **\n\
*************************************************************\n\
\nWill rebuild the filesystem (%s) tree\n"
static void warn_what_will_be_done(char *file_name, struct fsck_data *data)
{
	FILE *warn_to;

	warn_to = (data->progress ? data->progress : stderr);

	if (data->mode == FSCK_REBUILD)
		reiserfs_warning(warn_to, REBUILD_WARNING, file_name);

	/* warn about fsck mode */
	switch (data->mode) {
	case FSCK_CHECK:
		reiserfs_warning(warn_to,
				 "Will read-only check consistency of the "
				 "filesystem on %s\n", file_name);

		break;

	case FSCK_FIX_FIXABLE:

		reiserfs_warning(warn_to,
				 "Will check consistency of the filesystem "
				 "on %s\n", file_name);
		reiserfs_warning(warn_to,
				 "and will fix what can be fixed without "
				 "--rebuild-tree\n");

		break;

	case FSCK_SB:
		reiserfs_warning(warn_to,
				 "Will check superblock and rebuild it if "
				 "needed\n");

		break;

	case FSCK_REBUILD:
		if (data->options & OPT_SAVE_PASSES_DUMP) {
			reiserfs_warning(warn_to,
					 "Will run only 1 step of the rebuilding, "
					 "write state file '%s' and exit\n",
					 data->rebuild.passes_dump_file_name);
		} else if (data->options & OPT_INTERACTIVE)
			reiserfs_warning(warn_to,
					 "Will stop after every stage and ask for "
					 "confirmation before continuing\n");

		if (data->rebuild.bitmap_file_name)
			reiserfs_warning(warn_to,
					 "Will try to load a bitmap--of all "
					 "ReiserFS leaves in the partition--from the file \n'%s'\n",
					 data->rebuild.bitmap_file_name);

		if (data->options & OPT_ADJUST_FILE_SIZE)
			reiserfs_warning(warn_to,
					 "\tWill set file sizes in their metadata "
					 "to real file sizes actually found by fsck.\n");

		if (data->options & OPT_HASH_DEFINED)
			reiserfs_warning(warn_to,
					 "\tSuppose \"%s\" hash is in use\n",
					 data->rebuild.defined_hash);

		break;

	case FSCK_ROLLBACK_CHANGES:
		reiserfs_warning(warn_to,
				 "Will rollback all data saved in %s into %s\n",
				 data->rebuild.rollback_file, file_name);

		break;
	case FSCK_CLEAN_ATTRIBUTES:
		reiserfs_warning(warn_to, "Will clean file attributes on %s\n",
				 file_name);
		break;
	case FSCK_AUTO:
		return;
	}

	if (data->options & OPT_SAVE_ROLLBACK
	    && data->mode != FSCK_ROLLBACK_CHANGES)
		reiserfs_warning(warn_to,
				 "Will save all blocks to be changed into "
				 "file '%s'\n", data->rebuild.rollback_file);

	if (data->options & BADBLOCKS_FILE)
		reiserfs_warning(warn_to,
				 "Bad block list will contain only blocks specified in '%s' "
				 "file\n", badblocks_file);

	reiserfs_warning(warn_to, "Will put log info to '%s'\n",
			 (data->log == stdout) ? "stdout" :
			 (data->log_file_name ? data->
			  log_file_name : "fsck.run"));

	if (!(data->options & OPT_YES) && !(data->options & OPT_SILENT) &&
	    !user_confirmed(warn_to,
			    "\nDo you want to run this program?[N/Yes] "
			    "(note need to type Yes if you do):", "Yes\n"))
		exit(EXIT_USER);
}

static dma_info_t dma_info;
static dma_info_t old_dma_info;

static void check_dma(int sig)
{
	old_dma_info = dma_info;
	if (get_dma_info(&dma_info) == -1) {
		fsck_log("get_dma_info failed %s\n", strerror(errno));
		return;
	}

	if (dma_info.dma != old_dma_info.dma) {
		if (dma_info.dma == 0) {
			printf
			    ("\n********************************************************************\n");
			printf
			    ("* Warning: It was just detected that dma mode was turned off while *\n");
			printf
			    ("* operating -- probably  due  to some  problem with your hardware. *\n");
			printf
			    ("* Please check your hardware and have a look into the syslog file. *\n");
			printf
			    ("* Note: running with --rebuild-tree on faulty hardware may destroy *\n");
			printf
			    ("* your data.                                                       *\n");
			printf
			    ("********************************************************************\n");
			if (fsck_log_file(fs) != stdout)
				fsck_log
				    ("WARNING: dma mode has been turned off.\n");
		}
	}
	if (dma_info.speed != old_dma_info.speed) {
		if (dma_info.speed < old_dma_info.speed) {
			printf
			    ("\n********************************************************************\n");
			printf
			    ("* Warning: It was just detected that dma speed was descreased while*\n");
			printf
			    ("* operating -- probably  due  to some  problem with your hardware. *\n");
			printf
			    ("* Please check your hardware and have a look into the syslog file. *\n");
			printf
			    ("* Note: running with --rebuild-tree on faulty hardware may destroy *\n");
			printf
			    ("* your data.                                                       *\n");
			printf
			    ("********************************************************************\n");
			if (fsck_log_file(fs) != stdout)
				fsck_log
				    ("WARNING: dma speed has been descreased.\n");
		}
	}

	alarm(1);
}

static void register_timer(void)
{
	memset(&dma_info, 0, sizeof(dma_info));
	memset(&old_dma_info, 0, sizeof(old_dma_info));

	dma_info.fd = fs->fs_dev;

	if (prepare_dma_check(&dma_info) != 0)
		return;

	if (get_dma_info(&dma_info) == -1) {
		fsck_log("get_dma_info failed %s\n", strerror(errno));
		return;
	}

	if (dma_info.dma == 0) {
		printf
		    ("\n******************************************************\n");
		printf
		    ("* Warning: The dma on your hard drive is turned off. *\n");
		printf
		    ("* This may really slow down the fsck process.        *\n");
		printf
		    ("******************************************************\n");
		if (fsck_log_file(fs) != stdout)
			fsck_log("WARNING: DMA is turned off\n");
	}

	signal(SIGALRM, check_dma);
	alarm(1);
}

static void reset_super_block(reiserfs_filsys_t fs)
{
	struct reiserfs_super_block *sb;
	struct reiserfs_journal_header *jh;

	sb = fs->fs_ondisk_sb;

	set_sb_free_blocks(sb, get_sb_block_count(sb));
	set_sb_root_block(sb, 0);
	set_sb_tree_height(sb, ~0);

	/* make file system invalid unless fsck finished () */
	set_sb_fs_state(sb, get_sb_fs_state(sb) | FS_FATAL);

/*
    if ( is_reiser2fs_jr_magic_string (sb) ) {???
	set_sb_version (sb, REISERFS_VERSION_3);
    }
    if (is_reiser2fs_magic_string (sb)) {
	set_sb_version (sb, REISERFS_FORMAT_3_6);
    }
    if (is_reiserfs_magic_string (sb)) {
	set_sb_version (sb, REISERFS_FORMAT_3_5);
    }
*/
	/* make sure that hash code in super block match to set hash function */
	set_sb_hash_code(sb, func2code(fs->fs_hash_function));

	if (fsck_hash_defined(fs)) {
		/* --hash was specifed */
		fs->fs_hash_function =
		    name2func(fsck_data(fs)->rebuild.defined_hash);
		set_sb_hash_code(sb, func2code(fs->fs_hash_function));
	}

	if (reiserfs_journal_opened(fs)) {
		jh = (struct reiserfs_journal_header *)fs->fs_jh_bh->b_data;

		/* reset journal params if needed. */
		if (memcmp
		    (sb_jp(sb), &jh->jh_journal,
		     sizeof(struct journal_params))) {
			if (is_reiserfs_jr_magic_string(sb))
				memcpy(sb_jp(sb), &jh->jh_journal,
				       sizeof(struct journal_params));
			else {
				set_sb_reserved_for_journal(sb, 0);

				set_jp_journal_dev(sb_jp(sb), 0);
				set_jp_journal_magic(sb_jp(sb), get_random());
				set_jp_journal_1st_block(sb_jp(sb),
							 get_journal_start_must
							 (fs));
				set_jp_journal_size(sb_jp(sb),
						    journal_default_size(fs->
									 fs_super_bh->
									 b_blocknr,
									 fs->
									 fs_blocksize));
				set_jp_journal_max_trans_len(sb_jp(sb),
							     advise_journal_max_trans_len
							     (get_jp_journal_max_trans_len
							      (sb_jp(sb)),
							      get_jp_journal_size
							      (sb_jp(sb)),
							      fs->fs_blocksize,
							      0));
				set_jp_journal_max_batch(sb_jp(sb),
							 advise_journal_max_batch
							 (get_jp_journal_max_trans_len
							  (sb_jp(sb))));
				set_jp_journal_max_commit_age(sb_jp(sb),
							      advise_journal_max_commit_age
							      ());
				set_jp_journal_max_trans_age(sb_jp(sb),
							     advise_journal_max_trans_age
							     ());

				set_jp_journal_dev(&jh->jh_journal, 0);
				set_jp_journal_magic(&jh->jh_journal,
						     get_jp_journal_magic(sb_jp
									  (sb)));
				set_jp_journal_1st_block(&jh->jh_journal,
							 get_jp_journal_1st_block
							 (sb_jp(sb)));
				set_jp_journal_size(&jh->jh_journal,
						    get_jp_journal_size(sb_jp
									(sb)));
				set_jp_journal_max_trans_len(&jh->jh_journal,
							     get_jp_journal_max_trans_len
							     (sb_jp(sb)));
				set_jp_journal_max_batch(&jh->jh_journal,
							 get_jp_journal_max_batch
							 (sb_jp(sb)));
				set_jp_journal_max_commit_age(&jh->jh_journal,
							      get_jp_journal_max_commit_age
							      (sb_jp(sb)));
				set_jp_journal_max_trans_age(&jh->jh_journal,
							     get_jp_journal_max_trans_age
							     (sb_jp(sb)));
			}
		}
	}

	/* objectid map is not touched */
	mark_buffer_dirty(fs->fs_super_bh);
	bwrite(fs->fs_super_bh);
	if (!(fsck_data(fs)->options & OPT_SAVE_PASSES_DUMP))
		mark_buffer_do_not_flush(fs->fs_super_bh);
}

#define START_FROM_THE_BEGINNING 	1
#define START_FROM_PASS_1 		2
#define START_FROM_PASS_2 		3
#define START_FROM_SEMANTIC 		4
#define START_FROM_LOST_FOUND 		5
#define START_FROM_PASS_4 		6

/* this decides where to start from  */
static int where_to_start_from(reiserfs_filsys_t fs)
{
	int ret;
	FILE *fp = NULL;
	int last_run_state;

	last_run_state = get_sb_fs_state(fs->fs_ondisk_sb);
	if (last_run_state == 0 || !fsck_run_one_step(fs))
		/**/ return START_FROM_THE_BEGINNING;

	/* We are able to perform the next step only if there is a file with the previous
	 * step results. */
	fp = open_file(state_dump_file(fs), "r");
	if (fp == NULL) {
		set_sb_fs_state(fs->fs_ondisk_sb, 0);
		return START_FROM_THE_BEGINNING;
	}

	/* check start and end magics of dump file */
	ret = is_stage_magic_correct(fp);

	if (ret <= 0 || ret != last_run_state)
		return START_FROM_THE_BEGINNING;

	switch (last_run_state) {
	case PASS_0_DONE:
		/* skip pass 0 */
		if (!fsck_user_confirmed
		    (fs, "Pass 0 seems finished. Start from pass 1?(Yes)",
		     "Yes\n", 1))
			fsck_exit("Run without -d then\n");

		load_pass_0_result(fp, fs);
		fclose(fp);
		return START_FROM_PASS_1;

	case PASS_1_DONE:
		/* skip pass 1 */
		if (!fsck_user_confirmed
		    (fs,
		     "Passes 0 and 1 seems finished. Start from "
		     "pass 2?(Yes)", "Yes\n", 1)) {
			fsck_exit("Run without -d then\n");
		}

		load_pass_1_result(fp, fs);
		fclose(fp);
		return START_FROM_PASS_2;

	case TREE_IS_BUILT:
		if (!fsck_user_confirmed
		    (fs,
		     "Internal tree of filesystem looks built. "
		     "Skip rebuilding?(Yes)", "Yes\n", 1)) {
			fsck_exit("Run without -d then\n");
		}

		load_pass_2_result(fs);
		fclose(fp);
		return START_FROM_SEMANTIC;
	case SEMANTIC_DONE:
		if (!fsck_user_confirmed
		    (fs,
		     "Passes 0 and 1 seems finished. Start from "
		     "pass 2?(Yes)", "Yes\n", 1)) {
			fsck_exit("Run without -d then\n");
		}

		load_semantic_result(fp, fs);
		fclose(fp);
		return START_FROM_LOST_FOUND;
	case LOST_FOUND_DONE:
		if (!fsck_user_confirmed
		    (fs,
		     "Passes 0 and 1 seems finished. Start from "
		     "pass 2?(Yes)", "Yes\n", 1)) {
			fsck_exit("Run without -d then\n");
		}

		load_lost_found_result(fs);
		fclose(fp);
		return START_FROM_PASS_4;
	}

	return START_FROM_THE_BEGINNING;
}

static void reiserfs_update_interval_fields(reiserfs_filsys_t fs)
{
	/* Not supported on v3.5 */
	if (get_sb_version(fs->fs_ondisk_sb) == REISERFS_FORMAT_3_5)
		return;

	set_sb_v2_lastcheck(fs->fs_ondisk_sb, time(NULL));
	set_sb_v2_mnt_count(fs->fs_ondisk_sb, 1);

	if (get_sb_v2_max_mnt_count(fs->fs_ondisk_sb) == 0)
		set_sb_v2_max_mnt_count(fs->fs_ondisk_sb,
					DEFAULT_MAX_MNT_COUNT);
	if (get_sb_v2_check_interval(fs->fs_ondisk_sb) == 0)
		set_sb_v2_check_interval(fs->fs_ondisk_sb,
					 DEFAULT_CHECK_INTERVAL);
}

static void mark_filesystem_consistent(reiserfs_filsys_t fs)
{
	if (!is_opened_rw(fs))
		return;

	if (!reiserfs_journal_opened(fs)) {
		/* make sure journal is not standard */
		if (!is_reiserfs_jr_magic_string(fs->fs_ondisk_sb))
			reiserfs_exit(EXIT_OPER,
				      "Filesystem with default journal "
				      "must be opened.");

		fsck_progress
		    ("WARNING: You must use reiserfstune to specify a new "
		     "journal before mounting it.\n");

		/* mark filesystem such that it is not mountable until
		 * new journaldevice is defined */
		set_jp_journal_magic(sb_jp(fs->fs_ondisk_sb), NEED_TUNE);
	}

	set_sb_umount_state(fs->fs_ondisk_sb, FS_CLEANLY_UMOUNTED);
	set_sb_fs_state(fs->fs_ondisk_sb, FS_CONSISTENT);
	reiserfs_update_interval_fields(fs);

	mark_buffer_dirty(fs->fs_super_bh);
}

static void reiserfsck_replay_journal(reiserfs_filsys_t fs)
{
	struct reiserfs_super_block *on_place_sb;
	int sb_size = reiserfs_super_block_size(fs->fs_ondisk_sb);

	/* keep the super_block in the separate memory to avoid problems with replaying
	 * broken parameters. */
	on_place_sb = (struct reiserfs_super_block *)fs->fs_super_bh->b_data;
	fs->fs_ondisk_sb = getmem(sb_size);
	memcpy(fs->fs_ondisk_sb, on_place_sb, sb_size);

	reiserfs_replay_journal(fs);

	/* Copy checked reliable sb fields from backed up sb to a new one. */
	set_sb_block_count(on_place_sb, get_sb_block_count(fs->fs_ondisk_sb));
	memcpy(sb_jp(on_place_sb), sb_jp(fs->fs_ondisk_sb),
	       sizeof(struct journal_params));
	set_sb_block_size(on_place_sb, get_sb_block_size(fs->fs_ondisk_sb));
	set_sb_oid_maxsize(on_place_sb, get_sb_oid_maxsize(fs->fs_ondisk_sb));
	memcpy(on_place_sb->s_v1.s_magic, fs->fs_ondisk_sb->s_v1.s_magic, 10);
	set_sb_hash_code(on_place_sb, get_sb_hash_code(fs->fs_ondisk_sb));
	set_sb_bmap_nr(on_place_sb, get_sb_bmap_nr(fs->fs_ondisk_sb));
	set_sb_version(on_place_sb, get_sb_version(fs->fs_ondisk_sb));
	set_sb_reserved_for_journal(on_place_sb,
				    get_sb_reserved_for_journal(fs->
								fs_ondisk_sb));

	if (sb_size == SB_SIZE) {
		set_sb_v2_flags(on_place_sb, get_sb_v2_flags(fs->fs_ondisk_sb));
		memcpy(on_place_sb->s_uuid, fs->fs_ondisk_sb->s_uuid, 16);
		memcpy(on_place_sb->s_label, fs->fs_ondisk_sb->s_label, 16);
	}

	/* get rid of SB copy */
	freemem(fs->fs_ondisk_sb);
	fs->fs_ondisk_sb = on_place_sb;
}

static int the_end(reiserfs_filsys_t fs)
{
	int ret = EXIT_FIXED;

	/* put bitmap and objectid map on place */
	reiserfs_delete_bitmap(fs->fs_bitmap2);
	fs->fs_bitmap2 = fsck_new_bitmap(fs);
	if (!fs->fs_bitmap2->bm_dirty)
		die("Bitmap not dirty");

//    id_map_flush(proper_id_map (fs), fs);
	id_map_flush(semantic_id_map(fs), fs);
	id_map_free(proper_id_map(fs));
	id_map_free(semantic_id_map(fs));

/*    set_sb_free_blocks (sb, reiserfs_bitmap_zeros (fsck_new_bitmap (fs)));*/

	mark_filesystem_consistent(fs);
	clear_buffer_do_not_flush(fs->fs_super_bh);

	if (fsck_data(fs)->mounted == MF_RO) {
		reiserfs_warning(stderr, "\nThe partition is mounted ro. It "
				 "is better to umount and mount it again.\n\n");
		ret = EXIT_REBOOT;
	}

	/* write all dirty blocks */
	fsck_progress("Syncing..");
	fs->fs_dirt = 1;
	clean_after_dma_check(fs->fs_dev, &dma_info);
	fsck_progress("finished\n");
	reiserfs_close(fs);

	return ret;
}

/* check umounted or read-only mounted filesystems only */
static void prepare_fs_for_check(reiserfs_filsys_t fs)
{
	/* The method could be called from auto_check already. */
	if (fs->fs_flags == O_RDWR)
		return;

	reiserfs_reopen(fs, O_RDWR);

	fsck_data(fs)->mounted = misc_device_mounted(fs->fs_file_name);

	if (fsck_data(fs)->mounted > 0) {
		if (fsck_data(fs)->mounted == MF_RW) {
			fsck_progress
			    ("Partition %s is mounted with write permissions, "
			     "cannot check it\n", fs->fs_file_name);
			reiserfs_close(fs);
			exit(EXIT_USER);
		}

		/* If not CHECK mode, lock the process in the memory. */
		if (fsck_mode(fs) != FSCK_CHECK) {
			if (mlockall(MCL_CURRENT)) {
				reiserfs_exit(EXIT_OPER,
					      "Failed to lock the process to "
					      "fsck the mounted ro partition. %s.\n",
					      strerror(errno));
			}
		}

		if (fsck_skip_journal(fs)) {
			reiserfs_exit(EXIT_USER, "Jounrnal of the mounted "
				      "filesystem must be specified.\n");
		}

		if (!reiserfs_journal_opened(fs)) {
			/* just to make sure */
			reiserfs_panic("Journal is not opened");
		} else if (reiserfs_journal_params_check(fs)) {
			reiserfs_close(fs);
			exit(EXIT_FATAL);
		}

		fsck_progress
		    ("Filesystem seems mounted read-only. Skipping journal "
		     "replay.\n");
	} else if (!fsck_skip_journal(fs)) {
		if (reiserfs_journal_params_check(fs)) {
			reiserfs_close(fs);
			exit(EXIT_FATAL);
		}

		/* filesystem is not mounted, replay journal before checking */
		reiserfsck_replay_journal(fs);
	}
}

static void rebuild_tree(reiserfs_filsys_t fs)
{
	time_t t;
	int ret;

	init_rollback_file(state_rollback_file(fs), &fs->fs_blocksize,
			   fsck_data(fs)->log);

	prepare_fs_for_check(fs);

	ret = reiserfs_open_ondisk_bitmap(fs);
	if (ret < 0) {
		fsck_progress("reiserfsck: Could not open bitmap\n");
		reiserfs_close(fs);
		exit(EXIT_OPER);
	} else if (ret > 0) {
		fsck_log
		    ("Zero bit found in on-disk bitmap after the last valid bit. "
		     "Fixed.\n");
	}

	time(&t);
	fsck_progress("###########\n"
		      "reiserfsck --rebuild-tree started at %s"
		      "###########\n", ctime(&t));

	switch (where_to_start_from(fs)) {
	case START_FROM_THE_BEGINNING:
		reset_super_block(fs);
		pass_0(fs);

	case START_FROM_PASS_1:
		reset_super_block(fs);
		pass_1(fs);

	case START_FROM_PASS_2:
		pass_2(fs);

	case START_FROM_SEMANTIC:
		pass_3_semantic(fs);

		/* if --lost+found is set - link unaccessed directories to lost+found
		   directory */
	case START_FROM_LOST_FOUND:
		pass_3a_look_for_lost(fs);

	case START_FROM_PASS_4:
		/* 4. look for unaccessed items in the leaves */
		pass_4_check_unaccessed_items();

		ret = the_end(fs);
	}

	close_rollback_file();

	time(&t);
	fsck_progress("###########\n"
		      "reiserfsck finished at %s" "###########\n", ctime(&t));
	exit(ret);
}

static void clean_attributes(reiserfs_filsys_t fs)
{
	time_t t;

	time(&t);

	if (get_sb_umount_state(fs->fs_ondisk_sb) != FS_CLEANLY_UMOUNTED) {
		fsck_progress("Filesystem is not clean\n"
			      "Check consistency of the partition first.\n");
		exit(EXIT_USER);
	}
	if (get_sb_fs_state(fs->fs_ondisk_sb) != FS_CONSISTENT) {
		fsck_progress("Filesystem seems to be in unconsistent state.\n"
			      "Check consistency of the partition first.\n");
		exit(EXIT_USER);
	}

	if (get_reiserfs_format(fs->fs_ondisk_sb) != REISERFS_FORMAT_3_6) {
		fsck_progress
		    ("Filesystems of 3_5 format do not support extended "
		     "attributes.\n");

		exit(EXIT_USER);
	}
	fsck_progress("###########\n"
		      "reiserfsck --clean-attributes started at %s"
		      "###########\n", ctime(&t));

	init_rollback_file(state_rollback_file(fs), &fs->fs_blocksize,
			   fsck_data(fs)->log);

	prepare_fs_for_check(fs);

	do_clean_attributes(fs);

	clean_after_dma_check(fs->fs_dev, &dma_info);

	fsck_progress("###########\n"
		      "reiserfsck finished at %s" "###########\n", ctime(&t));
	reiserfs_close(fs);
	close_rollback_file();

	exit(EXIT_FIXED);

}

static int reiserfs_check_auto_state(reiserfs_filsys_t fs)
{
	time_t now = time(NULL);
	time_t lastcheck = get_sb_v2_lastcheck(fs->fs_ondisk_sb);
	unsigned int mnt_count = get_sb_v2_mnt_count(fs->fs_ondisk_sb);
	unsigned int max_mnt_count = get_sb_v2_max_mnt_count(fs->fs_ondisk_sb);
	unsigned int interval = get_sb_v2_check_interval(fs->fs_ondisk_sb);

	/* v3.5 file systems don't have the superblock fields for this */
	if (get_sb_version(fs->fs_ondisk_sb) == REISERFS_FORMAT_3_5)
		return 1;

	if (lastcheck == 0 || mnt_count == 0 || max_mnt_count == 0 ||
	    interval == 0) {
		fprintf(stderr, "File system hasn't been enabled for faster "
			"boot-time checking. It will be enabled after a "
			"successful run.\n");
		return 1;
	}

	if (interval != UINT_MAX && now > lastcheck + interval) {
		fprintf(stderr, "File system hasn't been checked in %lu days. "
			"Checking now.\n", (now - lastcheck) / (60 * 60 * 24));
		return 1;
	}

	if (interval != UINT_MAX && lastcheck > now) {
		fprintf(stderr, "File system check timestamp is in the future. "
			"Checking now.\n");
		return 1;
	}

	if (mnt_count > max_mnt_count && max_mnt_count != USHRT_MAX) {
		fprintf(stderr, "File system has been mounted %u times "
			"without being checked. Checking now.\n", mnt_count);
		return 1;
	}

	return 0;
}

static int auto_check(reiserfs_filsys_t fs)
{
	__u16 state;
	int retval = 0;
	int force = fsck_data(fs)->options & OPT_FORCE;

	print_super_block(stdout, fs, fs->fs_file_name, fs->fs_super_bh, 1);

	state = get_sb_fs_state(fs->fs_ondisk_sb);
	if ((state & FS_FATAL) == FS_FATAL) {
		fprintf(stderr,
			"Filesystem seems to have fatal corruptions. Running "
			"with --rebuild-tree is required.\n");
		goto error;
	}

	if ((state & FS_ERROR) == FS_ERROR) {
		fprintf(stderr,
			"Some corruptions on the filesystem were detected. Switching to "
			"the --fix-fixable mode.\n");
		/* run fixable pass. */
		return 0;
	}

	if (state != FS_CONSISTENT)
		fprintf(stderr,
			"Some strange state was specified in the super block. "
			"Do usual check.\n");

	if (get_sb_umount_state(fs->fs_ondisk_sb) == FS_CLEANLY_UMOUNTED &&
	    !force && !reiserfs_check_auto_state(fs))
		exit(EXIT_OK);
	prepare_fs_for_check(fs);
	if (!force && !reiserfs_check_auto_state(fs))
		exit(EXIT_OK);

	/* Check bitmaps. */
	retval = reiserfs_open_ondisk_bitmap(fs);

	if (retval > 0) {
		fsck_log
		    ("Zero bit found in on-disk bitmap after the last valid bit. "
		     "Switching to --fix-fixable mode.\n");
		/* run fixable pass. */
		return 0;
	} else if (retval < 0) {
		fsck_progress("reiserfsck: Could not open bitmap\n");
		goto error;
	}

	if (get_sb_block_count(fs->fs_ondisk_sb) -
	    get_sb_free_blocks(fs->fs_ondisk_sb) !=
	    fs->fs_bitmap2->bm_set_bits) {
		fsck_log
		    ("Wrong amount of used blocks. Switching to the --fix-fixable mode.\n");
		/* run fixable pass. */
		return 0;
	}

	check_fs_tree(fs);

	if (fsck_data(fs)->check.fatal_corruptions) {
		fprintf(stderr,
			"%lu fatal corruption(s) found in the root block. Running "
			"with the --rebuild-tree is required.\n",
			fsck_data(fs)->check.fatal_corruptions);
		goto fatal_error;
	} else if (fsck_data(fs)->check.fixable_corruptions) {
		/* seems that this cannot happen. */
		fprintf(stderr, "%lu fixable corruption(s) found. Switching to "
			"the --fix-fixable mode.\n",
			fsck_data(fs)->check.fixable_corruptions);
		fsck_data(fs)->check.fixable_corruptions = 0;
		/* run fixable pass. */
		return 0;
	}

	reiserfs_update_interval_fields(fs);
	mark_buffer_dirty(fs->fs_super_bh);
	bwrite(fs->fs_super_bh);
	fs->fs_dirt = 1;

	clean_after_dma_check(fs->fs_dev, &dma_info);

	reiserfs_close(fs);
	/* do not do anything else. */
	exit(EXIT_OK);

fatal_error:
	set_sb_fs_state(fs->fs_ondisk_sb, FS_FATAL);
	mark_buffer_dirty(fs->fs_super_bh);
	bwrite(fs->fs_super_bh);
error:
	clean_after_dma_check(fs->fs_dev, &dma_info);
	reiserfs_close(fs);
	exit(EXIT_FATAL);
}

/* check umounted or read-only mounted filesystems only */
static void check_fs(reiserfs_filsys_t fs)
{
	int retval = EXIT_OK;
	time_t t;

	time(&t);

	if (fsck_mode(fs) != FSCK_FIX_FIXABLE) {
		fsck_progress("###########\n"
			      "reiserfsck --check started at %s"
			      "###########\n", ctime(&t));
	} else {
		fsck_progress("###########\n"
			      "reiserfsck --fix-fixable started at %s"
			      "###########\n", ctime(&t));
	}

	init_rollback_file(state_rollback_file(fs), &fs->fs_blocksize,
			   fsck_data(fs)->log);

	prepare_fs_for_check(fs);

	if (!fs->fs_bitmap2)
		/* It could be done on auto_check already. */
		retval = reiserfs_open_ondisk_bitmap(fs);

	if (retval > 0) {
		if (fsck_mode(fs) != FSCK_FIX_FIXABLE) {
			fsck_log
			    ("Zero bit found in on-disk bitmap after the last valid "
			     "bit.\n");

			one_more_corruption(fs, FIXABLE);
		} else {
			fsck_log
			    ("Zero bit found in on-disk bitmap after the last valid "
			     "bit. Fixed.\n");
		}
	} else if (retval < 0) {
		fsck_progress("reiserfsck: Could not open bitmap\n");
		reiserfs_close(fs);
		exit(EXIT_OPER);
	}

	check_fs_tree(fs);

	semantic_check();

	if (fsck_data(fs)->check.fatal_corruptions) {
		fsck_progress
		    ("%lu found corruptions can be fixed only when running with "
		     "--rebuild-tree\n",
		     fsck_data(fs)->check.fatal_corruptions);

		set_sb_fs_state(fs->fs_ondisk_sb, FS_FATAL);
		mark_buffer_dirty(fs->fs_super_bh);
		retval = EXIT_FATAL;
	} else if (fsck_data(fs)->check.fixable_corruptions) {
		/* fixable corruptions found */
		if (fsck_mode(fs) == FSCK_FIX_FIXABLE) {
			/* fixable corruptions found and fix-fixable has not fixed them,
			 * do rebuild-tree */

			fsck_progress
			    ("Fatal error: %lu fixable corruptions found after "
			     "--fix-fixable.\n",
			     fsck_data(fs)->check.fixable_corruptions);
			retval = EXIT_OPER;
		} else {
			fsck_progress
			    ("%lu found corruptions can be fixed when running with "
			     "--fix-fixable\n",
			     fsck_data(fs)->check.fixable_corruptions);

			retval = EXIT_FIXABLE;
		}
		set_sb_fs_state(fs->fs_ondisk_sb, FS_ERROR);
		mark_buffer_dirty(fs->fs_super_bh);
	} else {
		fsck_progress("No corruptions found\n");
		stage_report(5, fs);

		if (fsck_mode(fs) != FSCK_CHECK) {
			if (misc_device_mounted(fs->fs_file_name) == MF_RO) {
				reiserfs_warning(stderr,
						 "\nThe partition is mounted ro. It is better "
						 "to umount and mount it again.\n\n");
				retval = EXIT_REBOOT;
			} else
				retval = EXIT_FIXED;
		} else
			retval = EXIT_OK;

		mark_filesystem_consistent(fs);
	}

	if (fsck_mode(fs) == FSCK_FIX_FIXABLE
	    && !fsck_data(fs)->check.fatal_corruptions)
		id_map_flush(proper_id_map(fs), fs);

	id_map_free(proper_id_map(fs));
	clean_after_dma_check(fs->fs_dev, &dma_info);

	//clear_relocated_list();

	time(&t);
	fsck_progress("###########\n"
		      "reiserfsck finished at %s" "###########\n", ctime(&t));

	reiserfs_close(fs);
	close_rollback_file();
	exit(retval);
}

static int open_devices_for_rollback(char *file_name, struct fsck_data *data)
{
	int fd;

	fd = open(file_name, O_RDWR | O_LARGEFILE);

	if (fd == -1) {
		reiserfs_warning(stderr, "reiserfsck: Cannot not open the fs "
				 "partition %s\n", file_name);

		return -1;
	}

	fs = getmem(sizeof(*fs));
	fs->fs_dev = fd;
	fs->fs_vp = data;
	asprintf(&fs->fs_file_name, "%s", file_name);

	if (data->journal_dev_name && strcmp(data->journal_dev_name, file_name)) {
		fs->fs_journal_dev = open(data->journal_dev_name,
					  O_RDWR | O_LARGEFILE);
		if (fs->fs_journal_dev == -1) {
			reiserfs_warning(stderr,
					 "Cannot open journal partition\n");
			return -1;
		}
	}

	if (open_rollback_file(state_rollback_file(fs), fsck_data(fs)->log))
		return -1;

	return 0;
}

static void fsck_rollback(reiserfs_filsys_t fs)
{
	time_t t;

	time(&t);
	fsck_progress("###########\n"
		      "reiserfsck --rollback-fsck-changes started at %s"
		      "###########\n", ctime(&t));

	do_fsck_rollback(fs->fs_dev, fs->fs_journal_dev,
			 fsck_progress_file(fs));
	close_rollback_file();

	close(fs->fs_journal_dev);
	free(fs->fs_file_name);
	fs->fs_file_name = NULL;
	close(fs->fs_dev);
	freemem(fs);

	time(&t);
	fsck_progress("###########\n"
		      "reiserfsck finished at %s" "###########\n", ctime(&t));

	exit(EXIT_FIXED);
}

int main(int argc, char *argv[])
{
	char *file_name;
	struct fsck_data *data;
	struct rlimit rlim = { RLIM_INFINITY, RLIM_INFINITY };
	char *width;
	int retval;
	long error;

	width = getenv("COLUMNS");
	if (width)
		screen_width = atoi(width);

	if (screen_width == 0)
		screen_width = 80;	// We default to 80 characters wide screen
	screen_width--;
	screen_savebuffer_len = screen_width;
	screen_savebuffer = getmem(screen_width + 1);
	memset(screen_savebuffer, 0, screen_savebuffer_len + 1);

	/* this is only needed (and works) when running under 2.4 on regular files */
	if (setrlimit(RLIMIT_FSIZE, &rlim) == -1) {
		reiserfs_warning(stderr,
				 "Cannot change the system limit for file size "
				 "with setrlimit: %s\n", strerror(errno));
	}

	data = getmem(sizeof(struct fsck_data));

	file_name = parse_options(data, argc, argv);

	if (data->mode != FSCK_AUTO)
		print_banner("reiserfsck");

	if (data->mode == DO_NOTHING) {
		freemem(data);
		exit(EXIT_OK);
	}

	if (data->options & OPT_BACKGROUND) {
		/* running in background reiserfsck appends progress information into
		   'fsck.run'. Logs get there if log file was not specified */
		data->options |= OPT_QUIET;
		data->progress = fopen("fsck.run", "a+");
		if (!data->progress) {
			reiserfs_exit(EXIT_OPER,
				      "reiserfsck: Cannot not open \"fsck.run\"");
		}

		if (data->log == stdout)
			/* no log file specifed - redirect log into 'fsck.run' */
			data->log = data->progress;

		retval = fork();
		if (retval == -1) {
			reiserfs_panic("reiserfsck: Fork failed: %s",
				       strerror(errno));
		} else if (retval != 0) {
			exit(EXIT_OPER);
		}
		reiserfs_warning(stderr,
				 "\nReiserfsck is running in background as "
				 "[%d],\nmake sure that it gets all the confirmations from stdin "
				 "that it requests.\n\n", getpid());
	}

	/* This asks for confirmation also. */
	if (data->mode != FSCK_AUTO)
		warn_what_will_be_done(file_name, data);

	if (data->mode == FSCK_ROLLBACK_CHANGES) {
		if (open_devices_for_rollback(file_name, data) == -1)
			exit(EXIT_OPER);
	} else {
		fs = reiserfs_open(file_name, O_RDONLY, &error, data,
				   data->mode != FSCK_SB);

		if (error) {
			reiserfs_exit(EXIT_OPER, "Failed to open the device "
				      "'%s': %s\n\n", file_name,
				      error_message(error));
		}

		if (data->mode != FSCK_SB) {
			if (no_reiserfs_found(fs)) {
				reiserfs_exit(EXIT_OPER,
					      "Failed to open the filesystem.\n\n"
					      "If the partition table has not been changed, "
					      "and the partition is\nvalid  and  it really  "
					      "contains  a reiserfs  partition,  then the\n"
					      "superblock  is corrupted and you need to run "
					      "this utility with\n--rebuild-sb.\n");
			}
			if (fsck_skip_journal(fs) &&
			    !is_reiserfs_jr_magic_string(fs->fs_ondisk_sb)) {
				reiserfs_warning(stderr,
						 "Filesystem with default journal found, "
						 "--no-journal-available is ignored\n");
				fsck_data(fs)->options &= ~OPT_SKIP_JOURNAL;
			}

			if (!fsck_skip_journal(fs)) {
				retval =
				    reiserfs_open_journal(fs,
							  data->
							  journal_dev_name,
							  O_RDONLY);

				if (retval) {
					fsck_progress
					    ("Failed to open the journal device (%s).\n",
					     data->journal_dev_name);

					if (retval == 1) {
						fsck_progress
						    ("Run --rebuild-sb to rebuild journal parameters.\n");
					}

					reiserfs_close(fs);
					exit(EXIT_OPER);
				}
			}

			if (data->options & BADBLOCKS_FILE) {
				if (create_badblock_bitmap(fs, badblocks_file)
				    != 0)
					exit(EXIT_OPER);
			}

			register_timer();
		}
	}

	switch (data->mode) {
	case FSCK_SB:
		rebuild_sb(fs, file_name, data);
		break;

	case FSCK_AUTO:
		/* perform some light-weight checks. If error, do fixable job. */
		if (auto_check(fs))
			break;
		data->mode = FSCK_FIX_FIXABLE;
	case FSCK_CHECK:
	case FSCK_FIX_FIXABLE:
		check_fs(fs);
		break;

	case FSCK_REBUILD:
	case DO_TEST:
		rebuild_tree(fs);
		break;

	case FSCK_ROLLBACK_CHANGES:
		fsck_rollback(fs);
		break;
	case FSCK_CLEAN_ATTRIBUTES:
		clean_attributes(fs);
	}

	exit(EXIT_OPER);
}
