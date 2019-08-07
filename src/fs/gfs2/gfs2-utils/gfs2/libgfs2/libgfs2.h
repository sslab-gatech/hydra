#ifndef __LIBGFS2_DOT_H__
#define __LIBGFS2_DOT_H__

#include <features.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <linux/types.h>
#include <linux/limits.h>
#include <endian.h>
#include <byteswap.h>
#include <mntent.h>

#include <linux/gfs2_ondisk.h>
#include "osi_list.h"
#include "osi_tree.h"

__BEGIN_DECLS

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#if __BYTE_ORDER == __BIG_ENDIAN

#define be16_to_cpu(x) (x)
#define be32_to_cpu(x) (x)
#define be64_to_cpu(x) (x)

#define cpu_to_be16(x) (x)
#define cpu_to_be32(x) (x)
#define cpu_to_be64(x) (x)

#define le16_to_cpu(x) (bswap_16((x)))
#define le32_to_cpu(x) (bswap_32((x)))
#define le64_to_cpu(x) (bswap_64((x)))

#define cpu_to_le16(x) (bswap_16((x)))
#define cpu_to_le32(x) (bswap_32((x)))
#define cpu_to_le64(x) (bswap_64((x)))

#endif  /*  __BYTE_ORDER == __BIG_ENDIAN  */


#if __BYTE_ORDER == __LITTLE_ENDIAN

#define be16_to_cpu(x) (bswap_16((x)))
#define be32_to_cpu(x) (bswap_32((x)))
#define be64_to_cpu(x) (bswap_64((x)))

#define cpu_to_be16(x) (bswap_16((x)))
#define cpu_to_be32(x) (bswap_32((x)))
#define cpu_to_be64(x) (bswap_64((x))) 

#define le16_to_cpu(x) (x)
#define le32_to_cpu(x) (x)
#define le64_to_cpu(x) (x)

#define cpu_to_le16(x) (x)
#define cpu_to_le32(x) (x)
#define cpu_to_le64(x) (x)

#endif  /*  __BYTE_ORDER == __LITTLE_ENDIAN  */

enum lgfs2_meta_type {
	LGFS2_MT_GFS2_SB = 0,
	LGFS2_MT_GFS_SB = 1,
	LGFS2_MT_RINDEX = 2,
	LGFS2_MT_GFS2_RGRP = 3,
	LGFS2_MT_GFS_RGRP = 4,
	LGFS2_MT_RGRP_BITMAP = 5,
	LGFS2_MT_GFS2_DINODE = 6,
	LGFS2_MT_GFS_DINODE = 7,
	LGFS2_MT_GFS2_INDIRECT = 8,
	LGFS2_MT_GFS_INDIRECT = 9,
	LGFS2_MT_DIR_LEAF = 10,
	LGFS2_MT_JRNL_DATA = 11,
	LGFS2_MT_GFS2_LOG_HEADER = 12,
	LGFS2_MT_GFS_LOG_HEADER = 13,
	LGFS2_MT_GFS2_LOG_DESC = 14,
	LGFS2_MT_GFS_LOG_DESC = 15,
	LGFS2_MT_GFS2_LOG_BLOCK = 16,
	LGFS2_MT_EA_ATTR = 17,
	LGFS2_MT_EA_DATA = 18,
	LGFS2_MT_GFS2_QUOTA_CHANGE = 19,
	LGFS2_MT_DIRENT = 20,
	LGFS2_MT_EA_HEADER = 21,
	LGFS2_MT_GFS2_INUM_RANGE = 22,
	LGFS2_MT_STATFS_CHANGE = 23,
	LGFS2_MT_GFS_JINDEX = 24,
	LGFS2_MT_GFS_BLOCK_TAG = 25,
	LGFS2_MT_DATA = 26,
	LGFS2_MT_FREE = 27,

	LGFS2_MT_NR,
};

struct lgfs2_symbolic {
	const uint32_t key;
	const char *value;
};

struct lgfs2_metafield {
	const char *name;
	const unsigned offset;
	const unsigned length;
	const unsigned flags;

#define LGFS2_MFF_RESERVED 0x00001	/* Field is reserved */
#define LGFS2_MFF_POINTER  0x00002	/* Field is a pointer to a block */
#define LGFS2_MFF_ENUM     0x00004	/* Field is an enum */
#define LGFS2_MFF_MASK     0x00008	/* Field is a bitmask */
#define LGFS2_MFF_UUID     0x00010	/* Field is a UUID */
#define LGFS2_MFF_STRING   0x00020	/* Field in an ASCII string */
#define LGFS2_MFF_UID      0x00040	/* Field is a UID */
#define LGFS2_MFF_GID      0x00080	/* Field is a GID */
#define LGFS2_MFF_MODE     0x00100	/* Field is a file mode */
#define LGFS2_MFF_FSBLOCKS 0x00200	/* Units are fs blocks */
#define LGFS2_MFF_BYTES    0x00400	/* Units are bytes */
#define LGFS2_MFF_SHIFT    0x00800	/* Log_{2} quantity */
#define LGFS2_MFF_CHECK    0x01000	/* Field is a checksum */
#define LGFS2_MFF_SECS     0x02000	/* Units are seconds */
#define LGFS2_MFF_NSECS    0x04000	/* Units are nsecs */
#define LGFS2_MFF_MAJOR    0x08000	/* Major device number */
#define LGFS2_MFF_MINOR    0x10000	/* Minor device number */

