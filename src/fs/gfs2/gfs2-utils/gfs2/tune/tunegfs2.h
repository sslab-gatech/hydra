#ifndef __GFS2_TUNE_DOT_H__
#define __GFS2_TUNE_DOT_H__

#define GFS2_DEFAULT_BSIZE	4096

struct tunegfs2 {
	char *devicename;
	int fd;
	unsigned long sb_start;
	struct gfs2_sb *sb;
	char *uuid;
	char *label;
	char *table;
	char *proto;
	char *mount_options;
	int opt_list;
	int opt_label;
	int opt_uuid;
	int opt_proto;
	int opt_table;
};

extern int print_super(const struct tunegfs2 *);
extern int read_super(struct tunegfs2 *);
extern int write_super(const struct tunegfs2 *);
extern int change_uuid(struct tunegfs2 *, const char *uuid);
extern int change_lockproto(struct tunegfs2 *, const char *label);
extern int change_locktable(struct tunegfs2 *, const char *label);

#endif

