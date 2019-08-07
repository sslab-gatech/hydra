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
#include <sys/time.h>

#include "libgfs2.h"
#include "config.h"
#include "crc32c.h"

#ifdef GFS2_HAS_UUID
#include <uuid.h>
#endif

int build_master(struct gfs2_sbd *sdp)
{
	struct gfs2_inum inum;
	uint64_t bn;
	struct gfs2_buffer_head *bh = NULL;
	int err = lgfs2_dinode_alloc(sdp, 1, &bn);

	if (err != 0)
		return -1;

	inum.no_formal_ino = sdp->md.next_inum++;
	inum.no_addr = bn;

	err = init_dinode(sdp, &bh, &inum, S_IFDIR | 0755, GFS2_DIF_SYSTEM, &inum);
	if (err != 0)
		return -1;

	sdp->master_dir = lgfs2_inode_get(sdp, bh);
	if (sdp->master_dir == NULL)
		return -1;

	if (cfg_debug) {
		printf("\nMaster dir:\n");
		gfs2_dinode_print(&sdp->master_dir->i_di);
	}
	sdp->master_dir->bh_owned = 1;
	return 0;
}

/**
 * Initialise a gfs2_sb structure with sensible defaults.
 */
void lgfs2_sb_init(struct gfs2_sb *sb, unsigned bsize)
{
	memset(sb, 0, sizeof(struct gfs2_sb));
	sb->sb_header.mh_magic = GFS2_MAGIC;
	sb->sb_header.mh_type = GFS2_METATYPE_SB;
	sb->sb_header.mh_format = GFS2_FORMAT_SB;
	sb->sb_fs_format = GFS2_FORMAT_FS;
	sb->sb_multihost_format = GFS2_FORMAT_MULTI;
	sb->sb_bsize = bsize;
	sb->sb_bsize_shift = ffs(bsize) - 1;
#ifdef GFS2_HAS_UUID
	uuid_generate(sb->sb_uuid);
#endif
}

int lgfs2_sb_write(const struct gfs2_sb *sb, int fd, const unsigned bsize)
{
	int i, err = -1;
	struct iovec *iov;
	const size_t sb_addr = GFS2_SB_ADDR * GFS2_BASIC_BLOCK / bsize;
	const size_t len = sb_addr + 1;

	/* We only need 2 blocks: one for zeroing and a second for the superblock */
	char *buf = calloc(2, bsize);
	if (buf == NULL)
		return -1;

	iov = malloc(len * sizeof(*iov));
	if (iov == NULL)
		goto out_buf;

	for (i = 0; i < len; i++) {
		iov[i].iov_base = buf;
		iov[i].iov_len = bsize;
	}

	gfs2_sb_out(sb, buf + bsize);
	iov[sb_addr].iov_base = buf + bsize;

	if (pwritev(fd, iov, len, 0) < (len * bsize))
		goto out_iov;

	err = 0;
out_iov:
	free(iov);
out_buf:
	free(buf);
	return err;
}

uint32_t lgfs2_log_header_hash(char *buf)
{
	/* lh_hash only CRCs the fields in the old lh, which ends where lh_crc is now */
	const off_t v1_end = offsetof(struct gfs2_log_header, lh_hash) + 4;

	return gfs2_disk_hash(buf, v1_end);
}

uint32_t lgfs2_log_header_crc(char *buf, unsigned bsize)
{
#ifdef GFS2_HAS_LH_V2
	/* lh_crc CRCs the rest of the block starting after lh_crc */
	const off_t v1_end = offsetof(struct gfs2_log_header, lh_hash) + 4;
	const unsigned char *lb = (const unsigned char *)buf;

	return crc32c(~0, lb + v1_end + 4, bsize - v1_end - 4);
#else
	return 0;
#endif
}

/**
 * Intialise and write the data blocks for a new journal as a contiguous
 * extent. The indirect blocks pointing to these data blocks should have been
 * written separately using lgfs2_write_filemeta() and the extent should have
 * been allocated using lgfs2_file_alloc().
 * ip: The journal's inode
 * Returns 0 on success or -1 with errno set on error.
 */
