/*****************************************************************************
******************************************************************************
**
**  gfs2_convert - convert a gfs1 filesystem into a gfs2 filesystem.
**
******************************************************************************
*****************************************************************************/

#include "clusterautoconfig.h"

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>
#include <termios.h>
#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)

#include <linux/types.h>
#include <linux/gfs2_ondisk.h>
#include <logging.h>
#include "osi_list.h"
#include "copyright.cf"
#include "libgfs2.h"

/* The following declares are needed because gfs2 can't have  */
/* dependencies on gfs1:                                      */
#define RGRP_STUFFED_BLKS(sb) (((sb)->sb_bsize - sizeof(struct gfs2_rgrp)) * GFS2_NBBY)
#define RGRP_BITMAP_BLKS(sb) (((sb)->sb_bsize - sizeof(struct gfs2_meta_header)) * GFS2_NBBY)

/* Define some gfs1 constants from gfs1's gfs_ondisk.h */
#define GFS_METATYPE_NONE       (0)
#define GFS_METATYPE_SB         (1)    /* Super-Block */
#define GFS_METATYPE_RG         (2)    /* Resource Group Header */
#define GFS_METATYPE_RB         (3)    /* Resource Group Block Alloc BitBlock */
#define GFS_METATYPE_DI         (4)    /* "Disk" inode (dinode) */
#define GFS_METATYPE_IN         (5)    /* Indirect dinode block list */
#define GFS_METATYPE_LF         (6)    /* Leaf dinode block list */
#define GFS_METATYPE_JD         (7)    /* Journal Data */
#define GFS_METATYPE_LH         (8)    /* Log Header (gfs_log_header) */
#define GFS_METATYPE_LD         (9)    /* Log Descriptor (gfs_log_descriptor) */
#define GFS_METATYPE_EA         (10)   /* Extended Attribute */
#define GFS_METATYPE_ED         (11)   /* Extended Attribute data */

/* GFS1 Dinode types  */
#define GFS_FILE_NON            (0)
#define GFS_FILE_REG            (1)    /* regular file */
#define GFS_FILE_DIR            (2)    /* directory */
#define GFS_FILE_LNK            (5)    /* link */
#define GFS_FILE_BLK            (7)    /* block device node */
#define GFS_FILE_CHR            (8)    /* character device node */
#define GFS_FILE_FIFO           (101)  /* fifo/pipe */
#define GFS_FILE_SOCK           (102)  /* socket */

#define GFS_FORMAT_SB           (100)  /* Super-Block */
#define GFS_FORMAT_FS           (1309) /* Filesystem (all-encompassing) */
#define GFS_FORMAT_MULTI        (1401) /* Multi-Host */

#define DIV_RU(x, y) (((x) + (y) - 1) / (y))

struct inode_dir_block {
	osi_list_t list;
	uint64_t di_addr;
	uint64_t di_paddr; /* Parent dir inode addr */
};

struct inode_block {
	osi_list_t list;
	uint64_t di_addr;
};

struct blocklist {
	osi_list_t list;
	uint64_t block;
	struct metapath mp;
	int height;
	char *ptrbuf;
};

struct gfs2_options {
	char *device;
	unsigned int yes:1;
	unsigned int no:1;
	unsigned int query:1;
};

struct gfs_sb  raw_gfs1_ondisk_sb;
struct gfs2_sbd sb2;
struct inode_block dirs_to_fix;  /* linked list of directories to fix */
struct inode_dir_block cdpns_to_fix; /* linked list of cdpn symlinks */
int seconds;
struct timeval tv;
uint64_t dirs_fixed;
uint64_t cdpns_fixed;
uint64_t dirents_fixed;
struct gfs_jindex *sd_jindex = NULL;    /* gfs1 journal index in memory */
int gfs2_inptrs;
uint64_t gfs2_heightsize[GFS2_MAX_META_HEIGHT];
uint64_t gfs2_jheightsize[GFS2_MAX_META_HEIGHT];
uint32_t gfs2_max_height;
uint32_t gfs2_max_jheight;
uint64_t jindex_addr = 0, rindex_addr = 0;
int print_level = MSG_NOTICE;
unsigned orig_journals = 0;

/* ------------------------------------------------------------------------- */
/* This function is for libgfs's sake.                                       */
/* ------------------------------------------------------------------------- */
void print_it(const char *label, const char *fmt, const char *fmt2, ...)
{
	va_list args;

	va_start(args, fmt2);
	printf("%s: ", label);
	vprintf(fmt, args);
	va_end(args);
}

/* ------------------------------------------------------------------------- */
/* convert_bitmaps - Convert gfs1 bitmaps to gfs2 bitmaps.                   */
/*                   Fixes all unallocated metadata bitmap states (which are */
/*                   valid in gfs1 but invalid in gfs2).                     */
/* ------------------------------------------------------------------------- */
static void convert_bitmaps(struct gfs2_sbd *sdp, struct rgrp_tree *rg)
{
	uint32_t blk;
	int x, y;
	struct gfs2_rindex *ri;
	unsigned char state;

	ri = &rg->ri;
	for (blk = 0; blk < ri->ri_length; blk++) {
		struct gfs2_bitmap *bi;
		x = (blk) ? sizeof(struct gfs2_meta_header) :
			sizeof(struct gfs2_rgrp);

		bi = &rg->bits[blk];
		for (; x < sdp->bsize; x++)
			for (y = 0; y < GFS2_NBBY; y++) {
				state = (bi->bi_bh->b_data[x] >>
					 (GFS2_BIT_SIZE * y)) & 0x03;
				if (state == 0x02) {/* unallocated metadata state invalid */
					bi->bi_bh->b_data[x] &= ~(0x02 << (GFS2_BIT_SIZE * y));
					bmodified(bi->bi_bh);
				}
			}
	}
}/* convert_bitmaps */

/* ------------------------------------------------------------------------- */
/* convert_rgs - Convert gfs1 resource groups to gfs2.                       */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int convert_rgs(struct gfs2_sbd *sbp)
{
	struct rgrp_tree *rgd;
	struct osi_node *n, *next = NULL;
	struct gfs_rgrp *rgd1;
	int rgs = 0;

	/* --------------------------------- */
	/* Now convert its rgs into gfs2 rgs */
	/* --------------------------------- */
	for (n = osi_first(&sbp->rgtree); n; n = next) {
		next = osi_next(n);
		rgd = (struct rgrp_tree *)n;

		rgd1 = (struct gfs_rgrp *)&rgd->rg; /* recast as gfs1 structure */
		/* rg_freemeta is a gfs1 structure, so libgfs2 doesn't know to */
		/* convert from be to cpu. We must do it now. */
		rgd->rg.rg_free = rgd1->rg_free + be32_to_cpu(rgd1->rg_freemeta);
		/* Zero it out so we don't add it again in case something breaks */
		/* later on in the process and we have to re-run convert */
		rgd1->rg_freemeta = 0;

		sbp->blks_total += rgd->ri.ri_data;
		sbp->blks_alloced += (rgd->ri.ri_data - rgd->rg.rg_free);
		sbp->dinodes_alloced += rgd1->rg_useddi;
		convert_bitmaps(sbp, rgd);
		/* Write the updated rgrp to the gfs2 buffer */
		gfs2_rgrp_out(&rgd->rg, rgd->bits[0].bi_bh->b_data);
		bmodified(rgd->bits[0].bi_bh);
		rgs++;
		if (rgs % 100 == 0) {
			printf(".");
			fflush(stdout);
		}
	}
	return 0;
}/* superblock_cvt */

/* ------------------------------------------------------------------------- */
/* calc_gfs2_tree_height - calculate new dinode height as if this is gfs2    */
/*                                                                           */
/* This is similar to calc_tree_height in libgfs2 but at the point this      */
/* function is called, I have the wrong (gfs1 not gfs2) constants in place.  */
/* ------------------------------------------------------------------------- */
static unsigned int calc_gfs2_tree_height(struct gfs2_inode *ip, uint64_t size)
{
	uint64_t *arr;
	unsigned int max, height;

	if (ip->i_di.di_size > size)
		size = ip->i_di.di_size;

	if (S_ISDIR(ip->i_di.di_mode)) {
		arr = gfs2_jheightsize;
		max = gfs2_max_jheight;
	} else {
		arr = gfs2_heightsize;
		max = gfs2_max_height;
	}

	for (height = 0; height < max; height++)
		if (arr[height] >= size)
			break;
	/* If calc_gfs2_tree_height was called, the dinode is not stuffed or
	   we would have returned before this point. After the call, a call is
	   made to fix_metatree, which unstuffs the dinode. Therefore, the
	   smallest height that can result after this call is 1. */
	if (!height)
		height = 1;

	return height;
}

/* ------------------------------------------------------------------------- */
/* mp_gfs1_to_gfs2 - convert a gfs1 metapath to a gfs2 metapath.             */
/* ------------------------------------------------------------------------- */
static void mp_gfs1_to_gfs2(struct gfs2_sbd *sbp, int gfs1_h, int gfs2_h,
		     struct metapath *gfs1mp, struct metapath *gfs2mp)
{
	uint64_t lblock;
	int h;
	uint64_t gfs1factor[GFS2_MAX_META_HEIGHT];
	uint64_t gfs2factor[GFS2_MAX_META_HEIGHT];

	/* figure out multiplication factors for each height - gfs1 */
	memset(&gfs1factor, 0, sizeof(gfs1factor));
	gfs1factor[gfs1_h - 1] = 1ull;
	for (h = gfs1_h - 1; h > 0; h--)
		gfs1factor[h - 1] = gfs1factor[h] * sbp->sd_inptrs;

	/* figure out multiplication factors for each height - gfs2 */
	memset(&gfs2factor, 0, sizeof(gfs2factor));
	gfs2factor[gfs2_h - 1] = 1ull;
	for (h = gfs2_h - 1; h > 0; h--)
		gfs2factor[h - 1] = gfs2factor[h] * gfs2_inptrs;

	/* Convert from gfs1 to a logical block */
	lblock = 0;
	for (h = 0; h < gfs1_h; h++)
		lblock += (gfs1mp->mp_list[h] * gfs1factor[h]);

	/* Convert from a logical block back to gfs2 */
	memset(gfs2mp, 0, sizeof(*gfs2mp));
	for (h = 0; h < gfs2_h; h++) {
		/* Can't use do_div here because the factors are too large. */
		gfs2mp->mp_list[h] = lblock / gfs2factor[h];
		lblock %= gfs2factor[h];
	}
}

