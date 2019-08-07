/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#define _GNU_SOURCE

#ifndef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "io.h"
#include "misc.h"
#include "reiserfs_lib.h"

#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

#include "../version.h"

/* main.c */
extern reiserfs_filsys_t fs;
int main(int argc, char *argv[]);

/* Exit codes. */
#define EXIT_OK		0
#define EXIT_FIXED	1
#define EXIT_REBOOT	2
#define EXIT_FATAL	4
#define EXIT_FIXABLE	6
#define EXIT_OPER	8	/* Some operation returns error. */
#define EXIT_USER	16

/*
 * modes
 */
#define DO_NOTHING              0
#define FSCK_CHECK              1
#define FSCK_FIX_FIXABLE        2
#define FSCK_SB                 3
#define FSCK_REBUILD            4
#define FSCK_ROLLBACK_CHANGES   5
#define FSCK_CLEAN_ATTRIBUTES	7
#define FSCK_AUTO               8	/* -a || -p specified */

/* temporary */
#define DO_TEST                 9

/*
 * options
 */

#define OPT_INTERACTIVE                 1 << 0
#define OPT_ADJUST_FILE_SIZE            1 << 1	/* not default yet */
#define OPT_QUIET                       1 << 2	/* no "speed" info */
#define OPT_SILENT                      1 << 3	/* no complains about found corruptions */
#define OPT_BACKGROUND                  1 << 4
#define OPT_SKIP_JOURNAL                1 << 5
#define OPT_HASH_DEFINED                1 << 6
#define OPT_SAVE_PASSES_DUMP            1 << 7
#define OPT_SAVE_ROLLBACK               1 << 8
#define OPT_YES				1 << 9
#define BADBLOCKS_FILE			1 << 10
#define OPT_FORCE			1 << 11

/* pass0.c */
extern reiserfs_bitmap_t *leaves_bitmap;
void pass_0(reiserfs_filsys_t );
void load_pass_0_result(FILE *, reiserfs_filsys_t );

int leaf_structure_check(reiserfs_filsys_t fs, struct buffer_head *bh);

int is_used_leaf(unsigned long block);
int is_good_unformatted(unsigned long block);
void mark_good_unformatted(unsigned long block);
int is_bad_unformatted(unsigned long block);

int are_there_allocable_blocks(unsigned int amout_needed);
unsigned long alloc_block(void);
void make_allocable(unsigned long block);
void register_uninsertable(unsigned long block);
void register_saved_item(void);
int still_bad_unfm_ptr_1(unsigned long block);
int still_bad_unfm_ptr_2(unsigned long block);
void make_alloc_bitmap(reiserfs_filsys_t );

void delete_aux_bitmaps();
void set_aux_bitmaps(reiserfs_bitmap_t *leaves, reiserfs_bitmap_t *good,
		     reiserfs_bitmap_t *bad);

#define __is_marked(name,block) reiserfs_bitmap_test_bit (name##_bitmap, block)
#define __mark(name,block) reiserfs_bitmap_set_bit (name##_bitmap, block)
#define __unmark(name,block) reiserfs_bitmap_clear_bit (name##_bitmap, block)

/* unformatted in tree */
extern reiserfs_bitmap_t *bad_unfm_in_tree_once_bitmap;
#define is_bad_unfm_in_tree_once(block) __is_marked (bad_unfm_in_tree_once, block)
#define mark_bad_unfm_in_tree_once(block) __mark (bad_unfm_in_tree_once, block)

/* pass1.c */
void pass_1(reiserfs_filsys_t );
void load_pass_1_result(FILE *, reiserfs_filsys_t );
struct buffer_head *make_buffer(int dev, unsigned long blocknr, int size,
				char *data);
void build_the_tree(void);
extern int g_unaccessed_items;
int is_item_reachable(struct item_head *ih);
void mark_item_reachable(struct item_head *ih, struct buffer_head *bh);
void mark_item_unreachable(struct item_head *ih);

struct si *remove_saved_item(struct si *si);
int tree_is_empty(void);
void make_single_leaf_tree(struct buffer_head *bh);

