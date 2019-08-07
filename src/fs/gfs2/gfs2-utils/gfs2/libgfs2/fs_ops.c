#include "clusterautoconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <linux/types.h>
#include "libgfs2.h"
#include "rgrp.h"

static __inline__ uint64_t *metapointer(struct gfs2_buffer_head *bh,
					unsigned int height,
					struct metapath *mp)
{
	unsigned int head_size = (height > 0) ?
		sizeof(struct gfs2_meta_header) : sizeof(struct gfs2_dinode);

	return ((uint64_t *)(bh->b_data + head_size)) + mp->mp_list[height];
}

/* Detect directory is a stuffed inode */
static int inode_is_stuffed(const struct gfs2_inode *ip)
{
	return !ip->i_di.di_height;
}

struct gfs2_inode *lgfs2_inode_get(struct gfs2_sbd *sdp, struct gfs2_buffer_head *bh)
{
	struct gfs2_inode *ip;

	ip = calloc(1, sizeof(struct gfs2_inode));
	if (ip == NULL) {
		return NULL;
	}
	gfs2_dinode_in(&ip->i_di, bh->b_data);
	ip->i_bh = bh;
	ip->i_sbd = sdp;
	return ip;
}

struct gfs2_inode *lgfs2_inode_read(struct gfs2_sbd *sdp, uint64_t di_addr)
{
	struct gfs2_inode *ip;
	struct gfs2_buffer_head *bh = bread(sdp, di_addr);
	if (bh == NULL) {
		return NULL;
	}
	ip = lgfs2_inode_get(sdp, bh);
	if (ip == NULL) {
		brelse(bh);
		return NULL;
	}
	ip->bh_owned = 1; /* We did the bread so we own the bh */
	return ip;
}

struct gfs2_inode *is_system_inode(struct gfs2_sbd *sdp, uint64_t block)
{
	int j;

	if (sdp->md.inum && block == sdp->md.inum->i_di.di_num.no_addr)
		return sdp->md.inum;
	if (sdp->md.statfs && block == sdp->md.statfs->i_di.di_num.no_addr)
		return sdp->md.statfs;
	if (sdp->md.jiinode && block == sdp->md.jiinode->i_di.di_num.no_addr)
		return sdp->md.jiinode;
	if (sdp->md.riinode && block == sdp->md.riinode->i_di.di_num.no_addr)
		return sdp->md.riinode;
	if (sdp->md.qinode && block == sdp->md.qinode->i_di.di_num.no_addr)
		return sdp->md.qinode;
	if (sdp->md.pinode && block == sdp->md.pinode->i_di.di_num.no_addr)
		return sdp->md.pinode;
	if (sdp->md.rooti && block == sdp->md.rooti->i_di.di_num.no_addr)
		return sdp->md.rooti;
	if (sdp->master_dir && block == sdp->master_dir->i_di.di_num.no_addr)
		return sdp->master_dir;
	for (j = 0; j < sdp->md.journals; j++)
		if (sdp->md.journal && sdp->md.journal[j] &&
		    block == sdp->md.journal[j]->i_di.di_num.no_addr)
			return sdp->md.journal[j];
	return NULL;
}

void inode_put(struct gfs2_inode **ip_in)
{
	struct gfs2_inode *ip = *ip_in;
	uint64_t block = ip->i_di.di_num.no_addr;
	struct gfs2_sbd *sdp = ip->i_sbd;

	if (ip->i_bh->b_modified) {
		gfs2_dinode_out(&ip->i_di, ip->i_bh->b_data);
		if (!ip->bh_owned && is_system_inode(sdp, block))
			fprintf(stderr, "Warning: Change made to inode "
				"were discarded.\n");
		/* This is for debugging only: a convenient place to set
		   a breakpoint. This means a system inode was modified but
		   not written.  That's not fatal: some places like
		   adjust_inode in gfs2_convert will do this on purpose.
		   It can also point out a coding problem, but we don't
		   want to raise alarm in the users either. */
	}
	if (ip->bh_owned)
		brelse(ip->i_bh);
	ip->i_bh = NULL;
	free(ip);
	*ip_in = NULL; /* make sure the memory isn't accessed again */
}

static uint64_t find_free_block(struct rgrp_tree *rgd)
{
	unsigned bm;
	uint64_t blkno = 0;

	if (rgd == NULL || rgd->rg.rg_free == 0) {
		errno = ENOSPC;
		return 0;
	}

	for (bm = 0; bm < rgd->ri.ri_length; bm++) {
		unsigned long blk = 0;
		struct gfs2_bitmap *bits = &rgd->bits[bm];

		blk = gfs2_bitfit((uint8_t *)bits->bi_bh->b_data + bits->bi_offset,
		                  bits->bi_len, blk, GFS2_BLKST_FREE);
		if (blk != BFITNOENT) {
			blkno = blk + (bits->bi_start * GFS2_NBBY) + rgd->ri.ri_data0;
			break;
		}
	}
	return blkno;
}

static int blk_alloc_in_rg(struct gfs2_sbd *sdp, unsigned state, struct rgrp_tree *rgd, uint64_t blkno, int dinode)
{
	if (blkno == 0)
		return -1;

	if (gfs2_set_bitmap(rgd, blkno, state))
		return -1;

	if (state == GFS2_BLKST_DINODE) {
		struct gfs_rgrp *gfs1rg = (struct gfs_rgrp *)&rgd->rg;

		if (dinode)
			rgd->rg.rg_dinodes++;
		else if (sdp->gfs1)
			gfs1rg->rg_usedmeta++;
	}

	rgd->rg.rg_free--;
	if (sdp->gfs1)
		gfs_rgrp_out((struct gfs_rgrp *)&rgd->rg, rgd->bits[0].bi_bh);
	else
		gfs2_rgrp_out(&rgd->rg, rgd->bits[0].bi_bh->b_data);
	bmodified(rgd->bits[0].bi_bh);
	sdp->blks_alloced++;
	return 0;
}

/**
 * Allocate a block in a bitmap. In order to plan ahead we look for a
 * resource group with blksreq free blocks but only allocate the one block.
 * Returns 0 on success with the allocated block number in *blkno or non-zero otherwise.
 */
static int block_alloc(struct gfs2_sbd *sdp, const uint64_t blksreq, int state, uint64_t *blkno, int dinode)
{
	int ret;
	int release = 0;
	struct rgrp_tree *rgt = NULL;
	struct osi_node *n = NULL;
	uint64_t bn = 0;

	for (n = osi_first(&sdp->rgtree); n; n = osi_next(n)) {
		rgt = (struct rgrp_tree *)n;
		if (rgt->rg.rg_free >= blksreq)
			break;
	}
	if (rgt == NULL)
		return -1;

	if (rgt->bits[0].bi_bh == NULL) {
		if (gfs2_rgrp_read(sdp, rgt))
			return -1;
		release = 1;
	}

	bn = find_free_block(rgt);
	ret = blk_alloc_in_rg(sdp, state, rgt, bn, dinode);
	if (release)
		gfs2_rgrp_relse(rgt);
	*blkno = bn;
	return ret;
}

int lgfs2_dinode_alloc(struct gfs2_sbd *sdp, const uint64_t blksreq, uint64_t *blkno)
{
	int ret = block_alloc(sdp, blksreq, GFS2_BLKST_DINODE, blkno, TRUE);
	if (ret == 0)
		sdp->dinodes_alloced++;
	return ret;
}

int lgfs2_meta_alloc(struct gfs2_inode *ip, uint64_t *blkno)
{
	int ret = block_alloc(ip->i_sbd, 1,
			      ip->i_sbd->gfs1 ? GFS2_BLKST_DINODE :
			      GFS2_BLKST_USED, blkno, FALSE);
	if (ret == 0) {
		ip->i_di.di_goal_meta = *blkno;
		bmodified(ip->i_bh);
	}
	return ret;
}

static __inline__ void buffer_clear_tail(struct gfs2_sbd *sdp,
					 struct gfs2_buffer_head *bh, int head)
{
	memset(bh->b_data + head, 0, sdp->bsize - head);
	bmodified(bh);
}

static __inline__ void
buffer_copy_tail(struct gfs2_sbd *sdp,
		 struct gfs2_buffer_head *to_bh, int to_head,
		 struct gfs2_buffer_head *from_bh, int from_head)
{
	memcpy(to_bh->b_data + to_head, from_bh->b_data + from_head,
	       sdp->bsize - from_head);
	memset(to_bh->b_data + sdp->bsize + to_head - from_head, 0,
	       from_head - to_head);
	bmodified(to_bh);
}

