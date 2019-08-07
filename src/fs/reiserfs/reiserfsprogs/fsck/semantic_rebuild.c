/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

int screen_width;
static int screen_curr_pos;
char *screen_savebuffer;
int screen_savebuffer_len;

#define MIN(a, b) (((a)>(b))?(b):(a))

/* semantic pass progress */
void print_name(char *name, int len)
{
	int i;

	if (fsck_quiet(fs))
		return;

	if (len + screen_curr_pos + 1 > screen_savebuffer_len) {
		char *t;

		t = expandmem(screen_savebuffer, screen_savebuffer_len + 1,
			      len + screen_curr_pos - screen_savebuffer_len +
			      1);
		if (!t) {
			fsck_progress("\nOut of memory\n");
			return;	// ????
		}
		screen_savebuffer = t;
		screen_savebuffer_len = len + screen_curr_pos + 1;
	}
	strcat(screen_savebuffer, "/");
	strncat(screen_savebuffer, name, len);
	i = screen_curr_pos;
	screen_curr_pos += len + 1;
	for (; i < screen_curr_pos; i++)
		if (screen_savebuffer[i] < 32)
			screen_savebuffer[i] = '?';
	screen_savebuffer[screen_curr_pos] = 0;

	if (screen_width < screen_curr_pos) {
		printf("\r... %.*s", screen_width - 4,
		       screen_savebuffer + (screen_curr_pos -
					    (screen_width - 4)));
	} else
		printf("/%.*s", len, screen_savebuffer + screen_curr_pos - len);

	fflush(stdout);
}

void erase_name(int len)
{
	int i;

	if (fsck_quiet(fs))
		return;

	if (screen_curr_pos < screen_width) {

		screen_curr_pos -= len + 1;
		screen_savebuffer[screen_curr_pos] = 0;

		for (i = 0; i <= len; i++)
			printf("\b");
		for (i = 0; i <= len + 1; i++)
			printf(" ");
		for (i = 0; i <= len + 1; i++)
			printf("\b");
	} else {
		screen_curr_pos -= len + 1;
		if (screen_curr_pos < 0)
			die("%s: Get out of buffer's data!\n", __FUNCTION__);

		screen_savebuffer[screen_curr_pos] = 0;
		printf("\r");
		if (screen_curr_pos >= screen_width) {
			int t_preface = MIN(screen_curr_pos - screen_width, 4);
			printf("%.*s%.*s", t_preface, "... ",
			       screen_width - t_preface - 1,
			       screen_savebuffer + (screen_curr_pos -
						    (screen_width -
						     t_preface)) + 1);
		} else {
			int space_used = printf("%s", screen_savebuffer);
			for (i = space_used; i < screen_width; i++)
				printf(" ");
			for (i = space_used; i < screen_width; i++)
				printf("\b");
		}
	}

	fflush(stdout);
}

/* *size is "real" file size, sd_size - size from stat data */
int wrong_st_size(const struct reiserfs_key *key,
		  unsigned long long max_file_size,
		  int blocksize, __u64 * size, __u64 sd_size, int type)
{
	if (sd_size <= max_file_size) {
		if (sd_size == *size)
			return 0;

		if (type == TYPE_DIRENTRY) {
			/* directory size must match to the sum of length of its entries */
			fsck_log
			    ("vpf-10650: The directory %K has the wrong size in the StatData "
			     "(%Lu)%s(%Lu)\n", key, sd_size,
			     fsck_mode(fs) ==
			     FSCK_CHECK ? ", should be " : " - corrected to ",
			     *size);
			return 1;
		}

		if (sd_size > *size) {
			/* size in stat data can be bigger than size calculated by items */
			if (fsck_adjust_file_size(fs) || type == TYPE_SYMLINK) {
				/* but it -o is given - fix that */
				fsck_log
				    ("vpf-10660: The file %K has too big size in the StatData "
				     "(%Lu)%s(%Lu)\n", key, sd_size,
				     fsck_mode(fs) ==
				     FSCK_CHECK ? ", should be " :
				     " - corrected to ", *size);
				sem_pass_stat(fs)->fixed_sizes++;
				return 1;
			}
			*size = sd_size;
			return 0;
		}

		if (!(*size % blocksize)) {
			/* last item is indirect */
			if (((sd_size & ~(blocksize - 1)) ==
			     (*size - blocksize)) && sd_size % blocksize) {
				/* size in stat data is correct */
				*size = sd_size;
				return 0;
			}
		} else {
			/* last item is a direct one */
			if (!(*size % 8)) {
				if (((sd_size & ~7) == (*size - 8))
				    && sd_size % 8) {
					/* size in stat data is correct */
					*size = sd_size;
					return 0;
				}
			}
		}
	}

	fsck_log
	    ("vpf-10670: The file %K has the wrong size in the StatData (%Lu)%s(%Lu)\n",
	     key, sd_size,
	     fsck_mode(fs) == FSCK_CHECK ? ", should be " : " - corrected to ",
	     *size);
	sem_pass_stat(fs)->fixed_sizes++;
	return 1;
}