/* pass2.c */
void pass_2(reiserfs_filsys_t );
void load_pass_2_result(reiserfs_filsys_t );
void insert_item_separately(struct item_head *ih, char *item, int was_in_tree);
void save_item(struct si **head, struct item_head *ih, char *item,
	       __u32 blocknr);
struct si *save_and_delete_file_item(struct si *si, struct reiserfs_path *path);
void take_bad_blocks_put_into_tree(void);
void rewrite_object(struct item_head *ih, int do_remap);
void pass_2_take_bad_blocks_put_into_tree(void);
/*int is_remapped (struct item_head * ih);*/
void link_relocated_files(void);
int should_relocate(struct item_head *ih);
void relocate_dir(struct item_head *ih, int change_ih);

extern __u32 objectid_for_relocation(const struct reiserfs_key *key);
extern void linked_already(const struct reiserfs_key *new_key);

/* file.c */
struct si {
	struct item_head si_ih;
	char *si_dnm_data;
	struct si *si_next;
	__u32 si_blocknr;

	// changed by XB;
	struct si *last_known;
};
void put_saved_items_into_tree(struct si *);
int reiserfsck_file_write(struct item_head *ih, char *item, int was_in_tree);
int are_file_items_correct(struct item_head *sd_ih, void *sd, __u64 * size,
			   __u32 * blocks, int mark_passed_items, int *symlink);
int delete_N_items_after_key(struct reiserfs_key *start_key,
			     struct si **save_here, int skip_dir_items,
			     int n_to_delete);
void rewrite_file(struct item_head *ih, int should_relocate,
		  int should_change_ih);

/* semantic.c */

/* return values for check_regular_file and check_semantic_tree */
#define OK 0
#define STAT_DATA_NOT_FOUND -1
#define DIRECTORY_HAS_NO_ITEMS -2
#define RELOCATED -3
#define LOOP_FOUND -4

void load_semantic_result(FILE *, reiserfs_filsys_t );

void pass_3_semantic(reiserfs_filsys_t );
void semantic_check(void);
int check_semantic_tree(struct reiserfs_key *key, struct reiserfs_key *parent,
			int is_dot_dot, int lost_found,
			struct item_head *new_ih);
void zero_nlink(struct item_head *ih, void *sd);
int not_a_directory(void *sd);
int not_a_regfile(void *sd);
int fix_obviously_wrong_sd_mode(struct reiserfs_path *path);
int is_dot_dot(char *name, int namelen);
int is_dot(char *name, int namelen);
/*void create_dir_sd (reiserfs_filsys_t fs,
  struct reiserfs_path *path, struct reiserfs_key *key);*/
int rebuild_check_regular_file(struct reiserfs_path *path, void *sd,
			       struct item_head *new_ih);
int rebuild_semantic_pass(struct reiserfs_key *key,
			  const struct reiserfs_key *parent,
			  int is_dot_dot, struct item_head *new_ih);

/* lost+found.c */
void pass_3a_look_for_lost(reiserfs_filsys_t );

void load_lost_found_result(reiserfs_filsys_t );

/* pass4.c */
void pass_4_check_unaccessed_items(void);

/* check.c */
int is_leaf_bad(struct buffer_head *bh);
int is_bad_item(struct buffer_head *bh, struct item_head *, char *);
/*int check_file_system (void);*/
void reiserfsck_check_pass1(void);
/*char * bad_name (char * name, int namelen);*/
/* to test result of direcotry item recovering on pass 0 */
int is_bad_directory(struct item_head *ih, char *item, int dev, int blocksize);

//extern int bad_block_number (struct super_block * s, blocknr_t block);

/* check_tree.c */
void check_fs_tree(reiserfs_filsys_t );
void do_clean_attributes(reiserfs_filsys_t fs);
int bad_pair(reiserfs_filsys_t , struct buffer_head *bh, int i);
int bad_leaf_2(reiserfs_filsys_t , struct buffer_head *bh);