/* ------------------------------------------------------------------------- */
/* fix_metatree - Fix up the metatree to match the gfs2 metapath info        */
/*                Similar to gfs2_writei in libgfs2 but we're only           */
/*                interested in rearranging the metadata while leaving the   */
/*                actual data blocks intact.                                 */
/* ------------------------------------------------------------------------- */
static void fix_metatree(struct gfs2_sbd *sbp, struct gfs2_inode *ip,
		  struct blocklist *blk, uint64_t *first_nonzero_ptr,
		  unsigned int size)
{
	uint64_t block;
	struct gfs2_buffer_head *bh;
	unsigned int amount, ptramt;
	int hdrsize, h, copied = 0, new;
	struct gfs2_meta_header mh;
	char *srcptr = (char *)first_nonzero_ptr;

	mh.mh_magic = GFS2_MAGIC;
	mh.mh_type = GFS2_METATYPE_IN;
	mh.mh_format = GFS2_FORMAT_IN;
	if (!ip->i_di.di_height)
		unstuff_dinode(ip);

	ptramt = blk->mp.mp_list[blk->height] * sizeof(uint64_t);
	amount = size;

	while (copied < size) {
		bh = ip->i_bh;
		/* First, build up the metatree */
		for (h = 0; h < blk->height; h++) {
			new = 0;
			lookup_block(ip, bh, h, &blk->mp, 1, &new, &block);
			if (bh != ip->i_bh)
				brelse(bh);
			if (!block)
				break;

			bh = bread(sbp, block);
			if (new)
				memset(bh->b_data, 0, sbp->bsize);
			gfs2_meta_header_out(&mh, bh->b_data);
			bmodified(bh);
		}

		hdrsize = blk->height ? sizeof(struct gfs2_meta_header) :
			sizeof(struct gfs2_dinode);

		if (amount > sbp->bsize - hdrsize - ptramt)
			amount = sbp->bsize - hdrsize - ptramt;

		memcpy(bh->b_data + hdrsize + ptramt, (char *)srcptr, amount);
		srcptr += amount;
		bmodified(bh);
		if (bh != ip->i_bh)
			brelse(bh);

		copied += amount;

		if (hdrsize + ptramt + amount >= sbp->bsize) {
			/* advance to the next metablock */
			blk->mp.mp_list[blk->height] +=
				(amount / sizeof(uint64_t));
			for (h = blk->height; h > 0; h--) {
				if (blk->mp.mp_list[h] >= gfs2_inptrs) {
					blk->mp.mp_list[h] = 0;
					blk->mp.mp_list[h - 1]++;
					continue;
				}
				break;
			}
		}
		amount = size - copied;
		ptramt = 0;
	}
}

/* ------------------------------------------------------------------------- */
/* adjust_indirect_blocks - convert all gfs_indirect blocks to gfs2.         */
/*                                                                           */
/* This function converts all gfs_indirect blocks to GFS2.  The difference   */
/* is that gfs1 indirect block has a 64-byte chunk of reserved space that    */
/* gfs2 does not.  Since GFS block locations (relative to the start of the   */
/* file have their locations defined by the offset from the end of the       */
/* structure, all block pointers must be shifted.                            */
/*                                                                           */
/* Stuffed inodes don't need to be shifted at since there are no indirect    */
/* blocks.  Inodes with height 1 don't need to be shifted either, because    */
/* the dinode size is the same between gfs and gfs2 (232 bytes), and         */
/* therefore you can fit the same number of block pointers after the dinode  */
/* structure.  For the normal 4K block size, that's 483 pointers.  For 1K    */
/* blocks, it's 99 pointers.                                                 */
/*                                                                           */
/* At height 2 things get complex.  GFS1 reserves an area of 64 (0x40) bytes */
/* at the start of the indirect block, so for 4K blocks, you can fit 501     */
/* pointers.  GFS2 doesn't reserve that space, so you can fit 509 pointers.  */
/* For 1K blocks, it's 117 pointers in GFS1 and 125 in GFS2.                 */
/*                                                                           */
/* That means, for example, that if you have 4K blocks, a 946MB file will    */
/* require a height of 3 for GFS, but only a height of 2 for GFS2.           */
/* There isn't a good way to shift the pointers around from one height to    */
/* another, so the only way to do it is to rebuild all those indirect blocks */
/* from empty ones.                                                          */
/*                                                                           */
/* For example, with a 1K block size, if you do:                             */
/*                                                                           */
/* dd if=/mnt/gfs/big of=/tmp/tocompare skip=496572346368 bs=1024 count=1    */
/*                                                                           */
/* the resulting metadata paths will look vastly different for the data:     */
/*                                                                           */
/* height    0     1     2     3     4     5                                 */
/* GFS1:  0x16  0x4b  0x70  0x11  0x5e  0x48                                 */
/* GFS2:  0x10  0x21  0x78  0x05  0x14  0x76                                 */
/*                                                                           */
/* To complicate matters, we can't really require free space.  A user might  */
/* be trying to migrate a "full" gfs1 file system to GFS2.  After we         */
/* convert the journals to GFS2, we might have more free space, so we can    */
/* allocate blocks at that time.                                             */
/*                                                                           */
/* Assumes: GFS1 values are in place for diptrs and inptrs.                  */
/*                                                                           */
/* Returns: 0 on success, -1 on failure                                      */
/*                                                                           */
/* Adapted from fsck.gfs2 metawalk.c's build_and_check_metalist              */
/* ------------------------------------------------------------------------- */

static void jdata_mp_gfs1_to_gfs2(struct gfs2_sbd *sbp, int gfs1_h, int gfs2_h,
			   struct metapath *gfs1mp, struct metapath *gfs2mp,
			   unsigned int *len, uint64_t dinode_size)
{
	uint64_t offset;
	int h;
	uint64_t gfs1factor[GFS2_MAX_META_HEIGHT];
	uint64_t gfs2factor[GFS2_MAX_META_HEIGHT];

	/* figure out multiplication factors for each height - gfs1 */
	memset(&gfs1factor, 0, sizeof(gfs1factor));
	gfs1factor[gfs1_h - 1] = sbp->bsize - sizeof(struct gfs2_meta_header);
	for (h = gfs1_h - 1; h > 0; h--)
		gfs1factor[h - 1] = gfs1factor[h] * sbp->sd_inptrs;

	/* figure out multiplication factors for each height - gfs2 */
	memset(&gfs2factor, 0, sizeof(gfs2factor));
	gfs2factor[gfs2_h] = 1ull;
	gfs2factor[gfs2_h - 1] = sbp->bsize;
	for (h = gfs2_h - 1; h > 0; h--)
		gfs2factor[h - 1] = gfs2factor[h] * gfs2_inptrs;

	/* Convert from gfs1 to an offset */
	offset = 0;
	for (h = 0; h < gfs1_h; h++)
		offset += (gfs1mp->mp_list[h] * gfs1factor[h]);

	if (dinode_size - offset < *len)
		*len = dinode_size - offset;

	/* Convert from an offset back to gfs2 */
	memset(gfs2mp, 0, sizeof(*gfs2mp));
	for (h = 0; h <= gfs2_h; h++) {
		/* Can't use do_div here because the factors are too large. */
		gfs2mp->mp_list[h] = offset / gfs2factor[h];
		offset %= gfs2factor[h];
	}
}

static uint64_t fix_jdatatree(struct gfs2_sbd *sbp, struct gfs2_inode *ip,
			      struct blocklist *blk, char *srcptr,
			      unsigned int size)
{
	uint64_t block;
	struct gfs2_buffer_head *bh;
	unsigned int amount, ptramt;
	int h, copied = 0, new = 0;
	struct gfs2_meta_header mh;

	mh.mh_magic = GFS2_MAGIC;
	mh.mh_type = GFS2_METATYPE_IN;
	mh.mh_format = GFS2_FORMAT_IN;

	if (!ip->i_di.di_height)
		unstuff_dinode(ip);

	ptramt = blk->mp.mp_list[blk->height];
	amount = size;

	while (copied < size) {
		bh = ip->i_bh;
		/* First, build up the metatree */
		for (h = 0; h < blk->height; h++) {
			new = 0;
			lookup_block(ip, bh, h, &blk->mp, 1, &new, &block);
			if (bh != ip->i_bh)
				brelse(bh);
			if (!block)
				break;

			bh = bread(sbp, block);
			if (new)
				memset(bh->b_data, 0, sbp->bsize);
			if (h < (blk->height - 1)) {
				gfs2_meta_header_out(&mh, bh->b_data);
				bmodified(bh);
			}
		}

		if (amount > sbp->bsize - ptramt)
			amount = sbp->bsize - ptramt;

		memcpy(bh->b_data + ptramt, (char *)srcptr, amount);
		srcptr += amount;
		bmodified(bh);
		if (bh != ip->i_bh)
			brelse(bh);

		copied += amount;

		if (ptramt + amount >= sbp->bsize) {
			/* advance to the next metablock */
			blk->mp.mp_list[blk->height] += amount;
			for (h = blk->height; h > 0; h--) {
				if (blk->mp.mp_list[h] >= gfs2_inptrs) {
					blk->mp.mp_list[h] = 0;
					blk->mp.mp_list[h - 1]++;
					continue;
				}
				break;
			}
		}
		amount = size - copied;
		ptramt = 0;
	}
	return block;
}

static int get_inode_metablocks(struct gfs2_sbd *sbp, struct gfs2_inode *ip, struct blocklist *blocks)
{
	struct blocklist *blk, *newblk;
	struct gfs2_buffer_head *bh, *dibh = ip->i_bh;
	osi_list_t *tmp;
	uint64_t *ptr1, block;
	int h, ptrnum;
	int bufsize = sbp->bsize - sizeof(struct gfs_indirect);

	/* Add dinode block to the list */
	blk = malloc(sizeof(struct blocklist));
	if (!blk) {
		log_crit(_("Error: Can't allocate memory for indirect block fix\n"));
		return -1;
	}
	memset(blk, 0, sizeof(*blk));
	blk->block = dibh->b_blocknr;
	blk->ptrbuf = malloc(bufsize);
	if (!blk->ptrbuf) {
		log_crit(_("Error: Can't allocate memory"
			 " for file conversion.\n"));
		free(blk);
		return -1;
	}
	memset(blk->ptrbuf, 0, bufsize);
	/* Fill in the pointers from the dinode buffer */
	memcpy(blk->ptrbuf, dibh->b_data + sizeof(struct gfs_dinode),
	       sbp->bsize - sizeof(struct gfs_dinode));
	/* Zero out the pointers so we can fill them in later. */
	memset(dibh->b_data + sizeof(struct gfs_dinode), 0,
	       sbp->bsize - sizeof(struct gfs_dinode));
	osi_list_add_prev(&blk->list, &blocks->list);

	/* Now run the metadata chain and build lists of all metadata blocks */
	osi_list_foreach(tmp, &blocks->list) {
		blk = osi_list_entry(tmp, struct blocklist, list);

		if (blk->height >= ip->i_di.di_height - 1)
			continue;
		for (ptr1 = (uint64_t *)blk->ptrbuf, ptrnum = 0;
		     ptrnum < sbp->sd_inptrs; ptr1++, ptrnum++) {
			if (!*ptr1)
				continue;
			block = be64_to_cpu(*ptr1);

			newblk = malloc(sizeof(struct blocklist));
			if (!newblk) {
				log_crit(_("Error: Can't allocate memory for indirect block fix.\n"));
				return -1;
			}
			memset(newblk, 0, sizeof(*newblk));
			newblk->ptrbuf = malloc(bufsize);
			if (!newblk->ptrbuf) {
				/* FIXME: This message should be different, to not conflit with the above file conversion */
				log_crit(_("Error: Can't allocate memory for file conversion.\n"));
				free(newblk);
				return -1;
			}
			memset(newblk->ptrbuf, 0, bufsize);
			newblk->block = block;
			newblk->height = blk->height + 1;
			/* Build the metapointer list from our predecessors */
			for (h = 0; h < blk->height; h++)
				newblk->mp.mp_list[h] = blk->mp.mp_list[h];
			newblk->mp.mp_list[h] = ptrnum;
			/* Queue it to be processed later on in the loop. */
			osi_list_add_prev(&newblk->list, &blocks->list);
			/* read the new metadata block's pointers */
			bh = bread(sbp, block);
			memcpy(newblk->ptrbuf, bh->b_data + sizeof(struct gfs_indirect), bufsize);
			/* Zero the buffer so we can fill it in later */
			memset(bh->b_data + sizeof(struct gfs_indirect), 0, bufsize);
			bmodified(bh);
			brelse(bh);
			/* Free the block so we can reuse it. This allows us to
			   convert a "full" file system. */
			ip->i_di.di_blocks--;
			gfs2_free_block(sbp, block);
		}
	}
	return 0;
}

