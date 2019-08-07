/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

/*
 * 2000/10/26 - Initial version.
 */

#include "includes.h"
#include <assert.h>

/* create clean bitmap */
reiserfs_bitmap_t *reiserfs_create_bitmap(unsigned int bit_count)
{
	reiserfs_bitmap_t *bm;

	bm = getmem(sizeof(*bm));
	if (!bm)
		return NULL;
	bm->bm_bit_size = bit_count;
	bm->bm_byte_size = ((unsigned long long)bit_count + 7) / 8;
	bm->bm_set_bits = 0;
	bm->bm_map = getmem(bm->bm_byte_size);
	if (!bm->bm_map) {
		freemem(bm);
		return NULL;
	}

	return bm;
}

/* Expand existing bitmap.  Return non-zero if can't. FIXME: it is
   assumed that bit_count is new number of blocks to be addressed */
int reiserfs_expand_bitmap(reiserfs_bitmap_t *bm, unsigned int bit_count)
{
	unsigned int byte_count = ((bit_count + 7) / 8);
	char *new_map;

	new_map = expandmem(bm->bm_map, bm->bm_byte_size,
			    byte_count - bm->bm_byte_size);

	if (!new_map) {
		return 1;
	}

	bm->bm_map = new_map;
	bm->bm_byte_size = byte_count;
	bm->bm_bit_size = bit_count;

	bm->bm_dirty = 1;

	return 0;
}

void reiserfs_shrink_bitmap(reiserfs_bitmap_t *bm, unsigned int bit_count)
{
	unsigned long i;

	assert(bm->bm_bit_size >= bit_count);

	bm->bm_byte_size = (bit_count + 7) / 8;
	bm->bm_bit_size = bit_count;
	bm->bm_set_bits = 0;

	bm->bm_dirty = 1;

	for (i = 0; i < bm->bm_bit_size; i++) {
		if (reiserfs_bitmap_test_bit(bm, i))
			bm->bm_set_bits++;
	}
}

/* bitmap destructor */
void reiserfs_delete_bitmap(reiserfs_bitmap_t *bm)
{
	freemem(bm->bm_map);
	bm->bm_map = NULL;	/* to not reuse bitmap handle */
	bm->bm_bit_size = 0;
	bm->bm_byte_size = 0;
	freemem(bm);
}

void reiserfs_bitmap_copy(reiserfs_bitmap_t *to, reiserfs_bitmap_t *from)
{
	assert(to->bm_byte_size == from->bm_byte_size);
	memcpy(to->bm_map, from->bm_map, from->bm_byte_size);
	to->bm_bit_size = from->bm_bit_size;
	to->bm_set_bits = from->bm_set_bits;
	to->bm_dirty = 1;
}

int reiserfs_bitmap_compare(reiserfs_bitmap_t *bm1, reiserfs_bitmap_t *bm2)
{
	unsigned long i, diff;
	unsigned long int bytes, bits;

	assert(bm1->bm_byte_size == bm2->bm_byte_size &&
	       bm1->bm_bit_size == bm2->bm_bit_size);

	diff = 0;

	/* compare full bytes */
	bytes = bm1->bm_bit_size / 8;
	bits = bytes * 8;
	if (memcmp(bm1->bm_map, bm2->bm_map, bytes)) {
		for (i = 0; i < bits; i++)
			if (reiserfs_bitmap_test_bit(bm1, i) !=
			    reiserfs_bitmap_test_bit(bm2, i))
				diff++;
	}

	/* compare last byte of bitmap which can be used partially */
	bits = bm1->bm_bit_size % 8;
	for (i = bm1->bm_bit_size / 8 * 8; i < bm1->bm_bit_size / 8 * 8 + bits;
	     i++)
		if (reiserfs_bitmap_test_bit(bm1, i) !=
		    reiserfs_bitmap_test_bit(bm2, i))
			diff++;

/*	int mask;

	mask = 255 >> (8 - bits);
	if ((bm1->bm_map [bytes] & mask) != (bm2->bm_map [bytes] & mask)) {
	    diff ++;
	}
    }*/
	return diff;
}