/* sd_blocks is 32 bit only */
/* old stat data shares sd_block and sd_dev - do not wipe sd_rdev out */
/* we should fix it as following:
|------------------------------------------------------------------|
|                    |        3.6                   |       3.5    |
|---------------------------------------------------|              |
|                    |  blocks |  r_dev|generation  | blocks/r_dev |
|------------------------------------------------------------------|
|   fifo, sockets    |    0    |    generation      |      0       |
|   chr/blk_dev      |    0    |     maj:min        |   maj:min    |
|   file, dir, link  |  blocks |    generation      |   blocks     |
|------------------------------------------------------------------|
*/
int wrong_st_blocks(const struct reiserfs_key *key, __u32 * blocks,
		    __u32 sd_blocks, __u16 mode, int new_format)
{
	int ret = 0;

	if (S_ISREG(mode) || S_ISLNK(mode) || S_ISDIR(mode)) {
		if ((!S_ISLNK(mode) && *blocks != sd_blocks) ||
		    (S_ISLNK(mode) && *blocks != sd_blocks
		     && (ROUND_UP(*blocks) != sd_blocks))) {
			fsck_log
			    ("vpf-10680: The %s %K has the wrong block count in the StatData "
			     "(%u)%s(%u)\n",
			     S_ISDIR(mode) ? "directory" : S_ISREG(mode) ?
			     "file" : "link", key, sd_blocks,
			     fsck_mode(fs) ==
			     FSCK_CHECK ? ", should be " : " - corrected to ",
			     *blocks);
			ret = 1;
		}
	} else if (new_format || (S_ISFIFO(mode) || S_ISSOCK(mode))) {
		if (sd_blocks != 0) {
			fsck_log
			    ("vpf-10690: The object %K has the wrong block count in the StatData "
			     "(%u)%s(%u)\n", key, sd_blocks,
			     fsck_mode(fs) ==
			     FSCK_CHECK ? ", should be " : " - corrected to ",
			     0);
			*blocks = 0;
			ret = 1;
		}
	}

	return ret;
}

/*
int wrong_st_rdev (struct reiserfs_key *key, __u32 * sd_rdev, __u16 mode, int new_format)
{
    int ret = 0;

    if (!new_format)
	return 0;

    if (!S_ISCHR (mode) && !S_ISBLK (mode)) {
	if (*sd_rdev != 0) {
	    fsck_log ("%s %K has wrong sd_rdev %u, has to be 0\n",
		S_ISDIR (mode) ? "dir" : "file", key, *sd_rdev);
	    *sd_rdev = 0;
	    ret = 1;
	}
    }
    return ret;
}
*/
/* only regular files and symlinks may have items but stat
   data. Symlink should have body */
int wrong_mode(const struct reiserfs_key *key, __u16 * mode, __u64 real_size,
	       int symlink)
{
	int retval = 0;
	if (S_ISLNK(*mode) && !symlink) {
		fsck_log
		    ("The file %K (%M) is too big to be the symlink%s regfile\n",
		     key, *mode,
		     fsck_mode(fs) ==
		     FSCK_CHECK ? ", should be the" : " - corrected " "to the");
		*mode &= ~S_IFMT;
		*mode |= S_IFREG;
		retval = 1;
	}

	if (ftypelet(*mode) != '?') {
		/* mode looks reasonable */
		if (S_ISREG(*mode) || S_ISLNK(*mode))
			return retval;

		/* device, pipe, socket have no items */
		if (!real_size)
			return retval;
	}
	/* there are items, so change file mode to regular file. Otherwise
	   - file bodies do not get deleted */
	if (fsck_mode(fs) == FSCK_CHECK) {
		fsck_log("The object %K has wrong mode (%M)\n", key, *mode);
	} else {
		fsck_log
		    ("The object %K has wrong mode (%M) - corrected to %M\n",
		     key, *mode, (S_IFREG | 0600));
	}
	*mode = (S_IFREG | 0600);
	return 1;
}

/* key is a key of last file item */
int wrong_first_direct_byte(const struct reiserfs_key *key, int blocksize,
			    __u32 * first_direct_byte,
			    __u32 sd_first_direct_byte, __u32 size)
{
	if (!size || is_indirect_key(key)) {
		/* there is no direct item */
		*first_direct_byte = NO_BYTES_IN_DIRECT_ITEM;
		if (sd_first_direct_byte != NO_BYTES_IN_DIRECT_ITEM) {
			if (fsck_mode(fs) == FSCK_CHECK) {
				fsck_log
				    ("The file %K: The wrong info about the tail in the StatData, "
				     "first_direct_byte (%d) - should be (%d)\n",
				     key, sd_first_direct_byte,
				     *first_direct_byte);
			} else {
				fsck_log
				    ("The file %K: The wrong info about the tail in the StatData, "
				     "first_direct_byte (%d) - corrected to (%d)\n",
				     key, sd_first_direct_byte,
				     *first_direct_byte);
			}
			return 1;
		}
		return 0;
	}

	/* there is direct item */
	*first_direct_byte = (get_offset(key) & ~(blocksize - 1)) + 1;
	if (*first_direct_byte != sd_first_direct_byte) {
		if (fsck_mode(fs) == FSCK_CHECK) {
			fsck_log
			    ("The file %K: The wrong info about the tail in the StatData, "
			     "first_direct_byte (%d) - should be (%d)\n", key,
			     sd_first_direct_byte, *first_direct_byte);
		} else {
			fsck_log
			    ("The file %K: The wrong info about the tail in the StatData, "
			     "first_direct_byte (%d) - corrected to (%d)\n",
			     key, sd_first_direct_byte, *first_direct_byte);
		}

		return 1;
	}
	return 0;
}

/* delete all items (but directory ones) with the same key 'ih' has
   (including stat data of not a directory) and put them back at the
   other place */