void unstuff_dinode(struct gfs2_inode *ip)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	struct gfs2_buffer_head *bh;
	uint64_t block = 0;
	int isdir = S_ISDIR(ip->i_di.di_mode) || is_gfs_dir(&ip->i_di);

	if (ip->i_di.di_size) {
		if (lgfs2_meta_alloc(ip, &block))
			exit(1);
		if (isdir) {
			struct gfs2_meta_header mh;

			bh = bget(sdp, block);
			mh.mh_magic = GFS2_MAGIC;
			mh.mh_type = GFS2_METATYPE_JD;
			mh.mh_format = GFS2_FORMAT_JD;
			gfs2_meta_header_out(&mh, bh->b_data);

			buffer_copy_tail(sdp, bh,
					 sizeof(struct gfs2_meta_header),
					 ip->i_bh, sizeof(struct gfs2_dinode));

			bmodified(bh);
			brelse(bh);
		} else {
			bh = bget(sdp, block);

			buffer_copy_tail(sdp, bh, 0,
					 ip->i_bh, sizeof(struct gfs2_dinode));
			brelse(bh);
		}
	}

	buffer_clear_tail(sdp, ip->i_bh, sizeof(struct gfs2_dinode));

	if (ip->i_di.di_size) {
		*(uint64_t *)(ip->i_bh->b_data + sizeof(struct gfs2_dinode)) = cpu_to_be64(block);
		/* no need: bmodified(ip->i_bh); buffer_clear_tail does it */
		ip->i_di.di_blocks++;
	}

	ip->i_di.di_height = 1;
}

/**
 * Calculate the total number of blocks required by a file containing 'bytes' bytes of data.
 */
uint64_t lgfs2_space_for_data(const struct gfs2_sbd *sdp, const unsigned bsize, const uint64_t bytes)
{
	uint64_t blks = (bytes + bsize - 1) / bsize;
	uint64_t ptrs = blks;

	if (bytes <= bsize - sizeof(struct gfs2_dinode))
		return 1;

	while (ptrs > sdp->sd_diptrs) {
		ptrs = (ptrs + sdp->sd_inptrs - 1) / sdp->sd_inptrs;
		blks += ptrs;
	}
	return blks + 1;
}

/**
 * Allocate an extent for a file in a resource group's bitmaps.
 * rg: The resource group in which to allocate the extent
 * di_size: The size of the file in bytes
 * ip: A pointer to the inode structure, whose fields will be set appropriately.
 *     If ip->i_di.di_num.no_addr is not 0, the extent search will be skipped and
 *     the file allocated from that address.
 * flags: GFS2_DIF_* flags
 * mode: File mode flags, see creat(2)
 * Returns 0 on success with the contents of ip set accordingly, or non-zero
 * with errno set on error. If errno is ENOSPC then rg does not contain a
 * large enough free extent for the given di_size.
 */
int lgfs2_file_alloc(lgfs2_rgrp_t rg, uint64_t di_size, struct gfs2_inode *ip, uint32_t flags, unsigned mode)
{
	unsigned extlen;
	struct gfs2_dinode *di = &ip->i_di;
	struct gfs2_sbd *sdp = rg->rgrps->sdp;
	struct lgfs2_rbm rbm = { .rgd = rg, .offset = 0, .bii = 0 };
	uint32_t blocks = lgfs2_space_for_data(sdp, sdp->bsize, di_size);

	if (ip->i_di.di_num.no_addr != 0) {
		if (lgfs2_rbm_from_block(&rbm, ip->i_di.di_num.no_addr) != 0)
			return 1;
	} else if (lgfs2_rbm_find(&rbm, GFS2_BLKST_FREE, &blocks) != 0) {
		return 1;
	}

	extlen = lgfs2_alloc_extent(&rbm, GFS2_BLKST_DINODE, blocks);
	if (extlen < blocks) {
		errno = EINVAL;
		return 1;
	}

	ip->i_sbd = sdp;

	di->di_header.mh_magic = GFS2_MAGIC;
	di->di_header.mh_type = GFS2_METATYPE_DI;
	di->di_header.mh_format = GFS2_FORMAT_DI;
	di->di_size = di_size;
	di->di_num.no_addr = lgfs2_rbm_to_block(&rbm);
	di->di_num.no_formal_ino = sdp->md.next_inum++;
	di->di_mode = mode;
	di->di_nlink = 1;
	di->di_blocks = blocks;
	di->di_atime = di->di_mtime = di->di_ctime = sdp->time;
	di->di_goal_data = di->di_num.no_addr + di->di_blocks - 1;
	di->di_goal_meta = di->di_goal_data - ((di_size + sdp->bsize - 1) / sdp->bsize);
	di->di_height = calc_tree_height(ip, di_size);
	di->di_flags = flags;

	rg->rg.rg_free -= blocks;
	rg->rg.rg_dinodes += 1;

	sdp->dinodes_alloced++;
	sdp->blks_alloced += blocks;

	return 0;
}

unsigned int calc_tree_height(struct gfs2_inode *ip, uint64_t size)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	uint64_t *arr;
	unsigned int max, height;

	if (ip->i_di.di_size > size)
		size = ip->i_di.di_size;

	if (S_ISDIR(ip->i_di.di_mode)) {
		arr = sdp->sd_jheightsize;
		max = sdp->sd_max_jheight;
	} else {
		arr = sdp->sd_heightsize;
		max = sdp->sd_max_height;
	}

	for (height = 0; height < max; height++)
		if (arr[height] >= size)
			break;

	return height;
}

void build_height(struct gfs2_inode *ip, int height)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	struct gfs2_buffer_head *bh;
	uint64_t block = 0, *bp;
	unsigned int x;
	int new_block;

	while (ip->i_di.di_height < height) {
		new_block = FALSE;
		bp = (uint64_t *)(ip->i_bh->b_data + sizeof(struct gfs2_dinode));
		for (x = 0; x < sdp->sd_diptrs; x++, bp++)
			if (*bp) {
				new_block = TRUE;
				break;
			}

		if (new_block) {
			struct gfs2_meta_header mh;

			if (lgfs2_meta_alloc(ip, &block))
				exit(1);
			bh = bget(sdp, block);
			mh.mh_magic = GFS2_MAGIC;
			mh.mh_type = GFS2_METATYPE_IN;
			mh.mh_format = GFS2_FORMAT_IN;
			gfs2_meta_header_out(&mh, bh->b_data);
			buffer_copy_tail(sdp, bh,
					 sizeof(struct gfs2_meta_header),
					 ip->i_bh, sizeof(struct gfs2_dinode));
			bmodified(bh);
			brelse(bh);
		}

		buffer_clear_tail(sdp, ip->i_bh, sizeof(struct gfs2_dinode));

		if (new_block) {
			*(uint64_t *)(ip->i_bh->b_data + sizeof(struct gfs2_dinode)) = cpu_to_be64(block);
			/* no need: bmodified(ip->i_bh);*/
			ip->i_di.di_blocks++;
		}

		ip->i_di.di_height++;
	}
}

void find_metapath(struct gfs2_inode *ip, uint64_t block, struct metapath *mp)
{
	const uint32_t inptrs = ip->i_sbd->sd_inptrs;
	unsigned int i = ip->i_di.di_height;

	memset(mp, 0, sizeof(struct metapath));
	while (i--) {
		mp->mp_list[i] = block % inptrs;
		block /= inptrs;
	}
}

void lookup_block(struct gfs2_inode *ip, struct gfs2_buffer_head *bh,
		  unsigned int height, struct metapath *mp,
		  int create, int *new, uint64_t *block)
{
	uint64_t *ptr = metapointer(bh, height, mp);

	if (*ptr) {
		*block = be64_to_cpu(*ptr);
		return;
	}

	*block = 0;

	if (!create)
		return;

	if (lgfs2_meta_alloc(ip, block))
		return;
	*ptr = cpu_to_be64(*block);
	bmodified(bh);
	ip->i_di.di_blocks++;
	bmodified(ip->i_bh);

	*new = 1;
}

