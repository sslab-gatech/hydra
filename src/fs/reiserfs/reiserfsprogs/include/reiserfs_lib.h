/*
 *  Copyright 2000-2004 by Hans Reiser, licensing governed by
 *  reiserfsprogs/README
 */

#ifndef REISERFSPROGS_LIB_H
#define REISERFSPROGS_LIB_H

#define BADBLOCK_DIRID	1
#define BADBLOCK_OBJID  (__u32)-1

typedef struct reiserfs_filsys * reiserfs_filsys_t;

#include <com_err.h>
#include "reiserfs_fs.h"

struct _bitmap {
	unsigned long bm_byte_size;
	unsigned long bm_bit_size;
	char *bm_map;
	unsigned long bm_set_bits;
	int bm_dirty;		/* used for fetched bitmap */
};

typedef struct _bitmap reiserfs_bitmap_t;

typedef __u32(*hashf_t) (const char *, int);

struct reiserfs_filsys {
	unsigned int fs_blocksize;
	int fs_format;		/* on-disk format version */
	hashf_t fs_hash_function;	/* pointer to function which is used to sort
					   names in directory. It is set by
					   reiserfs_open if it is set in the super
					   block, otherwise it is set by first
					   is_properly_hashed */

	char *fs_file_name;	/* file name of underlying device */
	int fs_dev;		/* descriptor of opened block device file */
	struct buffer_head *fs_super_bh;	/* buffer containing super block */
	struct reiserfs_super_block *fs_ondisk_sb;	/* pointer to its b_data */

	reiserfs_bitmap_t *fs_bitmap2;	/* ondisk bitmap after
					   reiserfs_open_ondisk_bitmap */

	/* opened journal fields */
	char *fs_j_file_name;	/* file name of relocated journal device */
	int fs_journal_dev;	/* descriptor of opened journal device */
	struct buffer_head *fs_jh_bh;	/* buffer containing journal header */

	/* badblocks */
	reiserfs_bitmap_t *fs_badblocks_bm;

	int fs_dirt;
	int fs_flags;
	void *fs_vp;
	int (*block_allocator) (reiserfs_filsys_t fs,
				unsigned long *free_blocknrs,
				unsigned long start, int amount_needed);
	int (*block_deallocator) (reiserfs_filsys_t fs, unsigned long block);
};

struct _transaction {
	unsigned long mount_id;
	unsigned long trans_id;
	unsigned long desc_blocknr;
	unsigned long trans_len;
	unsigned long commit_blocknr;
	unsigned long next_trans_offset;
};

typedef struct _transaction reiserfs_trans_t;

/* reiserfslib.c */

void init_tb_struct(struct tree_balance *tb, reiserfs_filsys_t ,
		    struct reiserfs_path *path, int size);

reiserfs_filsys_t reiserfs_open(const char *filename, int flags, long *error,
				 void *vp, int skip_check);
reiserfs_filsys_t reiserfs_create(const char *filename, int version,
				   unsigned long block_count, int block_size,
				   int default_journal, int new_format,
				   long *error);
void reiserfs_flush(reiserfs_filsys_t );
void reiserfs_free(reiserfs_filsys_t );
void reiserfs_close(reiserfs_filsys_t );
void reiserfs_reopen(reiserfs_filsys_t , int flags);
int is_opened_rw(reiserfs_filsys_t fs);

/*
void reiserfs_read_bitmap_blocks (reiserfs_filsys_t );
void reiserfs_free_bitmap_blocks (reiserfs_filsys_t );
*/
int no_reiserfs_found(reiserfs_filsys_t );
int is_block_count_correct(unsigned long block_of_super_block,
			   unsigned int block_size, unsigned long block_count,
			   unsigned long journal_size);
//unsigned long min_block_amount (int block_size, unsigned long journal_size);
unsigned long get_size_of_journal_or_reserved_area(struct reiserfs_super_block
						   *sb);