	/* If it is a pointer, then this field must be set */
	const unsigned points_to;
	/* If isenum or ismask are set, these must also be filled in */
	const struct lgfs2_symbolic *symtab;
	const unsigned nsyms;
};

struct lgfs2_metadata {
	const unsigned versions:2;
#define LGFS2_MD_GFS1 0x01
#define LGFS2_MD_GFS2 0x02
	const unsigned header:1;
	const uint32_t mh_type;
	const uint32_t mh_format;
	const char *name;
	const struct lgfs2_metafield *fields;
	const unsigned nfields;
	const unsigned size;
};

struct lgfs2_dev_info {
	struct stat stat;
	unsigned readonly:1;
	long ra_pages;
	int soft_block_size;
	int logical_block_size;
	unsigned int physical_block_size;
	unsigned int io_min_size;
	unsigned int io_optimal_size;
	int io_align_offset;
	uint64_t size;
};

struct device {
	uint64_t length;
};

struct gfs2_bitmap
{
	struct gfs2_buffer_head *bi_bh;
	uint32_t   bi_offset;  /* The offset in the buffer of the first byte */
	uint32_t   bi_start;   /* The position of the first byte in this block */
	uint32_t   bi_len;     /* The number of bytes in this block */
};

struct gfs2_sbd;
struct gfs2_inode;
typedef struct _lgfs2_rgrps *lgfs2_rgrps_t;

struct rgrp_tree {
	struct osi_node node;
	uint64_t start;	   /* The offset of the beginning of this resource group */
	uint64_t length;	/* The length of this resource group */

	struct gfs2_rindex ri;
	struct gfs2_rgrp rg;
	struct gfs2_bitmap *bits;
	lgfs2_rgrps_t rgrps;
};

typedef struct rgrp_tree *lgfs2_rgrp_t;

extern lgfs2_rgrps_t lgfs2_rgrps_init(struct gfs2_sbd *sdp, uint64_t align, uint64_t offset);
extern void lgfs2_rgrps_free(lgfs2_rgrps_t *rgs);
extern uint64_t lgfs2_rindex_entry_new(lgfs2_rgrps_t rgs, struct gfs2_rindex *entry, uint64_t addr, uint32_t len);
extern unsigned lgfs2_rindex_read_fd(int fd, lgfs2_rgrps_t rgs);
extern const struct gfs2_rindex *lgfs2_rindex_read_one(struct gfs2_inode *rip, lgfs2_rgrps_t rgs, unsigned i);
extern uint64_t lgfs2_rgrp_align_addr(const lgfs2_rgrps_t rgs, uint64_t addr);
extern uint32_t lgfs2_rgrp_align_len(const lgfs2_rgrps_t rgs, uint32_t len);
extern unsigned lgfs2_rgsize_for_data(uint64_t blksreq, unsigned bsize);
extern uint32_t lgfs2_rgrps_plan(const lgfs2_rgrps_t rgs, uint64_t space, uint32_t tgtsize);
extern lgfs2_rgrp_t lgfs2_rgrps_append(lgfs2_rgrps_t rgs, struct gfs2_rindex *entry, uint32_t rg_skip);
extern int lgfs2_rgrp_bitbuf_alloc(lgfs2_rgrp_t rg);
extern void lgfs2_rgrp_bitbuf_free(lgfs2_rgrp_t rg);
extern int lgfs2_rgrp_write(int fd, lgfs2_rgrp_t rg);
extern const struct gfs2_rindex *lgfs2_rgrp_index(lgfs2_rgrp_t rg);
extern const struct gfs2_rgrp *lgfs2_rgrp_rgrp(lgfs2_rgrp_t rg);
extern lgfs2_rgrp_t lgfs2_rgrp_first(lgfs2_rgrps_t rgs);
extern lgfs2_rgrp_t lgfs2_rgrp_last(lgfs2_rgrps_t rgs);
extern lgfs2_rgrp_t lgfs2_rgrp_next(lgfs2_rgrp_t rg);
extern lgfs2_rgrp_t lgfs2_rgrp_prev(lgfs2_rgrp_t rg);
// Temporary function to aid API migration
extern struct osi_node *lgfs2_rgrps_root(lgfs2_rgrps_t rgs) __attribute__((deprecated));

struct gfs2_buffer_head {
	osi_list_t b_altlist; /* alternate list */
	uint64_t b_blocknr;
	union {
		char *b_data;
		struct iovec iov;
	};
	struct gfs2_sbd *sdp;
	int b_modified;
};

