/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "debugreiserfs.h"
#include <time.h>

extern struct reiserfs_fsstat g_stat_info;

int do_one_ih_corrupt(struct item_head *ih, unsigned int nr_bytes);
int do_one_ih_random_corrupt(struct item_head *ih);

void do_one_corruption_in_one_block(reiserfs_filsys_t fs,
				    struct buffer_head *bh,
				    char *corruption_command);
int corrupt_block_header(struct block_head *blkh, unsigned int offset,
			 unsigned int bytes);
void do_one_block_random_corrupt(struct buffer_head *bh);

static int str2int(char *str, int *res)
{
	int val;
	char *tmp;

	val = (int)strtol(str, &tmp, 0);
	if (tmp == str)
		/* could not convert string into a number */
		return 0;
	*res = val;
	return 1;
}

static int get_rand(double min, double max)
{

	/* srand (time (0)); */
	int ret;

	ret = (int)(min + (int)((max - min + 1) * rand() / (RAND_MAX + 1.0)));
	if ((ret < min) || (ret > max))
		die("get_rand failed: min %d, max %d, returned %d\n", (int)min,
		    (int)max, ret);

	return ret;
}

static void edit_journal_params(struct journal_params *jp)
{
	char *str;
	size_t n;
	int num;

	printf("Journal parameters:\n");
	printf("\tDevice: current: %x: new:", get_jp_journal_dev(jp));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_jp_journal_dev(jp, num);

	printf("\tFirst block: current: %d: new:",
	       get_jp_journal_1st_block(jp));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_jp_journal_1st_block(jp, num);

	printf("\tSize: current: %d: new:", get_jp_journal_size(jp));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_jp_journal_size(jp, num);

	printf("\tMagic number: current: %d: new:", get_jp_journal_magic(jp));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_jp_journal_magic(jp, num);

	printf("\tMax transaction size: current: %d: new:",
	       get_jp_journal_max_trans_len(jp));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_jp_journal_max_trans_len(jp, num);

	printf("\tMax batch size: current: %d: new:",
	       get_jp_journal_max_batch(jp));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_jp_journal_max_batch(jp, num);

	printf("\tMax commit age: current: %d: new:",
	       get_jp_journal_max_commit_age(jp));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_jp_journal_max_commit_age(jp, num);
}

