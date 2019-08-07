#ifndef _LINK_H
#define _LINK_H

extern struct gfs2_bmap nlink1map; /* map of dinodes with nlink == 1 */
extern struct gfs2_bmap clink1map; /* map of dinodes w/counted links == 1 */

enum {
	incr_link_bad = -1,
	incr_link_good = 0,
	incr_link_ino_mismatch = 1,
	incr_link_check_orig = 2,
};

int link1_set(struct gfs2_bmap *bmap, uint64_t bblock, int mark);
int set_di_nlink(struct gfs2_inode *ip);
int incr_link_count(struct gfs2_inum no, struct gfs2_inode *ip,
		    const char *why);
int decr_link_count(uint64_t inode_no, uint64_t referenced_from, int gfs1,
		    const char *why);

#endif /* _LINK_H */
