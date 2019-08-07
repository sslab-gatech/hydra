#include "clusterautoconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <linux/types.h>
#include "libgfs2.h"
#ifdef GFS2_HAS_UUID
#include <uuid.h>
#endif

#define pv(struct, member, fmt, fmt2) do {				\
		print_it("  "#member, fmt, fmt2, struct->member);	\
	} while (FALSE);
#define pv2(struct, member, fmt, fmt2) do {				\
		print_it("  ", fmt, fmt2, struct->member);		\
	} while (FALSE);


#define CPIN_08(s1, s2, member, count) {memcpy((s1->member), (s2->member), (count));}
#define CPOUT_08(s1, s2, member, count) {memcpy((s2->member), (s1->member), (count));}
#define CPIN_16(s1, s2, member) {(s1->member) = be16_to_cpu((s2->member));}
#define CPOUT_16(s1, s2, member) {(s2->member) = cpu_to_be16((s1->member));}
#define CPIN_32(s1, s2, member) {(s1->member) = be32_to_cpu((s2->member));}
#define CPOUT_32(s1, s2, member) {(s2->member) = cpu_to_be32((s1->member));}
#define CPIN_64(s1, s2, member) {(s1->member) = be64_to_cpu((s2->member));}
#define CPOUT_64(s1, s2, member) {(s2->member) = cpu_to_be64((s1->member));}

/*
 * gfs2_xxx_in - read in an xxx struct
 * first arg: the cpu-order structure
 * buf: the disk-order block data
 *
 * gfs2_xxx_out - write out an xxx struct
 * first arg: the cpu-order structure
 * buf: the disk-order block data
 *
 * gfs2_xxx_print - print out an xxx struct
 * first arg: the cpu-order structure
 */

void gfs2_inum_in(struct gfs2_inum *no, char *buf)
{
	struct gfs2_inum *str = (struct gfs2_inum *)buf;

	CPIN_64(no, str, no_formal_ino);
	CPIN_64(no, str, no_addr);
}

void gfs2_inum_out(const struct gfs2_inum *no, char *buf)
{
	struct gfs2_inum *str = (struct gfs2_inum *)buf;

	CPOUT_64(no, str, no_formal_ino);
	CPOUT_64(no, str, no_addr);
}

void gfs2_inum_print(const struct gfs2_inum *no)
{
	pv(no, no_formal_ino, "%llu", "0x%llx");
	pv(no, no_addr, "%llu", "0x%llx");
}

void gfs2_meta_header_in(struct gfs2_meta_header *mh, char *buf)
{
	struct gfs2_meta_header *str = (struct gfs2_meta_header *)buf;

	CPIN_32(mh, str, mh_magic);
	CPIN_32(mh, str, mh_type);
	CPIN_32(mh, str, mh_format);
}

void gfs2_meta_header_out(const struct gfs2_meta_header *mh, char *buf)
{
	struct gfs2_meta_header *str = (struct gfs2_meta_header *)buf;

	CPOUT_32(mh, str, mh_magic);
	CPOUT_32(mh, str, mh_type);
	CPOUT_32(mh, str, mh_format);
	str->__pad0 = 0;
	str->__pad1 = 0;
}

void gfs2_meta_header_print(const struct gfs2_meta_header *mh)
{
	pv(mh, mh_magic, "0x%08X", NULL);
	pv(mh, mh_type, "%u", "0x%x");
	pv(mh, mh_format, "%u", "0x%x");
}

void gfs2_sb_in(struct gfs2_sb *sb, char *buf)
{
	struct gfs2_sb *str = (struct gfs2_sb *)buf;

	gfs2_meta_header_in(&sb->sb_header, buf);

	CPIN_32(sb, str, sb_fs_format);
	CPIN_32(sb, str, sb_multihost_format);
	CPIN_32(sb, str, __pad0);                        /* gfs sb_flags */

	CPIN_32(sb, str, sb_bsize);
	CPIN_32(sb, str, sb_bsize_shift);
	CPIN_32(sb, str, __pad1);                        /* gfs sb_seg_size */

	gfs2_inum_in(&sb->sb_master_dir, (char *)&str->sb_master_dir);
	gfs2_inum_in(&sb->sb_root_dir, (char *)&str->sb_root_dir);

	CPIN_08(sb, str, sb_lockproto, GFS2_LOCKNAME_LEN);
	CPIN_08(sb, str, sb_locktable, GFS2_LOCKNAME_LEN);
	gfs2_inum_in(&sb->__pad2, (char *)&str->__pad2); /* gfs rindex */
	gfs2_inum_in(&sb->__pad3, (char *)&str->__pad3); /* gfs quota */
	gfs2_inum_in(&sb->__pad4, (char *)&str->__pad4); /* gfs license */
#ifdef GFS2_HAS_UUID
	CPIN_08(sb, str, sb_uuid, sizeof(sb->sb_uuid));
#endif
}

