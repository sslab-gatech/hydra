#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libintl.h>
#include <ctype.h>
#include <fcntl.h>
#define _(String) gettext(String)

#include <logging.h>
#include "libgfs2.h"
#include "fsck.h"
#include "afterpass1_common.h"
#include "metawalk.h"
#include "util.h"

/**
 * find_remove_dup - find out if this is a duplicate ref.  If so, remove it.
 *
 * Returns: 1 if there are any remaining references to this block, else 0.
 */
static int find_remove_dup(struct gfs2_inode *ip, uint64_t block,
			   const char *btype, int *removed_last_meta)
{
	struct duptree *dt;
	struct inode_with_dups *id;
	int deleted_a_meta_ref = 0;
	int meta_refs_left = 0;

	dt = dupfind(block);
	if (!dt)
		return 0;

	/* remove the inode reference id structure for this reference. */
	id = find_dup_ref_inode(dt, ip);
	if (!id)
		goto more_refs;

	if (id->reftypecount[ref_as_meta])
		deleted_a_meta_ref = 1;
	dup_listent_delete(dt, id);
	if (dt->refs == 0) {
		log_info( _("This was the last reference: it's no longer a "
			    "duplicate.\n"));
		dup_delete(dt); /* not duplicate now */
		if (deleted_a_meta_ref) {
			log_debug("Removed the last reference as metadata.\n");
			*removed_last_meta = 1;
		}
		return 0;
	} else if (deleted_a_meta_ref) {
		/* If we deleted a metadata reference, see if there are more
		   references as meta, or if it was the last one. */
		meta_refs_left = count_dup_meta_refs(dt);
	}
more_refs:
	log_info(_("%d block reference(s) remain (%d as metadata).\n"),
		 dt->refs, meta_refs_left);
	if (deleted_a_meta_ref && meta_refs_left == 0) {
		log_debug("Removed the last reference as metadata.\n");
		*removed_last_meta = 1;
	}
	return 1; /* references still exist so do not free the block. */
}

/**
 * delete_block_if_notdup - delete blocks associated with an inode
 *
 * Ignore blocks that are already marked free.
 * If it has been identified as duplicate, remove the duplicate reference.
 * If all duplicate references have been removed, delete the block.
 */
static int delete_block_if_notdup(struct gfs2_inode *ip, uint64_t block,
				  struct gfs2_buffer_head **bh,
				  const char *btype, int *was_duplicate,
				  void *private)
{
	int q;
	int removed_lastmeta = 0;

	if (!valid_block_ip(ip, block))
		return meta_error;

	q = bitmap_type(ip->i_sbd, block);
	if (q == GFS2_BLKST_FREE) {
		log_info( _("%s block %lld (0x%llx), part of inode "
			    "%lld (0x%llx), was already free.\n"),
			  btype, (unsigned long long)block,
			  (unsigned long long)block,
			  (unsigned long long)ip->i_di.di_num.no_addr,
			  (unsigned long long)ip->i_di.di_num.no_addr);
		return meta_is_good;
	}
	if (find_remove_dup(ip, block, btype, &removed_lastmeta)) { /* a dup */
		if (was_duplicate) {
			if (removed_lastmeta)
				log_debug("Removed last reference as meta.\n");
			else
				*was_duplicate = 1;
		}
		log_err( _("Not clearing duplicate reference in inode "
			   "at block #%llu (0x%llx) to block #%llu (0x%llx) "
			   "because it's referenced by another inode.\n"),
			 (unsigned long long)ip->i_di.di_num.no_addr,
			 (unsigned long long)ip->i_di.di_num.no_addr,
			 (unsigned long long)block, (unsigned long long)block);
	} else {
		check_n_fix_bitmap(ip->i_sbd, ip->i_rgd, block, 0,
				   GFS2_BLKST_FREE);
	}
	return meta_is_good;
}

static int remove_dentry(struct gfs2_inode *ip, struct gfs2_dirent *dent,
			 struct gfs2_dirent *prev_de,
			 struct gfs2_buffer_head *bh,
			 char *filename, uint32_t *count, int *lindex,
			 void *private)
{
	/* the metawalk_fxn's private field must be set to the dentry
	 * block we want to clear */
	uint64_t *dentryblock = (uint64_t *) private;
	struct gfs2_dirent dentry, *de;

	memset(&dentry, 0, sizeof(struct gfs2_dirent));
	gfs2_dirent_in(&dentry, (char *)dent);
	de = &dentry;

	if (de->de_inum.no_addr == *dentryblock)
		dirent2_del(ip, bh, prev_de, dent);
	else
		(*count)++;

	return 0;

}

int remove_dentry_from_dir(struct gfs2_sbd *sdp, uint64_t dir,
			   uint64_t dentryblock)
{
	struct metawalk_fxns remove_dentry_fxns = {0};
	struct gfs2_inode *ip;
	int q;
	int error;

	log_debug( _("Removing dentry %llu (0x%llx) from directory %llu"
		     " (0x%llx)\n"), (unsigned long long)dentryblock,
		  (unsigned long long)dentryblock,
		  (unsigned long long)dir, (unsigned long long)dir);
	if (!valid_block(sdp, dir)) {
		log_err( _("Parent directory is invalid\n"));
		return 1;
	}
	remove_dentry_fxns.private = &dentryblock;
	remove_dentry_fxns.check_dentry = remove_dentry;

	q = bitmap_type(sdp, dir);
	if (q != GFS2_BLKST_DINODE) {
		log_info( _("Parent block is not an inode...ignoring\n"));
		return 1;
	}

	ip = fsck_load_inode(sdp, dir);
	if (ip == NULL) {
		stack;
		return -1;
	}
	/* Need to run check_dir with a private var of dentryblock,
	 * and fxns that remove that dentry if found */
	error = check_dir(sdp, ip, &remove_dentry_fxns);
	fsck_inode_put(&ip);
	return error;
}