struct special_blocks {
	osi_list_t list;
	uint64_t block;
};

struct gfs2_inode {
	struct gfs2_dinode i_di;
	struct gfs2_buffer_head *i_bh;
	struct gfs2_sbd *i_sbd;
	struct rgrp_tree *i_rgd; /* performance hint */
	int bh_owned; /* Is this bh owned, iow, should we release it later? */
};

struct master_dir
{
	struct gfs2_inode *inum;
	uint64_t next_inum;
	struct gfs2_inode *statfs;
	struct gfs2_inode *qinode;

	struct gfs2_inode       *jiinode;
	struct gfs2_inode       *riinode;
	struct gfs2_inode       *rooti;
	struct gfs2_inode       *pinode;

	struct gfs2_inode **journal;      /* Array of journals */
	uint32_t journals;                /* Journal count */
};

#define LGFS2_SB_ADDR(sdp) (GFS2_SB_ADDR >> (sdp)->sd_fsb2bb_shift)
struct gfs2_sbd {
	struct gfs2_sb sd_sb;    /* a copy of the ondisk structure */

	unsigned int bsize;	     /* The block size of the FS (in bytes) */
	unsigned int jsize;	     /* Size of journals (in MB) */
	unsigned int rgsize;     /* Size of resource groups (in MB) */
	unsigned int qcsize;     /* Size of quota change files (in MB) */

	/* Constants */

	uint32_t sd_fsb2bb;
	uint32_t sd_fsb2bb_shift;
	uint32_t sd_diptrs;
	uint32_t sd_inptrs;
	uint32_t sd_jbsize;
	uint32_t sd_hash_bsize;
	uint32_t sd_hash_bsize_shift;
	uint32_t sd_hash_ptrs;
	uint32_t sd_blocks_per_bitmap;
	uint32_t sd_max_dirres;
	uint32_t sd_max_height;
	uint32_t sd_max_jheight;
	uint64_t sd_heightsize[GFS2_MAX_META_HEIGHT];
	uint64_t sd_jheightsize[GFS2_MAX_META_HEIGHT];

	/* Not specified on the command line, but... */

	int64_t time;

	struct lgfs2_dev_info dinfo;
	struct device device;

	int device_fd;
	int path_fd;

	uint64_t fssize;
	uint64_t blks_total;
	uint64_t blks_alloced;
	uint64_t dinodes_alloced;

	uint64_t orig_rgrps;
	uint64_t rgrps;
	uint64_t new_rgrps;
	struct osi_root rgtree;
	struct osi_root rgcalc;

	struct gfs2_inode *master_dir;
	struct master_dir md;

	uint64_t rg_one_length;
	uint64_t rg_length;
	int gfs1;
};

struct metapath {
	unsigned int mp_list[GFS2_MAX_META_HEIGHT];
};


#define GFS2_DEFAULT_BSIZE          (4096)
#define GFS2_DEFAULT_JSIZE          (128)
#define GFS2_MAX_JSIZE              (1024)
#define GFS2_MIN_JSIZE              (8)
#define GFS2_DEFAULT_RGSIZE         (256)
#define GFS2_DEFAULT_UTSIZE         (1)
#define GFS2_DEFAULT_QCSIZE         (1)
#define GFS2_DEFAULT_LOCKPROTO      "lock_dlm"
#define GFS2_MIN_GROW_SIZE          (10)
#define GFS2_EXCESSIVE_RGS          (10000)

#define GFS2_MIN_RGSIZE             (32)
#define GFS2_MAX_RGSIZE             (2048)

/* meta.c */
extern const struct lgfs2_metadata lgfs2_metadata[];
extern const unsigned lgfs2_metadata_size;
extern const struct lgfs2_symbolic lgfs2_metatypes[];
extern const unsigned lgfs2_metatype_size;
extern const struct lgfs2_symbolic lgfs2_metaformats[];
extern const unsigned lgfs2_metaformat_size;
extern const struct lgfs2_symbolic lgfs2_di_flags[];
extern const unsigned lgfs2_di_flag_size;
extern const struct lgfs2_symbolic lgfs2_lh_flags[];
extern const unsigned lgfs2_lh_flag_size;
extern const struct lgfs2_symbolic lgfs2_ld_types[];
extern const unsigned lgfs2_ld_type_size;
extern const struct lgfs2_symbolic lgfs2_ld1_types[];
extern const unsigned lgfs2_ld1_type_size;
extern int lgfs2_selfcheck(void);
extern const struct lgfs2_metadata *lgfs2_find_mtype(uint32_t mh_type, const unsigned versions);
extern const struct lgfs2_metadata *lgfs2_find_mtype_name(const char *name, const unsigned versions);
extern const struct lgfs2_metafield *lgfs2_find_mfield_name(const char *name, const struct lgfs2_metadata *mtype);
extern int lgfs2_field_str(char *str, const size_t size, const char *blk, const struct lgfs2_metafield *field, int hex);
extern int lgfs2_field_assign(char *blk, const struct lgfs2_metafield *field, const void *val);