void gfs2_sb_out(const struct gfs2_sb *sb, char *buf)
{
	struct gfs2_sb *str = (struct gfs2_sb *)buf;

	gfs2_meta_header_out(&sb->sb_header, buf);

	CPOUT_32(sb, str, sb_fs_format);
	CPOUT_32(sb, str, sb_multihost_format);
	CPOUT_32(sb, str, __pad0);                        /* gfs sb_flags */

	CPOUT_32(sb, str, sb_bsize);
	CPOUT_32(sb, str, sb_bsize_shift);
	CPOUT_32(sb, str, __pad1);                        /* gfs sb_seg_size */

	gfs2_inum_out(&sb->sb_master_dir, (char *)&str->sb_master_dir);
	gfs2_inum_out(&sb->sb_root_dir, (char *)&str->sb_root_dir);

	CPOUT_08(sb, str, sb_lockproto, GFS2_LOCKNAME_LEN);
	CPOUT_08(sb, str, sb_locktable, GFS2_LOCKNAME_LEN);
	gfs2_inum_out(&sb->__pad2, (char *)&str->__pad2); /* gfs rindex */
	gfs2_inum_out(&sb->__pad3, (char *)&str->__pad3); /* gfs quota */
	gfs2_inum_out(&sb->__pad4, (char *)&str->__pad4); /* gfs license */
#ifdef GFS2_HAS_UUID
	memcpy(str->sb_uuid, sb->sb_uuid, 16);
#endif
}

void gfs2_sb_print(const struct gfs2_sb *sb)
{
	gfs2_meta_header_print(&sb->sb_header);

	pv(sb, sb_fs_format, "%u", "0x%x");
	pv(sb, sb_multihost_format, "%u", "0x%x");

	pv(sb, sb_bsize, "%u", "0x%x");
	pv(sb, sb_bsize_shift, "%u", "0x%x");

	gfs2_inum_print(&sb->sb_master_dir);
	gfs2_inum_print(&sb->sb_root_dir);

	pv(sb, sb_lockproto, "%s", NULL);
	pv(sb, sb_locktable, "%s", NULL);

#ifdef GFS2_HAS_UUID
	{
	char readable_uuid[36+1];

	uuid_unparse(sb->sb_uuid, readable_uuid);
	print_it("  uuid", "%36s", NULL, readable_uuid);
	}
#endif
}

void gfs2_rindex_in(struct gfs2_rindex *ri, char *buf)
{
	struct gfs2_rindex *str = (struct gfs2_rindex *)buf;

	CPIN_64(ri, str, ri_addr);
	CPIN_32(ri, str, ri_length);
	CPIN_32(ri, str, __pad);
	CPIN_64(ri, str, ri_data0);
	CPIN_32(ri, str, ri_data);
	CPIN_32(ri, str, ri_bitbytes);
	CPIN_08(ri, str, ri_reserved, sizeof(ri->ri_reserved));
}

void gfs2_rindex_out(const struct gfs2_rindex *ri, char *buf)
{
	struct gfs2_rindex *str = (struct gfs2_rindex *)buf;

	CPOUT_64(ri, str, ri_addr);
	CPOUT_32(ri, str, ri_length);
	str->__pad = 0;

	CPOUT_64(ri, str, ri_data0);
	CPOUT_32(ri, str, ri_data);

	CPOUT_32(ri, str, ri_bitbytes);

	CPOUT_08(ri, str, ri_reserved, sizeof(ri->ri_reserved));
}

