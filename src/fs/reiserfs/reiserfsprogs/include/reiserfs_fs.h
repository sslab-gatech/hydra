/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

/*
 *  Reiser File System constants and structures
 */

/* in reading the #defines, it may help to understand that they employ the
   following abbreviations:

   B = Buffer
   I = Item header
   H = Height within the tree (should be changed to LEV)
   N = Number of the item in the node
   STAT = stat data
   DEH = Directory Entry Header
   EC = Entry Count
   E = Entry number
   UL = Unsigned Long
   BLKH = BLocK Header
   UNFM = UNForMatted node
   DC = Disk Child
   P = Path

   These #defines are named by concatenating these abbreviations, where first
   comes the arguments, and last comes the return value, of the macro.

*/

#ifndef REISERFSPROGS_FS_H
#define REISERFSPROGS_FS_H

#include <linux/types.h>
#include "swab.h"

typedef __u16 __bitwise __le16;
typedef __u32 __bitwise __le32;
typedef __u64 __bitwise __le64;

typedef unsigned int blocknr_t;

#ifndef get_unaligned
#define get_unaligned(ptr)				\
({							\
        __typeof__(*(ptr)) __tmp;			\
        memcpy(&__tmp, (ptr), sizeof(*(ptr)));		\
        __tmp;						\
})
#endif

#ifndef put_unaligned
#define put_unaligned(val, ptr)				\
({							\
        __typeof__(*(ptr)) __tmp = (val);		\
        memcpy((ptr), &__tmp, sizeof(*(ptr)));		\
        (void)0;					\
})
#endif

#define get_leXX(xx,p,field)	(le##xx##_to_cpu ((p)->field))
#define set_leXX(xx,p,field,val) do { (p)->field = cpu_to_le##xx(val); } while (0)

#define get_le16(p,field)	get_leXX (16, p, field)
#define set_le16(p,field,val)	set_leXX (16, p, field, val)

#define get_le32(p,field)	get_leXX (32, p, field)
#define set_le32(p,field,val)	set_leXX (32, p, field, val)

#define get_le64(p,field)	get_leXX (64, p, field)
#define set_le64(p,field,val)	set_leXX (64, p, field, val)

/***************************************************************************/
/*                             SUPER BLOCK                                 */
/***************************************************************************/

#define UNSET_HASH 0		// read_super will guess about, what hash names
		     // in directories were sorted with
#define TEA_HASH  1
#define YURA_HASH 2
#define R5_HASH   3
#define DEFAULT_HASH R5_HASH

/* super block of prejournalled version */
struct reiserfs_super_block_v0 {
	__le32 s_block_count;
	__le32 s_free_blocks;
	__le32 s_root_block;
	__le16 s_blocksize;
	__le16 s_oid_maxsize;
	__le16 s_oid_cursize;
	__le16 s_state;
	char s_magic[16];
	__le16 s_tree_height;
	__le16 s_bmap_nr;
	__le16 s_reserved;
};

struct journal_params {
	__le32 jp_journal_1st_block;	/* where does journal start from on its
					   device */
	__le32 jp_journal_dev;	/* journal device st_rdev */
	__le32 jp_journal_size;	/* size of the journal on FS creation. used to
				   make sure they don't overflow it */
	__le32 jp_journal_trans_max;	/* max number of blocks in a transaction.  */
	__le32 jp_journal_magic;	/* random value made on fs creation (this was
				   sb_journal_block_count) */
	__le32 jp_journal_max_batch;	/* max number of blocks to batch into a trans */
	__le32 jp_journal_max_commit_age;	/* in seconds, how old can an async commit be */
	__le32 jp_journal_max_trans_age;	/* in seconds, how old can a transaction be */
};

#define get_jp_journal_1st_block(jp)	 get_le32 (jp, jp_journal_1st_block)
#define set_jp_journal_1st_block(jp,val) set_le32 (jp, jp_journal_1st_block, val)

#define get_jp_journal_dev(jp)		get_le32 (jp, jp_journal_dev)
#define set_jp_journal_dev(jp,val)	set_le32 (jp, jp_journal_dev, val)

#define get_jp_journal_size(jp)		get_le32 (jp, jp_journal_size)
#define set_jp_journal_size(jp,val)	set_le32 (jp, jp_journal_size, val)

#define get_jp_journal_max_trans_len(jp)	get_le32 (jp, jp_journal_trans_max)
#define set_jp_journal_max_trans_len(jp,val)	set_le32 (jp, jp_journal_trans_max, val)

#define get_jp_journal_magic(jp)	get_le32 (jp, jp_journal_magic)
#define set_jp_journal_magic(jp,val)	set_le32 (jp, jp_journal_magic, val)

#define NEED_TUNE 0xffffffff

#define get_jp_journal_max_batch(jp)	get_le32 (jp, jp_journal_max_batch)
#define set_jp_journal_max_batch(jp,val)	set_le32 (jp, jp_journal_max_batch, val)

#define get_jp_journal_max_commit_age(jp)	get_le32 (jp, jp_journal_max_commit_age)
#define set_jp_journal_max_commit_age(jp,val)	set_le32 (jp, jp_journal_max_commit_age, val)

#define get_jp_journal_max_trans_age(jp)	get_le32 (jp, jp_journal_max_commit_age)
#define set_jp_journal_max_trans_age(jp,val)	set_le32 (jp, jp_journal_max_commit_age, val)

/* this is the super from 3.5.X */
struct reiserfs_super_block_v1 {
	__le32 sb_block_count;	/* 0 number of block on data device */
	__le32 sb_free_blocks;	/* 4 free blocks count */
	__le32 sb_root_block;	/* 8 root of the tree */

	struct journal_params sb_journal;	/* 12 */

	__le16 sb_blocksize;	/* 44 */
	__le16 sb_oid_maxsize;	/* 46 max size of object id array, see
				   get_objectid() commentary */
	__le16 sb_oid_cursize;	/* 48 current size of object id array */
	__le16 sb_umount_state;	/* 50 this is set to 1 when filesystem was
				   umounted, to 2 - when not */

	char s_magic[10];	/* 52 reiserfs magic string indicates that
				   file system is reiserfs: "ReIsErFs" or
				   "ReIsEr2Fs" or "ReIsEr3Fs" */
	__le16 sb_fs_state;	/* 62 it is set to used by fsck to mark which phase of
				   rebuilding is done (used for fsck debugging) */
	__le32 sb_hash_function_code;	/* 64 code of fuction which was/is/will be
					   used to sort names in a directory. See
					   codes in above */
	__le16 sb_tree_height;	/* 68 height of filesytem tree. Tree
				   consisting of only one root block has 2
				   here */
	__le16 sb_bmap_nr;	/* 70 amount of bitmap blocks needed to
				   address each block of file system */
	__le16 sb_version;	/* 72 this field is only reliable on
				   filesystem with non-standard journal */
	__le16 sb_reserved_for_journal;	/* 74 size in blocks of journal area on
					   main device, we need to keep after
					   non-standard journal relocation */
};

#define SB_SIZE_V1 (sizeof(struct reiserfs_super_block_v1))	/* 76 bytes */

#define sb_jp(sb) (&((sb)->s_v1.sb_journal))

/* values for sb_version field of struct reiserfs_super_block. sb_version is
   only reliable on filesystem with non-standard journal */
#define REISERFS_FORMAT_3_5 0
#define REISERFS_FORMAT_3_6 2
#define REISERFS_FORMAT_UNKNOWN -1

/* values for sb_mount_state field */
#define FS_CLEANLY_UMOUNTED    1	/* this was REISERFS_VALID_FS */
#define FS_NOT_CLEANLY_UMOUNTED    2	/* this was REISERFS_ERROR. It
					   means that filesystem was not
					   cleanly unmounted */

/* Structure of super block on disk */
struct reiserfs_super_block {
/*  0 */ struct reiserfs_super_block_v1 s_v1;
/* 76 */ __le32 sb_inode_generation;
				/* 80 */ __le32 s_flags;
				/* Right now used only by inode-attributes, if enabled */
						/* 84 */ unsigned char s_uuid[16];
						/* filesystem unique identifier */
					/*100 */ char s_label[16];
					/* filesystem volume label */
/*116 */ __le16 s_mnt_count;
/*118 */ __le16 s_max_mnt_count;
/*120 */ __le32 s_lastcheck;
/*124 */ __le32 s_check_interval;
					/*128 */ char s_unused[76];
					/* zero filled by mkreiserfs and reiserfs_convert_objectid_map_v1()
					 * so any additions must be updated there as well. */
/*204*/
} __attribute__ ((__packed__));;

typedef enum {
	reiserfs_attrs_cleared = 0x00000001,
} reiserfs_super_block_flags;

#define SB_SIZE (sizeof(struct reiserfs_super_block))	/* 204 bytes */

/* set/get fields of super block with these defines */
#define get_sb_block_count(sb)		get_le32 (sb, s_v1.sb_block_count)
#define set_sb_block_count(sb,val)	set_le32 (sb, s_v1.sb_block_count, val)

#define get_sb_free_blocks(sb)		get_le32 (sb, s_v1.sb_free_blocks)
#define set_sb_free_blocks(sb,val)	set_le32 (sb, s_v1.sb_free_blocks, val)

#define get_sb_root_block(sb)		get_le32 (sb,s_v1.sb_root_block)
#define set_sb_root_block(sb,val)	set_le32 (sb, s_v1.sb_root_block, val)

#if 0
#define get_sb_mount_id(sb)		get_le32 (sb,s_v1.sb_mountid)
#define set_sb_mount_id(sb,val)		set_le32 (sb, s_v1.sb_mountid, val)

#define get_sb_journal_magic(sb)	get_le32 (sb, s_v1.sb_journal_magic)
#define set_sb_journal_magic(sb,val)	set_le32 (sb, s_v1.sb_journal_magic, val)
#endif

#define get_sb_block_size(sb)		get_le16 (sb, s_v1.sb_blocksize)
#define set_sb_block_size(sb,val)	set_le16 (sb, s_v1.sb_blocksize, val)