static int fix_ind_reg_or_dir(struct gfs2_sbd *sbp, struct gfs2_inode *ip, uint32_t di_height,
		       uint32_t gfs2_hgt, struct blocklist *blk, struct blocklist *blocks)
{
	unsigned int len, bufsize;
	uint64_t *ptr1, *ptr2;
	int ptrnum;
	struct metapath gfs2mp;

	bufsize = sbp->bsize - sizeof(struct gfs_indirect);
	len = bufsize;

	/* Skip zero pointers at the start of the buffer.  This may 
	   seem pointless, but the gfs1 blocks won't align with the 
	   gfs2 blocks.  That means that a single block write of 
	   gfs1's pointers is likely to span two blocks on gfs2. 
	   That's a problem if the file system is full. 
	   So I'm trying to truncate the data at the start and end 
	   of the buffers (i.e. write only what we need to). */
	for (ptr1 = (uint64_t *)blk->ptrbuf, ptrnum = 0;
	     ptrnum < sbp->sd_inptrs; ptr1++, ptrnum++) {
		if (*ptr1 != 0x00)
			break;
		len -= sizeof(uint64_t);
	}
	/* Skip zero bytes at the end of the buffer */
	ptr2 = (uint64_t *)(blk->ptrbuf + bufsize) - 1;
	while (len > 0 && *ptr2 == 0) {
		ptr2--;
		len -= sizeof(uint64_t);
	}
	blk->mp.mp_list[di_height - 1] = ptrnum;
	mp_gfs1_to_gfs2(sbp, di_height, gfs2_hgt, &blk->mp, &gfs2mp);
	memcpy(&blk->mp, &gfs2mp, sizeof(struct metapath));
	blk->height -= di_height - gfs2_hgt;
	if (len) {
		fix_metatree(sbp, ip, blk, ptr1, len);
		ip->i_di.di_goal_meta = be64_to_cpu(*ptr2);
	}

	return 0;
}

static int fix_ind_jdata(struct gfs2_sbd *sbp, struct gfs2_inode *ip, uint32_t di_height, 
		  uint32_t gfs2_hgt, uint64_t dinode_size, struct blocklist *blk, 
		  struct blocklist *blocks)
{
	/*FIXME: Messages here should be different, to not conflit with messages in get_inode_metablocks */
	struct blocklist *newblk;
	unsigned int len, bufsize;
	uint64_t *ptr1, block;
	int ptrnum, h;
	struct metapath gfs2mp;
	struct gfs2_buffer_head *bh;

	bufsize = sbp->bsize - sizeof(struct gfs2_meta_header);
	/*
	 * For each metadata block that holds jdata block pointers,
	 * get the blk pointers and copy them block by block
	 */
	for (ptr1 = (uint64_t *) blk->ptrbuf, ptrnum = 0;
	     ptrnum < sbp->sd_inptrs; ptr1++, ptrnum++) {
		if (!*ptr1)
			continue;
		block = be64_to_cpu(*ptr1);

		newblk = malloc(sizeof(struct blocklist));
		if (!newblk) {
			log_crit(_("Error: Can't allocate memory for indirect block fix.\n"));
			return -1;
		}
		memset(newblk, 0, sizeof(*newblk));
		newblk->ptrbuf = malloc(bufsize); 
		if (!newblk->ptrbuf) {
			log_crit(_("Error: Can't allocate memory for file conversion.\n"));
			free(newblk);
			return -1;
		}
		memset(newblk->ptrbuf, 0, bufsize);
		newblk->block = block;
		newblk->height = blk->height + 1;
		/* Build the metapointer list from our predecessors */
		for (h=0; h < blk->height; h++)
			newblk->mp.mp_list[h] = blk->mp.mp_list[h];
		newblk->mp.mp_list[h] = ptrnum;
		bh = bread(sbp, block);
		/* This is a data block. i.e newblk->height == ip->i_di.di_height */
		/* read in the jdata block */
		memcpy(newblk->ptrbuf, bh->b_data +
		       sizeof(struct gfs2_meta_header), bufsize);
		memset(bh->b_data + sizeof(struct gfs2_meta_header), 0, bufsize);
		bmodified(bh);
		brelse(bh);
		/* Free the block so we can reuse it. This allows us to
		   convert a "full" file system */
		ip->i_di.di_blocks--;
		gfs2_free_block(sbp, block);

		len = bufsize;
		jdata_mp_gfs1_to_gfs2(sbp, di_height, gfs2_hgt, &newblk->mp, &gfs2mp,
				      &len, dinode_size);
		memcpy(&newblk->mp, &gfs2mp, sizeof(struct metapath));
		newblk->height -= di_height - gfs2_hgt;
		if (len)
			ip->i_di.di_goal_meta = fix_jdatatree(sbp, ip, newblk,
							      newblk->ptrbuf, len);
		free(newblk->ptrbuf);
		free(newblk);
	}
	return 0;
}

static int adjust_indirect_blocks(struct gfs2_sbd *sbp, struct gfs2_inode *ip)
{
	uint64_t dinode_size;
	uint32_t gfs2_hgt, di_height;
	osi_list_t *tmp=NULL, *x;
	struct blocklist blocks, *blk;
	int error = 0;

	int isdir = S_ISDIR(ip->i_di.di_mode); /* is always jdata */
	int isjdata = ((GFS2_DIF_JDATA & ip->i_di.di_flags) && !isdir);
	int isreg = (!isjdata && !isdir);
	int issys = (GFS2_DIF_SYSTEM & ip->i_di.di_flags);

	/* regular files and dirs are same upto height=2
	   jdata files (not dirs) are same only when height=0 */
	if (((isreg||isdir) && ip->i_di.di_height <= 1) ||
	    (isjdata && ip->i_di.di_height == 0)) {
		if (!issys)
			ip->i_di.di_goal_meta = ip->i_di.di_num.no_addr;
		return 0; /* nothing to do */
	}

	osi_list_init(&blocks.list);

	error = get_inode_metablocks(sbp, ip, &blocks);
	if (error)
		goto out;

	/* The gfs2 height may be different.  We need to rebuild the
	   metadata tree to the gfs2 height. */
	gfs2_hgt = calc_gfs2_tree_height(ip, ip->i_di.di_size);
	/* Save off the size because we're going to empty the contents
	   and add the data blocks back in later. */
	dinode_size = ip->i_di.di_size;
	ip->i_di.di_size = 0ULL;
	di_height = ip->i_di.di_height;
	ip->i_di.di_height = 0;

	/* Now run through the block list a second time.  If the block
	   is a data block, rewrite the data to the gfs2 offset. */
	osi_list_foreach_safe(tmp, &blocks.list, x) {

		blk = osi_list_entry(tmp, struct blocklist, list);
		/* If it's not metadata that holds data block pointers
		   (i.e. metadata pointing to other metadata) */
		if (blk->height != di_height - 1) {
			osi_list_del(tmp);
			free(blk->ptrbuf);
			free(blk);
			continue;
		}
		if (isreg || isdir) /* more or less same way to deal with either */
			error = fix_ind_reg_or_dir(sbp, ip, di_height, 
						   gfs2_hgt, blk, &blocks);
		else if (isjdata)
			error = fix_ind_jdata(sbp, ip, di_height, gfs2_hgt, 
					      dinode_size, blk, &blocks);
		if (error)
			goto out;

		osi_list_del(tmp);
		free(blk->ptrbuf);
		free(blk);
	}

	ip->i_di.di_size = dinode_size;

	/* Set the new dinode height, which may or may not have changed.  */
	/* The caller will take it from the ip and write it to the buffer */
	ip->i_di.di_height = gfs2_hgt;
	return error;

out:
	while (!osi_list_empty(&blocks.list)) {
		blk = osi_list_entry(tmp, struct blocklist, list);
		osi_list_del(&blocks.list);
		free(blk->ptrbuf);
		free(blk);
	}
	return error;
}

const char *cdpn[14] = {"{hostname}", "{mach}", "{os}", "{uid}", "{gid}", "{sys}", "{jid}",
			"@hostname", "@mach", "@os", "@uid", "@gid", "@sys", "@jid"};
static int has_cdpn(const char *str)
{
	int i;
	for (i=0; i<14; i++)
		if (strstr(str, cdpn[i]) != NULL)
			return 1;
	return 0;
}

static int fix_cdpn_symlink(struct gfs2_sbd *sbp, struct gfs2_buffer_head *bh, struct gfs2_inode *ip)
{
	char *linkptr = NULL;

	if (ip->i_di.di_height != 0)
		return 0;

	linkptr = bh->b_data + sizeof(struct gfs_dinode);
	if (has_cdpn(linkptr)) {
		struct inode_dir_block *fix;
		/* Save the symlink di_addr. We'll find the parent di_addr later */
		fix = malloc(sizeof(struct inode_dir_block));
		if (!fix) {
			log_crit(_("Error: out of memory.\n"));
			return -1;
		}
		memset(fix, 0, sizeof(struct inode_dir_block));
		fix->di_addr = ip->i_di.di_num.no_addr;
		osi_list_add_prev((osi_list_t *)&fix->list,
				  (osi_list_t *)&cdpns_to_fix);
	}

	return 0;
}

/*
 * fix_xattr -
 * Extended attributes can be either direct (in the ip->i_di.di_eattr block) or
 * then can be at a maximum of 1 indirect level. Multiple levels of indirection
 * are not supported. If the di_eattr block contains extended attribute data,
 * i.e block type = GFS_METATYPE_EA, we ignore it.
 * If the di_eattr block contains block pointers to extended attributes we need
 * to fix the header. gfs1 uses gfs_indirect as the header which is 64 bytes
 * bigger than gfs2_meta_header that gfs2 uses.
 */
static int fix_xattr(struct gfs2_sbd *sbp, struct gfs2_buffer_head *bh, struct gfs2_inode *ip)
{
	int len, old_hdr_sz, new_hdr_sz;
	struct gfs2_buffer_head *eabh;
	char *buf;

	/* Read in the i_di.di_eattr block */
	eabh = bread(sbp, ip->i_di.di_eattr);
        if (!gfs2_check_meta(eabh, GFS_METATYPE_IN)) {/* if it is an indirect block */
		len = sbp->bsize - sizeof(struct gfs_indirect);
		buf = malloc(len);
		if (!buf) {
			/*FIXME: Same message as fix_cdpn_symlink */
			log_crit(_("Error: out of memory.\n"));
			return -1;
		}
		old_hdr_sz = sizeof(struct gfs_indirect);
		new_hdr_sz = sizeof(struct gfs2_meta_header);
		memcpy(buf, eabh->b_data + old_hdr_sz, sbp->bsize - old_hdr_sz);
		memset(eabh->b_data + new_hdr_sz, 0, sbp->bsize - new_hdr_sz);
		memcpy(eabh->b_data + new_hdr_sz, buf, len);
		free(buf);
		bmodified(eabh);
	}
        brelse(eabh);

	return 0;
}

