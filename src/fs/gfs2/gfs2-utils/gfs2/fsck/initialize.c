#include "clusterautoconfig.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <libintl.h>
#include <errno.h>
#include <time.h>

#define _(String) gettext(String)

#include <logging.h>
#include "libgfs2.h"
#include "fsck.h"
#include "util.h"
#include "fs_recovery.h"
#include "metawalk.h"
#include "inode_hash.h"

#define CLEAR_POINTER(x) \
	if (x) { \
		free(x); \
		x = NULL; \
	}
#define HIGHEST_BLOCK 0xffffffffffffffff

static int was_mounted_ro = 0;
static uint64_t possible_root = HIGHEST_BLOCK;
static struct master_dir fix_md;
static unsigned long long blks_2free = 0;
extern int sb_fixed;

/**
 * block_mounters
 *
 * Change the lock protocol so nobody can mount the fs
 *
 */
static int block_mounters(struct gfs2_sbd *sdp, int block_em)
{
	if (block_em) {
		/* verify it starts with lock_ */
		if (!strncmp(sdp->sd_sb.sb_lockproto, "lock_", 5)) {
			/* Change lock_ to fsck_ */
			memcpy(sdp->sd_sb.sb_lockproto, "fsck_", 5);
		}
		/* FIXME: Need to do other verification in the else
		 * case */
	} else {
		/* verify it starts with fsck_ */
		/* verify it starts with lock_ */
		if (!strncmp(sdp->sd_sb.sb_lockproto, "fsck_", 5)) {
			/* Change fsck_ to lock_ */
			memcpy(sdp->sd_sb.sb_lockproto, "lock_", 5);
		}
	}

	if (lgfs2_sb_write(&sdp->sd_sb, sdp->device_fd, sdp->bsize)) {
		stack;
		return -1;
	}
	return 0;
}

static void gfs2_dup_free(void)
{
	struct osi_node *n;
	struct duptree *dt;

	while ((n = osi_first(&dup_blocks))) {
		dt = (struct duptree *)n;
		dup_delete(dt);
	}
}

static void gfs2_dirtree_free(void)
{
	struct osi_node *n;
	struct dir_info *dt;

	while ((n = osi_first(&dirtree))) {
		dt = (struct dir_info *)n;
		dirtree_delete(dt);
	}
}

static void gfs2_inodetree_free(void)
{
	struct osi_node *n;
	struct inode_info *dt;

	while ((n = osi_first(&inodetree))) {
		dt = (struct inode_info *)n;
		inodetree_delete(dt);
	}
}

/*
 * empty_super_block - free all structures in the super block
 * sdp: the in-core super block
 *
 * This function frees all allocated structures within the
 * super block.  It does not free the super block itself.
 *
 * Returns: Nothing
 */
static void empty_super_block(struct gfs2_sbd *sdp)
{
	log_info( _("Freeing buffers.\n"));
	gfs2_rgrp_free(&sdp->rgtree);

	gfs2_inodetree_free();
	gfs2_dirtree_free();
	gfs2_dup_free();
}


/**
 * set_block_ranges
 * @sdp: superblock
 *
 * Uses info in rgrps and jindex to determine boundaries of the
 * file system.
 *
 * Returns: 0 on success, -1 on failure
 */
static int set_block_ranges(struct gfs2_sbd *sdp)
{
	struct osi_node *n, *next = NULL;
	struct rgrp_tree *rgd;
	struct gfs2_rindex *ri;
	char buf[sdp->sd_sb.sb_bsize];
	uint64_t rmax = 0;
	uint64_t rmin = 0;
	int error;

	log_info( _("Setting block ranges..."));

	for (n = osi_first(&sdp->rgtree); n; n = next) {
		next = osi_next(n);
		rgd = (struct rgrp_tree *)n;
		ri = &rgd->ri;
		if (ri->ri_data0 + ri->ri_data &&
		    ri->ri_data0 + ri->ri_data - 1 > rmax)
			rmax = ri->ri_data0 + ri->ri_data - 1;
		if (!rmin || ri->ri_data0 < rmin)
			rmin = ri->ri_data0;
	}

	last_fs_block = rmax;
	if (last_fs_block > 0xffffffff && sizeof(unsigned long) <= 4) {
		log_crit( _("This file system is too big for this computer to handle.\n"));
		log_crit( _("Last fs block = 0x%llx, but sizeof(unsigned long) is %zu bytes.\n"),
			 (unsigned long long)last_fs_block,
			 sizeof(unsigned long));
		goto fail;
	}

	last_data_block = rmax;
	first_data_block = rmin;

	if (fsck_lseek(sdp->device_fd, (last_fs_block * sdp->sd_sb.sb_bsize))){
		log_crit( _("Can't seek to last block in file system: %llu"
			 " (0x%llx)\n"), (unsigned long long)last_fs_block,
			 (unsigned long long)last_fs_block);
		goto fail;
	}

	memset(buf, 0, sdp->sd_sb.sb_bsize);
	error = read(sdp->device_fd, buf, sdp->sd_sb.sb_bsize);
	if (error != sdp->sd_sb.sb_bsize){
		log_crit( _("Can't read last block in file system (error %u), "
			 "last_fs_block: %llu (0x%llx)\n"), error,
			 (unsigned long long)last_fs_block,
			 (unsigned long long)last_fs_block);
		goto fail;
	}

	log_info(_("0x%llx to 0x%llx\n"), (unsigned long long)first_data_block,
		 (unsigned long long)last_data_block);
	return 0;

 fail:
	log_info( _("Error\n"));
	return -1;
}

/**
 * check_rgrp_integrity - verify a rgrp free block count against the bitmap
 */
