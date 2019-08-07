#include "clusterautoconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <dirent.h>
#include <libintl.h>
#define _(String) gettext(String)

#include <logging.h>
#include "libgfs2.h"
#include "osi_list.h"
#include "fsck.h"
#include "lost_n_found.h"
#include "link.h"
#include "metawalk.h"
#include "util.h"
#include "afterpass1_common.h"

static int attach_dotdot_to(struct gfs2_sbd *sdp, uint64_t newdotdot,
			    uint64_t olddotdot, uint64_t block)
{
	const char *filename = "..";
	int filename_len = 2;
	int err;
	struct gfs2_inode *ip, *pip;

	ip = fsck_load_inode(sdp, block);
	pip = fsck_load_inode(sdp, newdotdot);
	/* FIXME: Need to add some interactive
	 * options here and come up with a
	 * good default for non-interactive */
	/* FIXME: do i need to correct the
	 * '..' entry for this directory in
	 * this case? */

	if (gfs2_dirent_del(ip, filename, filename_len))
		log_warn( _("Unable to remove \"..\" directory entry.\n"));
	else
		decr_link_count(olddotdot, block, sdp->gfs1, _("old \"..\""));
	err = dir_add(ip, filename, filename_len, &pip->i_di.di_num,
		      (sdp->gfs1 ? GFS_FILE_DIR : DT_DIR));
	if (err) {
		log_err(_("Error adding directory %s: %s\n"),
		        filename, strerror(errno));
		exit(FSCK_ERROR);
	}
	incr_link_count(pip->i_di.di_num, ip, _("new \"..\""));
	fsck_inode_put(&ip);
	fsck_inode_put(&pip);
	return 0;
}

static struct dir_info *mark_and_return_parent(struct gfs2_sbd *sdp,
					       struct dir_info *di)
{
	struct dir_info *pdi;
	int q_dotdot, q_treewalk;
	int error = 0;
	struct dir_info *dt_dotdot, *dt_treewalk;

	di->checked = 1;

	if (!di->treewalk_parent)
		return NULL;

	if (di->dotdot_parent.no_addr == di->treewalk_parent) {
		q_dotdot = bitmap_type(sdp, di->dotdot_parent.no_addr);
		if (q_dotdot != GFS2_BLKST_DINODE) {
			log_err( _("Orphaned directory at block %llu (0x%llx) "
				   "moved to lost+found\n"),
				 (unsigned long long)di->dinode.no_addr,
				 (unsigned long long)di->dinode.no_addr);
			return NULL;
		}
		goto out;
	}

	log_warn( _("Directory '..' and treewalk connections disagree for "
		    "inode %llu (0x%llx)\n"),
		  (unsigned long long)di->dinode.no_addr,
		  (unsigned long long)di->dinode.no_addr);
	log_notice( _("'..' has %llu (0x%llx), treewalk has %llu (0x%llx)\n"),
		    (unsigned long long)di->dotdot_parent.no_addr,
		    (unsigned long long)di->dotdot_parent.no_addr,
		    (unsigned long long)di->treewalk_parent,
		    (unsigned long long)di->treewalk_parent);
	q_dotdot = bitmap_type(sdp, di->dotdot_parent.no_addr);
	dt_dotdot = dirtree_find(di->dotdot_parent.no_addr);
	q_treewalk = bitmap_type(sdp, di->treewalk_parent);
	dt_treewalk = dirtree_find(di->treewalk_parent);
	/* if the dotdot entry isn't a directory, but the
	 * treewalk is, treewalk is correct - if the treewalk
	 * entry isn't a directory, but the dotdot is, dotdot
	 * is correct - if both are directories, which do we
	 * choose? if neither are directories, we have a
	 * problem - need to move this directory into lost+found
	 */
	if (q_dotdot != GFS2_BLKST_DINODE || dt_dotdot == NULL) {
		if (q_treewalk != GFS2_BLKST_DINODE) {
			log_err( _("Orphaned directory, move to "
				   "lost+found\n"));
			return NULL;
		} else {
			log_warn( _("Treewalk parent is correct, fixing "
				    "dotdot -> %llu (0x%llx)\n"),
				  (unsigned long long)di->treewalk_parent,
				  (unsigned long long)di->treewalk_parent);
			attach_dotdot_to(sdp, di->treewalk_parent,
					 di->dotdot_parent.no_addr,
					 di->dinode.no_addr);
			di->dotdot_parent.no_addr = di->treewalk_parent;
		}
		goto out;
	}
	if (dt_treewalk) {
		log_err( _("Both .. and treewalk parents are directories, "
			   "going with treewalk...\n"));
		attach_dotdot_to(sdp, di->treewalk_parent,
				 di->dotdot_parent.no_addr,
				 di->dinode.no_addr);
		di->dotdot_parent.no_addr = di->treewalk_parent;
		goto out;
	}
	log_warn( _(".. parent is valid, but treewalk is bad - reattaching to "
		    "lost+found"));