#if 0
extern int should_be_relocated(struct reiserfs_key *key);
extern void to_be_relocated(struct reiserfs_key *key);
extern void clear_relocated_list(void);
#endif

/* ustree.c */
void reiserfsck_paste_into_item(struct reiserfs_path *path, const char *body,
				int size);
void reiserfsck_insert_item(struct reiserfs_path *path, struct item_head *ih,
			    const char *body);
void reiserfsck_delete_item(struct reiserfs_path *path, int temporary);
void reiserfsck_cut_from_item(struct reiserfs_path *path, int cut_size);
/*typedef	int (comp3_function_t)(void * key1, void * key2, int version);*/
/*typedef int (comp_function_t)(struct reiserfs_key *key1, struct reiserfs_key *key2);*/
/*int ubin_search_id(__u32 * id, __u32 * base, __u32 number, __u32 * pos);*/
/*int usearch_by_key (struct super_block * s, struct reiserfs_key *key, struct reiserfs_path *path);*/
/*int usearch_by_key_3 (struct super_block * s, struct reiserfs_key *key, struct reiserfs_path *path, int * repeat, int stop_level,
		      comp3_function_t comp_func, int version);		
		      int usearch_by_entry_key (struct super_block * s, struct reiserfs_key *key, struct reiserfs_path *path);*/

typedef int do_after_read_t(reiserfs_filsys_t , struct buffer_head **, int h);
typedef int do_on_full_path_t(reiserfs_filsys_t , struct buffer_head **, int);
void pass_through_tree(reiserfs_filsys_t , do_after_read_t, do_on_full_path_t,
		       int depth);

/* bitmap.c */
int reiserfsck_reiserfs_new_blocknrs(reiserfs_filsys_t fs,
				     unsigned long *pblocknrs,
				     unsigned long start_from,
				     int amount_needed);
int reiserfsck_reiserfs_free_block(reiserfs_filsys_t fs, unsigned long block);
struct buffer_head *reiserfsck_get_new_buffer(unsigned long start);
int is_block_used(unsigned long block);
void mark_block_used(unsigned long block, int check_hardware);
void mark_block_uninsertable(unsigned long block);
int is_block_uninsertable(unsigned long block);

typedef struct id_map {
	void **index;
	__u32 count, last_used;
} id_map_t;

id_map_t *id_map_init();
void id_map_free(id_map_t *);
int id_map_test(id_map_t *map, __u32 id);
int id_map_mark(id_map_t *map, __u32 id);
__u32 id_map_alloc(id_map_t *map);
void id_map_flush(struct id_map *map, reiserfs_filsys_t fs);

/* FIXME: Needs to be implemented
void fetch_objectid_map (struct id_map * map, reiserfs_filsys_t fs);

void reiserfs_objectid_map_save (FILE * fp, struct id_map * id_map);
struct id_map * reiserfs_objectid_map_load (FILE * fp);
*/

void mark_formatted_pointed_by_indirect(__u32);
int is_formatted_pointed_by_indirect(__u32);

#define MAP_NOT_PACKED	0
#define MAP_PACKED	1

struct pass0_stat {
	unsigned long dealt_with;	/* number of blocks read during pass 0 */
	unsigned long leaves;	/* blocks looking like reiserfs leaves found */
	unsigned long leaves_corrected;
	unsigned long all_contents_removed;
	unsigned long too_old_leaves;	/* these are leaves which contains
					   direntries with different hash from the
					   one specified with -h */
	unsigned long wrong_pointers;	/* pointers in indirect items pointing to
					   wrong area */
	unsigned long pointed;	/* pointers blocks of device pointed by all
				   indirect items */
};

struct pass1_stat {
	unsigned long leaves;	/* leaves found in pass0 to build tree off */
	unsigned long inserted_leaves;	/* number of leaves inserted by pointers */
	unsigned long pointed_leaves;	/* pointers in indirect items which pointed
					   to leaves (zeroed) */
	unsigned long uninsertable_leaves;
	unsigned long non_unique_pointers;	/* pointers to already pointed unformatted nodes */
	unsigned long correct_pointers;
	unsigned long saved_items;	/* items saved on pass1: should be 0 */
	unsigned long allocable_blocks;	/* allocable blocks before pass 1 starts */
};