/* ------------------------------------------------------------------------- */
/* adjust_inode - change an inode from gfs1 to gfs2                          */
/*                                                                           */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int adjust_inode(struct gfs2_sbd *sbp, struct gfs2_buffer_head *bh)
{
	struct gfs2_inode *inode;
	struct inode_block *fixdir;
	int inode_was_gfs1;

	inode = lgfs2_gfs_inode_get(sbp, bh);
	if (inode == NULL) {
		log_crit(_("Error reading inode: %s\n"), strerror(errno));
		return -1;
	}

	inode_was_gfs1 = (inode->i_di.di_num.no_formal_ino ==
					  inode->i_di.di_num.no_addr);
	/* Fix the inode number: */
	inode->i_di.di_num.no_formal_ino = sbp->md.next_inum;

	/* Fix the inode type: gfs1 uses di_type, gfs2 uses di_mode. */
	inode->i_di.di_mode &= ~S_IFMT;
	switch (inode->i_di.__pad1) { /* formerly di_type */
	case GFS_FILE_DIR:           /* directory        */
		inode->i_di.di_mode |= S_IFDIR;
		/* Add this directory to the list of dirs to fix later. */
		fixdir = malloc(sizeof(struct inode_block));
		if (!fixdir) {
			/*FIXME: Same message as fix_cdpn_symlink */
			log_crit(_("Error: out of memory.\n"));
			goto err_freei;
		}
		memset(fixdir, 0, sizeof(struct inode_block));
		fixdir->di_addr = inode->i_di.di_num.no_addr;
		osi_list_add_prev((osi_list_t *)&fixdir->list,
						  (osi_list_t *)&dirs_to_fix);
		break;
	case GFS_FILE_REG:           /* regular file     */
		inode->i_di.di_mode |= S_IFREG;
		break;
	case GFS_FILE_LNK:           /* symlink          */
		inode->i_di.di_mode |= S_IFLNK;
		break;
	case GFS_FILE_BLK:           /* block device     */
		inode->i_di.di_mode |= S_IFBLK;
		break;
	case GFS_FILE_CHR:           /* character device */
		inode->i_di.di_mode |= S_IFCHR;
		break;
	case GFS_FILE_FIFO:          /* fifo / pipe      */
		inode->i_di.di_mode |= S_IFIFO;
		break;
	case GFS_FILE_SOCK:          /* socket           */
		inode->i_di.di_mode |= S_IFSOCK;
		break;
	}

	/* ----------------------------------------------------------- */
	/* gfs2 inodes are slightly different from gfs1 inodes in that */
	/* di_goal_meta has shifted locations and di_goal_data has     */
	/* changed from 32-bits to 64-bits.  The following code        */
	/* adjusts for the shift.                                      */
	/*                                                             */
	/* Note: It may sound absurd, but we need to check if this     */
	/*       inode has already been converted to gfs2 or if it's   */
	/*       still a gfs1 inode.  That's just in case there was a  */
	/*       prior attempt to run gfs2_convert that never finished */
	/*       (due to power out, ctrl-c, kill, segfault, whatever.) */
	/*       If it is unconverted gfs1 we want to do a full        */
	/*       conversion.  If it's a gfs2 inode from a prior run,   */
	/*       we still need to renumber the inode, but here we      */
	/*       don't want to shift the data around.                  */
	/* ----------------------------------------------------------- */
	if (inode_was_gfs1) {
		struct gfs_dinode *gfs1_dinode_struct;
		int ret = 0;

		gfs1_dinode_struct = (struct gfs_dinode *)&inode->i_di;
		inode->i_di.di_goal_meta = inode->i_di.di_goal_data;
		inode->i_di.di_goal_data = 0; /* make sure the upper 32b are 0 */
		inode->i_di.di_goal_data = gfs1_dinode_struct->di_goal_dblk;
		inode->i_di.di_generation = 0;

		if (adjust_indirect_blocks(sbp, inode))
			goto err_freei;
		/* Check for cdpns */
		if (S_ISLNK(inode->i_di.di_mode)) {
			ret = fix_cdpn_symlink(sbp, bh, inode);
			if (ret)
				goto err_freei;
		}
		/* Check for extended attributes */
		if (inode->i_di.di_eattr) {
			ret = fix_xattr(sbp, bh, inode);
			if (ret)
				goto err_freei;
		}
	}

	bmodified(inode->i_bh);
	inode_put(&inode); /* does gfs2_dinode_out if modified */
	sbp->md.next_inum++; /* update inode count */
	return 0;
err_freei:
	inode_put(&inode);
	return -1;
} /* adjust_inode */

static int next_rg_meta(struct rgrp_tree *rgd, uint64_t *block, int first)
{
	struct gfs2_bitmap *bits = NULL;
	uint32_t length = rgd->ri.ri_length;
	uint32_t blk = (first)? 0: (uint32_t)((*block + 1) - rgd->ri.ri_data0);
	int i;

	if (!first && (*block < rgd->ri.ri_data0)) {
		fprintf(stderr, "next_rg_meta:  Start block is outside rgrp bounds.\n");
		exit(1);
	}
	for (i = 0; i < length; i++){
		bits = &rgd->bits[i];
		if (blk < bits->bi_len * GFS2_NBBY)
			break;
		blk -= bits->bi_len * GFS2_NBBY;
	}
	for (; i < length; i++){
		bits = &rgd->bits[i];
		blk = gfs2_bitfit((uint8_t *)bits->bi_bh->b_data + bits->bi_offset,
		                   bits->bi_len, blk, GFS2_BLKST_DINODE);
		if(blk != BFITNOENT){
			*block = blk + (bits->bi_start * GFS2_NBBY) +
				rgd->ri.ri_data0;
			break;
		}
		blk = 0;
	}
	if (i == length)
		return -1;
	return 0;
}

static int next_rg_metatype(struct gfs2_sbd *sdp, struct rgrp_tree *rgd,
                            uint64_t *block, uint32_t type, int first)
{
	struct gfs2_buffer_head *bh = NULL;

	do{
		if (bh)
			brelse(bh);
		if (next_rg_meta(rgd, block, first))
			return -1;
		bh = bread(sdp, *block);
		first = 0;
	} while(gfs2_check_meta(bh, type));
	brelse(bh);
	return 0;
}

/* ------------------------------------------------------------------------- */
/* inode_renumber - renumber the inodes                                      */
/*                                                                           */
/* In gfs1, the inode number WAS the inode address.  In gfs2, the inodes are */
/* numbered sequentially.                                                    */
/*                                                                           */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int inode_renumber(struct gfs2_sbd *sbp, uint64_t root_inode_addr, osi_list_t *cdpn_to_fix)
{
	struct rgrp_tree *rgd;
	struct osi_node *n, *next = NULL;
	uint64_t block = 0;
	struct gfs2_buffer_head *bh;
	int first;
	int error = 0;
	int rgs_processed = 0;

	log_notice(_("Converting inodes.\n"));
	sbp->md.next_inum = 1; /* starting inode numbering */
	gettimeofday(&tv, NULL);
	seconds = tv.tv_sec;

	/* ---------------------------------------------------------------- */
	/* Traverse the resource groups to figure out where the inodes are. */
	/* ---------------------------------------------------------------- */
	for (n = osi_first(&sbp->rgtree); n; n = next) {
		next = osi_next(n);
		rgd = (struct rgrp_tree *)n;
		rgs_processed++;
		first = 1;
		while (1) {    /* for all inodes in the resource group */
			gettimeofday(&tv, NULL);
			/* Put out a warm, fuzzy message every second so the customer */
			/* doesn't think we hung.  (This may take a long time).       */
			if (tv.tv_sec - seconds) {
				seconds = tv.tv_sec;
				log_notice(_("\r%llu inodes from %d rgs converted."),
					   (unsigned long long)sbp->md.next_inum,
					   rgs_processed);
				fflush(stdout);
			}
			/* Get the next metadata block.  Break out if we reach the end. */
			/* We have to check all metadata blocks because the bitmap may  */
			/* be "11" (used meta) for both inodes and indirect blocks.     */
			/* We need to process the inodes and change the indirect blocks */
			/* to have a bitmap type of "01" (data).                        */
			if (next_rg_metatype(sbp, rgd, &block, 0, first))
				break;
			/* If this is the root inode block, remember it for later: */
			if (block == root_inode_addr) {
				sbp->sd_sb.sb_root_dir.no_addr = block;
				sbp->sd_sb.sb_root_dir.no_formal_ino = sbp->md.next_inum;
			}
			bh = bread(sbp, block);
			if (!gfs2_check_meta(bh, GFS_METATYPE_DI)) {/* if it is an dinode */
				/* Skip the rindex and jindex inodes for now. */
				if (block != rindex_addr && block != jindex_addr) {
					error = adjust_inode(sbp, bh);
					if (error)
						return error;
				}
			} else { /* It's metadata, but not an inode, so fix the bitmap. */
				int blk, buf_offset;
				int bitmap_byte; /* byte within the bitmap to fix */
				int byte_bit; /* bit within the byte */

				/* Figure out the absolute bitmap byte we need to fix.   */
				/* ignoring structure offsets and bitmap blocks for now. */
				bitmap_byte = (block - rgd->ri.ri_data0) / GFS2_NBBY;
				byte_bit = (block - rgd->ri.ri_data0) % GFS2_NBBY;
				/* Now figure out which bitmap block the byte is on */
				for (blk = 0; blk < rgd->ri.ri_length; blk++) {
					struct gfs2_bitmap *bi = &rgd->bits[blk];
					/* figure out offset of first bitmap byte for this map: */
					buf_offset = (blk) ? sizeof(struct gfs2_meta_header) :
						sizeof(struct gfs2_rgrp);
					/* if it's on this page */
					if (buf_offset + bitmap_byte < sbp->bsize) {
						bi->bi_bh->b_data[buf_offset + bitmap_byte] &=
							~(0x03 << (GFS2_BIT_SIZE * byte_bit));
						bi->bi_bh->b_data[buf_offset + bitmap_byte] |=
							(0x01 << (GFS2_BIT_SIZE * byte_bit));
						bmodified(bi->bi_bh);
						break;
					}
					bitmap_byte -= (sbp->bsize - buf_offset);
				}
			}
			brelse(bh);
			first = 0;
		} /* while 1 */
	} /* for all rgs */
	log_notice(_("\r%llu inodes from %d rgs converted."),
		   (unsigned long long)sbp->md.next_inum, rgs_processed);
	fflush(stdout);
	return 0;
}/* inode_renumber */

/* ------------------------------------------------------------------------- */
/* fetch_inum - fetch an inum entry from disk, given its block               */
/* ------------------------------------------------------------------------- */
static int fetch_inum(struct gfs2_sbd *sbp, uint64_t iblock,
		      struct gfs2_inum *inum, uint64_t *eablk)
{
	struct gfs2_inode *fix_inode;

	fix_inode = lgfs2_inode_read(sbp, iblock);
	if (fix_inode == NULL)
		return 1;
	inum->no_formal_ino = fix_inode->i_di.di_num.no_formal_ino;
	inum->no_addr = fix_inode->i_di.di_num.no_addr;
	if (eablk)
		*eablk = fix_inode->i_di.di_eattr;

	inode_put(&fix_inode);
	return 0;
}/* fetch_inum */

/* ------------------------------------------------------------------------- */
/* process_dirent_info - fix one dirent (directory entry) buffer             */
/*                                                                           */
/* We changed inode numbers, so we must update that number into the          */
/* directory entries themselves.                                             */
/*                                                                           */
/* Returns: 0 on success, -1 on failure, -EISDIR when dentmod marked DT_DIR  */
/* ------------------------------------------------------------------------- */
static int process_dirent_info(struct gfs2_inode *dip, struct gfs2_sbd *sbp,
			       struct gfs2_buffer_head *bh, int dir_entries, uint64_t dentmod)
{
	int error = 0;
	struct gfs2_dirent *dent;
	int de; /* directory entry index */
	