void block_map(struct gfs2_inode *ip, uint64_t lblock, int *new,
	       uint64_t *dblock, uint32_t *extlen, int prealloc)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	struct gfs2_buffer_head *bh;
	struct metapath mp;
	int create = *new;
	unsigned int bsize;
	unsigned int height;
	unsigned int end_of_metadata;
	unsigned int x;

	*new = 0;
	*dblock = 0;
	if (extlen)
		*extlen = 0;

	if (inode_is_stuffed(ip)) {
		if (!lblock) {
			*dblock = ip->i_di.di_num.no_addr;
			if (extlen)
				*extlen = 1;
		}
		return;
	}

	bsize = (S_ISDIR(ip->i_di.di_mode)) ? sdp->sd_jbsize : sdp->bsize;

	height = calc_tree_height(ip, (lblock + 1) * bsize);
	if (ip->i_di.di_height < height) {
		if (!create)
			return;

		build_height(ip, height);
	}

	find_metapath(ip, lblock, &mp);
	end_of_metadata = ip->i_di.di_height - 1;

	bh = ip->i_bh;

	for (x = 0; x < end_of_metadata; x++) {
		lookup_block(ip, bh, x, &mp, create, new, dblock);
		if (bh != ip->i_bh)
			brelse(bh);
		if (!*dblock)
			return;

		if (*new) {
			struct gfs2_meta_header mh;
			bh = bget(sdp, *dblock);
			mh.mh_magic = GFS2_MAGIC;
			mh.mh_type = GFS2_METATYPE_IN;
			mh.mh_format = GFS2_FORMAT_IN;
			gfs2_meta_header_out(&mh, bh->b_data);
			bmodified(bh);
		} else {
			if (*dblock == ip->i_di.di_num.no_addr)
				bh = ip->i_bh;
			else
				bh = bread(sdp, *dblock);
		}
	}

	if (!prealloc)
		lookup_block(ip, bh, end_of_metadata, &mp, create, new, dblock);

	if (extlen && *dblock) {
		*extlen = 1;

		if (!*new) {
			uint64_t tmp_dblock;
			int tmp_new;
			unsigned int nptrs;

			nptrs = (end_of_metadata) ? sdp->sd_inptrs : sdp->sd_diptrs;

			while (++mp.mp_list[end_of_metadata] < nptrs) {
				lookup_block(ip, bh, end_of_metadata, &mp, FALSE, &tmp_new,
							 &tmp_dblock);

				if (*dblock + *extlen != tmp_dblock)
					break;

				(*extlen)++;
			}
		}
	}

	if (bh != ip->i_bh)
		brelse(bh);
}

static void
copy2mem(struct gfs2_buffer_head *bh, void **buf, unsigned int offset,
	 unsigned int size)
{
	char **p = (char **)buf;

	if (bh)
		memcpy(*p, bh->b_data + offset, size);
	else
		memset(*p, 0, size);

	*p += size;
}

int gfs2_readi(struct gfs2_inode *ip, void *buf,
			   uint64_t offset, unsigned int size)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	struct gfs2_buffer_head *bh;
	uint64_t lblock, dblock;
	unsigned int o;
	uint32_t extlen = 0;
	unsigned int amount;
	int not_new = 0;
	int isdir = !!(S_ISDIR(ip->i_di.di_mode));
	int journaled = ip->i_di.di_flags & GFS2_DIF_JDATA;
	int copied = 0;

	if (offset >= ip->i_di.di_size)
		return 0;

	if ((offset + size) > ip->i_di.di_size)
		size = ip->i_di.di_size - offset;

	if (!size)
		return 0;

	if ((sdp->gfs1 && journaled) || (!sdp->gfs1 && isdir)) {
		lblock = offset;
		o = lblock % sdp->sd_jbsize;
		lblock /= sdp->sd_jbsize;
	} else {
		lblock = offset >> sdp->sd_sb.sb_bsize_shift;
		o = offset & (sdp->bsize - 1);
	}

	if (inode_is_stuffed(ip))
		o += sizeof(struct gfs2_dinode);
	else if ((sdp->gfs1 && journaled) || (!sdp->gfs1 && isdir))
		o += sizeof(struct gfs2_meta_header);

	while (copied < size) {
		amount = size - copied;
		if (amount > sdp->bsize - o)
			amount = sdp->bsize - o;

		if (!extlen) {
			if (sdp->gfs1)
				gfs1_block_map(ip, lblock, &not_new, &dblock,
					       &extlen, FALSE);
			else
				block_map(ip, lblock, &not_new, &dblock,
					  &extlen, FALSE);
		}

		if (dblock) {
			if (dblock == ip->i_di.di_num.no_addr)
				bh = ip->i_bh;
			else
				bh = bread(sdp, dblock);
			dblock++;
			extlen--;
		} else
			bh = NULL;

		copy2mem(bh, &buf, o, amount);
		if (bh && bh != ip->i_bh)
			brelse(bh);

		copied += amount;
		lblock++;

		if (sdp->gfs1)
			o = (journaled) ? sizeof(struct gfs2_meta_header) : 0;
		else
			o = (isdir) ? sizeof(struct gfs2_meta_header) : 0;
	}

	return copied;
}

static void copy_from_mem(struct gfs2_buffer_head *bh, void **buf,
			  unsigned int offset, unsigned int size)
{
	char **p = (char **)buf;

	memcpy(bh->b_data + offset, *p, size);
	bmodified(bh);
	*p += size;
}

int __gfs2_writei(struct gfs2_inode *ip, void *buf,
		  uint64_t offset, unsigned int size, int resize)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	struct gfs2_buffer_head *bh;
	uint64_t lblock, dblock;
	unsigned int o;
	uint32_t extlen = 0;
	unsigned int amount;
	int new;
	int isdir = !!(S_ISDIR(ip->i_di.di_mode));
	const uint64_t start = offset;
	int copied = 0;

	if (!size)
		return 0;

	if (inode_is_stuffed(ip) &&
	    ((start + size) > (sdp->bsize - sizeof(struct gfs2_dinode))))
		unstuff_dinode(ip);

	if (isdir) {
		lblock = offset;
		o = lblock % sdp->sd_jbsize;
		lblock /= sdp->sd_jbsize;
	} else {
		lblock = offset >> sdp->sd_sb.sb_bsize_shift;
		o = offset & (sdp->bsize - 1);
	}

	if (inode_is_stuffed(ip))
		o += sizeof(struct gfs2_dinode);
	else if (isdir)
		o += sizeof(struct gfs2_meta_header);

	while (copied < size) {
		amount = size - copied;
		if (amount > sdp->bsize - o)
			amount = sdp->bsize - o;

		if (!extlen) {
			new = TRUE;
			block_map(ip, lblock, &new, &dblock, &extlen, FALSE);
		}

		if (new) {
			bh = bget(sdp, dblock);
			if (isdir) {
				struct gfs2_meta_header mh;
				mh.mh_magic = GFS2_MAGIC;
				mh.mh_type = GFS2_METATYPE_JD;
				mh.mh_format = GFS2_FORMAT_JD;
				gfs2_meta_header_out(&mh, bh->b_data);
				bmodified(bh);
			}
		} else {
			if (dblock == ip->i_di.di_num.no_addr)
				bh = ip->i_bh;
			else
				bh = bread(sdp, dblock);
		}
		copy_from_mem(bh, &buf, o, amount);
		if (bh != ip->i_bh)
			brelse(bh);

		copied += amount;
		lblock++;
		dblock++;
		extlen--;

		o = (isdir) ? sizeof(struct gfs2_meta_header) : 0;
	}

	if (resize && ip->i_di.di_size < start + copied) {
		bmodified(ip->i_bh);
		ip->i_di.di_size = start + copied;
	}

	return copied;
}

struct gfs2_buffer_head *get_file_buf(struct gfs2_inode *ip, uint64_t lbn,
				      int prealloc)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	uint64_t dbn;
	int new = TRUE;

	if (inode_is_stuffed(ip))
		unstuff_dinode(ip);

	block_map(ip, lbn, &new, &dbn, NULL, prealloc);
	if (!dbn) {
		fprintf(stderr, "get_file_buf\n");
		exit(1);
	}

	if (!prealloc && new &&
	    ip->i_di.di_size < (lbn + 1) << sdp->sd_sb.sb_bsize_shift) {
		bmodified(ip->i_bh);
		ip->i_di.di_size = (lbn + 1) << sdp->sd_sb.sb_bsize_shift;
	}
	if (dbn == ip->i_di.di_num.no_addr)
		return ip->i_bh;
	else
		return bread(sdp, dbn);
}