struct pass2_stat {
	unsigned long leaves;	/* leaves inserted item by item */
	unsigned long safe_non_unique_pointers;	/* these are just the same pointers */
	unsigned long relocated;
	unsigned long shared_objectids;
	unsigned long rewritten;
};

struct semantic_stat {
	unsigned long oid_sharing_files_relocated;
	unsigned long regular_files;
	unsigned long directories;
	unsigned long symlinks;
	unsigned long broken_files;
	unsigned long others;
	unsigned long fixed_sizes;
	unsigned long oid_sharing;
	unsigned long oid_sharing_dirs_relocated;
	unsigned long deleted_entries;
};

struct lost_found_stat {
	unsigned long dir_recovered;
	unsigned long empty_lost_dirs;
	unsigned long lost_found;
	unsigned long lost_found_files;
	unsigned long lost_found_dirs;
	unsigned long oid_sharing;
	unsigned long oid_sharing_dirs_relocated;
	unsigned long oid_sharing_files_relocated;
};

struct pass_4_stat {
	unsigned long deleted_items;
};

struct rebuild_info {
	union {
		struct pass0_stat pass0;
		struct pass1_stat pass1;
		struct pass2_stat pass2;
		struct {
			struct semantic_stat semantic;
			struct lost_found_stat lost_found;
		} tree;
		struct pass_4_stat pass4;
	} pass_u;

	/* bitmaps */
	reiserfs_bitmap_t *source_bitmap;
	reiserfs_bitmap_t *new_bitmap;
	reiserfs_bitmap_t *allocable_bitmap;
	reiserfs_bitmap_t *uninsertables;

	char *bitmap_file_name;
	/*char * new_bitmap_file_name; */
	char *passes_dump_file_name;	/* after pass 0, 1 or 2 reiserfsck can store
					   data with which it will be able to start
					   from the point it stopped last time at */

	unsigned short mode;
	unsigned long options;

	/* rollback file */
	char *rollback_file;

	/* hash hits stat */
	int hash_amount;
	unsigned long *hash_hits;
	char *defined_hash;

#define USED_BLOCKS 1
#define EXTERN_BITMAP 2
#define ALL_BLOCKS 3
	int scan_area;
	int use_journal_area;
	int test;
};

struct check_info {
	unsigned long bad_nodes;
	unsigned long fatal_corruptions;
	unsigned long fixable_corruptions;
//    unsigned long badblocks_corruptions;
	unsigned long leaves;
	unsigned long internals;
	unsigned long dirs;
	unsigned long files;
	unsigned long safe;
	unsigned long unfm_pointers;
	unsigned long zero_unfm_pointers;
	reiserfs_bitmap_t *deallocate_bitmap;
};

struct fsck_data {
	unsigned short mode;	/* check, rebuild, etc */
	unsigned long options;
	unsigned long mounted;

	struct rebuild_info rebuild;
	struct check_info check;

	char *journal_dev_name;
	/* log file name and handle */
	char *log_file_name;
	FILE *log;

	/* this is a file where reiserfsck will explain what it is doing. This is
	   usually stderr. But when -g is specified - reiserfsck runs in the
	   background and append progress information into 'fsck.run' */
	FILE *progress;

	/* objectid maps */
	id_map_t *proper_id_map;
	id_map_t *semantic_id_map;	/* this objectid map is used to
					   cure objectid sharing problem */
};

#define fsck_data(fs) ((struct fsck_data *)((fs)->fs_vp))
#define pass_0_stat(fs) (&(fsck_data(fs)->rebuild.pass_u.pass0))
#define pass_1_stat(fs) (&(fsck_data(fs)->rebuild.pass_u.pass1))
#define pass_2_stat(fs) (&(fsck_data(fs)->rebuild.pass_u.pass2))
#define sem_pass_stat(fs) (&(fsck_data(fs)->rebuild.pass_u.tree.semantic))
#define lost_found_pass_stat(fs) (&(fsck_data(fs)->rebuild.pass_u.tree.lost_found))
#define pass_4_stat(fs) (&(fsck_data(fs)->rebuild.pass_u.pass4))