	error = gfs2_dirent_first(dip, bh, &dent);
	if (error != IS_LEAF && error != IS_DINODE) {
		log_crit(_("Error retrieving directory.\n"));
		return -1;
	}
	error = 0;
	/* Go through every dirent in the buffer and process it. */
	/* Turns out you can't trust dir_entries is correct.     */
	for (de = 0; ; de++) {
		struct gfs2_inum inum;
		int dent_was_gfs1;

		if (dentmod) {
			if (dent->de_type == cpu_to_be16(DT_LNK)
			    && cpu_to_be64(dent->de_inum.no_addr) == dentmod) {
				dent->de_type = cpu_to_be16(DT_DIR);
				error = -EISDIR;
				break;
			}
			goto skip_next;
		}

		gettimeofday(&tv, NULL);
		/* Do more warm fuzzy stuff for the customer. */
		dirents_fixed++;
		if (tv.tv_sec - seconds) {
			seconds = tv.tv_sec;
			log_notice(_("\r%llu directories, %llu dirents fixed."),
				   (unsigned long long)dirs_fixed,
				   (unsigned long long)dirents_fixed);
			fflush(stdout);
		}
		/* fix the dirent's inode number based on the inode */
		gfs2_inum_in(&inum, (char *)&dent->de_inum);
		dent_was_gfs1 = (dent->de_inum.no_addr == dent->de_inum.no_formal_ino);
		if (inum.no_formal_ino) { /* if not a sentinel (placeholder) */
			error = fetch_inum(sbp, inum.no_addr, &inum, NULL);
			if (error) {
				log_crit(_("Error retrieving inode 0x%llx\n"),
					 (unsigned long long)inum.no_addr);
				break;
			}
			/* fix the dirent's inode number from the fetched inum. */
			dent->de_inum.no_formal_ino = cpu_to_be64(inum.no_formal_ino);
		}
		/* Fix the dirent's filename hash: They are the same as gfs1 */
		/* dent->de_hash = cpu_to_be32(gfs2_disk_hash((char *)(dent + 1), */
		/*                             be16_to_cpu(dent->de_name_len))); */
		/* Fix the dirent's file type.  Gfs1 used home-grown values.  */
		/* Gfs2 uses standard values from include/linux/fs.h          */
		/* Only do this if the dent was a true gfs1 dent, and not a   */
		/* gfs2 dent converted from a previously aborted run.         */
		if (dent_was_gfs1) {
			switch be16_to_cpu(dent->de_type) {
			case GFS_FILE_NON:
				dent->de_type = cpu_to_be16(DT_UNKNOWN);
				break;
			case GFS_FILE_REG:    /* regular file */
				dent->de_type = cpu_to_be16(DT_REG);
				break;
			case GFS_FILE_DIR:    /* directory */
				dent->de_type = cpu_to_be16(DT_DIR);
				break;
			case GFS_FILE_LNK:    /* link */
				dent->de_type = cpu_to_be16(DT_LNK);
				break;
			case GFS_FILE_BLK:    /* block device node */
				dent->de_type = cpu_to_be16(DT_BLK);
				break;
			case GFS_FILE_CHR:    /* character device node */
				dent->de_type = cpu_to_be16(DT_CHR);
				break;
			case GFS_FILE_FIFO:   /* fifo/pipe */
				dent->de_type = cpu_to_be16(DT_FIFO);
				break;
			case GFS_FILE_SOCK:   /* socket */
				dent->de_type = cpu_to_be16(DT_SOCK);
				break;
			}
		}
		/*
		 * Compare this dirent address with every one in the
		 * cdpns_to_fix list to find if this directory (dip) is
		 * a cdpn symlink's parent. If so add it to the list element
		 */
		if (dent->de_type == cpu_to_be16(DT_LNK)) {
			osi_list_t *tmp;
			struct inode_dir_block *fix;
			osi_list_foreach(tmp, &cdpns_to_fix.list) {
				fix = osi_list_entry(tmp, struct inode_dir_block, list);
				if (fix->di_addr == inum.no_addr)
					fix->di_paddr = dip->i_di.di_num.no_addr;
			}
		}

	skip_next:
		error = gfs2_dirent_next(dip, bh, &dent);
		if (error) {
			if (error == -ENOENT) /* beyond the end of this bh */
				error = 0;
			break;
		}
	} /* for every directory entry */
	return error;
}/* process_dirent_info */

/* ------------------------------------------------------------------------- */
/* fix_one_directory_exhash - fix one directory's inode numbers.             */
/*                                                                           */
/* This is for exhash directories, where the inode has a list of "leaf"      */
/* blocks, each of which is a buffer full of dirents that must be processed. */
/*                                                                           */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int fix_one_directory_exhash(struct gfs2_sbd *sbp, struct gfs2_inode *dip, uint64_t dentmod)
{
	struct gfs2_buffer_head *bh_leaf;
	int error;
	uint64_t leaf_block, prev_leaf_block;
	uint32_t leaf_num;
	
	prev_leaf_block = 0;
	/* for all the leafs, get the leaf block and process the dirents inside */
	for (leaf_num = 0; ; leaf_num++) {
		uint64_t buf;
		struct gfs2_leaf leaf;

		error = gfs2_readi(dip, (char *)&buf, leaf_num * sizeof(uint64_t),
						   sizeof(uint64_t));
		if (!error) /* end of file */
			return 0; /* success */
		else if (error != sizeof(uint64_t)) {
			log_crit(_("fix_one_directory_exhash: error reading directory.\n"));
			return -1;
		}
		else {
			leaf_block = be64_to_cpu(buf);
			error = 0;
		}
	leaf_chain:
		/* leaf blocks may be repeated, so skip the duplicates: */
		if (leaf_block == prev_leaf_block) /* same block? */
			continue;                      /* already converted */

		prev_leaf_block = leaf_block;
		/* read the leaf buffer in */
		error = gfs2_get_leaf(dip, leaf_block, &bh_leaf);
		if (error) {
			log_crit(_("Error reading leaf %llx\n"),
				 (unsigned long long)leaf_block);
			break;
		}
		gfs2_leaf_in(&leaf, bh_leaf->b_data);
		error = process_dirent_info(dip, sbp, bh_leaf, leaf.lf_entries, dentmod);
		bmodified(bh_leaf);
		brelse(bh_leaf);
		if (dentmod && error == -EISDIR) /* dentmod was marked DT_DIR, break out */
			break;
		if (leaf.lf_next) { /* leaf has a leaf chain, process leaves in chain */
			leaf_block = leaf.lf_next;
			error = 0;
			goto leaf_chain;
		}
	} /* for leaf_num */
	return 0;
}/* fix_one_directory_exhash */

static int process_directory(struct gfs2_sbd *sbp, uint64_t dirblock, uint64_t dentmod)
{
	struct gfs2_inode *dip;
	int error = 0;
	/* read in the directory inode */
	dip = lgfs2_inode_read(sbp, dirblock);
	if (dip == NULL)
		return -1;
	/* fix the directory: either exhash (leaves) or linear (stuffed) */
	if (dip->i_di.di_flags & GFS2_DIF_EXHASH) {
		if (fix_one_directory_exhash(sbp, dip, dentmod)) {
			log_crit(_("Error fixing exhash directory.\n"));
			inode_put(&dip);
			return -1;
		}
	} else {
		error = process_dirent_info(dip, sbp, dip->i_bh, dip->i_di.di_entries, dentmod);
		if (error && error != -EISDIR) {
			log_crit(_("Error fixing linear directory.\n"));
			inode_put(&dip);
			return -1;
		}
	}
	bmodified(dip->i_bh);
	inode_put(&dip);
	return 0;
}
/* ------------------------------------------------------------------------- */
/* fix_directory_info - sync new inode numbers with directory info           */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int fix_directory_info(struct gfs2_sbd *sbp, osi_list_t *dir_to_fix)
{
	osi_list_t *tmp, *fix;
	struct inode_block *dir_iblk;
	uint64_t dirblock;
	uint32_t gfs1_inptrs = sbp->sd_inptrs;
	/* Directory inodes have been converted to gfs2, use gfs2 inptrs */
	sbp->sd_inptrs = (sbp->bsize - sizeof(struct gfs2_meta_header))
		/ sizeof(uint64_t);

	dirs_fixed = 0;
	dirents_fixed = 0;
	gettimeofday(&tv, NULL);
	seconds = tv.tv_sec;
	log_notice(_("\nFixing file and directory information.\n"));
	fflush(stdout);
	tmp = NULL;
	/* for every directory in the list */
	for (fix = dir_to_fix->next; fix != dir_to_fix; fix = fix->next) {
		if (tmp) {
			osi_list_del(tmp);
			free(tmp);
		}
		tmp = fix; /* remember the addr to free next time */
		dirs_fixed++;
		/* figure out the directory inode block and read it in */
		dir_iblk = (struct inode_block *)fix;
		dirblock = dir_iblk->di_addr; /* addr of dir inode */
		if (process_directory(sbp, dirblock, 0)) {
			log_crit(_("Error processing directory\n"));
			return -1;
		}
	}
	/* Free the last entry in memory: */
	if (tmp) {
		osi_list_del(tmp);
		free(tmp);
	}
	sbp->sd_inptrs = gfs1_inptrs;
	return 0;
}/* fix_directory_info */

/* ------------------------------------------------------------------------- */
/* fix_cdpn_symlinks - convert cdpn symlinks to empty directories            */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int fix_cdpn_symlinks(struct gfs2_sbd *sbp, osi_list_t *cdpn_to_fix)
{
	osi_list_t *tmp, *x;
	int error = 0;

	cdpns_fixed = 0;
	osi_list_foreach_safe(tmp, cdpn_to_fix, x) {
		struct gfs2_inum fix, dir;
		struct inode_dir_block *l_fix;
		struct gfs2_buffer_head *bh = NULL;
		struct gfs2_inode *fix_inode;
		uint64_t eablk;

		l_fix = osi_list_entry(tmp, struct inode_dir_block, list);
		osi_list_del(tmp);

		/* convert symlink to empty dir */
		error = fetch_inum(sbp, l_fix->di_addr, &fix, &eablk);
		if (error) {
			log_crit(_("Error retrieving inode at block %llx\n"), 
				 (unsigned long long)l_fix->di_addr);
			break;
		}
		error = fetch_inum(sbp, l_fix->di_paddr, &dir, NULL);
		if (error) {
			log_crit(_("Error retrieving inode at block %llx\n"),
				 (unsigned long long)l_fix->di_paddr);
			break;
		}

		/* initialize the symlink inode to be a directory */
		error = init_dinode(sbp, &bh, &fix, S_IFDIR | 0755, 0, &dir);
		if (error != 0)
			return -1;

		fix_inode = lgfs2_inode_get(sbp, bh);
		if (fix_inode == NULL)
			return -1;
		fix_inode->i_di.di_eattr = eablk; /*fix extended attribute */
		inode_put(&fix_inode);
		bmodified(bh);
		brelse(bh);

		/* fix the parent directory dirent entry for this inode */
		error = process_directory(sbp, l_fix->di_paddr, l_fix->di_addr);
		if (error) {
			log_crit(_("Error trying to fix cdpn dentry\n"));
			break;
		}
		free(l_fix);
		cdpns_fixed++;
	}
	return error;
} /* fix_cdpn_symlinks */

/* ------------------------------------------------------------------------- */
/* Fetch gfs1 jindex structure from buffer                                   */
/* ------------------------------------------------------------------------- */
static void gfs1_jindex_in(struct gfs_jindex *jindex, char *buf)
{
	struct gfs_jindex *str = (struct gfs_jindex *)buf;

	jindex->ji_addr = be64_to_cpu(str->ji_addr);
	jindex->ji_nsegment = be32_to_cpu(str->ji_nsegment);
	memset(jindex->ji_reserved, 0, 64);
}