void gfs2_rindex_print(const struct gfs2_rindex *ri)
{
	pv(ri, ri_addr, "%llu", "0x%llx");
	pv(ri, ri_length, "%u", "0x%x");

	pv(ri, ri_data0, "%llu", "0x%llx");
	pv(ri, ri_data, "%u", "0x%x");

	pv(ri, ri_bitbytes, "%u", "0x%x");
}

void gfs2_rgrp_in(struct gfs2_rgrp *rg, char *buf)
{
	struct gfs2_rgrp *str = (struct gfs2_rgrp *)buf;

	gfs2_meta_header_in(&rg->rg_header, buf);
	CPIN_32(rg, str, rg_flags);
	CPIN_32(rg, str, rg_free);
	CPIN_32(rg, str, rg_dinodes);
#ifdef GFS2_HAS_RG_SKIP
	CPIN_32(rg, str, rg_skip);
#else
	CPIN_32(rg, str, __pad);
#endif
	CPIN_64(rg, str, rg_igeneration);
#ifdef GFS2_HAS_RG_RI_FIELDS
	CPIN_64(rg, str, rg_data0);
	CPIN_32(rg, str, rg_data);
	CPIN_32(rg, str, rg_bitbytes);
	CPIN_32(rg, str, rg_crc);
#endif
	CPIN_08(rg, str, rg_reserved, sizeof(rg->rg_reserved));
}

void gfs2_rgrp_out(const struct gfs2_rgrp *rg, char *buf)
{
	struct gfs2_rgrp *str = (struct gfs2_rgrp *)buf;

	gfs2_meta_header_out(&rg->rg_header, buf);
	CPOUT_32(rg, str, rg_flags);
	CPOUT_32(rg, str, rg_free);
	CPOUT_32(rg, str, rg_dinodes);
#ifdef GFS2_HAS_RG_SKIP
	CPOUT_32(rg, str, rg_skip);
#else
	CPOUT_32(rg, str, __pad);
#endif
	CPOUT_64(rg, str, rg_igeneration);
#ifdef GFS2_HAS_RG_RI_FIELDS
	CPOUT_64(rg, str, rg_data0);
	CPOUT_32(rg, str, rg_data);
	CPOUT_32(rg, str, rg_bitbytes);
	CPOUT_08(rg, str, rg_reserved, sizeof(rg->rg_reserved));
	lgfs2_rgrp_crc_set(buf);
#else
	CPOUT_08(rg, str, rg_reserved, sizeof(rg->rg_reserved));
#endif
}

void gfs2_rgrp_print(const struct gfs2_rgrp *rg)
{
	gfs2_meta_header_print(&rg->rg_header);
	pv(rg, rg_flags, "%u", "0x%x");
	pv(rg, rg_free, "%u", "0x%x");
	pv(rg, rg_dinodes, "%u", "0x%x");
#ifdef GFS2_HAS_RG_SKIP
	pv(rg, rg_skip, "%u", "0x%x");
#else
	pv(rg, __pad, "%u", "0x%x");
#endif
	pv(rg, rg_igeneration, "%llu", "0x%llx");
#ifdef GFS2_HAS_RG_RI_FIELDS
	pv(rg, rg_data0, "%llu", "0x%llx");
	pv(rg, rg_data, "%u", "0x%x");
	pv(rg, rg_bitbytes, "%u", "0x%x");
	pv(rg, rg_crc, "%u", "0x%x");
#endif
}

void gfs2_quota_in(struct gfs2_quota *qu, char *buf)
{
	struct gfs2_quota *str = (struct gfs2_quota *)buf;

	CPIN_64(qu, str, qu_limit);
	CPIN_64(qu, str, qu_warn);
	CPIN_64(qu, str, qu_value);
	CPIN_08(qu, str, qu_reserved, sizeof(qu->qu_reserved));
}

void gfs2_quota_out(struct gfs2_quota *qu, char *buf)
{
	struct gfs2_quota *str = (struct gfs2_quota *)buf;

	CPOUT_64(qu, str, qu_limit);
	CPOUT_64(qu, str, qu_warn);
	CPOUT_64(qu, str, qu_value);
	memset(qu->qu_reserved, 0, sizeof(qu->qu_reserved));
}

void gfs2_quota_print(const struct gfs2_quota *qu)
{
	pv(qu, qu_limit, "%llu", "0x%llx");
	pv(qu, qu_warn, "%llu", "0x%llx");
	pv(qu, qu_value, "%lld", "0x%llx");
}