/*
    Does X | Y for every bit of the bitmap `to`, where
    X - bit of the `to` bitmap,
    Y - `from` bitmap.
    Save result in the `to` bitmap.
*/
void reiserfs_bitmap_disjunction(reiserfs_bitmap_t *to,
				 reiserfs_bitmap_t *from)
{
	unsigned int i;

	assert(to->bm_byte_size == from->bm_byte_size &&
	       to->bm_bit_size == from->bm_bit_size);

	for (i = 0; i < to->bm_bit_size; i++) {
		if (misc_test_bit(i, from->bm_map)
		    && !misc_test_bit(i, to->bm_map)) {
			misc_set_bit(i, to->bm_map);
			to->bm_set_bits++;
			to->bm_dirty = 1;
		}
	}
}

/*
    Does X & !Y for every bit of the bitmap `base`, where
    X - bit of the `base` bitmap,
    Y - `exclude` bitmap.
    Save result in the `base` bitmap.
*/
void reiserfs_bitmap_delta(reiserfs_bitmap_t *base,
			   reiserfs_bitmap_t *exclude)
{
	unsigned int i;

	assert(base->bm_byte_size == exclude->bm_byte_size &&
	       base->bm_bit_size == exclude->bm_bit_size);

	for (i = 0; i < base->bm_bit_size; i++) {
		if (misc_test_bit(i, exclude->bm_map)
		    && misc_test_bit(i, base->bm_map)) {
			misc_clear_bit(i, base->bm_map);
			base->bm_set_bits--;
			base->bm_dirty = 1;
		}
	}
}

void reiserfs_bitmap_set_bit(reiserfs_bitmap_t *bm, unsigned int bit_number)
{
	assert(bit_number < bm->bm_bit_size);
	if (misc_test_bit(bit_number, bm->bm_map))
		return;
	misc_set_bit(bit_number, bm->bm_map);
	bm->bm_set_bits++;
	bm->bm_dirty = 1;
}

void reiserfs_bitmap_clear_bit(reiserfs_bitmap_t *bm, unsigned int bit_number)
{
	assert(bit_number < bm->bm_bit_size);
	if (!misc_test_bit(bit_number, bm->bm_map))
		return;
	misc_clear_bit(bit_number, bm->bm_map);
	bm->bm_set_bits--;
	bm->bm_dirty = 1;
}

int reiserfs_bitmap_test_bit(reiserfs_bitmap_t *bm, unsigned int bit_number)
{
	if (bit_number >= bm->bm_bit_size)
		printf("bit %u, bitsize %lu\n", bit_number, bm->bm_bit_size);
	assert(bit_number < bm->bm_bit_size);
	return misc_test_bit(bit_number, bm->bm_map);
}

unsigned int reiserfs_bitmap_zeros(reiserfs_bitmap_t *bm)
{
	return bm->bm_bit_size - bm->bm_set_bits;
}

unsigned int reiserfs_bitmap_ones(reiserfs_bitmap_t *bm)
{
	return bm->bm_set_bits;
}

int reiserfs_bitmap_find_zero_bit(reiserfs_bitmap_t *bm, unsigned long *first)
{
	unsigned long bit_nr = *first;
	assert(*first < bm->bm_bit_size);

	bit_nr = misc_find_next_zero_bit(bm->bm_map, bm->bm_bit_size, *first);

	if (bit_nr >= bm->bm_bit_size) {	/* search failed */
		return 1;
	}

	*first = bit_nr;
	return 0;
}

