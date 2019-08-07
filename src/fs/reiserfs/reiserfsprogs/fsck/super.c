/*
 * Copyright 2001-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
#    include <uuid/uuid.h>
#endif

#define fsck_conditional_log(sb_found, fmt, list...) {	\
    if (sb_found)					\
    	fsck_log(fmt, ## list);				\
}

static int what_fs_version(void)
{
	size_t n = 0;
	char *answer = NULL;
	int version;
	printf("\nwhat the version of ReiserFS do you use[1-4]\n"
	       "\t(1)   3.6.x\n"
	       "\t(2) >=3.5.9 (introduced in the middle of 1999) (if you use linux 2.2, choose this one)\n"
	       "\t(3) < 3.5.9 converted to new format (don't choose if unsure)\n"
	       "\t(4) < 3.5.9 (this is very old format, don't choose if unsure)\n"
	       "\t(X)   exit\n");
	getline(&answer, &n, stdin);
	version = atoi(answer);
	if (version < 1 || version > 4)
		die("rebuild_sb: wrong version");
	return version;
}

/*
#define super_error(exit_code, text) {	\
    fsck_log(text);			\
    return exit_code;			\
}

int check_sb (reiserfs_filsys_t fs) {
    int magic = 0, version = 0;

    if (!is_blocksize_correct (fs->fs_blocksize))
	super_error(-1, "Wrong blocksize found in the super block\n");

    if (is_reiserfs_3_6_magic_string (sb))
	magic = 2;
    else if (is_reiserfs_3_5_magic_string (sb))
	magic = 1;
    else if (is_reiserfs_jr_magic_string (sb))
	magic = 3;
    else
	super_error(-1, "Invalid magic found in the super block.\n");

    if (magic == 1 || magic == 2) {
	if (fsck_data (fs)->journal_dev_name)
            fsck_log("Reiserfs with standard journal found, but there was specified a "
		"journal dev\n");
	
	standard_journal = 1;
    } else {
	if (!fsck_data (fs)->journal_dev_name)
            super_error(-1, "Reiserfs with non standard journal found, but there was not "
		"specified any journal dev\n");
	
	standard_journal = 0;
    }

    if (get_sb_version (sb) != REISERFS_FORMAT_3_6 && get_sb_version (sb) != REISERFS_FORMAT_3_5)
	super_error(-1, "Invalid format found in the super block.\n");
	
    if (is_new_sb_location(fs->fs_super_bh->b_blocknr, fs->fs_blocksize))
    {
	if (magic == 3)
	    version = get_sb_version (sb) == REISERFS_FORMAT_3_6 ? 1 : 2;
	else
	    version = magic == 2 ? 1 : 2;	
    } else if (is_old_sb_location(fs->fs_super_bh->b_blocknr, fs->fs_blocksize)) {
	if (magic == 3)
	    version = get_sb_version (sb) == REISERFS_FORMAT_3_6 ? 3 : 4;
	else
	    version = magic == 2 ? 3 : 4;	
    } else
	die("Super block in the wong block(%d).\n", fs->fs_super_bh->b_blocknr);

    if (version == 0)
	die ("FS format must be figured out here.\n");

    if (get_sb_block_count (sb) > count_blocks (filename, fs->fs_blocksize))
	super_error(-1, "Invalid block count found in the super block.\n");

    if (get_sb_block_size (sb) != fs->fs_blocksize)
        super_error(-1, "Invalid block size found in the super block (%lu).\n",
	    get_sb_block_size (sb));

//Non needed from here
    p_oid_maxsize = (fs->fs_blocksize - reiserfs_super_block_size (sb)) / sizeof(__u32) / 2 * 2;
    if (get_sb_oid_maxsize (sb) != p_oid_maxsize)
	super_error(-1, "Invalid objectid map max size found in the super block (%lu).\n",
	    get_sb_oid_maxsize (sb));

    if (get_sb_oid_cursize (sb) == 1 || get_sb_oid_cursize (sb) > get_sb_oid_maxsize (sb))
        super_error(-1, "Invalid objectid map found in the super block (%lu).\n",
	    get_sb_oid_cursize (sb));

    if (get_sb_root_block (sb) > block_count && get_sb_root_block (sb) != ~0)
	fsck_log("Invalid root block found in the super block (%lu).\n",
	    get_sb_root_block (sb));

    if (get_sb_free_blocks (sb) > block_count)
	fsck_log ("Invalid free block count found in the super block (%lu).\n",
	    get_sb_free_blocks (sb));

    if (get_sb_tree_height (sb) && ((get_sb_tree_height (sb) < DISK_LEAF_NODE_LEVEL + 1) ||
	(get_sb_tree_height (sb) > MAX_HEIGHT) && (get_sb_tree_height (sb) != ~0)))
	super_error(-1, "Invalid tree height found in the super block (%lu).\n",
	    get_sb_tree_height (sb));

    if (get_sb_hash_code (sb) && code2name (get_sb_hash_code (sb)) == 0)
	super_error(-1, "Invalid hash found in the super block (%lu).\n",
	    get_sb_hash_code (sb));

    if (version == 1 || version == 3) {
        if (!uuid_is_correct(sb->s_uuid))
	    fsck_log ("Invalid uuid found, you should generate a new one.\n");

	if (sb->s_flags & 0xfffffffe)
	    fsck_log ("rebuild-sb: super block flags found (%u), zeroed\n", sb->s_flags);
    }

//Not needed till here.

    p_bmap_nr = (block_count + (fs->fs_blocksize * 8 - 1)) / (fs->fs_blocksize * 8);
    if (get_sb_bmap_nr (sb) != p_bmap_nr)
	super_error(-1, "Invalid bitmap number found in the super block (%lu).\n",
	    get_sb_bmap_nr (sb));

    if (!fsck_skip_journal (fs) && standard_journal == 1) {
        if (get_jp_journal_dev (sb_jp(sb)) != 0)
 	    super_error(-1, "Invalid journal device found (%lu).\n", get_jp_journal_dev (sb_jp(sb)));

        if (get_jp_journal_1st_block (sb_jp(sb)) != get_journal_start_must (fs))
            super_error(-1, "Invalid journal first block found (%lu).\n", get_jp_journal_1st_block (sb_jp(sb)));

        if (get_jp_journal_size (sb_jp(sb)) != journal_default_size(fs->fs_super_bh->b_blocknr, fs->fs_blocksize))
	    super_error(-1, "Invalid journal size found (%lu).\n", get_jp_journal_size (sb_jp(sb)) + 1);


	
	if (get_jp_journal_max_batch (sb_jp(sb)) != advise_journal_max_batch(get_jp_journal_max_trans_len (sb_jp(sb)))) {
 	    fsck_conditional_log (magic_was_found, "rebuild-sb: wrong journal max batch size occured (%lu), fixed (%d)\n",
 	        get_jp_journal_max_batch (sb_jp(sb)), advise_journal_max_batch(get_jp_journal_max_trans_len (sb_jp(sb))));
 	    set_jp_journal_max_batch (sb_jp(sb), advise_journal_max_batch(get_jp_journal_max_trans_len (sb_jp(sb))));
        }
        if (get_jp_journal_max_commit_age (sb_jp(sb)) != advise_journal_max_commit_age()) {
 	    fsck_conditional_log (magic_was_found, "rebuild-sb: wrong journal  max commit age occured (%lu), fixed (%d)\n",
 	        get_jp_journal_max_commit_age (sb_jp(sb)), advise_journal_max_commit_age());
 	    set_jp_journal_max_commit_age (sb_jp(sb), advise_journal_max_commit_age());
        }
        if (get_jp_journal_max_trans_age (sb_jp(sb)) != advise_journal_max_trans_age()) {
 	    fsck_log ("rebuild-sb: wrong journal  max commit age occured (%lu), fixed (0)\n",
 	        get_jp_journal_max_trans_age (sb_jp(sb)), advise_journal_max_trans_age());
 	    set_jp_journal_max_trans_age (sb_jp(sb), advise_journal_max_trans_age());
        }


}
*/