/* this allows to edit all super block fields */
static void edit_super_block(reiserfs_filsys_t fs)
{
	char *str;
	size_t n;
	int num;

	str = NULL;
	n = 0;

	/* bs_block_count */
	printf("\tBlock count: current: %u: new:",
	       get_sb_block_count(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_block_count(fs->fs_ondisk_sb, num);

	/* sb_free_blocks */
	printf("\tFree block count: current: %u: new:",
	       get_sb_free_blocks(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_free_blocks(fs->fs_ondisk_sb, num);

	/* sb_root_block */
	printf("\tRoot block: current: %u: new:",
	       get_sb_root_block(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_root_block(fs->fs_ondisk_sb, num);

	/* sb_journal */
	edit_journal_params(sb_jp(fs->fs_ondisk_sb));

	/* sb_blocksize */
	printf("\tBlocksize: current: %u: new:",
	       get_sb_block_size(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_block_size(fs->fs_ondisk_sb, num);

	/* sb_oid_maxsize */
	printf("\tMax objectid size: current: %u: new:",
	       get_sb_oid_maxsize(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_oid_maxsize(fs->fs_ondisk_sb, num);

	/* sb_oid_cursize */
	printf("\tCur objectid size: current: %u: new:",
	       get_sb_oid_cursize(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_oid_cursize(fs->fs_ondisk_sb, num);

	/* sb_state */
	printf("\tUmount state: current: %u: new:",
	       get_sb_umount_state(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_umount_state(fs->fs_ondisk_sb, num);

	/* char s_magic [10]; */
	printf("\tMagic: current: \"%s\": new:",
	       fs->fs_ondisk_sb->s_v1.s_magic);
	getline(&str, &n, stdin);
	if (strcmp(str, "\n"))
		strncpy(fs->fs_ondisk_sb->s_v1.s_magic, str, n > 10 ? 10 : n);

	/* __u16 sb_fsck_state; */
	printf("\tFilesystem state: current: %u: new:",
	       get_sb_fs_state(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_fs_state(fs->fs_ondisk_sb, num);

	/* __u32 sb_hash_function_code; */
	printf("\tHash code: current: %u: new (tea %d, r5 %d, rupasov %d):",
	       get_sb_hash_code(fs->fs_ondisk_sb), TEA_HASH, R5_HASH,
	       YURA_HASH);
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_hash_code(fs->fs_ondisk_sb, num);

	/* __u16 sb_tree_height; */
	printf("\tTree height: current: %u: new:",
	       get_sb_tree_height(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_tree_height(fs->fs_ondisk_sb, num);

	/* __u16 sb_bmap_nr; */
	printf("\tNumber of bitmaps: current: %u: new:",
	       get_sb_bmap_nr(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_bmap_nr(fs->fs_ondisk_sb, num);

	/* __u16 sb_version; */
	printf("\tFilesystem format: current: %u: new:",
	       le16_to_cpu(fs->fs_ondisk_sb->s_v1.sb_version));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_version(fs->fs_ondisk_sb, num);

	/* __u16 sb_reserved_for_journal; */
	printf("\tSpace reserved for journal: current: %u: new:",
	       get_sb_reserved_for_journal(fs->fs_ondisk_sb));
	getline(&str, &n, stdin);
	if (str2int(str, &num))
		set_sb_reserved_for_journal(fs->fs_ondisk_sb, num);

	print_block(stdout, fs, fs->fs_super_bh);
	if (user_confirmed(stderr, "Is this ok ? [N/Yes]: ", "Yes\n")) {
		mark_buffer_dirty(fs->fs_super_bh);
		bwrite(fs->fs_super_bh);
	}
}

static void corrupt_clobber_hash(char *name, struct item_head *ih,
				 struct reiserfs_de_head *deh)
{
	printf("\tCorrupting deh_offset of entry \"%s\" of [%u %u]\n", name,
	       get_key_dirid(&ih->ih_key), get_key_objectid(&ih->ih_key));
	set_deh_offset(deh, 700);
}

/* this reads list of desired corruptions from stdin and perform the
   corruptions. Format of that list:
   A hash_code
   C name objectid     - 'C'ut entry 'name' from directory item with 'objectid'
   H name objectid     - clobber 'H'hash of entry 'name' of directory 'objectid'
   I item_num pos_in_item  make pos_in_item-th slot of indirect item to point out of device
   O item_num          - destroy item 'O'rder - make 'item_num'-th to have key bigger than 'item_num' + 1-th item
   D item_num          - 'D'elete item_num-th item
   S item_num value    - change file size (item_num-th item must be stat data)
   F item_num value    - change sd_first_direct_byte of stat data
   J item_num objectid
   E name objectid new - change entry's deh_objectid to new
   P                   - print the block
   B offset bytes_to_corrupt - corrupt bytes_to_corrupt bytes in block header,
                               start from offset
*/

void do_corrupt_one_block(reiserfs_filsys_t fs, char *fline)
{
	struct buffer_head *bh;
	char *line = NULL;
	size_t n = 0;
	unsigned long block;

	block = certain_block(fs);
	printf("block = %lu\n", block);
	if (block == fs->fs_super_bh->b_blocknr) {
		edit_super_block(fs);
		return;
	}

	if (!fs->fs_bitmap2) {
		struct buffer_head *bm_bh;
		unsigned long bm_block;

		if (spread_bitmaps(fs))
			bm_block = (block / (fs->fs_blocksize * 8)) ?
			    (block / (fs->fs_blocksize * 8)) *
			    (fs->fs_blocksize *
			     8) : fs->fs_super_bh->b_blocknr + 1;
		else
			bm_block =
			    fs->fs_super_bh->b_blocknr + 1 +
			    (block / (fs->fs_blocksize * 8));

		bm_bh = bread(fs->fs_dev, bm_block, fs->fs_blocksize);
		if (bm_bh) {
			if (misc_test_bit
			    ((block % (fs->fs_blocksize * 8)), bm_bh->b_data))
				fprintf(stderr,
					"%lu is used in ondisk bitmap\n",
					block);
			else
				fprintf(stderr,
					"%lu is free in ondisk bitmap\n",
					block);

			brelse(bm_bh);
		}
	} else {
		if (reiserfs_bitmap_test_bit(fs->fs_bitmap2, block))
			fprintf(stderr, "%lu is used in ondisk bitmap\n",
				block);
		else
			fprintf(stderr, "%lu is free in ondisk bitmap\n",
				block);
	}

	/* READ block */
	bh = bread(fs->fs_dev, block, fs->fs_blocksize);
	if (!bh) {
		printf("corrupt_one_block: bread fialed\n");
		return;
	}

	if (who_is_this(bh->b_data, fs->fs_blocksize) != THE_LEAF) {
		printf("Can not corrupt not a leaf node\n");
		brelse(bh);
		return;
	}

	printf("Corrupting block %lu..\n", bh->b_blocknr);

	if (data(fs)->log_file_name) {
		printf("Log file : %s\n", data(fs)->log_file_name);
	} else {
		printf("No Log file specified\n");
	}

	if (fline != NULL) {
		do_one_corruption_in_one_block(fs, bh, fline);
		if ((data(fs)->log_file_name) && (data(fs)->log)) {
			fprintf(data(fs)->log, "%lu\n", block);
			fprintf(data(fs)->log, "%s\n", fline);
		}
	} else {
		/* Get list of corruptions from stdin */
		while (getline(&line, &n, stdin) != -1) {
			if (line[0] == '\n') {
				free(line);
				line = NULL;
				n = 0;
				break;
			}
			do_one_corruption_in_one_block(fs, bh, line);
			if ((data(fs)->log_file_name) && (data(fs)->log)) {
				fprintf(data(fs)->log, "%lu\n", block);
				fprintf(data(fs)->log, "%s\n", line);
			}

			free(line);
			line = NULL;
			n = 0;
		}
	}

	printf("Done\n");
	bwrite(bh);
	brelse(bh);
	return;
}

void do_one_corruption_in_one_block(reiserfs_filsys_t fs,
				    struct buffer_head *bh,
				    char *corruption_command)
{
	int i, j;
	struct item_head *ih;
	int item_num;
	int item_numbers;
	int bytes_to_corrupt;
	char code, name[100];
	__u32 objectid, new_objectid;
	int value;
	int hash_code;
	unsigned int pos_in_item;
	int type, format;

	printf("corruption_command  : %s", corruption_command);

	switch (corruption_command[0]) {
	case '#':
	case '\n':
		break;

	case '?':
		printf("A hash_code     - reset hAsh code in super block\n"
		       "T item_num type (0, 1, 2, 3) format (0, 1)\n"
		       "C name objectid - Cut entry 'name' from directory item with 'objectid'\n"
		       "H name objectid - clobber Hash of entry 'name' of directory 'objectid'\n"
		       "I item_num pos_in_item  make pos_in_tem-th slot of Indirect item to point out of device\n"
		       "O item_num      - destroy item Order - make 'item_num'-th to have key bigger than 'item_num' + 1-th item\n"
		       "D item_num      - Delete item_num-th item\n"
		       "S item_num value - change file Size (item_num-th item must be stat data)\n"
		       "F item_num value - change sd_First_direct_byte of stat data\n"
		       "J item_num objectid - set 'obJectid' of 'item_num'-th item\n"
		       "E name objectid objectid - set deh_objectid of an entry to objectid\n"
		       "N item_numbers bytes_to_corrupt - corrupt bytes_to_corrupt in number of bytes in item_numbers items\n"
		       "B offset bytes_to_corrupt - corrupt bytes_to_corrupt in block_header, start corruption from offset\n");

		break;

	case 'P':
		print_block(stderr, fs, bh, 3, -1, -1);
		break;

	case 'A':
		/* corrupt hash record in super block */
		if (sscanf(corruption_command, "%c %d\n", &code, &hash_code) !=
		    2) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		reiserfs_warning(stderr, "Changing %s to %s\n",
				 code2name(get_sb_hash_code(fs->fs_ondisk_sb)),
				 code2name(hash_code));
		set_sb_hash_code(fs->fs_ondisk_sb, hash_code);
		break;

	case 'C':		/* cut entry */
	case 'H':		/* make hash wrong */
		if (sscanf
		    (corruption_command, "%c %s %u\n", &code, name,
		     &objectid) != 3) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}

		ih = item_head(bh, 0);
		for (i = 0; i < get_blkh_nr_items(B_BLK_HEAD(bh)); i++, ih++) {
			struct reiserfs_de_head *deh;
			/* look for property objectid */
			if (get_key_objectid(&ih->ih_key) != objectid
			    || !is_direntry_ih(ih))
				continue;

			deh = B_I_DEH(bh, ih);

			for (j = 0; j < get_ih_entry_count(ih); j++, deh++) {
				/* look for proper entry */
				if (name_in_entry_length(ih, deh, j) ==
				    (int)strlen(name)
				    && !strncmp(name, name_in_entry(deh, j),
						strlen(name)))
					break;
			}
			if (j == get_ih_entry_count(ih)) {
				printf("Can't find entry %s\n", name);
				exit(1);
			}
			switch (code) {
			case 'H':	/* clobber hash */
				corrupt_clobber_hash(name, ih, deh);
				break;

			case 'C':	/* cut entry */
				cut_entry(fs, bh, i, j, 1);
				break;

			default:
				printf("Unknown command found\n");
			}
		}

		if (!B_IS_IN_TREE(bh)) {
			printf("NOTE: block is deleted from the tree\n");
			exit(0);
		}
		break;

	case 'E':		/* set objectid : used to simulate objectid sharing problem */
		if (sscanf
		    (corruption_command, "%c %s %u %d\n", &code, name,
		     &objectid, &new_objectid) != 4) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		ih = item_head(bh, 0);
		for (i = 0; i < get_blkh_nr_items(B_BLK_HEAD(bh)); i++, ih++) {
			struct reiserfs_de_head *deh;
			/* look for property objectid */
			if (get_key_objectid(&ih->ih_key) != objectid
			    || !is_direntry_ih(ih))
				continue;
			deh = B_I_DEH(bh, ih);
			set_deh_objectid(deh, new_objectid);
			break;
		}
		break;

	case 'T':		/* set type of item */
		if (sscanf
		    (corruption_command, "%c %d %d %d\n", &code, &item_num,
		     &type, &format) != 4) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		if (item_num > get_blkh_nr_items(B_BLK_HEAD(bh))) {
			printf("Wrong format \'%c\', wrong item_num \n",
			       corruption_command[0]);
			return;
		}

		ih = item_head(bh, item_num);
		set_ih_key_format(ih, format);
		set_type(format, &ih->ih_key, type);

		break;

	case 'J':		/* set objectid : used to simulate objectid sharing problem */
		if (sscanf
		    (corruption_command, "%c %d %d\n", &code, &item_num,
		     &objectid) != 3) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		if (item_num > get_blkh_nr_items(B_BLK_HEAD(bh))) {
			printf("Wrong format \'%c\', wrong item_num \n",
			       corruption_command[0]);
			return;
		}
		ih = item_head(bh, item_num);
		set_key_objectid(&ih->ih_key, objectid);
		break;

	case 'I':		/* break unformatted node pointer */
		if (sscanf
		    (corruption_command, "%c %d %u\n", &code, &item_num,
		     &pos_in_item) != 3) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		if (item_num > get_blkh_nr_items(B_BLK_HEAD(bh))) {
			printf("Wrong format \'%c\', wrong item_num \n",
			       corruption_command[0]);
			return;
		}
		ih = item_head(bh, item_num);
		if (!is_indirect_ih(ih) || pos_in_item >= I_UNFM_NUM(ih)) {
			reiserfs_warning(stderr,
					 "Not an indirect item or there is "
					 "not so many unfm ptrs in it\n");
			return;
		}
		d32_put((__le32 *) ih_item_body(bh, ih), pos_in_item,
			get_sb_block_count(fs->fs_ondisk_sb) + 100);
		break;

	case 'D':		/* delete item */
		if (sscanf(corruption_command, "%c %d\n", &code, &item_num) !=
		    2) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		if (item_num > get_blkh_nr_items(B_BLK_HEAD(bh))) {
			printf("Wrong format \'%c\', wrong item_num \n",
			       corruption_command[0]);
			return;
		}
		delete_item(fs, bh, item_num);
		break;

	case 'O':		/* make item out of order */
		{
			struct reiserfs_key *key;
			if (sscanf
			    (corruption_command, "%c %d\n", &code,
			     &item_num) != 2) {
				printf("Wrong format \'%c\'\n",
				       corruption_command[0]);
				return;
			}
			if (item_num > get_blkh_nr_items(B_BLK_HEAD(bh))) {
				printf("Wrong format \'%c\', wrong item_num \n",
				       corruption_command[0]);
				return;
			}

			/* destroy item order */

			if (item_num == get_blkh_nr_items(B_BLK_HEAD(bh)) - 1) {
				printf("can not destroy order\n");
				return;
			}
			ih = item_head(bh, item_num);
			key = &(ih + 1)->ih_key;
			set_key_dirid(&ih->ih_key, get_key_dirid(key) + 1);

			break;
		}
	case 'S':		/* corrupt st_size */
		{
			/* fixme: old stat data only */
			struct stat_data_v1 *sd;

			if (sscanf
			    (corruption_command, "%c %d %d\n", &code, &item_num,
			     &value) != 3) {
				printf("Wrong format \'%c\'\n",
				       corruption_command[0]);
				return;
			}
			if (item_num > get_blkh_nr_items(B_BLK_HEAD(bh))) {
				printf("Wrong format \'%c\', wrong item_num \n",
				       corruption_command[0]);
				return;
			}
			ih = item_head(bh, item_num);

			sd = (struct stat_data_v1 *)ih_item_body(bh, ih);
			reiserfs_warning(stderr,
					 "Changing sd_size of %k from %d to %d\n",
					 &ih->ih_key, sd_v1_size(sd), value);
			set_sd_v1_size(sd, value);
			break;
		}

	case 'F':		/*         st_first_direct_byte */
		{
			/* fixme: old stat data only */
			struct stat_data_v1 *sd;

			if (sscanf
			    (corruption_command, "%c %d %d\n", &code, &item_num,
			     &value) != 3) {
				printf("Wrong format \'%c\'\n",
				       corruption_command[0]);
				return;
			}
			if (item_num > get_blkh_nr_items(B_BLK_HEAD(bh))) {
				printf("Wrong format \'%c\', wrong item_num \n",
				       corruption_command[0]);
				return;
			}
			ih = item_head(bh, item_num);

			sd = (struct stat_data_v1 *)ih_item_body(bh, ih);
			reiserfs_warning(stderr,
					 "Changing sd_first_direct_byte of %k from %d to %d\n",
					 &ih->ih_key,
					 sd_v1_first_direct_byte(sd), value);
			set_sd_v1_first_direct_byte(sd, value);
			break;
		}

	case 'N':		/* corrupt N number of items */
		if (sscanf(corruption_command, "%c %d %d\n",
			   &code, &item_numbers, &bytes_to_corrupt) != 3) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}

		if (item_numbers > get_blkh_nr_items(B_BLK_HEAD(bh))) {
			printf
			    ("Wrong item_numbers %d expected not more then %d\n",
			     item_numbers, get_blkh_nr_items(B_BLK_HEAD(bh)));
			return;
		}

		for (i = 0; i < item_numbers; i++) {
			printf("Do corruptions :  %d item header; \n", i);
			ih = item_head(bh, i);
			do_one_ih_corrupt(ih, bytes_to_corrupt);
			printf("Ok\n");
		}

		break;
	case 'B':
		{
			struct block_head *blkh;
			unsigned int offset;

			if (sscanf(corruption_command, "%c %d %d\n",
				   &code, &offset, &bytes_to_corrupt) != 3) {
				printf("Wrong format \'%c\'\n",
				       corruption_command[0]);
				return;
			}

			blkh = B_BLK_HEAD(bh);
			corrupt_block_header(blkh, offset, bytes_to_corrupt);
			break;
		}

	default:
		printf("Unknown command found\n");
	}
	mark_buffer_dirty(bh);
	return;
}

/* corrupt first nr_bytes bytes in item header */
int do_one_ih_corrupt(struct item_head *ih, unsigned int nr_bytes)
{
	if (nr_bytes > IH_SIZE) {
		printf("Bad byte number %u expected not more then %lu\n",
		       nr_bytes, (unsigned long)IH_SIZE);
		exit(1);
	}

	if (memset((char *)ih, 0, nr_bytes) != ih) {
		perror("do_one_ih_corrupt: memset failed");
		exit(1);
	}

	if ((data(fs)->log_file_name) && (data(fs)->log))
		fprintf(data(fs)->log, "\tfirst %u bytes corrupted\n",
			nr_bytes);

	printf("\tfirst %u bytes corrupted\n", nr_bytes);
	return 0;
}

/* item header random corruption */
int do_one_ih_random_corrupt(struct item_head *ih)
{
	unsigned int i;
	unsigned int from;
	unsigned int count;

	from = get_rand(0, IH_SIZE - 1);
	count = get_rand(1, IH_SIZE);

	if (from + count > IH_SIZE)
		count = IH_SIZE - from;

	for (i = from; i < from + count; i++)
		((char *)ih)[i] = get_rand(0, 255);

	if ((data(fs)->log_file_name) && (data(fs)->log))
		fprintf(data(fs)->log, "\tfrom %u ( %u )\n", from, count);

	printf("\tfrom %u ( %u )\n", from, count);
	return 0;
}

/* Corrupt n bytes in block header */
int corrupt_block_header(struct block_head *blkh, unsigned int offset,
			 unsigned int bytes)
{
	if ((offset + bytes) > BLKH_SIZE) {
		printf
		    ("Bad offset number: %u or bad bytes number: %u, the suumary "
		     "value expected not more then %lu\n", offset, bytes,
		     (unsigned long)BLKH_SIZE);
		exit(1);
	}

	if (memset((char *)blkh, 0, bytes) != blkh) {
		perror("corrupt_block_head: memset failed");
		exit(1);
	}

	printf("offset : %u, corrupt %u bytes\n", offset, bytes);
	return 0;
}

/* corrupt random number of bytes within block header started from random
   offset */
static void do_one_blkh_random_corrupt(struct buffer_head *bh)
{
	struct block_head *blkh;
	unsigned int from;
	unsigned int count;
	unsigned int i;

	from = get_rand(0, BLKH_SIZE - 1);
	count = get_rand(1, BLKH_SIZE);

	blkh = B_BLK_HEAD(bh);

	if (from + count > BLKH_SIZE)
		count = BLKH_SIZE - from;

	for (i = from; i < from + count; i++)
		((char *)blkh)[i] = get_rand(0, 255);

	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log, "# : %lu #    ", bh->b_blocknr);
		fprintf(data(fs)->log, "from %u (%u)\n", from, count);
	}

	printf("# : %lu #   ", bh->b_blocknr);
	printf("from %u (%u)\n", from, count);

}

void do_leaves_corruption(reiserfs_filsys_t fs, unsigned long nr_leaves_cr)
{
	struct buffer_head *bh;
	unsigned long nr_leaves = 0;
	unsigned int i, should_be_corrupted;

	srand(time(NULL));
	printf("%lu leaves will be corrupted\n", nr_leaves_cr);
	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log,
			"Block headers in %lu leaves will be corrupted\n",
			nr_leaves_cr);
	}

	if (reiserfs_open_ondisk_bitmap(fs) < 0)
		reiserfs_exit(1, "Could not open ondisk bitmap");

	for (i = 0; (i < get_sb_block_count(fs->fs_ondisk_sb)) &&
	     nr_leaves < nr_leaves_cr; i++) {

		if (!reiserfs_bitmap_test_bit(fs->fs_bitmap2, i))
			continue;

		bh = bread(fs->fs_dev, i, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr, "could not read block %lu\n",
					 i);
			continue;
		}

		if (who_is_this(bh->b_data, bh->b_size) != THE_LEAF) {
			brelse(bh);
			continue;
		}

		if ((!is_leaf_node(bh)) || (block_of_journal(fs, i))) {
			brelse(bh);
			continue;
		}

		should_be_corrupted =
		    (unsigned int)get_rand((double)0, (double)1);
		if (should_be_corrupted == 0) {
			brelse(bh);
			continue;
		}
		do_one_blkh_random_corrupt(bh);
		/* do_one_block_random_corrupt (bh); */
		mark_buffer_dirty(bh);
		bwrite(bh);
		brelse(bh);
		nr_leaves++;
	}

	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log,
			"%lu leaves WERE corrupted\n", nr_leaves);
	}

	printf("%lu leaves WERE corrupted\n", nr_leaves);

	reiserfs_close_ondisk_bitmap(fs);
	return;
}

void do_one_block_random_corrupt(struct buffer_head *bh)
{
	unsigned int from = get_rand(0, bh->b_size - 1);
	unsigned int count = get_rand(1, bh->b_size);
	unsigned int i;

	if (from + count > bh->b_size)
		count = bh->b_size - from;

	for (i = from; i < from + count; i++)
		((char *)bh->b_data)[i] = get_rand(0, 255);

	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log, "# block %lu: ", bh->b_blocknr);
		fprintf(data(fs)->log, "from %u ( %u )\n", from, count);
	}

	printf("# block %lu: ", bh->b_blocknr);
	printf("from %u ( %u )\n", from, count);

}

void do_bitmap_corruption(reiserfs_filsys_t fs)
{

	unsigned long first = fs->fs_super_bh->b_blocknr + 1;
	unsigned long nr_bitmap_to_corrupt;
	unsigned long block;
	struct buffer_head *bh;
	unsigned int i;

	nr_bitmap_to_corrupt =
	    (unsigned long)get_rand(1, reiserfs_fs_bmap_nr(fs) - 1);

	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log, "%lu bitmaps will be corrupted\n",
			nr_bitmap_to_corrupt);
	}
	printf("%lu bitmaps will be corrupted\n", nr_bitmap_to_corrupt);
	for (i = 0; i < nr_bitmap_to_corrupt; i++) {
		block = (i == 0) ? first : fs->fs_blocksize * 8 * i;
		bh = bread(fs->fs_dev, block, fs->fs_blocksize);

		if (!bh) {
			printf
			    ("do_bitmap_corruption: bread failed for bitmap %d: %lu\n",
			     i, block);
			exit(1);
		}
		do_one_block_random_corrupt(bh);
		mark_buffer_dirty(bh);
		bwrite(bh);
		brelse(bh);
	}

}

/* corrupt the random number of item headers in random number of leaves */
static void do_ih_random_corrupt(reiserfs_filsys_t fs,
				 unsigned long nr_leaves_cr)
{
	unsigned int nr_ih_cr;
	unsigned int i, j;
	struct buffer_head *bh;
	struct item_head *ih;
	unsigned long nr_leaves = 0;
	unsigned int should_be_corrupted = 0;

	srand(time(NULL));

	printf("item headers in %lu leaves will be corrupted\n", nr_leaves_cr);
	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log,
			"item headers in %lu leaves will be corrupted\n",
			nr_leaves_cr);
	}

	if (reiserfs_open_ondisk_bitmap(fs) < 0)
		reiserfs_exit(1, "Could not open ondisk bitmap");

	for (i = 0; (i < get_sb_block_count(fs->fs_ondisk_sb)) &&
	     nr_leaves < nr_leaves_cr; i++) {

		if (!reiserfs_bitmap_test_bit(fs->fs_bitmap2, i))
			continue;

		bh = bread(fs->fs_dev, i, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr, "could not read block %lu\n",
					 i);
			continue;
		}

		if (who_is_this(bh->b_data, bh->b_size) != THE_LEAF) {
			brelse(bh);
			continue;
		}

		if ((!is_leaf_node(bh)) || (block_of_journal(fs, i))) {
			brelse(bh);
			continue;
		}

		should_be_corrupted =
		    (unsigned int)get_rand((double)0, (double)1);
		if (should_be_corrupted == 0) {
			brelse(bh);
			continue;
		}

		nr_ih_cr = get_rand(1, get_blkh_nr_items(B_BLK_HEAD(bh)));
		for (j = 0; j < nr_ih_cr; j++) {
			should_be_corrupted =
			    (unsigned int)get_rand((double)0, (double)1);
			if (should_be_corrupted == 0)
				continue;

			if ((data(fs)->log_file_name) && (data(fs)->log))
				fprintf(data(fs)->log,
					"# block %lu , item header %d\n",
					bh->b_blocknr, j);
			printf("# block %lu , item header %d\n", bh->b_blocknr,
			       j);

			ih = item_head(bh, j);
			do_one_ih_random_corrupt(ih);
		}
		mark_buffer_dirty(bh);
		bwrite(bh);
		brelse(bh);
		nr_leaves++;
	}

	if ((data(fs)->log_file_name) && (data(fs)->log))
		fprintf(data(fs)->log,
			"item headers in %lu leaves WERE corrupted\n",
			nr_leaves);
	printf("item headers in %lu leaves WERE corrupted\n", nr_leaves);

	reiserfs_close_ondisk_bitmap(fs);
}

/* corrupt item */
static void do_one_item_random_corrupt(struct buffer_head *bh,
				       struct item_head *ih)
{
	unsigned int i;
	unsigned int from;
	unsigned int count;
	char *p;

	p = (char *)ih_item_body(bh, ih);

	from = get_rand(0, get_ih_item_len(ih) - 1);
	count = get_rand(1, get_ih_item_len(ih));

	if (from + count > get_ih_item_len(ih))
		count = get_ih_item_len(ih) - from;

	for (i = from; i < from + count; i++)
		((char *)p)[i] = get_rand(0, 255);

	if ((data(fs)->log_file_name) && (data(fs)->log))
		fprintf(data(fs)->log, "item body \tfrom %u ( %u )\n", from,
			count);

	printf("item body \tfrom %u ( %u )\n", from, count);
	return;

}

/* corrupt the random number of directory items in random number of leaves */
static void do_dir_random_corrupt(reiserfs_filsys_t fs,
				  unsigned long nr_leaves_cr)
{
	unsigned int nr_ih_cr;
	unsigned int i, j;
	struct buffer_head *bh;
	struct item_head *ih;
	unsigned long nr_leaves = 0;
	unsigned int should_be_corrupted = 0;

	srand(time(NULL));

	printf("DIR items in %lu leaves will be corrupted\n", nr_leaves_cr);
	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log,
			"DIR items in %lu leaves will be corrupted\n",
			nr_leaves_cr);
	}

	if (reiserfs_open_ondisk_bitmap(fs) < 0)
		reiserfs_exit(1, "Could not open ondisk bitmap");

	for (i = 0; (i < get_sb_block_count(fs->fs_ondisk_sb)) &&
	     nr_leaves < nr_leaves_cr; i++) {

		if (!reiserfs_bitmap_test_bit(fs->fs_bitmap2, i))
			continue;

		bh = bread(fs->fs_dev, i, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr, "could not read block %lu\n",
					 i);
			continue;
		}

		if (who_is_this(bh->b_data, bh->b_size) != THE_LEAF) {
			brelse(bh);
			continue;
		}

		if ((!is_leaf_node(bh)) || (block_of_journal(fs, i))) {
			brelse(bh);
			continue;
		}

		should_be_corrupted =
		    (unsigned int)get_rand((double)0, (double)1);
		if (should_be_corrupted == 0) {
			brelse(bh);
			continue;
		}

		/* get next item, look is it a DIR */
		nr_ih_cr = get_rand(1, get_blkh_nr_items(B_BLK_HEAD(bh)));
		for (j = 0; j < nr_ih_cr; j++) {
			should_be_corrupted =
			    (unsigned int)get_rand((double)0, (double)1);
			if (should_be_corrupted == 0)
				continue;

			if ((data(fs)->log_file_name) && (data(fs)->log))
				fprintf(data(fs)->log,
					"# block %lu , item %d\n",
					bh->b_blocknr, j);
			printf("# block %lu , item %d\n", bh->b_blocknr, j);

			ih = item_head(bh, j);
			if (get_type(&ih->ih_key) != TYPE_DIRENTRY)
				continue;
			do_one_item_random_corrupt(bh, ih);
		}
		mark_buffer_dirty(bh);
		bwrite(bh);
		brelse(bh);
		nr_leaves++;
	}

	if ((data(fs)->log_file_name) && (data(fs)->log))
		fprintf(data(fs)->log,
			"DIR items in %lu leaves WERE corrupted\n", nr_leaves);
	printf("DIR items in %lu leaves WERE corrupted\n", nr_leaves);

	reiserfs_close_ondisk_bitmap(fs);
}

/* corrupt the random number of stat data items in random number of leaves */
static void do_sd_random_corrupt(reiserfs_filsys_t fs,
				 unsigned long nr_leaves_cr)
{
	unsigned int nr_ih_cr;
	unsigned int i, j;
	struct buffer_head *bh;
	struct item_head *ih;
	unsigned long nr_leaves = 0;
	unsigned int should_be_corrupted = 0;

	srand(time(NULL));

	printf("SD items in %lu leaves will be corrupted\n", nr_leaves_cr);
	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log,
			"SD items in %lu leaves will be corrupted\n",
			nr_leaves_cr);
	}

	if (reiserfs_open_ondisk_bitmap(fs) < 0)
		reiserfs_exit(1, "Could not open ondisk bitmap");

	for (i = 0; (i < get_sb_block_count(fs->fs_ondisk_sb)) &&
	     nr_leaves < nr_leaves_cr; i++) {

		if (!reiserfs_bitmap_test_bit(fs->fs_bitmap2, i))
			continue;

		bh = bread(fs->fs_dev, i, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr, "could not read block %lu\n",
					 i);
			continue;
		}

		if (who_is_this(bh->b_data, bh->b_size) != THE_LEAF) {
			brelse(bh);
			continue;
		}

		if ((!is_leaf_node(bh)) || (block_of_journal(fs, i))) {
			brelse(bh);
			continue;
		}

		should_be_corrupted =
		    (unsigned int)get_rand((double)0, (double)1);
		if (should_be_corrupted == 0) {
			brelse(bh);
			continue;
		}

		/* get next item, look is it a SD */
		nr_ih_cr = get_rand(1, get_blkh_nr_items(B_BLK_HEAD(bh)));
		for (j = 0; j < nr_ih_cr; j++) {
			should_be_corrupted =
			    (unsigned int)get_rand((double)0, (double)1);
			if (should_be_corrupted == 0)
				continue;

			if ((data(fs)->log_file_name) && (data(fs)->log))
				fprintf(data(fs)->log,
					"# block %lu , item %d\n",
					bh->b_blocknr, j);
			printf("# block %lu , item %d\n", bh->b_blocknr, j);

			ih = item_head(bh, j);
			if (get_type(&ih->ih_key) != TYPE_STAT_DATA)
				continue;
			do_one_item_random_corrupt(bh, ih);
		}
		mark_buffer_dirty(bh);
		bwrite(bh);
		brelse(bh);
		nr_leaves++;
	}

	if ((data(fs)->log_file_name) && (data(fs)->log))
		fprintf(data(fs)->log,
			"SD items in %lu leaves WERE corrupted\n", nr_leaves);
	printf("SD items in %lu leaves WERE corrupted\n", nr_leaves);

	reiserfs_close_ondisk_bitmap(fs);
}