int gfs2_dirent_first(struct gfs2_inode *dip, struct gfs2_buffer_head *bh,
					  struct gfs2_dirent **dent)
{
	struct gfs2_meta_header *h = (struct gfs2_meta_header *)bh->b_data;

	if (be32_to_cpu(h->mh_type) == GFS2_METATYPE_LF) {
		*dent = (struct gfs2_dirent *)(bh->b_data + sizeof(struct gfs2_leaf));
		return IS_LEAF;
	} else {
		*dent = (struct gfs2_dirent *)(bh->b_data + sizeof(struct gfs2_dinode));
		return IS_DINODE;
	}
}

int gfs2_dirent_next(struct gfs2_inode *dip, struct gfs2_buffer_head *bh,
					 struct gfs2_dirent **dent)
{
	char *bh_end;
	uint16_t cur_rec_len;

	bh_end = bh->b_data + dip->i_sbd->bsize;
	cur_rec_len = be16_to_cpu((*dent)->de_rec_len);

	if (cur_rec_len == 0 || (char *)(*dent) + cur_rec_len >= bh_end)
		return -ENOENT;

	*dent = (struct gfs2_dirent *)((char *)(*dent) + cur_rec_len);

	return 0;
}

/**
 * Allocate a gfs2 dirent
 * Returns 0 on success, with *dent_out pointing to the new dirent,
 * or -1 on failure, with errno set
 */
static int dirent_alloc(struct gfs2_inode *dip, struct gfs2_buffer_head *bh,
			int name_len, struct gfs2_dirent **dent_out)
{
	struct gfs2_dirent *dent, *new;
	unsigned int rec_len = GFS2_DIRENT_SIZE(name_len);
	unsigned int entries = 0, offset = 0;
	int type;

	type = gfs2_dirent_first(dip, bh, &dent);

	if (type == IS_LEAF) {
		struct gfs2_leaf *leaf = (struct gfs2_leaf *)bh->b_data;
		entries = be16_to_cpu(leaf->lf_entries);
		offset = sizeof(struct gfs2_leaf);
	} else {
		struct gfs2_dinode *dinode = (struct gfs2_dinode *)bh->b_data;
		entries = be32_to_cpu(dinode->di_entries);
		offset = sizeof(struct gfs2_dinode);
	}

	if (!entries) {
		dent->de_rec_len = cpu_to_be16(dip->i_sbd->bsize - offset);
		dent->de_name_len = cpu_to_be16(name_len);
		bmodified(bh);
		*dent_out = dent;
		dip->i_di.di_entries++;
		bmodified(dip->i_bh);
		return 0;
	}

	do {
		uint16_t cur_rec_len;
		uint16_t cur_name_len;
		uint16_t new_rec_len;

		cur_rec_len = be16_to_cpu(dent->de_rec_len);
		cur_name_len = be16_to_cpu(dent->de_name_len);

		if ((!dent->de_inum.no_formal_ino && cur_rec_len >= rec_len) ||
		    (cur_rec_len >= GFS2_DIRENT_SIZE(cur_name_len) + rec_len)) {

			if (dent->de_inum.no_formal_ino) {
				new = (struct gfs2_dirent *)((char *)dent +
							    GFS2_DIRENT_SIZE(cur_name_len));
				memset(new, 0, sizeof(struct gfs2_dirent));

				new->de_rec_len = cpu_to_be16(cur_rec_len -
					  GFS2_DIRENT_SIZE(cur_name_len));
				new->de_name_len = cpu_to_be16(name_len);

				new_rec_len = be16_to_cpu(new->de_rec_len);
				dent->de_rec_len = cpu_to_be16(cur_rec_len - new_rec_len);

				*dent_out = new;
				bmodified(bh);
				dip->i_di.di_entries++;
				bmodified(dip->i_bh);
				return 0;
			}

			dent->de_name_len = cpu_to_be16(name_len);

			*dent_out = dent;
			bmodified(bh);
			dip->i_di.di_entries++;
			bmodified(dip->i_bh);
			return 0;
		}
	} while (gfs2_dirent_next(dip, bh, &dent) == 0);

	errno = ENOSPC;
	return -1;
}

void dirent2_del(struct gfs2_inode *dip, struct gfs2_buffer_head *bh,
		 struct gfs2_dirent *prev, struct gfs2_dirent *cur)
{
	uint16_t cur_rec_len, prev_rec_len;

	bmodified(bh);
	if (gfs2_check_meta(bh, GFS2_METATYPE_LF) == 0) {
		struct gfs2_leaf *lf = (struct gfs2_leaf *)bh->b_data;

		lf->lf_entries = be16_to_cpu(lf->lf_entries) - 1;
		lf->lf_entries = cpu_to_be16(lf->lf_entries);
	}

	if (dip->i_di.di_entries) {
		bmodified(dip->i_bh);
		dip->i_di.di_entries--;
	}
	if (!prev) {
		cur->de_inum.no_addr = 0;
		cur->de_inum.no_formal_ino = 0;
		return;
	}

	prev_rec_len = be16_to_cpu(prev->de_rec_len);
	cur_rec_len = be16_to_cpu(cur->de_rec_len);

	prev_rec_len += cur_rec_len;
	prev->de_rec_len = cpu_to_be16(prev_rec_len);
}

int lgfs2_get_leaf_ptr(struct gfs2_inode *dip, const uint32_t lindex, uint64_t *ptr)
{
	uint64_t leaf_no;
	int count = gfs2_readi(dip, (char *)&leaf_no, lindex * sizeof(uint64_t), sizeof(uint64_t));
	if (count != sizeof(uint64_t))
		return -1;

	*ptr = be64_to_cpu(leaf_no);
	return 0;
}

void dir_split_leaf(struct gfs2_inode *dip, uint32_t start, uint64_t leaf_no,
		    struct gfs2_buffer_head *obh)
{
	struct gfs2_buffer_head *nbh;
	struct gfs2_leaf *nleaf, *oleaf;
	struct gfs2_dirent *dent, *prev = NULL, *next = NULL, *new;
	uint32_t len, half_len, divider;
	uint64_t bn, *lp;
	uint32_t name_len;
	int x, moved = FALSE;
	int count;

	if (lgfs2_meta_alloc(dip, &bn))
		exit(1);
	nbh = bget(dip->i_sbd, bn);
	{
		struct gfs2_meta_header mh;
		mh.mh_magic = GFS2_MAGIC;
		mh.mh_type = GFS2_METATYPE_LF;
		mh.mh_format = GFS2_FORMAT_LF;
		gfs2_meta_header_out(&mh, nbh->b_data);
		bmodified(nbh);
		buffer_clear_tail(dip->i_sbd, nbh,
				  sizeof(struct gfs2_meta_header));
	}

	nleaf = (struct gfs2_leaf *)nbh->b_data;
	nleaf->lf_dirent_format = cpu_to_be32(GFS2_FORMAT_DE);

	oleaf = (struct gfs2_leaf *)obh->b_data;

	len = 1 << (dip->i_di.di_depth - be16_to_cpu(oleaf->lf_depth));
	half_len = len >> 1;

	lp = calloc(1, half_len * sizeof(uint64_t));
	if (lp == NULL) {
		fprintf(stderr, "Out of memory in %s\n", __FUNCTION__);
		exit(-1);
	}
	for (x = 0; x < half_len; x++)
		lp[x] = cpu_to_be64(bn);

	if (dip->i_sbd->gfs1)
		count = gfs1_writei(dip, (char *)lp, start * sizeof(uint64_t),
				    half_len * sizeof(uint64_t));
	else
		count = gfs2_writei(dip, (char *)lp, start * sizeof(uint64_t),
				    half_len * sizeof(uint64_t));
	if (count != half_len * sizeof(uint64_t)) {
		fprintf(stderr, "dir_split_leaf (2)\n");
		exit(1);
	}

	free(lp);

	divider = (start + half_len) << (32 - dip->i_di.di_depth);

	gfs2_dirent_first(dip, obh, &dent);