/* block_list.c */

extern struct special_blocks *blockfind(struct special_blocks *blist, uint64_t num);
extern void gfs2_special_add(struct special_blocks *blocklist, uint64_t block);
extern void gfs2_special_set(struct special_blocks *blocklist, uint64_t block);
extern void gfs2_special_free(struct special_blocks *blist);
extern void gfs2_special_clear(struct special_blocks *blocklist,
			       uint64_t block);

/* buf.c */
extern struct gfs2_buffer_head *bget(struct gfs2_sbd *sdp, uint64_t num);
extern struct gfs2_buffer_head *__bread(struct gfs2_sbd *sdp, uint64_t num,
					int line, const char *caller);
extern int __breadm(struct gfs2_sbd *sdp, struct gfs2_buffer_head **bhs, size_t n, uint64_t block, int line, const char *caller);
extern int bwrite(struct gfs2_buffer_head *bh);
extern int brelse(struct gfs2_buffer_head *bh);
extern uint32_t lgfs2_get_block_type(const struct gfs2_buffer_head *lbh);

#define bmodified(bh) do { bh->b_modified = 1; } while(0)

#define bread(bl, num) __bread(bl, num, __LINE__, __FUNCTION__)
#define breadm(bl, bhs, n, block) __breadm(bl, bhs, n, block, __LINE__, __FUNCTION__)

/* config.c */
extern void lgfs2_set_debug(int enable);

/* device_geometry.c */
extern int lgfs2_get_dev_info(int fd, struct lgfs2_dev_info *i);
extern void fix_device_geometry(struct gfs2_sbd *sdp);

/* fs_bits.c */
#define BFITNOENT (0xFFFFFFFF)

/* functions with blk #'s that are buffer relative */
extern unsigned long gfs2_bitfit(const unsigned char *buffer,
				 const unsigned int buflen,
				 unsigned long goal, unsigned char old_state);

/* functions with blk #'s that are rgrp relative */
extern uint32_t gfs2_blkalloc_internal(struct rgrp_tree *rgd, uint32_t goal,
				       unsigned char old_state,
				       unsigned char new_state, int do_it);
extern int gfs2_check_range(struct gfs2_sbd *sdp, uint64_t blkno);

/* functions with blk #'s that are file system relative */
extern int lgfs2_get_bitmap(struct gfs2_sbd *sdp, uint64_t blkno, struct rgrp_tree *rgd);
extern int gfs2_set_bitmap(lgfs2_rgrp_t rg, uint64_t blkno, int state);

/* fs_geometry.c */
extern uint32_t rgblocks2bitblocks(const unsigned int bsize, const uint32_t rgblocks,
                                    uint32_t *ri_data) __attribute__((nonnull(3)));
extern int build_rgrps(struct gfs2_sbd *sdp, int write);

/* fs_ops.c */
#define IS_LEAF     (1)
#define IS_DINODE   (2)

extern void find_metapath(struct gfs2_inode *ip, uint64_t block, struct metapath *mp);
extern void lookup_block(struct gfs2_inode *ip, struct gfs2_buffer_head *bh,
			 unsigned int height, struct metapath *mp,
			 int create, int *new, uint64_t *block);
extern struct gfs2_inode *lgfs2_inode_get(struct gfs2_sbd *sdp,
				    struct gfs2_buffer_head *bh);
extern struct gfs2_inode *lgfs2_inode_read(struct gfs2_sbd *sdp, uint64_t di_addr);
extern struct gfs2_inode *is_system_inode(struct gfs2_sbd *sdp,
					  uint64_t block);
extern void inode_put(struct gfs2_inode **ip);
extern uint64_t data_alloc(struct gfs2_inode *ip);
extern int lgfs2_meta_alloc(struct gfs2_inode *ip, uint64_t *blkno);
extern int lgfs2_dinode_alloc(struct gfs2_sbd *sdp, const uint64_t blksreq, uint64_t *blkno);
extern uint64_t lgfs2_space_for_data(const struct gfs2_sbd *sdp, unsigned bsize, uint64_t bytes);
extern int lgfs2_file_alloc(lgfs2_rgrp_t rg, uint64_t di_size, struct gfs2_inode *ip, uint32_t flags, unsigned mode);

extern int gfs2_readi(struct gfs2_inode *ip, void *buf, uint64_t offset,
		      unsigned int size);
#define gfs2_writei(ip, buf, offset, size) \
	__gfs2_writei(ip, buf, offset, size, 1)
extern int __gfs2_writei(struct gfs2_inode *ip, void *buf, uint64_t offset,
			 unsigned int size, int resize);
extern struct gfs2_buffer_head *get_file_buf(struct gfs2_inode *ip,
					     uint64_t lbn, int prealloc);
extern int init_dinode(struct gfs2_sbd *sdp, struct gfs2_buffer_head **bhp, struct gfs2_inum *inum,
                       unsigned int mode, uint32_t flags, struct gfs2_inum *parent);