/* corrupt the random number of indierct items in random number of leaves */
static void do_ind_random_corrupt(reiserfs_filsys_t fs,
				  unsigned long nr_leaves_cr)
{
	unsigned int nr_ih_cr;
	unsigned int i, j;
	struct buffer_head *bh;
	struct item_head *ih;
	unsigned long nr_leaves = 0;
	unsigned int should_be_corrupted = 0;

	srand(time(NULL));

	printf("IND items in %lu leaves will be corrupted\n", nr_leaves_cr);
	if ((data(fs)->log_file_name) && (data(fs)->log)) {
		fprintf(data(fs)->log,
			"IND items in %lu leaves will be corrupted\n",
			nr_leaves_cr);
	}

	if (reiserfs_open_ondisk_bitmap(fs) < 0)
		reiserfs_exit(1, "Could not open ondisk bitmap");

	for (i = 0; (i < get_sb_block_count(fs->fs_ondisk_sb)) &&
	     nr_leaves < nr_leaves_cr; i++) {

		if (!reiserfs_bitmap_test_bit(fs->fs_bitmap2, i))
			continue;

		bh = bread(fs->fs_dev, i, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr, "could not read block %lu\n",
					 i);
			continue;
		}

		if (who_is_this(bh->b_data, bh->b_size) != THE_LEAF) {
			brelse(bh);
			continue;
		}

		if ((!is_leaf_node(bh)) || (block_of_journal(fs, i))) {
			brelse(bh);
			continue;
		}

		should_be_corrupted =
		    (unsigned int)get_rand((double)0, (double)1);
		if (should_be_corrupted == 0) {
			brelse(bh);
			continue;
		}

		/* get next item, look is it an IND */
		nr_ih_cr = get_rand(1, get_blkh_nr_items(B_BLK_HEAD(bh)));
		for (j = 0; j < nr_ih_cr; j++) {
			should_be_corrupted =
			    (unsigned int)get_rand((double)0, (double)1);
			if (should_be_corrupted == 0)
				continue;

			ih = item_head(bh, j);
			if (get_type(&ih->ih_key) != TYPE_INDIRECT)
				continue;
			if ((data(fs)->log_file_name) && (data(fs)->log))
				fprintf(data(fs)->log,
					"# block %lu , item %d\n",
					bh->b_blocknr, j);
			printf("# block %lu , item %d\n", bh->b_blocknr, j);

			do_one_item_random_corrupt(bh, ih);
		}
		mark_buffer_dirty(bh);
		bwrite(bh);
		brelse(bh);
		nr_leaves++;
	}

	if ((data(fs)->log_file_name) && (data(fs)->log))
		fprintf(data(fs)->log,
			"IND items in %lu leaves WERE corrupted\n", nr_leaves);
	printf("IND items in %lu leaves WERE corrupted\n", nr_leaves);

	reiserfs_close_ondisk_bitmap(fs);
}