#define get_sb_oid_maxsize(sb)		get_le16 (sb, s_v1.sb_oid_maxsize)
#define set_sb_oid_maxsize(sb,val)	set_le16 (sb, s_v1.sb_oid_maxsize, val)

#define get_sb_oid_cursize(sb)		get_le16 (sb, s_v1.sb_oid_cursize)
#define set_sb_oid_cursize(sb,val)	set_le16 (sb, s_v1.sb_oid_cursize, val)

#define get_sb_umount_state(sb)		get_le16 (sb, s_v1.sb_umount_state)
#define set_sb_umount_state(sb,val)	set_le16 (sb, s_v1.sb_umount_state, val)

#define get_sb_fs_state(sb)		get_le16 (sb, s_v1.sb_fs_state)
#define set_sb_fs_state(sb,flag)	set_le16 (sb, s_v1.sb_fs_state, flag)

#define get_sb_hash_code(sb)		get_le32 (sb, s_v1.sb_hash_function_code)
#define set_sb_hash_code(sb,val)	set_le32 (sb, s_v1.sb_hash_function_code, val)

#define get_sb_tree_height(sb)		get_le16 (sb, s_v1.sb_tree_height)
#define set_sb_tree_height(sb,val)	set_le16 (sb, s_v1.sb_tree_height, val)

#define get_sb_bmap_nr(sb)		get_le16 (sb, s_v1.sb_bmap_nr)
#define set_sb_bmap_nr(sb,val)		set_le16 (sb, s_v1.sb_bmap_nr, val)

#define get_sb_version(sb)		get_le16 (sb, s_v1.sb_version)
#define set_sb_version(sb,val)		set_le16 (sb, s_v1.sb_version, val)

#define get_sb_reserved_for_journal(sb)		get_le16 (sb, s_v1.sb_reserved_for_journal)
#define set_sb_reserved_for_journal(sb,val)	set_le16 (sb, s_v1.sb_reserved_for_journal, val)

#define get_sb_v2_inode_generation(sb)		get_le32 (sb, sb_inode_generation)
#define set_sb_v2_inode_generation(sb,val)	set_le32 (sb, sb_inode_generation, val)

#define get_sb_v2_mnt_count(sb)		get_le16 (sb, s_mnt_count)
#define set_sb_v2_mnt_count(sb,val)	set_le16 (sb, s_mnt_count, val)

#define get_sb_v2_max_mnt_count(sb)				\
	get_le16 (sb, s_max_mnt_count)
#define set_sb_v2_max_mnt_count(sb,val)			\
	set_le16 (sb, s_max_mnt_count, val)

#define get_sb_v2_lastcheck(sb)		get_le32 (sb, s_lastcheck)
#define set_sb_v2_lastcheck(sb,val)		set_le32 (sb, s_lastcheck, val)

#define get_sb_v2_check_interval(sb)				\
	get_le32 (sb, s_check_interval)
#define set_sb_v2_check_interval(sb,val)			\
	set_le32 (sb, s_check_interval, val)

#define get_sb_v2_flags(sb)		get_le32 (sb, s_flags)
#define set_sb_v2_flags(sb, val)	set_le32 (sb, s_flags, val)

#define get_sb_v2_flag(sb, flag)       (get_le32 (sb, s_flags) & flag)
#define set_sb_v2_flag(sb, flag)	set_le32 (sb, s_flags, get_le32 (sb, s_flags) | flag)
#define clear_sb_v2_flag(sb, flag)	set_le32 (sb, s_flags, get_le32 (sb, s_flags) & ~(flag))

/*
#define journal_is_relocated(sb)        get_jp_journal_dev(sb_jp (sb))
*/

#define DEFAULT_MAX_MNT_COUNT	30	/* 30 mounts */
#define DEFAULT_CHECK_INTERVAL	(180 * 60 * 60 * 24)	/* 180 days */

/* these are possible values for sb_fs_state */
#define FS_CONSISTENT   0x0	/* this is set by mkreiserfs and by reiserfsck */
#define FS_ERROR	0x1	/* this is set by the kernel when fsck is wanted. */
#define FS_FATAL	0x2	/* this is set by fsck when fatal corruption is found */
#define IO_ERROR	0x4	/* this is set by kernel when io error occures */
#define PASS_0_DONE     0xfa02	/* set by fsck when pass-by-pass (-d),
				   FS_FATAL flag included */
#define PASS_1_DONE     0xfb02	/* set by fsck when pass-by-pass (-d),
				   FS_FATAL flag included */
#define TREE_IS_BUILT   0xfc02	/* set by fsck when pass-by-pass (-d),
				   FS_FATAL flag included */
#define SEMANTIC_DONE   0xfd02	/* set by fsck when pass-by-pass (-d),
				   FS_FATAL flag included */
#define LOST_FOUND_DONE 0xfe02	/* set by fsck when pass-by-pass (-d),
				   FS_FATAL flag included */

/* struct stat_data* access macros */
/* v1 */
#define sd_v1_mode(sd)                 (le16_to_cpu((sd)->sd_mode))
#define set_sd_v1_mode(sd,n)           ((sd)->sd_mode = cpu_to_le16((n)))
#define sd_v1_nlink(sd)                (le16_to_cpu((sd)->sd_nlink))
#define set_sd_v1_nlink(sd,n)          ((sd)->sd_nlink = cpu_to_le16((n)))
#define sd_v1_uid(sd)                  (le16_to_cpu((sd)->sd_uid))
#define set_sd_v1_uid(sd,n)            ((sd)->sd_uid = cpu_to_le16((n)))
#define sd_v1_gid(sd)                  (le16_to_cpu((sd)->sd_gid))
#define set_sd_v1_gid(sd,n)            ((sd)->sd_gid = cpu_to_le16((n)))
#define sd_v1_size(sd)                 (le32_to_cpu((sd)->sd_size))
#define set_sd_v1_size(sd,n)           ((sd)->sd_size = cpu_to_le32((n)))
#define sd_v1_atime(sd)                (le32_to_cpu((sd)->sd_atime))
#define set_sd_v1_atime(sd,n)          ((sd)->sd_atime = cpu_to_le32((n)))
#define sd_v1_mtime(sd)                (le32_to_cpu((sd)->sd_mtime))
#define set_sd_v1_mtime(sd,n)          ((sd)->sd_mtime = cpu_to_le32((n)))
#define sd_v1_ctime(sd)                (le32_to_cpu((sd)->sd_ctime))
#define set_sd_v1_ctime(sd,n)          ((sd)->sd_ctime = cpu_to_le32((n)))
#define sd_v1_blocks(sd)               (le32_to_cpu((sd)->u.sd_blocks))
#define set_sd_v1_blocks(sd,n)         ((sd)->u.sd_blocks = cpu_to_le32((n)))
#define sd_v1_rdev(sd)                 (le32_to_cpu((sd)->u.sd_rdev))
#define set_sd_v1_rdev(sd,n)           ((sd)->u.sd_rdev = cpu_to_le32((n)))
#define sd_v1_first_direct_byte(sd)    (le32_to_cpu((sd)->sd_first_direct_byte))
#define set_sd_v1_first_direct_byte(sd,n) \
                                 ((sd)->sd_first_direct_byte = cpu_to_le32((n)))
/* v2 */
#define sd_v2_mode(sd)                 (le16_to_cpu((sd)->sd_mode))
#define set_sd_v2_mode(sd,n)           ((sd)->sd_mode = cpu_to_le16((n)))
#define sd_v2_sd_attrs(sd)             (le16_to_cpu((sd)->sd_attrs))
#define set_sd_v2_sd_attrs(sd,n)       ((sd)->sd_attrs = cpu_to_le16((n)))
#define sd_v2_nlink(sd)                (le32_to_cpu((sd)->sd_nlink))
#define set_sd_v2_nlink(sd,n)          ((sd)->sd_nlink = cpu_to_le32((n)))
#define sd_v2_size(sd)                 (le64_to_cpu((sd)->sd_size))
#define set_sd_v2_size(sd,n)           ((sd)->sd_size = cpu_to_le64((n)))
#define sd_v2_uid(sd)                  (le32_to_cpu((sd)->sd_uid))
#define set_sd_v2_uid(sd,n)            ((sd)->sd_uid = cpu_to_le32((n)))
#define sd_v2_gid(sd)                  (le32_to_cpu((sd)->sd_gid))
#define set_sd_v2_gid(sd,n)            ((sd)->sd_gid = cpu_to_le32((n)))
#define sd_v2_atime(sd)                (le32_to_cpu((sd)->sd_atime))
#define set_sd_v2_atime(sd,n)          ((sd)->sd_atime = cpu_to_le32((n)))
#define sd_v2_mtime(sd)                (le32_to_cpu((sd)->sd_mtime))
#define set_sd_v2_mtime(sd,n)          ((sd)->sd_mtime = cpu_to_le32((n)))
#define sd_v2_ctime(sd)                (le32_to_cpu((sd)->sd_ctime))
#define set_sd_v2_ctime(sd,n)          ((sd)->sd_ctime = cpu_to_le32((n)))
#define sd_v2_blocks(sd)               (le32_to_cpu((sd)->sd_blocks))
#define set_sd_v2_blocks(sd,n)         ((sd)->sd_blocks = cpu_to_le32((n)))
#define sd_v2_rdev(sd)                 (le32_to_cpu((sd)->u.sd_rdev))
#define set_sd_v2_rdev(sd,n)           ((sd)->u.sd_rdev = cpu_to_le32((n)))

/* ReiserFS leaves the first 64k unused, so that partition labels have enough
   space.  If someone wants to write a fancy bootloader that needs more than
   64k, let us know, and this will be increased in size.  This number must be
   larger than than the largest block size on any platform, or code will
   break.  -Hans */
#define REISERFS_DISK_OFFSET_IN_BYTES (64 * 1024)

/*#define MD_RAID_SUPERBLOCKS_IN_BYTES (128 * 1024)*/

