#ifndef __HEXVIEW_DOT_H__
#define __HEXVIEW_DOT_H__

#include <sys/types.h>
#include <inttypes.h>
#include <limits.h>
#include <linux/gfs2_ondisk.h>
#include <string.h>

#include "libgfs2.h"
#include "copyright.cf"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DMODES 3
enum dsp_mode { HEX_MODE = 0, GFS2_MODE = 1, EXTENDED_MODE = 2, INIT_MODE = 3 };
#define BLOCK_STACK_SIZE 256

#define pv(struct, member, fmt, fmt2) do {				\
		print_it("  "#member, fmt, fmt2, struct->member);	\
	} while (FALSE);
#define RGLIST_DUMMY_BLOCK -2
#define JOURNALS_DUMMY_BLOCK -3

extern const char *mtypes[];
extern struct gfs2_sb sb;
extern int blockhist;
extern int edit_mode;
extern int line;
extern char edit_fmt[80];
extern char estring[1024]; /* edit string */
extern char efield[64];
extern uint64_t dev_offset;
extern uint64_t max_block;
extern int termlines;
extern int insert;
extern const char *termtype;
extern int line;
extern int struct_len;
extern unsigned int offset;
extern int edit_row[DMODES], edit_col[DMODES], print_entry_ndx;
extern int start_row[DMODES], end_row[DMODES], lines_per_row[DMODES];
extern int edit_size[DMODES], last_entry_onscreen[DMODES];
extern char edit_fmt[80];
extern struct gfs2_sbd sbd;
extern struct gfs_sb *sbd1;
extern struct gfs2_inum gfs1_quota_di;   /* kludge because gfs2 sb too small */
extern struct gfs2_inum gfs1_license_di; /* kludge because gfs2 sb too small */
extern struct gfs2_dinode di;
extern int screen_chunk_size; /* how much of the 4K can fit on screen */
extern int gfs2_struct_type;
extern int identify;
extern int color_scheme;
extern WINDOW *wind;
extern int editing;
extern uint64_t temp_blk;
extern uint64_t starting_blk;
extern const char *block_type_str[15];
extern int dsplines;
extern int dsp_lines[DMODES];
extern int combined_display;
extern int details;
extern const char *allocdesc[2][5];

struct gfs2_dirents {
	uint64_t block;
	struct gfs2_dirent dirent;
	char filename[NAME_MAX];
};

struct indirect_info {
	int is_dir;
	int height;
	uint64_t block;
	uint32_t dirents;
	struct gfs2_leaf lf;
	struct metapath mp;
	struct gfs2_dirents dirent[64];
	uint64_t ptroff;
};

struct iinfo {
	struct indirect_info ii[512];
};

struct blkstack_info {
	uint64_t block;
	int start_row[DMODES];
	int end_row[DMODES];
	int lines_per_row[DMODES];
	int edit_row[DMODES];
	int edit_col[DMODES];
	enum dsp_mode dmode;
	int gfs2_struct_type;
	struct metapath mp;
};

extern struct blkstack_info blockstack[BLOCK_STACK_SIZE];
extern struct iinfo *indirect; /* more than the most indirect
			       pointers possible for any given 4K block */
extern struct indirect_info masterdir; /* Master directory info */
extern int indirect_blocks;  /* count of indirect blocks */
extern enum dsp_mode dmode;

/* ------------------------------------------------------------------------ */
/* block_is_rgtree - there's no such block as the rglist.  This is a        */
/*                   special case meant to parse the rindex and follow the  */
/*                   blocks to the real rgs.                                */
/* ------------------------------------------------------------------------ */
static inline int block_is_rgtree(uint64_t blk)
{
	if (blk == RGLIST_DUMMY_BLOCK)
		return TRUE;
	return FALSE;
}

static inline int block_is_journals(uint64_t blk)
{
	if (blk == JOURNALS_DUMMY_BLOCK)
		return TRUE;
	return FALSE;
}

#define SCREEN_HEIGHT   (16)
#define SCREEN_WIDTH    (16)