int reiserfs_new_blocknrs(reiserfs_filsys_t ,
			  unsigned long *free_blocknrs, unsigned long start,
			  int amount_needed);
int reiserfs_free_block(reiserfs_filsys_t , unsigned long block);
int spread_bitmaps(reiserfs_filsys_t );
int filesystem_dirty(reiserfs_filsys_t );
void mark_filesystem_dirty(reiserfs_filsys_t );

void reiserfs_paste_into_item(reiserfs_filsys_t , struct reiserfs_path *path,
			      const void *body, int size);
void reiserfs_insert_item(reiserfs_filsys_t , struct reiserfs_path *path,
			  struct item_head *ih, const void *body);

int reiserfs_locate_entry(reiserfs_filsys_t , struct reiserfs_key *dir,
			  const char *name, struct reiserfs_path *path);
int reiserfs_find_entry(reiserfs_filsys_t , const struct reiserfs_key *dir,
			const char *name, unsigned int *min_gen_counter,
			struct reiserfs_key *key);
int reiserfs_add_entry(reiserfs_filsys_t , const struct reiserfs_key *dir,
		       const char *name, int name_len,
		       const struct reiserfs_key *key, __u16 fsck_need);

struct reiserfs_key *uget_lkey(const struct reiserfs_path *path);
struct reiserfs_key *uget_rkey(const struct reiserfs_path *path);
int reiserfs_search_by_key_3(reiserfs_filsys_t , const struct reiserfs_key *key,
			     struct reiserfs_path *path);
int reiserfs_search_by_key_4(reiserfs_filsys_t , const struct reiserfs_key *key,
			     struct reiserfs_path *path);
int reiserfs_search_by_entry_key(reiserfs_filsys_t,
				 const struct reiserfs_key *key,
				 struct reiserfs_path *path);
int reiserfs_search_by_position(reiserfs_filsys_t , struct reiserfs_key *key,
				int version, struct reiserfs_path *path);
struct reiserfs_key *reiserfs_next_key(const struct reiserfs_path *path);
void copy_key(void *to, const void *from);
void copy_short_key(void *to, const void *from);
int comp_keys(const void *k1, const void *k2);
int comp_keys_3(const void *k1, const void *k2);
int comp_short_keys(const void *p_s_key1, const void *p_s_key2);
int comp_items(struct item_head *p_s_ih, struct reiserfs_path *p_s_path);

__u32 hash_value(hashf_t func, const char *name, int namelen);

int create_dir_sd(reiserfs_filsys_t fs,
		  struct reiserfs_path *path, const struct reiserfs_key *key,
		  void (*modify_item) (struct item_head *, void *));
void make_sure_root_dir_exists(reiserfs_filsys_t fs,
			       void (*modyfy_item) (struct item_head *, void *),
			       int ih_flags);

typedef void (*badblock_func_t) (reiserfs_filsys_t fs,
				 struct reiserfs_path *badblock_path,
				 void *data);

void mark_badblock(reiserfs_filsys_t fs, struct reiserfs_path *badblock_path,
		   void *data);
int create_badblock_bitmap(reiserfs_filsys_t fs, const char *badblocks_file);
void add_badblock_list(reiserfs_filsys_t fs, int no_badblock_in_tree_yet);
void badblock_list(reiserfs_filsys_t fs, badblock_func_t action, void *data);
#define reiserfs_fs_bmap_nr(fs) reiserfs_bmap_nr(get_sb_block_count(fs->fs_ondisk_sb), fs->fs_blocksize)
#define reiserfs_bmap_nr(count, blk_size) ((count - 1) / (blk_size * 8) + 1)
#define reiserfs_bmap_over(nr) (nr > ((1LL << 16) - 1))

typedef int (*reiserfs_file_iterate_indirect_fn)(reiserfs_filsys_t fs,
						 __u64 position, __u64 size,
						 int num_blocks, __u32 *blocks,
						 void *data);