static void check_rgrp_integrity(struct gfs2_sbd *sdp, struct rgrp_tree *rgd,
				 int *fixit, int *this_rg_fixed,
				 int *this_rg_bad, int *this_rg_cleaned)
{
	uint32_t rg_free, rg_reclaimed, rg_unlinked, rg_usedmeta, rg_useddi;
	int rgb, x, y, off, bytes_to_check, total_bytes_to_check, asked = 0;
	unsigned int state;
	struct gfs_rgrp *gfs1rg = (struct gfs_rgrp *)&rgd->rg;
	uint64_t diblock;
	struct gfs2_buffer_head *bh;

	rg_free = rg_reclaimed = rg_unlinked = rg_usedmeta = rg_useddi = 0;
	total_bytes_to_check = rgd->ri.ri_bitbytes;

	*this_rg_fixed = *this_rg_bad = *this_rg_cleaned = 0;

	diblock = rgd->ri.ri_data0;
	for (rgb = 0; rgb < rgd->ri.ri_length; rgb++){
		/* Count up the free blocks in the bitmap */
		off = (rgb) ? sizeof(struct gfs2_meta_header) :
			sizeof(struct gfs2_rgrp);
		if (total_bytes_to_check <= sdp->bsize - off)
			bytes_to_check = total_bytes_to_check;
		else
			bytes_to_check = sdp->bsize - off;
		total_bytes_to_check -= bytes_to_check;
		for (x = 0; x < bytes_to_check; x++) {
			unsigned char *byte;

			byte = (unsigned char *)&rgd->bits[rgb].bi_bh->b_data[off + x];
			if (*byte == 0x55) {
				diblock += GFS2_NBBY;
				continue;
			}
			if (*byte == 0x00) {
				diblock += GFS2_NBBY;
				rg_free += GFS2_NBBY;
				continue;
			}
			for (y = 0; y < GFS2_NBBY; y++) {
				state = (*byte >>
					 (GFS2_BIT_SIZE * y)) & GFS2_BIT_MASK;
				if (state == GFS2_BLKST_USED) {
					diblock++;
					continue;
				}
				if (state == GFS2_BLKST_DINODE) {
					if (sdp->gfs1) {
						bh = bread(sdp, diblock);
						if (!gfs2_check_meta(bh,
							GFS2_METATYPE_DI))
							rg_useddi++;
						else
							rg_usedmeta++;
						brelse(bh);
					}
					diblock++;
					continue;
				}
				if (state == GFS2_BLKST_FREE) {
					diblock++;
					rg_free++;
					continue;
				}
				/* GFS2_BLKST_UNLINKED */
				if (sdp->gfs1)
					log_info(_("Free metadata block 0x%llx"
						   " found.\n"),
						 (unsigned long long)diblock);
				else
					log_info(_("Unlinked dinode 0x%llx "
						   "found.\n"),
						 (unsigned long long)diblock);
				if (!asked) {
					char msg[256];

					asked = 1;
					sprintf(msg,
						_("Okay to reclaim free "
						  "metadata in resource group "
						  "%lld (0x%llx)? (y/n)"),
						(unsigned long long)rgd->ri.ri_addr,
						(unsigned long long)rgd->ri.ri_addr);
					if (query("%s", msg))
						*fixit = 1;
				}
				if (!(*fixit)) {
					rg_unlinked++;
					diblock++;
					continue;
				}
				*byte &= ~(GFS2_BIT_MASK <<
					   (GFS2_BIT_SIZE * y));
				bmodified(rgd->bits[rgb].bi_bh);
				rg_reclaimed++;
				rg_free++;
				rgd->rg.rg_free++;
				if (sdp->gfs1 && gfs1rg->rg_freemeta)
					gfs1rg->rg_freemeta--;
				log_info(_("Free metadata block %lld (0x%llx) "
					   "reclaimed.\n"),
					 (unsigned long long)diblock,
					 (unsigned long long)diblock);
				bh = bread(sdp, diblock);
				if (!gfs2_check_meta(bh, GFS2_METATYPE_DI)) {
					struct gfs2_inode *ip =
						fsck_inode_get(sdp, rgd, bh);
					if (ip->i_di.di_blocks > 1) {
						blks_2free +=
							ip->i_di.di_blocks - 1;
						log_info(_("%lld blocks "
							   "(total) may need "
							   "to be freed in "
							   "pass 5.\n"),
							 blks_2free);
					}
					fsck_inode_put(&ip);
				}
				brelse(bh);
				diblock++;
			}
		}
	}
	/* The unlinked blocks we reclaim shouldn't be considered errors,
	   since we're just reclaiming them as a courtesy. If we already
	   got permission to reclaim them, we adjust the rgrp counts
	   accordingly. That way, only "real" rgrp count inconsistencies
	   will be reported. */
	if (rg_reclaimed && *fixit) {
		if (sdp->gfs1)
			gfs_rgrp_out((struct gfs_rgrp *)&rgd->rg, rgd->bits[0].bi_bh);
		else
			gfs2_rgrp_out(&rgd->rg, rgd->bits[0].bi_bh->b_data);
		bmodified(rgd->bits[0].bi_bh);
		*this_rg_cleaned = 1;
		log_info( _("The rgrp at %lld (0x%llx) was cleaned of %d "
			    "free metadata blocks.\n"),
			  (unsigned long long)rgd->ri.ri_addr,
			  (unsigned long long)rgd->ri.ri_addr,
			  rg_reclaimed);
	}
	if (rgd->rg.rg_free != rg_free) {
		*this_rg_bad = 1;
		*this_rg_cleaned = 0;
		log_err( _("Error: resource group %lld (0x%llx): "
			   "free space (%d) does not match bitmap (%d)\n"),
			 (unsigned long long)rgd->ri.ri_addr,
			 (unsigned long long)rgd->ri.ri_addr,
			 rgd->rg.rg_free, rg_free);
		if (query( _("Fix the rgrp free blocks count? (y/n)"))) {
			rgd->rg.rg_free = rg_free;
			if (sdp->gfs1)
				gfs_rgrp_out((struct gfs_rgrp *)&rgd->rg, rgd->bits[0].bi_bh);
			else
				gfs2_rgrp_out(&rgd->rg, rgd->bits[0].bi_bh->b_data);
			bmodified(rgd->bits[0].bi_bh);
			*this_rg_fixed = 1;
			log_err( _("The rgrp was fixed.\n"));
		} else
			log_err( _("The rgrp was not fixed.\n"));
	}
	if (!sdp->gfs1)
		return;

	if (gfs1rg->rg_freemeta != rg_unlinked) {
		*this_rg_bad = 1;
		*this_rg_cleaned = 0;
		log_err( _("Error: resource group %lld (0x%llx): "
			   "free meta  (%d) does not match bitmap (%d)\n"),
			 (unsigned long long)rgd->ri.ri_addr,
			 (unsigned long long)rgd->ri.ri_addr,
			 gfs1rg->rg_freemeta, rg_unlinked);
		if (query( _("Fix the rgrp free meta blocks count? (y/n)"))) {
			gfs1rg->rg_freemeta = rg_unlinked;
			gfs_rgrp_out((struct gfs_rgrp *)&rgd->rg, rgd->bits[0].bi_bh);
			*this_rg_fixed = 1;
			log_err( _("The rgrp was fixed.\n"));
		} else
			log_err( _("The rgrp was not fixed.\n"));
	}
	if (gfs1rg->rg_useddi != rg_useddi) {
		*this_rg_bad = 1;
		*this_rg_cleaned = 0;
		log_err( _("Error: resource group %lld (0x%llx): used dinode "
			   "count (%d) does not match bitmap (%d)\n"),
			 (unsigned long long)rgd->ri.ri_addr,
			 (unsigned long long)rgd->ri.ri_addr,
			 gfs1rg->rg_useddi, rg_useddi);
		if (query( _("Fix the rgrp used dinode block count? (y/n)"))) {
			gfs1rg->rg_useddi = rg_useddi;
			gfs_rgrp_out((struct gfs_rgrp *)&rgd->rg,
				     rgd->bits[0].bi_bh);
			*this_rg_fixed = 1;
			log_err( _("The rgrp was fixed.\n"));
		} else
			log_err( _("The rgrp was not fixed.\n"));
	}
	if (gfs1rg->rg_usedmeta != rg_usedmeta) {
		*this_rg_bad = 1;
		*this_rg_cleaned = 0;
		log_err( _("Error: resource group %lld (0x%llx): used "
			   "metadata (%d) does not match bitmap (%d)\n"),
			 (unsigned long long)rgd->ri.ri_addr,
			 (unsigned long long)rgd->ri.ri_addr,
			 gfs1rg->rg_usedmeta, rg_usedmeta);
		if (query( _("Fix the rgrp used meta blocks count? (y/n)"))) {
			gfs1rg->rg_usedmeta = rg_usedmeta;
			gfs_rgrp_out((struct gfs_rgrp *)&rgd->rg,
				     rgd->bits[0].bi_bh);
			*this_rg_fixed = 1;
			log_err( _("The rgrp was fixed.\n"));
		} else
			log_err( _("The rgrp was not fixed.\n"));
	}
	/*
	else {
		log_debug( _("Resource group %lld (0x%llx) free space "
			     "is consistent: free: %d reclaimed: %d\n"),
			   (unsigned long long)rgd->ri.ri_addr,
			   (unsigned long long)rgd->ri.ri_addr,
			   rg_free, rg_reclaimed);
	}*/
}

/**
 * check_rgrps_integrity - verify rgrp consistency
 * Note: We consider an rgrp "cleaned" if the unlinked meta blocks are
 *       cleaned, so not quite "bad" and not quite "good" but rewritten anyway.
 *
 * Returns: 0 on success, 1 if errors were detected
 */
static void check_rgrps_integrity(struct gfs2_sbd *sdp)
{
	struct osi_node *n, *next = NULL;
	int rgs_good = 0, rgs_bad = 0, rgs_fixed = 0, rgs_cleaned = 0;
	int was_bad = 0, was_fixed = 0, was_cleaned = 0;
	struct rgrp_tree *rgd;
	int reclaim_unlinked = 0;

	log_info( _("Checking the integrity of all resource groups.\n"));
	for (n = osi_first(&sdp->rgtree); n; n = next) {
		next = osi_next(n);
		rgd = (struct rgrp_tree *)n;
		if (fsck_abort)
			return;
		check_rgrp_integrity(sdp, rgd, &reclaim_unlinked,
				     &was_fixed, &was_bad, &was_cleaned);
		if (was_fixed)
			rgs_fixed++;
		if (was_cleaned)
			rgs_cleaned++;
		else if (was_bad)
			rgs_bad++;
		else
			rgs_good++;
	}
	if (rgs_bad || rgs_cleaned) {
		log_err( _("RGs: Consistent: %d   Cleaned: %d   Inconsistent: "
			   "%d   Fixed: %d   Total: %d\n"),
			 rgs_good, rgs_cleaned, rgs_bad, rgs_fixed,
			 rgs_good + rgs_bad + rgs_cleaned);
		if (rgs_cleaned && blks_2free)
			log_err(_("%lld blocks may need to be freed in pass 5 "
				  "due to the cleaned resource groups.\n"),
				blks_2free);
	}
}