	do {
		next = dent;
		if (gfs2_dirent_next(dip, obh, &next))
			next = NULL;

		if (dent->de_inum.no_formal_ino &&
		    be32_to_cpu(dent->de_hash) < divider) {
			name_len = be16_to_cpu(dent->de_name_len);

			if (dirent_alloc(dip, nbh, name_len, &new)) {
				fprintf(stderr, "dir_split_leaf (3)\n");
				exit(1);
			}

			new->de_inum = dent->de_inum;
			new->de_hash = dent->de_hash;
			new->de_type = dent->de_type;
			memcpy((char *)(new + 1), (char *)(dent + 1), name_len);

			nleaf->lf_entries = be16_to_cpu(nleaf->lf_entries) + 1;
			nleaf->lf_entries = cpu_to_be16(nleaf->lf_entries);

			dirent2_del(dip, obh, prev, dent);

			if (!prev)
				prev = dent;

			moved = TRUE;
		} else
			prev = dent;

		dent = next;
	} while (dent);

	if (!moved) {
		if (dirent_alloc(dip, nbh, 0, &new)) {
			fprintf(stderr, "dir_split_leaf (4)\n");
			exit(1);
		}
		new->de_inum.no_formal_ino = 0;
		/* Don't count the sentinel dirent as an entry */
		dip->i_di.di_entries--;
	}

	oleaf->lf_depth = be16_to_cpu(oleaf->lf_depth) + 1;
	oleaf->lf_depth = cpu_to_be16(oleaf->lf_depth);
	nleaf->lf_depth = oleaf->lf_depth;

#ifdef GFS2_HAS_LEAF_HINTS
	nleaf->lf_inode = cpu_to_be64(dip->i_di.di_num.no_addr);
#endif
	dip->i_di.di_blocks++;
	bmodified(dip->i_bh);

	bmodified(obh); /* Need to do this in case nothing was moved */
	bmodified(nbh);
	brelse(nbh);
}

static void dir_double_exhash(struct gfs2_inode *dip)
{
	struct gfs2_sbd *sdp = dip->i_sbd;
	uint64_t *buf;
	uint64_t *from, *to;
	uint64_t block;
	int x;
	int count;

	buf = calloc(1, 3 * sdp->sd_hash_bsize);
	if (buf == NULL) {
		fprintf(stderr, "Out of memory in %s\n", __FUNCTION__);
		exit(-1);
	}

	for (block = dip->i_di.di_size >> sdp->sd_hash_bsize_shift; block--;) {
		count = gfs2_readi(dip, (char *)buf,
			      block * sdp->sd_hash_bsize,
			      sdp->sd_hash_bsize);
		if (count != sdp->sd_hash_bsize) {
			fprintf(stderr, "dir_double_exhash (1)\n");
			exit(1);
		}

		from = buf;
		to = (uint64_t *)((char *)buf + sdp->sd_hash_bsize);

		for (x = sdp->sd_hash_ptrs; x--; from++) {
			*to++ = *from;
			*to++ = *from;
		}

		if (sdp->gfs1)
			count = gfs1_writei(dip, (char *)buf +
					    sdp->sd_hash_bsize,
					    block * sdp->bsize, sdp->bsize);
		else
			count = gfs2_writei(dip, (char *)buf +
					    sdp->sd_hash_bsize,
					    block * sdp->bsize, sdp->bsize);
		if (count != sdp->bsize) {
			fprintf(stderr, "dir_double_exhash (2)\n");
			exit(1);
		}
	}

	free(buf);

	dip->i_di.di_depth++;
	bmodified(dip->i_bh);
}

/**
 * get_leaf - Get leaf
 * @dip:
 * @leaf_no:
 * @bh_out:
 *
 * Returns: 0 on success, error code otherwise
 */

int gfs2_get_leaf(struct gfs2_inode *dip, uint64_t leaf_no,
				  struct gfs2_buffer_head **bhp)
{
	int error = 0;

	*bhp = bread(dip->i_sbd, leaf_no);
	error = gfs2_check_meta(*bhp, GFS2_METATYPE_LF);
	if(error)
		brelse(*bhp);
	return error;
}

/**
 * get_first_leaf - Get first leaf
 * @dip: The GFS2 inode
 * @index:
 * @bh_out:
 *
 * Returns: 0 on success, error code otherwise
 */

static int get_first_leaf(struct gfs2_inode *dip, uint32_t lindex, struct gfs2_buffer_head **bh_out)
{
	uint64_t leaf_no;

	if (lgfs2_get_leaf_ptr(dip, lindex, &leaf_no) != 0)
		return -1;
	*bh_out = bread(dip->i_sbd, leaf_no);
	if (*bh_out == NULL)
		return -1;
	return 0;
}

/**
 * get_next_leaf - Get next leaf
 * @dip: The GFS2 inode
 * @bh_in: The buffer
 * @bh_out:
 *
 * Returns: 0 on success, error code otherwise
 */

static int get_next_leaf(struct gfs2_inode *dip,struct gfs2_buffer_head *bh_in,
						 struct gfs2_buffer_head **bh_out)
{
	struct gfs2_leaf *leaf;

	leaf = (struct gfs2_leaf *)bh_in->b_data;

	if (!leaf->lf_next)
		return -1;
	/* Check for a leaf that points to itself as "next" */
	if (be64_to_cpu(leaf->lf_next) == bh_in->b_blocknr)
		return -1;
	*bh_out = bread(dip->i_sbd, be64_to_cpu(leaf->lf_next));
	if (*bh_out == NULL)
		return -ENOENT;
	/* Check for a leaf pointing to a non-leaf */
	if (gfs2_check_meta(*bh_out, GFS2_METATYPE_LF)) {
		brelse(*bh_out);
		*bh_out = NULL;
		return -ENOENT;
	}
	return 0;
}

static int dir_e_add(struct gfs2_inode *dip, const char *filename, int len,
		      struct gfs2_inum *inum, unsigned int type)
{
	struct gfs2_buffer_head *bh, *nbh;
	struct gfs2_leaf *leaf, *nleaf;
	struct gfs2_dirent *dent;
	uint32_t lindex, llen;
	uint32_t hash;
	uint64_t leaf_no, bn;
	int err = 0;

	hash = gfs2_disk_hash(filename, len);
restart:
	/* Have to kludge because (hash >> 32) gives hash for some reason. */
	if (dip->i_di.di_depth)
		lindex = hash >> (32 - dip->i_di.di_depth);
	else
		lindex = 0;

	err = lgfs2_get_leaf_ptr(dip, lindex, &leaf_no);
	if (err)
		return err;

	for (;;) {
		bh = bread(dip->i_sbd, leaf_no);
		leaf = (struct gfs2_leaf *)bh->b_data;

		if (dirent_alloc(dip, bh, len, &dent)) {

			if (be16_to_cpu(leaf->lf_depth) < dip->i_di.di_depth) {
				llen = 1 << (dip->i_di.di_depth -
					     be16_to_cpu(leaf->lf_depth));
				dir_split_leaf(dip, lindex & ~(llen - 1),
					       leaf_no, bh);
				brelse(bh);
				goto restart;

			} else if (dip->i_di.di_depth < GFS2_DIR_MAX_DEPTH) {
				brelse(bh);
				dir_double_exhash(dip);
				goto restart;

			} else if (leaf->lf_next) {
				leaf_no = be64_to_cpu(leaf->lf_next);
				brelse(bh);
				continue;

			} else {
				struct gfs2_meta_header mh;

				if (lgfs2_meta_alloc(dip, &bn))
					exit(1);
				nbh = bget(dip->i_sbd, bn);
				mh.mh_magic = GFS2_MAGIC;
				mh.mh_type = GFS2_METATYPE_LF;
				mh.mh_format = GFS2_FORMAT_LF;
				gfs2_meta_header_out(&mh, nbh->b_data);
				bmodified(nbh);

				leaf->lf_next = cpu_to_be64(bn);

				nleaf = (struct gfs2_leaf *)nbh->b_data;
				nleaf->lf_depth = leaf->lf_depth;
				nleaf->lf_dirent_format = cpu_to_be32(GFS2_FORMAT_DE);
#ifdef GFS2_HAS_LEAF_HINTS
				nleaf->lf_inode = cpu_to_be64(dip->i_di.di_num.no_addr);
#endif
				err = dirent_alloc(dip, nbh, len, &dent);
				if (err)
					return err;
				dip->i_di.di_blocks++;
				bmodified(dip->i_bh);
				bmodified(bh);
				brelse(bh);
				bh = nbh;
				leaf = nleaf;
			}
		}

		gfs2_inum_out(inum, (char *)&dent->de_inum);
		dent->de_hash = cpu_to_be32(hash);
		dent->de_type = cpu_to_be16(type);
		memcpy((char *)(dent + 1), filename, len);

		leaf->lf_entries = be16_to_cpu(leaf->lf_entries) + 1;
		leaf->lf_entries = cpu_to_be16(leaf->lf_entries);

		bmodified(bh);
		brelse(bh);
		return err;
	}
}

