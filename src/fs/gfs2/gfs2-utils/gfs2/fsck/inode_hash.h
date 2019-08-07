#ifndef _INODE_HASH_H
#define _INODE_HASH_H

struct inode_info;

extern struct inode_info *inodetree_find(uint64_t block);
extern struct inode_info *inodetree_insert(struct gfs2_inum di_num);
extern void inodetree_delete(struct inode_info *b);

#endif /* _INODE_HASH_H */