int delete_metadata(struct gfs2_inode *ip, uint64_t block,
		    struct gfs2_buffer_head **bh, int h, int *is_valid,
		    int *was_duplicate, void *private)
{
	*is_valid = 1;
	*was_duplicate = 0;
	return delete_block_if_notdup(ip, block, bh, _("metadata"),
				      was_duplicate, private);
}

int delete_leaf(struct gfs2_inode *ip, uint64_t block, void *private)
{
	return delete_block_if_notdup(ip, block, NULL, _("leaf"), NULL,
				      private);
}

int delete_data(struct gfs2_inode *ip, uint64_t metablock,
		uint64_t block, void *private, struct gfs2_buffer_head *bh,
		uint64_t *ptr)
{
	return delete_block_if_notdup(ip, block, NULL, _("data"), NULL,
				      private);
}

static int del_eattr_generic(struct gfs2_inode *ip, uint64_t block,
			     uint64_t parent, struct gfs2_buffer_head **bh,
			     void *private, const char *eatype)
{
	int ret = 0;
	int was_free = 0;
	int q;

	if (valid_block_ip(ip, block)) {
		q = bitmap_type(ip->i_sbd, block);
		if (q == GFS2_BLKST_FREE)
			was_free = 1;
		ret = delete_block_if_notdup(ip, block, NULL, eatype,
					     NULL, private);
		if (!ret) {
			*bh = bread(ip->i_sbd, block);
			if (!was_free)
				ip->i_di.di_blocks--;
			bmodified(ip->i_bh);
		}
	}
	/* Even if it's a duplicate reference, we want to eliminate the
	   reference itself, and adjust di_blocks accordingly. */
	if (ip->i_di.di_eattr) {
		if (block == ip->i_di.di_eattr)
			ip->i_di.di_eattr = 0;
		bmodified(ip->i_bh);
	}
	return ret;
}

int delete_eattr_indir(struct gfs2_inode *ip, uint64_t block, uint64_t parent,
		       struct gfs2_buffer_head **bh, void *private)
{
	return del_eattr_generic(ip, block, parent, bh, private,
				 _("extended attribute"));
}

int delete_eattr_leaf(struct gfs2_inode *ip, uint64_t block, uint64_t parent,
		      struct gfs2_buffer_head **bh, void *private)
{
	return del_eattr_generic(ip, block, parent, bh, private,
				 _("indirect extended attribute"));
}

int delete_eattr_entry(struct gfs2_inode *ip, struct gfs2_buffer_head *leaf_bh,
		       struct gfs2_ea_header *ea_hdr,
		       struct gfs2_ea_header *ea_hdr_prev, void *private)
{
	struct gfs2_sbd *sdp = ip->i_sbd;
	char ea_name[256];
	uint32_t avail_size;
	int max_ptrs;

	if (!ea_hdr->ea_name_len){
		/* Skip this entry for now */
		return 1;
	}

	memset(ea_name, 0, sizeof(ea_name));
	strncpy(ea_name, (char *)ea_hdr + sizeof(struct gfs2_ea_header),
		ea_hdr->ea_name_len);

	if (!GFS2_EATYPE_VALID(ea_hdr->ea_type) &&
	   ((ea_hdr_prev) || (!ea_hdr_prev && ea_hdr->ea_type))){
		/* Skip invalid entry */
		return 1;
	}

	if (!ea_hdr->ea_num_ptrs)
		return 0;

	avail_size = sdp->sd_sb.sb_bsize - sizeof(struct gfs2_meta_header);
	max_ptrs = (be32_to_cpu(ea_hdr->ea_data_len) + avail_size - 1) /
		avail_size;

	if (max_ptrs > ea_hdr->ea_num_ptrs)
		return 1;

	log_debug( _("  Pointers Required: %d\n  Pointers Reported: %d\n"),
		   max_ptrs, ea_hdr->ea_num_ptrs);

	return 0;
}

int delete_eattr_extentry(struct gfs2_inode *ip, int i, uint64_t *ea_data_ptr,
			  struct gfs2_buffer_head *leaf_bh, uint32_t tot_ealen,
			  struct gfs2_ea_header *ea_hdr,
			  struct gfs2_ea_header *ea_hdr_prev, void *private)
{
	uint64_t block = be64_to_cpu(*ea_data_ptr);
	int error;

	error = delete_block_if_notdup(ip, block, NULL,
				       _("extended attribute"), NULL, private);
	if (error) {
		log_err(_("Bad extended attribute found at block %lld "
			  "(0x%llx)"),
			(unsigned long long)be64_to_cpu(*ea_data_ptr),
			(unsigned long long)be64_to_cpu(*ea_data_ptr));
		if (query( _("Repair the bad Extended Attribute? (y/n) "))) {
			ea_hdr->ea_num_ptrs = i;
			ea_hdr->ea_data_len = cpu_to_be32(tot_ealen);
			*ea_data_ptr = 0;
			bmodified(leaf_bh);
			/* Endianness doesn't matter in this case because it's
			   a single byte. */
			fsck_bitmap_set(ip, ip->i_di.di_eattr,
					_("extended attribute"),
					ip->i_sbd->gfs1 ? GFS2_BLKST_DINODE :
					GFS2_BLKST_USED);
			log_err( _("The EA was fixed.\n"));
		} else {
			error = 1;
			log_err( _("The bad EA was not fixed.\n"));
		}
	}
	return error;
}