void gfs2_dinode_in(struct gfs2_dinode *di, char *buf)
{
	struct gfs2_dinode *str = (struct gfs2_dinode *)buf;

	gfs2_meta_header_in(&di->di_header, buf);
	gfs2_inum_in(&di->di_num, (char *)&str->di_num);

	CPIN_32(di, str, di_mode);
	CPIN_32(di, str, di_uid);
	CPIN_32(di, str, di_gid);
	CPIN_32(di, str, di_nlink);
	CPIN_64(di, str, di_size);
	CPIN_64(di, str, di_blocks);
	CPIN_64(di, str, di_atime);
	CPIN_64(di, str, di_mtime);
	CPIN_64(di, str, di_ctime);
	CPIN_32(di, str, di_major);
	CPIN_32(di, str, di_minor);

	CPIN_64(di, str, di_goal_meta);
	CPIN_64(di, str, di_goal_data);

	CPIN_32(di, str, di_flags);
	CPIN_32(di, str, di_payload_format);
	CPIN_16(di, str, __pad1);
	CPIN_16(di, str, di_height);

	CPIN_16(di, str, di_depth);
	CPIN_32(di, str, di_entries);

	CPIN_64(di, str, di_eattr);

	CPIN_08(di, str, di_reserved, 32);
}

void gfs2_dinode_out(struct gfs2_dinode *di, char *buf)
{
	struct gfs2_dinode *str = (struct gfs2_dinode *)buf;

	gfs2_meta_header_out(&di->di_header, buf);
	gfs2_inum_out(&di->di_num, (char *)&str->di_num);

	CPOUT_32(di, str, di_mode);
	CPOUT_32(di, str, di_uid);
	CPOUT_32(di, str, di_gid);
	CPOUT_32(di, str, di_nlink);
	CPOUT_64(di, str, di_size);
	CPOUT_64(di, str, di_blocks);
	CPOUT_64(di, str, di_atime);
	CPOUT_64(di, str, di_mtime);
	CPOUT_64(di, str, di_ctime);
	CPOUT_32(di, str, di_major);
	CPOUT_32(di, str, di_minor);

	CPOUT_64(di, str, di_goal_meta);
	CPOUT_64(di, str, di_goal_data);

	CPOUT_32(di, str, di_flags);
	CPOUT_32(di, str, di_payload_format);
	CPOUT_16(di, str, __pad1);
	CPOUT_16(di, str, di_height);

	CPOUT_16(di, str, di_depth);
	CPOUT_32(di, str, di_entries);

	CPOUT_64(di, str, di_eattr);

	CPOUT_08(di, str, di_reserved, 32);
}

void gfs2_dinode_print(const struct gfs2_dinode *di)
{
	gfs2_meta_header_print(&di->di_header);
	gfs2_inum_print(&di->di_num);

	pv(di, di_mode, "0%o", NULL);
	pv(di, di_uid, "%u", "0x%x");
	pv(di, di_gid, "%u", "0x%x");
	pv(di, di_nlink, "%u", "0x%x");
	pv(di, di_size, "%llu", "0x%llx");
	pv(di, di_blocks, "%llu", "0x%llx");
	pv(di, di_atime, "%lld", "0x%llx");
	pv(di, di_mtime, "%lld", "0x%llx");
	pv(di, di_ctime, "%lld", "0x%llx");
	pv(di, di_major, "%u", "0x%llx");
	pv(di, di_minor, "%u", "0x%llx");

	pv(di, di_goal_meta, "%llu", "0x%llx");
	pv(di, di_goal_data, "%llu", "0x%llx");

	pv(di, di_flags, "0x%.8X", NULL);
	pv(di, di_payload_format, "%u", "0x%x");
	pv(di, di_height, "%u", "0x%x");

	pv(di, di_depth, "%u", "0x%x");
	pv(di, di_entries, "%u", "0x%x");

	pv(di, di_eattr, "%llu", "0x%llx");
}