int lgfs2_write_journal_data(struct gfs2_inode *ip)
{
	struct gfs2_log_header lh = {
		.lh_header.mh_magic = GFS2_MAGIC,
		.lh_header.mh_type = GFS2_METATYPE_LH,
		.lh_header.mh_format = GFS2_FORMAT_LH,
		.lh_tail = 0,
		.lh_blkno = 0,
		.lh_hash = 0,
#ifdef GFS2_HAS_LH_V2
		.lh_flags = GFS2_LOG_HEAD_UNMOUNT | GFS2_LOG_HEAD_USERSPACE,
		.lh_crc = 0,
		.lh_nsec = 0,
		.lh_sec = 0,
		.lh_jinode = ip->i_di.di_num.no_addr,
		.lh_statfs_addr = 0,
		.lh_quota_addr = 0,
		.lh_local_total = 0,
		.lh_local_free = 0,
		.lh_local_dinodes = 0,
#else
		.lh_flags = GFS2_LOG_HEAD_UNMOUNT,
#endif
	};
	struct gfs2_buffer_head *bh;
	struct gfs2_sbd *sdp = ip->i_sbd;
	unsigned blocks = (ip->i_di.di_size + sdp->bsize - 1) / sdp->bsize;
	uint64_t jext0 = ip->i_di.di_num.no_addr + ip->i_di.di_blocks - blocks;
	uint64_t seq = ((blocks) * (random() / (RAND_MAX + 1.0)));

	bh = bget(sdp, jext0);
	if (bh == NULL)
		return -1;

	crc32c_optimization_init();
	do {
		struct gfs2_log_header *buflh = (struct gfs2_log_header *)bh->b_data;

		lh.lh_sequence = seq;
		lh.lh_blkno = bh->b_blocknr - jext0;
		gfs2_log_header_out(&lh, bh->b_data);

		buflh->lh_hash = cpu_to_be32(lgfs2_log_header_hash(bh->b_data));
#ifdef GFS2_HAS_LH_V2
		buflh->lh_addr = cpu_to_be64(bh->b_blocknr);
		buflh->lh_crc = cpu_to_be32(lgfs2_log_header_crc(bh->b_data, sdp->bsize));
#endif

		if (bwrite(bh)) {
			free(bh);
			return -1;
		}

		if (++seq == blocks)
			seq = 0;

	} while (++bh->b_blocknr < jext0 + blocks);

	free(bh);
	return 0;
}

int write_journal(struct gfs2_inode *jnl, unsigned bsize, unsigned int blocks)
{
	struct gfs2_log_header lh;
	unsigned int x;
	uint64_t seq = ((blocks) * (random() / (RAND_MAX + 1.0)));
	uint32_t hash;
	unsigned int height;

	/* Build the height up so our journal blocks will be contiguous and */
	/* not broken up by indirect block pages.                           */
	height = calc_tree_height(jnl, (blocks + 1) * bsize);
	build_height(jnl, height);

	memset(&lh, 0, sizeof(struct gfs2_log_header));
	lh.lh_header.mh_magic = GFS2_MAGIC;
	lh.lh_header.mh_type = GFS2_METATYPE_LH;
	lh.lh_header.mh_format = GFS2_FORMAT_LH;
	lh.lh_flags = GFS2_LOG_HEAD_UNMOUNT;
#ifdef GFS2_HAS_LH_V2
	lh.lh_flags |= GFS2_LOG_HEAD_USERSPACE;
	lh.lh_jinode = jnl->i_di.di_num.no_addr;
#endif
	for (x = 0; x < blocks; x++) {
		struct gfs2_buffer_head *bh = get_file_buf(jnl, x, TRUE);
		if (!bh)
			return -1;
		bmodified(bh);
		brelse(bh);
	}
	crc32c_optimization_init();
	for (x = 0; x < blocks; x++) {
		struct gfs2_buffer_head *bh = get_file_buf(jnl, x, FALSE);
		if (!bh)
			return -1;

		memset(bh->b_data, 0, bsize);
		lh.lh_sequence = seq;
		lh.lh_blkno = x;
		gfs2_log_header_out(&lh, bh->b_data);
		hash = lgfs2_log_header_hash(bh->b_data);
		((struct gfs2_log_header *)bh->b_data)->lh_hash = cpu_to_be32(hash);
#ifdef GFS2_HAS_LH_V2
		((struct gfs2_log_header *)bh->b_data)->lh_addr = cpu_to_be64(bh->b_blocknr);
		hash = lgfs2_log_header_crc(bh->b_data, bsize);
		((struct gfs2_log_header *)bh->b_data)->lh_crc = cpu_to_be32(hash);
#endif
		bmodified(bh);
		brelse(bh);

		if (++seq == blocks)
			seq = 0;
	}

	return 0;
}