void rebuild_sb(reiserfs_filsys_t fs, char *filename, struct fsck_data *data)
{
	int version = 0;
	struct reiserfs_super_block *ondisk_sb = NULL;
	struct reiserfs_super_block *sb = NULL;
	struct reiserfs_journal_header *j_head;

	int magic_was_found = 0;
	unsigned long block_count = 0;
	__u16 p_oid_maxsize;
	unsigned int bmap_nr;
	__u32 p_jp_journal_1st_block = 0;
	__u32 p_jp_dev_size = 0;
	int standard_journal = -1;
	char *journal_dev_name = NULL;
	char *tmp;
	int sb_size;

	char *answer = NULL;
	size_t n = 0;
	struct stat stat_buf;
	int retval, exit_code = EXIT_OK;
	long error;

	if (!no_reiserfs_found(fs)) {
		sb = getmem(sizeof(*sb));
		if (!is_opened_rw(fs)) {
			close(fs->fs_dev);
			fs->fs_dev =
			    open(fs->fs_file_name, O_RDWR | O_LARGEFILE);
		}

		if (!is_blocksize_correct(fs->fs_blocksize)) {
			printf
			    ("\nCannot find a proper blocksize, enter block size [4096]: \n");
			getline(&answer, &n, stdin);
			if (strcmp(answer, "\n")) {
				retval = (int)strtol(answer, &tmp, 0);
				if ((*tmp && strcmp(tmp, "\n")) || retval < 0)
					reiserfs_exit(EXIT_USER,
						      "rebuild_sb: wrong block size specified\n");
				if (!is_blocksize_correct(retval))
					reiserfs_exit(EXIT_USER,
						      "rebuild_sb: wrong block size specified, "
						      "only power of 2 from 512-8192 interval are supported.\n");
			} else
				retval = 4096;

			fs->fs_blocksize = retval;
		}

		if (!(block_count = count_blocks(filename, fs->fs_blocksize)))
			exit(EXIT_OPER);

		/* save ondisk_sb somewhere and work in temp area */
		ondisk_sb = fs->fs_ondisk_sb;
		memcpy(sb, fs->fs_ondisk_sb, sizeof(*sb));
		fs->fs_ondisk_sb = sb;

		if (is_reiserfs_3_6_magic_string(sb)) {
			/* 3_6 magic */
			if (fsck_data(fs)->journal_dev_name)
				/* journal dev must not be specified with standard journal */
				reiserfs_exit(EXIT_USER,
					      "ReiserFS with default journal "
					      "is found, but there was specified a journal device.");

			if (fs->fs_super_bh->b_blocknr ==
			    REISERFS_DISK_OFFSET_IN_BYTES / fs->fs_blocksize)
				version = 1;
			else if (fs->fs_super_bh->b_blocknr ==
				 REISERFS_OLD_DISK_OFFSET_IN_BYTES /
				 fs->fs_blocksize)
				version = 3;

			magic_was_found = 2;
		} else if (is_reiserfs_3_5_magic_string(sb)) {
			if (fsck_data(fs)->journal_dev_name)
				/* journal dev must not be specified with standard journal */
				reiserfs_exit(EXIT_USER,
					      "ReiserFS with default journal "
					      "is found, but there was specified a journal device.");

			/* 3_5 magic */
			if (fs->fs_super_bh->b_blocknr ==
			    REISERFS_DISK_OFFSET_IN_BYTES / fs->fs_blocksize)
				version = 2;
			else if (fs->fs_super_bh->b_blocknr ==
				 REISERFS_OLD_DISK_OFFSET_IN_BYTES /
				 fs->fs_blocksize)
				version = 4;

			magic_was_found = 1;
		} else if (is_reiserfs_jr_magic_string(sb)) {
			if (!fsck_data(fs)->journal_dev_name
			    && !fsck_skip_journal(fs))
				/* journal dev must be specified with non standard journal */
				reiserfs_exit(EXIT_USER,
					      "ReiserFS with non default journal "
					      "is found, but there was not specified any journal device.");

			if (get_sb_version(sb) == REISERFS_FORMAT_3_6) {
				/*non-standard magic + sb_format == 3_6 */
				if (fs->fs_super_bh->b_blocknr ==
				    REISERFS_DISK_OFFSET_IN_BYTES /
				    fs->fs_blocksize)
					version = 1;
				else if (fs->fs_super_bh->b_blocknr ==
					 REISERFS_OLD_DISK_OFFSET_IN_BYTES /
					 fs->fs_blocksize)
					version = 3;

				magic_was_found = 3;
			} else if (get_sb_version(sb) == REISERFS_FORMAT_3_5) {
				/* non-standard magic + sb_format == 3_5 */
				if (fs->fs_super_bh->b_blocknr ==
				    REISERFS_DISK_OFFSET_IN_BYTES /
				    fs->fs_blocksize)
					version = 2;
				else if (fs->fs_super_bh->b_blocknr ==
					 REISERFS_OLD_DISK_OFFSET_IN_BYTES /
					 fs->fs_blocksize)
					version = 4;

				magic_was_found = 3;
			} else {
				/* non-standard magic + bad sb_format */
				version = 0;
				magic_was_found = 3;
			}
		} else
			reiserfs_exit(EXIT_USER,
				      "We opened device but there is no magic "
				      "and there is no correct superblock format found.");

		if (magic_was_found == 1 || magic_was_found == 2)
			standard_journal = 1;
		else
			standard_journal = 0;

		if (version == 0)
			version = what_fs_version();

		if (get_sb_block_count(sb) != block_count) {
			do {
				printf("\nDid you use resizer(y/n)[n]: ");
				getline(&answer, &n, stdin);
			} while (strcmp("y\n", answer) &&
				 strcmp("n\n", answer) && strcmp("\n", answer));

			if (!strcmp("y\n", answer)) {
				printf("\nEnter partition size [%lu]: ",
				       block_count);
				getline(&answer, &n, stdin);
				if (strcmp("\n", answer))
					block_count = atoi(answer);
			} else {
				fsck_conditional_log(magic_was_found,
						     "rebuild-sb: wrong block count "
						     "occured (%lu), fixed (%lu)\n",
						     get_sb_block_count(sb),
						     block_count);
			}

			set_sb_block_count(sb, block_count);
		}

		if (get_sb_block_size(sb) != fs->fs_blocksize) {
			fsck_log
			    ("rebuild-sb: wrong block size occured (%lu), fixed (%lu)\n",
			     get_sb_block_size(sb), fs->fs_blocksize);
			set_sb_block_size(sb, fs->fs_blocksize);
		}
	}

	/* if no reiserfs_found or bad data found in that SB, what was checked in previous
	 * clause */
	if (no_reiserfs_found(fs)) {
		int fd;

		fd = open(filename, O_RDWR | O_LARGEFILE);

		if (fd == -1) {
			reiserfs_exit(EXIT_OPER,
				      "rebuils_sb: cannot open device %s",
				      filename);
		}

		version = what_fs_version();

		if (version == 3 || version == 4) {
			retval = 4096;
		} else {
			printf("\nEnter block size [4096]: \n");
			getline(&answer, &n, stdin);
			if (strcmp(answer, "\n")) {
				retval = (int)strtol(answer, &tmp, 0);
				if ((*tmp && strcmp(tmp, "\n")) || retval < 0)
					reiserfs_exit(EXIT_USER,
						      "rebuild_sb: wrong block size specified\n");
				if (!is_blocksize_correct(retval))
					reiserfs_exit(EXIT_USER,
						      "rebuild_sb: wrong block size specified, "
						      "only divisible by 1024 are supported currently\n");
			} else
				retval = 4096;
		}

		if (!(block_count = count_blocks(filename, retval)))
			exit(EXIT_OPER);

		switch (version) {
		case 1:
			fs = reiserfs_create(filename, REISERFS_FORMAT_3_6,
					     block_count, retval, 1, 1,
					     &error);
			break;
		case 2:
			fs = reiserfs_create(filename, REISERFS_FORMAT_3_5,
					     block_count, retval, 1, 1,
					     &error);
			break;
		case 3:
			fs = reiserfs_create(filename, REISERFS_FORMAT_3_6,
					     block_count, retval, 1, 0,
					     &error);
			break;
		case 4:
			fs = reiserfs_create(filename, REISERFS_FORMAT_3_5,
					     block_count, retval, 1, 0,
					     &error);
			break;
		}

		if (fs == NULL) {
			fprintf(stderr, "reiserfs_create failed: %s\n",
				error_message(error));
			return;
		}

		sb = fs->fs_ondisk_sb;
		fs->fs_vp = data;

		if (!fsck_skip_journal(fs)) {
			if (!fsck_data(fs)->journal_dev_name) {
				do {
					printf
					    ("\nNo journal device was specified. (If journal is not "
					     "available, re-run with --no-journal-available option specified).\n"
					     "Is journal default? (y/n)[y]: ");

					getline(&answer, &n, stdin);
				} while (strcmp("y\n", answer) &&
					 strcmp("n\n", answer) &&
					 strcmp("\n", answer));

				if (!strcmp("n\n", answer)) {
					printf
					    ("\nSpecify journal device with -j option.\n");
					exit(EXIT_USER);
				}
				standard_journal = 1;
			} else {
				standard_journal = 0;
				memcpy(fs->fs_ondisk_sb->s_v1.s_magic,
				       REISERFS_JR_SUPER_MAGIC_STRING,
				       strlen(REISERFS_JR_SUPER_MAGIC_STRING));
			}
		}

		do {
			printf("\nDid you use resizer(y/n)[n]: ");
			getline(&answer, &n, stdin);
		} while (strcmp("y\n", answer) && strcmp("n\n", answer)
			 && strcmp("\n", answer));
		if (!strcmp("y\n", answer)) {
			printf("\nEnter partition size [%lu]: ", block_count);
			getline(&answer, &n, stdin);
			if (strcmp("\n", answer))
				block_count = atoi(answer);
			set_sb_block_count(sb, block_count);
		}

		set_sb_fs_state(sb, FS_ERROR);
	}

	if (version == 1 || version == 3) {
		if (get_reiserfs_format(sb) != REISERFS_FORMAT_3_6) {
			fsck_log
			    ("rebuild-sb: wrong reiserfs version occured (%lu), fixed (%lu)\n",
			     get_reiserfs_format(sb), REISERFS_FORMAT_3_6);
			set_sb_version(sb, REISERFS_FORMAT_3_6);
		}
	} else if (version == 2 || version == 4) {
		if (get_reiserfs_format(sb) != REISERFS_FORMAT_3_5) {
			fsck_log
			    ("rebuild-sb: wrong reiserfs version occured (%lu), fixed (%lu)\n",
			     get_reiserfs_format(sb), REISERFS_FORMAT_3_5);
			set_sb_version(sb, REISERFS_FORMAT_3_5);
		}
	}

	p_oid_maxsize = (fs->fs_blocksize - reiserfs_super_block_size(sb)) /
	    sizeof(__u32) / 2 * 2;

	if (get_sb_oid_maxsize(sb) != p_oid_maxsize) {
		fsck_log
		    ("rebuild-sb: wrong objectid map max size occured (%lu), fixed (%lu)\n",
		     get_sb_oid_maxsize(sb), p_oid_maxsize);
		set_sb_oid_maxsize(sb, p_oid_maxsize);
	}

	bmap_nr = reiserfs_fs_bmap_nr(fs);

	if (reiserfs_bmap_over(bmap_nr) && get_sb_bmap_nr(sb) != 0) {
		fsck_log
		    ("rebuild-sb: wrong bitmap number occured (%u), fixed (0) "
		     "(really %u)\n", get_sb_bmap_nr(sb), bmap_nr);
		set_sb_bmap_nr(sb, 0);
	} else if (get_sb_bmap_nr(sb) != bmap_nr) {
		fsck_log("rebuild-sb: wrong bitmap number occured (%u), "
			 "fixed (%u)\n", get_sb_bmap_nr(sb), bmap_nr);
		set_sb_bmap_nr(sb, bmap_nr);
	}

	if (get_sb_root_block(sb) > block_count) {
		fsck_log("rebuild-sb: wrong root block occured (%lu), zeroed\n",
			 get_sb_root_block(sb));
		set_sb_root_block(sb, 0);
	}

	if (get_sb_free_blocks(sb) > block_count) {
		fsck_log
		    ("rebuild-sb: wrong free block count occured (%lu), zeroed\n",
		     get_sb_free_blocks(sb));
		set_sb_free_blocks(sb, 0);
	}

	if (get_sb_umount_state(sb) != FS_CLEANLY_UMOUNTED &&
	    get_sb_umount_state(sb) != FS_NOT_CLEANLY_UMOUNTED) {
		fsck_conditional_log(magic_was_found,
				     "rebuild-sb: wrong umount state (%u), "
				     "fixed to (FS_NOT_CLEANLY_UMOUNTED)\n",
				     get_sb_umount_state(sb));
		set_sb_umount_state(sb, FS_NOT_CLEANLY_UMOUNTED);
	}

	if (get_sb_oid_cursize(sb) == 1 ||
	    get_sb_oid_cursize(sb) > get_sb_oid_maxsize(sb)) {
		fsck_log
		    ("rebuild-sb: wrong objectid map occured (%lu), zeroed\n",
		     get_sb_oid_cursize(sb));

		set_sb_oid_cursize(sb, 0);
	}

	if (get_sb_tree_height(sb) &&
	    ((get_sb_tree_height(sb) < DISK_LEAF_NODE_LEVEL + 1) ||
	     (get_sb_tree_height(sb) > MAX_HEIGHT))) {
		fsck_log("rebuild-sb: wrong tree height occured (%u), zeroed\n",
			 get_sb_tree_height(sb));

		set_sb_tree_height(sb, 0);
	}

	if (get_sb_hash_code(sb) && code2name(get_sb_hash_code(sb)) == NULL) {
		fsck_log("rebuild-sb: wrong hash occured (%lu), zeroed\n",
			 get_sb_hash_code(sb));

		set_sb_hash_code(sb, 0);
	}

	if (version == 1 || version == 3) {
#if defined(HAVE_LIBUUID) && defined(HAVE_UUID_UUID_H)
		if (uuid_is_null(sb->s_uuid)) {
			uuid_generate(sb->s_uuid);
			fsck_log("rebuild-sb: no uuid found, a new uuid was "
				 "generated (%U)\n", sb->s_uuid);
		}
#endif
		if (sb->s_flags != 0 && sb->s_flags != cpu_to_le32(1)) {
			fsck_log
			    ("rebuild-sb: super block flags found (%u), zeroed\n",
			     sb->s_flags);
			sb->s_flags = 0;
		}
	}

	/*
	   if we have a standard journal
	   reserved = 0
	   dev - same
	   size = journal_default_size(fs->fs_super_bh->b_blocknr, fs)
	   offset = journal_default_size(fs)
	   if we have a non standard journal
	   if we found magic string
	   try to find a jhead and comare dev, size, offset there
	   if params are not equal move to "if we did not find a magic string" clause
	   if we did not find a magic string
	   ask user about his journal
	   try to find a jhead and comare dev, size, offset there
	   if params are not equal exit with error
	 */

	p_jp_journal_1st_block = get_journal_start_must(fs);

	if (standard_journal == 1) {
		if (get_jp_journal_dev(sb_jp(sb)) != 0) {
			fsck_conditional_log(magic_was_found,
					     "rebuild-sb: wrong journal device "
					     "occured (%lu), fixed (0)\n",
					     get_jp_journal_dev(sb_jp(sb)));
			set_jp_journal_dev(sb_jp(sb), 0);
		}
		if (get_sb_reserved_for_journal(sb) != 0) {
			fsck_log
			    ("rebuild-sb: wrong size reserved for default journal occured "
			     "(%lu), fixed (0)\n",
			     get_sb_reserved_for_journal(sb));
			set_sb_reserved_for_journal(sb, 0);
		}
		if (get_jp_journal_1st_block(sb_jp(sb)) !=
		    p_jp_journal_1st_block) {
			fsck_conditional_log(magic_was_found,
					     "rebuild-sb: wrong journal first "
					     "block occured (%lu), fixed (%lu)\n",
					     get_jp_journal_1st_block(sb_jp
								      (sb)),
					     p_jp_journal_1st_block);

			set_jp_journal_1st_block(sb_jp(sb),
						 p_jp_journal_1st_block);
		}
		if (get_jp_journal_size(sb_jp(sb)) !=
		    journal_default_size(fs->fs_super_bh->b_blocknr,
					 fs->fs_blocksize)) {
			fsck_conditional_log(magic_was_found,
					     "rebuild-sb: wrong journal size "
					     "occured (%lu), fixed (%lu)\n",
					     get_jp_journal_size(sb_jp(sb)) + 1,
					     journal_default_size(fs->
								  fs_super_bh->
								  b_blocknr,
								  fs->
								  fs_blocksize)
					     + 1);

			set_jp_journal_size(sb_jp(sb),
					    journal_default_size(fs->
								 fs_super_bh->
								 b_blocknr,
								 fs->
								 fs_blocksize));
		}

		if ((retval =
		     reiserfs_open_journal(fs, filename,
					   O_RDWR | O_LARGEFILE))) {
			fsck_log
			    ("\nrebuild-sb: Failed to open the journal device (%s).\n",
			     filename);

			exit(retval < 0 ? EXIT_OPER : EXIT_USER);
		}
	} else if (!fsck_skip_journal(fs)) {
		/* Check that specified non-standard journal device exists. */

		journal_dev_name = fsck_data(fs)->journal_dev_name;
		retval = stat(journal_dev_name, &stat_buf);

		if (retval == -1)
			reiserfs_exit(EXIT_USER,
				      "rebuild_sb: error while detecting the "
				      "specified journal device (%s): %s\n",
				      journal_dev_name, strerror(errno));

		retval = 0;
		if (magic_was_found) {
			/* Super block was found. Try to open the journal on the base of its
			 * journal parameters. */

			retval = reiserfs_open_journal(fs, journal_dev_name,
						       O_RDWR | O_LARGEFILE);

			if (retval == 0) {
				j_head =
				    (struct reiserfs_journal_header *)(fs->
								       fs_jh_bh->
								       b_data);
				retval =
				    memcmp(&j_head->jh_journal, sb_jp(sb),
					   sizeof(struct journal_params));

				if (retval) {
					/* journal parameters from the SB and from the J_Header does not
					 * match. Close the jouranl, ask the user about correct journal
					 * parameters. */
					reiserfs_close_journal(fs);
				}
			}

			if (!reiserfs_journal_opened(fs)) {
				fsck_log
				    ("Journal cannot be opened, assuming specified "
				     "journal device is correct\n");
			}
		}

		if (!reiserfs_journal_opened(fs)) {
			__u64 default_value;

			/* journal was not found or SB and J_Header parameters does not match. */
			if (magic_was_found == 0)
				default_value =
				    (!strcmp
				     (fs->fs_file_name,
				      journal_dev_name)) ?
				    p_jp_journal_1st_block : 0;
			else
				default_value =
				    get_jp_journal_1st_block(sb_jp(sb));

			printf
			    ("\nEnter journal offset on %s in blocks [%Lu]: \n",
			     journal_dev_name,
			     (unsigned long long)default_value);

			getline(&answer, &n, stdin);
			if (strcmp(answer, "\n")) {
				retval = (int)strtol(answer, &tmp, 0);
				if ((*tmp && strcmp(tmp, "\n")) || retval < 0)
					reiserfs_exit(EXIT_USER,
						      "rebuild_sb: wrong offset specified\n");

				set_jp_journal_1st_block(sb_jp(sb), retval);
			} else
				set_jp_journal_1st_block(sb_jp(sb),
							 default_value);

			if (!
			    (p_jp_dev_size =
			     count_blocks(journal_dev_name, fs->fs_blocksize)))
				exit(EXIT_OPER);

			/* some checks for journal offset */
			if (strcmp(fs->fs_file_name, journal_dev_name) != 0) {
				if (p_jp_dev_size <
				    get_jp_journal_1st_block(sb_jp(sb)) + 1)
					reiserfs_exit(EXIT_USER,
						      "rebuild_sb: offset is much than device size\n");
			}

			/* default size if magic was not found is device size - journal_1st_block;
			   default size if magic was found is found value + 1 block for journal
			   header */
			if (magic_was_found == 0)
				default_value =
				    (!strcmp
				     (fs->fs_file_name,
				      journal_dev_name)) ?
				    journal_default_size(fs->fs_super_bh->
							 b_blocknr,
							 fs->fs_blocksize) +
				    1 : p_jp_dev_size -
				    get_jp_journal_1st_block(sb_jp(sb));
			else
				default_value =
				    get_jp_journal_size(sb_jp(sb)) + 1;

			printf
			    ("\nEnter journal size (including 1 block for journal header) on "
			     "%s in blocks [%Lu]: \n", journal_dev_name,
			     (unsigned long long)default_value);

			getline(&answer, &n, stdin);
			if (strcmp(answer, "\n")) {
				retval = (int)strtol(answer, &tmp, 0);
				if ((*tmp && strcmp(tmp, "\n")) || retval < 0)
					reiserfs_exit(EXIT_USER,
						      "rebuild_sb: wrong offset specified\n");

				set_jp_journal_size(sb_jp(sb), retval - 1);
			} else {
				set_jp_journal_size(sb_jp(sb),
						    default_value - 1);
			}

			/* some checks for journal size */
			if (get_jp_journal_size(sb_jp(sb)) +
			    get_jp_journal_1st_block(sb_jp(sb)) + 1 >
			    p_jp_dev_size)
				reiserfs_exit(EXIT_USER,
					      "rebuild_sb: journal offset + journal size is "
					      "greater than device size\n");

			/* some checks for journal size */
			if (get_jp_journal_size(sb_jp(sb)) < JOURNAL_MIN_SIZE)
				reiserfs_exit(EXIT_USER,
					      "rebuild_sb: journal size cannot be less than "
					      "%lu blocks.\n",
					      JOURNAL_MIN_SIZE + 1);

			if ((retval =
			     reiserfs_open_journal(fs, journal_dev_name,
						   O_RDWR | O_LARGEFILE))) {
				fsck_log
				    ("\nrebuild-sb: Failed to open a journal device (%s).",
				     journal_dev_name);

				exit(retval < 0 ? EXIT_OPER : EXIT_USER);
			}

			/* SB was found, but journal params were broken and have been recovered.
			 * Futher work goes as SB would not be found. */
			magic_was_found = 0;
		}

		if (strcmp(fs->fs_file_name, journal_dev_name))
			set_jp_journal_dev(sb_jp(sb), stat_buf.st_rdev);
		else
			set_jp_journal_dev(sb_jp(sb), 0);

	} else {
		fsck_log
		    ("\nJournal was specified as not available. reiserfstune is "
		     "needed.\n\n");

		set_jp_journal_magic(sb_jp(sb), NEED_TUNE);
	}

	if (reiserfs_journal_opened(fs)) {
		/* Journal was openned. Check/fix journal parameters and copy it the journal
		 * header. */

		if (get_jp_journal_max_trans_len(sb_jp(sb)) !=
		    advise_journal_max_trans_len(get_jp_journal_max_trans_len
						 (sb_jp(sb)),
						 get_jp_journal_size(sb_jp(sb)),
						 fs->fs_blocksize, 0)) {
			fsck_conditional_log(magic_was_found,
					     "rebuild-sb: wrong journal max "
					     "transaction length occured (%lu), fixed (%d)\n",
					     get_jp_journal_max_trans_len(sb_jp
									  (sb)),
					     advise_journal_max_trans_len
					     (get_jp_journal_max_trans_len
					      (sb_jp(sb)),
					      get_jp_journal_size(sb_jp(sb)),
					      fs->fs_blocksize, 0));

			set_jp_journal_max_trans_len(sb_jp(sb),
						     advise_journal_max_trans_len
						     (get_jp_journal_max_trans_len
						      (sb_jp(sb)),
						      get_jp_journal_size(sb_jp
									  (sb)),
						      fs->fs_blocksize, 0));
		}

		if (get_jp_journal_max_batch(sb_jp(sb)) !=
		    advise_journal_max_batch(get_jp_journal_max_trans_len
					     (sb_jp(sb)))) {
			fsck_conditional_log(magic_was_found,
					     "rebuild-sb: wrong journal max "
					     "batch size occured (%lu), fixed (%d)\n",
					     get_jp_journal_max_batch(sb_jp
								      (sb)),
					     advise_journal_max_batch
					     (get_jp_journal_max_trans_len
					      (sb_jp(sb))));

			set_jp_journal_max_batch(sb_jp(sb),
						 advise_journal_max_batch
						 (get_jp_journal_max_trans_len
						  (sb_jp(sb))));
		}

		if (get_jp_journal_max_commit_age(sb_jp(sb)) !=
		    advise_journal_max_commit_age()) {
			fsck_conditional_log(magic_was_found,
					     "rebuild-sb: wrong journal "
					     "max commit age occured (%lu), fixed (%d)\n",
					     get_jp_journal_max_commit_age(sb_jp
									   (sb)),
					     advise_journal_max_commit_age());

			set_jp_journal_max_commit_age(sb_jp(sb),
						      advise_journal_max_commit_age
						      ());
		}

		if (get_jp_journal_max_trans_age(sb_jp(sb)) !=
		    advise_journal_max_trans_age()) {
			fsck_log
			    ("rebuild-sb: wrong journal  max commit age occured (%u), "
			     "fixed (%u)\n",
			     get_jp_journal_max_trans_age(sb_jp(sb)),
			     advise_journal_max_trans_age());

			set_jp_journal_max_trans_age(sb_jp(sb),
						     advise_journal_max_trans_age
						     ());
		}

		j_head =
		    (struct reiserfs_journal_header *)(fs->fs_jh_bh->b_data);

		if (standard_journal == 0) {
			if (get_jp_journal_magic(sb_jp(sb)) == 0 ||
			    get_jp_journal_magic(sb_jp(sb)) == NEED_TUNE) {
				int magic;

				magic = random();
				fsck_log
				    ("rebuild-sb: genarate the new journal magic (%d)\n",
				     magic);
				set_jp_journal_magic(sb_jp(sb), magic);
				set_jp_journal_magic(&j_head->jh_journal,
						     magic);

			}
		}

		retval =
		    memcmp(&j_head->jh_journal, sb_jp(sb),
			   sizeof(struct journal_params));

		if (retval) {
			if (standard_journal == 1) {
				fsck_log
				    ("\nrebuild-sb: You either have a corrupted journal or have just "
				     "changed\nthe start of the partition with some partition table editor. "
				     "If you are\nsure that the start of the partition is ok, rebuild the "
				     "journal header.\n");
			} else if (!magic_was_found) {
				fsck_log
				    ("\nrebuild-sb: journal header is not found. You either have "
				     "a corrupted,\nbad configured(device/offset/size), not available "
				     "journal or have just changed\nthe start of the journal partition "
				     "with some partition table editor. In the \ncase of corrupted "
				     "journal you need to use --no-journal-available. If you are\n"
				     "sure that the start of the partition is ok and journal is "
				     "available, rebuild\nthe journal header.\n");
			}

			if (standard_journal || !magic_was_found) {
				if (!user_confirmed
				    (stdout,
				     "Do you want to rebuild the journal header? "
				     "(y/n)[n]: ", "y\n")) {
					exit(EXIT_USER);
				} else
					exit_code = EXIT_FIXED;
			}

			memcpy(&j_head->jh_journal, sb_jp(sb),
			       sizeof(struct journal_params));
			mark_buffer_dirty(fs->fs_jh_bh);
			bwrite(fs->fs_jh_bh);
		}
	}

	/*  whether journal header contains params with the same dev, offset, size will be
	   checked in open_journal */

	if (version == 1 || version == 3)
		sb_size = SB_SIZE;
	else
		sb_size = SB_SIZE_V1;

	if (ondisk_sb == NULL ||
	    memcmp(ondisk_sb, sb, sb_size - ((sb_size == SB_SIZE) ?
					     sizeof(fs->fs_ondisk_sb->
						    s_unused) : 0))) {
		/* smth was changed in SB or a new one has been built */
		set_sb_fs_state(sb, get_sb_fs_state(sb) | FS_ERROR);

		if (ondisk_sb) {
			/* if super_block was found, we keep sb in ondisk_sb */
			fs->fs_ondisk_sb = ondisk_sb;
			memcpy(ondisk_sb, sb, sb_size);
			freemem(sb);
		}

		fflush(stdout);
		print_block(stderr, fs, fs->fs_super_bh);

		if (user_confirmed(stderr, "Is this ok ? (y/n)[n]: ", "y\n")) {
			mark_buffer_uptodate(fs->fs_super_bh, 1);
			mark_buffer_dirty(fs->fs_super_bh);
			bwrite(fs->fs_super_bh);
			fsck_progress
			    ("The fs may still be unconsistent. Run reiserfsck --check.\n\n");
			exit_code = EXIT_FIXED;
		} else {
			mark_buffer_clean(fs->fs_super_bh);
			fsck_progress("Super block was not written\n");
		}
	} else {
		print_block(stderr, fs, fs->fs_super_bh);

		mark_buffer_clean(fs->fs_super_bh);
		fsck_progress("\nSuper block seems to be correct\n\n");
	}

	exit(exit_code);
}

/*	if (version == 0) {
	    brelse (fs->fs_super_bh);
            freemem (fs);
            close (fs->fs_dev);
            fs = NULL;
	}
*/