void gfs2_dirent_in(struct gfs2_dirent *de, char *buf)
{
	struct gfs2_dirent *str = (struct gfs2_dirent *)buf;

	gfs2_inum_in(&de->de_inum, buf);
	CPIN_32(de, str, de_hash);
	CPIN_16(de, str, de_rec_len);
	CPIN_16(de, str, de_name_len);
	CPIN_16(de, str, de_type);
#ifdef GFS2_HAS_DE_RAHEAD
	CPIN_16(de, str, de_rahead);
#ifdef GFS2_HAS_DE_COOKIE
	CPIN_32(de, str, de_cookie);
	CPIN_08(de, str, pad3, 8);
#else
	CPIN_08(de, str, pad2, 12);
#endif /* GFS2_HAS_DE_COOKIE */
#else
	CPIN_08(de, str, __pad, 14);
#endif /* GFS2_HAS_DE_RAHEAD */
}

void gfs2_dirent_out(struct gfs2_dirent *de, char *buf)
{
	struct gfs2_dirent *str = (struct gfs2_dirent *)buf;

	gfs2_inum_out(&de->de_inum, buf);
	CPOUT_32(de, str, de_hash);
	CPOUT_16(de, str, de_rec_len);
	CPOUT_16(de, str, de_name_len);
	CPOUT_16(de, str, de_type);
#ifdef GFS2_HAS_DE_RAHEAD
	CPOUT_16(de, str, de_rahead);
#ifdef GFS2_HAS_DE_COOKIE
	CPOUT_32(de, str, de_cookie);
	CPOUT_08(de, str, pad3, 8);
#else
	CPOUT_08(de, str, pad2, 12);
#endif /* GFS2_HAS_DE_COOKIE */
#else
	CPOUT_08(de, str, __pad, 14);
#endif /* GFS2_HAS_DE_RAHEAD */
}

void gfs2_leaf_in(struct gfs2_leaf *lf, char *buf)
{
	struct gfs2_leaf *str = (struct gfs2_leaf *)buf;

	gfs2_meta_header_in(&lf->lf_header, buf);
	CPIN_16(lf, str, lf_depth);
	CPIN_16(lf, str, lf_entries);
	CPIN_32(lf, str, lf_dirent_format);
	CPIN_64(lf, str, lf_next);
#ifdef GFS2_HAS_LEAF_HINTS
	CPIN_64(lf, str, lf_inode);
	CPIN_32(lf, str, lf_dist);
	CPIN_32(lf, str, lf_nsec);
	CPIN_64(lf, str, lf_sec);
	CPIN_08(lf, str, lf_reserved2, 40);
#else
	CPIN_08(lf, str, lf_reserved, 32);
#endif
}

void gfs2_leaf_out(struct gfs2_leaf *lf, char *buf)
{
	struct gfs2_leaf *str = (struct gfs2_leaf *)buf;

	gfs2_meta_header_out(&lf->lf_header, buf);
	CPOUT_16(lf, str, lf_depth);
	CPOUT_16(lf, str, lf_entries);
	CPOUT_32(lf, str, lf_dirent_format);
	CPOUT_64(lf, str, lf_next);
#ifdef GFS2_HAS_LEAF_HINTS
	CPOUT_64(lf, str, lf_inode);
	CPOUT_32(lf, str, lf_dist);
	CPOUT_32(lf, str, lf_nsec);
	CPOUT_64(lf, str, lf_sec);
	CPOUT_08(lf, str, lf_reserved2, 40);
#else
	CPOUT_08(lf, str, lf_reserved, 64);
#endif
}

void gfs2_leaf_print(const struct gfs2_leaf *lf)
{
	gfs2_meta_header_print(&lf->lf_header);
	pv(lf, lf_depth, "%u", "0x%x");
	pv(lf, lf_entries, "%u", "0x%x");
	pv(lf, lf_dirent_format, "%u", "0x%x");
	pv(lf, lf_next, "%llu", "0x%llx");
#ifdef GFS2_HAS_LEAF_HINTS
	pv(lf, lf_inode, "%llu", "0x%llx");
	pv(lf, lf_dist, "%u", "0x%x");
	pv(lf, lf_nsec, "%u", "0x%x");
	pv(lf, lf_sec, "%llu", "0x%llx");
#endif
}