#define fsck_check_stat(fs) (&(fsck_data(fs)->check))

#define proper_id_map(s) fsck_data(s)->proper_id_map
#define semantic_id_map(s) fsck_data(s)->semantic_id_map

#define fsck_source_bitmap(fs) fsck_data(fs)->rebuild.source_bitmap
#define fsck_new_bitmap(fs) fsck_data(fs)->rebuild.new_bitmap
#define fsck_allocable_bitmap(fs) fsck_data(fs)->rebuild.allocable_bitmap
#define fsck_uninsertables(fs) fsck_data(fs)->rebuild.uninsertables

#define fsck_deallocate_bitmap(fs) fsck_data(fs)->check.deallocate_bitmap

#define fsck_interactive(fs) (fsck_data(fs)->options & OPT_INTERACTIVE)
//#define fsck_fix_fixable(fs) (fsck_data(fs)->options & OPT_FIX_FIXABLE)

#define fsck_run_one_step(fs) (fsck_data(fs)->options & OPT_SAVE_PASSES_DUMP)

#define fsck_save_rollback(fs) (fsck_data(fs)->options & OPT_SAVE_ROLLBACK)

/* change unknown modes (corrupted) to mode of regular files, fix file
   sizes which are bigger than a real file size, relocate files with
   shared objectids (this slows fsck down (when there are too many
   files sharing the same objectid), it will also remove other names
   pointing to this file */
#define fsck_adjust_file_size(fs) (fsck_data(fs)->options & OPT_ADJUST_FILE_SIZE)
#define fsck_quiet(fs)	(fsck_data(fs)->options & OPT_QUIET)
#define fsck_silent(fs)	(fsck_data(fs)->options & OPT_SILENT)
#define fsck_in_background(fs) (fsck_data(fs)->options & OPT_BACKGROUND)
#define fsck_hash_defined(fs) (fsck_data(fs)->options & OPT_HASH_DEFINED)
#define fsck_skip_journal(fs) (fsck_data(fs)->options & OPT_SKIP_JOURNAL)
#define fsck_yes_all(fs) (fsck_data(fs)->options & OPT_YES)

#define fsck_mode(fs) (fsck_data(fs)->mode)
#define fsck_log_file(fs) (fsck_data(fs)->log)
#define fsck_progress_file(fs) ((fs && fsck_data(fs)->progress) ? fsck_data(fs)->progress : stderr)

/* name of file where we store information for continuing */
#define state_dump_file(fs) fsck_data(fs)->rebuild.passes_dump_file_name

/* name of file where we store rollback data */
#define state_rollback_file(fs) fsck_data(fs)->rebuild.rollback_file

int fsck_user_confirmed(reiserfs_filsys_t fs, char *q, char *a,
			int default_answer);
void stage_report(int, reiserfs_filsys_t );

/*pass1: rebuild super block*/
void rebuild_sb(reiserfs_filsys_t fs, char *filename, struct fsck_data *data);

#define fsck_log(fmt, list...) \
{\
if (!fsck_silent (fs))\
    reiserfs_warning (fsck_log_file (fs), fmt, ## list);\
}

#define fsck_progress(fmt, list...) \
{\
reiserfs_warning (fsck_progress_file(fs), fmt, ## list);\
fflush (fsck_progress_file(fs));\
}

#define FATAL	1
#define FIXABLE 2

void one_more_corruption(reiserfs_filsys_t fs, int kind);
void one_less_corruption(reiserfs_filsys_t fs, int kind);

/*
#define one_more_corruption(fs,kind) fsck_check_stat (fs)->kind##_corruptions++
#define one_less_corruption(fs,kind) fsck_check_stat (fs)->kind##_corruptions--
*/

#define fsck_exit(fmt, list...) \
{\
reiserfs_warning (fsck_progress_file(fs), fmt, ## list);\
exit (EXIT_USER);\
}
