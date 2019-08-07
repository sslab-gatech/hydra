/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "debugreiserfs.h"

/* counters for each kind of blocks */
unsigned int packed, packed_leaves, full_blocks, having_ih_array,	/* blocks with broken block head */
 bad_leaves,			/* failed to compress */
 internals, descs, others;

reiserfs_bitmap_t *what_to_pack;

/* these are to calculate compression */
unsigned long sent_bytes;	/* how many bytes sent to stdout */
unsigned long had_to_be_sent;	/* how many bytes were to be sent */

static void pack_ih(struct packed_item *pi, struct item_head *ih)
{
	__u32 v32;
	__u16 v16;

	/* send packed item head first */
	fwrite(pi, sizeof(*pi), 1, stdout);
	sent_bytes += sizeof(*pi);

	/* sen key components which are to be sent */
	if (get_pi_mask(pi) & DIR_ID) {
		v32 = get_key_dirid(&ih->ih_key);
		fwrite_le32(&v32);
	}

	if (get_pi_mask(pi) & OBJECT_ID) {
		v32 = get_key_objectid(&ih->ih_key);
		fwrite_le32(&v32);
	}

	if (get_pi_mask(pi) & OFFSET_BITS_64) {
		__u64 offset;

		offset = get_offset(&ih->ih_key);
		fwrite_le64(&offset);
	}

	if (get_pi_mask(pi) & OFFSET_BITS_32) {
		__u32 offset;

		offset = get_offset(&ih->ih_key);
		fwrite_le32(&offset);
	}

	if (get_pi_mask(pi) & IH_FREE_SPACE) {
		v16 = get_ih_entry_count(ih);
		fwrite_le16(&v16);
	}

	if (get_pi_mask(pi) & IH_FORMAT) {
		/* fixme */
		fwrite16(&ih->ih_format);
	}
}

static void pack_direct(struct packed_item *pi, struct buffer_head *bh,
			struct item_head *ih)
{
	if (get_ih_free_space(ih) != 0xffff)
		/* ih_free_space has unexpected value */
		set_pi_mask(pi, get_pi_mask(pi) | IH_FREE_SPACE);

	if (get_pi_mask(pi) & SAFE_LINK)
		set_key_dirid(&ih->ih_key,
			      d32_get((__le32 *) ih_item_body(bh, ih), 0));

	/* send key components which are to be sent */
	pack_ih(pi, ih);
}

/* if there is at least one extent longer than 2 - it is worth packing */
static int should_pack_indirect(__le32 * ind_item, int unfm_num)
{
	int i, len;

	for (i = 1, len = 1; i < unfm_num; i++) {
		if ((d32_get(ind_item, i) == 0 && d32_get(ind_item, i - 1) == 0)
		    || d32_get(ind_item, i) == d32_get(ind_item, i - 1) + 1) {
			len++;
			if (len > 2)
				return 1;
		} else {
			/* sequence of blocks or hole broke */
			len = 1;
		}
	}
	return 0;
}

/* indirect item can be either packed using "extents" (when it is
   worth doing) or be stored as is. Size of item in packed form is not
   stored. Unpacking will stop when full item length is reached */
static void pack_indirect(struct packed_item *pi, struct buffer_head *bh,
			  struct item_head *ih)
{
	unsigned int i;
	__le32 *ind_item;
	__u16 len;

	if (get_ih_entry_count(ih))
		set_pi_mask(pi, get_pi_mask(pi) | IH_FREE_SPACE);

	ind_item = (__le32 *) ih_item_body(bh, ih);
	if (!should_pack_indirect(ind_item, I_UNFM_NUM(ih)))
		set_pi_mask(pi, get_pi_mask(pi) | WHOLE_INDIRECT);

	if (get_pi_mask(pi) & SAFE_LINK)
		set_key_dirid(&ih->ih_key, d32_get(ind_item, 0));

	pack_ih(pi, ih);