/* die() used to be in libgfs2.h */
static __inline__ __attribute__((noreturn, format (printf, 1, 2)))
void die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(-1);
}

/*  Memory macros  */

#define type_alloc(ptr, type, count) \
{ \
  (ptr) = (type *)malloc(sizeof(type) * (count)); \
  if (!(ptr)) \
    die("unable to allocate memory on line %d of file %s\n", \
	__LINE__, __FILE__); \
}

#define printk printw

/*  Divide x by y.  Round up if there is a remainder.  */
#define DIV_RU(x, y) (((x) + (y) - 1) / (y))

#define TITLE1 "gfs2_edit - Global File System Editor (use with extreme caution)"
#define TITLE2 REDHAT_COPYRIGHT " - Press H for help"

#define COLOR_TITLE     1
#define COLOR_NORMAL    2
#define COLOR_INVERSE   3
#define COLOR_SPECIAL   4
#define COLOR_HIGHLIGHT 5
#define COLOR_OFFSETS   6
#define COLOR_CONTENTS  7

#define COLORS_TITLE     \
	do { \
		if (termlines) { \
			attrset(COLOR_PAIR(COLOR_TITLE)); \
			attron(A_BOLD); \
		} \
	} while (0)
#define COLORS_NORMAL    \
	do { \
		if (termlines) { \
			attrset(COLOR_PAIR(COLOR_NORMAL)); \
			attron(A_BOLD); \
		} \
	} while (0)
#define COLORS_INVERSE   \
	do { \
		if (termlines) { \
			attrset(COLOR_PAIR(COLOR_INVERSE)); \
			attron(A_BOLD); \
		} \
	} while (0)
#define COLORS_SPECIAL   \
	do { \
		if (termlines) { \
			attrset(COLOR_PAIR(COLOR_SPECIAL)); \
			attron(A_BOLD); \
		} \
	} while (0)
#define COLORS_HIGHLIGHT \
	do { \
		if (termlines) { \
			attrset(COLOR_PAIR(COLOR_HIGHLIGHT)); \
			attron(A_BOLD); \
		} \
	} while (0)
#define COLORS_OFFSETS   \
	do { \
		if (termlines) { \
			attrset(COLOR_PAIR(COLOR_OFFSETS)); \
			attron(A_BOLD); \
		} \
	} while (0)
#define COLORS_CONTENTS  \
	do { \
		if (termlines) { \
			attrset(COLOR_PAIR(COLOR_CONTENTS)); \
			attron(A_BOLD); \
		} \
	} while (0)

extern int block_is_jindex(uint64_t blk);
extern int block_is_rindex(uint64_t blk);
extern int block_is_inum_file(uint64_t blk);
extern int block_is_statfs_file(uint64_t blk);
extern int block_is_quota_file(uint64_t blk);
extern int block_is_per_node(uint64_t blk);
extern int display_block_type(struct gfs2_buffer_head *bh, int from_restore);
extern void gfs_jindex_in(struct gfs_jindex *jindex, char *buf);
extern void gfs_log_header_in(struct gfs_log_header *head,
			      struct gfs2_buffer_head *bh);
extern void gfs_log_header_print(struct gfs_log_header *lh);
extern void gfs_dinode_in(struct gfs_dinode *di, struct gfs2_buffer_head *bh);
extern void savemeta(char *out_fn, int saveoption, int gziplevel);
extern void restoremeta(const char *in_fn, const char *out_device,
			uint64_t printblocksonly);
extern int display(int identify_only, int trunc_zeros, uint64_t flagref,
		   uint64_t ref_blk);
extern uint64_t check_keywords(const char *kword);
extern uint64_t masterblock(const char *fn);
extern void gfs_rgrp_print(struct gfs_rgrp *rg);
extern int has_indirect_blocks(void);
extern uint32_t get_block_type(const struct gfs2_buffer_head *lbh,
			       int *structlen);

#endif /* __HEXVIEW_DOT_H__ */