void relocate_dir(struct item_head *ih, int change_ih)
{
	struct reiserfs_key key;
	struct reiserfs_key *rkey;
	struct reiserfs_path path;
	struct item_head *path_ih;
	struct si *si;
	__u32 new_objectid;

	/* starting with the leftmost one - look for all items of file,
	   store them and delete */
	key = ih->ih_key;
	set_type_and_offset(KEY_FORMAT_1, &key, SD_OFFSET, TYPE_STAT_DATA);

	si = NULL;
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
			/* there is no more items of a directory */
			pathrelse(&path);
			break;
		}

		if (is_stat_data_ih(tp_item_head(&path)))
			fix_obviously_wrong_sd_mode(&path);

		path_ih = tp_item_head(&path);
		if (not_of_one_file(&key, &(path_ih->ih_key))) {
			/* there are no more item with this key */
			pathrelse(&path);
			break;
		}

		/* ok, item found, but make sure that it is not a directory one */
		if ((is_stat_data_ih(path_ih)
		     && not_a_directory(tp_item_body(&path)))
		    || is_direct_ih(path_ih) || is_indirect_ih(path_ih)) {
			/* item of not a directory found. Leave it in the
			   tree. FIXME: should not happen */
			key = path_ih->ih_key;
			set_offset(KEY_FORMAT_1, &key, get_offset(&key) + 1);
			pathrelse(&path);
			continue;
		}

		/* directory stat data ro directory item */
		si = save_and_delete_file_item(si, &path);
	}

	if (!si) {
		fsck_log
		    ("relocate_dir: WARNING: No one item of the directory %K found\n",
		     &key);
		return;
	}

	/* get new objectid for relocation or get objectid with which file
	   was relocated already */
	new_objectid = objectid_for_relocation(&ih->ih_key);
	set_key_objectid(&ih->ih_key, new_objectid);

	/* put all items removed back into tree */
	while (si) {
		fsck_log("relocate_dir: Moving %k to ", &si->si_ih.ih_key);
		set_key_objectid(&si->si_ih.ih_key, new_objectid);
		fsck_log("%k\n", &si->si_ih.ih_key);
		if (get_offset(&(si->si_ih.ih_key)) == DOT_OFFSET) {
			/* fix "." entry to point to a directtory properly */
			struct reiserfs_de_head *deh;

			deh = (struct reiserfs_de_head *)si->si_dnm_data;
			set_deh_objectid(deh, new_objectid);
		}
		insert_item_separately(&(si->si_ih), si->si_dnm_data,
				       1 /*was in tree */ );
		si = remove_saved_item(si);
	}
}

/* path is path to stat data. If file will be relocated - new_ih will contain
   a key file was relocated with */
int rebuild_check_regular_file(struct reiserfs_path *path, void *sd,
			       struct item_head *new_ih)
{
	int is_new_file;
//    struct reiserfs_key sd_key;
	__u16 mode;
	__u32 nlink;
	__u64 real_size, saved_size;
	__u32 blocks, saved_blocks;	/* proper values and value in stat data */
	__u32 first_direct_byte, saved_first_direct_byte;

	struct buffer_head *bh;
	struct item_head *ih, sd_ih;
	int fix_sd;
	int symlnk = 0;
	int retval;

	retval = OK;

	/* stat data of a file */
	ih = tp_item_head(path);
	bh = get_bh(path);

	if (new_ih) {
		/* this objectid is used already */
		*new_ih = *ih;
		pathrelse(path);
		rewrite_file(new_ih, 1, 1);
		linked_already(&new_ih->ih_key);
		sem_pass_stat(fs)->oid_sharing_files_relocated++;
		retval = RELOCATED;
		if (reiserfs_search_by_key_4(fs, &(new_ih->ih_key), path) ==
		    ITEM_NOT_FOUND)
			reiserfs_panic
			    ("%s: Could not find the StatData of the relocated file %k",
			     __FUNCTION__, &(new_ih->ih_key));
		/* stat data is marked unreachable again due to relocation, fix that */
		ih = tp_item_head(path);
		bh = get_bh(path);
		mark_item_reachable(ih, bh);
		sd = tp_item_body(path);

	}

	id_map_mark(semantic_id_map(fs), get_key_objectid(&ih->ih_key));

	/* check and set nlink first */
	get_sd_nlink(ih, sd, &nlink);
	nlink++;
	set_sd_nlink(ih, sd, &nlink);
	mark_buffer_dirty(bh);

	if (nlink > 1)
		return retval;

	/* firts name of a file found */
	if (get_ih_item_len(ih) == SD_SIZE)
		is_new_file = 1;
	else
		is_new_file = 0;

	get_sd_mode(ih, sd, &mode);
	get_sd_size(ih, sd, &saved_size);
	get_sd_blocks(ih, sd, &saved_blocks);
	if (!is_new_file)
		get_sd_first_direct_byte(ih, sd, &saved_first_direct_byte);

	/* we met this file first time */
	if (S_ISREG(mode)) {
		sem_pass_stat(fs)->regular_files++;
	} else if (S_ISLNK(mode)) {
		symlnk = 1;
		sem_pass_stat(fs)->symlinks++;
	} else {
		sem_pass_stat(fs)->others++;
	}

	sd_ih = *ih;
//    sd_key = sd_ih.ih_key;
	pathrelse(path);

	if (are_file_items_correct
	    (&sd_ih, sd, &real_size, &blocks, 1 /*mark items reachable */ ,
	     &symlnk) != 1) {
		/* unpassed items will be deleted in pass 4 as they left unaccessed */
		sem_pass_stat(fs)->broken_files++;
	}

	fix_sd = 0;

	fix_sd +=
	    wrong_mode( /*&sd_key, */ &sd_ih.ih_key, &mode, real_size, symlnk);

	if (!is_new_file)
		fix_sd +=
		    wrong_first_direct_byte(&sd_ih.ih_key, fs->fs_blocksize,
					    &first_direct_byte,
					    saved_first_direct_byte, real_size);

	fix_sd += wrong_st_size( /*&sd_key, */ &sd_ih.ih_key,
				is_new_file ? MAX_FILE_SIZE_V2 :
				MAX_FILE_SIZE_V1, fs->fs_blocksize, &real_size,
				saved_size, symlnk ? TYPE_SYMLINK : 0);