	if (get_pi_mask(pi) & SAFE_LINK)
		return;

	if (get_pi_mask(pi) & WHOLE_INDIRECT) {
		fwrite(ind_item, get_ih_item_len(ih), 1, stdout);
		sent_bytes += get_ih_item_len(ih);
		return;
	}

	fwrite32(&ind_item[0]);

	for (i = 1, len = 1; i < I_UNFM_NUM(ih); i++) {
		if ((d32_get(ind_item, i) == 0 && d32_get(ind_item, i - 1) == 0)
		    || d32_get(ind_item, i) == d32_get(ind_item, i - 1) + 1) {
			len++;
		} else {
			fwrite_le16(&len);
			fwrite32((char *)(ind_item + i));
			len = 1;
		}
	}
	fwrite_le16(&len);

	return;
}

/* directory item is packed:
   entry count - 16 bits
   for each entry
   	mask (8 bits) - it shows whether there are any of (deh_dir_id, gen counter, deh_state)
	entry length 16 bits
	entry itself
	deh_objectid - 32 bits
		maybe deh_dir_id (32 bits)
		maybe gencounter (16)
		maybe deh_state (16)
*/
static void pack_direntry(reiserfs_filsys_t fs, struct packed_item *pi,
			  struct buffer_head *bh, struct item_head *ih)
{
	int i;
	struct reiserfs_de_head *deh;
	struct packed_dir_entry pe;
	__u16 entry_count, gen_counter;

	set_pi_mask(pi, get_pi_mask(pi) | IH_ENTRY_COUNT);

	/* send item_head components which are to be sent */
	pack_ih(pi, ih);

	/* entry count is sent unconditionally */
	entry_count = get_ih_entry_count(ih);

	deh = B_I_DEH(bh, ih);
	for (i = 0; i < entry_count; i++, deh++) {
		pe.entrylen = entry_length(ih, deh, i);
		pe.mask = 0;
		if (get_deh_dirid(deh) != get_key_objectid(&ih->ih_key))
			/* entry points to name of another directory, store deh_dir_id */
			pe.mask |= HAS_DIR_ID;

		gen_counter = GET_GENERATION_NUMBER(get_deh_offset(deh));
		if (gen_counter != 0)
			/* store generation counter if it is != 0 */
			pe.mask |= HAS_GEN_COUNTER;

		if (get_deh_state(deh) != 4)
			/* something unusual in deh_state. Store it */
			pe.mask |= HAS_STATE;

		fwrite8(&pe.mask);
		fwrite_le16(&pe.entrylen);
		fwrite(name_in_entry(deh, i), pe.entrylen, 1, stdout);
		sent_bytes += pe.entrylen;
		fwrite32(&(deh->deh2_objectid));

		if (pe.mask & HAS_DIR_ID)
			fwrite32(&deh->deh2_dir_id);

		if (pe.mask & HAS_GEN_COUNTER)
			fwrite_le16(&gen_counter);

		if (pe.mask & HAS_STATE)
			fwrite16(&deh->deh2_state);
	}
}

static void pack_stat_data(struct packed_item *pi, struct buffer_head *bh,
			   struct item_head *ih)
{
	if (get_ih_free_space(ih) != 0xffff)
		/* ih_free_space has unexpected value */
		set_pi_mask(pi, get_pi_mask(pi) | IH_FREE_SPACE);