/**
 * rebuild_master - rebuild a destroyed master directory
 */
static int rebuild_master(struct gfs2_sbd *sdp)
{
	struct gfs2_inum inum;
	struct gfs2_buffer_head *bh = NULL;
	int err = 0;

	log_err(_("The system master directory seems to be destroyed.\n"));
	if (!query(_("Okay to rebuild it? (y/n)"))) {
		log_err(_("System master not rebuilt; aborting.\n"));
		return -1;
	}
	log_err(_("Trying to rebuild the master directory.\n"));
	inum.no_formal_ino = sdp->md.next_inum++;
	inum.no_addr = sdp->sd_sb.sb_master_dir.no_addr;
	err = init_dinode(sdp, &bh, &inum, S_IFDIR | 0755, GFS2_DIF_SYSTEM, &inum);
	if (err != 0)
		return -1;
	sdp->master_dir = lgfs2_inode_get(sdp, bh);
	if (sdp->master_dir == NULL) {
		log_crit(_("Error reading master: %s\n"), strerror(errno));
		return -1;
	}
	sdp->master_dir->bh_owned = 1;

	if (fix_md.jiinode) {
		inum.no_formal_ino = sdp->md.next_inum++;
		inum.no_addr = fix_md.jiinode->i_di.di_num.no_addr;
		err = dir_add(sdp->master_dir, "jindex", 6, &inum,
		              IF2DT(S_IFDIR | 0700));
		if (err) {
			log_crit(_("Error %d adding jindex directory\n"), errno);
			exit(FSCK_ERROR);
		}
		sdp->master_dir->i_di.di_nlink++;
	} else {
		err = build_jindex(sdp);
		if (err) {
			log_crit(_("Error %d building jindex\n"), err);
			exit(FSCK_ERROR);
		}
	}

	if (fix_md.pinode) {
		inum.no_formal_ino = sdp->md.next_inum++;
		inum.no_addr = fix_md.pinode->i_di.di_num.no_addr;
		err = dir_add(sdp->master_dir, "per_node", 8, &inum,
			IF2DT(S_IFDIR | 0700));
		if (err) {
			log_crit(_("Error %d adding per_node directory\n"),
				 errno);
			exit(FSCK_ERROR);
		}
		sdp->master_dir->i_di.di_nlink++;
	} else {
		err = build_per_node(sdp);
		if (err) {
			log_crit(_("Error %d building per_node directory\n"),
			         err);
			exit(FSCK_ERROR);
		}
	}

	if (fix_md.inum) {
		inum.no_formal_ino = sdp->md.next_inum++;
		inum.no_addr = fix_md.inum->i_di.di_num.no_addr;
		err = dir_add(sdp->master_dir, "inum", 4, &inum,
			IF2DT(S_IFREG | 0600));
		if (err) {
			log_crit(_("Error %d adding inum inode\n"), errno);
			exit(FSCK_ERROR);
		}
	} else {
		err = build_inum(sdp);
		if (err) {
			log_crit(_("Error %d building inum inode\n"), err);
			exit(FSCK_ERROR);
		}
		gfs2_lookupi(sdp->master_dir, "inum", 4, &sdp->md.inum);
	}

	if (fix_md.statfs) {
		inum.no_formal_ino = sdp->md.next_inum++;
		inum.no_addr = fix_md.statfs->i_di.di_num.no_addr;
		err = dir_add(sdp->master_dir, "statfs", 6, &inum,
			      IF2DT(S_IFREG | 0600));
		if (err) {
			log_crit(_("Error %d adding statfs inode\n"), errno);
			exit(FSCK_ERROR);
		}
	} else {
		err = build_statfs(sdp);
		if (err) {
			log_crit(_("Error %d building statfs inode\n"), err);
			exit(FSCK_ERROR);
		}
		gfs2_lookupi(sdp->master_dir, "statfs", 6, &sdp->md.statfs);
	}

	if (fix_md.riinode) {
		inum.no_formal_ino = sdp->md.next_inum++;
		inum.no_addr = fix_md.riinode->i_di.di_num.no_addr;
		err = dir_add(sdp->master_dir, "rindex", 6, &inum,
			IF2DT(S_IFREG | 0600));
		if (err) {
			log_crit(_("Error %d adding rindex inode\n"), errno);
			exit(FSCK_ERROR);
		}
	} else {
		err = build_rindex(sdp);
		if (err) {
			log_crit(_("Error %d building rindex inode\n"), err);
			exit(FSCK_ERROR);
		}
	}

	if (fix_md.qinode) {
		inum.no_formal_ino = sdp->md.next_inum++;
		inum.no_addr = fix_md.qinode->i_di.di_num.no_addr;
		err = dir_add(sdp->master_dir, "quota", 5, &inum,
			IF2DT(S_IFREG | 0600));
		if (err) {
			log_crit(_("Error %d adding quota inode\n"), errno);
			exit(FSCK_ERROR);
		}
	} else {
		err = build_quota(sdp);
		if (err) {
			log_crit(_("Error %d building quota inode\n"), err);
			exit(FSCK_ERROR);
		}
	}

	log_err(_("Master directory rebuilt.\n"));
	inode_put(&sdp->md.inum);
	inode_put(&sdp->md.statfs);
	inode_put(&sdp->master_dir);
	return 0;
}

/**
 * lookup_per_node - Make sure the per_node directory is read in
 *
 * This function is used to read in the per_node directory.  It is called
 * twice.  The first call tries to read in the dinode early on.  That ensures
 * that if any journals are missing, we can figure out the number of journals
 * from per_node.  However, we unfortunately can't rebuild per_node at that
 * point in time because our resource groups aren't read in yet.
 * The second time it's called is much later when we can rebuild it.
 *
 * allow_rebuild: 0 if rebuilds are not allowed
 *                1 if rebuilds are allowed
 */
static void lookup_per_node(struct gfs2_sbd *sdp, int allow_rebuild)
{
	if (sdp->md.pinode)
		return;

	gfs2_lookupi(sdp->master_dir, "per_node", 8, &sdp->md.pinode);
	if (sdp->md.pinode)
		return;
	if (!allow_rebuild) {
		log_err( _("The gfs2 system per_node directory "
			   "inode is missing, so we might not be \nable to "
			   "rebuild missing journals this run.\n"));
		return;
	}

	if (query( _("The gfs2 system per_node directory "
		     "inode is missing. Okay to rebuild it? (y/n) "))) {
		int err;

		err = build_per_node(sdp);
		if (err) {
			log_crit(_("Error %d rebuilding per_node directory\n"),
				 err);
			exit(FSCK_ERROR);
		}
	}
	gfs2_lookupi(sdp->master_dir, "per_node", 8, &sdp->md.pinode);
	if (!sdp->md.pinode) {
		log_err( _("Unable to rebuild per_node; aborting.\n"));
		exit(FSCK_ERROR);
	}
}

/**
 * fetch_rgrps - fetch the resource groups from disk, and check their integrity
 */