	fix_sd +=
	    wrong_st_blocks(&sd_ih.ih_key, &blocks, saved_blocks, mode,
			    is_new_file);

	if (fix_sd) {
		/* find stat data and correct it */
		set_type_and_offset(KEY_FORMAT_1, &sd_ih.ih_key, SD_OFFSET,
				    TYPE_STAT_DATA);
		if (reiserfs_search_by_key_4(fs, &sd_ih.ih_key, path) !=
		    ITEM_FOUND)
			reiserfs_panic
			    ("%s: The StatData of the file %k could not be found",
			     __FUNCTION__, &sd_ih.ih_key);

		bh = get_bh(path);
		ih = tp_item_head(path);
		sd = tp_item_body(path);
		set_sd_size(ih, sd, &real_size);
		set_sd_blocks(ih, sd, &blocks);
		set_sd_mode(ih, sd, &mode);
		if (!is_new_file)
			set_sd_first_direct_byte(ih, sd, &first_direct_byte);
		mark_buffer_dirty(bh);
	}

	return retval;
}

static int is_rootdir_key(const struct reiserfs_key *key)
{
	if (comp_keys(key, &root_dir_key))
		return 0;
	return 1;
}

/* returns buffer, containing found directory item.*/
static char *get_next_directory_item(struct reiserfs_key *key,	/* on return this will
								   contain key of next item
								   in the tree */
				     const struct reiserfs_key *parent,
				     struct item_head *ih,	/*not in tree */
				     __u32 * pos_in_item)
{
	INITIALIZE_REISERFS_PATH(path);
	char *dir_item;
	struct reiserfs_key *rdkey;
	struct buffer_head *bh;
	struct reiserfs_de_head *deh;
	int i;
	int retval;

	if ((retval =
	     reiserfs_search_by_entry_key(fs, key, &path)) != POSITION_FOUND)
		reiserfs_panic
		    ("get_next_directory_item: The current directory %k cannot be "
		     "found", key);

	/* leaf containing directory item */
	bh = PATH_PLAST_BUFFER(&path);
	*pos_in_item = path.pos_in_item;
	*ih = *tp_item_head(&path);
	deh = B_I_DEH(bh, ih);

	/* make sure, that ".." exists as well */
	if (get_offset(key) == DOT_OFFSET) {
		if (get_ih_entry_count(ih) < 2 ||
		    name_in_entry_length(ih, deh + 1, 1) != 2 ||
		    strncmp(name_in_entry(deh + 1, 1), "..", 2)) {
			fsck_log
			    ("get_next_directory_item: The entry \"..\" cannot be found in %k\n",
			     &ih->ih_key);
			pathrelse(&path);
			return NULL;
		}
	}

	/* mark hidden entries as visible, set "." and ".." correctly */
	deh += *pos_in_item;
	for (i = *pos_in_item; i < get_ih_entry_count(ih); i++, deh++) {
		int namelen;
		char *name;

		name = name_in_entry(deh, i);
		namelen = name_in_entry_length(ih, deh, i);
		if (de_hidden(deh))
			reiserfs_panic
			    ("get_next_directory_item: item %k: hidden entry %d \'%.*s\'\n",
			     key, i, namelen, name);

		if (get_deh_offset(deh) == DOT_OFFSET) {
			if (not_of_one_file(&(deh->deh2_dir_id), key)) {
				/* "." must point to the directory it is in */

				//deh->deh_objectid != REISERFS_ROOT_PARENT_OBJECTID)/*????*/ {
				fsck_log
				    ("get_next_directory_item: The entry \".\" of the directory %K pointes to %K, instead of %K",
				     key,
				     (struct reiserfs_key
				      *)(&(deh->deh2_dir_id)), key);
				set_deh_dirid(deh, get_key_dirid(key));
				set_deh_objectid(deh, get_key_objectid(key));
				mark_buffer_dirty(bh);
				fsck_log(" - corrected\n");
			}
		}

		if (get_deh_offset(deh) == DOT_DOT_OFFSET) {
			/* set ".." so that it points to the correct parent directory */
			if (comp_short_keys(&(deh->deh2_dir_id), parent)) {
				fsck_log
				    ("get_next_directory_item: The entry \"..\" of the directory %K pointes to %K, instead of %K",
				     key,
				     (struct reiserfs_key
				      *)(&(deh->deh2_dir_id)), parent);
				set_deh_dirid(deh, get_key_dirid(parent));
				set_deh_objectid(deh, get_key_objectid(parent));
				mark_buffer_dirty(bh);
				fsck_log(" - corrected\n");
			}
		}
	}

	/* copy directory item to the temporary buffer */
	dir_item = getmem(get_ih_item_len(ih));
	memcpy(dir_item, ih_item_body(bh, ih), get_ih_item_len(ih));

	/* next item key */
	if (PATH_LAST_POSITION(&path) == (B_NR_ITEMS(bh) - 1) &&
	    (rdkey = uget_rkey(&path)))
		copy_key(key, rdkey);
	else {
		set_key_dirid(key, 0);
		set_key_objectid(key, 0);
	}

	if (fsck_mode(fs) == FSCK_REBUILD)
		mark_item_reachable(tp_item_head(&path), bh);
	pathrelse(&path);

	return dir_item;
}

// get key of an object pointed by direntry and the key of the entry itself
void get_object_key(struct reiserfs_de_head *deh, struct reiserfs_key *key,
		    struct reiserfs_key *entry_key, struct item_head *ih)
{
	/* entry points to this key */
	set_key_dirid(key, get_deh_dirid(deh));
	set_key_objectid(key, get_deh_objectid(deh));
	set_key_offset_v1(key, SD_OFFSET);
	set_key_uniqueness(key, 0);