static void dir_make_exhash(struct gfs2_inode *dip)
{
	struct gfs2_sbd *sdp = dip->i_sbd;
	struct gfs2_dirent *dent;
	struct gfs2_buffer_head *bh;
	struct gfs2_leaf *leaf;
	int y;
	uint32_t x;
	uint64_t *lp, bn;

	if (lgfs2_meta_alloc(dip, &bn))
		exit(1);
	bh = bget(sdp, bn);
	{
		struct gfs2_meta_header mh;
		mh.mh_magic = GFS2_MAGIC;
		mh.mh_type = GFS2_METATYPE_LF;
		mh.mh_format = GFS2_FORMAT_LF;
		gfs2_meta_header_out(&mh, bh->b_data);
		bmodified(bh);
	}

	leaf = (struct gfs2_leaf *)bh->b_data;
	leaf->lf_dirent_format = cpu_to_be32(GFS2_FORMAT_DE);
	leaf->lf_entries = cpu_to_be16(dip->i_di.di_entries);
#ifdef GFS2_HAS_LEAF_HINTS
	leaf->lf_inode = cpu_to_be64(dip->i_di.di_num.no_addr);
#endif
	buffer_copy_tail(sdp, bh, sizeof(struct gfs2_leaf),
			 dip->i_bh, sizeof(struct gfs2_dinode));

	x = 0;
	gfs2_dirent_first(dip, bh, &dent);

	do {
		if (!dent->de_inum.no_formal_ino)
			continue;
		if (++x == dip->i_di.di_entries)
			break;
	} while (gfs2_dirent_next(dip, bh, &dent) == 0);

	dent->de_rec_len = be16_to_cpu(dent->de_rec_len);
	dent->de_rec_len = cpu_to_be16(dent->de_rec_len +
		sizeof(struct gfs2_dinode) - sizeof(struct gfs2_leaf));

	/* no need to: bmodified(bh); (buffer_copy_tail does it) */
	brelse(bh);

	buffer_clear_tail(sdp, dip->i_bh, sizeof(struct gfs2_dinode));

	lp = (uint64_t *)(dip->i_bh->b_data + sizeof(struct gfs2_dinode));

	for (x = sdp->sd_hash_ptrs; x--; lp++)
		*lp = cpu_to_be64(bn);

	dip->i_di.di_size = sdp->bsize / 2;
	dip->i_di.di_blocks++;
	dip->i_di.di_flags |= GFS2_DIF_EXHASH;
	dip->i_di.di_payload_format = 0;
	/* no need: bmodified(dip->i_bh); buffer_clear_tail does it. */

	for (x = sdp->sd_hash_ptrs, y = -1; x; x >>= 1, y++) ;
	dip->i_di.di_depth = y;

	gfs2_dinode_out(&dip->i_di, dip->i_bh->b_data);
	bwrite(dip->i_bh);
}

static int dir_l_add(struct gfs2_inode *dip, const char *filename, int len,
		      struct gfs2_inum *inum, unsigned int type)
{
	struct gfs2_dirent *dent;
	int err = 0;

	if (dirent_alloc(dip, dip->i_bh, len, &dent)) {
		dir_make_exhash(dip);
		err = dir_e_add(dip, filename, len, inum, type);
		return err;
	}

	gfs2_inum_out(inum, (char *)&dent->de_inum);
	dent->de_hash = gfs2_disk_hash(filename, len);
	dent->de_hash = cpu_to_be32(dent->de_hash);
	dent->de_type = cpu_to_be16(type);
	memcpy((char *)(dent + 1), filename, len);
	bmodified(dip->i_bh);
	return err;
}

int dir_add(struct gfs2_inode *dip, const char *filename, int len,
	     struct gfs2_inum *inum, unsigned int type)
{
	int err = 0;
	if (dip->i_di.di_flags & GFS2_DIF_EXHASH)
		err = dir_e_add(dip, filename, len, inum, type);
	else
		err = dir_l_add(dip, filename, len, inum, type);
	return err;
}

static int __init_dinode(struct gfs2_sbd *sdp, struct gfs2_buffer_head **bhp, struct gfs2_inum *inum,
                         unsigned int mode, uint32_t flags, struct gfs2_inum *parent, int gfs1)
{
	struct gfs2_buffer_head *bh;
	struct gfs2_dinode di = {{0}};
	int is_dir;

	if (gfs1)
		is_dir = (IF2DT(mode) == GFS_FILE_DIR);
	else
		is_dir = S_ISDIR(mode);

	errno = EINVAL;
	if (bhp == NULL)
		return 1;

	if (*bhp == NULL) {
		*bhp = bget(sdp, inum->no_addr);
		if (*bhp == NULL)
			return 1;
	}

	bh = *bhp;

	di.di_header.mh_magic = GFS2_MAGIC;
	di.di_header.mh_type = GFS2_METATYPE_DI;
	di.di_header.mh_format = GFS2_FORMAT_DI;
	di.di_num = *inum;
	di.di_mode = mode;
	di.di_nlink = 1;
	di.di_blocks = 1;
	di.di_atime = di.di_mtime = di.di_ctime = sdp->time;
	di.di_goal_meta = di.di_goal_data = bh->b_blocknr;
	di.di_flags = flags;

	if (is_dir) {
		struct gfs2_dirent de1, de2;

		memset(&de1, 0, sizeof(struct gfs2_dirent));
		de1.de_inum = di.di_num;
		de1.de_hash = gfs2_disk_hash(".", 1);
		de1.de_rec_len = GFS2_DIRENT_SIZE(1);
		de1.de_name_len = 1;
		de1.de_type = (gfs1 ? GFS_FILE_DIR : IF2DT(S_IFDIR));

		memset(&de2, 0, sizeof(struct gfs2_dirent));
		de2.de_inum = *parent;
		de2.de_hash = gfs2_disk_hash("..", 2);
		de2.de_rec_len = sdp->bsize - sizeof(struct gfs2_dinode) - de1.de_rec_len;
		de2.de_name_len = 2;
		de2.de_type = (gfs1 ? GFS_FILE_DIR : IF2DT(S_IFDIR));

		gfs2_dirent_out(&de1, bh->b_data + sizeof(struct gfs2_dinode));
		memcpy(bh->b_data +
		       sizeof(struct gfs2_dinode) +
		       sizeof(struct gfs2_dirent),
		       ".", 1);
		gfs2_dirent_out(&de2, bh->b_data + sizeof(struct gfs2_dinode) + de1.de_rec_len);
		memcpy(bh->b_data +
		       sizeof(struct gfs2_dinode) +
		       de1.de_rec_len +
		       sizeof(struct gfs2_dirent),
		       "..", 2);

		di.di_nlink = 2;
		di.di_size = sdp->bsize - sizeof(struct gfs2_dinode);
		di.di_flags |= GFS2_DIF_JDATA;
		di.di_payload_format = GFS2_FORMAT_DE;
		di.di_entries = 2;
	}
	gfs2_dinode_out(&di, bh->b_data);
	bmodified(bh);
	return 0;
}

int init_dinode(struct gfs2_sbd *sdp, struct gfs2_buffer_head **bhp, struct gfs2_inum *inum,
                unsigned int mode, uint32_t flags, struct gfs2_inum *parent)
{
	return __init_dinode(sdp, bhp, inum, mode, flags, parent, 0);
}

static void lgfs2_fill_indir(char *start, char *end, uint64_t ptr0, unsigned n, unsigned *p)
{
	char *bp;
	memset(start, 0, end - start);
	for (bp = start; bp < end && *p < n; bp += sizeof(uint64_t)) {
		uint64_t pn = ptr0 + *p;
		*(uint64_t *)bp = cpu_to_be64(pn);
		(*p)++;
	}
}

/**
 * Calculate and write the indirect blocks for a single-extent file of a given
 * size.
 * ip: The inode for which to write indirect blocks, with fields already set
 *     appropriately (see lgfs2_file_alloc).
 * Returns 0 on success or non-zero with errno set on failure.
 */