/* ------------------------------------------------------------------------- */
/* read_gfs1_jiindex - read the gfs1 jindex file.                            */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int read_gfs1_jiindex(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip = sdp->md.jiinode;
	char buf[sizeof(struct gfs_jindex)];
	unsigned int j;
	int error=0;
	unsigned int tmp_mode = 0;

	if(ip->i_di.di_size % sizeof(struct gfs_jindex) != 0){
		log_crit(_("The size reported in the journal index"
				" inode is not a\n"
				"\tmultiple of the size of a journal index.\n"));
		return -1;
	}
	if(!(sd_jindex = (struct gfs_jindex *)malloc(ip->i_di.di_size))) {
		log_crit(_("Unable to allocate journal index\n"));
		return -1;
	}
	if(!memset(sd_jindex, 0, ip->i_di.di_size)) {
		log_crit(_("Unable to zero journal index\n"));
		return -1;
	}
	/* ugly hack
	 * Faking the gfs1_jindex inode as a directory to gfs2_readi
	 * so it skips the metaheader struct in the data blocks
	 * in the inode. gfs2_jindex inode doesn't have metaheaders
	 * in the data blocks */
	tmp_mode = ip->i_di.di_mode;
	ip->i_di.di_mode &= ~S_IFMT;
	ip->i_di.di_mode |= S_IFDIR;
	for (j = 0; ; j++) {
		struct gfs_jindex *journ;

		error = gfs2_readi(ip, buf, j * sizeof(struct gfs_jindex),
						   sizeof(struct gfs_jindex));
		if(!error)
			break;
		if (error != sizeof(struct gfs_jindex)){
			log_crit(_("An error occurred while reading the"
					" journal index file.\n"));
			goto fail;
		}
		journ = sd_jindex + j;
		gfs1_jindex_in(journ, buf);
		sdp->jsize = (journ->ji_nsegment * 16 * sdp->bsize) >> 20;
	}
	ip->i_di.di_mode = tmp_mode;
	if(j * sizeof(struct gfs_jindex) != ip->i_di.di_size){
		log_crit(_("journal inode size invalid\n"));
		goto fail;
	}
	sdp->md.journals = orig_journals = j;
	return 0;

 fail:
	free(sd_jindex);
	return -1;
}

static int sanity_check(struct gfs2_sbd *sdp)
{
	int error = 0;
	if (!raw_gfs1_ondisk_sb.sb_quota_di.no_addr) {
		log_crit(_("Error: Superblock Quota inode address is NULL\n"));
		error = 1;
	}
	if (!raw_gfs1_ondisk_sb.sb_license_di.no_addr) {
		log_crit(_("Error: Superblock Statfs inode address is NULL\n"));
		error = 1;
	}
	if (!raw_gfs1_ondisk_sb.sb_seg_size) {
		log_crit(_("Error: Superblock segment size is zero\n"));
		error = 1;
	}
	return error;
}

/* ------------------------------------------------------------------------- */
/* init - initialization code                                                */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int init(struct gfs2_sbd *sbp, struct gfs2_options *opts)
{
	struct gfs2_buffer_head *bh;
	int rgcount;
	struct gfs2_inum inum;

	memset(sbp, 0, sizeof(struct gfs2_sbd));
	if ((sbp->device_fd = open(opts->device, O_RDWR)) < 0) {
		perror(opts->device);
		exit(-1);
	}
	/* --------------------------------- */
	/* initialize the incore superblock  */
	/* --------------------------------- */
	sbp->sd_sb.sb_header.mh_magic = GFS2_MAGIC;
	sbp->sd_sb.sb_header.mh_type = GFS2_METATYPE_SB;
	sbp->sd_sb.sb_header.mh_format = GFS2_FORMAT_SB;

	osi_list_init((osi_list_t *)&dirs_to_fix);
	osi_list_init((osi_list_t *)&cdpns_to_fix);
	/* ---------------------------------------------- */
	/* Initialize lists and read in the superblock.   */
	/* ---------------------------------------------- */
	sbp->jsize = GFS2_DEFAULT_JSIZE;
	sbp->rgsize = GFS2_DEFAULT_RGSIZE;
	sbp->qcsize = GFS2_DEFAULT_QCSIZE;
	sbp->time = time(NULL);
	sbp->blks_total = 0;   /* total blocks         - total them up later */
	sbp->blks_alloced = 0; /* blocks allocated     - total them up later */
	sbp->dinodes_alloced = 0; /* dinodes allocated - total them up later */
	sbp->sd_sb.sb_bsize = GFS2_DEFAULT_BSIZE;
	sbp->bsize = sbp->sd_sb.sb_bsize;
	sbp->rgtree.osi_node = NULL;
	if (compute_constants(sbp)) {
		log_crit("%s\n", _("Failed to compute file system constants"));
		exit(-1);
	}

	bh = bread(sbp, GFS2_SB_ADDR >> sbp->sd_fsb2bb_shift);
	memcpy(&raw_gfs1_ondisk_sb, (struct gfs_sb *)bh->b_data,
		   sizeof(struct gfs_sb));
	gfs2_sb_in(&sbp->sd_sb, bh->b_data);

	jindex_addr = be64_to_cpu(raw_gfs1_ondisk_sb.sb_jindex_di.no_addr);
	rindex_addr = be64_to_cpu(raw_gfs1_ondisk_sb.sb_rindex_di.no_addr);

	sbp->bsize = sbp->sd_sb.sb_bsize;
	sbp->fssize = lseek(sbp->device_fd, 0, SEEK_END) / sbp->sd_sb.sb_bsize;
	sbp->sd_inptrs = (sbp->bsize - sizeof(struct gfs_indirect)) /
		sizeof(uint64_t);
	sbp->sd_diptrs = (sbp->bsize - sizeof(struct gfs_dinode)) /
		sizeof(uint64_t);
	sbp->sd_jbsize = sbp->bsize - sizeof(struct gfs2_meta_header);
	brelse(bh);
	if (compute_heightsize(sbp->bsize, sbp->sd_heightsize, &sbp->sd_max_height,
				sbp->bsize, sbp->sd_diptrs, sbp->sd_inptrs)) {
		log_crit("%s\n", _("Failed to compute file system constants"));
		exit(-1);
	}

	if (compute_heightsize(sbp->bsize, sbp->sd_jheightsize, &sbp->sd_max_jheight,
				sbp->sd_jbsize, sbp->sd_diptrs, sbp->sd_inptrs)) {
		log_crit("%s\n", _("Failed to compute file system constants"));
		exit(-1);
	}
	/* -------------------------------------------------------- */
	/* Our constants are for gfs1.  Need some for gfs2 as well. */
	/* -------------------------------------------------------- */
	gfs2_inptrs = (sbp->bsize - sizeof(struct gfs2_meta_header)) /
                sizeof(uint64_t); /* How many ptrs can we fit on a block? */
	memset(gfs2_heightsize, 0, sizeof(gfs2_heightsize));
	if (compute_heightsize(sbp->bsize, gfs2_heightsize, &gfs2_max_height,
				sbp->bsize, sbp->sd_diptrs, gfs2_inptrs)) {
		log_crit("%s\n", _("Failed to compute file system constants"));
		exit(-1);
	}
	memset(gfs2_jheightsize, 0, sizeof(gfs2_jheightsize));
	if (compute_heightsize(sbp->bsize, gfs2_jheightsize, &gfs2_max_jheight,
				sbp->sd_jbsize, sbp->sd_diptrs, gfs2_inptrs)) {
		log_crit("%s\n", _("Failed to compute file system constants"));
		exit(-1);
	}

	/* ---------------------------------------------- */
	/* Make sure we're really gfs1                    */
	/* ---------------------------------------------- */
	if (sbp->sd_sb.sb_fs_format != GFS_FORMAT_FS ||
		sbp->sd_sb.sb_header.mh_type != GFS_METATYPE_SB ||
		sbp->sd_sb.sb_header.mh_format != GFS_FORMAT_SB ||
		sbp->sd_sb.sb_multihost_format != GFS_FORMAT_MULTI) {
		log_crit(_("Error: %s does not look like a gfs1 filesystem.\n"), opts->device);
		close(sbp->device_fd);
		exit(-1);
	}
	/* get gfs1 rindex inode - gfs1's rindex inode ptr became __pad2 */
	gfs2_inum_in(&inum, (char *)&raw_gfs1_ondisk_sb.sb_rindex_di);
	sbp->md.riinode = lgfs2_gfs_inode_read(sbp, inum.no_addr);
	if (sbp->md.riinode == NULL) {
		log_crit(_("Could not read resource group index: %s\n"), strerror(errno));
		exit(-1);
	}
	/* get gfs1 jindex inode - gfs1's journal index inode ptr became master */
	gfs2_inum_in(&inum, (char *)&raw_gfs1_ondisk_sb.sb_jindex_di);
	sbp->md.jiinode = lgfs2_inode_read(sbp, inum.no_addr);
	if (sbp->md.jiinode == NULL) {
		log_crit(_("Could not read journal index: %s\n"), strerror(errno));
		exit(-1);
	}
	/* read in the journal index data */
	read_gfs1_jiindex(sbp);
	/* read in the resource group index data: */

	/* We've got a slight dilemma here.  In gfs1, we used to have a meta */
	/* header in front of the rgindex pages.  In gfs2, we don't.  That's */
	/* apparently only for directories.  So we need to fake out libgfs2  */
	/* so that it adjusts for the metaheader by faking out the inode to  */
	/* look like a directory, temporarily.                               */
	sbp->md.riinode->i_di.di_mode &= ~S_IFMT;
	sbp->md.riinode->i_di.di_mode |= S_IFDIR;
	printf(_("Examining file system"));
	if (gfs1_ri_update(sbp, 0, &rgcount, 0)){
		log_crit(_("Unable to fill in resource group information.\n"));
		return -1;
	}
	printf("\n");
	fflush(stdout);
	inode_put(&sbp->md.riinode);
	inode_put(&sbp->md.jiinode);
	log_debug(_("%d rgs found.\n"), rgcount);
	return 0;
}/* fill_super_block */

/* ------------------------------------------------------------------------- */
/* give_warning - give the all-important warning message.                    */
/* ------------------------------------------------------------------------- */
static void give_warning(void)
{
	printf(_("This program will convert a gfs1 filesystem to a "	\
		   "gfs2 filesystem.\n"));
	printf(_("WARNING: This can't be undone.  It is strongly advised "	\
		   "that you:\n\n"));
	printf(_("   1. Back up your entire filesystem first.\n"));
	printf(_("   2. Run fsck.gfs2 first to ensure filesystem integrity.\n"));
	printf(_("   3. Make sure the filesystem is NOT mounted from any node.\n"));
	printf(_("   4. Make sure you have the latest software versions.\n"));
}/* give_warning */

/* ------------------------------------------------------------------------- */
/* version  - print version information                                      */
/* ------------------------------------------------------------------------- */
static void version(void)
{
	log_notice(_("gfs2_convert version %s (built %s %s)\n"), VERSION,
			   __DATE__, __TIME__);
	log_notice("%s\n\n", REDHAT_COPYRIGHT);
}

/* ------------------------------------------------------------------------- */
/* usage - print usage information                                           */
/* ------------------------------------------------------------------------- */
static void usage(const char *name)
{
	give_warning();
	printf(_("\nUsage:\n"));
	printf(_("%s [-hnqvVy] <device>\n\n"), name);
	printf("Flags:\n");
	printf(_("\th - print this help message\n"));
	printf(_("\tn - assume 'no' to all questions\n"));
	printf(_("\tq - quieter output\n"));
	printf(_("\tv - more verbose output\n"));
	printf(_("\tV - print version information\n"));
	printf(_("\ty - assume 'yes' to all questions\n"));
}/* usage */

/* ------------------------------------------------------------------------- */
/* process_parameters                                                        */
/* ------------------------------------------------------------------------- */
static void process_parameters(int argc, char **argv, struct gfs2_options *opts)

