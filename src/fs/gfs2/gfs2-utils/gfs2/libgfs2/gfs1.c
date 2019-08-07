#include "clusterautoconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/gfs2_ondisk.h>

#include "osi_list.h"
#include "libgfs2.h"

/* GFS1 compatibility functions - so that programs like gfs2_convert
   and gfs2_edit can examine/manipulate GFS1 file systems. */

static __inline__ int fs_is_jdata(struct gfs2_inode *ip)
{
        return ip->i_di.di_flags & GFS2_DIF_JDATA;
}

static __inline__ uint64_t *
gfs1_metapointer(struct gfs2_buffer_head *bh, unsigned int height,
		 struct metapath *mp)
{
	unsigned int head_size = (height > 0) ?
		sizeof(struct gfs_indirect) : sizeof(struct gfs_dinode);

	return ((uint64_t *)(bh->b_data + head_size)) + mp->mp_list[height];
}

int is_gfs_dir(struct gfs2_dinode *dinode)
{
	if (dinode->__pad1 == GFS_FILE_DIR)
		return 1;
	return 0;
}

void gfs1_lookup_block(struct gfs2_inode *ip, struct gfs2_buffer_head *bh,
		  unsigned int height, struct metapath *mp,
		  int create, int *new, uint64_t *block)
{
	uint64_t *ptr = gfs1_metapointer(bh, height, mp);

	if (*ptr) {
		*block = be64_to_cpu(*ptr);
		return;
	}

	*block = 0;

	if (!create)
		return;

	if (lgfs2_meta_alloc(ip, block)) {
		*block = 0;
		return;
	}

	*ptr = cpu_to_be64(*block);
	bmodified(bh);
	ip->i_di.di_blocks++;
	bmodified(ip->i_bh);

	*new = 1;
}