	if (stat_data_v1(ih)) {
		/* for old stat data: we take
		   mode - 16 bits
		   nlink - 16 bits
		   size - 32 bits
		   blocks/rdev - 32 bits
		   maybe first_direct byte 32 bits
		 */
		struct stat_data_v1 *sd_v1;

		sd_v1 = (struct stat_data_v1 *)ih_item_body(bh, ih);
		if (sd_v1->sd_first_direct_byte != 0xffffffff)	/* ok if -1 */
			set_pi_mask(pi,
				    get_pi_mask(pi) |
				    WITH_SD_FIRST_DIRECT_BYTE);

		pack_ih(pi, ih);

		fwrite16(&sd_v1->sd_mode);
		fwrite16(&sd_v1->sd_nlink);
		fwrite32(&sd_v1->sd_size);
		fwrite32(&sd_v1->u.sd_blocks);

		if (get_pi_mask(pi) & WITH_SD_FIRST_DIRECT_BYTE)
			fwrite32(&sd_v1->sd_first_direct_byte);
	} else {
		/* for new stat data
		   mode - 16 bits
		   nlink in either 16 or 32 bits
		   size in either 32 or 64 bits
		   blocks - 32 bits
		 */
		struct stat_data *sd;
		/* these will maintain disk-order values */
		__le16 nlink16;
		__le32 nlink32, size32;
		__u64 size64;

		sd = (struct stat_data *)ih_item_body(bh, ih);
		if (sd_v2_nlink(sd) > 0xffff) {
			set_pi_mask(pi, get_pi_mask(pi) | NLINK_BITS_32);
			nlink32 = sd->sd_nlink;
		} else {
			/* This is required to deal with big endian systems */
			nlink16 = cpu_to_le16((__u16) sd_v2_nlink(sd));
		}
		if (sd_v2_size(sd) > 0xffffffff) {
			set_pi_mask(pi, get_pi_mask(pi) | SIZE_BITS_64);
			size64 = sd->sd_size;
		} else {
			/* This is required to deal with big endian systems */
			size32 = cpu_to_le32((__u32) sd_v2_size(sd));
		}

		pack_ih(pi, ih);

		fwrite16(&sd->sd_mode);

		if (get_pi_mask(pi) & NLINK_BITS_32) {
			fwrite32(&nlink32);
		} else {
			fwrite16(&nlink16);
		}

		if (get_pi_mask(pi) & SIZE_BITS_64) {
			fwrite64(&size64);
		} else {
			fwrite32(&size32);
		}

		fwrite32(&sd->sd_blocks);
	}
}

static void pack_full_block(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	__u16 magic;
	__u32 block;

	magic = FULL_BLOCK_START_MAGIC;
	fwrite_le16(&magic);

	block = bh->b_blocknr;
	fwrite_le32(&block);

	fwrite(bh->b_data, fs->fs_blocksize, 1, stdout);

	sent_bytes += fs->fs_blocksize;
	had_to_be_sent += fs->fs_blocksize;

	full_blocks++;
}

#if 0
/* unformatted node pointer is considered bad when it points either to blocks
   of journal, bitmap blocks, super block or is transparently out of range of
   disk block numbers */
static int check_unfm_ptr(reiserfs_filsys_t fs, __u32 block)
{
	if (block >= SB_BLOCK_COUNT(fs))
		return 1;

	if (not_data_block(fs, block))
		return 1;

	return 0;
}
#endif

/* we only pack leaves which do not have any corruptions */
static int can_pack_leaf(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	int i;
	struct item_head *ih;

	ih = item_head(bh, 0);
	for (i = 0; i < get_blkh_nr_items(B_BLK_HEAD(bh)); i++, ih++) {
		if (is_it_bad_item
		    (fs, ih, ih_item_body(bh, ih), 0 /*check_unfm_ptr */ ,
		     1 /*bad dir */ ))
			return 0;
	}
	return 1;
}

/* pack leaf only if all its items are correct: keys are correct,
   direntries are hashed properly and hash function is defined,
   indirect items are correct, stat data ?, */