/* the spot for the super in versions 3.5 - 3.5.11 (inclusive) */
#define REISERFS_OLD_DISK_OFFSET_IN_BYTES (8 * 1024)

/* prejournalled reiserfs had signature in the other place in super block */
#define REISERFS_SUPER_MAGIC_STRING_OFFSET_NJ 20

/* f_type of struct statfs will be set at this value by statfs(2) */
#define REISERFS_SUPER_MAGIC 0x52654973

/* various reiserfs signatures. We have 3 so far. ReIsErFs for 3.5 format with
   standard journal, ReIsEr2Fs for 3.6 (or converted 3.5) and ReIsEr3Fs for
   filesystem with non-standard journal (formats are distinguished by
   sb_version in that case). Those signatures should be looked for at the
   64-th and at the 8-th 1k block of the device */
#define REISERFS_3_5_SUPER_MAGIC_STRING "ReIsErFs"
#define REISERFS_3_6_SUPER_MAGIC_STRING "ReIsEr2Fs"
#define REISERFS_JR_SUPER_MAGIC_STRING "ReIsEr3Fs"	/* JR stands for Journal
							   Relocation */

#define get_reiserfs_ondisk_offset(block_of_super_block, block_size) \
    (block_of_super_block * block_size)

#define is_new_sb_location(block_of_super_block, block_size) \
    ((get_reiserfs_ondisk_offset(block_of_super_block, block_size) == REISERFS_DISK_OFFSET_IN_BYTES) \
	? 1 : 0)

/*only 4k blocks for old location*/
#define is_old_sb_location(block_of_super_block, block_size) \
    ((get_reiserfs_ondisk_offset(block_of_super_block, 4096) == REISERFS_OLD_DISK_OFFSET_IN_BYTES) \
	? 1 : 0)

/***************************************************************************/
/*                             JOURNAL                                     */
/***************************************************************************/

#define JOURNAL_DESC_MAGIC "ReIsErLB"	/* ick.  magic string to find desc
					   blocks in the journal */

/* journal.c see journal.c for all the comments here */

//#define JOURNAL_TRANS_HALF 1018   /* must be correct to keep the desc and commit structs at 4k */

/* first block written in a commit.  BUG, not 64bit safe */
struct reiserfs_journal_desc {
	__le32 j2_trans_id;	/* id of commit */
	__le32 j2_len;		/* length of commit. len +1 is the commit block */
	__le32 j2_mount_id;	/* mount id of this trans */
	__le32 j2_realblock[1];	/* real locations for each block */
};

#define get_jd_magic(bh) (bh->b_data + bh->b_size - 12)

#define journal_trans_half(blocksize) \
((blocksize - sizeof (struct reiserfs_journal_desc) + sizeof (__le32) - 12) / sizeof (__le32))

#define jdesc_header(bh) ((struct reiserfs_journal_desc *)bh->b_data)

#define get_desc_trans_id(bh)	  get_le32 (jdesc_header (bh), j2_trans_id)
#define set_desc_trans_id(bh,val)	  set_le32 (jdesc_header (bh), j2_trans_id, val)

#define get_desc_trans_len(bh)	          get_le32 (jdesc_header (bh), j2_len)
#define set_desc_trans_len(bh,val)	  set_le32 (jdesc_header (bh), j2_len, val)

#define get_desc_mount_id(bh)	  get_le32 (jdesc_header (bh), j2_mount_id)
#define set_desc_mount_id(bh,val)	  set_le32 (jdesc_header (bh), j2_mount_id, val)

/* last block written in a commit BUG, not 64bit safe */
struct reiserfs_journal_commit {
	__le32 j3_trans_id;	/* must match j_trans_id from the desc block */
	__le32 j3_len;		/* ditto */
	__le32 j3_realblock[1];	/* real locations for each block */
};

#define jcommit_header(bh) ((struct reiserfs_journal_commit *)bh->b_data)

#define get_commit_trans_id(bh)		get_le32 (jcommit_header(bh), j3_trans_id)
#define set_commit_trans_id(bh,val)	set_le32 (jcommit_header(bh), j3_trans_id, val)

#define get_commit_trans_len(bh)	get_le32 (jcommit_header(bh), j3_len)
#define set_comm_trans_len(bh,val)	set_le32 (jcommit_header(bh), j3_len, val)

/* this header block gets written whenever a transaction is considered fully
** flushed, and is more recent than the last fully flushed transaction.  fully
** flushed means all the log blocks and all the real blocks are on disk, and
** this transaction does not need to be replayed.  */
struct reiserfs_journal_header {
	__le32 jh_last_flush_trans_id;	/* id of last fully flushed transaction */
	__le32 jh_first_unflushed_offset;	/* offset in the log of where to start replay after a crash */
	__le32 jh_mount_id;

	struct journal_params jh_journal;

	__le32 jh_last_check_mount_id;	/* the mount id of the fs during the last reiserfsck --check. */
};

/* set/get fields of journal header with these defines */
#define get_jh_mount_id(jh)	get_le32 (jh, jh_mount_id)
#define set_jh_mount_id(jh,val)	set_le32 (jh, jh_mount_id, val)

#define get_jh_last_flushed(jh)		get_le32 (jh, jh_last_flush_trans_id)
#define set_jh_last_flushed(jh,val)	set_le32 (jh, jh_last_flush_trans_id, val)

#define get_jh_replay_start_offset(jh)		get_le32 (jh, jh_first_unflushed_offset)
#define set_jh_replay_start_offset(jh,val)	set_le32 (jh, jh_first_unflushed_offset, val)

/* journal default settings */

#define JOURNAL_MIN_SIZE 	512
#define JOURNAL_TRANS_MAX 	1024	/* biggest possible single transaction, don't
					   change for now (8/3/99) */
#define JOURNAL_TRANS_MIN 	256	/* need to check whether it works */
#define JOURNAL_DEFAULT_RATIO 	8	/* default journal size / max trans length */
#define JOURNAL_MIN_RATIO 	2
#define JOURNAL_MAX_BATCH   	900	/* max blocks to batch into one transaction,
					   don't make this any bigger than 900 */
#define JOURNAL_MAX_COMMIT_AGE 	30
#define JOURNAL_MAX_TRANS_AGE 	30

/* journal max size is a maximum number of blocks pointed by first bitmap -
   REISERFS_DISK_OFFSET - superblock - first bitmap - journal herader */
#define journal_max_size(block_of_super_block,blocksize) \
	blocksize * 8 - (block_of_super_block + 1 + 1 + 1)

#define journal_default_size(block_of_super_block,blocksize) \
	(unsigned long long)((8192 > journal_max_size (block_of_super_block,blocksize)) ? \
		journal_max_size (block_of_super_block,blocksize) : 8192)

int reiserfs_replay_journal(reiserfs_filsys_t fs);

//#define JOURNAL_DEFAULT_SIZE 8192  number of blocks in the journal
//#define JOURNAL_DEFAULT_SIZE_FOR_BS_1024 8125  number of blocks in the journal for block size 1KB

#define bh_desc(bh) ((struct reiserfs_journal_desc *)((bh)->b_data))
#define bh_commit(bh) ((struct reiserfs_journal_commit *)((bh)->b_data))

/***************************************************************************/
/*                       KEY & ITEM HEAD                                   */
/***************************************************************************/

struct offset_v1 {
	__le32 k_offset;
	__le32 k_uniqueness;
} __attribute__ ((__packed__));

/*
 * little endian structure:
 * bits 0-59 [60]: offset
 * bits 60-63 [4]: type
 */
struct offset_v2 {
	__le64 v;
} __attribute__ ((__packed__));

/* Key of the object determines object's location in the tree, composed of 4 components */
struct reiserfs_key {
	__le32 k2_dir_id;	/* packing locality: by default parent directory object id */
	__le32 k2_objectid;	/* object identifier */
	union {
		struct offset_v1 k2_offset_v1;
		struct offset_v2 k2_offset_v2;
	} __attribute__ ((__packed__)) u;
} __attribute__ ((__packed__));

extern const struct reiserfs_key MIN_KEY, MAX_KEY;

/* set/get fields of keys on disk with these defines */
#define get_key_dirid(k)		get_le32 (k, k2_dir_id)
#define set_key_dirid(k,val)		set_le32 (k, k2_dir_id, val)

#define get_key_objectid(k)		get_le32 (k, k2_objectid)
#define set_key_objectid(k,val)		set_le32 (k, k2_objectid, val)

#define get_key_offset_v1(k)		get_le32 (k, u.k2_offset_v1.k_offset)
#define set_key_offset_v1(k,val)	set_le32 (k, u.k2_offset_v1.k_offset, val)

#define get_key_uniqueness(k)		get_le32 (k, u.k2_offset_v1.k_uniqueness)
#define set_key_uniqueness(k,val)	set_le32 (k, u.k2_offset_v1.k_uniqueness, val)

/*
#define get_key_offset_v2(k)		get_le64 (k, u.k2_offset_v2.k_offset)
#define set_key_offset_v2(k,val)	set_le64 (k, u.k2_offset_v2.k_offset, val)
*/
/* ??? */
/*
#define get_key_type(k)			get_le16 (k, u.k2_offset_v2.k_type)
#define set_key_type(k,val)		set_le16 (k, u.k2_offset_v2.k_type, val)
*/
/*
#define key_dir_id(key)                 (le32_to_cpu((key)->k_dir_id))
#define set_key_dir_id(key,n)           ((key)->k_dir_id = cpu_to_le32((n)))
#define key_objectid(key)               (le32_to_cpu((key)->k_objectid))
#define set_key_objectid(key,n)         ((key)->k_objectid = cpu_to_le32((n)))
*/

#define KEY_SIZE (sizeof(struct reiserfs_key))
#define SHORT_KEY_SIZE 8

// values for k_uniqueness field of the struct reiserfs_key
#define V1_SD_UNIQUENESS 0
#define V1_DIRENTRY_UNIQUENESS 500
#define DIRENTRY_UNIQUENESS 500
#define V1_DIRECT_UNIQUENESS 0xffffffff
#define V1_INDIRECT_UNIQUENESS 0xfffffffe
#define V1_UNKNOWN_UNIQUENESS 555