static int fetch_rgrps(struct gfs2_sbd *sdp)
{
	enum rgindex_trust_level trust_lvl;
	int rgcount, sane = 1;

	const char *level_desc[] = {
		_("Checking if all rgrp and rindex values are good"),
		_("Checking if rindex values may be easily repaired"),
		_("Calculating where the rgrps should be if evenly spaced"),
		_("Trying to rebuild rindex assuming evenly spaced rgrps"),
		_("Trying to rebuild rindex assuming unevenly spaced rgrps"),
	};
	const char *fail_desc[] = {
		_("Some damage was found; we need to take remedial measures"),
		_("rindex is unevenly spaced: either gfs1-style or corrupt"),
		_("rindex calculations don't match: uneven rgrp boundaries"),
		_("Too many rgrp misses: rgrps must be unevenly spaced"),
		_("Too much damage found: we cannot rebuild this rindex"),
	};
	/*******************************************************************
	 ********  Validate and read in resource group information  ********
	 *******************************************************************/
	log_notice(_("Validating resource group index.\n"));
	for (trust_lvl = blind_faith; trust_lvl <= indignation; trust_lvl++) {
		int ret = 0;

		log_notice(_("Level %d resource group check: %s.\n"), trust_lvl + 1,
			  level_desc[trust_lvl]);
		if ((rg_repair(sdp, trust_lvl, &rgcount, &sane) == 0) &&
		    ((ret = ri_update(sdp, 0, &rgcount, &sane)) == 0)) {
			log_notice(_("(level %d passed)\n"), trust_lvl + 1);
			break;
		} else {
			if (ret == -1)
				log_err( _("(level %d failed: %s)\n"),
					 trust_lvl + 1, fail_desc[trust_lvl]);
			else
				log_err( _("(level %d failed at block %lld "
					   "(0x%llx): %s)\n"), trust_lvl + 1,
					 (unsigned long long)ret,
					 (unsigned long long)ret,
					 fail_desc[trust_lvl]);
		}
		if (fsck_abort)
			break;
	}
	if (trust_lvl > indignation) {
		log_err( _("Resource group recovery impossible; I can't fix "
			   "this file system.\n"));
		return -1;
	}
	log_info( _("%u resource groups found.\n"), rgcount);

	check_rgrps_integrity(sdp);
	return 0;
}

/**
 * init_system_inodes
 *
 * Returns: 0 on success, -1 on failure
 */
static int init_system_inodes(struct gfs2_sbd *sdp)
{
	uint64_t inumbuf = 0;
	char *buf;
	struct gfs2_statfs_change sc;
	int err;

	/*******************************************************************
	 ******************  Initialize important inodes  ******************
	 *******************************************************************/

	log_info( _("Initializing special inodes...\n"));

	/* Get root dinode */
	sdp->md.rooti = lgfs2_inode_read(sdp, sdp->sd_sb.sb_root_dir.no_addr);
	if (sdp->md.rooti == NULL)
		return -1;

	/*******************************************************************
	 *****************  Initialize more system inodes  *****************
	 *******************************************************************/
	if (!sdp->gfs1) {
		/* Look for "inum" entry in master dinode */
		gfs2_lookupi(sdp->master_dir, "inum", 4, &sdp->md.inum);
		if (!sdp->md.inum) {
			if (!query( _("The gfs2 system inum inode is missing. "
				      "Okay to rebuild it? (y/n) "))) {
				log_err( _("fsck.gfs2 cannot continue without "
					   "a valid inum file; aborting.\n"));
				goto fail;
			}
			err = build_inum(sdp);
			if (err) {
				log_crit(_("Error %d rebuilding inum inode\n"),
					 err);
				exit(FSCK_ERROR);
			}
			gfs2_lookupi(sdp->master_dir, "inum", 4,
				     &sdp->md.inum);
			if (!sdp->md.inum) {
				log_crit(_("System inum inode was not rebuilt."
					   " Aborting.\n"));
				goto fail;
			}
		}
		/* Read inum entry into buffer */
		err = gfs2_readi(sdp->md.inum, &inumbuf, 0,
				 sdp->md.inum->i_di.di_size);
		if (err != sdp->md.inum->i_di.di_size) {
			log_crit(_("Error %d reading system inum inode. "
				   "Aborting.\n"), err);
			goto fail;
		}
		/* call gfs2_inum_range_in() to retrieve range */
		sdp->md.next_inum = be64_to_cpu(inumbuf);
	}

	if (sdp->gfs1) {
		/* In gfs1, the license_di is always 3 blocks after the jindex_di */
		if ((sbd1->sb_license_di.no_addr != sbd1->sb_jindex_di.no_addr + 3) ||
		    (sbd1->sb_license_di.no_formal_ino != sbd1->sb_jindex_di.no_addr + 3)) {
			if (!query( _("The gfs system statfs inode pointer is incorrect. "
				      "Okay to correct? (y/n) "))) {
				log_err( _("fsck.gfs2 cannot continue without a valid "
					   "statfs file; aborting.\n"));
				goto fail;
			}
			sbd1->sb_license_di.no_addr = sbd1->sb_license_di.no_formal_ino
				= sbd1->sb_jindex_di.no_addr + 3;
		}

		sdp->md.statfs = lgfs2_inode_read(sdp, sbd1->sb_license_di.no_addr);
		if (sdp->md.statfs == NULL) {
			log_crit(_("Error reading statfs inode: %s\n"), strerror(errno));
			goto fail;
		}
	} else
		gfs2_lookupi(sdp->master_dir, "statfs", 6, &sdp->md.statfs);
	if (!sdp->gfs1 && !sdp->md.statfs) {
		if (!query( _("The gfs2 system statfs inode is missing. "
			      "Okay to rebuild it? (y/n) "))) {
			log_err( _("fsck.gfs2 cannot continue without a valid "
				   "statfs file; aborting.\n"));
			goto fail;
		}
		err = build_statfs(sdp);
		if (err) {
			log_crit(_("Error %d rebuilding statfs inode\n"), err);
			exit(FSCK_ERROR);
		}
		gfs2_lookupi(sdp->master_dir, "statfs", 6, &sdp->md.statfs);
		if (!sdp->md.statfs) {
			log_err( _("Rebuild of statfs system file failed."));
			log_err( _("fsck.gfs2 cannot continue without "
				   "a valid statfs file; aborting.\n"));
			goto fail;
		}
		do_init_statfs(sdp);
	}
	if (sdp->md.statfs->i_di.di_size) {
		buf = malloc(sdp->md.statfs->i_di.di_size);
		if (buf) {
			err = gfs2_readi(sdp->md.statfs, buf, 0,
					 sdp->md.statfs->i_di.di_size);
			if (err != sdp->md.statfs->i_di.di_size) {
				log_crit(_("Error %d reading statfs file. "
					   "Aborting.\n"), err);
				free(buf);
				goto fail;
			}
			/* call gfs2_inum_range_in() to retrieve range */
			gfs2_statfs_change_in(&sc, buf);
			free(buf);
		}
	}

	if (sdp->gfs1) {
		/* In gfs1, the quota_di is always 2 blocks after the jindex_di */
		if ((sbd1->sb_quota_di.no_addr != sbd1->sb_jindex_di.no_addr + 2) ||
		    (sbd1->sb_quota_di.no_formal_ino != sbd1->sb_jindex_di.no_addr + 2)) {
			if (!query( _("The gfs system quota inode pointer is incorrect. "
				      " Okay to correct? (y/n) "))) {
				log_err( _("fsck.gfs2 cannot continue without a valid "
					   "quota file; aborting.\n"));
				goto fail;
			}
			sbd1->sb_quota_di.no_addr = sbd1->sb_quota_di.no_formal_ino
				= sbd1->sb_jindex_di.no_addr + 2;
		}

		sdp->md.qinode = lgfs2_inode_read(sdp, sbd1->sb_quota_di.no_addr);
		if (sdp->md.qinode == NULL) {
			log_crit(_("Error reading quota inode: %s\n"), strerror(errno));
			goto fail;
		}
	} else
		gfs2_lookupi(sdp->master_dir, "quota", 5, &sdp->md.qinode);
	if (!sdp->gfs1 && !sdp->md.qinode) {
		if (!query( _("The gfs2 system quota inode is missing. "
			      "Okay to rebuild it? (y/n) "))) {
			log_crit(_("System quota inode was not "
				   "rebuilt.  Aborting.\n"));
			goto fail;
		}
		err = build_quota(sdp);
		if (err) {
			log_crit(_("Error %d rebuilding quota inode\n"), err);
			exit(FSCK_ERROR);
		}
		gfs2_lookupi(sdp->master_dir, "quota", 5, &sdp->md.qinode);
		if (!sdp->md.qinode) {
			log_crit(_("Unable to rebuild system quota file "
				   "inode.  Aborting.\n"));
			goto fail;
		}
	}

	/* Try to lookup the per_node inode.  If it was missing, it is now
	   safe to rebuild it. */
	if (!sdp->gfs1)
		lookup_per_node(sdp, 1);

	/*******************************************************************
	 *******  Now, set boundary fields in the super block  *************
	 *******************************************************************/
	if (set_block_ranges(sdp)){
		log_err( _("Unable to determine the boundaries of the"
			" file system.\n"));
		goto fail;
	}

	return 0;
 fail:
	empty_super_block(sdp);

	return -1;
}