/* read every bitmap block and copy their content into bitmap 'bm' */
static int reiserfs_fetch_ondisk_bitmap(reiserfs_bitmap_t *bm,
					reiserfs_filsys_t fs)
{
	unsigned int last_byte_unused_bits;
	unsigned long block, to_copy;
	struct buffer_head *bh;
	unsigned int i;
	int copied;
	int ret = 0;
	char *p;

	to_copy = (get_sb_block_count(fs->fs_ondisk_sb) + 7) / 8;

	/*reiserfs_warning (stderr, "Fetching on-disk bitmap.."); */
	assert(bm->bm_byte_size == to_copy);

	copied = fs->fs_blocksize;
	p = bm->bm_map;
	block = fs->fs_super_bh->b_blocknr + 1;

	while (to_copy) {
		bh = bread(fs->fs_dev, block, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr,
					 "reiserfs_fetch_ondisk_bitmap: "
					 "bread failed reading bitmap (%lu)\n",
					 block);

			bh = getblk(fs->fs_dev, block, fs->fs_blocksize);
			if (!bh) {
				reiserfs_warning(stderr, "%s: getblk failed",
						 __func__);
				return -1;
			}

			memset(bh->b_data, 0xff, bh->b_size);
			mark_buffer_uptodate(bh, 1);
		}

		if (to_copy < fs->fs_blocksize) {
			for (i = to_copy; i < fs->fs_blocksize; i++) {
				if (bh->b_data[i] != (char)0xff) {
					ret = 1;
					break;
				}
			}

			copied = to_copy;
		}
		memcpy(p, bh->b_data, copied);
		brelse(bh);
		p += copied;
		to_copy -= copied;

		/* next bitmap block */
		if (spread_bitmaps(fs))
			block =
			    (block / (fs->fs_blocksize * 8) +
			     1) * (fs->fs_blocksize * 8);
		else
			block++;
	}

	/* on disk bitmap has bits out of SB_BLOCK_COUNT set to 1, where as
	   reiserfs_bitmap_t has those bits set to 0 */
	last_byte_unused_bits = bm->bm_byte_size * 8 - bm->bm_bit_size;

	for (i = 0; i < last_byte_unused_bits; i++) {
		if (misc_test_bit(bm->bm_bit_size + i, bm->bm_map) == 0)
			ret = 1;
		else
			misc_clear_bit(bm->bm_bit_size + i, bm->bm_map);
	}

	bm->bm_set_bits = 0;
	/* FIXME: optimize that */
	for (i = 0; i < bm->bm_bit_size; i++)
		if (reiserfs_bitmap_test_bit(bm, i))
			bm->bm_set_bits++;

	bm->bm_dirty = 0;

	return ret;
}

/* copy bitmap 'bm' to buffers which hold on-disk bitmap if bitmap was ever
   changed and return 1. Otherwise - return 0. Returns -1 on error. */
int reiserfs_flush_to_ondisk_bitmap(reiserfs_bitmap_t *bm,
				    reiserfs_filsys_t fs)
{
	unsigned int last_byte_unused_bits, i;
	unsigned long to_copy, copied, block;
	struct buffer_head *bh;
	char *p;

	/* make sure that the device is big enough */
	bh = bread(fs->fs_dev, bm->bm_bit_size - 1, fs->fs_blocksize);
	if (!bh) {
		reiserfs_warning(stderr,
				 "reiserfs_flush_to_ondisk_bitmap: bread failed for block %lu\n",
				 bm->bm_bit_size - 1);
/*
	bh = getblk (fs->fs_dev, bm->bm_bit_size - 1, fs->fs_blocksize);
	if (!bh)
	    reiserfs_panic ("reiserfs_flush_to_ondisk_bitmap: getblk failed");
	mark_buffer_uptodate (bh, 1);
	mark_buffer_dirty (bh);
	bwrite (bh);*/
	}
	brelse(bh);

	if (!bm->bm_dirty)
		return 0;

	to_copy = bm->bm_byte_size;

	copied = fs->fs_blocksize;
	p = bm->bm_map;
	block = fs->fs_super_bh->b_blocknr + 1;

	while (to_copy) {
		/* we bread to make sure that filesystem contains enough blocks */
		bh = getblk(fs->fs_dev, block, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(stderr, "Getblk failed for (%lu)\n",
					 block);
			return -1;
		}

		memset(bh->b_data, 0xff, bh->b_size);
		mark_buffer_uptodate(bh, 1);

		if (to_copy < fs->fs_blocksize)
			copied = to_copy;
		memcpy(bh->b_data, p, copied);
		if (copied == to_copy) {
			/* set unused bits of last byte of a bitmap to 1 */
			last_byte_unused_bits =
			    bm->bm_byte_size * 8 - bm->bm_bit_size;

			for (i = 0; i < last_byte_unused_bits; i++)
				misc_set_bit((bm->bm_bit_size %
					      (fs->fs_blocksize * 8)) + i,
					     bh->b_data);
		}
		mark_buffer_dirty(bh);
		brelse(bh);
		p += copied;
		to_copy -= copied;

		/* next bitmap block */
		if (spread_bitmaps(fs))
			block =
			    (block / (fs->fs_blocksize * 8) +
			     1) * (fs->fs_blocksize * 8);
		else
			block++;
	}

	return 1;
}