// values for k_type field of the struct reiserfs_key
#define TYPE_STAT_DATA 0
#define TYPE_INDIRECT 1
#define TYPE_DIRECT 2
#define TYPE_DIRENTRY 3
#define TYPE_MAXTYPE 4

#define TYPE_UNKNOWN 15

/* special type for symlink not conflicting to any of item types. */
#define TYPE_SYMLINK	4

#define KEY_FORMAT_1 0
#define KEY_FORMAT_2 1
#define KEY_FORMAT_UNDEFINED 15

 /* Our function for comparing keys can compare keys of different lengths.  It
    takes as a parameter the length of the keys it is to compare.  These
    defines are used in determining what is to be passed to it as that
    parameter. */
#define REISERFS_FULL_KEY_LEN     4

#define REISERFS_SHORT_KEY_LEN    2

/* Everything in the filesystem is stored as a set of items.  The item head
   contains the key of the item, its free space (for indirect items) and
   specifies the location of the item itself within the block.  */

struct item_head {
	struct reiserfs_key ih_key;	/* Everything in the tree is found by searching for it
					   based on its key. */

	union {
		__le16 ih2_free_space;	/* The free space in the last unformatted node
					   of an indirect item if this is an indirect
					   item.  This equals 0xFFFF iff this is a direct
					   item or stat data item. Note that the key, not
					   this field, is used to determine the item
					   type, and thus which field this union
					   contains. */
		__le16 ih2_entry_count;	/* Iff this is a directory item, this field
					   equals the number of directory entries in
					   the directory item. */
	} __attribute__ ((__packed__)) u;
	__le16 ih2_item_len;	/* total size of the item body */
	__le16 ih2_item_location;	/* an offset to the item body within the
					   block */

	__le16 ih_format;	/* key format is stored in bits 0-11 of this item
				   flags are stored in bits 12-15 */
#if 0
	struct {
		__le16 key_format:12;	/* KEY_FORMAT_1 or KEY_FORMAT_2. This is not
					   necessary, but we have space, let use it */
		__le16 flags:4;	/* fsck set here its flag (reachable/unreachable) */
	} __attribute__ ((__packed__)) ih2_format;
#endif
} __attribute__ ((__packed__));

/* size of item header     */
#define IH_SIZE (sizeof(struct item_head))

/* set/get fields of item head on disk with these defines */
#define get_ih_entry_count(ih)		get_le16 (ih, u.ih2_entry_count)
#define set_ih_entry_count(ih,val)	set_le16 (ih, u.ih2_entry_count, val)

#define get_ih_free_space(ih)		get_le16 (ih, u.ih2_free_space)
#define set_ih_free_space(ih,val)	set_le16 (ih, u.ih2_free_space, 0)

#define get_ih_item_len(ih)		get_le16 (ih, ih2_item_len)
#define set_ih_item_len(ih,val)		set_le16 (ih, ih2_item_len, val)

#define get_ih_location(ih)		get_le16 (ih, ih2_item_location)
#define set_ih_location(ih,val)		set_le16 (ih, ih2_item_location, val)

__u16 get_ih_flags(const struct item_head *ih);
__u16 get_ih_key_format(const struct item_head *ih);
void set_ih_flags(struct item_head *ih, __u16 val);
void set_ih_key_format(struct item_head *ih, __u16 val);

/*
#define get_ih_key_format(ih)		get_le16 (ih, ih2_format.key_format)
#define set_ih_key_format(ih,val)	set_le16 (ih, ih2_format.key_format, val)

#define get_ih_flags(ih)		get_le16 (ih, ih2_format.flags)
#define set_ih_flags(ih,val)		set_le16 (ih, ih2_format.flags, val)
*/

#define I_K_KEY_IN_ITEM(p_s_ih, p_s_key, n_blocksize) \
    ( ! not_of_one_file(p_s_ih, p_s_key) && \
          I_OFF_BYTE_IN_ITEM(p_s_ih, get_offset (p_s_key), n_blocksize) )

#define IH_Unreachable 0
#define IH_Was_Tail    1
#define IH_Checked     2
#define IH_Writable    3

/* Unreachable bit is set on tree rebuilding and is cleared in semantic pass */
#define clean_ih_flags(ih) set_ih_flags (ih, 0)

#define ih_reachable(ih)        (!(get_ih_flags (ih) & (1 << IH_Unreachable)))
#define mark_ih_reachable(ih)   set_ih_flags (ih, get_ih_flags (ih) & ~(1 << IH_Unreachable))
#define mark_ih_unreachable(ih) set_ih_flags (ih, get_ih_flags (ih) | (1 << IH_Unreachable))

#define ih_was_tail(ih)         (get_ih_flags (ih) & (1 << IH_Was_Tail))
#define mark_ih_was_tail(ih)    set_ih_flags (ih, get_ih_flags (ih) | (1 << IH_Was_Tail))
#define mark_ih_become_tail(ih) set_ih_flags (ih, get_ih_flags (ih) & ~(1 << IH_Was_Tail))

#define ih_checked(ih)          (get_ih_flags (ih) & (1 << IH_Checked))
#define mark_ih_checked(ih)     set_ih_flags (ih, get_ih_flags (ih) | (1 << IH_Checked))
#define clear_ih_checked(ih)    set_ih_flags (ih, get_ih_flags (ih) & ~(1 << IH_Checked))

#define ih_writable(ih)         (get_ih_flags (ih) & (1 << IH_Writable))
#define mark_ih_writable(ih)    set_ih_flags (ih, get_ih_flags (ih) | (1 << IH_Writable))
#define clear_ih_writable(ih)   set_ih_flags (ih, get_ih_flags (ih) & ~(1 << IH_Writable))

/* maximal length of item */
#define MAX_ITEM_LEN(block_size) (block_size - BLKH_SIZE - IH_SIZE)
#define MIN_ITEM_LEN 1

/* object identifier for root dir */
#define REISERFS_ROOT_OBJECTID 2
#define REISERFS_ROOT_PARENT_OBJECTID 1

/*
 * Picture represents a leaf of internal tree
 *  ______________________________________________________
 * |      |  Array of     |                   |           |
 * |Block |  Object-Item  |      F r e e      |  Objects- |
 * | head |  Headers      |     S p a c e     |   Items   |
 * |______|_______________|___________________|___________|
 */

/* Header of a disk block.  More precisely, header of a formatted leaf
   or internal node, and not the header of an unformatted node. */
struct block_head {
	__le16 blk2_level;	/* Level of a block in the tree. */
	__le16 blk2_nr_item;	/* Number of keys/items in a block. */
	__le16 blk2_free_space;	/* Block free space in bytes. */
	__le16 blk_reserved;
	__le32 reserved[4];
};

#define BLKH_SIZE (sizeof(struct block_head))

/* set/get fields of block head on disk with these defines */
#define get_blkh_level(blkh)		get_le16 (blkh, blk2_level)
#define set_blkh_level(blkh,val)	set_le16 (blkh, blk2_level, val)

#define get_blkh_nr_items(blkh)		get_le16 (blkh, blk2_nr_item)
#define set_blkh_nr_items(blkh,val)	set_le16 (blkh, blk2_nr_item, val)

#define get_blkh_free_space(blkh)	get_le16 (blkh, blk2_free_space)
#define set_blkh_free_space(blkh,val)	set_le16 (blkh, blk2_free_space, val)

/*
 * values for blk_type field
 */

#define FREE_LEVEL        0	/* Node of this level is out of the tree. */
#define DISK_LEAF_NODE_LEVEL  1	/* Leaf node level.                       */

#define is_leaf_block_head(buf) (get_blkh_level ((struct block_head *)(buf)) == DISK_LEAF_NODE_LEVEL)
#define is_internal_block_head(buf) \
((get_blkh_level (((struct block_head *)(buf))) > DISK_LEAF_NODE_LEVEL) &&\
 (get_blkh_level (((struct block_head *)(buf))) <= MAX_HEIGHT))

/* Given the buffer head of a formatted node, resolve to the block head of that node. */
#define B_BLK_HEAD(p_s_bh)  ((struct block_head *)((p_s_bh)->b_data))

#define B_NR_ITEMS(bh)		get_blkh_nr_items (B_BLK_HEAD(bh))
#define B_LEVEL(bh)		get_blkh_level (B_BLK_HEAD(bh))
#define B_FREE_SPACE(bh)	get_blkh_free_space (B_BLK_HEAD(bh))

#define is_leaf_node(bh) is_leaf_block_head ((bh)->b_data)
#define is_internal_node(bh) is_internal_block_head ((bh)->b_data)

/***************************************************************************/
/*                             STAT DATA                                   */
/***************************************************************************/

/* Stat Data on disk (reiserfs version of UFS disk inode minus the address blocks) */

/* The sense of adding union to stat data is to keep a value of real number of
   blocks used by file.  The necessity of adding such information is caused by
   existing of files with holes.  Reiserfs should keep number of used blocks
   for file, but not calculate it from file size (that is not correct for
   holed files). Thus we have to add additional information to stat data.
   When we have a device special file, there is no need to get number of used
   blocks for them, and, accordingly, we doesn't need to keep major and minor
   numbers for regular files, which might have holes. So this field is being
   overloaded.  */

struct stat_data_v1 {
	__le16 sd_mode;		/* file type, permissions */
	__le16 sd_nlink;		/* number of hard links */
	__le16 sd_uid;		/* owner */
	__le16 sd_gid;		/* group */
	__le32 sd_size;		/* file size */
	__le32 sd_atime;		/* time of last access */
	__le32 sd_mtime;		/* time file was last modified  */
	__le32 sd_ctime;		/* time inode (stat data) was last changed (except
				   changes to sd_atime and sd_mtime) */
	union {
		__le32 sd_rdev;
		__le32 sd_blocks;	/* number of blocks file uses */
	} __attribute__ ((__packed__)) u;
	__le32 sd_first_direct_byte;	/* first byte of file which is stored
					   in a direct item: except that if it
					   equals 1 it is a symlink and if it
					   equals MAX_KEY_OFFSET there is no
					   direct item.  The existence of this
					   field really grates on me. Let's
					   replace it with a macro based on
					   sd_size and our tail suppression
					   policy.  Someday.  -Hans */
} __attribute__ ((__packed__));
#define SD_V1_SIZE (sizeof(struct stat_data_v1))