/**
 * is_journal_copy - Is this a "real" dinode or a copy inside a journal?
 * A real dinode will be located at the block number in its no_addr.
 * A journal-copy will be at a different block (inside the journal).
 */
static int is_journal_copy(struct gfs2_inode *ip, struct gfs2_buffer_head *bh)
{
	if (ip->i_di.di_num.no_addr == bh->b_blocknr)
		return 0;
	return 1; /* journal copy */
}

/**
 * peruse_system_dinode - process a system dinode
 *
 * This function looks at a system dinode and tries to figure out which
 * dinode it is: statfs, inum, per_node, master, etc.  Some of them we
 * can deduce from the contents.  For example, di_size will be a multiple
 * of 96 for the rindex.  di_size will be 8 for inum, 24 for statfs, etc.
 * the per_node directory will have a ".." entry that will lead us to
 * the master dinode if it's been destroyed.
 */
static void peruse_system_dinode(struct gfs2_sbd *sdp, struct gfs2_dinode *di,
				 struct gfs2_buffer_head *bh)
{
	struct gfs2_inode *ip, *child_ip;
	struct gfs2_inum inum;
	int error;

	if (di->di_num.no_formal_ino == 2) {
		if (sdp->sd_sb.sb_master_dir.no_addr)
			return;
		log_warn(_("Found system master directory at: 0x%llx.\n"),
			 di->di_num.no_addr);
		sdp->sd_sb.sb_master_dir.no_addr = di->di_num.no_addr;
		return;
	}
	ip = lgfs2_inode_read(sdp, di->di_num.no_addr);
	if (ip == NULL) {
		log_crit(_("Error reading inode: %s\n"), strerror(errno));
		return;
	}
	if ((!sdp->gfs1 && di->di_num.no_formal_ino == 3) ||
	    (sdp->gfs1 && (di->di_flags & GFS2_DIF_JDATA) &&
	     (di->di_size % sizeof(struct gfs_jindex) == 0))) {
		if (fix_md.jiinode || is_journal_copy(ip, bh))
			goto out_discard_ip;
		log_warn(_("Found system jindex file at: 0x%llx\n"),
			 di->di_num.no_addr);
		fix_md.jiinode = ip;
	} else if (!sdp->gfs1 && is_dir(di, sdp->gfs1)) {
		/* Check for a jindex dir entry. Only one system dir has a
		   jindex: master */
		gfs2_lookupi(ip, "jindex", 6, &child_ip);
		if (child_ip) {
			if (fix_md.jiinode || is_journal_copy(ip, bh)) {
				inode_put(&child_ip);
				goto out_discard_ip;
			}
			fix_md.jiinode = child_ip;
			sdp->sd_sb.sb_master_dir.no_addr = di->di_num.no_addr;
			log_warn(_("Found system master directory at: "
				   "0x%llx\n"), di->di_num.no_addr);
			return;
		}

		/* Check for a statfs_change0 dir entry. Only one system dir
		   has a statfs_change: per_node, and its .. will be master. */
		gfs2_lookupi(ip, "statfs_change0", 14, &child_ip);
		if (child_ip) {
			inode_put(&child_ip);
			if (fix_md.pinode || is_journal_copy(ip, bh))
				goto out_discard_ip;
			log_warn(_("Found system per_node directory at: "
				   "0x%llx\n"), ip->i_di.di_num.no_addr);
			fix_md.pinode = ip;
			error = dir_search(ip, "..", 2, NULL, &inum);
			if (!error && inum.no_addr) {
				sdp->sd_sb.sb_master_dir.no_addr =
					inum.no_addr;
				log_warn(_("From per_node\'s \'..\' I "
					   "backtracked the master directory "
					   "to: 0x%llx\n"), inum.no_addr);
			}
			return;
		}
		log_debug(_("Unknown system directory at block 0x%llx\n"),
			  di->di_num.no_addr);
		goto out_discard_ip;
	} else if (!sdp->gfs1 && di->di_size == 8) {
		if (fix_md.inum || is_journal_copy(ip, bh))
			goto out_discard_ip;
		fix_md.inum = ip;
		log_warn(_("Found system inum file at: 0x%llx\n"),
			 di->di_num.no_addr);
	} else if (di->di_size == 24) {
		if (fix_md.statfs || is_journal_copy(ip, bh))
			goto out_discard_ip;
		fix_md.statfs = ip;
		log_warn(_("Found system statfs file at: 0x%llx\n"),
			 di->di_num.no_addr);
	} else if ((di->di_size % 96) == 0) {
		if (fix_md.riinode || is_journal_copy(ip, bh))
			goto out_discard_ip;
		fix_md.riinode = ip;
		log_warn(_("Found system rindex file at: 0x%llx\n"),
			 di->di_num.no_addr);
	} else if (!fix_md.qinode && di->di_size >= 176 &&
		   di->di_num.no_formal_ino >= 12 &&
		   di->di_num.no_formal_ino <= 100) {
		if (is_journal_copy(ip, bh))
			goto out_discard_ip;
		fix_md.qinode = ip;
		log_warn(_("Found system quota file at: 0x%llx\n"),
			 di->di_num.no_addr);
	} else {
out_discard_ip:
		inode_put(&ip);
	}
}

/**
 * peruse_user_dinode - process a user dinode trying to find the root directory
 *
 */
static void peruse_user_dinode(struct gfs2_sbd *sdp, struct gfs2_dinode *di,
			       struct gfs2_buffer_head *bh)
{
	struct gfs2_inode *ip, *parent_ip;
	struct gfs2_inum inum;
	int error;

	if (sdp->sd_sb.sb_root_dir.no_addr) /* if we know the root dinode */
		return;             /* we don't need to find the root */
	if (!is_dir(di, sdp->gfs1))  /* if this isn't a directory */
		return;             /* it can't lead us to the root anyway */

	if (di->di_num.no_formal_ino == 1) {
		struct gfs2_buffer_head *root_bh;

		if (di->di_num.no_addr == bh->b_blocknr) {
			log_warn(_("Found the root directory at: 0x%llx.\n"),
				 di->di_num.no_addr);
			sdp->sd_sb.sb_root_dir.no_addr = di->di_num.no_addr;
			return;
		}
		log_warn(_("The root dinode should be at block 0x%llx but it "
			   "seems to be destroyed.\n"),
			 (unsigned long long)di->di_num.no_addr);
		log_warn(_("Found a copy of the root directory in a journal "
			   "at block: 0x%llx.\n"),
			 (unsigned long long)bh->b_blocknr);
		if (!query(_("Do you want to replace the root dinode from the "
			     "copy? (y/n)"))) {
			log_err(_("Damaged root dinode not fixed.\n"));
			return;
		}
		root_bh = bread(sdp, di->di_num.no_addr);
		memcpy(root_bh->b_data, bh->b_data, sdp->bsize);
		bmodified(root_bh);
		brelse(root_bh);
		log_warn(_("Root directory copied from the journal.\n"));
		return;
	}
	ip = lgfs2_inode_read(sdp, di->di_num.no_addr);
	if (ip == NULL) {
		log_crit(_("Error reading inode: %s\n"), strerror(errno));
		return;
	}
	while (ip) {
		gfs2_lookupi(ip, "..", 2, &parent_ip);
		if (parent_ip && parent_ip->i_di.di_num.no_addr ==
		    ip->i_di.di_num.no_addr) {
			log_warn(_("Found the root directory at: 0x%llx\n"),
				 ip->i_di.di_num.no_addr);
			sdp->sd_sb.sb_root_dir.no_addr =
				ip->i_di.di_num.no_addr;
			inode_put(&parent_ip);
			inode_put(&ip);
			return;
		}
		if (!parent_ip)
			break;
		inode_put(&ip);
		ip = parent_ip;
	}
	error = dir_search(ip, "..", 2, NULL, &inum);
	if (!error && inum.no_addr && inum.no_addr < possible_root) {
			possible_root = inum.no_addr;
			log_debug(_("Found a possible root at: 0x%llx\n"),
				  (unsigned long long)possible_root);
	}
	inode_put(&ip);
}