{
	int c;

	opts->yes = 0;
	opts->no = 0;
	if (argc == 1) {
		usage(argv[0]);
		exit(0);
	}
	while((c = getopt(argc, argv, "hnqvyV")) != -1) {
		switch(c) {

		case 'h':
			usage(argv[0]);
			exit(0);
			break;
		case 'n':
			opts->no = 1;
			break;
		case 'q':
			decrease_verbosity();
			break;
		case 'v':
			increase_verbosity();
			break;
		case 'V':
			exit(0);
		case 'y':
			opts->yes = 1;
			break;
		default:
			fprintf(stderr,_("Parameter not understood: %c\n"), c);
			usage(argv[0]);
			exit(0);
		}
	}
	if(argc > optind) {
		opts->device = argv[optind];
	} else {
		fprintf(stderr, _("No device specified. Please use '-h' for help\n"));
		exit(1);
	}
} /* process_parameters */

/* ------------------------------------------------------------------------- */
/* rgrp_length - Calculate the length of a resource group                    */
/* @size: The total size of the resource group                               */
/* ------------------------------------------------------------------------- */
static uint64_t rgrp_length(uint64_t size, struct gfs2_sbd *sdp)
{
	uint64_t bitbytes = RGRP_BITMAP_BLKS(&sdp->sd_sb) + 1;
	uint64_t stuff = RGRP_STUFFED_BLKS(&sdp->sd_sb) + 1;
	uint64_t blocks = 1;

	if (size >= stuff) {
		size -= stuff;
		while (size > bitbytes) {
			blocks++;
			size -= bitbytes;
		}
		if (size)
			blocks++;
	}
	return blocks;
}/* rgrp_length */

/* ------------------------------------------------------------------------- */
/* journ_space_to_rg - convert gfs1 journal space to gfs2 rg space.          */
/*                                                                           */
/* In gfs1, the journals were kept separate from the files and directories.  */
/* They had a dedicated section of the fs carved out for them.               */
/* In gfs2, the journals are just files like any other, (but still hidden).  */
/* Therefore, the old journal space has to be converted to normal resource   */
/* group space.                                                              */
/*                                                                           */
/* Returns: 0 on success, -1 on failure                                      */
/* ------------------------------------------------------------------------- */
static int journ_space_to_rg(struct gfs2_sbd *sdp)
{
	int error = 0;
	int j, x;
	struct gfs_jindex *jndx;
	struct rgrp_tree *rgd, *rgdhigh;
	struct osi_node *n, *next = NULL;
	struct gfs2_meta_header mh;
	uint64_t ri_addr;

	mh.mh_magic = GFS2_MAGIC;
	mh.mh_type = GFS2_METATYPE_RB;
	mh.mh_format = GFS2_FORMAT_RB;
	log_notice(_("Converting journal space to rg space.\n"));
	/* Go through each journal, converting them one by one */
	for (j = 0; j < orig_journals; j++) { /* for each journal */
		uint64_t size;

		jndx = &sd_jindex[j];
		/* go through all rg index entries, keeping track of the
		   highest that's still in the first subdevice.
		   Note: we really should go through all of the rgindex because
		   we might have had rg's added by gfs_grow, and journals added
		   by jadd.  gfs_grow adds rgs out of order, so we can't count
		   on them being in ascending order. */
		rgdhigh = NULL;
		for (n = osi_first(&sdp->rgtree); n; n = next) {
			next = osi_next(n);
			rgd = (struct rgrp_tree *)n;
			if (rgd->ri.ri_addr < jndx->ji_addr &&
				((rgdhigh == NULL) ||
				 (rgd->ri.ri_addr > rgdhigh->ri.ri_addr)))
				rgdhigh = rgd;
		} /* for each rg */
		if (!rgdhigh) { /* if we somehow didn't find one. */
			log_crit(_("Error: No suitable rg found for journal.\n"));
			return -1;
		}
		log_info(_("Addr 0x%llx comes after rg at addr 0x%llx\n"),
			 (unsigned long long)jndx->ji_addr,
			 (unsigned long long)rgdhigh->ri.ri_addr);
		ri_addr = jndx->ji_addr;
		/* Allocate a new rgd entry which includes rg and ri. */
		rgd = rgrp_insert(&sdp->rgtree, ri_addr);
		/* convert the gfs1 rgrp into a new gfs2 rgrp */
		size = jndx->ji_nsegment *
			be32_to_cpu(raw_gfs1_ondisk_sb.sb_seg_size);
		rgd->rg.rg_header.mh_magic = GFS2_MAGIC;
		rgd->rg.rg_header.mh_type = GFS2_METATYPE_RG;
		rgd->rg.rg_header.mh_format = GFS2_FORMAT_RG;
		rgd->rg.rg_flags = 0;
		rgd->rg.rg_dinodes = 0;

		rgd->ri.ri_addr = jndx->ji_addr; /* new rg addr becomes ji addr */
		rgd->ri.ri_length = rgrp_length(size, sdp); /* aka bitblocks */

		rgd->ri.ri_data0 = jndx->ji_addr + rgd->ri.ri_length;
		rgd->ri.ri_data = size - rgd->ri.ri_length;
		/* Round down to nearest multiple of GFS2_NBBY */
		while (rgd->ri.ri_data & 0x03)
			rgd->ri.ri_data--;
		sdp->blks_total += rgd->ri.ri_data; /* For statfs file update */
		rgd->rg.rg_free = rgd->ri.ri_data;
		rgd->ri.ri_bitbytes = rgd->ri.ri_data / GFS2_NBBY;

		if (gfs2_compute_bitstructs(sdp->sd_sb.sb_bsize, rgd)) {
			log_crit(_("gfs2_convert: Error converting bitmaps.\n"));
			exit(-1);
		}

		for (x = 0; x < rgd->ri.ri_length; x++)
			rgd->bits[x].bi_bh = bget(sdp, rgd->ri.ri_addr + x);

		convert_bitmaps(sdp, rgd);
		for (x = 0; x < rgd->ri.ri_length; x++) {
			if (x)
				gfs2_meta_header_out(&mh, rgd->bits[x].bi_bh->b_data);
			else
				gfs2_rgrp_out(&rgd->rg, rgd->bits[x].bi_bh->b_data);
			bmodified(rgd->bits[x].bi_bh);
		}
	} /* for each journal */
	return error;
}/* journ_space_to_rg */

/* ------------------------------------------------------------------------- */
/* update_inode_file - update the inode file with the new next_inum          */
/* ------------------------------------------------------------------------- */
static void update_inode_file(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip = sdp->md.inum;
	uint64_t buf;
	int count;
	
	buf = cpu_to_be64(sdp->md.next_inum);
	count = gfs2_writei(ip, &buf, 0, sizeof(uint64_t));
	if (count != sizeof(uint64_t)) {
		fprintf(stderr, "update_inode_file\n");
		exit(1);
	}
	
	log_debug(_("\nNext Inum: %llu\n"), (unsigned long long)sdp->md.next_inum);
}/* update_inode_file */

/* ------------------------------------------------------------------------- */
/* write_statfs_file - write the statfs file                                 */
/* ------------------------------------------------------------------------- */
static void write_statfs_file(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip = sdp->md.statfs;
	struct gfs2_statfs_change sc;
	char buf[sizeof(struct gfs2_statfs_change)];
	int count;

	sc.sc_total = sdp->blks_total;
	sc.sc_free = sdp->blks_total - sdp->blks_alloced;
	sc.sc_dinodes = sdp->dinodes_alloced;

	gfs2_statfs_change_out(&sc, buf);
	count = gfs2_writei(ip, buf, 0, sizeof(struct gfs2_statfs_change));
	if (count != sizeof(struct gfs2_statfs_change)) {
		fprintf(stderr, "do_init (2)\n");
		exit(1);
	}
}/* write_statfs_file */

/* ------------------------------------------------------------------------- */
/* remove_obsolete_gfs1 - remove obsolete gfs1 inodes.                       */
/* ------------------------------------------------------------------------- */
static void remove_obsolete_gfs1(struct gfs2_sbd *sbp)
{
	struct gfs2_inum inum;

	log_notice(_("Removing obsolete GFS1 file system structures.\n"));
	fflush(stdout);
	/* Delete the old gfs1 Journal index: */
	gfs2_inum_in(&inum, (char *)&raw_gfs1_ondisk_sb.sb_jindex_di);
	gfs2_freedi(sbp, inum.no_addr);

	/* Delete the old gfs1 rgindex: */
	gfs2_inum_in(&inum, (char *)&raw_gfs1_ondisk_sb.sb_rindex_di);
	gfs2_freedi(sbp, inum.no_addr);

	/* Delete the old gfs1 Quota file: */
	gfs2_inum_in(&inum, (char *)&raw_gfs1_ondisk_sb.sb_quota_di);
	gfs2_freedi(sbp, inum.no_addr);

	/* Delete the old gfs1 License file: */
	gfs2_inum_in(&inum, (char *)&raw_gfs1_ondisk_sb.sb_license_di);
	gfs2_freedi(sbp, inum.no_addr);
}

/* ------------------------------------------------------------------------- */
/* lifted from libgfs2/structures.c                                          */
/* ------------------------------------------------------------------------- */
static int conv_build_jindex(struct gfs2_sbd *sdp)
{
	unsigned int j;

	sdp->md.jiinode = createi(sdp->master_dir, "jindex", S_IFDIR | 0700,
				  GFS2_DIF_SYSTEM);
	if (sdp->md.jiinode == NULL) {
		return errno;
	}

	sdp->md.journal = malloc(sdp->md.journals *
				 sizeof(struct gfs2_inode *));
	if (sdp->md.journal == NULL) {
		return errno;
	}
	for (j = 0; j < sdp->md.journals; j++) {
		char name[256];

		printf(_("Writing journal #%d..."), j + 1);
		fflush(stdout);
		sprintf(name, "journal%u", j);
		sdp->md.journal[j] = createi(sdp->md.jiinode, name, S_IFREG |
					     0600, GFS2_DIF_SYSTEM);
		write_journal(sdp->md.journal[j], sdp->bsize,
			      sdp->jsize << 20 >> sdp->sd_sb.sb_bsize_shift);
		inode_put(&sdp->md.journal[j]);
		printf(_("done.\n"));
		fflush(stdout);
	}

	free(sdp->md.journal);
	inode_put(&sdp->md.jiinode);
	return 0;
}

static unsigned int total_file_blocks(struct gfs2_sbd *sdp, 
				      uint64_t filesize, int journaled)
{
	unsigned int data_blks = 0, meta_blks = 0;
	unsigned int max, height, bsize;
	uint64_t *arr;

	/* Now find the total meta blocks required for data_blks */
	if (filesize <= sdp->bsize - sizeof(struct gfs2_dinode)) {
		goto out;
	}

	if (journaled) {
		arr = sdp->sd_jheightsize;
		max = sdp->sd_max_jheight;
		bsize = sdp->sd_jbsize;
	} else {
		arr = sdp->sd_heightsize;
		max = sdp->sd_max_height;
		bsize = sdp->bsize;
	}
	data_blks = DIV_RU(filesize, bsize); /* total data blocks reqd */

	for (height = 0; height < max; height++)
		if (arr[height] >= filesize)
			break;
	if (height == 1) {
		goto out;
	}

	meta_blks = DIV_RU(data_blks, sdp->sd_inptrs);
out:
	return data_blks + meta_blks;
}

/* We check if the GFS2 filesystem files/structures created after the call to 
 * check_fit() in main() will fit in the currently available free blocks
 */