void gfs2_ea_header_in(struct gfs2_ea_header *ea, char *buf)
{
	struct gfs2_ea_header *str = (struct gfs2_ea_header *)buf;

	CPIN_32(ea, str, ea_rec_len);
	CPIN_32(ea, str, ea_data_len);
	ea->ea_name_len = str->ea_name_len;
	ea->ea_type = str->ea_type;
	ea->ea_flags = str->ea_flags;
	ea->ea_num_ptrs = str->ea_num_ptrs;
}

void gfs2_ea_header_print(const struct gfs2_ea_header *ea, char *name)
{
	char buf[GFS2_EA_MAX_NAME_LEN + 1];

	pv(ea, ea_rec_len, "%u", "0x%x");
	pv(ea, ea_data_len, "%u", "0x%x");
	pv(ea, ea_name_len, "%u", "0x%x");
	pv(ea, ea_type, "%u", "0x%x");
	pv(ea, ea_flags, "%u", "0x%x");
	pv(ea, ea_num_ptrs, "%u", "0x%x");

	memset(buf, 0, GFS2_EA_MAX_NAME_LEN + 1);
	memcpy(buf, name, ea->ea_name_len);
	print_it("  name", "%s", NULL, buf);
}

void gfs2_log_header_v1_in(struct gfs2_log_header *lh, char *buf)
{
	struct gfs2_log_header *str = (struct gfs2_log_header *)buf;

	gfs2_meta_header_in(&lh->lh_header, buf);
	CPIN_64(lh, str, lh_sequence);
	CPIN_32(lh, str, lh_flags);
	CPIN_32(lh, str, lh_tail);
	CPIN_32(lh, str, lh_blkno);
	CPIN_32(lh, str, lh_hash);
}

void gfs2_log_header_in(struct gfs2_log_header *lh, char *buf)
{
	gfs2_log_header_v1_in(lh, buf);
#ifdef GFS2_HAS_LH_V2
	{
	struct gfs2_log_header *str = (struct gfs2_log_header *)buf;

	CPIN_32(lh, str, lh_crc);
	CPIN_32(lh, str, lh_nsec);
	CPIN_64(lh, str, lh_sec);
	CPIN_64(lh, str, lh_addr);
	CPIN_64(lh, str, lh_jinode);
	CPIN_64(lh, str, lh_statfs_addr);
	CPIN_64(lh, str, lh_quota_addr);
	CPIN_64(lh, str, lh_local_total);
	CPIN_64(lh, str, lh_local_free);
	CPIN_64(lh, str, lh_local_dinodes);
	}
#endif
}

void gfs2_log_header_v1_out(struct gfs2_log_header *lh, char *buf)
{
	struct gfs2_log_header *str = (struct gfs2_log_header *)buf;

	gfs2_meta_header_out(&lh->lh_header, buf);
	CPOUT_64(lh, str, lh_sequence);
	CPOUT_32(lh, str, lh_flags);
	CPOUT_32(lh, str, lh_tail);
	CPOUT_32(lh, str, lh_blkno);
	CPOUT_32(lh, str, lh_hash);
}

void gfs2_log_header_out(struct gfs2_log_header *lh, char *buf)
{
	gfs2_log_header_v1_out(lh, buf);
#ifdef GFS2_HAS_LH_V2
	{
	struct gfs2_log_header *str = (struct gfs2_log_header *)buf;

	CPOUT_32(lh, str, lh_crc);
	CPOUT_32(lh, str, lh_nsec);
	CPOUT_64(lh, str, lh_sec);
	CPOUT_64(lh, str, lh_addr);
	CPOUT_64(lh, str, lh_jinode);
	CPOUT_64(lh, str, lh_statfs_addr);
	CPOUT_64(lh, str, lh_quota_addr);
	CPOUT_64(lh, str, lh_local_total);
	CPOUT_64(lh, str, lh_local_free);
	CPOUT_64(lh, str, lh_local_dinodes);
	}
#endif
}

void gfs2_log_header_v1_print(const struct gfs2_log_header *lh)
{
	gfs2_meta_header_print(&lh->lh_header);
	pv(lh, lh_sequence, "%llu", "0x%llx");
	pv(lh, lh_flags, "0x%.8X", NULL);
	pv(lh, lh_tail, "%u", "0x%x");
	pv(lh, lh_blkno, "%u", "0x%x");
	pv(lh, lh_hash, "0x%.8X", NULL);
}