	/* key of entry */
	set_key_dirid(entry_key, get_key_dirid(&ih->ih_key));
	set_key_objectid(entry_key, get_key_objectid(&ih->ih_key));
	set_key_offset_v1(entry_key, get_deh_offset(deh));
	set_key_uniqueness(entry_key, DIRENTRY_UNIQUENESS);
}

int fix_obviously_wrong_sd_mode(struct reiserfs_path *path)
{
	struct reiserfs_key *next_key = NULL;
	__u16 mode;
	int retval = 0;

	next_key = reiserfs_next_key(path);

	if (!next_key || not_of_one_file(next_key, &tp_item_head(path)->ih_key))
		return 0;

	/* next item exists and of the same file. Fix the SD mode */

	if (not_a_directory(tp_item_body(path)) && is_direntry_key(next_key)) {
		/* make SD mode SD of dir */
		get_sd_mode(tp_item_head(path), tp_item_body(path), &mode);
		fsck_log("The directory %K had wrong mode %M",
			 &tp_item_head(path)->ih_key, mode);

		if (fsck_mode(fs) != FSCK_CHECK) {
			mode &= ~S_IFMT;
			mode |= S_IFDIR;
			fsck_log(" - corrected to %M\n", mode);
			set_sd_mode(tp_item_head(path), tp_item_body(path), &mode);
			mark_buffer_dirty(get_bh(path));
		} else {
			fsck_log("\n");
			retval = 1;
		}
	} else if (!not_a_directory(tp_item_body(path))
		   && !is_direntry_key(next_key)) {
		/* make SD mode SD of regular file */
		get_sd_mode(tp_item_head(path), tp_item_body(path), &mode);
		fsck_log("The file %K had wrong mode %M", &tp_item_head(path)->ih_key,
			 mode);
		if (fsck_mode(fs) != FSCK_CHECK) {
			mode &= ~S_IFMT;
			mode |= S_IFREG;
			fsck_log(" - corrected to %M\n", mode);
			set_sd_mode(tp_item_head(path), tp_item_body(path), &mode);
			mark_buffer_dirty(get_bh(path));
		} else {
			fsck_log("\n");
			retval = 1;
		}

	}

	return retval;
}

/* check recursively the semantic tree. Returns OK if entry points to good
   object, STAT_DATA_NOT_FOUND if stat data was not found or RELOCATED when
   file was relocated because its objectid was already marked as used by
   another file */