static void pack_leaf(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	int i;
	struct item_head *ih;
	struct packed_item pi;
	__u16 v16;

	if (!can_pack_leaf(fs, bh)) {
		/* something looks suspicious in this leaf - pack whole block */
		bad_leaves++;
		pack_full_block(fs, bh);
		return;
	}

	/* start magic in low 8 bits, hash code in high 8 bits */
	v16 = (LEAF_START_MAGIC | (func2code(fs->fs_hash_function) << 8));
	fwrite_le16(&v16);

	/* block number */
	fwrite_le32(&bh->b_blocknr);

	/* item number */
	v16 = get_blkh_nr_items(B_BLK_HEAD(bh));
	fwrite_le16(&v16);

	ih = item_head(bh, 0);

	for (i = 0; i < v16; i++, ih++) {
#if 0
		v32 = ITEM_START_MAGIC;
		fwrite32(&v32);
#endif

		set_pi_mask(&pi, 0);
		set_pi_item_len(&pi, get_ih_item_len(ih));
		set_pi_type(&pi, get_type(&ih->ih_key));

		// format
		if (get_ih_key_format(ih) == KEY_FORMAT_2)
			set_pi_mask(&pi, get_pi_mask(&pi) | NEW_FORMAT);

		// k_dir_id
		if (!i
		    || (i
			&& get_key_dirid(&ih->ih_key) !=
			get_key_dirid(&(ih - 1)->ih_key))) {
			/* if item is first in the leaf or if previous item has different
			   k_dir_id - store it */
			set_pi_mask(&pi, get_pi_mask(&pi) | DIR_ID);
		}
		// k_object_id
		if (!i
		    || (i
			&& get_key_objectid(&ih->ih_key) !=
			get_key_objectid(&(ih - 1)->ih_key))) {
			/* if item is first in the leaf or if previous item has different
			   k_objectid - store it */
			set_pi_mask(&pi, get_pi_mask(&pi) | OBJECT_ID);
		}

		/* store offset if it is != 0 in 32 or 64 bits */
		if (get_offset(&ih->ih_key)) {
			int send_offset = 1;

			if ((get_pi_mask(&pi) & DIR_ID) == 0
			    && (get_pi_mask(&pi) & OBJECT_ID) == 0) {
				/* previous item is of the same object, so try to avoid
				   sending k_offset */
				if ((is_stat_data_ih(ih - 1)
				     && get_offset(&ih->ih_key) == 1)
				    || (is_indirect_ih(ih - 1)
					&& is_direct_ih(ih)
					&& get_offset(&(ih - 1)->ih_key) +
					get_bytes_number(ih - 1,
							 fs->fs_blocksize) ==
					get_offset(&ih->ih_key)))
					/* unpack can calculate offset itself */
					send_offset = 0;
			}
			if (send_offset) {
				if (get_offset(&ih->ih_key) > 0xffffffffULL)
					set_pi_mask(&pi,
						    get_pi_mask(&pi) |
						    OFFSET_BITS_64);
				else
					set_pi_mask(&pi,
						    get_pi_mask(&pi) |
						    OFFSET_BITS_32);
			}
		}

		/* ih key format is correct, check fsck_need field */
		if (get_ih_flags(ih))
			set_pi_mask(&pi, get_pi_mask(&pi) | IH_FORMAT);

		if ((get_key_dirid(&ih->ih_key) == (__u32) - 1)
		    && (get_ih_item_len(ih) == 4))
			set_pi_mask(&pi, get_pi_mask(&pi) | SAFE_LINK);

		if (is_direct_ih(ih)) {
			pack_direct(&pi, bh, ih);
		} else if (is_indirect_ih(ih))
			pack_indirect(&pi, bh, ih);
		else if (is_direntry_ih(ih))
			pack_direntry(fs, &pi, bh, ih);
		else if (is_stat_data_ih(ih))
			pack_stat_data(&pi, bh, ih);
		else
			die("pack_leaf: unknown item found");
#if 0
		v32 = ITEM_END_MAGIC;
		fwrite32(&v32);
#endif
	}

	v16 = LEAF_END_MAGIC;
	fwrite_le16(&v16);

	had_to_be_sent += fs->fs_blocksize;
	packed_leaves++;

	return;
}

static int can_pack_internal(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	return 0;
}