/* this is used to check sd_size of stat data v1 */
#define MAX_FILE_SIZE_V1 0x7fffffff

// sd_first_direct_byte is set to this when there are no direct items in a
// file
#define NO_BYTES_IN_DIRECT_ITEM 0xffffffff

/* Stat Data on disk (reiserfs version of UFS disk inode minus the
   address blocks) */
struct stat_data {
	__le16 sd_mode;		/* file type, permissions */
	__le16 sd_attrs;
	__le32 sd_nlink;		/* 32 bit nlink! */
	__le64 sd_size;		/* 64 bit size! */
	__le32 sd_uid;		/* 32 bit uid! */
	__le32 sd_gid;		/* 32 bit gid! */
	__le32 sd_atime;		/* time of last access */
	__le32 sd_mtime;		/* time file was last modified  */
	__le32 sd_ctime;		/* time inode (stat data) was last changed (except
				   changes to sd_atime and sd_mtime) */
	__le32 sd_blocks;
	union {
		__le32 sd_rdev;
		__le32 sd_generation;
		//__le32 sd_first_direct_byte;
		/* first byte of file which is stored in a direct item: except that if
		   it equals 1 it is a symlink and if it equals ~(__le32)0 there is no
		   direct item.  The existence of this field really grates on me. Let's
		   replace it with a macro based on sd_size and our tail suppression
		   policy? */
	} __attribute__ ((__packed__)) u;
} __attribute__ ((__packed__));
//
// this is 44 bytes long
//
#define SD_SIZE (sizeof(struct stat_data))

// there are two ways: to check length of item or ih_format field
// (for old stat data it is set to 0 (KEY_FORMAT_1))
#define stat_data_v1(ih) (get_ih_key_format (ih) == KEY_FORMAT_1)

/* this is used to check sd_size of stat data v2: max offset which can
   be reached with a key of format 2 is 60 bits */
#define MAX_FILE_SIZE_V2 0xfffffffffffffffLL

/***************************************************************************/
/*                      DIRECTORY STRUCTURE                                */
/***************************************************************************/
/*
   Picture represents the structure of directory items
   ________________________________________________
   |  Array of     |   |     |        |       |   |
   | directory     |N-1| N-2 | ....   |   1st |0th|
   | entry headers |   |     |        |       |   |
   |_______________|___|_____|________|_______|___|
                    <----   directory entries         ------>

 First directory item has k_offset component 1. We store "." and ".."
 in one item, always, we never split "." and ".." into differing
 items.  This makes, among other things, the code for removing
 directories simpler. */
#define SD_OFFSET  0
#define DOT_OFFSET 1
#define DOT_DOT_OFFSET 2

/* Each directory entry has its header. This header has deh_dir_id and
   deh_objectid fields, those are key of object, entry points to */

/* NOT IMPLEMENTED:
   Directory will someday contain stat data of object */
struct reiserfs_de_head {
	__le32 deh2_offset;	/* third component of the directory entry key */
	__le32 deh2_dir_id;	/* objectid of the parent directory of the object,
				   that is referenced by directory entry */
	__le32 deh2_objectid;	/* objectid of the object, that is referenced by
				   directory entry */
	__le16 deh2_location;	/* offset of name in the whole item */
	__le16 deh2_state;	/* whether 1) entry contains stat data (for future),
				   and 2) whether entry is hidden (unlinked) */
} __attribute__ ((__packed__));

#define DEH_SIZE sizeof(struct reiserfs_de_head)

/* set/get fields of dir entry head these defines */
#define get_deh_offset(deh)	get_le32 (deh, deh2_offset)
#define set_deh_offset(deh,val)	set_le32 (deh, deh2_offset, val)

#define get_deh_dirid(deh)	get_le32 (deh, deh2_dir_id)
#define set_deh_dirid(deh,val)	set_le32 (deh, deh2_dir_id, val)

#define get_deh_objectid(deh)		get_le32 (deh, deh2_objectid)
#define set_deh_objectid(deh,val)	set_le32 (deh, deh2_objectid, val)

#define get_deh_location(deh)		get_le16 (deh, deh2_location)
#define set_deh_location(deh,val)	set_le16 (deh, deh2_location, val)

#define get_deh_state(deh)	get_le16 (deh, deh2_state)
#define set_deh_state(deh,val)	set_le16 (deh, deh2_state, val)

#define deh_offset(deh) (le32_to_cpu ((deh)->deh_offset))
#define deh_dir_id(deh) (le32_to_cpu ((deh)->deh_dir_id))
#define deh_objectid(deh) (le32_to_cpu ((deh)->deh_objectid))
#define deh_location(deh) (le16_to_cpu ((deh)->deh_location))
#define deh_state(deh) (le16_to_cpu ((deh)->deh_state))

/* empty directory contains two entries "." and ".." and their headers */
#define EMPTY_DIR_SIZE \
(DEH_SIZE * 2 + ROUND_UP (strlen (".")) + ROUND_UP (strlen ("..")))

/* old format directories have this size when empty */
#define EMPTY_DIR_SIZE_V1 (DEH_SIZE * 2 + 3)

#define DEH_Statdata 0		/* not used now */
#define DEH_Visible2 2

#define DEH_Bad_offset 4	/* fsck marks entries to be deleted with this flag */
#define DEH_Bad_location 5

#define test_deh_state_le_bit(deh,bit) (get_deh_state (deh) & (1 << bit))

#define set_deh_state_le_bit(deh,bit) \
{\
	__u16 state;\
	state = get_deh_state (deh);\
	state |= (1 << bit);\
	set_deh_state(deh, state);\
}

#define clear_deh_state_le_bit(deh,bit) \
{\
	__u16 state;\
	state = get_deh_state (deh);\
	state &= ~(1 << bit);\
	set_deh_state(deh, state);\
}

#define mark_de_without_sd(deh)	clear_deh_state_le_bit (deh, DEH_Statdata)
#define mark_de_visible(deh)	set_deh_state_le_bit (deh, DEH_Visible2)
#define mark_de_hidden(deh)	clear_deh_state_le_bit (deh, DEH_Visible)

#define de_with_sd(deh)		test_deh_state_le_bit (deh, DEH_Statdata)
#define de_visible(deh)	    	test_deh_state_le_bit (deh, DEH_Visible2)
#define de_hidden(deh)	    	!test_deh_state_le_bit (deh, DEH_Visible2)

/* Bad means "hashed unproperly or/and invalid location" */
#define de_bad_location(deh)	test_deh_state_le_bit (deh, DEH_Bad_location)
#define mark_de_bad_location(deh) set_deh_state_le_bit (deh, DEH_Bad_location)
#define mark_de_good_location(deh) clear_deh_state_le_bit (deh, DEH_Bad_location)

#define de_bad_offset(deh)	test_deh_state_le_bit (deh, DEH_Bad_offset)
#define mark_de_bad_offset(deh)	set_deh_state_le_bit (deh, DEH_Bad_offset)

#define de_bad(deh) (de_bad_location(deh) || de_bad_offset(deh))

/* for directories st_blocks is number of 512 byte units which fit into dir
   size round up to blocksize */
#define dir_size2st_blocks(size) ((size + 511) / 512)

/* array of the entry headers */
#define B_I_DEH(bh,ih) ((struct reiserfs_de_head *)(ih_item_body(bh,ih)))

#define REISERFS_MAX_NAME_LEN(block_size) (block_size - BLKH_SIZE - IH_SIZE - DEH_SIZE)
	/* -SD_SIZE when entry will contain stat data */

/* hash value occupies 24 bits starting from 7 up to 30 */
#define GET_HASH_VALUE(offset) ((offset) & 0x7fffff80)
/* generation number occupies 7 bits starting from 0 up to 6 */
#define GET_GENERATION_NUMBER(offset) ((offset) & 0x0000007f)

/*
 * Picture represents an internal node of the reiserfs tree
 *  ______________________________________________________
 * |      |  Array of     |  Array of         |  Free     |
 * |block |    keys       |  pointers         | space     |
 * | head |      N        |      N+1          |           |
 * |______|_______________|___________________|___________|
 */

/***************************************************************************/
/*                      DISK CHILD                                         */
/***************************************************************************/
/* Disk child pointer: The pointer from an internal node of the tree
   to a node that is on disk. */
struct disk_child {
	__le32 dc2_block_number;	/* Disk child's block number. */
	__le16 dc2_size;		/* Disk child's used space.   */
	__le16 dc2_reserved;
} __attribute__ ((__packed__));

#define DC_SIZE (sizeof(struct disk_child))

/* set/get fields of disk_child with these defines */
#define get_dc_child_blocknr(dc)	get_le32 (dc, dc2_block_number)
#define set_dc_child_blocknr(dc,val)	set_le32 (dc, dc2_block_number, val)

#define get_dc_child_size(dc)		get_le16 (dc, dc2_size)
#define set_dc_child_size(dc,val)	set_le16 (dc, dc2_size, val)

#define set_dc(dc, size, blocknr)	\
({					\
    set_dc_child_blocknr(dc, blocknr);	\
    set_dc_child_size(dc, size);	\
    set_le16(dc, dc2_reserved, 0);	\
})

/* Get disk child by buffer header and position in the tree node. */
#define B_N_CHILD(p_s_bh,n_pos)  ((struct disk_child *)\
            ((p_s_bh)->b_data + BLKH_SIZE + B_NR_ITEMS(p_s_bh) \
            * KEY_SIZE + DC_SIZE * (n_pos)))

 /* maximal value of field child_size in structure disk_child */
 /* child size is the combined size of all items and their headers */