static int check_fit(struct gfs2_sbd *sdp)
{
	unsigned int blks_need = 0, blks_avail = sdp->blks_total - sdp->blks_alloced;

	/* build_master() */
	blks_need++; /*creation of master dir inode - 1 block */

	/* conv_build_jindex() */
	{
		blks_need++; /* creation of 'jindex' disk inode */
		/* creation of journals */
		blks_need += sdp->md.journals *
			total_file_blocks(sdp, sdp->jsize << 20, 1);
	}
	/* build_per_node() */
	{
		blks_need++; /* creation of 'per_node' dir inode */
		/* njourn x (inum_range + statfs_change + quota_change inodes) */
		blks_need += sdp->md.journals * 3;
		/* quota change inodes are prealloced */
		blks_need += sdp->md.journals *
			total_file_blocks(sdp, sdp->qcsize << 20, 1);
	}
	/* build_inum() */
	blks_need++; /* creation of 'inum' disk inode */

	/* build_statfs() */
	blks_need++; /* creation of 'statfs' disk inode */

	/* build_rindex() */
	{
		struct osi_node *n, *next = NULL;
		unsigned int rg_count = 0;

		blks_need++; /* creationg of 'rindex' disk inode */
		/* find the total # of rindex entries, gives size of rindex inode */
		for (n = osi_first(&sdp->rgtree); n; n = next) {
			next = osi_next(n);
			rg_count++;
		}
		blks_need += total_file_blocks(sdp, rg_count *
					       sizeof(struct gfs2_rindex), 1);
	}
	/* build_quota() */
	blks_need++; /* quota inode block and uid=gid=0 quota - total 1 block */

	/* Up until this point we require blks_need blocks. We don't 
	 * include the blocks freed by the next step (remove_obsolete_gfs1)
	 * because it's possible for us to exceed the available blocks
	 * before this step */

	return blks_avail > blks_need;
}

/* We fetch the old quota inode block and copy the contents of the block
 * (minus the struct gfs2_dinode) into the new quota block. We update the 
 * inode height/size of the new quota file to that of the old one and set the 
 * old quota inode height/size to zero, so only the inode block gets freed.
 */
static void copy_quotas(struct gfs2_sbd *sdp)
{
	struct gfs2_inum inum;
	struct gfs2_inode *oq_ip, *nq_ip;
	int err;

	err = gfs2_lookupi(sdp->master_dir, "quota", 5, &nq_ip);
	if (err) {
		fprintf(stderr, _("Couldn't lookup new quota file: %d\n"), err);
		exit(1);
	}

	gfs2_inum_in(&inum, (char *)&raw_gfs1_ondisk_sb.sb_quota_di);
	oq_ip = lgfs2_inode_read(sdp, inum.no_addr);
	if (oq_ip == NULL) {
		fprintf(stderr, _("Couldn't lookup old quota file: %s\n"), strerror(errno));
		exit(1);
	}

	nq_ip->i_di.di_height = oq_ip->i_di.di_height;
	nq_ip->i_di.di_size = oq_ip->i_di.di_size;
	nq_ip->i_di.di_blocks = oq_ip->i_di.di_blocks;
	memcpy(nq_ip->i_bh->b_data + sizeof(struct gfs2_dinode), 
	       oq_ip->i_bh->b_data + sizeof(struct gfs2_dinode),
	       sdp->bsize - sizeof(struct gfs2_dinode));

	oq_ip->i_di.di_height = 0;
	oq_ip->i_di.di_size = 0;

	bmodified(nq_ip->i_bh);
	inode_put(&nq_ip);

	bmodified(oq_ip->i_bh);
	inode_put(&oq_ip);
}

static int gfs2_query(struct gfs2_options *opts, const char *dev)
{
	int res = 0;

	if(opts->yes)
		return 1;
	if(opts->no)
		return 0;

	opts->query = TRUE;
	while (1) {
		char *line = NULL;
		size_t len = 0;
		int ret;

		printf(_("Convert %s from GFS1 to GFS2? (y/n)"), dev);
		fflush(stdout);
		ret = getline(&line, &len, stdin);
		res = rpmatch(line);
		free(line);
		if (ret <= 0)
			continue;
		if (res == 1 || res == 0)
			break;
		/* Unrecognized input; go again. */
	}
	opts->query = FALSE;
	return res;
}

int main(int argc, char **argv)
{
	int error;
	struct gfs2_buffer_head *bh;
	struct gfs2_options opts;

	/* Set i18n support to gfs2_convert */
	setlocale(LC_ALL, "");
	textdomain("gfs2-utils");

	version();
	process_parameters(argc, argv, &opts);
	error = init(&sb2, &opts);

	/*
	 * Check for some common fs errors
	 */
	if (!error) {
		if (sanity_check(&sb2)) {
			log_crit(_("%s is not a clean gfs filesystem. Please use the"
				   " fsck.gfs2 utility to correct these errors and"
				   " try again.\n"), opts.device);
			exit(0);
		}
	}
	/* ---------------------------------------------- */
	/* Make them seal their fate.                     */
	/* ---------------------------------------------- */
	if (!error) {
		give_warning();
		if (!gfs2_query(&opts, opts.device)) {
			log_crit(_("%s not converted.\n"), opts.device);
			close(sb2.device_fd);
			exit(0);
		}
	}
	/* ---------------------------------------------- */
	/* Convert incore gfs1 sb to gfs2 sb              */
	/* ---------------------------------------------- */
	if (!error) {
		log_notice(_("Converting resource groups."));
		fflush(stdout);
		error = convert_rgs(&sb2);
		log_notice("\n");
		if (error)
			log_crit(_("%s: Unable to convert resource groups.\n"), opts.device);
		fsync(sb2.device_fd); /* write the buffers to disk */
	}
	/* ---------------------------------------------- */
	/* Renumber the inodes consecutively.             */
	/* ---------------------------------------------- */
	if (!error) {
		/* Add a string notifying inode converstion start? */
		error = inode_renumber(&sb2, sb2.sd_sb.sb_root_dir.no_addr,
				       (osi_list_t *)&cdpns_to_fix);
		if (error)
			log_crit(_("\n%s: Error renumbering inodes.\n"), opts.device);
		fsync(sb2.device_fd); /* write the buffers to disk */
	}
	/* ---------------------------------------------- */
	/* Fix the directories to match the new numbers.  */
	/* ---------------------------------------------- */
	if (!error) {
		error = fix_directory_info(&sb2, (osi_list_t *)&dirs_to_fix);
		log_notice(_("\r%llu directories, %llu dirents fixed."),
			   (unsigned long long)dirs_fixed,
			   (unsigned long long)dirents_fixed);
		fflush(stdout);
		if (error)
			log_crit(_("\n%s: Error fixing directories.\n"), opts.device);
	}
	/* ---------------------------------------------- */
	/* Convert cdpn symlinks to empty dirs            */
	/* ---------------------------------------------- */
	if (!error) {
		error = fix_cdpn_symlinks(&sb2, (osi_list_t *)&cdpns_to_fix);
		log_notice(_("\r%llu cdpn symlinks moved to empty directories."),
			   (unsigned long long)cdpns_fixed);
		fflush(stdout);
		if (error)
			log_crit(_("\n%s: Error fixing cdpn symlinks.\n"), opts.device);
	}
	/* ---------------------------------------------- */
	/* Convert journal space to rg space              */
	/* ---------------------------------------------- */
	if (!error) {
		log_notice(_("\nConverting journals.\n"));
		error = journ_space_to_rg(&sb2);
		if (error)
			log_crit(_("%s: Error converting journal space.\n"), opts.device);
		fsync(sb2.device_fd); /* write the buffers to disk */
	}
	/* ---------------------------------------------- */
	/* Create our system files and directories.       */
	/* ---------------------------------------------- */
	if (!error) {
		int jreduce = 0;

		/* Now we've got to treat it as a gfs2 file system */
		if (compute_constants(&sb2)) {
			log_crit("%s\n", _("Failed to compute file system constants"));
			exit(-1);
		}

		/* Check if all the files we're about to create will 
		 * fit into the space remaining on the device */
		while (!check_fit(&sb2)) {
			sb2.jsize--; /* reduce jsize by 1MB each time */
			jreduce = 1;
		}
		if (jreduce)
			log_notice(_("Reduced journal size to %u MB to accommodate "
				   "GFS2 file system structures.\n"), sb2.jsize);
		/* Build the master subdirectory. */
		build_master(&sb2); /* Does not do inode_put */
		sb2.sd_sb.sb_master_dir = sb2.master_dir->i_di.di_num;
		/* Build empty journal index file. */
		error = conv_build_jindex(&sb2);
		if (error) {
			log_crit(_("Error: could not build jindex: %s\n"), strerror(error));
			exit(-1);
		}
		log_notice(_("Building GFS2 file system structures.\n"));
		/* Build the per-node directories */
		error = build_per_node(&sb2);
		if (error) {
			log_crit(_("Error building per-node directories: %s\n"),
			         strerror(error));
			exit(-1);
		}
		/* Create the empty inode number file */
		error = build_inum(&sb2); /* Does not do inode_put */
		if (error) {
			log_crit(_("Error building inum inode: %s\n"),
			         strerror(error));
			exit(-1);
		}
		gfs2_lookupi(sb2.master_dir, "inum", 4, &sb2.md.inum);
		/* Create the statfs file */
		error = build_statfs(&sb2); /* Does not do inode_put */
		if (error) {
			log_crit(_("Error building statfs inode: %s\n"),
			         strerror(error));
			exit(-1);
		}
		gfs2_lookupi(sb2.master_dir, "statfs", 6, &sb2.md.statfs);
		do_init_statfs(&sb2);

		/* Create the resource group index file */
		error = build_rindex(&sb2);
		if (error) {
			log_crit(_("Error building rindex inode: %s\n"),
			         strerror(error));
			exit(-1);
		}
		/* Create the quota file */
		error = build_quota(&sb2);
		if (error) {
			log_crit(_("Error building quota inode: %s\n"),
			         strerror(error));
			exit(-1);
		}

		/* Copy out the master dinode */
		{
			struct gfs2_inode *ip = sb2.master_dir;
			if (ip->i_bh->b_modified)
				gfs2_dinode_out(&ip->i_di, ip->i_bh->b_data);
		}
		/* Copy old quotas */
		copy_quotas(&sb2);

		update_inode_file(&sb2);
		/* Now delete the now-obsolete gfs1 files: */
		remove_obsolete_gfs1(&sb2);

		write_statfs_file(&sb2);

		inode_put(&sb2.master_dir);
		inode_put(&sb2.md.inum);
		inode_put(&sb2.md.statfs);

		fsync(sb2.device_fd); /* write the buffers to disk */

		/* Now free all the in memory */
		gfs2_rgrp_free(&sb2.rgtree);
		log_notice(_("Committing changes to disk.\n"));
		fflush(stdout);
		/* Set filesystem type in superblock to gfs2.  We do this at the */
		/* end because if the tool is interrupted in the middle, we want */
		/* it to not reject the partially converted fs as already done   */
		/* when it's run a second time.                                  */
		bh = bread(&sb2, LGFS2_SB_ADDR(&sb2));
		sb2.sd_sb.sb_fs_format = GFS2_FORMAT_FS;
		sb2.sd_sb.sb_multihost_format = GFS2_FORMAT_MULTI;
		gfs2_sb_out(&sb2.sd_sb, bh->b_data);
		bmodified(bh);
		brelse(bh);

		error = fsync(sb2.device_fd);
		if (error)
			perror(opts.device);
		else
			log_notice(_("%s: filesystem converted successfully to gfs2.\n"), opts.device);
	}
	close(sb2.device_fd);
	if (sd_jindex)
		free(sd_jindex);
	exit(0);
}