/* pack internal node as a full block */
static void pack_internal(reiserfs_filsys_t fs, struct buffer_head *bh)
{
	internals++;
	if (!can_pack_internal(fs, bh)) {
		pack_full_block(fs, bh);
		return;
	}

	reiserfs_panic("pack_internal: packing code is not ready");
}

/* packed blocks are marked free in the bitmap*/
static void send_block(reiserfs_filsys_t fs, struct buffer_head *bh,
		       int send_unknown)
{
	int type;

	packed++;
	type = who_is_this(bh->b_data, bh->b_size);
	switch (type) {
	case THE_LEAF:
		pack_leaf(fs, bh);
		break;

	case HAS_IH_ARRAY:
		having_ih_array++;
//      fprintf (stderr, "BROKEN BLOCK HEAD %lu\n", bh->b_blocknr);
		pack_full_block(fs, bh);
		break;

	case THE_INTERNAL:
		pack_internal(fs, bh);
		break;

	default:
		if (send_unknown)
			pack_full_block(fs, bh);
		else
			packed--;
		break;
	}

	/* do not send one block twice */
	reiserfs_bitmap_clear_bit(what_to_pack, bh->b_blocknr);
}

/* super block, journal, bitmaps */
static void pack_frozen_data(reiserfs_filsys_t fs)
{
	struct buffer_head *bh;
	unsigned long block;
	__u16 magic16;
	int sent_journal_start_magic = 0;
	unsigned int i;

	if (is_reiserfs_jr_magic_string(fs->fs_ondisk_sb) &&
	    get_jp_journal_dev(sb_jp(fs->fs_ondisk_sb)) &&
	    !journal_device_name(fs)) {
		if (!user_confirmed(stderr,
				    "\n File system has non-standard journal "
				    "that hasn't been specified.\n"
				    "Continue packing without journal? [N/Yes] (note need to type Yes):",
				    "Yes\n"))
			exit(0);
	}

	/* super block */
	reiserfs_warning(stderr, "super block..");
	fflush(stderr);
	send_block(fs, fs->fs_super_bh,
		   1 /*send block even if its format is not determined */ );
	reiserfs_warning(stderr, "ok\nbitmaps..(%d).. ",
			 reiserfs_fs_bmap_nr(fs));
	fflush(stderr);

	/* bitmaps */
	block = fs->fs_super_bh->b_blocknr + 1;
	for (i = 0; i < reiserfs_fs_bmap_nr(fs); i++) {
		bh = bread(fs->fs_dev, block, fs->fs_blocksize);
		if (!bh) {
			fprintf(stderr, "pack_frozen_data: bread failed: %lu\n",
				block);
			continue;
		}
		send_block(fs, bh, 1);
		if (spread_bitmaps(fs))
			block =
			    (block / (fs->fs_blocksize * 8) +
			     1) * (fs->fs_blocksize * 8);
		else
			block++;
		brelse(bh);
	}

	/* journal */

	if (get_jp_journal_dev(sb_jp(fs->fs_ondisk_sb))) {
		/* non-standard journal is on a separate device */

		if (journal_device_name(fs) && !reiserfs_journal_opened(fs))
			die("Specified journal is not available. Specify it correctly or " "don't specify at all");
		else if (!journal_device_name(fs))
			/* non-standard journal was not specified (that confirmed by user) -
			   skipped packing journal */
			return;
		else {
			magic16 = SEPARATED_JOURNAL_START_MAGIC;
			fwrite_le16(&magic16);
			sent_journal_start_magic = 1;
		}
	}
	block = get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb));
	reiserfs_warning(stderr, "ok\njournal (from %lu to %lu)..",
			 block,
			 block + get_jp_journal_size(sb_jp(fs->fs_ondisk_sb)));
	fflush(stderr);
	for (i = 0; i <= get_jp_journal_size(sb_jp(fs->fs_ondisk_sb)); i++) {
		bh = bread(fs->fs_journal_dev, block + i, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr,
					 "could not read %lu, skipped\n", i);
			continue;
		}

		send_block(fs, bh, 1);
		brelse(bh);
	}
	if (sent_journal_start_magic) {
		magic16 = SEPARATED_JOURNAL_END_MAGIC;
		fwrite_le16(&magic16);
	}
	reiserfs_warning(stderr, "ok\n");
	fflush(stderr);
	reiserfs_warning(stderr,
			 "Super block, bitmaps, journal - %u blocks - done, %u blocks left\n",
			 packed, reiserfs_bitmap_ones(what_to_pack));
}