/* this reads list of desired corruptions from stdin and performs the
   corruptions. Format of that list:
   B           - the random number of bitmap to be corrupted
   L nr_leaves - block headers in nr_leaves leaves to be corupted
   H nr_leaves - the random number of item headers in nr_leaves to be corrupted
   S nr_leaves - the random number of stat data items in nr_leaves to be
                 corrupted
   D nr_leaves - the random number of directory items in nr_leaves to be
                 corrupted
   I nr_leaves - the random number of indirect items in nr_leaves to be
                 corrupted
*/

static void what_to_corrupt(reiserfs_filsys_t fs, char *corruption_command)
{
	unsigned long nr_leaves_cr;
	char code;

	switch (corruption_command[0]) {
	case 'B':
		/* bitmap */
		do_bitmap_corruption(fs);
		break;

	case 'L':
		/* leaves */
		if (sscanf(corruption_command, "%c %lu\n", &code, &nr_leaves_cr)
		    != 2) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		do_leaves_corruption(fs, nr_leaves_cr);
		break;

	case 'H':
		/* item headers */
		if (sscanf(corruption_command, "%c %lu\n", &code, &nr_leaves_cr)
		    != 2) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		do_ih_random_corrupt(fs, nr_leaves_cr);
		break;

	case 'D':
		/* directory items */
		if (sscanf(corruption_command, "%c %lu\n", &code, &nr_leaves_cr)
		    != 2) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		do_dir_random_corrupt(fs, nr_leaves_cr);
		break;

	case 'S':
		/* stat data items */
		if (sscanf(corruption_command, "%c %lu\n", &code, &nr_leaves_cr)
		    != 2) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		do_sd_random_corrupt(fs, nr_leaves_cr);
		break;

	case 'I':
		/* indirect items */
		if (sscanf(corruption_command, "%c %lu\n", &code, &nr_leaves_cr)
		    != 2) {
			printf("Wrong format \'%c\'\n", corruption_command[0]);
			return;
		}
		do_ind_random_corrupt(fs, nr_leaves_cr);
		break;

	default:
		printf("Unknown command specified\n");
	}
}