typedef int (*reiserfs_file_iterate_direct_fn)(reiserfs_filsys_t fs,
					       __u64 position, __u64 size,
					       const char *body, size_t len,
					       void *data);
int reiserfs_iterate_file_data(reiserfs_filsys_t fs,
			       const struct reiserfs_key const *short_key,
			       reiserfs_file_iterate_indirect_fn indirect_fn,
			       reiserfs_file_iterate_direct_fn direct_fn,
			       void *data);

typedef int (*reiserfs_iterate_dir_fn)(reiserfs_filsys_t fs,
		       const struct reiserfs_key const *dir_short_key,
		       const char *name, size_t len,
		       __u32 deh_dirid, __u32 deh_objectid, void *data);
int reiserfs_iterate_dir(reiserfs_filsys_t fs,
			 const struct reiserfs_key const *dir_short_key,
			 const reiserfs_iterate_dir_fn callback, void *data);


extern struct reiserfs_key root_dir_key;
extern struct reiserfs_key parent_root_dir_key;
extern struct reiserfs_key lost_found_dir_key;
extern __u16 root_dir_format;
extern __u16 lost_found_dir_format;

/* bitmap.c */
int reiserfs_open_ondisk_bitmap(reiserfs_filsys_t );
int reiserfs_create_ondisk_bitmap(reiserfs_filsys_t );
void reiserfs_free_ondisk_bitmap(reiserfs_filsys_t );
void reiserfs_close_ondisk_bitmap(reiserfs_filsys_t );
int reiserfs_flush_to_ondisk_bitmap(reiserfs_bitmap_t *bm,
				    reiserfs_filsys_t fs);
unsigned int reiserfs_calc_bmap_nr(reiserfs_filsys_t fs, unsigned int blocks);

reiserfs_bitmap_t *reiserfs_create_bitmap(unsigned int bit_count);
int reiserfs_expand_bitmap(reiserfs_bitmap_t *bm, unsigned int bit_count);
void reiserfs_shrink_bitmap(reiserfs_bitmap_t *bm, unsigned int bit_count);
void reiserfs_delete_bitmap(reiserfs_bitmap_t *bm);
void reiserfs_bitmap_copy(reiserfs_bitmap_t *to, reiserfs_bitmap_t *from);
int reiserfs_bitmap_compare(reiserfs_bitmap_t *bm1, reiserfs_bitmap_t *bm2);
void reiserfs_bitmap_disjunction(reiserfs_bitmap_t *disk,
				 reiserfs_bitmap_t *cont);
void reiserfs_bitmap_delta(reiserfs_bitmap_t *base,
			   reiserfs_bitmap_t *exclude);
void reiserfs_bitmap_set_bit(reiserfs_bitmap_t *bm, unsigned int bit_number);
void reiserfs_bitmap_clear_bit(reiserfs_bitmap_t *bm, unsigned int bit_number);

int reiserfs_bitmap_test_bit(reiserfs_bitmap_t *bm, unsigned int bit_number);
int reiserfs_bitmap_find_zero_bit(reiserfs_bitmap_t *bm, unsigned long *start);
/*int reiserfs_fetch_ondisk_bitmap (reiserfs_bitmap_t *bm, reiserfs_filsys_t );*/
/*int reiserfs_flush_bitmap (reiserfs_bitmap_t *bm, reiserfs_filsys_t );*/
void reiserfs_bitmap_zero(reiserfs_bitmap_t *bm);
void reiserfs_bitmap_fill(reiserfs_bitmap_t *bm);
unsigned int reiserfs_bitmap_ones(reiserfs_bitmap_t *bm);
unsigned int reiserfs_bitmap_zeros(reiserfs_bitmap_t *bm);

FILE *open_file(const char *filename, char *option);
void close_file(FILE * fp);
void reiserfs_bitmap_save(FILE * fp, reiserfs_bitmap_t *bm);

