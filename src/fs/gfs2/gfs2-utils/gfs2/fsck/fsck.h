#ifndef _FSCK_H
#define _FSCK_H

#include "libgfs2.h"
#include "osi_tree.h"

#define FSCK_HASH_SHIFT         (13)
#define FSCK_HASH_SIZE          (1 << FSCK_HASH_SHIFT)
#define FSCK_HASH_MASK          (FSCK_HASH_SIZE - 1)

#define query(fmt, args...) fsck_query(fmt, ##args)

/*
 * Exit codes used by fsck-type programs
 * Copied from e2fsck's e2fsck.h
 */
#define FSCK_OK          0      /* No errors */
#define FSCK_NONDESTRUCT 1      /* File system errors corrected */
#define FSCK_REBOOT      2      /* System should be rebooted */
#define FSCK_UNCORRECTED 4      /* File system errors left uncorrected */
#define FSCK_ERROR       8      /* Operational error */
#define FSCK_USAGE       16     /* Usage or syntax error */
#define FSCK_CANCELED    32     /* Aborted with a signal or ^C */
#define FSCK_LIBRARY     128    /* Shared library error */

#define BAD_POINTER_TOLERANCE 10 /* How many bad pointers is too many? */

struct gfs2_bmap {
	uint64_t size;
	uint64_t mapsize;
	unsigned char *map;
};

struct inode_info
{
	struct osi_node node;
	struct gfs2_inum di_num;
	uint32_t   di_nlink;    /* the number of links the inode
				 * thinks it has */
	uint32_t   counted_links; /* the number of links we've found */
};

struct dir_info
{
	struct osi_node node;
	struct gfs2_inum dinode;
	uint64_t treewalk_parent;
	struct gfs2_inum dotdot_parent;
	uint32_t di_nlink;
	uint32_t counted_links;
	uint8_t  checked:1;
};

struct dir_status {
	uint8_t dotdir:1;
	uint8_t dotdotdir:1;
	int q;
	uint32_t entry_count;
};

#define DUPFLAG_REF1_FOUND 1 /* Has the original reference been found? */
#define DUPFLAG_REF1_IS_DUPL 2 /* The original reference is also where we
				  determined there was a duplicate. */

struct duptree {
	struct osi_node node;
	int dup_flags;
	int refs;
	uint64_t block;
	osi_list_t ref_inode_list; /* list of inodes referencing a dup block */
	osi_list_t ref_invinode_list; /* list of invalid inodes referencing */
};

enum dup_ref_type {
	ref_as_data = 0, /* dinode references this block as a data block */
	ref_as_meta = 1, /* dinode references this block as a metadata block */
	ref_as_ea   = 2, /* dinode references this block as an extended attr */
	ref_is_inode= 3, /* The reference is itself a dinode.  In other words,
			    it's a dinode, not pointed to as data or
			    metadata */
	ref_types   = 4,
};

struct inode_with_dups {
	osi_list_t list;
	uint64_t block_no;
	int dup_count;
	int reftypecount[ref_types];
	uint64_t parent;
	char *name;
};

enum rgindex_trust_level { /* how far can we trust our RG index? */
	blind_faith = 0, /* We'd like to trust the rgindex. We always used to
			    before bz 179069. This should cover most cases. */
	ye_of_little_faith = 1, /* The rindex seems trustworthy but there's
				   rg damage that need to be fixed. */
	open_minded = 2, /* At least 1 RG is corrupt. Try to calculate what it
			    should be, in a perfect world where our RGs are all
			    on even boundaries. Blue sky. Chirping birds. */
	distrust = 3,  /* The world isn't perfect, our RGs are not on nice neat
			  boundaries.  The fs must have been messed with by
			  gfs2_grow or something.  Count the RGs by hand. */
	indignation = 4 /* Not only do we have corruption, but the rgrps
			   aren't on even boundaries, so this file system
			   must have been converted from gfs2_convert. */
};

struct error_block {
	uint64_t metablk; /* metadata block where error was found */
	int metaoff; /* offset in that metadata block where error found */
	uint64_t errblk; /* error block */
};

extern struct gfs2_inode *fsck_load_inode(struct gfs2_sbd *sdp, uint64_t block);
extern struct gfs2_inode *fsck_inode_get(struct gfs2_sbd *sdp,
					 struct rgrp_tree *rgd,
					 struct gfs2_buffer_head *bh);
extern void fsck_inode_put(struct gfs2_inode **ip);

extern int initialize(struct gfs2_sbd *sdp, int force_check, int preen,
		      int *all_clean);
extern void destroy(struct gfs2_sbd *sdp);
extern int pass1(struct gfs2_sbd *sdp);
extern int pass1b(struct gfs2_sbd *sdp);
extern int pass1c(struct gfs2_sbd *sdp);
extern int pass2(struct gfs2_sbd *sdp);
extern int pass3(struct gfs2_sbd *sdp);
extern int pass4(struct gfs2_sbd *sdp);
extern int pass5(struct gfs2_sbd *sdp, struct gfs2_bmap *bl);
extern int rg_repair(struct gfs2_sbd *sdp, int trust_lvl, int *rg_count,
		     int *sane);
extern int fsck_query(const char *format, ...)
	__attribute__((format(printf,1,2)));
extern struct dir_info *dirtree_find(uint64_t block);
extern void dup_delete(struct duptree *dt);
extern void dirtree_delete(struct dir_info *b);

/* FIXME: Hack to get this going for pass2 - this should be pulled out
 * of pass1 and put somewhere else... */
struct dir_info *dirtree_insert(struct gfs2_inum inum);

struct gfs2_options {
	char *device;
	unsigned int yes:1;
	unsigned int no:1;
	unsigned int query:1;
};

extern struct gfs2_options opts;
extern struct gfs2_inode *lf_dip; /* Lost and found directory inode */
extern int lf_was_created;
extern uint64_t last_fs_block, last_reported_block;
extern int64_t last_reported_fblock;
extern int skip_this_pass, fsck_abort;
extern int errors_found, errors_corrected;
extern uint64_t last_data_block;
extern uint64_t first_data_block;
extern struct osi_root dup_blocks;
extern struct osi_root dirtree;
extern struct osi_root inodetree;
extern int dups_found; /* How many duplicate references have we found? */
extern int dups_found_first; /* How many duplicates have we found the original
				reference for? */
extern struct gfs_sb *sbd1;

static inline int valid_block(struct gfs2_sbd *sdp, uint64_t blkno)
{
	return !((blkno > sdp->fssize) || (blkno <= LGFS2_SB_ADDR(sdp)) ||
	         (lgfs2_get_bitmap(sdp, blkno, NULL) < 0));
}

static inline int rgrp_contains_block(struct rgrp_tree *rgd, uint64_t blk)
{
	if (blk < rgd->ri.ri_addr)
		return 0;
	if (blk >= rgd->ri.ri_data0 + rgd->ri.ri_data)
		return 0;
	return 1;
}

static inline int valid_block_ip(struct gfs2_inode *ip, uint64_t blk)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	struct rgrp_tree *rgd = ip->i_rgd;

	if (blk > sdp->fssize)
		return 0;
	if (blk <= LGFS2_SB_ADDR(sdp))
		return 0;
	if (rgd == NULL || !rgrp_contains_block(rgd, blk)) {
		rgd = gfs2_blk2rgrpd(sdp, blk);
		if (rgd == NULL)
			return 0;
	}

	return rgrp_contains_block(rgd, blk);
}

#endif /* _FSCK_H */