void reiserfs_bitmap_zero(reiserfs_bitmap_t *bm)
{
	memset(bm->bm_map, 0, bm->bm_byte_size);
	bm->bm_set_bits = 0;
	bm->bm_dirty = 1;
}

void reiserfs_bitmap_fill(reiserfs_bitmap_t *bm)
{
	memset(bm->bm_map, 0xff, bm->bm_byte_size);
	bm->bm_set_bits = bm->bm_bit_size;
	bm->bm_dirty = 1;
}

/* format of bitmap saved in a file:
   magic number (32 bits)
   bm_bit_size (32 bits)
   number of ranges of used and free blocks (32 bits)
   number of contiguously used block, .. of free blocks, used, free, etc
   magic number (32 bits) */

#define BITMAP_START_MAGIC 374031
#define BITMAP_END_MAGIC 7786472

FILE *open_file(const char *filename, char *const option)
{
	FILE *fp = fopen(filename, option);
	if (!fp) {
		reiserfs_warning(stderr, "open_file: could not open file %s\n",
				 filename);
		return NULL;
	}
	reiserfs_warning(stderr, "Temp file opened by fsck: \"%s\" .. \n",
			 filename);
	return fp;
}

void close_file(FILE * fp)
{
	fclose(fp);
	/*reiserfs_warning (stderr, "done\n"); fflush (stderr); */
}

void reiserfs_bitmap_save(FILE * fp, reiserfs_bitmap_t *bm)
{
//    FILE * fp;
	__u32 v;
	int zeros;
	int count;
	unsigned int i;
	int extents;
	long position;

	/*  fp = fopen (filename, "w+");
	   if (!fp) {
	   reiserfs_warning (stderr, "reiserfs_bitmap_save: could not save bitmap in %s: %s",
	   filename, strerror(errno));
	   return;
	   } */

//    reiserfs_warning (stderr, "Saving bitmap in \"%s\" .. ", filename); fflush (stderr);

	v = BITMAP_START_MAGIC;
	fwrite(&v, 4, 1, fp);

	v = bm->bm_bit_size;
	fwrite(&v, 4, 1, fp);

	/*printf ("SAVE: bit_size - %d\n", v); */

	position = ftell(fp);

	if (fseek(fp, 4, SEEK_CUR)) {
		reiserfs_warning(stderr,
				 "reiserfs_bitmap_save: fseek failed: %s\n",
				 strerror(errno));
//      fclose (fp);
		return;
	}

	zeros = 0;
	count = 0;
	extents = 0;
	for (i = 0; i < v; i++) {
		if (reiserfs_bitmap_test_bit(bm, i)) {
			if (zeros) {
				/* previous bit was not set, write amount of not set
				   bits, switch to count set bits */
				fwrite(&count, 4, 1, fp);
				/*printf ("SAVE: Free %d\n", count); */
				extents++;
				count = 1;
				zeros = 0;
			} else {
				/* one more zero bit appeared */
				count++;
			}
		} else {
			/* zero bit found */
			if (zeros) {
				count++;
			} else {
				/* previous bit was set, write amount of set bits,
				   switch to count not set bits */
				fwrite(&count, 4, 1, fp);
				/*printf ("SAVE: Used %d\n", count); */
				extents++;
				count = 1;
				zeros = 1;
			}
		}
	}

	fwrite(&count, 4, 1, fp);
	extents++;
/*
    if (zeros)
	printf ("SAVE: Free %d\n", count);
    else	
	printf ("SAVE: Used %d\n", count);
*/

	v = BITMAP_END_MAGIC;
	fwrite(&v, 4, 1, fp);

	if (fseek(fp, position, SEEK_SET)) {
		reiserfs_warning(stderr,
				 "reiserfs_bitmap_save: fseek failed: %s",
				 strerror(errno));
		return;
	}

	fwrite(&extents, 4, 1, fp);

	if (fseek(fp, 0, SEEK_END)) {
		reiserfs_warning(stderr,
				 "reiserfs_bitmap_save: fseek failed: %s",
				 strerror(errno));
		return;
	}
}