#define MAX_CHILD_SIZE(blocksize) ((blocksize) - BLKH_SIZE)
#define MAX_FREE_SPACE(blocksize) MAX_CHILD_SIZE(blocksize)

/* amount of used space in buffer (not including block head) */
#define B_CHILD_SIZE(cur) (MAX_CHILD_SIZE(cur->b_size)-(B_FREE_SPACE(cur)))

/* max and min number of keys in internal node */
#define MAX_NR_KEY(bh) ( (MAX_CHILD_SIZE(bh->b_size)-DC_SIZE)/(KEY_SIZE+DC_SIZE) )
#define MIN_NR_KEY(bh)    (MAX_NR_KEY(bh)/2)

/***************************************************************************/
/*                      PATH STRUCTURES AND DEFINES                        */
/***************************************************************************/

/* Search_by_key fills up the path from the root to the leaf as it
   descends the tree looking for the key.  It uses reiserfs_bread to
   try to find buffers in the cache given their block number.  If it
   does not find them in the cache it reads them from disk.  For each
   node search_by_key finds using reiserfs_bread it then uses
   bin_search to look through that node.  bin_search will find the
   position of the block_number of the next node if it is looking
   through an internal node.  If it is looking through a leaf node
   bin_search will find the position of the item which has key either
   equal to given key, or which is the maximal key less than the given
   key. */

struct reiserfs_path_element {
	struct buffer_head *pe_buffer;	/* Pointer to the buffer at the path in
					   the tree. */
	unsigned int pe_position;	/* Position in the tree node which is placed
					   in the buffer above. */
};

#define MAX_HEIGHT 6
#define FIRST_PATH_ELEMENT_OFFSET   2
#define EXTENDED_MAX_HEIGHT         (MAX_HEIGHT + FIRST_PATH_ELEMENT_OFFSET)

#define ILLEGAL_PATH_ELEMENT_OFFSET 1
#define MAX_FEB_SIZE (MAX_HEIGHT + 1)

/* We need to keep track of who the ancestors of nodes are.  When we
   perform a search we record which nodes were visited while
   descending the tree looking for the node we searched for. This list
   of nodes is called the path.  This information is used while
   performing balancing.  Note that this path information may become
   invalid, and this means we must check it when using it to see if it
   is still valid. You'll need to read search_by_key and the comments
   in it, especially about decrement_counters_in_path(), to understand
   this structure. */
struct reiserfs_path {
	unsigned int path_length;	/* Length of the array above.   */
	struct reiserfs_path_element path_elements[EXTENDED_MAX_HEIGHT];	/* Array of the path elements.  */
	unsigned int pos_in_item;
};

#define INITIALIZE_REISERFS_PATH(var) \
struct reiserfs_path var = {ILLEGAL_PATH_ELEMENT_OFFSET, }

/* Get path element by path and path position. */
#define PATH_OFFSET_PELEMENT(p_s_path,n_offset)  ((p_s_path)->path_elements +(n_offset))

/* Get buffer header at the path by path and path position. */
#define PATH_OFFSET_PBUFFER(p_s_path,n_offset)   (PATH_OFFSET_PELEMENT(p_s_path,n_offset)->pe_buffer)

/* Get position in the element at the path by path and path position. */
#define PATH_OFFSET_POSITION(p_s_path,n_offset) (PATH_OFFSET_PELEMENT(p_s_path,n_offset)->pe_position)

#define PATH_PLAST_BUFFER(p_s_path) (PATH_OFFSET_PBUFFER((p_s_path), (p_s_path)->path_length))
#define PATH_LAST_POSITION(p_s_path) (PATH_OFFSET_POSITION((p_s_path), (p_s_path)->path_length))

#define tp_item_head(p_s_path)    item_head(PATH_PLAST_BUFFER(p_s_path),PATH_LAST_POSITION(p_s_path))

/* in do_balance leaf has h == 0 in contrast with path structure,
   where root has level == 0. That is why we need these defines */
#define PATH_H_PBUFFER(p_s_path, h) PATH_OFFSET_PBUFFER (p_s_path, p_s_path->path_length - (h))	/* tb->S[h] */
#define PATH_H_PPARENT(path, h) PATH_H_PBUFFER (path, (h) + 1)	/* tb->F[h] or tb->S[0]->b_parent */
#define PATH_H_POSITION(path, h) PATH_OFFSET_POSITION (path, path->path_length - (h))
#define PATH_H_B_ITEM_ORDER(path, h) PATH_H_POSITION(path, h + 1)	/* tb->S[h]->b_item_order */

#define PATH_H_PATH_OFFSET(p_s_path, n_h) ((p_s_path)->path_length - (n_h))

#define get_bh(path) PATH_PLAST_BUFFER(path)
#define get_item_pos(path) PATH_LAST_POSITION(path)
#define tp_item_body(path) ((void *)item_body(PATH_PLAST_BUFFER(path), PATH_LAST_POSITION (path)))
#define item_moved(ih,path) comp_items(ih, path)
#define path_changed(ih,path) comp_items (ih, path)

/***************************************************************************/
/*                       MISC                                              */
/***************************************************************************/

/* n must be power of 2 */
#define _ROUND_UP(x,n) (((x)+(n)-1u) & ~((n)-1u))

// to be ok for alpha and others we have to align structures to 8 byte
// boundary.
// FIXME: do not change 4 by anything else: there is code which relies on that
#define ROUND_UP(x) _ROUND_UP(x,8LL)

// search_by_key (and clones) and fix_nodes error code
#define CARRY_ON		    0

#define NO_DISK_SPACE		    3
/* #define IO_ERROR		    0x4 - defined above as 0x4 */
#define NO_BALANCING_NEEDED	    5
#define ITEM_FOUND		    6
#define ITEM_NOT_FOUND		    7
#define GOTO_PREVIOUS_ITEM	    10
#define POSITION_FOUND_INVISIBLE    11
#define FILE_NOT_FOUND		    12

// used by fsck
#define DIRECTORY_NOT_FOUND	    13
#define REGULAR_FILE_FOUND	    14
#define DIRECTORY_FOUND		    15

/* Size of pointer to the unformatted node. */
#define UNFM_P_SIZE (sizeof(__le32))

#define MAX_KEY1_OFFSET	 0xffffffff
#define MAX_KEY2_OFFSET  0xfffffffffffffffLL

/* this is aggressive tail suppression policy taken from the kernel */
/* It should be MAX_DIRECT_ITEM_LEN used here, but sometimes it is not enough,
 * and items got deleted. */
#define STORE_TAIL_IN_UNFM(n_file_size,n_tail_size,n_block_size) \
(\
  (!(n_tail_size)) || \
  (((n_tail_size) > MAX_ITEM_LEN(n_block_size)) || \
   ( (n_file_size) >= (n_block_size) * 4 ) || \
   ( ( (n_file_size) >= (n_block_size) * 3 ) && \
     ( (n_tail_size) >=   (MAX_ITEM_LEN(n_block_size))/4) ) || \
   ( ( (n_file_size) >= (n_block_size) * 2 ) && \
     ( (n_tail_size) >=   (MAX_ITEM_LEN(n_block_size))/2) ) || \
   ( ( (n_file_size) >= (n_block_size) ) && \
     ( (n_tail_size) >=   (MAX_ITEM_LEN(n_block_size) * 3)/4) ) ) \
)

/***************************************************************************/
/*                  FIXATE NODES                                           */
/***************************************************************************/

#define VI_TYPE_STAT_DATA 1
#define VI_TYPE_DIRECT 2
#define VI_TYPE_INDIRECT 4
#define VI_TYPE_DIRECTORY 8
#define VI_TYPE_FIRST_DIRECTORY_ITEM 16
#define VI_TYPE_INSERTED_DIRECTORY_ITEM 32

#define VI_TYPE_LEFT_MERGEABLE 64
#define VI_TYPE_RIGHT_MERGEABLE 128

/* To make any changes in the tree we always first find node, that contains
   item to be changed/deleted or place to insert a new item. We call this node
   S. To do balancing we need to decide what we will shift to left/right
   neighbor, or to a new node, where new item will be etc. To make this
   analysis simpler we build virtual node. Virtual node is an array of items,
   that will replace items of node S. (For instance if we are going to delete
   an item, virtual node does not contain it). Virtual node keeps information
   about item sizes and types, mergeability of first and last items, sizes of
   all entries in directory item. We use this array of items when calculating
   what we can shift to neighbors and how many nodes we have to have if we do
   not any shiftings, if we shift to left/right neighbor or to both. */
struct virtual_item {
	unsigned short vi_type;	/* item type, mergeability */
	unsigned short vi_item_len;	/* length of item that it will have after balancing */
	__u64 vi_item_offset;	/* offset of item that it have before balancing */

	short vi_entry_count;	/* number of entries in directory item
				   (including the new one if any, or excluding
				   entry if it must be cut) */
	unsigned short *vi_entry_sizes;	/* array of entry lengths for directory item */
};

struct virtual_node {
	char *vn_free_ptr;	/* this is a pointer to the free space in the buffer */
	unsigned short vn_nr_item;	/* number of items in virtual node */
	short vn_size;		/* size of node , that node would have if it has unlimited
				   size and no balancing is performed */
	short vn_mode;		/* mode of balancing (paste, insert, delete, cut) */
	short vn_affected_item_num;
	short vn_pos_in_item;
	struct item_head *vn_ins_ih;	/* item header of inserted item, 0 for other modes */
	struct virtual_item *vn_vi;	/* array of items (including a new one, excluding item to be deleted) */
};

/***************************************************************************/
/*                  TREE BALANCE                                           */
/***************************************************************************/

/* This temporary structure is used in tree balance algorithms, and
   constructed as we go to the extent that its various parts are needed.  It
   contains arrays of nodes that can potentially be involved in the balancing
   of node S, and parameters that define how each of the nodes must be
   balanced.  Note that in these algorithms for balancing the worst case is to
   need to balance the current node S and the left and right neighbors and all
   of their parents plus create a new node.  We implement S1 balancing for the
   leaf nodes and S0 balancing for the internal nodes (S1 and S0 are defined
   in our papers.)*/