	/* FIXME: add a dinode for this entry instead? */

	if (!query( _("Remove directory entry for bad inode %llu (0x%llx) in "
		      "%llu (0x%llx)? (y/n)"),
		    (unsigned long long)di->dinode.no_addr,
		    (unsigned long long)di->dinode.no_addr,
		    (unsigned long long)di->treewalk_parent,
		    (unsigned long long)di->treewalk_parent)) {
		log_err( _("Directory entry to invalid inode remains\n"));
		return NULL;
	}
	error = remove_dentry_from_dir(sdp, di->treewalk_parent,
				       di->dinode.no_addr);
	if (error < 0) {
		stack;
		return NULL;
	}
	if (error > 0)
		log_warn( _("Unable to find dentry for block %llu"
			    " (0x%llx) in %llu (0x%llx)\n"),
			  (unsigned long long)di->dinode.no_addr,
			  (unsigned long long)di->dinode.no_addr,
			  (unsigned long long)di->treewalk_parent,
			  (unsigned long long)di->treewalk_parent);
	log_warn( _("Directory entry removed\n"));
	log_info( _("Marking directory unlinked\n"));

	return NULL;

out:
	pdi = dirtree_find(di->dotdot_parent.no_addr);

	return pdi;
}

/**
 * pass3 - check connectivity of directories
 *
 * handle disconnected directories
 * handle lost+found directory errors (missing, not a directory, no space)
 */