int rebuild_semantic_pass(struct reiserfs_key *key,
			  const struct reiserfs_key *parent,
			  int dot_dot, struct item_head *new_ih)
{
	struct reiserfs_path path;
	void *sd;
	int is_new_dir;
	__u32 nlink;
	struct buffer_head *bh;
	struct item_head *ih;
	int retval, retval1;
	char *dir_item;
	__u32 pos_in_item;
	struct item_head tmp_ih;
	struct reiserfs_key item_key, entry_key, object_key;
	__u64 dir_size;
	__u32 blocks;
	__u64 saved_size;
	__u32 saved_blocks;
	int fix_sd;
	int relocate;
	int dir_format = 0;
	__u16 mode;

	retval = OK;

start_again:			/* when directory was relocated */

	if (!KEY_IS_STAT_DATA_KEY(key))
		reiserfs_panic
		    ("rebuild_semantic_pass: The key %k must be key of a StatData",
		     key);

	/* look for stat data of an object */
	if (reiserfs_search_by_key_4(fs, key, &path) == ITEM_NOT_FOUND) {
		pathrelse(&path);
		if (is_rootdir_key(key))
			/* root directory has to exist at this point */
			reiserfs_panic
			    ("rebuild_semantic_pass: The root directory StatData was not found");

		return STAT_DATA_NOT_FOUND;
	}

	/* stat data has been found */
	bh = get_bh(&path);
	ih = tp_item_head(&path);
	sd = tp_item_body(&path);

	/* */
	get_sd_nlink(ih, sd, &nlink);

	relocate = 0;
	if (!nlink) {
		/* we reached the stat data for the first time */
		if (id_map_mark
		    (semantic_id_map(fs), get_key_objectid(&ih->ih_key))) {
			/* calculate number of found files/dirs who are using objectid
			   which is used by another file */
			sem_pass_stat(fs)->oid_sharing++;
			if (1 /*fsck_adjust_file_size (fs) */ )
				/* this works for files only */
				relocate = 1;
		}

		mark_item_reachable(ih, bh);
	}

	fix_obviously_wrong_sd_mode(&path);

	if (not_a_directory(sd)) {
		retval =
		    rebuild_check_regular_file(&path, sd,
					       relocate ? new_ih : NULL);
		pathrelse(&path);
		return retval;
	}

	if (relocate) {
		if (!new_ih)
			reiserfs_panic
			    ("rebuild_semantic_pass: Memory is not prepared for relocation of %K",
			     &ih->ih_key);
		*new_ih = *ih;
		pathrelse(&path);
		sem_pass_stat(fs)->oid_sharing_dirs_relocated++;
		relocate_dir(new_ih, 1);
		linked_already(&new_ih->ih_key);
		*key = new_ih->ih_key;
		retval = RELOCATED;

		goto start_again;
	}

	/* it looks like stat data of a directory found */
	if (nlink) {
		/* we saw this directory already */
		if (!dot_dot) {
			/* this name is not ".."  - and hard links are not allowed on
			   directories */
			pathrelse(&path);
			return STAT_DATA_NOT_FOUND;
		} else {
			/* ".." found */
			nlink++;
			set_sd_nlink(ih, sd, &nlink);
			mark_buffer_dirty(bh);
			pathrelse(&path);
			return OK;
		}
	}

	/* we see the directory first time */
	sem_pass_stat(fs)->directories++;
	nlink = 2;
	if (get_key_objectid(key) == REISERFS_ROOT_OBJECTID)
		nlink++;
	set_sd_nlink(ih, sd, &nlink);
	mark_buffer_dirty(bh);

	if (get_ih_item_len(ih) == SD_SIZE)
		is_new_dir = 1;
	else
		is_new_dir = 0;

/*
    {
	struct reiserfs_key *tmp;

	// check next item: if it is not a directory item of the same file
	// therefore sd_mode is corrupted so we just reset it to regular file
	// mode
	tmp = reiserfs_next_key (&path);
	if (tmp && !not_of_one_file (tmp, key) && !is_direntry_key (tmp)) {
	    __u16 mode;

	    get_sd_mode (ih, sd, &mode);
	    fsck_log ("file %K had broken mode %M, ", key, mode);
	    mode &= ~S_IFMT;
	    mode |= S_IFREG;
	    fsck_log ("fixed to %M\n", mode);
	    set_sd_mode (ih, sd, &mode);

	    nlink = 0;
	    set_sd_nlink (ih, sd, &nlink);
	
	    retval = rebuild_check_regular_file (&path, sd, 0); //no relocate
	    pathrelse (&path);
	    return retval;
	}
    }
*/

	dir_format =
	    (get_ih_item_len(tp_item_head(&path)) ==
	     SD_SIZE) ? KEY_FORMAT_2 : KEY_FORMAT_1;
	/* save stat data's size and st_blocks */
	get_sd_size(ih, sd, &saved_size);
	get_sd_blocks(ih, sd, &saved_blocks);
	get_sd_mode(ih, sd, &mode);

	/* release path pointing to stat data */
	pathrelse(&path);

	/* make sure that "." and ".." exist */
	reiserfs_add_entry(fs, key, ".", name_length(".", dir_format), key,
			   1 << IH_Unreachable);
	reiserfs_add_entry(fs, key, "..", name_length("..", dir_format), parent,
			   1 << IH_Unreachable);

	set_key_dirid(&item_key, get_key_dirid(key));
	set_key_objectid(&item_key, get_key_objectid(key));
	set_key_offset_v1(&item_key, DOT_OFFSET);
	set_key_uniqueness(&item_key, DIRENTRY_UNIQUENESS);

	dir_size = 0;
	while ((dir_item =
		get_next_directory_item(&item_key, parent, &tmp_ih,
					&pos_in_item)) != NULL) {
		/* dir_item is copy of the item in separately allocated memory,
		   item_key is a key of next item in the tree */
		int i;
		char *name = NULL;
		int namelen, entry_len;
		struct reiserfs_de_head *deh =
		    (struct reiserfs_de_head *)dir_item + pos_in_item;

		for (i = pos_in_item; i < get_ih_entry_count(&tmp_ih);
		     i++, deh++) {
			struct item_head relocated_ih;

			if (name) {
				free(name);
				name = NULL;
			}

			namelen = name_in_entry_length(&tmp_ih, deh, i);
			asprintf(&name, "%.*s", namelen, name_in_entry(deh, i));

			entry_len = entry_length(&tmp_ih, deh, i);

			get_object_key(deh, &object_key, &entry_key, &tmp_ih);

			if ((dir_format == KEY_FORMAT_2)
			    && (entry_len % 8 != 0)) {
				/* not alighed directory of new format - delete it */
				fsck_log
				    ("Entry %K (\"%.*s\") in the directory %K is not formated properly - fixed.\n",
				     (struct reiserfs_key *)&(deh->deh2_dir_id),
				     namelen, name, &tmp_ih.ih_key);
				reiserfs_remove_entry(fs, &entry_key);
				entry_len = name_length(name, dir_format);
				reiserfs_add_entry(fs, key, name, entry_len,
						   (struct reiserfs_key *)
						   &(deh->deh2_dir_id), 0);
			}
/*	
	    if ((dir_format == KEY_FORMAT_1) && (namelen != entry_len)) {
	    	// aligned entry in directory of old format - remove and insert it back
		fsck_log ("Entry %K (\"%.*s\") in the directory %K is not formated properly - deleted\n",
		    (struct reiserfs_key *)&(deh->deh2_dir_id), namelen, name, &tmp_ih.ih_key);
		reiserfs_remove_entry (fs, &entry_key);
		entry_len = name_length (name, dir_format);
		reiserfs_add_entry (fs, key, name, entry_len,
				(struct reiserfs_key *)&(deh->deh2_dir_id), 0);
	    }
*/
			if (is_dot(name, namelen)) {
				dir_size += DEH_SIZE + entry_len;
				continue;
			}

			print_name(name, namelen);

			if (!is_properly_hashed
			    (fs, name, namelen, get_deh_offset(deh)))
				reiserfs_panic
				    ("rebuild_semantic_pass: Hash mismatch detected for (\"%.*s\") in directory %K\n",
				     namelen, name, &tmp_ih.ih_key);

			retval1 =
			    rebuild_semantic_pass(&object_key, key,
						  is_dot_dot(name, namelen),
						  &relocated_ih);

			erase_name(namelen);

			switch (retval1) {
			case OK:
				dir_size += DEH_SIZE + entry_len;
				break;

			case STAT_DATA_NOT_FOUND:
			case DIRECTORY_HAS_NO_ITEMS:
				if (get_offset(&entry_key) == DOT_DOT_OFFSET &&
				    get_key_objectid(&object_key) ==
				    REISERFS_ROOT_PARENT_OBJECTID) {
					/* ".." of root directory can not be found */
					dir_size += DEH_SIZE + entry_len;
					continue;
				}
				fsck_log
				    ("%s: The entry %K (\"%.*s\") in directory %K points to nowhere - is removed\n",
				     __FUNCTION__, &object_key, namelen, name,
				     &tmp_ih.ih_key);
				reiserfs_remove_entry(fs, &entry_key);
				sem_pass_stat(fs)->deleted_entries++;
				break;

			case RELOCATED:
				/* file was relocated, update key in corresponding directory entry */
				if (reiserfs_search_by_entry_key
				    (fs, &entry_key, &path) != POSITION_FOUND) {
					fsck_log
					    ("WARNING: Cannot find the name of the relocated file %K in the directory %K\n",
					     &object_key, &tmp_ih.ih_key);
				} else {
					/* update key dir entry points to */
					struct reiserfs_de_head *tmp_deh;

					tmp_deh =
					    B_I_DEH(get_bh(&path),
						    tp_item_head(&path)) +
					    path.pos_in_item;
					fsck_log
					    ("The entry %K (\"%.*s\") in directory %K updated to point to ",
					     &object_key, namelen, name,
					     &tmp_ih.ih_key);
					set_deh_dirid(tmp_deh,
						      get_key_dirid
						      (&relocated_ih.ih_key));
					set_deh_objectid(tmp_deh,
							 get_key_objectid
							 (&relocated_ih.
							  ih_key));

					fsck_log("%K\n", &tmp_deh->deh2_dir_id);
					mark_buffer_dirty(get_bh(&path));
				}
				dir_size += DEH_SIZE + entry_len;
				pathrelse(&path);
				break;
			}
		}		/* for */

		freemem(dir_item);
		free(name);
		name = NULL;

		if (not_of_one_file(&item_key, key))
			/* next key is not of this directory */
			break;

	}			/* while (dir_item) */

	if (dir_size == 0)
		/* FIXME: is it possible? */
		return DIRECTORY_HAS_NO_ITEMS;

	/* calc correct value of sd_blocks field of stat data */
	blocks = dir_size2st_blocks(dir_size);

	fix_sd = 0;
	fix_sd += wrong_st_blocks(key, &blocks, saved_blocks, mode, is_new_dir);
	fix_sd +=
	    wrong_st_size(key, is_new_dir ? MAX_FILE_SIZE_V2 : MAX_FILE_SIZE_V1,
			  fs->fs_blocksize, &dir_size, saved_size,
			  TYPE_DIRENTRY);

	if (fix_sd) {
		/* we have to fix either sd_size or sd_blocks, so look for stat data again */
		if (reiserfs_search_by_key_4(fs, key, &path) != ITEM_FOUND)
			reiserfs_panic
			    ("rebuild_semantic_pass: The StatData of the file %K was not found",
			     key);

		bh = get_bh(&path);
		ih = tp_item_head(&path);
		sd = tp_item_body(&path);

		set_sd_size(ih, sd, &dir_size);
		set_sd_blocks(ih, sd, &blocks);
		mark_buffer_dirty(bh);
		pathrelse(&path);
	}

	return retval;
}

