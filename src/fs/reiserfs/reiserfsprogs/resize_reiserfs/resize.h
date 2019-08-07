/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#ifndef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "io.h"
#include "misc.h"
#include "reiserfs_lib.h"

#include "../version.h"

#if __GLIBC__ >= 2
#include <sys/mount.h>
#else
#include <linux/fs.h>
#endif

#include <errno.h>
#include <string.h>

#include <mntent.h>

#define print_usage_and_exit() {\
 fprintf (stderr, "Usage: %s  [-s[+|-]#[G|M|K]] [-fqvV] device\n\n", argv[0]);\
 exit(16);\
}

#define DIE(form, args...) die("%s: " form "\n", g_progname , ## args)

/* reiserfs_resize.c */
extern struct buffer_head *g_sb_bh;
extern char *g_progname;

extern int opt_force;
extern int opt_verbose;
extern int opt_nowrite;
extern int opt_safe;

/* fe.c */
extern int resize_fs_online(char *devname, long long int blocks);

/* do_shrink.c */
extern int shrink_fs(reiserfs_filsys_t , long long int blocks);