int pass3(struct gfs2_sbd *sdp)
{
	struct osi_node *tmp, *next = NULL;
	struct dir_info *di, *tdi;
	struct gfs2_inode *ip;
	int q;

	di = dirtree_find(sdp->md.rooti->i_di.di_num.no_addr);
	if (di) {
		log_info( _("Marking root inode connected\n"));
		di->checked = 1;
	}
	if (sdp->gfs1) {
		di = dirtree_find(sdp->md.statfs->i_di.di_num.no_addr);
		if (di) {
			log_info( _("Marking GFS1 statfs file inode "
				    "connected\n"));
			di->checked = 1;
		}
		di = dirtree_find(sdp->md.jiinode->i_di.di_num.no_addr);
		if (di) {
			log_info( _("Marking GFS1 jindex file inode "
				    "connected\n"));
			di->checked = 1;
		}
		di = dirtree_find(sdp->md.riinode->i_di.di_num.no_addr);
		if (di) {
			log_info( _("Marking GFS1 rindex file inode "
				    "connected\n"));
			di->checked = 1;
		}
		di = dirtree_find(sdp->md.qinode->i_di.di_num.no_addr);
		if (di) {
			log_info( _("Marking GFS1 quota file inode "
				    "connected\n"));
			di->checked = 1;
		}
	} else {
		di = dirtree_find(sdp->master_dir->i_di.di_num.no_addr);
		if (di) {
			log_info( _("Marking master directory inode "
				    "connected\n"));
			di->checked = 1;
		}
	}

	/* Go through the directory list, working up through the parents
	 * until we find one that's been checked already.  If we don't
	 * find a parent, put in lost+found.
	 */
	log_info( _("Checking directory linkage.\n"));
	for (tmp = osi_first(&dirtree); tmp; tmp = next) {
		next = osi_next(tmp);
		di = (struct dir_info *)tmp;
		while (!di->checked) {
			/* FIXME: Change this so it returns success or
			 * failure and put the parent inode in a
			 * param */
			if (skip_this_pass || fsck_abort) /* if asked to skip the rest */
				return FSCK_OK;
			tdi = mark_and_return_parent(sdp, di);

			if (tdi) {
				log_debug( _("Directory at block %llu "
					     "(0x%llx) connected\n"),
					   (unsigned long long)di->dinode.no_addr,
					   (unsigned long long)di->dinode.no_addr);
				di = tdi;
				continue;
			}
			q = bitmap_type(sdp, di->dinode.no_addr);
			ip = fsck_load_inode(sdp, di->dinode.no_addr);
			if (q == GFS2_BLKST_FREE) {
				log_err( _("Found unlinked directory "
					   "containing bad block at block %llu"
					   " (0x%llx)\n"),
					(unsigned long long)di->dinode.no_addr,
					(unsigned long long)di->dinode.no_addr);
				if (query(_("Clear unlinked directory "
					   "with bad blocks? (y/n) "))) {
					log_warn( _("inode %lld (0x%llx) is "
						    "now marked as free\n"),
						  (unsigned long long)
						  di->dinode.no_addr,
						  (unsigned long long)
						  di->dinode.no_addr);
					check_n_fix_bitmap(sdp, ip->i_rgd,
							   di->dinode.no_addr,
							   0, GFS2_BLKST_FREE);
					fsck_inode_put(&ip);
					break;
				} else
					log_err( _("Unlinked directory with bad block remains\n"));
			}
			if (q != GFS2_BLKST_DINODE) {
				log_err( _("Unlinked block marked as an inode "
					   "is not an inode\n"));
				if (!query(_("Clear the unlinked block?"
					    " (y/n) "))) {
					log_err( _("The block was not "
						   "cleared\n"));
					fsck_inode_put(&ip);
					break;
				}
				log_warn( _("inode %lld (0x%llx) is now "
					    "marked as free\n"),
					  (unsigned long long)di->dinode.no_addr,
					  (unsigned long long)di->dinode.no_addr);
				check_n_fix_bitmap(sdp, ip->i_rgd,
						   di->dinode.no_addr, 0,
						   GFS2_BLKST_FREE);
				log_err( _("The block was cleared\n"));
				fsck_inode_put(&ip);
				break;
			}

			log_err( _("Found unlinked directory at block %llu"
				   " (0x%llx)\n"),
				 (unsigned long long)di->dinode.no_addr,
				 (unsigned long long)di->dinode.no_addr);
			/* Don't skip zero size directories with eattrs */
			if (!ip->i_di.di_size && !ip->i_di.di_eattr){
				log_err( _("Unlinked directory has zero "
					   "size.\n"));
				if (query( _("Remove zero-size unlinked "
					    "directory? (y/n) "))) {
					fsck_bitmap_set(ip, di->dinode.no_addr,
						_("zero-sized unlinked inode"),
							GFS2_BLKST_FREE);
					fsck_inode_put(&ip);
					break;
				} else {
					log_err( _("Zero-size unlinked "
						   "directory remains\n"));
				}
			}
			if (query( _("Add unlinked directory to "
				    "lost+found? (y/n) "))) {
				if (add_inode_to_lf(ip)) {
					fsck_inode_put(&ip);
					stack;
					return FSCK_ERROR;
				}
				log_warn( _("Directory relinked to lost+found\n"));
			} else {
				log_err( _("Unlinked directory remains unlinked\n"));
			}
			fsck_inode_put(&ip);
			break;
		}
	}
	if (lf_dip) {
		log_debug( _("At end of pass3, lost+found entries is %u\n"),
				  lf_dip->i_di.di_entries);
	}
	return FSCK_OK;
}