int is_dot(char *name, int namelen)
{
	return (namelen == 1 && name[0] == '.') ? 1 : 0;
}

int is_dot_dot(char *name, int namelen)
{
	return (namelen == 2 && name[0] == '.' && name[1] == '.') ? 1 : 0;
}

int not_a_directory(void *sd)
{
	/* mode is at the same place and of the same size in both stat
	   datas (v1 and v2) */
	struct stat_data_v1 *sd_v1 = sd;

	return !(S_ISDIR(le16_to_cpu(sd_v1->sd_mode)));
}

int not_a_regfile(void *sd)
{
	/* mode is at the same place and of the same size in both stat
	   datas (v1 and v2) */
	struct stat_data_v1 *sd_v1 = sd;

	return !(S_ISREG(le16_to_cpu(sd_v1->sd_mode)));
}

void zero_nlink(struct item_head *ih, void *sd)
{
	int zero = 0;

	if (get_ih_item_len(ih) == SD_V1_SIZE
	    && get_ih_key_format(ih) != KEY_FORMAT_1) {
		fsck_log
		    ("zero_nlink: The StatData %k of the wrong format version (%d) - corrected to (%d)\n",
		     ih, get_ih_key_format(ih), KEY_FORMAT_1);
		set_ih_key_format(ih, KEY_FORMAT_1);
	}
	if (get_ih_item_len(ih) == SD_SIZE
	    && get_ih_key_format(ih) != KEY_FORMAT_2) {
		fsck_log
		    ("zero_nlink: The StatData %k of the wrong format version (%d) - corrected to (%d)\n",
		     ih, get_ih_key_format(ih), KEY_FORMAT_2);
		set_ih_key_format(ih, KEY_FORMAT_2);
	}

	set_sd_nlink(ih, sd, &zero);
}

void modify_item(struct item_head *ih, void *item)
{
	zero_nlink(ih, item);
	mark_item_unreachable(ih);
}

