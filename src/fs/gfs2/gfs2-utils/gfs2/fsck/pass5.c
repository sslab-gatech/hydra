#include "clusterautoconfig.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <libintl.h>
#define _(String) gettext(String)

#include <logging.h>
#include "libgfs2.h"
#include "fsck.h"
#include "util.h"

#define GFS1_BLKST_USEDMETA 4

static int check_block_status(struct gfs2_sbd *sdp,  struct gfs2_bmap *bl,
			      char *buffer, unsigned int buflen,
			      uint64_t *rg_block, uint64_t rg_data,
			      uint32_t *count)
{
	unsigned char *byte, *end;
	unsigned int bit;
	unsigned char rg_status;
	int q;
	uint64_t block;

	/* FIXME verify cast */
	byte = (unsigned char *) buffer;
	bit = 0;
	end = (unsigned char *) buffer + buflen;

	while (byte < end) {
		rg_status = ((*byte >> bit) & GFS2_BIT_MASK);
		block = rg_data + *rg_block;
		warm_fuzzy_stuff(block);
		if (skip_this_pass || fsck_abort) /* if asked to skip the rest */
			return 0;

		q = block_type(bl, block);
		/* GFS1 file systems will have to suffer from slower fsck run
		 * times because in GFS, there's no 1:1 relationship between
		 * bits and counts. If a bit is marked "dinode" in GFS1, it
		 * may be dinode -OR- any kind of metadata. I consider GFS1 to
		 * be a rare exception, so acceptable loss at this point. So
		 * we must determine whether it's really a dinode or other
		 * metadata by reading it in. */
		if (sdp->gfs1 && q == GFS2_BLKST_DINODE) {
			struct gfs2_buffer_head *bh;

			bh = bread(sdp, block);
			if (gfs2_check_meta(bh, GFS2_METATYPE_DI) == 0)
				count[GFS2_BLKST_DINODE]++;
			else
				count[GFS1_BLKST_USEDMETA]++;
			brelse(bh);
		} else {
			count[q]++;
		}

		/* If one node opens a file and another node deletes it, we
		   may be left with a block that appears to be "unlinked" in
		   the bitmap, but nothing links to it. This is a valid case
		   and should be cleaned up by the file system eventually.
		   So we ignore it. */
		if (q == GFS2_BLKST_UNLINKED) {
			log_err( _("Unlinked inode found at block %llu "
				   "(0x%llx).\n"),
				 (unsigned long long)block,
				 (unsigned long long)block);
			if (query(_("Do you want to reclaim the block? "
				   "(y/n) "))) {
				lgfs2_rgrp_t rg = gfs2_blk2rgrpd(sdp, block);
				if (gfs2_set_bitmap(rg, block, GFS2_BLKST_FREE))
					log_err(_("Unlinked block %llu "
						  "(0x%llx) bitmap not fixed."
						  "\n"),
						(unsigned long long)block,
						(unsigned long long)block);
				else {
					log_err(_("Unlinked block %llu "
						  "(0x%llx) bitmap fixed.\n"),
						(unsigned long long)block,
						(unsigned long long)block);
					count[GFS2_BLKST_UNLINKED]--;
					count[GFS2_BLKST_FREE]++;
				}
			} else {
				log_info( _("Unlinked block found at block %llu"
					    " (0x%llx), left unchanged.\n"),
					(unsigned long long)block,
					(unsigned long long)block);
			}
		} else if (rg_status != q) {
			log_err( _("Block %llu (0x%llx) bitmap says %u (%s) "
				   "but FSCK saw %u (%s)\n"),
				 (unsigned long long)block,
				 (unsigned long long)block, rg_status,
				 block_type_string(rg_status), q,
				 block_type_string(q));
			if (q) /* Don't print redundant "free" */
				log_err( _("Metadata type is %u (%s)\n"), q,
					 block_type_string(q));

			if (query(_("Fix bitmap for block %llu (0x%llx) ? (y/n) "),
				 (unsigned long long)block,
				 (unsigned long long)block)) {
				lgfs2_rgrp_t rg = gfs2_blk2rgrpd(sdp, block);
				if (gfs2_set_bitmap(rg, block, q))
					log_err( _("Repair failed.\n"));
				else
					log_err( _("Fixed.\n"));
			} else
				log_err( _("Bitmap at block %llu (0x%llx) left inconsistent\n"),
					(unsigned long long)block,
					(unsigned long long)block);
		}
		(*rg_block)++;
		bit += GFS2_BIT_SIZE;
		if (bit >= 8){
			bit = 0;
			byte++;
		}
	}

	return 0;
}

static void update_rgrp(struct gfs2_sbd *sdp, struct rgrp_tree *rgp,
			struct gfs2_bmap *bl, uint32_t *count)
{
	uint32_t i;
	struct gfs2_bitmap *bits;
	uint64_t rg_block = 0;
	int update = 0;
	struct gfs_rgrp *gfs1rg = (struct gfs_rgrp *)&rgp->rg;