/* this probably should be in fsck */
void reiserfs_begin_stage_info_save(FILE * file, unsigned long stage);
void reiserfs_end_stage_info_save(FILE * file);
int is_stage_magic_correct(FILE * fp);
//void reiserfs_stage_info_save(struct fsck_data *, FILE * file);

reiserfs_bitmap_t *reiserfs_bitmap_load(FILE * fp);
void reiserfs_bitmap_invert(reiserfs_bitmap_t *bm);

int reiserfs_remove_entry(reiserfs_filsys_t, const struct reiserfs_key *key);

/* node_formats.c */

#define THE_LEAF 1
#define THE_INTERNAL 2
#define THE_SUPER 3
#define THE_JDESC 4
#define HAS_IH_ARRAY 5
#define THE_UNKNOWN 6

int is_blocksize_correct(unsigned int blocksize);
int is_reiserfs_3_5_magic_string(struct reiserfs_super_block *rs);
int is_reiserfs_3_6_magic_string(struct reiserfs_super_block *rs);
int is_reiserfs_jr_magic_string(struct reiserfs_super_block *rs);
int does_look_like_super_block(struct reiserfs_super_block *rs);
int is_any_reiserfs_magic_string(struct reiserfs_super_block *rs);
int get_reiserfs_format(struct reiserfs_super_block *sb);
int reiserfs_super_block_size(struct reiserfs_super_block *rs);
/*int magic_2_version (struct reiserfs_super_block * rs);*/
int is_prejournaled_reiserfs(struct reiserfs_super_block *rs);
int who_is_this(const char *buf, int blocksize);

int leaf_count_ih(const char *buf, int blocksize);
int leaf_free_space_estimate(const char *buf, int blocksize);
int is_a_leaf(const char *buf, int blocksize);
int leaf_item_number_estimate(const struct buffer_head *bh);

char *which_block(int code);
int get_journal_size(reiserfs_filsys_t );
int not_data_block(reiserfs_filsys_t , unsigned long block);
int not_journalable(reiserfs_filsys_t , unsigned long block);
int block_of_bitmap(reiserfs_filsys_t , unsigned long block);
int block_of_journal(reiserfs_filsys_t , unsigned long block);
int is_tree_node(struct buffer_head *bh, int level);
int is_properly_hashed(reiserfs_filsys_t ,
		       const char *name, int namelen, __u32 offset);
int dir_entry_bad_location(struct reiserfs_de_head *deh,
			   struct item_head *ih, int first);
void make_dir_stat_data(int blocksize, int key_format,
			__u32 dirid, __u32 objectid,
			struct item_head *ih, void *sd);
void make_empty_dir_item_v1(char *body, __u32 dirid, __u32 objid,
			    __u32 par_dirid, __u32 par_objid);
void make_empty_dir_item(char *body, __u32 dirid, __u32 objid,
			 __u32 par_dirid, __u32 par_objid);
int reiserfs_is_fs_consistent(reiserfs_filsys_t fs);

typedef void (*item_action_t) (struct buffer_head * bh, struct item_head * ih);
typedef void (*item_head_action_t) (struct item_head * ih);

void for_every_item(struct buffer_head *bh, item_head_action_t action,
		    item_action_t *actions);
int key_format(const struct reiserfs_key *key);
unsigned long long get_offset(const struct reiserfs_key *key);
int uniqueness2type(__u32 uniqueness);
__u32 type2uniqueness(int type);
int get_type(const struct reiserfs_key *key);
char *key_of_what(const struct reiserfs_key *key);
int type_unknown(const struct reiserfs_key *key);
void set_type(int format, struct reiserfs_key *key, int type);
void set_offset(int format, struct reiserfs_key *key, loff_t offset);
void set_type_and_offset(int format, struct reiserfs_key *key, loff_t offset,
			 int type);