void gfs2_log_header_print(const struct gfs2_log_header *lh)
{
	gfs2_log_header_v1_print(lh);
#ifdef GFS2_HAS_LH_V2
	pv(lh, lh_crc, "0x%.8X", NULL);
	pv(lh, lh_nsec, "%u", "0x%x");
	pv(lh, lh_sec, "%llu", "0x%llx");
	pv(lh, lh_addr, "%llu", "0x%llx");
	pv(lh, lh_jinode, "%llu", "0x%llx");
	pv(lh, lh_statfs_addr, "%llu", "0x%llx");
	pv(lh, lh_quota_addr, "%llu", "0x%llx");
	pv(lh, lh_local_total, "%lld", "0x%llx");
	pv(lh, lh_local_free, "%lld", "0x%llx");
	pv(lh, lh_local_dinodes, "%lld", "0x%llx");
#endif
}

void gfs2_log_descriptor_in(struct gfs2_log_descriptor *ld, char *buf)
{
	struct gfs2_log_descriptor *str = (struct gfs2_log_descriptor *)buf;

	gfs2_meta_header_in(&ld->ld_header, buf);
	CPIN_32(ld, str, ld_type);
	CPIN_32(ld, str, ld_length);
	CPIN_32(ld, str, ld_data1);
	CPIN_32(ld, str, ld_data2);

	CPIN_08(ld, str, ld_reserved, 32);
}

void gfs2_log_descriptor_out(struct gfs2_log_descriptor *ld, char *buf)
{
	struct gfs2_log_descriptor *str = (struct gfs2_log_descriptor *)buf;

	gfs2_meta_header_out(&ld->ld_header, buf);
	CPOUT_32(ld, str, ld_type);
	CPOUT_32(ld, str, ld_length);
	CPOUT_32(ld, str, ld_data1);
	CPOUT_32(ld, str, ld_data2);

	CPOUT_08(ld, str, ld_reserved, 32);
}

void gfs2_log_descriptor_print(const struct gfs2_log_descriptor *ld)
{
	gfs2_meta_header_print(&ld->ld_header);
	pv(ld, ld_type, "%u", "0x%x");
	pv(ld, ld_length, "%u", "0x%x");
	pv(ld, ld_data1, "%u", "0x%x");
	pv(ld, ld_data2, "%u", "0x%x");
}

void gfs2_statfs_change_in(struct gfs2_statfs_change *sc, char *buf)
{
	struct gfs2_statfs_change *str = (struct gfs2_statfs_change *)buf;

	CPIN_64(sc, str, sc_total);
	CPIN_64(sc, str, sc_free);
	CPIN_64(sc, str, sc_dinodes);
}

void gfs2_statfs_change_out(struct gfs2_statfs_change *sc, char *buf)
{
	struct gfs2_statfs_change *str = (struct gfs2_statfs_change *)buf;

	CPOUT_64(sc, str, sc_total);
	CPOUT_64(sc, str, sc_free);
	CPOUT_64(sc, str, sc_dinodes);
}

void gfs2_statfs_change_print(const struct gfs2_statfs_change *sc)
{
	pv(sc, sc_total, "%lld", "0x%llx");
	pv(sc, sc_free, "%lld", "0x%llx");
	pv(sc, sc_dinodes, "%lld", "0x%llx");
}

void gfs2_quota_change_in(struct gfs2_quota_change *qc, char *buf)
{
	struct gfs2_quota_change *str = (struct gfs2_quota_change *)(buf +
	                                 sizeof(struct gfs2_meta_header));

	CPIN_64(qc, str, qc_change);
	CPIN_32(qc, str, qc_flags);
	CPIN_32(qc, str, qc_id);
}

void gfs2_quota_change_out(struct gfs2_quota_change *qc, char *buf)
{
	struct gfs2_quota_change *str = (struct gfs2_quota_change *)(buf +
	                                 sizeof(struct gfs2_meta_header));

	CPOUT_64(qc, str, qc_change);
	CPOUT_32(qc, str, qc_flags);
	CPOUT_32(qc, str, qc_id);
}

void gfs2_quota_change_print(const struct gfs2_quota_change *qc)
{
	pv(qc, qc_change, "%lld", "0x%llx");
	pv(qc, qc_flags, "0x%.8X", NULL);
	pv(qc, qc_id, "%u", "0x%x");
}