/* mkreiserfs should have created this */
static void make_sure_lost_found_exists(reiserfs_filsys_t fs)
{
	int retval;
	INITIALIZE_REISERFS_PATH(path);
	unsigned int gen_counter;
	__u32 objectid;
	__u64 sd_size;
	__u32 sd_blocks;
	struct buffer_head *bh;
	void *sd;
	int item_len;

	/* look for "lost+found" in the root directory */
	retval = reiserfs_find_entry(fs, &root_dir_key,
				     "lost+found", &gen_counter,
				     &lost_found_dir_key);
	if (!retval) {
		objectid = id_map_alloc(proper_id_map(fs));
		if (!objectid) {
			fsck_progress
			    ("Could not allocate an objectid for \"/lost+found\", \
		lost files will not be linked\n");
			return;
		}
		set_key_dirid(&lost_found_dir_key, REISERFS_ROOT_OBJECTID);
		set_key_objectid(&lost_found_dir_key, objectid);
	}

	/* look for stat data of "lost+found" */
	retval = reiserfs_search_by_key_4(fs, &lost_found_dir_key, &path);
	if (retval == ITEM_NOT_FOUND)
		lost_found_dir_format =
		    create_dir_sd(fs, &path, &lost_found_dir_key, modify_item);
	else {
		struct item_head *ih = tp_item_head(&path);

		if (!is_stat_data_ih(ih))
			reiserfs_panic("It must be lost+found's stat data %k\n",
				       &ih->ih_key);

		fix_obviously_wrong_sd_mode(&path);

		if (not_a_directory(tp_item_body(&path))) {
			fsck_progress
			    ("\"/lost+found\" exists, but it is not a directory, \
		lost files will not be linked\n");
			set_key_objectid(&lost_found_dir_key, 0);
			pathrelse(&path);
			return;
		}

		lost_found_dir_format =
		    (get_ih_item_len(tp_item_head(&path)) ==
		     SD_SIZE) ? KEY_FORMAT_2 : KEY_FORMAT_1;

		pathrelse(&path);
	}

	/* add "." and ".." if any of them do not exist */
	reiserfs_add_entry(fs, &lost_found_dir_key, ".",
			   name_length(".", lost_found_dir_format),
			   &lost_found_dir_key, 1 << IH_Unreachable);
	reiserfs_add_entry(fs, &lost_found_dir_key, "..",
			   name_length("..", lost_found_dir_format),
			   &root_dir_key, 1 << IH_Unreachable);

	item_len = reiserfs_add_entry(fs, &root_dir_key, "lost+found",
				      name_length("lost+found",
						  root_dir_format),
				      &lost_found_dir_key, 1 << IH_Unreachable);

	if (item_len) {
		struct item_head *ih;
		if (reiserfs_search_by_key_4(fs, &root_dir_key, &path) ==
		    ITEM_NOT_FOUND)
			reiserfs_panic
			    ("%s: StatData of the root directory must exists",
			     __FUNCTION__);

		bh = get_bh(&path);
		ih = tp_item_head(&path);
		sd = tp_item_body(&path);

		get_sd_size(ih, sd, &sd_size);
		sd_size += item_len;
		set_sd_size(ih, sd, &sd_size);
		sd_blocks = dir_size2st_blocks(sd_size);
		set_sd_blocks(ih, sd, &sd_blocks);
		mark_buffer_dirty(bh);
		pathrelse(&path);
	}

	return;
}

/* Result of the rebuild pass will be saved in the state file which is needed to start
 * fsck again from the next pass. */
static void save_rebuild_semantic_result(reiserfs_filsys_t fs)
{
	FILE *file;
	int retval;

	file = open_file("temp_fsck_file.deleteme", "w+");
	if (!file)
		return;

	reiserfs_begin_stage_info_save(file, SEMANTIC_DONE);
	/*  Method not implemented yet.
	   reiserfs_objectid_map_save (file, semantic_id_map (fs));
	 */
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
void load_semantic_result(FILE * file, reiserfs_filsys_t fs)
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
	   semantic_id_map (fs) = reiserfs_objectid_map_load (file);
	 */
}

static void before_pass_3(reiserfs_filsys_t fs)
{
	semantic_id_map(fs) = id_map_init();
}

static void after_pass_3(reiserfs_filsys_t fs)
{
	int ret;
	/* update super block: objectid map, fsck state */
	set_sb_fs_state(fs->fs_ondisk_sb, SEMANTIC_DONE);
	mark_buffer_dirty(fs->fs_super_bh);

	/* write all dirty blocks */
	fsck_progress("Flushing..");
	id_map_flush(proper_id_map(fs), fs);
	fs->fs_dirt = 1;
	ret = reiserfs_flush_to_ondisk_bitmap(fsck_new_bitmap(fs), fs);
	if (ret < 0)
		reiserfs_exit(1, "Exiting after unrecoverable error.");

	reiserfs_flush(fs);
	fsck_progress("finished\n");

	stage_report(3, fs);

	if (!fsck_run_one_step(fs)) {
		if (fsck_user_confirmed(fs, "Continue? (Yes):", "Yes\n", 1))
			/* reiserfsck continues */
			return;
	}

	save_rebuild_semantic_result(fs);

	id_map_free(proper_id_map(fs));
	proper_id_map(fs) = NULL;

	fs->fs_dirt = 1;
	reiserfs_close(fs);
	exit(0);
}

/* this is part of rebuild tree */
void pass_3_semantic(reiserfs_filsys_t fs)
{
	before_pass_3(fs);

	fsck_progress("Pass 3 (semantic):\n");

	/* when warnings go not to stderr - separate them in the log */
	if (fsck_log_file(fs) != stderr)
		fsck_log("####### Pass 3 #########\n");

	if (!fs->fs_hash_function)
		reiserfs_panic("Hash function should be selected already");

	make_sure_root_dir_exists(fs, modify_item, 1 << IH_Unreachable);
	make_sure_lost_found_exists(fs);

	id_map_mark(proper_id_map(fs), get_key_objectid(&root_dir_key));
	id_map_mark(proper_id_map(fs), get_key_objectid(&lost_found_dir_key));

	/* link all relocated files into /lost+found directory */
	link_relocated_files();

	rebuild_semantic_pass(&root_dir_key, &parent_root_dir_key,
			      0 /*!dot_dot */ , NULL /*reloc_ih */ );

	add_badblock_list(fs, 1);

	after_pass_3(fs);
}