typedef int (*check_unfm_func_t) (reiserfs_filsys_t , __u32);
int is_it_bad_item(reiserfs_filsys_t , struct item_head *, const char *,
		   check_unfm_func_t, int bad_dir);

#define hash_func_is_unknown(fs) ((fs)->fs_hash_function == NULL)
#define reiserfs_hash(fs) ((fs)->fs_hash_function)

int known_hashes(void);
char *code2name(unsigned int code);
int func2code(hashf_t func);
hashf_t code2func(unsigned int code);
hashf_t name2func(const char *hash);
int find_hash_in_use(const char *name, int namelen, __u32 deh_offset,
		     unsigned int code_to_try_first);

int entry_length(const struct item_head *ih, const struct reiserfs_de_head *deh,
		 int pos_in_item);
char *name_in_entry(const struct reiserfs_de_head *deh, int pos_in_item);
int name_in_entry_length(const struct item_head *ih,
			 const struct reiserfs_de_head *deh, int pos_in_item);
int name_length(const char *name, int key_format);

/*  access to stat data fields */
void get_set_sd_field(int field, struct item_head *ih, void *sd,
		      void *value, int set);
#define GET_SD_MODE 0
#define GET_SD_SIZE 1
#define GET_SD_NLINK 2
#define GET_SD_BLOCKS 3
#define GET_SD_FIRST_DIRECT_BYTE 4

#define get_sd_mode(ih,sd,pmode) get_set_sd_field (GET_SD_MODE, ih, sd, pmode, 0/*get*/)
#define set_sd_mode(ih,sd,pmode) get_set_sd_field (GET_SD_MODE, ih, sd, pmode, 1/*set*/)

#define get_sd_size(ih,sd,psize) get_set_sd_field (GET_SD_SIZE, ih, sd, psize, 0/*get*/)
#define set_sd_size(ih,sd,psize) get_set_sd_field (GET_SD_SIZE, ih, sd, psize, 1/*set*/)

#define get_sd_blocks(ih,sd,pblocks) get_set_sd_field (GET_SD_BLOCKS, ih, sd, pblocks, 0/*get*/)
#define set_sd_blocks(ih,sd,pblocks) get_set_sd_field (GET_SD_BLOCKS, ih, sd, pblocks, 1/*set*/)

//#define get_sd_rdev(ih,sd,pblocks) get_set_sd_field (GET_SD_RDEV, ih, sd, pblocks, 0/*get*/)
//#define set_sd_rdev(ih,sd,pblocks) get_set_sd_field (GET_SD_RDEV, ih, sd, pblocks, 1/*set*/)

//#define get_sd_generation(ih,sd,pblocks) get_set_sd_field (GET_SD_GENER, ih, sd, pblocks, 0/*get*/)
//#define set_sd_generation(ih,sd,pblocks) get_set_sd_field (GET_SD_GENER, ih, sd, pblocks, 1/*set*/)

#define get_sd_nlink(ih,sd,pnlink) get_set_sd_field (GET_SD_NLINK, ih, sd, pnlink, 0/*get*/)
#define set_sd_nlink(ih,sd,pnlink) get_set_sd_field (GET_SD_NLINK, ih, sd, pnlink, 1/*set*/)

#define get_sd_first_direct_byte(ih,sd,pfdb) get_set_sd_field (GET_SD_FIRST_DIRECT_BYTE, ih, sd, pfdb, 0/*get*/)
#define set_sd_first_direct_byte(ih,sd,pfdb) get_set_sd_field (GET_SD_FIRST_DIRECT_BYTE, ih, sd, pfdb, 1/*set*/)

int is_objectid_used(reiserfs_filsys_t fs, __u32 objectid);
void mark_objectid_used(reiserfs_filsys_t fs, __u32 objectid);

/* journal.c */
int get_boundary_transactions(reiserfs_filsys_t , reiserfs_trans_t *,
			      reiserfs_trans_t *);