extern struct gfs2_inode *createi(struct gfs2_inode *dip, const char *filename,
				  unsigned int mode, uint32_t flags);
extern struct gfs2_inode *gfs_createi(struct gfs2_inode *dip,
				      const char *filename, unsigned int mode,
				      uint32_t flags);
extern void dirent2_del(struct gfs2_inode *dip, struct gfs2_buffer_head *bh,
			struct gfs2_dirent *prev, struct gfs2_dirent *cur);
extern int dir_search(struct gfs2_inode *dip, const char *filename, int len,
		      unsigned int *type, struct gfs2_inum *inum);
extern int gfs2_lookupi(struct gfs2_inode *dip, const char *filename, int len,
			struct gfs2_inode **ipp);
extern int dir_add(struct gfs2_inode *dip, const char *filename, int len,
		    struct gfs2_inum *inum, unsigned int type);
extern int gfs2_dirent_del(struct gfs2_inode *dip, const char *filename,
			   int filename_len);
extern void block_map(struct gfs2_inode *ip, uint64_t lblock, int *new,
		      uint64_t *dblock, uint32_t *extlen, int prealloc);
extern int lgfs2_get_leaf_ptr(struct gfs2_inode *dip, uint32_t index, uint64_t *ptr) __attribute__((warn_unused_result));
extern void dir_split_leaf(struct gfs2_inode *dip, uint32_t start,
			   uint64_t leaf_no, struct gfs2_buffer_head *obh);
extern void gfs2_free_block(struct gfs2_sbd *sdp, uint64_t block);
extern int gfs2_freedi(struct gfs2_sbd *sdp, uint64_t block);
extern int gfs2_get_leaf(struct gfs2_inode *dip, uint64_t leaf_no,
			 struct gfs2_buffer_head **bhp);
extern int gfs2_dirent_first(struct gfs2_inode *dip,
			     struct gfs2_buffer_head *bh,
			     struct gfs2_dirent **dent);
extern int gfs2_dirent_next(struct gfs2_inode *dip, struct gfs2_buffer_head *bh,
			    struct gfs2_dirent **dent);
extern void build_height(struct gfs2_inode *ip, int height);
extern void unstuff_dinode(struct gfs2_inode *ip);
extern unsigned int calc_tree_height(struct gfs2_inode *ip, uint64_t size);
extern int write_journal(struct gfs2_inode *jnl, unsigned bsize, unsigned blocks);
extern int lgfs2_write_journal_data(struct gfs2_inode *ip);
extern int lgfs2_write_filemeta(struct gfs2_inode *ip);
extern uint32_t lgfs2_log_header_hash(char *buf);
extern uint32_t lgfs2_log_header_crc(char *buf, unsigned bsize);

/* gfs1.c - GFS1 backward compatibility structures and functions */

#define GFS_FORMAT_SB           (100)  /* Super-Block */
#define GFS_METATYPE_SB         (1)    /* Super-Block */
#define GFS_FORMAT_FS           (1309) /* Filesystem (all-encompassing) */
#define GFS_FORMAT_MULTI        (1401) /* Multi-Host */
/* GFS1 Dinode types  */
#define GFS_FILE_NON            (0)
#define GFS_FILE_REG            (1)    /* regular file */
#define GFS_FILE_DIR            (2)    /* directory */
#define GFS_FILE_LNK            (5)    /* link */
#define GFS_FILE_BLK            (7)    /* block device node */
#define GFS_FILE_CHR            (8)    /* character device node */
#define GFS_FILE_FIFO           (101)  /* fifo/pipe */
#define GFS_FILE_SOCK           (102)  /* socket */

/* GFS 1 journal block types: */
#define GFS_LOG_DESC_METADATA   (300)    /* metadata */
#define GFS_LOG_DESC_IUL        (400)    /* unlinked inode */
#define GFS_LOG_DESC_IDA        (401)    /* de-allocated inode */
#define GFS_LOG_DESC_Q          (402)    /* quota */
#define GFS_LOG_DESC_LAST       (500)    /* final in a logged transaction */

struct gfs_indirect {
	struct gfs2_meta_header in_header;

	char in_reserved[64];
};

struct gfs_dinode {
	struct gfs2_meta_header di_header;

	struct gfs2_inum di_num; /* formal inode # and block address */

	__be32 di_mode;	/* mode of file */
	__be32 di_uid;	/* owner's user id */
	__be32 di_gid;	/* owner's group id */
	__be32 di_nlink;	/* number (qty) of links to this file */
	__be64 di_size;	/* number (qty) of bytes in file */
	__be64 di_blocks;	/* number (qty) of blocks in file */
	__be64 di_atime;	/* time last accessed */
	__be64 di_mtime;	/* time last modified */
	__be64 di_ctime;	/* time last changed */

	/*  Non-zero only for character or block device nodes  */
	__be32 di_major;	/* device major number */
	__be32 di_minor;	/* device minor number */