int build_journal(struct gfs2_sbd *sdp, int j, struct gfs2_inode *jindex)
{
	char name[256];
	int ret;

	sprintf(name, "journal%u", j);
	sdp->md.journal[j] = createi(jindex, name, S_IFREG | 0600,
				     GFS2_DIF_SYSTEM);
	if (sdp->md.journal[j] == NULL) {
		return errno;
	}
	ret = write_journal(sdp->md.journal[j], sdp->bsize,
			    sdp->jsize << 20 >> sdp->sd_sb.sb_bsize_shift);
	return ret;
}

/**
 * Write a jindex file given a list of journal inums.
 * master: Inode of the master directory
 * jnls: List of inum structures relating to previously created journals.
 * nmemb: The number of entries in the list (number of journals).
 * Returns 0 on success or non-zero on error with errno set.
 */
int lgfs2_build_jindex(struct gfs2_inode *master, struct gfs2_inum *jnls, size_t nmemb)
{
	char fname[GFS2_FNAMESIZE + 1];
	struct gfs2_inode *jindex;
	unsigned j;
	int ret;

	if (nmemb == 0 || jnls == NULL) {
		errno = EINVAL;
		return 1;
	}
	jindex = createi(master, "jindex", S_IFDIR | 0700, GFS2_DIF_SYSTEM);
	if (jindex == NULL)
		return 1;

	fname[GFS2_FNAMESIZE] = '\0';

	for (j = 0; j < nmemb; j++) {
		snprintf(fname, GFS2_FNAMESIZE, "journal%u", j);
		ret = dir_add(jindex, fname, strlen(fname), &jnls[j], IF2DT(S_IFREG | 0600));
		if (ret) {
			inode_put(&jindex);
			return 1;
		}
	}

	if (cfg_debug) {
		printf("\nJindex:\n");
		gfs2_dinode_print(&jindex->i_di);
	}

	inode_put(&jindex);
	return 0;
}

int build_jindex(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *jindex;
	unsigned int j;
	int ret;

	jindex = createi(sdp->master_dir, "jindex", S_IFDIR | 0700,
			 GFS2_DIF_SYSTEM);
	if (jindex == NULL) {
		return errno;
	}
	sdp->md.journal = malloc(sdp->md.journals *
				 sizeof(struct gfs2_inode *));
	for (j = 0; j < sdp->md.journals; j++) {
		ret = build_journal(sdp, j, jindex);
		if (ret)
			return ret;
		inode_put(&sdp->md.journal[j]);
	}
	if (cfg_debug) {
		printf("\nJindex:\n");
		gfs2_dinode_print(&jindex->i_di);
	}

	free(sdp->md.journal);
	inode_put(&jindex);
	return 0;
}

int build_inum_range(struct gfs2_inode *per_node, unsigned int j)
{
	char name[256];
	struct gfs2_inode *ip;

	sprintf(name, "inum_range%u", j);
	ip = createi(per_node, name, S_IFREG | 0600,
		     GFS2_DIF_SYSTEM | GFS2_DIF_JDATA);
	if (ip == NULL) {
		return errno;
	}
	ip->i_di.di_size = sizeof(struct gfs2_inum_range);
	gfs2_dinode_out(&ip->i_di, ip->i_bh->b_data);
	bmodified(ip->i_bh);
	if (cfg_debug) {
		printf("\nInum Range %u:\n", j);
		gfs2_dinode_print(&ip->i_di);
	}

	inode_put(&ip);
	return 0;
}

int build_statfs_change(struct gfs2_inode *per_node, unsigned int j)
{
	char name[256];
	struct gfs2_inode *ip;

	sprintf(name, "statfs_change%u", j);
	ip = createi(per_node, name, S_IFREG | 0600,
		     GFS2_DIF_SYSTEM | GFS2_DIF_JDATA);
	if (ip == NULL) {
		return errno;
	}
	ip->i_di.di_size = sizeof(struct gfs2_statfs_change);
	gfs2_dinode_out(&ip->i_di, ip->i_bh->b_data);
	bmodified(ip->i_bh);
	if (cfg_debug) {
		printf("\nStatFS Change %u:\n", j);
		gfs2_dinode_print(&ip->i_di);
	}

	inode_put(&ip);
	return 0;
}