int next_transaction(reiserfs_filsys_t , reiserfs_trans_t *, reiserfs_trans_t);

int replay_one_transaction(reiserfs_filsys_t , reiserfs_trans_t *);

typedef void (*action_on_trans_t) (reiserfs_filsys_t , reiserfs_trans_t *);
void for_each_transaction(reiserfs_filsys_t , action_on_trans_t);

typedef void (*action_on_block_t) (reiserfs_filsys_t , reiserfs_trans_t *,
				   unsigned int index,
				   unsigned long in_journal,
				   unsigned long in_place);
void for_each_block(reiserfs_filsys_t fs, reiserfs_trans_t *trans,
		    action_on_block_t action);

int reiserfs_open_journal(reiserfs_filsys_t , const char *, int flags);
int reiserfs_journal_params_check(reiserfs_filsys_t fs);
int reiserfs_create_journal(reiserfs_filsys_t fs, const char *j_filename,
			    unsigned long offset, unsigned long len,
			    int transaction_max_size, int force);
int reiserfs_journal_opened(reiserfs_filsys_t );
void reiserfs_flush_journal(reiserfs_filsys_t fs);
void reiserfs_free_journal(reiserfs_filsys_t fs);
void reiserfs_close_journal(reiserfs_filsys_t );
void reiserfs_reopen_journal(reiserfs_filsys_t fs, int flag);
__u32 advise_journal_max_trans_age(void);
__u32 advise_journal_max_commit_age(void);
__u32 advise_journal_max_batch(unsigned long journal_trans_max);
__u32 advise_journal_max_trans_len(__u32 desired, __u32 journal_size,
				   int blocksize, int verbose);

/* prints.c */
void print_indirect_item(FILE * fp, struct buffer_head *bh, int item_num);
void print_block(FILE * fp, reiserfs_filsys_t , struct buffer_head *bh, ...);	//int print_mode, int first, int last);
int print_super_block(FILE * fp, reiserfs_filsys_t , const char *file_name,
		      struct buffer_head *bh, int short_print);
void print_journal(reiserfs_filsys_t );
void print_journal_header(reiserfs_filsys_t fs);
void reiserfs_warning(FILE * fp, const char *fmt, ...);
char ftypelet(mode_t mode);
void reiserfs_print_item(FILE * fp, struct buffer_head *bh,
			 struct item_head *ih);
void print_filesystem_state(FILE * fp, reiserfs_filsys_t fs);
void print_one_transaction(reiserfs_filsys_t fs, reiserfs_trans_t *trans);
void print_journal_params(FILE * fp, struct journal_params *jp);
char *get_reiserfs_version(__u16 version);
int can_we_format_it(const char *device_name, int force);

#define reiserfs_panic(fmt, list...) \
{\
	fflush (stdout);\
	fprintf (stderr, "%s %d %s\n", __FILE__, __LINE__, __FUNCTION__);\
	reiserfs_warning (stderr, (const char *)fmt, ## list);\
        reiserfs_warning (stderr, "\n" );\
        abort ();\
}
#define reiserfs_exit(val, fmt, list...) \
{\
	fflush (stdout);\
	reiserfs_warning (stderr, (const char *)fmt, ## list);\
        reiserfs_warning (stderr, "\n" );\
        exit (val);\
}

#define check_forcing_ask_confirmation(force) \
	if (force < 1) {\
	    /* avoid formatting it without being forced */\
	    reiserfs_warning (stderr, "Use -f to force over\n");\
	    return 0;\
	}\
	if (force < 2) {\
	    if (!user_confirmed (stderr, "Continue (y/n):", "y\n"))\
		return 0;\
	}\

/* xattr.c */
__u32 reiserfs_xattr_hash(const char *msg, int len);
int reiserfs_check_xattr(const void *body, int len);
int reiserfs_acl_count(size_t size);
#endif /* REISERFSPROGS_LIB_H */