	/*  Block allocation strategy  */
	__be64 di_rgrp;	/* dinode rgrp block number */
	__be64 di_goal_rgrp;	/* rgrp to alloc from next */
	__be32 di_goal_dblk;	/* data block goal */
	__be32 di_goal_mblk;	/* metadata block goal */

	__be32 di_flags;	/* GFS_DIF_... */

	/*  struct gfs_rindex, struct gfs_jindex, or struct gfs_dirent */
	__be32 di_payload_format;  /* GFS_FORMAT_... */
	__be16 di_type;	/* GFS_FILE_... type of file */
	__be16 di_height;	/* height of metadata (0 == stuffed) */
	__be32 di_incarn;	/* incarnation (unused, see gfs_meta_header) */
	__be16 di_pad;

	/*  These only apply to directories  */
	__be16 di_depth;	/* Number of bits in the table */
	__be32 di_entries;	/* The # (qty) of entries in the directory */

	/*  This formed an on-disk chain of unused dinodes  */
	struct gfs2_inum di_next_unused;  /* used in old versions only */

	__be64 di_eattr;	/* extended attribute block number */

	char di_reserved[56];
};

struct gfs_sb {
	/*  Order is important; need to be able to read old superblocks
	    in order to support on-disk version upgrades */
	struct gfs2_meta_header sb_header;

	__be32 sb_fs_format;         /* GFS_FORMAT_FS (on-disk version) */
	__be32 sb_multihost_format;  /* GFS_FORMAT_MULTI */
	__be32 sb_flags;             /* ?? */

	__be32 sb_bsize;             /* fundamental FS block size in bytes */
	__be32 sb_bsize_shift;       /* log2(sb_bsize) */
	__be32 sb_seg_size;          /* Journal segment size in FS blocks */

	/* These special inodes do not appear in any on-disk directory. */
	struct gfs2_inum sb_jindex_di;  /* journal index inode */
	struct gfs2_inum sb_rindex_di;  /* resource group index inode */
	struct gfs2_inum sb_root_di;    /* root directory inode */

	/* Default inter-node locking protocol (lock module) and namespace */
	uint8_t sb_lockproto[GFS2_LOCKNAME_LEN]; /* lock protocol name */
	uint8_t sb_locktable[GFS2_LOCKNAME_LEN]; /* unique name for this FS */

	/* More special inodes */
	struct gfs2_inum sb_quota_di;   /* quota inode */
	struct gfs2_inum sb_license_di; /* license inode */

	char sb_reserved[96];
};

struct gfs_rgrp {
	struct gfs2_meta_header rg_header;

	__be32 rg_flags;
	__be32 rg_free;       /* Number (qty) of free data blocks */

	/* Dinodes are USEDMETA, but are handled separately from other METAs */
	__be32 rg_useddi;     /* Number (qty) of dinodes (used or free) */
	__be32 rg_freedi;     /* Number (qty) of unused (free) dinodes */
	struct gfs2_inum rg_freedi_list; /* 1st block in chain of free dinodes */

	/* These META statistics do not include dinodes (used or free) */
	__be32 rg_usedmeta;   /* Number (qty) of used metadata blocks */
	__be32 rg_freemeta;   /* Number (qty) of unused metadata blocks */

	char rg_reserved[64];
};

struct gfs_log_header {
	struct gfs2_meta_header lh_header;

	__be32 lh_flags;      /* GFS_LOG_HEAD_... */
	__be32 lh_pad;

	__be64 lh_first;     /* Block number of first header in this trans */
	__be64 lh_sequence;   /* Sequence number of this transaction */

	__be64 lh_tail;       /* Block number of log tail */
	__be64 lh_last_dump;  /* Block number of last dump */

	uint8_t lh_reserved[64];
};

struct gfs_jindex {
        __be64 ji_addr;       /* starting block of the journal */
        __be32 ji_nsegment;   /* number (quantity) of segments in journal */
        __be32 ji_pad;

        uint8_t ji_reserved[64];
};

struct gfs_log_descriptor {
	struct gfs2_meta_header ld_header;

	__be32 ld_type;       /* GFS_LOG_DESC_... Type of this log chunk */
	__be32 ld_length;     /* Number of buffers in this chunk */
	__be32 ld_data1;      /* descriptor-specific field */
	__be32 ld_data2;      /* descriptor-specific field */
	uint8_t ld_reserved[64];
};

extern int is_gfs_dir(struct gfs2_dinode *dinode);
extern void gfs1_lookup_block(struct gfs2_inode *ip,
			      struct gfs2_buffer_head *bh,
			      unsigned int height, struct metapath *mp,
			      int create, int *new, uint64_t *block);
extern void gfs1_block_map(struct gfs2_inode *ip, uint64_t lblock, int *new,
			   uint64_t *dblock, uint32_t *extlen, int prealloc);
extern int gfs1_writei(struct gfs2_inode *ip, char *buf, uint64_t offset,
		       unsigned int size);