#define MAX_FREE_BLOCK 7	/* size of the array of buffers to free at end of do_balance */

/* maximum number of FEB blocknrs on a single level */
#define MAX_AMOUNT_NEEDED 2

/* someday somebody will prefix every field in this struct with tb_ */
struct tree_balance {
	struct reiserfs_transaction_handle *transaction_handle;
	reiserfs_filsys_t tb_fs;
	struct reiserfs_path *tb_path;
	struct buffer_head *L[MAX_HEIGHT];	/* array of left neighbors of nodes in the path */
	struct buffer_head *R[MAX_HEIGHT];	/* array of right neighbors of nodes in the path */
	struct buffer_head *FL[MAX_HEIGHT];	/* array of fathers of the left  neighbors      */
	struct buffer_head *FR[MAX_HEIGHT];	/* array of fathers of the right neighbors      */
	struct buffer_head *CFL[MAX_HEIGHT];	/* array of common parents of center node and its left neighbor  */
	struct buffer_head *CFR[MAX_HEIGHT];	/* array of common parents of center node and its right neighbor */

	/* array of blocknr's that are free and are the nearest to the left node that are usable
	   for writing dirty formatted leaves, using the write_next_to algorithm. */
	/*unsigned long free_and_near[MAX_DIRTIABLE]; */

	struct buffer_head *FEB[MAX_FEB_SIZE];	/* array of empty buffers. Number of buffers in array equals
						   cur_blknum. */
	struct buffer_head *used[MAX_FEB_SIZE];
	short int lnum[MAX_HEIGHT];	/* array of number of items which must be shifted to the left in
					   order to balance the current node; for leaves includes item
					   that will be partially shifted; for internal nodes, it is
					   the number of child pointers rather than items. It includes
					   the new item being created.  For preserve_shifted() purposes
					   the code sometimes subtracts one from this number to get the
					   number of currently existing items being shifted, and even
					   more often for leaves it subtracts one to get the number of
					   wholly shifted items for other purposes. */
	short int rnum[MAX_HEIGHT];	/* substitute right for left in comment above */
	short int lkey[MAX_HEIGHT];	/* array indexed by height h mapping the key delimiting L[h] and
					   S[h] to its item number within the node CFL[h] */
	short int rkey[MAX_HEIGHT];	/* substitute r for l in comment above */
	short int insert_size[MAX_HEIGHT];	/* the number of bytes by we are trying to add or remove from
						   S[h]. A negative value means removing.  */
	short int blknum[MAX_HEIGHT];	/* number of nodes that will replace node S[h] after
					   balancing on the level h of the tree.  If 0 then S is
					   being deleted, if 1 then S is remaining and no new nodes
					   are being created, if 2 or 3 then 1 or 2 new nodes is
					   being created */

	/* fields that are used only for balancing leaves of the tree */
	short int cur_blknum;	/* number of empty blocks having been already allocated                 */
	short int s0num;	/* number of items that fall into left most  node when S[0] splits     */
	short int s1num;	/* number of items that fall into first  new node when S[0] splits     */
	short int s2num;	/* number of items that fall into second new node when S[0] splits     */
	short int lbytes;	/* number of bytes which can flow to the left neighbor from the        left    */
	/* most liquid item that cannot be shifted from S[0] entirely               */
	/* if -1 then nothing will be partially shifted */
	short int rbytes;	/* number of bytes which will flow to the right neighbor from the right        */
	/* most liquid item that cannot be shifted from S[0] entirely               */
	/* if -1 then nothing will be partially shifted                           */
	short int s1bytes;	/* number of bytes which flow to the first  new node when S[0] splits   */
	/* note: if S[0] splits into 3 nodes, then items do not need to be cut  */
	short int s2bytes;
	struct buffer_head *buf_to_free[MAX_FREE_BLOCK];	/* buffers which are to be freed after do_balance finishes by unfix_nodes */
	char *vn_buf;		/* kmalloced memory. Used to create
				   virtual node and keep map of
				   dirtied bitmap blocks */
	int vn_buf_size;	/* size of the vn_buf */
	struct virtual_node *tb_vn;	/* VN starts after bitmap of bitmap blocks */
};

/* These are modes of balancing */

/* When inserting an item. */
#define M_INSERT	'i'
/* When inserting into (directories only) or appending onto an already
   existant item. */
#define M_PASTE		'p'
/* When deleting an item. */
#define M_DELETE	'd'
/* When truncating an item or removing an entry from a (directory) item. */
#define M_CUT 		'c'

/* used when balancing on leaf level skipped (in reiserfsck) */
#define M_INTERNAL	'n'

/* When further balancing is not needed, then do_balance does not need
   to be called. */
#define M_SKIP_BALANCING 		's'
#define M_CONVERT	'v'

/* modes of leaf_move_items */
#define LEAF_FROM_S_TO_L 0
#define LEAF_FROM_S_TO_R 1
#define LEAF_FROM_R_TO_L 2
#define LEAF_FROM_L_TO_R 3
#define LEAF_FROM_S_TO_SNEW 4

#define FIRST_TO_LAST 0
#define LAST_TO_FIRST 1

/* used in do_balance for passing parent of node information that has been
   gotten from tb struct */
struct buffer_info {
    reiserfs_filsys_t bi_fs;
    struct buffer_head * bi_bh;
    struct buffer_head * bi_parent;
    int bi_position;
};

/* there are 4 types of items: stat data, directory item, indirect, direct.
   FIXME: This table does not describe new key format
+-------------------+------------+--------------+------------+
|	            |  k_offset  | k_uniqueness | mergeable? |
+-------------------+------------+--------------+------------+
|     stat data     |	0        |      0       |   no       |
+-------------------+------------+--------------+------------+
| 1st directory item| DOT_OFFSET |DIRENTRY_UNIQUENESS|   no       |
| non 1st directory | hash value |              |   yes      |
|     item          |            |              |            |
+-------------------+------------+--------------+------------+
| indirect item     | offset + 1 |TYPE_INDIRECT |   if this is not the first indirect item of the object
+-------------------+------------+--------------+------------+
| direct item       | offset + 1 |TYPE_DIRECT   | if not this is not the first direct item of the object
+-------------------+------------+--------------+------------+
*/



#define KEY_IS_STAT_DATA_KEY(p_s_key) 	( get_type (p_s_key) == TYPE_STAT_DATA )
#define KEY_IS_DIRECTORY_KEY(p_s_key)	( get_type (p_s_key) == TYPE_DIRENTRY )
#define KEY_IS_DIRECT_KEY(p_s_key) 	( get_type (p_s_key) == TYPE_DIRECT )
#define KEY_IS_INDIRECT_KEY(p_s_key)	( get_type (p_s_key) == TYPE_INDIRECT )

#define I_IS_STAT_DATA_ITEM(p_s_ih) 	KEY_IS_STAT_DATA_KEY(&((p_s_ih)->ih_key))
#define I_IS_DIRECTORY_ITEM(p_s_ih) 	KEY_IS_DIRECTORY_KEY(&((p_s_ih)->ih_key))
#define I_IS_DIRECT_ITEM(p_s_ih) 	KEY_IS_DIRECT_KEY(&((p_s_ih)->ih_key))
#define I_IS_INDIRECT_ITEM(p_s_ih) 	KEY_IS_INDIRECT_KEY(&((p_s_ih)->ih_key))

#define is_indirect_ih(ih) I_IS_INDIRECT_ITEM(ih)
#define is_direct_ih(ih) I_IS_DIRECT_ITEM(ih)
#define is_direntry_ih(ih) I_IS_DIRECTORY_ITEM(ih)
#define is_stat_data_ih(ih) I_IS_STAT_DATA_ITEM(ih)

#define is_indirect_key(key) KEY_IS_INDIRECT_KEY(key)
#define is_direct_key(key) KEY_IS_DIRECT_KEY(key)
#define is_direntry_key(key) KEY_IS_DIRECTORY_KEY(key)
#define is_stat_data_key(key) KEY_IS_STAT_DATA_KEY(key)

#define COMP_KEYS comp_keys

//#define COMP_SHORT_KEYS comp_short_keys
#define not_of_one_file comp_short_keys

/* number of blocks pointed to by the indirect item */
#define I_UNFM_NUM(p_s_ih)	( get_ih_item_len(p_s_ih) / UNFM_P_SIZE )

/* the used space within the unformatted node corresponding to pos within the item pointed to by ih */
#define I_POS_UNFM_SIZE(ih,pos,size) (((pos) == I_UNFM_NUM(ih) - 1 ) ? (size) - ih_free_space (ih) : (size))

/* check whether byte number 'offset' is in this item */
#define I_OFF_BYTE_IN_ITEM(p_s_ih, n_offset, n_blocksize) \
                  ( get_offset(&(p_s_ih)->ih_key) <= (n_offset) && \
                    get_offset(&(p_s_ih)->ih_key) + get_bytes_number(p_s_ih,n_blocksize) > (n_offset) )

/* these operate on indirect items, where you've got an array of ints
** at a possibly unaligned location.  These are a noop on ia32
**
** p is the array of __u32, i is the index into the array, v is the value
** to store there.
*/
#define d32_get(p, i) le32_to_cpu(get_unaligned((p) + (i)))
#define d32_put(p, i, v) put_unaligned(cpu_to_le32(v), (p) + (i))


/* get the item header */
#define item_head(bh,item_num) ( (struct item_head * )((bh)->b_data + BLKH_SIZE) + (item_num) )

/* get key */
#define internal_key(bh,item_num) ( (struct reiserfs_key *)((bh)->b_data + BLKH_SIZE) + (item_num) )

/* get the key */
#define leaf_key(bh,item_num) ( &(item_head(bh,item_num)->ih_key) )

/* get item body */
#define item_body(bh,item_num) ( (bh)->b_data + get_ih_location (item_head((bh),(item_num))))