	for(i = 0; i < rgp->ri.ri_length; i++) {
		bits = &rgp->bits[i];

		/* update the bitmaps */
		if (check_block_status(sdp, bl, bits->bi_bh->b_data + bits->bi_offset,
		                       bits->bi_len, &rg_block, rgp->ri.ri_data0, count))
			return;
		if (skip_this_pass || fsck_abort) /* if asked to skip the rest */
			return;
	}

	/* actually adjust counters and write out to disk */
	if (rgp->rg.rg_free != count[GFS2_BLKST_FREE]) {
		log_err( _("RG #%llu (0x%llx) free count inconsistent: "
			"is %u should be %u\n"),
			(unsigned long long)rgp->ri.ri_addr,
			(unsigned long long)rgp->ri.ri_addr,
			rgp->rg.rg_free, count[GFS2_BLKST_FREE]);
		rgp->rg.rg_free = count[GFS2_BLKST_FREE];
		update = 1;
	}
	if (rgp->rg.rg_dinodes != count[GFS2_BLKST_DINODE]) {
		log_err( _("RG #%llu (0x%llx) Inode count inconsistent: is "
			   "%u should be %u\n"),
			 (unsigned long long)rgp->ri.ri_addr,
			 (unsigned long long)rgp->ri.ri_addr,
			 rgp->rg.rg_dinodes, count[GFS2_BLKST_DINODE]);
		rgp->rg.rg_dinodes = count[GFS2_BLKST_DINODE];
		update = 1;
	}
	if (sdp->gfs1 && gfs1rg->rg_usedmeta != count[GFS1_BLKST_USEDMETA]) {
		log_err( _("RG #%llu (0x%llx) Used metadata count "
			   "inconsistent: is %u should be %u\n"),
			 (unsigned long long)rgp->ri.ri_addr,
			 (unsigned long long)rgp->ri.ri_addr,
			 gfs1rg->rg_usedmeta, count[GFS1_BLKST_USEDMETA]);
		gfs1rg->rg_usedmeta = count[GFS1_BLKST_USEDMETA];
		update = 1;
	}
	if (sdp->gfs1 && gfs1rg->rg_freemeta != count[GFS2_BLKST_UNLINKED]) {
		log_err( _("RG #%llu (0x%llx) Free metadata count "
			   "inconsistent: is %u should be %u\n"),
			 (unsigned long long)rgp->ri.ri_addr,
			 (unsigned long long)rgp->ri.ri_addr,
			 gfs1rg->rg_freemeta, count[GFS2_BLKST_UNLINKED]);
		gfs1rg->rg_freemeta = count[GFS2_BLKST_UNLINKED];
		update = 1;
	}
	if (!sdp->gfs1 && (rgp->ri.ri_data != count[GFS2_BLKST_FREE] +
			   count[GFS2_BLKST_USED] +
			   count[GFS2_BLKST_UNLINKED] +
			   count[GFS2_BLKST_DINODE])) {
		/* FIXME not sure how to handle this case ATM - it
		 * means that the total number of blocks we've counted
		 * exceeds the blocks in the rg */
		log_err( _("Internal fsck error: %u != %u + %u + %u + %u\n"),
			 rgp->ri.ri_data, count[GFS2_BLKST_FREE],
			 count[GFS2_BLKST_USED], count[GFS2_BLKST_UNLINKED],
			 count[GFS2_BLKST_DINODE]);
		exit(FSCK_ERROR);
	}
	if (update) {
		if (query( _("Update resource group counts? (y/n) "))) {
			log_warn( _("Resource group counts updated\n"));
			/* write out the rgrp */
			if (sdp->gfs1)
				gfs_rgrp_out(gfs1rg, rgp->bits[0].bi_bh);
			else
				gfs2_rgrp_out(&rgp->rg, rgp->bits[0].bi_bh->b_data);
		} else
			log_err( _("Resource group counts left inconsistent\n"));
	}
}

/**
 * pass5 - check resource groups
 *
 * fix free block maps
 * fix used inode maps
 */
int pass5(struct gfs2_sbd *sdp, struct gfs2_bmap *bl)
{
	struct osi_node *n, *next = NULL;
	struct rgrp_tree *rgp = NULL;
	uint32_t count[5]; /* we need 5 because of GFS1 usedmeta */
	uint64_t rg_count = 0;

	/* Reconcile RG bitmaps with fsck bitmap */
	for (n = osi_first(&sdp->rgtree); n; n = next) {
		next = osi_next(n);
		if (skip_this_pass || fsck_abort) /* if asked to skip the rest */
			return FSCK_OK;
		log_info( _("Verifying Resource Group #%llu\n"), (unsigned long long)rg_count);
		memset(count, 0, sizeof(count));
		rgp = (struct rgrp_tree *)n;

		rg_count++;
		/* Compare the bitmaps and report the differences */
		update_rgrp(sdp, rgp, bl, count);
	}
	/* Fix up superblock info based on this - don't think there's
	 * anything to do here... */

	return FSCK_OK;
}