static void save_ino(reiserfs_filsys_t fs, struct buffer_head *bh,
		       int send_unknown, int fd)
{
	int type;
    __u64 blockno = bh->b_blocknr;

	packed++;
	type = who_is_this(bh->b_data, bh->b_size);
	switch (type) {
	case THE_LEAF:
        if (write(fd, &blockno, sizeof(__u64)) != sizeof(__u64))
            die("fail to write block nr\n");
		break;

	case HAS_IH_ARRAY:
		having_ih_array++;
        if (write(fd, &blockno, sizeof(__u64)) != sizeof(__u64))
            die("fail to write block nr\n");
		break;

	case THE_INTERNAL:
        if (write(fd, &blockno, sizeof(__u64)) != sizeof(__u64))
            die("fail to write block nr\n");
		break;

	default:
		if (send_unknown) {
            if (write(fd, &blockno, sizeof(__u64)) != sizeof(__u64))
                die("fail to write block nr\n");
        }
		else
			packed--;
		break;
	}

	/* do not send one block twice */
	reiserfs_bitmap_clear_bit(what_to_pack, bh->b_blocknr);
}

/* super block, journal, bitmaps */
static void ino_frozen_data(reiserfs_filsys_t fs, int fd)
{
	struct buffer_head *bh;
	unsigned long block;
	unsigned int i;

	/* super block */
	reiserfs_warning(stderr, "super block..");
	fflush(stderr);
	save_ino(fs, fs->fs_super_bh,
		   1 /*send block even if its format is not determined */,
           fd);
	reiserfs_warning(stderr, "ok\nbitmaps..(%d).. ",
			 reiserfs_fs_bmap_nr(fs));
	fflush(stderr);

	/* bitmaps */
	block = fs->fs_super_bh->b_blocknr + 1;
	for (i = 0; i < reiserfs_fs_bmap_nr(fs); i++) {
		bh = bread(fs->fs_dev, block, fs->fs_blocksize);
		if (!bh) {
			fprintf(stderr, "pack_frozen_data: bread failed: %lu\n",
				block);
			continue;
		}
		save_ino(fs, bh, 1, fd);

		if (spread_bitmaps(fs))
			block =
			    (block / (fs->fs_blocksize * 8) +
			     1) * (fs->fs_blocksize * 8);
		else
			block++;
		brelse(bh);
	}

	/* journal */
	block = get_jp_journal_1st_block(sb_jp(fs->fs_ondisk_sb));
	reiserfs_warning(stderr, "ok\njournal (from %lu to %lu)..",
			 block,
			 block + get_jp_journal_size(sb_jp(fs->fs_ondisk_sb)));
	fflush(stderr);
	for (i = 0; i <= get_jp_journal_size(sb_jp(fs->fs_ondisk_sb)); i++) {
		bh = bread(fs->fs_journal_dev, block + i, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr,
					 "could not read %lu, skipped\n", i);
			continue;
		}

		save_ino(fs, bh, 1, fd);
		brelse(bh);

        if (i > 0x10)
            break;
	}

	reiserfs_warning(stderr, "ok\n");
	fflush(stderr);
	reiserfs_warning(stderr,
			 "Super block, bitmaps, journal - %u blocks - done, %u blocks left\n",
			 packed, reiserfs_bitmap_ones(what_to_pack));
}