int lgfs2_write_filemeta(struct gfs2_inode *ip)
{
	unsigned height = 0;
	struct metapath mp;
	struct gfs2_sbd *sdp = ip->i_sbd;
	uint64_t dblocks = (ip->i_di.di_size + sdp->bsize - 1) / sdp->bsize;
	uint64_t ptr0 = ip->i_di.di_num.no_addr + 1;
	unsigned ptrs = 1;
	struct gfs2_meta_header mh = {
		.mh_magic = GFS2_MAGIC,
		.mh_type = GFS2_METATYPE_IN,
		.mh_format = GFS2_FORMAT_IN,
	};
	struct gfs2_buffer_head *bh = bget(sdp, ip->i_di.di_num.no_addr);
	if (bh == NULL)
		return 1;

	/* Using find_metapath() to find the last data block in the file will
	   effectively give a remainder for the number of pointers at each
	   height. Just need to add 1 to convert ptr index to quantity later. */
	find_metapath(ip, dblocks - 1, &mp);

	for (height = 0; height < ip->i_di.di_height; height++) {
		unsigned p;
		/* The number of pointers in this height will be the number of
		   full indirect blocks pointed to by the previous height
		   multiplied by the pointer capacity of an indirect block,
		   plus the remainder which find_metapath() gave us. */
		ptrs = ((ptrs - 1) * sdp->sd_inptrs) + mp.mp_list[height] + 1;

		for (p = 0; p < ptrs; bh->b_blocknr++) {
			char *start = bh->b_data;
			if (height == 0) {
				start += sizeof(struct gfs2_dinode);
				gfs2_dinode_out(&ip->i_di, bh->b_data);
			} else {
				start += sizeof(struct gfs2_meta_header);
				gfs2_meta_header_out(&mh, bh->b_data);
			}
			lgfs2_fill_indir(start, bh->b_data + sdp->bsize, ptr0, ptrs, &p);
			if (bwrite(bh)) {
				free(bh);
				return 1;
			}
		}
		ptr0 += ptrs;
	}
	free(bh);
	return 0;
}

static struct gfs2_inode *__createi(struct gfs2_inode *dip,
				    const char *filename, unsigned int mode,
				    uint32_t flags, int if_gfs1)
{
	struct gfs2_sbd *sdp = dip->i_sbd;
	uint64_t bn;
	struct gfs2_inum inum;
	struct gfs2_buffer_head *bh = NULL;
	struct gfs2_inode *ip;
	int err = 0;
	int is_dir;

	gfs2_lookupi(dip, filename, strlen(filename), &ip);
	if (!ip) {
		err = lgfs2_dinode_alloc(sdp, 1, &bn);
		if (err != 0)
			return NULL;

		if (if_gfs1)
			inum.no_formal_ino = bn;
		else
			inum.no_formal_ino = sdp->md.next_inum++;
		inum.no_addr = bn;

		err = dir_add(dip, filename, strlen(filename), &inum, IF2DT(mode));
		if (err)
			return NULL;

		if (if_gfs1)
			is_dir = (IF2DT(mode) == GFS_FILE_DIR);
		else
			is_dir = S_ISDIR(mode);
		if (is_dir) {
			bmodified(dip->i_bh);
			dip->i_di.di_nlink++;
		}

		err = __init_dinode(sdp, &bh, &inum, mode, flags, &dip->i_di.di_num, if_gfs1);
		if (err != 0)
			return NULL;

		ip = lgfs2_inode_get(sdp, bh);
		if (ip == NULL)
			return NULL;
		bmodified(bh);
	}
	ip->bh_owned = 1;
	return ip;
}

struct gfs2_inode *createi(struct gfs2_inode *dip, const char *filename,
			   unsigned int mode, uint32_t flags)
{
	return __createi(dip, filename, mode, flags, 0);
}

struct gfs2_inode *gfs_createi(struct gfs2_inode *dip, const char *filename,
			   unsigned int mode, uint32_t flags)
{
	return __createi(dip, filename, mode, flags, 1);
}

/**
 * gfs2_filecmp - Compare two filenames
 * @file1: The first filename
 * @file2: The second filename
 * @len_of_file2: The length of the second file
 *
 * This routine compares two filenames and returns 1 if they are equal.
 *
 * Returns: 1 if the files are the same, otherwise 0.
 */

static int gfs2_filecmp(const char *file1, const char *file2, int len_of_file2)
{
	if (strlen(file1) != len_of_file2)
		return 0;
	if (memcmp(file1, file2, len_of_file2))
		return 0;
	return 1;
}

/**
 * leaf_search
 * @bh:
 * @id:
 * @dent_out:
 * @dent_prev:
 *
 * Returns:
 */
static int leaf_search(struct gfs2_inode *dip, struct gfs2_buffer_head *bh, 
		       const char *filename, int len,
		       struct gfs2_dirent **dent_out,
		       struct gfs2_dirent **dent_prev)
{
	uint32_t hash;
	struct gfs2_dirent *dent, *prev = NULL;
	unsigned int entries = 0, x = 0;
	int type;

	type = gfs2_dirent_first(dip, bh, &dent);

	if (type == IS_LEAF){
		struct gfs2_leaf *leaf = (struct gfs2_leaf *)bh->b_data;
		entries = be16_to_cpu(leaf->lf_entries);
	} else if (type == IS_DINODE)
		entries = dip->i_di.di_entries;
	else
		return -1;

	hash = gfs2_disk_hash(filename, len);

	do{
		if (!dent->de_inum.no_formal_ino){
			prev = dent;
			continue;
		}
		
		if (be32_to_cpu(dent->de_hash) == hash &&
			gfs2_filecmp(filename, (char *)(dent + 1),
				     be16_to_cpu(dent->de_name_len))) {
			*dent_out = dent;
			if (dent_prev)
				*dent_prev = prev;
			return 0;
		}
		
		if(x >= entries)
			return -1;
		x++;
		prev = dent;
	} while (gfs2_dirent_next(dip, bh, &dent) == 0);

	return -ENOENT;
}

/**
 * linked_leaf_search - Linked leaf search
 * @dip: The GFS2 inode
 * @id:
 * @dent_out:
 * @dent_prev:
 * @bh_out:
 *
 * Returns: 0 on sucess, error code otherwise
 */

static int linked_leaf_search(struct gfs2_inode *dip, const char *filename,
			      int len, struct gfs2_dirent **dent_out,
			      struct gfs2_buffer_head **bh_out)
{
	struct gfs2_buffer_head *bh = NULL, *bh_next;
	uint32_t hsize, lindex;
	uint32_t hash;
	int error = 0;

	hsize = 1 << dip->i_di.di_depth;
	if(hsize * sizeof(uint64_t) != dip->i_di.di_size)
		return -1;

	/*  Figure out the address of the leaf node.  */

	hash = gfs2_disk_hash(filename, len);
	lindex = hash >> (32 - dip->i_di.di_depth);

	error = get_first_leaf(dip, lindex, &bh_next);
	if (error)
		return error;
	if (bh_next == NULL)
		return errno;

	/*  Find the entry  */
	do{
		if (bh && bh != dip->i_bh)
			brelse(bh);

		bh = bh_next;

		error = leaf_search(dip, bh, filename, len, dent_out, NULL);
		switch (error){
		case 0:
			*bh_out = bh;
			return 0;

		case -ENOENT:
			break;

		default:
			if (bh && bh != dip->i_bh)
				brelse(bh);
			return error;
		}

		error = get_next_leaf(dip, bh, &bh_next);
	} while (!error && bh_next != NULL);

	if (bh && bh != dip->i_bh)
		brelse(bh);

	return error;
}

/**
 * dir_e_search -
 * @dip: The GFS2 inode
 * @id:
 * @inode:
 *
 * Returns:
 */
static int dir_e_search(struct gfs2_inode *dip, const char *filename,
			int len, unsigned int *type, struct gfs2_inum *inum)
{
	struct gfs2_buffer_head *bh = NULL;
	struct gfs2_dirent *dent;
	int error;

	error = linked_leaf_search(dip, filename, len, &dent, &bh);
	if (error)
		return error;

	gfs2_inum_in(inum, (char *)&dent->de_inum);
	if (type)
		*type = be16_to_cpu(dent->de_type);

	brelse(bh);

	return 0;
}