extern int gfs1_ri_update(struct gfs2_sbd *sdp, int fd, int *rgcount, int quiet);
extern struct gfs2_inode *lgfs2_gfs_inode_get(struct gfs2_sbd *sdp,
					struct gfs2_buffer_head *bh);
extern struct gfs2_inode *lgfs2_gfs_inode_read(struct gfs2_sbd *sdp,
					 uint64_t di_addr);
extern void gfs_jindex_in(struct gfs_jindex *jindex, char *buf);
extern void gfs_rgrp_in(struct gfs_rgrp *rg, struct gfs2_buffer_head *bh);
extern void gfs_rgrp_out(struct gfs_rgrp *rg, struct gfs2_buffer_head *bh);

/* misc.c */
extern int compute_heightsize(unsigned bsize, uint64_t *heightsize,
		uint32_t *maxheight, uint32_t bsize1, int diptrs, int inptrs);
extern int compute_constants(struct gfs2_sbd *sdp);
extern int lgfs2_open_mnt(const char *path, int dirflags, int *dirfd, int devflags, int *devfd, struct mntent **mnt);
extern int lgfs2_open_mnt_dev(const char *path, int flags, struct mntent **mnt);
extern int lgfs2_open_mnt_dir(const char *path, int flags, struct mntent **mnt);

/* recovery.c */
extern void gfs2_replay_incr_blk(struct gfs2_inode *ip, unsigned int *blk);
extern int gfs2_replay_read_block(struct gfs2_inode *ip, unsigned int blk,
				  struct gfs2_buffer_head **bh);
extern int gfs2_revoke_add(struct gfs2_sbd *sdp, uint64_t blkno, unsigned int where);
extern int gfs2_revoke_check(struct gfs2_sbd *sdp, uint64_t blkno,
			     unsigned int where);
extern void gfs2_revoke_clean(struct gfs2_sbd *sdp);
extern int get_log_header(struct gfs2_inode *ip, unsigned int blk,
			  struct gfs2_log_header *head);
extern int gfs2_find_jhead(struct gfs2_inode *ip, struct gfs2_log_header *head);
extern int clean_journal(struct gfs2_inode *ip, struct gfs2_log_header *head);

/* rgrp.c */
extern int gfs2_compute_bitstructs(const uint32_t bsize, struct rgrp_tree *rgd);
extern struct rgrp_tree *gfs2_blk2rgrpd(struct gfs2_sbd *sdp, uint64_t blk);
extern int lgfs2_rgrp_crc_check(char *buf);
extern void lgfs2_rgrp_crc_set(char *buf);
extern uint64_t gfs2_rgrp_read(struct gfs2_sbd *sdp, struct rgrp_tree *rgd);
extern void gfs2_rgrp_relse(struct rgrp_tree *rgd);
extern struct rgrp_tree *rgrp_insert(struct osi_root *rgtree,
				     uint64_t rgblock);
extern void gfs2_rgrp_free(struct osi_root *rgrp_tree);
/* figure out the size of the given resource group, in blocks */
static inline unsigned int rgrp_size(struct rgrp_tree *rgrp)
{
	return rgrp->ri.ri_data + rgrp->ri.ri_length;
}

/* structures.c */
extern int build_master(struct gfs2_sbd *sdp);
extern void lgfs2_sb_init(struct gfs2_sb *sb, unsigned bsize);
extern int lgfs2_sb_write(const struct gfs2_sb *sb, int fd, const unsigned bsize);
extern int build_journal(struct gfs2_sbd *sdp, int j,
			 struct gfs2_inode *jindex);
extern int build_jindex(struct gfs2_sbd *sdp);
extern int lgfs2_build_jindex(struct gfs2_inode *master, struct gfs2_inum *jnls, size_t nmemb);
extern int build_per_node(struct gfs2_sbd *sdp);
extern int build_inum(struct gfs2_sbd *sdp);
extern int build_statfs(struct gfs2_sbd *sdp);
extern int build_rindex(struct gfs2_sbd *sdp);
extern int build_quota(struct gfs2_sbd *sdp);
extern int build_root(struct gfs2_sbd *sdp);
extern int do_init_inum(struct gfs2_sbd *sdp);
extern int do_init_statfs(struct gfs2_sbd *sdp);
extern int gfs2_check_meta(struct gfs2_buffer_head *bh, int type);
extern unsigned lgfs2_bm_scan(struct rgrp_tree *rgd, unsigned idx,
			      uint64_t *buf, uint8_t state);
extern int build_inum_range(struct gfs2_inode *per_node, unsigned int j);
extern int build_statfs_change(struct gfs2_inode *per_node, unsigned int j);
extern int build_quota_change(struct gfs2_inode *per_node, unsigned int j);

/* super.c */
extern int check_sb(struct gfs2_sb *sb);
extern int read_sb(struct gfs2_sbd *sdp);
extern int rindex_read(struct gfs2_sbd *sdp, int fd, uint64_t *count1,
		       int *sane);
extern int ri_update(struct gfs2_sbd *sdp, int fd, int *rgcount, int *sane);
extern int write_sb(struct gfs2_sbd *sdp);