/* get the stat data by the buffer header and the item order */
#define B_N_STAT_DATA(bh,nr) \
( (struct stat_data *)((bh)->b_data+get_ih_location(item_head((bh),(nr))) ) )

 /* following defines use reiserfs buffer header and item header */
 /* get item body */
#define ih_item_body(bh,ih) ( (bh)->b_data + get_ih_location(ih))

/* get stat-data */
#define B_I_STAT_DATA(bh, ih) ( (struct stat_data * )ih_item_body(bh,ih) )

#define MAX_DIRECT_ITEM_LEN(size) ((size) - BLKH_SIZE - 2*IH_SIZE - SD_SIZE - UNFM_P_SIZE)
#define MAX_INDIRECT_ITEM_LEN(size) MAX_ITEM_LEN(size)

/* Extended attributes */
/* Magic value in header */
#define REISERFS_XATTR_MAGIC 0x52465841 /* "RFXA" */

struct reiserfs_xattr_header {
	__le32 h_magic;
	__le32 h_hash;
};

/* ACLs */
#define REISERFS_ACL_VERSION 0x0001
struct reiserfs_acl_entry {
	__le16	e_tag;
	__le16	e_perm;
	__le32	e_id;
};

struct reiserfs_acl_entry_short {
	__le16	e_tag;
	__le16	e_perm;
};

struct reiserfs_acl_header {
	__le32	a_version;
};

/***************************************************************************/
/*                    FUNCTION DECLARATIONS                                */
/***************************************************************************/



/* stree.c */
void padd_item (char * item, int total_length, int length);
int B_IS_IN_TREE(const struct buffer_head *);
const struct reiserfs_key *get_rkey(const struct reiserfs_path *p_s_chk_path,
				    const reiserfs_filsys_t );
int bin_search (const void * p_v_key, const void * p_v_base, int p_n_num, int p_n_width, unsigned int * p_n_pos);
int search_by_key (reiserfs_filsys_t , const struct reiserfs_key *, struct reiserfs_path *, int);
int search_by_entry_key (reiserfs_filsys_t , struct reiserfs_key *, struct reiserfs_path *);
int search_for_position_by_key (reiserfs_filsys_t , struct reiserfs_key *, struct reiserfs_path *);
int search_by_objectid (reiserfs_filsys_t , struct reiserfs_key *, struct reiserfs_path *, int *);
void decrement_counters_in_path (struct reiserfs_path *p_s_search_path);
void pathrelse (struct reiserfs_path *p_s_search_path);


int is_left_mergeable (reiserfs_filsys_t s, struct reiserfs_path *path);
int is_right_mergeable (reiserfs_filsys_t s, struct reiserfs_path *path);
int are_items_mergeable (struct item_head * left, struct item_head * right, int bsize);


/* fix_nodes.c */
int fix_nodes (/*struct reiserfs_transaction_handle *th,*/ int n_op_mode, struct tree_balance * p_s_tb, 
               /*int n_pos_in_item,*/ struct item_head * p_s_ins_ih);
void unfix_nodes (/*struct reiserfs_transaction_handle *th,*/ struct tree_balance *);
void free_buffers_in_tb (struct tree_balance * p_s_tb);
void init_path (struct reiserfs_path *);

/* prints.c */
/* options */
#define PRINT_TREE_DETAILS 	0x1	/* print all items from internal tree */
#define PRINT_DETAILS 		0x2       /* print all items from bitmap */
#define PRINT_ITEM_DETAILS 	0x4 	/* print contents of directory items and stat
                                	   data items and indirect items */
#define PRINT_DIRECT_ITEMS 	0x8 	/* print contents of direct items */

void print_tb (int mode, int item_pos, int pos_in_item, struct tree_balance * tb, const char * mes);


void print_bmap (FILE * fp, reiserfs_filsys_t fs, int silent);
void print_objectid_map (FILE * fp, reiserfs_filsys_t fs);



/* lbalance.c */
int leaf_move_items (int shift_mode, struct tree_balance * tb,
                     int mov_num, int mov_bytes, struct buffer_head * Snew);
int leaf_shift_left (struct tree_balance * tb, int shift_num, int shift_bytes);
int leaf_shift_right (struct tree_balance * tb, int shift_num, int shift_bytes);
void leaf_delete_items(struct buffer_info *cur_bi, int last_first, int first,
		       int del_num, int del_bytes);
void leaf_insert_into_buf(struct buffer_info *bi, int before,
			  struct item_head * inserted_item_ih,
			  const char * inserted_item_body, int zeros_number);
void leaf_paste_in_buffer(struct buffer_info *bi, int pasted_item_num,
			  int pos_in_item, int paste_size, const char * body,
			  int zeros_number);
void leaf_cut_from_buffer(struct buffer_info *bi, int cut_item_num,
                           int pos_in_item, int cut_size);
void leaf_paste_entries (struct buffer_head * bh, int item_num, int before, int new_entry_count,
			 struct reiserfs_de_head * new_dehs, const char * records,
			 int paste_size);
void delete_item (reiserfs_filsys_t , struct buffer_head * bh, int item_num);
void cut_entry (reiserfs_filsys_t , struct buffer_head * bh,
		int item_num, int entry_num, int del_count);


/* ibalance.c */
int balance_internal (struct tree_balance * , int, int, struct item_head * ,
                      struct buffer_head **);

/* do_balance.c */
void do_balance (struct tree_balance * tb,
                 struct item_head * ih, const char * body, int flag, int zeros_num);
void reiserfs_invalidate_buffer (struct tree_balance * tb, struct buffer_head * bh);
int get_left_neighbor_position (const struct tree_balance * tb, int h);
int get_right_neighbor_position (const struct tree_balance * tb, int h);
void replace_key (reiserfs_filsys_t , struct buffer_head *, int, struct buffer_head *, int);
void replace_lkey (struct tree_balance *, int, struct item_head *);
void replace_rkey (struct tree_balance *, int, struct item_head *);
void make_empty_node (struct buffer_info *);
void make_empty_leaf (struct buffer_head *);
struct buffer_head * get_FEB (struct tree_balance *);


__u32 get_bytes_number (struct item_head * ih, int blocksize);




/* hashes.c */
__u32 keyed_hash (const char *msg, int len);
__u32 yura_hash (const char *msg, int len);
__u32 r5_hash (const char *msg, int len);



/* node_format.c */
extern unsigned int get_journal_old_start_must (reiserfs_filsys_t fs);
extern unsigned int get_journal_new_start_must (reiserfs_filsys_t fs);
extern unsigned int get_journal_start_must (reiserfs_filsys_t fs);
/*extern hashf_t hashes [];*/

static inline void buffer_info_init_left(const struct tree_balance *tb,
					 struct buffer_info *bi, int h)
{
	bi->bi_fs	= tb->tb_fs;
	bi->bi_bh	= tb->L[h];
	bi->bi_parent	= tb->FL[h];
	bi->bi_position = get_left_neighbor_position(tb, h);
}

static inline void buffer_info_init_right(const struct tree_balance *tb,
					  struct buffer_info *bi, int h)
{
	bi->bi_fs	= tb->tb_fs;
	bi->bi_bh	= tb->R[h];
	bi->bi_parent	= tb->FR[h];
	bi->bi_position = get_right_neighbor_position(tb, h);
}

static inline void buffer_info_init_last(const struct tree_balance *tb,
					 struct buffer_info *bi)
{
	bi->bi_fs	= tb->tb_fs;
	bi->bi_bh	= PATH_PLAST_BUFFER(tb->tb_path);
	bi->bi_parent	= PATH_H_PPARENT(tb->tb_path, 0);
	bi->bi_position	= PATH_H_B_ITEM_ORDER(tb->tb_path, 0);
}

static inline void buffer_info_init_tbSh(const struct tree_balance *tb,
					 struct buffer_info *bi, int h)
{
	bi->bi_fs	= tb->tb_fs;
	bi->bi_bh	= PATH_H_PBUFFER(tb->tb_path, h);
	bi->bi_parent	= PATH_H_PPARENT(tb->tb_path, h);
	bi->bi_position = PATH_H_POSITION(tb->tb_path, h + 1);
}

static inline void buffer_info_init_tbS0(const struct tree_balance *tb,
					 struct buffer_info *bi)
{
	buffer_info_init_tbSh(tb, bi, 0);
}

static inline void buffer_info_init_bh(const struct tree_balance *tb,
				       struct buffer_info *bi,
				       struct buffer_head *bh)
{
	bi->bi_fs	= tb ? tb->tb_fs : NULL;
	bi->bi_bh	= bh;
	bi->bi_parent	= NULL;
	bi->bi_position = 0;
}

static inline __u64 get_key_offset_v2(const struct reiserfs_key *key)
{
	const struct offset_v2 *v2 = &key->u.k2_offset_v2;
	return le64_to_cpu(v2->v) & (~0ULL >> 4);
}

static inline __u32 get_key_type_v2(const struct reiserfs_key *key)
{
	const struct offset_v2 *v2 = &key->u.k2_offset_v2;
	char type = le64_to_cpu(v2->v) >> 60;
	return (type <= TYPE_MAXTYPE) ? type : TYPE_UNKNOWN;
}

static inline void set_key_offset_v2(struct reiserfs_key *key, __u64 offset)
{
	struct offset_v2 *v2 = &key->u.k2_offset_v2;
	offset &= (~0ULL >> 4);
	v2->v = (v2->v & cpu_to_le64(15ULL << 60)) | cpu_to_le64(offset);
}

static inline void set_key_type_v2(struct reiserfs_key *key, __u32 type)
{
	struct offset_v2 *v2 = &key->u.k2_offset_v2;
	__u64 type64 = type;
	v2->v = (v2->v & cpu_to_le64(~0ULL >> 4)) | cpu_to_le64(type64 << 60);
}
#endif

/*
 * Local variables:
 * c-indentation-style: "bsd"
 * c-basic-offset: 4
 * tab-width: 8
 * fill-column: 78
 * End:
 */
