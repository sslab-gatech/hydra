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
#include "config.h"

/**
 * Given a number of blocks in a resource group, return the number of blocks
 * needed for bitmaps. Also calculate the adjusted number of free data blocks
 * in the resource group and store it in *ri_data.
 */
uint32_t rgblocks2bitblocks(const unsigned int bsize, const uint32_t rgblocks, uint32_t *ri_data)
{
	uint32_t mappable = 0;
	uint32_t bitblocks = 0;
	/* Number of blocks mappable by bitmap blocks with these header types */
	const uint32_t blks_rgrp = GFS2_NBBY * (bsize - sizeof(struct gfs2_rgrp));
	const uint32_t blks_meta = GFS2_NBBY * (bsize - sizeof(struct gfs2_meta_header));

	while (blks_rgrp + (blks_meta * bitblocks) < ((rgblocks - bitblocks) & ~(uint32_t)3))
		bitblocks++;

	if (bitblocks > 0)
		mappable = blks_rgrp + (blks_meta * (bitblocks - 1));

	*ri_data = (rgblocks - (bitblocks + 1)) & ~(uint32_t)3;
	if (mappable < *ri_data)
		bitblocks++;

	return bitblocks;
}

/**
 * build_rgrps - write a bunch of resource groups to disk.
 * If fd > 0, write the data to the given file handle.
 * Otherwise, use gfs2 buffering in buf.c.
 */
int build_rgrps(struct gfs2_sbd *sdp, int do_write)
{
	struct osi_node *n, *next = NULL;
	struct rgrp_tree *rl;
	uint32_t rgblocks, bitblocks;
	struct gfs2_rindex *ri;
	struct gfs2_meta_header mh;
	unsigned int x;

	mh.mh_magic = GFS2_MAGIC;
	mh.mh_type = GFS2_METATYPE_RB;
	mh.mh_format = GFS2_FORMAT_RB;
	if (do_write)
		n = osi_first(&sdp->rgtree);
	else
		n = osi_first(&sdp->rgcalc);

	for (; n; n = next) {
		next = osi_next(n);
		rl = (struct rgrp_tree *)n;
		ri = &rl->ri;

		bitblocks = rgblocks2bitblocks(sdp->bsize, rl->length, &rgblocks);

		ri->ri_addr = rl->start;
		ri->ri_length = bitblocks;
		ri->ri_data0 = rl->start + bitblocks;
		ri->ri_data = rgblocks;
		ri->ri_bitbytes = rgblocks / GFS2_NBBY;

		memset(&rl->rg, 0, sizeof(rl->rg));
		rl->rg.rg_header.mh_magic = GFS2_MAGIC;
		rl->rg.rg_header.mh_type = GFS2_METATYPE_RG;
		rl->rg.rg_header.mh_format = GFS2_FORMAT_RG;
		rl->rg.rg_free = rgblocks;

		if (gfs2_compute_bitstructs(sdp->sd_sb.sb_bsize, rl))
			return -1;

		if (do_write) {
			for (x = 0; x < bitblocks; x++) {
				rl->bits[x].bi_bh = bget(sdp, rl->start + x);
				if (x)
					gfs2_meta_header_out(&mh, rl->bits[x].bi_bh->b_data);
				else
					gfs2_rgrp_out(&rl->rg, rl->bits[x].bi_bh->b_data);
				bmodified(rl->bits[x].bi_bh);
			}
		}

		if (cfg_debug) {
			printf("\n");
			gfs2_rindex_print(ri);
		}

		sdp->blks_total += rgblocks;
		sdp->fssize = ri->ri_data0 + ri->ri_data;
	}
	return 0;
}