/* ondisk.c */
extern uint32_t gfs2_disk_hash(const char *data, int len);
extern void print_it(const char *label, const char *fmt, const char *fmt2, ...)
	__attribute__((format(printf,2,4)));

/* Translation functions */

extern void gfs2_inum_in(struct gfs2_inum *no, char *buf);
extern void gfs2_inum_out(const struct gfs2_inum *no, char *buf);
extern void gfs2_meta_header_in(struct gfs2_meta_header *mh, char *buf);
extern void gfs2_meta_header_out(const struct gfs2_meta_header *mh, char *buf);
extern void gfs2_sb_in(struct gfs2_sb *sb, char *buf);
extern void gfs2_sb_out(const struct gfs2_sb *sb, char *buf);
extern void gfs2_rindex_in(struct gfs2_rindex *ri, char *buf);
extern void gfs2_rindex_out(const struct gfs2_rindex *ri, char *buf);
extern void gfs2_rgrp_in(struct gfs2_rgrp *rg, char *buf);
extern void gfs2_rgrp_out(const struct gfs2_rgrp *rg, char *buf);
extern void gfs2_quota_in(struct gfs2_quota *qu, char *buf);
extern void gfs2_quota_out(struct gfs2_quota *qu, char *buf);
extern void gfs2_dinode_in(struct gfs2_dinode *di, char *buf);
extern void gfs2_dinode_out(struct gfs2_dinode *di, char *buf);
extern void gfs2_dirent_in(struct gfs2_dirent *de, char *buf);
extern void gfs2_dirent_out(struct gfs2_dirent *de, char *buf);
extern void gfs2_leaf_in(struct gfs2_leaf *lf, char *buf);
extern void gfs2_leaf_out(struct gfs2_leaf *lf, char *buf);
extern void gfs2_ea_header_in(struct gfs2_ea_header *ea, char *buf);
extern void gfs2_log_header_v1_in(struct gfs2_log_header *lh, char *buf);
extern void gfs2_log_header_in(struct gfs2_log_header *lh, char *buf);
extern void gfs2_log_header_v1_out(struct gfs2_log_header *lh, char *buf);
extern void gfs2_log_header_out(struct gfs2_log_header *lh, char *buf);
extern void gfs2_log_descriptor_in(struct gfs2_log_descriptor *ld, char *buf);
extern void gfs2_log_descriptor_out(struct gfs2_log_descriptor *ld, char *buf);
extern void gfs2_statfs_change_in(struct gfs2_statfs_change *sc, char *buf);
extern void gfs2_statfs_change_out(struct gfs2_statfs_change *sc, char *buf);
extern void gfs2_quota_change_in(struct gfs2_quota_change *qc, char *buf);
extern void gfs2_quota_change_out(struct gfs2_quota_change *qc, char *buf);

/* Printing functions */

extern void gfs2_inum_print(const struct gfs2_inum *no);
extern void gfs2_meta_header_print(const struct gfs2_meta_header *mh);
extern void gfs2_sb_print(const struct gfs2_sb *sb);
extern void gfs2_rindex_print(const struct gfs2_rindex *ri);
extern void gfs2_rgrp_print(const struct gfs2_rgrp *rg);
extern void gfs2_quota_print(const struct gfs2_quota *qu);
extern void gfs2_dinode_print(const struct gfs2_dinode *di);
extern void gfs2_leaf_print(const struct gfs2_leaf *lf);
extern void gfs2_ea_header_print(const struct gfs2_ea_header *ea, char *name);
extern void gfs2_log_header_v1_print(const struct gfs2_log_header *lh);
extern void gfs2_log_header_print(const struct gfs2_log_header *lh);
extern void gfs2_log_descriptor_print(const struct gfs2_log_descriptor *ld);
extern void gfs2_statfs_change_print(const struct gfs2_statfs_change *sc);
extern void gfs2_quota_change_print(const struct gfs2_quota_change *qc);

/* Language functions */

struct lgfs2_lang_state;

struct lgfs2_lang_result {
	uint64_t lr_blocknr;
	struct gfs2_buffer_head *lr_bh;
	const struct lgfs2_metadata *lr_mtype;
	int lr_state; // GFS2_BLKST_*
};

extern struct lgfs2_lang_state *lgfs2_lang_init(void);
extern int lgfs2_lang_parsef(struct lgfs2_lang_state *state, FILE *script);
extern int lgfs2_lang_parses(struct lgfs2_lang_state *state, const char *script);
extern struct lgfs2_lang_result *lgfs2_lang_result_next(struct lgfs2_lang_state *state, struct gfs2_sbd *sbd);
extern int lgfs2_lang_result_print(struct lgfs2_lang_result *result);
extern void lgfs2_lang_result_free(struct lgfs2_lang_result **result);
extern void lgfs2_lang_free(struct lgfs2_lang_state **state);

__END_DECLS

#endif /* __LIBGFS2_DOT_H__ */