void ino_partition(reiserfs_filsys_t fs, int fd)
{
	struct buffer_head *bh;
	__u32 blocksize;
	unsigned long done = 0, total;
	unsigned int i;

	blocksize = fs->fs_blocksize;
    printf("blocksize: %d\n", blocksize);
    write(fd, &blocksize, sizeof(__u32));

	/* will get information about what is to be packed. Bits corresponding to
	   packed blocks will be cleared */
	what_to_pack = input_bitmap(fs);

	/* super block, journal, bitmaps */
	ino_frozen_data(fs, fd);

	/* what's left */
	total = reiserfs_bitmap_ones(what_to_pack);

	for (i = 0; i < get_sb_block_count(fs->fs_ondisk_sb); i++) {
		if (!reiserfs_bitmap_test_bit(what_to_pack, i))
			continue;

		print_how_far(stderr, &done, total, 1, be_quiet(fs));

		bh = bread(fs->fs_dev, i, blocksize);
		if (!bh) {
			reiserfs_warning(stderr, "could not read block %lu\n",
					 i);
			continue;
		}

		save_ino(fs, bh, 0, fd);
		brelse(bh);
	}
}

/* pack all "not data blocks" and correct leaf */
void pack_partition(reiserfs_filsys_t fs)
{
	struct buffer_head *bh;
	__u32 magic32;
	__u16 blocksize;
	__u16 magic16;
	unsigned long done = 0, total;
	unsigned int i;

	magic32 = REISERFS_SUPER_MAGIC;
	fwrite_le32(&magic32);

	blocksize = fs->fs_blocksize;
	fwrite_le16(&blocksize);

	/* will get information about what is to be packed. Bits corresponding to
	   packed blocks will be cleared */
	what_to_pack = input_bitmap(fs);

	/* super block, journal, bitmaps */
	pack_frozen_data(fs);

	/* what's left */
	total = reiserfs_bitmap_ones(what_to_pack);

	for (i = 0; i < get_sb_block_count(fs->fs_ondisk_sb); i++) {
		if (!reiserfs_bitmap_test_bit(what_to_pack, i))
			continue;

		print_how_far(stderr, &done, total, 1, be_quiet(fs));

		bh = bread(fs->fs_dev, i, blocksize);
		if (!bh) {
			reiserfs_warning(stderr, "could not read block %lu\n",
					 i);
			continue;
		}

		send_block(fs, bh,
			   0 /*do not send block of not determined format */ );
		brelse(bh);
	}

	magic16 = END_MAGIC;
	fwrite_le16(&magic16);

	fprintf(stderr, "\nPacked %u blocks:\n"
		"\tcompessed %u\n"
		"\tfull blocks %u\n"
		"\t\tleaves with broken block head %u\n"
		"\t\tcorrupted leaves %u\n"
		"\t\tinternals %u\n"
		"\t\tdescriptors %u\n",
		packed,
		packed_leaves, full_blocks, having_ih_array,
		bad_leaves, internals, descs);

	fprintf(stderr, "data packed with ratio %.2f\n",
		(double)sent_bytes / had_to_be_sent);
}

void pack_one_block(reiserfs_filsys_t fs, unsigned long block)
{
	__u32 magic32;
	__u16 magic16;
	struct buffer_head *bh;

	// reiserfs magic
	magic32 = REISERFS_SUPER_MAGIC;
	fwrite_le32(&magic32);

	// blocksize
	fwrite_le16(&fs->fs_blocksize);

	bh = bread(fs->fs_dev, block, fs->fs_blocksize);

	if (!bh)
		return;

	if (who_is_this(bh->b_data, bh->b_size) == THE_LEAF)
		pack_leaf(fs, bh);
	else
		pack_full_block(fs, bh);

	brelse(bh);

	// end magic
	magic16 = END_MAGIC;
	fwrite_le16(&magic16);

	fprintf(stderr, "Done\n");
}