void gfs1_block_map(struct gfs2_inode *ip, uint64_t lblock, int *new,
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

	if (!ip->i_di.di_height) { /* stuffed */
		if (!lblock) {
			*dblock = ip->i_di.di_num.no_addr;
			if (extlen)
				*extlen = 1;
		}
		return;
	}

	bsize = (fs_is_jdata(ip)) ? sdp->sd_jbsize : sdp->bsize;

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
		gfs1_lookup_block(ip, bh, x, &mp, create, new, dblock);
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
		gfs1_lookup_block(ip, bh, end_of_metadata, &mp, create, new,
				  dblock);

	if (extlen && *dblock) {
		*extlen = 1;

		if (!*new) {
			uint64_t tmp_dblock;
			int tmp_new;
			unsigned int nptrs;

			nptrs = (end_of_metadata) ? sdp->sd_inptrs : sdp->sd_diptrs;

			while (++mp.mp_list[end_of_metadata] < nptrs) {
				gfs1_lookup_block(ip, bh, end_of_metadata, &mp,
						  FALSE, &tmp_new,
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

int gfs1_writei(struct gfs2_inode *ip, char *buf, uint64_t offset,
		unsigned int size)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	struct gfs2_buffer_head *bh;
	uint64_t lblock, dblock;
	uint32_t extlen = 0;
	unsigned int amount;
	int new;
	int journaled = fs_is_jdata(ip);
	const uint64_t start = offset;
	int copied = 0;

	if (!size)
		return 0;

	if (!ip->i_di.di_height && /* stuffed */
	    ((start + size) > (sdp->bsize - sizeof(struct gfs_dinode))))
		unstuff_dinode(ip);

	if (journaled) {
		lblock = offset / sdp->sd_jbsize;
		offset %= sdp->sd_jbsize;
	} else {
		lblock = offset >> sdp->sd_sb.sb_bsize_shift;
		offset &= sdp->bsize - 1;
	}

	if (!ip->i_di.di_height) /* stuffed */
		offset += sizeof(struct gfs_dinode);
	else if (journaled)
		offset += sizeof(struct gfs2_meta_header);

	while (copied < size) {
		amount = size - copied;
		if (amount > sdp->bsize - offset)
			amount = sdp->bsize - offset;

		if (!extlen){
			new = TRUE;
			gfs1_block_map(ip, lblock, &new, &dblock, &extlen, 0);
			if (!dblock)
				return -1;
		}

		if (dblock == ip->i_di.di_num.no_addr)
			bh = ip->i_bh;
		else
			bh = bread(sdp, dblock);

		if (journaled && dblock != ip->i_di.di_num.no_addr ) {
			struct gfs2_meta_header mh;

			mh.mh_magic = GFS2_MAGIC;
			mh.mh_type = GFS2_METATYPE_JD;
			mh.mh_format = GFS2_FORMAT_JD;
			gfs2_meta_header_out(&mh, bh->b_data);
		}

		memcpy(bh->b_data + offset, buf + copied, amount);
		bmodified(bh);
		if (bh != ip->i_bh)
			brelse(bh);

		copied += amount;
		lblock++;
		dblock++;
		extlen--;

		offset = (journaled) ? sizeof(struct gfs2_meta_header) : 0;
	}

	if (ip->i_di.di_size < start + copied) {
		bmodified(ip->i_bh);
		ip->i_di.di_size = start + copied;
	}
	ip->i_di.di_mtime = ip->i_di.di_ctime = time(NULL);
	gfs2_dinode_out(&ip->i_di, ip->i_bh->b_data);
	bmodified(ip->i_bh);
	return copied;
}

/* ------------------------------------------------------------------------ */
/* gfs_dinode_in */
/* ------------------------------------------------------------------------ */
static void gfs_dinode_in(struct gfs_dinode *di, struct gfs2_buffer_head *bh)
{
	struct gfs_dinode *str = (struct gfs_dinode *)bh->b_data;

	gfs2_meta_header_in(&di->di_header, bh->b_data);
	gfs2_inum_in(&di->di_num, (char *)&str->di_num);

	di->di_mode = be32_to_cpu(str->di_mode);
	di->di_uid = be32_to_cpu(str->di_uid);
	di->di_gid = be32_to_cpu(str->di_gid);
	di->di_nlink = be32_to_cpu(str->di_nlink);
	di->di_size = be64_to_cpu(str->di_size);
	di->di_blocks = be64_to_cpu(str->di_blocks);
	di->di_atime = be64_to_cpu(str->di_atime);
	di->di_mtime = be64_to_cpu(str->di_mtime);
	di->di_ctime = be64_to_cpu(str->di_ctime);
	di->di_major = be32_to_cpu(str->di_major);
	di->di_minor = be32_to_cpu(str->di_minor);
	di->di_goal_dblk = be64_to_cpu(str->di_goal_dblk);
	di->di_goal_mblk = be64_to_cpu(str->di_goal_mblk);
	di->di_flags = be32_to_cpu(str->di_flags);
	di->di_payload_format = be32_to_cpu(str->di_payload_format);
	di->di_type = be16_to_cpu(str->di_type);
	di->di_height = be16_to_cpu(str->di_height);
	di->di_depth = be16_to_cpu(str->di_depth);
	di->di_entries = be32_to_cpu(str->di_entries);
	di->di_eattr = be64_to_cpu(str->di_eattr);
}

static struct gfs2_inode *__gfs_inode_get(struct gfs2_sbd *sdp,
					  struct gfs2_buffer_head *bh,
					  uint64_t di_addr)
{
	struct gfs_dinode gfs1_dinode;
	struct gfs2_inode *ip;

	ip = calloc(1, sizeof(struct gfs2_inode));
	if (ip == NULL) {
		return NULL;
	}

	ip->bh_owned = 0;
	if (!bh) {
		bh = bread(sdp, di_addr);
		ip->bh_owned = 1;
	}
	gfs_dinode_in(&gfs1_dinode, bh);
	memcpy(&ip->i_di.di_header, &gfs1_dinode.di_header,
	       sizeof(struct gfs2_meta_header));
	memcpy(&ip->i_di.di_num, &gfs1_dinode.di_num,
	       sizeof(struct gfs2_inum));
	ip->i_di.di_mode = gfs1_dinode.di_mode;
	ip->i_di.di_uid = gfs1_dinode.di_uid;
	ip->i_di.di_gid = gfs1_dinode.di_gid;
	ip->i_di.di_nlink = gfs1_dinode.di_nlink;
	ip->i_di.di_size = gfs1_dinode.di_size;
	ip->i_di.di_blocks = gfs1_dinode.di_blocks;
	ip->i_di.di_atime = gfs1_dinode.di_atime;
	ip->i_di.di_mtime = gfs1_dinode.di_mtime;
	ip->i_di.di_ctime = gfs1_dinode.di_ctime;
	ip->i_di.di_major = gfs1_dinode.di_major;
	ip->i_di.di_minor = gfs1_dinode.di_minor;
	ip->i_di.di_goal_data = gfs1_dinode.di_goal_dblk;
	ip->i_di.di_goal_meta = gfs1_dinode.di_goal_mblk;
	ip->i_di.di_flags = gfs1_dinode.di_flags;
	ip->i_di.di_payload_format = gfs1_dinode.di_payload_format;
	ip->i_di.__pad1 = gfs1_dinode.di_type;
	ip->i_di.di_height = gfs1_dinode.di_height;
	ip->i_di.di_depth = gfs1_dinode.di_depth;
	ip->i_di.di_entries = gfs1_dinode.di_entries;
	ip->i_di.di_eattr = gfs1_dinode.di_eattr;
	ip->i_bh = bh;
	ip->i_sbd = sdp;
	return ip;
}

struct gfs2_inode *lgfs2_gfs_inode_get(struct gfs2_sbd *sdp,
				 struct gfs2_buffer_head *bh)
{
	return __gfs_inode_get(sdp, bh, 0);
}

struct gfs2_inode *lgfs2_gfs_inode_read(struct gfs2_sbd *sdp, uint64_t di_addr)
{
	return __gfs_inode_get(sdp, NULL, di_addr);
}

/* ------------------------------------------------------------------------ */
/* gfs_jindex_in - read in a gfs1 jindex structure.                         */
/* ------------------------------------------------------------------------ */
void gfs_jindex_in(struct gfs_jindex *jindex, char *jbuf)
{
	struct gfs_jindex *str = (struct gfs_jindex *) jbuf;

	jindex->ji_addr = be64_to_cpu(str->ji_addr);
	jindex->ji_nsegment = be32_to_cpu(str->ji_nsegment);
	jindex->ji_pad = be32_to_cpu(str->ji_pad);
	memcpy(jindex->ji_reserved, str->ji_reserved, 64);
}

/* ------------------------------------------------------------------------ */
/* gfs_rgrp_in - Read in a resource group header                            */
/* ------------------------------------------------------------------------ */
void gfs_rgrp_in(struct gfs_rgrp *rgrp, struct gfs2_buffer_head *rbh)
{
	struct gfs_rgrp *str = (struct gfs_rgrp *)rbh->b_data;

	gfs2_meta_header_in(&rgrp->rg_header, rbh->b_data);
	rgrp->rg_flags = be32_to_cpu(str->rg_flags);
	rgrp->rg_free = be32_to_cpu(str->rg_free);
	rgrp->rg_useddi = be32_to_cpu(str->rg_useddi);
	rgrp->rg_freedi = be32_to_cpu(str->rg_freedi);
	gfs2_inum_in(&rgrp->rg_freedi_list, (char *)&str->rg_freedi_list);
	rgrp->rg_usedmeta = be32_to_cpu(str->rg_usedmeta);
	rgrp->rg_freemeta = be32_to_cpu(str->rg_freemeta);

	memcpy(rgrp->rg_reserved, str->rg_reserved, 64);
}

/* ------------------------------------------------------------------------ */
/* gfs_rgrp_out */
/* ------------------------------------------------------------------------ */
void gfs_rgrp_out(struct gfs_rgrp *rgrp, struct gfs2_buffer_head *rbh)
{
	struct gfs_rgrp *str = (struct gfs_rgrp *)rbh->b_data;

	gfs2_meta_header_out(&rgrp->rg_header, rbh->b_data);
	str->rg_flags = cpu_to_be32(rgrp->rg_flags);
	str->rg_free = cpu_to_be32(rgrp->rg_free);
	str->rg_useddi = cpu_to_be32(rgrp->rg_useddi);
	str->rg_freedi = cpu_to_be32(rgrp->rg_freedi);
	gfs2_inum_out(&rgrp->rg_freedi_list, (char *)&str->rg_freedi_list);
	str->rg_usedmeta = cpu_to_be32(rgrp->rg_usedmeta);
	str->rg_freemeta = cpu_to_be32(rgrp->rg_freemeta);

	memcpy(str->rg_reserved, rgrp->rg_reserved, 64);
	bmodified(rbh);
}