/* format of fsck dump file:
        after pass0
   magic number                 (32 bits)
   passed stage number
   bitmap of leaves
   bitmap of good_unfm
   bitmap of bad_unfm
   magic number                 (32 bits) */

#define FSCK_DUMP_START_MAGIC 374033
#define FSCK_DUMP_END_MAGIC 7786470

void reiserfs_begin_stage_info_save(FILE * file, unsigned long stage)
{
	__u32 v = FSCK_DUMP_START_MAGIC;
	fwrite(&v, 4, 1, file);
	fwrite(&stage, 4, 1, file);
}

void reiserfs_end_stage_info_save(FILE * file)
{
	__u32 v = FSCK_DUMP_END_MAGIC;
	fwrite(&v, 4, 1, file);
}

/*return last passed stage*/
int is_stage_magic_correct(FILE * fp)
{
	__u32 v;

	if (fseek(fp, -4, SEEK_END)) {
		reiserfs_warning(stderr,
				 "is_stage_magic_correct: fseek failed: %s\n",
				 strerror(errno));
		return -1;
	}

	fread(&v, 4, 1, fp);
	if (v != FSCK_DUMP_END_MAGIC) {
		reiserfs_warning(stderr,
				 "is_stage_magic_correct: no magic found\n");
		return -1;
	}

	if (fseek(fp, 0, SEEK_SET)) {
		reiserfs_warning(stderr,
				 "is_stage_magic_correct: fseek failed: %s\n",
				 strerror(errno));
		return -1;
	}

	fread(&v, 4, 1, fp);
	if (v != FSCK_DUMP_START_MAGIC) {
		reiserfs_warning(stderr,
				 "is_stage_magic_correct: no magic found\n");
		return -1;
	}

	fread(&v, 4, 1, fp);
	if (v != PASS_0_DONE && v != PASS_1_DONE && v != TREE_IS_BUILT
	    && v != SEMANTIC_DONE && v != LOST_FOUND_DONE) {
		reiserfs_warning(stderr,
				 "is_stage_magic_correct: wrong pass found");
		return -1;
	}

	return (__u16) v;
}

reiserfs_bitmap_t *reiserfs_bitmap_load(FILE * fp)
{
//    FILE * fp;
	__u32 v;
	int count;
	int i, j;
	int extents;
	int bit;
	reiserfs_bitmap_t *bm;

/*    fp = fopen (filename, "r");
    if (!fp) {
	reiserfs_warning (stderr, "reiserfs_bitmap_load: fopen failed: %s\n", strerror(errno));
	return 0;
    }*/

	fread(&v, 4, 1, fp);
	if (v != BITMAP_START_MAGIC) {
		reiserfs_warning(stderr, "reiserfs_bitmap_load: "
				 "no bitmap start magic found");
//      fclose (fp);
		return NULL;
	}

	/* read bit size of bitmap */
	fread(&v, 4, 1, fp);

	bm = reiserfs_create_bitmap(v);
	if (!bm) {
		reiserfs_warning(stderr,
				 "reiserfs_bitmap_load: creation failed");
//      fclose (fp);
		return NULL;
	}

	/*printf ("LOAD: bit_size - %d\n", v); */

	fread(&extents, 4, 1, fp);

	/*printf ("LOAD: extents - %d\n", extents); */

	bit = 0;
	for (i = 0; i < extents; i++) {
		fread(&count, 4, 1, fp);
/*
	if (i % 2)
	    printf ("LOAD: Free %d\n", count);
	else
	    printf ("LOAD: Used %d\n", count);
*/
		for (j = 0; j < count; j++, bit++)
			if (i % 2 == 0) {
				reiserfs_bitmap_set_bit(bm, bit);
			}
	}

	fread(&v, 4, 1, fp);

	/*printf ("LOAD: Endmagic %d\n", v); */

//    fclose (fp);
	if (v != BITMAP_END_MAGIC) {
		reiserfs_warning(stderr, "reiserfs_bitmap_load: "
				 "no bitmap end magic found");
		return NULL;
	}

	/*    reiserfs_warning (stderr, "%d bits set - done\n", reiserfs_bitmap_ones (bm)); */
	fflush(stderr);
	return bm;
}