void do_fs_random_corrupt(reiserfs_filsys_t fs)
{
	char *line = NULL;
	size_t n = 0;

	printf("Corrupting fs. Please insert one of the following command\n"
	       " B - the random number of bitmap to be corrupted\n"
	       " L nr_leaves - block headers in nr_leaves leaves to be corupted\n"
	       " H nr_leaves - the random number of item headers in nr_leaves to be corrupted\n"
	       " S nr_leaves - the random number of stat data items in nr_leaves to be corrupted\n"
	       " D nr_leaves - the random number of directory items in nr_leaves to be corrupted\n"
	       " I nr_leaves - the random number of indirect items in nr_leaves to be corrupted\n"
	       ".. ->\n");

	/* Get list of corruptions from stdin */
	while (getline(&line, &n, stdin) != -1) {
		if (line[0] == '\n') {
			free(line);
			line = NULL;
			n = 0;
			break;
		}
		printf("################## command : %s", line);
		if ((data(fs)->log_file_name) && (data(fs)->log)) {
			fprintf(data(fs)->log,
				"################## command : %s", line);
		}

		what_to_corrupt(fs, line);

		free(line);
		line = NULL;
		n = 0;
	}
}

/*
   Local variables:
   c-indentation-style: "K&R"
   mode-name: "LC"
   c-basic-offset: 4
   tab-width: 4
   fill-column: 80
   End:
*/