/**
 * find_rgs_for_bsize - check a range of blocks for rgrps to determine bsize.
 * Assumes: device is open.
 */
static int find_rgs_for_bsize(struct gfs2_sbd *sdp, uint64_t startblock,
			      uint32_t *known_bsize)
{
	uint64_t blk, max_rg_size, rb_addr;
	struct gfs2_buffer_head *bh, *rb_bh;
	uint32_t bsize, bsize2;
	uint32_t chk;
	char *p;
	int found_rg;
	struct gfs2_meta_header mh;

	sdp->bsize = GFS2_DEFAULT_BSIZE;
	max_rg_size = 524288;
	/* Max RG size is 2GB. Max block size is 4K. 2G / 4K blks = 524288,
	   So this is traversing 2GB in 4K block increments. */
	for (blk = startblock; blk < startblock + max_rg_size; blk++) {
		bh = bread(sdp, blk);
		found_rg = 0;
		for (bsize = 0; bsize < GFS2_DEFAULT_BSIZE;
		     bsize += GFS2_BASIC_BLOCK) {
			p = bh->b_data + bsize;
			chk = ((struct gfs2_meta_header *)p)->mh_magic;
			if (be32_to_cpu(chk) != GFS2_MAGIC)
				continue;
			chk = ((struct gfs2_meta_header *)p)->mh_type;
			if (be32_to_cpu(chk) == GFS2_METATYPE_RG) {
				found_rg = 1;
				break;
			}
		}
		if (!found_rg)
			continue;
		/* Try all the block sizes in 512 byte multiples */
		for (bsize2 = GFS2_BASIC_BLOCK; bsize2 <= GFS2_DEFAULT_BSIZE;
		     bsize2 += GFS2_BASIC_BLOCK) {
			rb_addr = (bh->b_blocknr *
				   (GFS2_DEFAULT_BSIZE / bsize2)) +
				(bsize / bsize2) + 1;
			sdp->bsize = bsize2; /* temporarily */
			rb_bh = bread(sdp, rb_addr);
			gfs2_meta_header_in(&mh, rb_bh->b_data);
			brelse(rb_bh);
			if (mh.mh_magic == GFS2_MAGIC &&
			    mh.mh_type == GFS2_METATYPE_RB) {
				log_debug(_("boff:%d bsize2:%d rg:0x%llx, "
					    "rb:0x%llx\n"), bsize, bsize2,
					  (unsigned long long)blk,
					  (unsigned long long)rb_addr);
				*known_bsize = bsize2;
				break;
			}
		}
		brelse(bh);
		if (!(*known_bsize)) {
			sdp->bsize = GFS2_DEFAULT_BSIZE;
			continue;
		}

		sdp->bsize = *known_bsize;
		log_warn(_("Block size determined to be: %d\n"), *known_bsize);
		return 0;
	}
	return 0;
}

/**
 * peruse_metadata - check a range of blocks for metadata
 * Assumes: device is open.
 */
static int peruse_metadata(struct gfs2_sbd *sdp, uint64_t startblock)
{
	uint64_t blk, max_rg_size;
	struct gfs2_buffer_head *bh;
	struct gfs2_dinode di;

	max_rg_size = 2147483648ull / sdp->bsize;
	/* Max RG size is 2GB. 2G / bsize. */
	for (blk = startblock; blk < startblock + max_rg_size; blk++) {
		bh = bread(sdp, blk);
		if (gfs2_check_meta(bh, GFS2_METATYPE_DI)) {
			brelse(bh);
			continue;
		}
		gfs2_dinode_in(&di, bh->b_data);
		if (di.di_flags & GFS2_DIF_SYSTEM)
			peruse_system_dinode(sdp, &di, bh);
		else
			peruse_user_dinode(sdp, &di, bh);
		brelse(bh);
	}
	return 0;
}

/**
 * sb_repair - repair a damaged superblock
 * Assumes: device is open.
 *          The biggest RG size is 2GB
 */
static int sb_repair(struct gfs2_sbd *sdp)
{
	uint64_t half;
	uint32_t known_bsize = 0;
	int error = 0;

	memset(&fix_md, 0, sizeof(fix_md));
	/* Step 1 - First we need to determine the correct block size. */
	sdp->bsize = GFS2_DEFAULT_BSIZE;
	log_warn(_("Gathering information to repair the gfs2 superblock.  "
		   "This may take some time.\n"));
	error = find_rgs_for_bsize(sdp, (GFS2_SB_ADDR * GFS2_BASIC_BLOCK) /
				   GFS2_DEFAULT_BSIZE, &known_bsize);
	if (error)
		return error;
	if (!known_bsize) {
		log_warn(_("Block size not apparent; checking elsewhere.\n"));
		/* First, figure out the device size.  We need that so we can
		   find a suitable start point to determine what's what. */
		half = sdp->dinfo.size / 2; /* in bytes */
		half /= sdp->bsize;
		/* Start looking halfway through the device for gfs2
		   structures.  If there aren't any at all, forget it. */
		error = find_rgs_for_bsize(sdp, half, &known_bsize);
		if (error)
			return error;
	}
	if (!known_bsize) {
		log_err(_("Unable to determine the block size; this "
			  "does not look like a gfs2 file system.\n"));
		return -1;
	}
	/* Step 2 - look for the sytem dinodes */
	error = peruse_metadata(sdp, (GFS2_SB_ADDR * GFS2_BASIC_BLOCK) /
				GFS2_DEFAULT_BSIZE);
	if (error)
		return error;
	if (!sdp->sd_sb.sb_master_dir.no_addr) {
		log_err(_("Unable to locate the system master  directory.\n"));
		return -1;
	}
	if (!sdp->sd_sb.sb_root_dir.no_addr) {
		struct gfs2_inum inum;

		log_err(_("Unable to locate the root directory.\n"));
		if (possible_root == HIGHEST_BLOCK) {
			/* Take advantage of the fact that mkfs.gfs2
			   creates master immediately after root. */
			log_err(_("Can't find any dinodes that might "
				  "be the root; using master - 1.\n"));
			possible_root = sdp->sd_sb.sb_master_dir.no_addr - 1;
		}
		log_err(_("Found a possible root at: 0x%llx\n"),
			(unsigned long long)possible_root);
		sdp->sd_sb.sb_root_dir.no_addr = possible_root;
		sdp->md.rooti = lgfs2_inode_read(sdp, possible_root);
		if (!sdp->md.rooti ||
		    sdp->md.rooti->i_di.di_header.mh_magic != GFS2_MAGIC) {
			struct gfs2_buffer_head *bh = NULL;

			log_err(_("The root dinode block is destroyed.\n"));
			log_err(_("At this point I recommend "
				  "reinitializing it.\n"
				  "Hopefully everything will later "
				  "be put into lost+found.\n"));
			if (!query(_("Okay to reinitialize the root "
				     "dinode? (y/n)"))) {
				log_err(_("The root dinode was not "
					  "reinitialized; aborting.\n"));
				return -1;
			}
			inum.no_formal_ino = 1;
			inum.no_addr = possible_root;
			error = init_dinode(sdp, &bh, &inum, S_IFDIR | 0755, 0, &inum);
			if (error != 0)
				return -1;
			brelse(bh);
		}
	}
	/* Step 3 - Rebuild the lock protocol and file system table name */
	if (query(_("Okay to fix the GFS2 superblock? (y/n)"))) {
		struct gfs2_sb sb;
		log_info(_("Found system master directory at: 0x%llx\n"),
			 sdp->sd_sb.sb_master_dir.no_addr);
		sdp->master_dir = lgfs2_inode_read(sdp,
					     sdp->sd_sb.sb_master_dir.no_addr);
		if (sdp->master_dir == NULL) {
			log_crit(_("Error reading master inode: %s\n"), strerror(errno));
			return -1;
		}
		sdp->master_dir->i_di.di_num.no_addr =
			sdp->sd_sb.sb_master_dir.no_addr;
		log_info(_("Found the root directory at: 0x%llx\n"),
			 sdp->sd_sb.sb_root_dir.no_addr);
		sdp->md.rooti = lgfs2_inode_read(sdp,
					   sdp->sd_sb.sb_root_dir.no_addr);
		if (sdp->md.rooti == NULL) {
			log_crit(_("Error reading root inode: %s\n"), strerror(errno));
			return -1;
		}
		lgfs2_sb_init(&sb, sdp->bsize);
		strcpy(sb.sb_lockproto, GFS2_DEFAULT_LOCKPROTO);
		strcpy(sb.sb_locktable, "unknown");
		sb.sb_master_dir = sdp->master_dir->i_di.di_num;
		sb.sb_root_dir = sdp->md.rooti->i_di.di_num;
		lgfs2_sb_write(&sb, sdp->device_fd, sdp->bsize);
		inode_put(&sdp->md.rooti);
		inode_put(&sdp->master_dir);
		sb_fixed = 1;
	} else {
		log_crit(_("GFS2 superblock not fixed; fsck cannot proceed "
			   "without a valid superblock.\n"));
		return -1;
	}
	return 0;
}