int build_quota_change(struct gfs2_inode *per_node, unsigned int j)
{
	struct gfs2_sbd *sdp = per_node->i_sbd;
	struct gfs2_meta_header mh;
	char name[256];
	struct gfs2_inode *ip;
	unsigned int blocks = sdp->qcsize << (20 - sdp->sd_sb.sb_bsize_shift);
	unsigned int x;
	unsigned int hgt;
	struct gfs2_buffer_head *bh;

	memset(&mh, 0, sizeof(struct gfs2_meta_header));
	mh.mh_magic = GFS2_MAGIC;
	mh.mh_type = GFS2_METATYPE_QC;
	mh.mh_format = GFS2_FORMAT_QC;

	sprintf(name, "quota_change%u", j);
	ip = createi(per_node, name, S_IFREG | 0600, GFS2_DIF_SYSTEM);
	if (ip == NULL) {
		return errno;
	}

	hgt = calc_tree_height(ip, (blocks + 1) * sdp->bsize);
	build_height(ip, hgt);

	for (x = 0; x < blocks; x++) {
		bh = get_file_buf(ip, x, FALSE);
		if (!bh)
			return -1;

		memset(bh->b_data, 0, sdp->bsize);
		gfs2_meta_header_out(&mh, bh->b_data);
		bmodified(bh);
		brelse(bh);
	}

	if (cfg_debug) {
		printf("\nQuota Change %u:\n", j);
		gfs2_dinode_print(&ip->i_di);
	}

	inode_put(&ip);
	return 0;
}

int build_per_node(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *per_node;
	unsigned int j;
	int err;

	per_node = createi(sdp->master_dir, "per_node", S_IFDIR | 0700,
			   GFS2_DIF_SYSTEM);
	if (per_node == NULL) {
		return errno;
	}

	for (j = 0; j < sdp->md.journals; j++) {
		err = build_inum_range(per_node, j);
		if (err) {
			return err;
		}
		err = build_statfs_change(per_node, j);
		if (err) {
			return err;
		}
		err = build_quota_change(per_node, j);
		if (err) {
			return err;
		}
	}

	if (cfg_debug) {
		printf("\nper_node:\n");
		gfs2_dinode_print(&per_node->i_di);
	}

	inode_put(&per_node);
	return 0;
}

int build_inum(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip;

	ip = createi(sdp->master_dir, "inum", S_IFREG | 0600,
		     GFS2_DIF_SYSTEM | GFS2_DIF_JDATA);
	if (ip == NULL) {
		return errno;
	}

	if (cfg_debug) {
		printf("\nInum Inode:\n");
		gfs2_dinode_print(&ip->i_di);
	}

	inode_put(&ip);
	return 0;
}

int build_statfs(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip;

	ip = createi(sdp->master_dir, "statfs", S_IFREG | 0600,
		     GFS2_DIF_SYSTEM | GFS2_DIF_JDATA);
	if (ip == NULL) {
		return errno;
	}

	if (cfg_debug) {
		printf("\nStatFS Inode:\n");
		gfs2_dinode_print(&ip->i_di);
	}

	inode_put(&ip);
	return 0;
}

int build_rindex(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip;
	struct osi_node *n, *next = NULL;
	struct rgrp_tree *rl;
	char buf[sizeof(struct gfs2_rindex)];
	int count;

	ip = createi(sdp->master_dir, "rindex", S_IFREG | 0600,
		     GFS2_DIF_SYSTEM | GFS2_DIF_JDATA);
	if (ip == NULL) {
		return errno;
	}
	ip->i_di.di_payload_format = GFS2_FORMAT_RI;
	bmodified(ip->i_bh);

	for (n = osi_first(&sdp->rgtree); n; n = next) {
		next = osi_next(n);
		rl = (struct rgrp_tree *)n;

		gfs2_rindex_out(&rl->ri, buf);

		count = gfs2_writei(ip, buf, ip->i_di.di_size,
				    sizeof(struct gfs2_rindex));
		if (count != sizeof(struct gfs2_rindex))
			return -1;
	}
	memset(buf, 0, sizeof(struct gfs2_rindex));
	count = __gfs2_writei(ip, buf, ip->i_di.di_size,
			      sizeof(struct gfs2_rindex), 0);
	if (count != sizeof(struct gfs2_rindex))
		return -1;

	if (cfg_debug) {
		printf("\nResource Index:\n");
		gfs2_dinode_print(&ip->i_di);
	}

	inode_put(&ip);
	return 0;
}