void reiserfs_bitmap_invert(reiserfs_bitmap_t *bm)
{
	unsigned int i;

	/*reiserfs_warning (stderr, "Bitmap inverting..");fflush (stderr); */
	for (i = 0; i < bm->bm_bit_size; i++) {
		if (reiserfs_bitmap_test_bit(bm, i))
			reiserfs_bitmap_clear_bit(bm, i);
		else
			reiserfs_bitmap_set_bit(bm, i);
	}

	/*reiserfs_warning (stderr, "done\n"); */
}

void reiserfs_free_ondisk_bitmap(reiserfs_filsys_t fs)
{
	if (fs->fs_bitmap2) {
		reiserfs_delete_bitmap(fs->fs_bitmap2);
		fs->fs_bitmap2 = NULL;
	}
}

/* read bitmap blocks */
int reiserfs_open_ondisk_bitmap(reiserfs_filsys_t fs)
{
	unsigned int blocks = get_sb_block_count(fs->fs_ondisk_sb);
	unsigned int bmap_nr = reiserfs_bmap_nr(blocks, fs->fs_blocksize);

	if (fs->fs_bitmap2)
		reiserfs_panic("bitmap is initiaized already");

	fs->fs_bitmap2 = reiserfs_create_bitmap(blocks);
	if (!fs->fs_bitmap2)
		return -1;

	if (reiserfs_bmap_over(bmap_nr)) {
		if (get_sb_bmap_nr(fs->fs_ondisk_sb) != 0) {
			reiserfs_warning(stderr, "%s: large file system has "
					 "incorrect bitmap count %u. Should be 0 "
					 "to indicate overflow.\nPlease re-run with "
					 "--rebuild-sb to fix it.\n",
					 __FUNCTION__,
					 get_sb_bmap_nr(fs->fs_ondisk_sb));
			return -1;
		}
	} else if (bmap_nr != get_sb_bmap_nr(fs->fs_ondisk_sb)) {
		reiserfs_warning(stderr, "%s: wrong either bitmaps number,\n",
				 __FUNCTION__);
		reiserfs_warning(stderr,
				 "count of blocks or blocksize, run with "
				 "--rebuild-sb to fix it\n");
		return -1;
	}

	return reiserfs_fetch_ondisk_bitmap(fs->fs_bitmap2, fs);
}

int reiserfs_create_ondisk_bitmap(reiserfs_filsys_t fs)
{
	if (fs->fs_bitmap2)
		reiserfs_panic("bitmap is initiaized already");

	fs->fs_bitmap2 =
	    reiserfs_create_bitmap(get_sb_block_count(fs->fs_ondisk_sb));
	if (!fs->fs_bitmap2)
		return 0;

	return 1;
}

void reiserfs_close_ondisk_bitmap(reiserfs_filsys_t fs)
{
	int ret;
	if (!fs->fs_bitmap2)
		return;
	ret = reiserfs_flush_to_ondisk_bitmap(fs->fs_bitmap2, fs);
	if (ret < 0)
		reiserfs_exit(1, "Exiting after unrecoverable error.");
	reiserfs_free_ondisk_bitmap(fs);
}