/**
 * fill_super_block
 * @sdp:
 *
 * Returns: 0 on success, -1 on failure
 */
static int fill_super_block(struct gfs2_sbd *sdp)
{
	int ret;

	sync();

	/********************************************************************
	 ***************** First, initialize all lists **********************
	 ********************************************************************/
	log_info( _("Initializing lists...\n"));
	sdp->rgtree.osi_node = NULL;

	/********************************************************************
	 ************  next, read in on-disk SB and set constants  **********
	 ********************************************************************/
	sdp->sd_sb.sb_bsize = GFS2_DEFAULT_BSIZE;
	sdp->bsize = sdp->sd_sb.sb_bsize;

	if (sizeof(struct gfs2_sb) > sdp->sd_sb.sb_bsize){
		log_crit( _("GFS superblock is larger than the blocksize!\n"));
		log_debug("sizeof(struct gfs2_sb) > sdp->sd_sb.sb_bsize\n");
		return -1;
	}

	if (compute_constants(sdp)) {
		log_crit("%s\n", _("Failed to compute file system constants"));
		exit(FSCK_ERROR);
	}
	ret = read_sb(sdp);
	if (ret < 0) {
		if (sb_repair(sdp) != 0)
			return -1; /* unrepairable, so exit */
		/* Now that we've tried to repair it, re-read it. */
		ret = read_sb(sdp);
		if (ret < 0)
			return -1;
	}
	if (sdp->gfs1)
		sbd1 = (struct gfs_sb *)&sdp->sd_sb;
	return 0;
}

static void gfs_log_header_out(struct gfs_log_header *head, char *buf)
{
        struct gfs_log_header *str = (struct gfs_log_header *) buf;

	str->lh_header.mh_magic = cpu_to_be32(head->lh_header.mh_magic);
	str->lh_header.mh_type = cpu_to_be32(head->lh_header.mh_type);
	str->lh_header.mh_format = cpu_to_be32(head->lh_header.mh_format);
	str->lh_header.__pad0 = cpu_to_be32(head->lh_header.__pad0);

	str->lh_flags = cpu_to_be32(head->lh_flags);
	str->lh_pad = cpu_to_be32(head->lh_pad);
	str->lh_first = cpu_to_be64(head->lh_first);
	str->lh_sequence = cpu_to_be64(head->lh_sequence);
	str->lh_tail = cpu_to_be64(head->lh_tail);
	str->lh_last_dump = cpu_to_be64(head->lh_last_dump);
}

/*
 * reconstruct_single_journal - write a fresh GFS1 journal
 * @sdp: superblock
 * @jnum: journal number
 *
 * This function will write a fresh journal over the top of
 * the previous journal.  All journal information is lost.  This
 * process is basically stolen from write_journals() in the mkfs code.
 *
 * Returns: -1 on error, 0 otherwise
 */
static int reconstruct_single_journal(struct gfs2_sbd *sdp, int jnum,
				      uint32_t ji_nsegment)
{
	struct gfs_log_header lh;
	uint32_t seg, sequence;
	struct gfs2_buffer_head *bh;

	srandom(time(NULL));
	sequence = ji_nsegment / (RAND_MAX + 1.0) * random();

	log_info(_("Clearing journal %d\n"), jnum);

	for (seg = 0; seg < ji_nsegment; seg++){
		memset(&lh, 0, sizeof(struct gfs_log_header));

		lh.lh_header.mh_magic = GFS2_MAGIC;
		lh.lh_header.mh_type = GFS2_METATYPE_LH;
		lh.lh_header.mh_format = GFS2_FORMAT_LH;
		lh.lh_header.__pad0 = 0x101674; /* mh_generation */
		lh.lh_flags = GFS2_LOG_HEAD_UNMOUNT;
		lh.lh_first = sdp->md.journal[jnum]->i_di.di_num.no_addr +
			(seg * sbd1->sb_seg_size);
		lh.lh_sequence = sequence;

		bh = bget(sdp, lh.lh_first * sdp->bsize);
		memset(bh->b_data, 0, sdp->bsize);
		gfs_log_header_out(&lh, bh->b_data);
		gfs_log_header_out(&lh, bh->b_data + GFS2_BASIC_BLOCK -
				   sizeof(struct gfs_log_header));
		brelse(bh);

		if (++sequence == ji_nsegment)
			sequence = 0;
	}
	return 0;
}

static int reset_journal_seg_size(unsigned int jsize, unsigned int nsegs,
					     unsigned int bsize)
{
	unsigned int seg_size = jsize / (nsegs * bsize);
	if (!seg_size)
		seg_size = 16; /* The default with 128MB journal and 4K bsize */
	if (seg_size != sbd1->sb_seg_size) {
		sbd1->sb_seg_size = seg_size;
		if (!query(_("Computed correct journal segment size to %u."
			     " Reset it? (y/n) "), seg_size)) {
			log_crit(_("Error: Cannot proceed without a valid journal"
				   " segment size value.\n"));
			return -1;
		}
		log_err(_("Resetting journal segment size to %u\n"), sbd1->sb_seg_size);
	}
	return 0;
}

static int correct_journal_seg_size(struct gfs2_sbd *sdp)
{
	int count;
	struct gfs_jindex ji_0, ji_1;
	char buf[sizeof(struct gfs_jindex)];
	unsigned int jsize = GFS2_DEFAULT_JSIZE * 1024 * 1024;

	count = gfs2_readi(sdp->md.jiinode, buf, 0, sizeof(struct gfs_jindex));
	if (count != sizeof(struct gfs_jindex)) {
		log_crit(_("Error %d reading system journal index inode. "
			   "Aborting\n"), count);
		return -1;
	}
	gfs_jindex_in(&ji_0, buf);

	if (sdp->md.journals == 1) {
		if (sbd1->sb_seg_size == 0) {
			if (!query(_("The gfs2 journal segment size is 0 and a"
				     " correct value cannot be determined in a"
				     " single-journal filesystem.\n"
				     "Continue with default? (y/n) "))) {
				log_crit(_("Error: Cannot proceed without a valid"
					   " sb_seg_size value.\n"));
				return -1;
			}
			goto out;
		}
		/* Don't mess with sb_seg_size because we don't know what
		 * it needs to be
		 */
		return 0;
	}

	count = gfs2_readi(sdp->md.jiinode, buf, sizeof(struct gfs_jindex),
			   sizeof(struct gfs_jindex));
	if (count != sizeof(struct gfs_jindex)) {
		log_crit(_("Error %d reading system journal index inode. "
			   "Aborting\n"), count);
		return -1;
	}
	gfs_jindex_in(&ji_1, buf);

	jsize = (ji_1.ji_addr - ji_0.ji_addr) * sbd1->sb_bsize;
out:
	return reset_journal_seg_size(jsize, ji_0.ji_nsegment, sbd1->sb_bsize);
}