int build_quota(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip;
	struct gfs2_quota qu;
	char buf[sizeof(struct gfs2_quota)];
	int count;

	ip = createi(sdp->master_dir, "quota", S_IFREG | 0600,
		     GFS2_DIF_SYSTEM | GFS2_DIF_JDATA);
	if (ip == NULL) {
		return errno;
	}
	ip->i_di.di_payload_format = GFS2_FORMAT_QU;
	bmodified(ip->i_bh);

	memset(&qu, 0, sizeof(struct gfs2_quota));
	qu.qu_value = 1;
	gfs2_quota_out(&qu, buf);

	count = gfs2_writei(ip, buf, ip->i_di.di_size, sizeof(struct gfs2_quota));
	if (count != sizeof(struct gfs2_quota))
		return -1;
	count = gfs2_writei(ip, buf, ip->i_di.di_size, sizeof(struct gfs2_quota));
	if (count != sizeof(struct gfs2_quota))
		return -1;

	if (cfg_debug) {
		printf("\nRoot quota:\n");
		gfs2_quota_print(&qu);
	}

	inode_put(&ip);
	return 0;
}

int build_root(struct gfs2_sbd *sdp)
{
	struct gfs2_inum inum;
	uint64_t bn;
	struct gfs2_buffer_head *bh = NULL;
	int err = lgfs2_dinode_alloc(sdp, 1, &bn);

	if (err != 0)
		return -1;

	inum.no_formal_ino = sdp->md.next_inum++;
	inum.no_addr = bn;

	err = init_dinode(sdp, &bh, &inum, S_IFDIR | 0755, 0, &inum);
	if (err != 0)
		return -1;

	sdp->md.rooti = lgfs2_inode_get(sdp, bh);
	if (sdp->md.rooti == NULL)
		return -1;

	if (cfg_debug) {
		printf("\nRoot directory:\n");
		gfs2_dinode_print(&sdp->md.rooti->i_di);
	}
	sdp->md.rooti->bh_owned = 1;
	return 0;
}

int do_init_inum(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip = sdp->md.inum;
	uint64_t buf;
	int count;

	buf = cpu_to_be64(sdp->md.next_inum);
	count = gfs2_writei(ip, &buf, 0, sizeof(uint64_t));
	if (count != sizeof(uint64_t))
		return -1;

	if (cfg_debug)
		printf("\nNext Inum: %"PRIu64"\n",
		       sdp->md.next_inum);
	return 0;
}

int do_init_statfs(struct gfs2_sbd *sdp)
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
	if (count != sizeof(struct gfs2_statfs_change))
		return -1;

	if (cfg_debug) {
		printf("\nStatfs:\n");
		gfs2_statfs_change_print(&sc);
	}
	return 0;
}

int gfs2_check_meta(struct gfs2_buffer_head *bh, int type)
{
	uint32_t check_magic = ((struct gfs2_meta_header *)(bh->b_data))->mh_magic;
	uint32_t check_type = ((struct gfs2_meta_header *)(bh->b_data))->mh_type;

	check_magic = be32_to_cpu(check_magic);
	check_type = be32_to_cpu(check_type);
	if((check_magic != GFS2_MAGIC) || (type && (check_type != type)))
		return -1;
	return 0;
}

unsigned lgfs2_bm_scan(struct rgrp_tree *rgd, unsigned idx, uint64_t *buf, uint8_t state)
{
	struct gfs2_bitmap *bi = &rgd->bits[idx];
	unsigned n = 0;
	uint32_t blk = 0;

	while(blk < (bi->bi_len * GFS2_NBBY)) {
		blk = gfs2_bitfit((uint8_t *)bi->bi_bh->b_data + bi->bi_offset,
				  bi->bi_len, blk, state);
		if (blk == BFITNOENT)
			break;
		buf[n++] = blk + (bi->bi_start * GFS2_NBBY) + rgd->ri.ri_data0;
		blk++;
	}

	return n;
}