/**
 * dir_l_search -
 * @dip: The GFS2 inode
 * @id:
 * @inode:
 *
 * Returns:
 */
static int dir_l_search(struct gfs2_inode *dip, const char *filename,
			int len, unsigned int *type, struct gfs2_inum *inum)
{
	struct gfs2_dirent *dent;
	int error;

	if(!inode_is_stuffed(dip))
		return -1;

	error = leaf_search(dip, dip->i_bh, filename, len, &dent, NULL);
	if (!error) {
		gfs2_inum_in(inum, (char *)&dent->de_inum);
		if(type)
			*type = be16_to_cpu(dent->de_type);
	}
	return error;
}

/**
 * dir_search - Search a directory
 * @dip: The GFS inode
 * @id
 * @type:
 *
 * This routine searches a directory for a file or another directory
 * given its filename.  The component of the identifier that is
 * not being used to search will be filled in and must be freed by
 * the caller.
 *
 * Returns: 0 if found, -1 on failure, -ENOENT if not found.
 */
int dir_search(struct gfs2_inode *dip, const char *filename, int len,
		      unsigned int *type, struct gfs2_inum *inum)
{
	int error;

	if(!S_ISDIR(dip->i_di.di_mode) && !is_gfs_dir(&dip->i_di))
		return -1;

	if (dip->i_di.di_flags & GFS2_DIF_EXHASH)
		error = dir_e_search(dip, filename, len, type, inum);
	else
		error = dir_l_search(dip, filename, len, type, inum);

	return error;
}

static int dir_e_del(struct gfs2_inode *dip, const char *filename, int len)
{
	int lindex;
	int error;
	int found = 0;
	uint64_t leaf_no;
	struct gfs2_buffer_head *bh = NULL;
	struct gfs2_dirent *cur, *prev;

	lindex = (1 << (dip->i_di.di_depth))-1;

	for(; (lindex >= 0) && !found; lindex--){
		error = lgfs2_get_leaf_ptr(dip, lindex, &leaf_no);
		if (error)
			return error;

		while(leaf_no && !found){
			bh = bread(dip->i_sbd, leaf_no);
			error = leaf_search(dip, bh, filename, len, &cur, &prev);
			if (error) {
				if(error != -ENOENT){
					brelse(bh);
					return -1;
				}
				leaf_no = be64_to_cpu(((struct gfs2_leaf *)bh->b_data)->lf_next);
				brelse(bh);
			} else
				found = 1;
		}
	}

	if(!found)
		return 1;

	if (bh) {
		dirent2_del(dip, bh, prev, cur);
		brelse(bh);
	}
	return 0;
}

static int dir_l_del(struct gfs2_inode *dip, const char *filename, int len)
{
	int error=0;
	struct gfs2_dirent *cur, *prev;

	if(!inode_is_stuffed(dip))
		return -1;

	error = leaf_search(dip, dip->i_bh, filename, len, &cur, &prev);
	if (error) {
		if (error == -ENOENT)
			return 1;
		else
			return -1;
	}

	dirent2_del(dip, dip->i_bh, prev, cur);
	return 0;
}


/*
 * gfs2_dirent_del
 * @dip
 * filename
 *
 * Delete a directory entry from a directory.  This _only_
 * removes the directory entry - leaving the dinode in
 * place.  (Likely without a link.)
 *
 * Returns: 0 on success (or if it doesn't already exist), -1 on failure
 */
int gfs2_dirent_del(struct gfs2_inode *dip, const char *filename, int len)
{
	int error;

	if(!S_ISDIR(dip->i_di.di_mode) && !is_gfs_dir(&dip->i_di))
		return -1;

	if (dip->i_di.di_flags & GFS2_DIF_EXHASH)
		error = dir_e_del(dip, filename, len);
	else
		error = dir_l_del(dip, filename, len);
	bmodified(dip->i_bh);
	return error;
}

/**
 * gfs2_lookupi - Look up a filename in a directory and return its inode
 * @dip: The directory to search
 * @name: The name of the inode to look for
 * @ipp: Used to return the found inode if any
 *
 * Returns: 0 on success, -EXXXX on failure
 */
int gfs2_lookupi(struct gfs2_inode *dip, const char *filename, int len,
		 struct gfs2_inode **ipp)
{
	struct gfs2_sbd *sdp = dip->i_sbd;
	int error = 0;
	struct gfs2_inum inum;

	*ipp = NULL;

	if (!len || len > GFS2_FNAMESIZE)
		return -ENAMETOOLONG;
	if (gfs2_filecmp(filename, (char *)".", 1)) {
		*ipp = dip;
		return 0;
	}
	error = dir_search(dip, filename, len, NULL, &inum);
	if (!error)
		*ipp = lgfs2_inode_read(sdp, inum.no_addr);

	return error;
}

/**
 * gfs2_free_block - free up a block given its block number
 */
void gfs2_free_block(struct gfs2_sbd *sdp, uint64_t block)
{
	struct rgrp_tree *rgd;

	/* Adjust the free space count for the freed block */
	rgd = gfs2_blk2rgrpd(sdp, block); /* find the rg for indir block */
	if (rgd) {
		gfs2_set_bitmap(rgd, block, GFS2_BLKST_FREE);
		rgd->rg.rg_free++; /* adjust the free count */
		if (sdp->gfs1)
			gfs_rgrp_out((struct gfs_rgrp *)&rgd->rg, rgd->bits[0].bi_bh);
		else
			gfs2_rgrp_out(&rgd->rg, rgd->bits[0].bi_bh->b_data);
		bmodified(rgd->bits[0].bi_bh);
		sdp->blks_alloced--;
	}
}

/**
 * gfs2_freedi - unlink a disk inode by block number.
 * Note: currently only works for regular files.
 */
int gfs2_freedi(struct gfs2_sbd *sdp, uint64_t diblock)
{
	struct gfs2_inode *ip;
	struct gfs2_buffer_head *bh, *nbh;
	int h, head_size;
	uint64_t *ptr, block;
	struct rgrp_tree *rgd;
	uint32_t height;
	osi_list_t metalist[GFS2_MAX_META_HEIGHT];
	osi_list_t *cur_list, *next_list, *tmp;

	for (h = 0; h < GFS2_MAX_META_HEIGHT; h++)
		osi_list_init(&metalist[h]);

	bh = bread(sdp, diblock);
	if (bh == NULL)
		return -1;
	ip = lgfs2_inode_get(sdp, bh);
	if (ip == NULL)
		return -1;
	height = ip->i_di.di_height;
	osi_list_add(&bh->b_altlist, &metalist[0]);

	for (h = 0; h < height; h++){
		cur_list = &metalist[h];
		next_list = &metalist[h + 1];
		head_size = (h > 0 ? sizeof(struct gfs2_meta_header) :
			     sizeof(struct gfs2_dinode));

		for (tmp = cur_list->next; tmp != cur_list; tmp = tmp->next){
			bh = osi_list_entry(tmp, struct gfs2_buffer_head,
					    b_altlist);

			for (ptr = (uint64_t *)(bh->b_data + head_size);
			     (char *)ptr < (bh->b_data + sdp->bsize); ptr++) {
				if (!*ptr)
					continue;

				block = be64_to_cpu(*ptr);
				gfs2_free_block(sdp, block);
				if (h == height - 1) /* if not metadata */
					continue; /* don't queue it up */
				/* Read the next metadata block in the chain */
				nbh = bread(sdp, block);
				osi_list_add(&nbh->b_altlist, next_list);
				brelse(nbh);
			}
		}
	}
	rgd = gfs2_blk2rgrpd(sdp, diblock);
	gfs2_set_bitmap(rgd, diblock, GFS2_BLKST_FREE);
	inode_put(&ip);
	/* inode_put deallocated the extra block used by the disk inode, */
	/* so adjust it in the superblock struct */
	sdp->blks_alloced--;
	rgd->rg.rg_free++;
	rgd->rg.rg_dinodes--;
	if (sdp->gfs1)
		gfs_rgrp_out((struct gfs_rgrp *)&rgd->rg, rgd->bits[0].bi_bh);
	else
		gfs2_rgrp_out(&rgd->rg, rgd->bits[0].bi_bh->b_data);
	bmodified(rgd->bits[0].bi_bh);
	sdp->dinodes_alloced--;
	return 0;
}