/*
 * reconstruct_journals - write fresh journals for GFS1 only
 * sdp: the super block
 *
 * Returns: 0 on success, -1 on failure
 */
static int reconstruct_journals(struct gfs2_sbd *sdp)
{
	int i, count;
	struct gfs_jindex ji;
	char buf[sizeof(struct gfs_jindex)];

	/* Ensure that sb_seg_size is valid */
	if (correct_journal_seg_size(sdp)) {
		log_crit(_("Failed to set correct journal segment size. Cannot continue\n"));
		return -1;
	}

	log_err(_("Clearing GFS journals (this may take a while)\n"));
	for (i = 0; i < sdp->md.journals; i++) {
		count = gfs2_readi(sdp->md.jiinode, buf,
				   i * sizeof(struct gfs_jindex),
				   sizeof(struct gfs_jindex));
		if (count != sizeof(struct gfs_jindex))
			return 0;
		gfs_jindex_in(&ji, buf);
		if ((i % 2) == 0)
			log_err(".");
		if (reconstruct_single_journal(sdp, i, ji.ji_nsegment))
			return -1;
	}
	log_err(_("\nJournals cleared.\n"));
	return 0;
}

/**
 * init_rindex - read in the rindex file
 */
static int init_rindex(struct gfs2_sbd *sdp)
{
	int err;

	if (sdp->gfs1)
		sdp->md.riinode = lgfs2_inode_read(sdp, sbd1->sb_rindex_di.no_addr);
	else
		gfs2_lookupi(sdp->master_dir, "rindex", 6, &sdp->md.riinode);

	if (sdp->md.riinode)
		return 0;

	if (!query( _("The gfs2 system rindex inode is missing. "
		      "Okay to rebuild it? (y/n) "))) {
		log_crit(_("Error: Cannot proceed without a valid rindex.\n"));
		return -1;
	}
	if ((err = build_rindex(sdp))) {
		log_crit(_("Error %d rebuilding rindex\n"), err);
		return -1;
	}
	return 0;
}

/**
 * initialize - initialize superblock pointer
 *
 */
int initialize(struct gfs2_sbd *sdp, int force_check, int preen,
	       int *all_clean)
{
	int clean_journals = 0, open_flag;

	*all_clean = 0;

	if (opts.no)
		open_flag = O_RDONLY;
	else
		open_flag = O_RDWR | O_EXCL;

	sdp->device_fd = open(opts.device, open_flag);
	if (sdp->device_fd < 0) {
		struct mntent *mnt;
		if (open_flag == O_RDONLY || errno != EBUSY) {
			log_crit( _("Unable to open device: %s\n"),
				  opts.device);
			return FSCK_USAGE;
		}
		/* We can't open it EXCL.  It may be already open rw (in which
		   case we want to deny them access) or it may be mounted as
		   the root file system at boot time (in which case we need to
		   allow it.)
		   If the device is busy, but not because it's mounted, fail.
		   This protects against cases where the file system is LVM
		   and perhaps mounted on a different node.
		   Try opening without O_EXCL. */
		sdp->device_fd = lgfs2_open_mnt_dev(opts.device, O_RDWR, &mnt);
		if (sdp->device_fd < 0)
			goto mount_fail;
		/* If the device is mounted, but not mounted RO, fail.  This
		   protects them against cases where the file system is
		   mounted RW, but still allows us to check our own root
		   file system. */
		if (!hasmntopt(mnt, MNTOPT_RO))
			goto close_fail;
		/* The device is mounted RO, so it's likely our own root
		   file system.  We can only do so much to protect the users
		   from themselves. */
		was_mounted_ro = 1;
	}

	if (lgfs2_get_dev_info(sdp->device_fd, &sdp->dinfo)) {
		perror(opts.device);
		return FSCK_ERROR;
	}

	/* read in sb from disk */
	if (fill_super_block(sdp))
		return FSCK_ERROR;

	/* Change lock protocol to be fsck_* instead of lock_* */
	if (!opts.no && preen_is_safe(sdp, preen, force_check)) {
		if (block_mounters(sdp, 1)) {
			log_err( _("Unable to block other mounters\n"));
			return FSCK_USAGE;
		}
	}

	/* Get master dinode */
	if (sdp->gfs1)
		sdp->master_dir = NULL;
	else
		sdp->master_dir = lgfs2_inode_read(sdp,
					     sdp->sd_sb.sb_master_dir.no_addr);
	if (!sdp->gfs1 &&
	    (sdp->master_dir->i_di.di_header.mh_magic != GFS2_MAGIC ||
	     sdp->master_dir->i_di.di_header.mh_type != GFS2_METATYPE_DI ||
	     !sdp->master_dir->i_di.di_size)) {
		inode_put(&sdp->master_dir);
		rebuild_master(sdp);
		sdp->master_dir = lgfs2_inode_read(sdp,
					     sdp->sd_sb.sb_master_dir.no_addr);
		if (sdp->master_dir == NULL) {
			log_crit(_("Error reading master directory: %s\n"), strerror(errno));
			return FSCK_ERROR;
		}
	}

	/* Look up the "per_node" inode.  If there are journals missing, we
	   need to figure out what's missing from per_node. And we need all
	   our journals to be there before we can replay them. */
	if (!sdp->gfs1)
		lookup_per_node(sdp, 0);

	/* We need rindex first in case jindex is missing and needs to read
	   in the rgrps before rebuilding it. However, note that if the rindex
	   is damaged, we need the journals to repair it. That's because the
	   journals likely contain rgrps and bitmaps, which we need to ignore
	   when we're trying to find the rgrps. */
	if (init_rindex(sdp))
		return FSCK_ERROR;

	if (fetch_rgrps(sdp))
		return FSCK_ERROR;

	/* We need to read in jindex in order to replay the journals. If
	   there's an error, we may proceed and let init_system_inodes
	   try to rebuild it. */
	if (init_jindex(sdp, 1) == 0) {
		/* If GFS, rebuild the journals. If GFS2, replay them. We don't
		   have the smarts to replay GFS1 journals (neither did
		   gfs_fsck). */
		if (sdp->gfs1) {
			if (reconstruct_journals(sdp))
				return FSCK_ERROR;
		} else if (replay_journals(sdp, preen, force_check,
					   &clean_journals)) {
			if (!opts.no && preen_is_safe(sdp, preen, force_check))
				block_mounters(sdp, 0);
			stack;
			return FSCK_ERROR;
		}
		if (sdp->md.journals == clean_journals)
			*all_clean = 1;
		else if (force_check || !preen)
			log_notice( _("\nJournal recovery complete.\n"));

		if (!force_check && *all_clean && preen)
			return FSCK_OK;
	}

	if (init_system_inodes(sdp))
		return FSCK_ERROR;

	return FSCK_OK;

close_fail:
	close(sdp->device_fd);
mount_fail:
	log_crit( _("Device %s is busy.\n"), opts.device);
	return FSCK_USAGE;
}

void destroy(struct gfs2_sbd *sdp)
{
	if (!opts.no) {
		if (block_mounters(sdp, 0)) {
			log_warn( _("Unable to unblock other mounters - manual intervention required\n"));
			log_warn( _("Use 'gfs2_tool sb <device> proto' to fix\n"));
		}
		log_info( _("Syncing the device.\n"));
		fsync(sdp->device_fd);
	}
	empty_super_block(sdp);
	close(sdp->device_fd);
	if (was_mounted_ro && errors_corrected) {
		sdp->device_fd = open("/proc/sys/vm/drop_caches", O_WRONLY);
		if (sdp->device_fd >= 0) {
			if (write(sdp->device_fd, "2", 1) == 2) {
				close(sdp->device_fd);
				return;
			}
			close(sdp->device_fd);
		}
		log_warn(_("fsck.gfs2: Could not flush caches (non-fatal).\n"));
	}
}
