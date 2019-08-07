#include "clusterautoconfig.h"

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <signal.h>
#include <libintl.h>
#include <locale.h>
#include <sys/time.h>
#include <time.h>
#define _(String) gettext(String)
#include <syslog.h>

#include <logging.h>
#include "copyright.cf"
#include "libgfs2.h"
#include "fsck.h"
#include "link.h"
#include "osi_list.h"
#include "metawalk.h"
#include "util.h"

struct gfs2_options opts = {0};
struct gfs2_inode *lf_dip = NULL; /* Lost and found directory inode */
int lf_was_created = 0;
uint64_t last_fs_block, last_reported_block = -1;
int64_t last_reported_fblock = -1000000;
int skip_this_pass = FALSE, fsck_abort = FALSE;
int errors_found = 0, errors_corrected = 0;
const char *pass = "";
uint64_t last_data_block;
uint64_t first_data_block;
int preen = 0, force_check = 0;
struct osi_root dup_blocks;
struct osi_root dirtree;
struct osi_root inodetree;
int dups_found = 0, dups_found_first = 0;
struct gfs_sb *sbd1 = NULL;
int sb_fixed = 0;
int print_level = MSG_NOTICE;

/* This function is for libgfs2's sake.                                      */
void print_it(const char *label, const char *fmt, const char *fmt2, ...)
{
	va_list args;

	va_start(args, fmt2);
	printf("%s: ", label);
	vprintf(fmt, args);
	va_end(args);
}

static void usage(char *name)
{
	printf("Usage: %s [-afhnpqvVy] <device> \n", basename(name));
}

static void version(void)
{
	printf( _("GFS2 fsck %s (built %s %s)\n"),
	       VERSION, __DATE__, __TIME__);
	printf(REDHAT_COPYRIGHT "\n");
}

static int read_cmdline(int argc, char **argv, struct gfs2_options *gopts)
{
	int c;

	while ((c = getopt(argc, argv, "afhnpqvyV")) != -1) {
		switch(c) {

		case 'a':
		case 'p':
			if (gopts->yes || gopts->no) {
				fprintf(stderr, _("Options -p/-a, -y and -n may not be used together\n"));
				return FSCK_USAGE;
			}
			preen = 1;
			gopts->yes = 1;
			break;
		case 'f':
			force_check = 1;
			break;
		case 'h':
			usage(argv[0]);
			exit(FSCK_OK);
			break;
		case 'n':
			if (gopts->yes || preen) {
				fprintf(stderr, _("Options -p/-a, -y and -n may not be used together\n"));
				return FSCK_USAGE;
			}
			gopts->no = 1;
			break;
		case 'q':
			decrease_verbosity();
			break;
		case 'v':
			increase_verbosity();
			break;
		case 'V':
			version();
			exit(FSCK_OK);
			break;
		case 'y':
			if (gopts->no || preen) {
				fprintf(stderr, _("Options -p/-a, -y and -n may not be used together\n"));
				return FSCK_USAGE;
			}
			gopts->yes = 1;
			break;
		case ':':
		case '?':
			fprintf(stderr, _("Please use '-h' for help.\n"));
			return FSCK_USAGE;
		default:
			fprintf(stderr, _("Invalid option %c\n"), c);
			return FSCK_USAGE;

		}
	}
	if (argc > optind) {
		gopts->device = (argv[optind]);
		if (!gopts->device) {
			fprintf(stderr, _("Please use '-h' for help.\n"));
			return FSCK_USAGE;
		}
	} else {
		fprintf(stderr, _("No device specified (Please use '-h' for help)\n"));
		return FSCK_USAGE;
	}
	return 0;
}

static void interrupt(int sig)
{
	char response;
	char progress[PATH_MAX];

	if (!last_reported_block || last_reported_block == last_fs_block)
		sprintf(progress, _("progress unknown.\n"));
	else
		sprintf(progress, _("processing block %llu out of %llu\n"),
			(unsigned long long)last_reported_block,
			(unsigned long long)last_fs_block);
	
	response = generic_interrupt("fsck.gfs2", pass, progress,
				     _("Do you want to abort fsck.gfs2, skip " \
				     "the rest of this pass or continue " \
				     "(a/s/c)?"), "asc");
	if (tolower(response) == 's') {
		skip_this_pass = TRUE;
		return;
	}
	else if (tolower(response) == 'a') {
		fsck_abort = TRUE;
		return;
	}
}

static int check_statfs(struct gfs2_sbd *sdp)
{
	struct osi_node *n, *next = NULL;
	struct rgrp_tree *rgd;
	struct gfs2_rindex *ri;
	struct gfs2_statfs_change sc = {0,};
	char buf[sizeof(struct gfs2_statfs_change)];
	int count;

	if (sdp->gfs1 && !sdp->md.statfs->i_di.di_size) {
		log_info("This GFS1 file system is not using fast_statfs.\n");
		return 0;
	}
	/* Read the current statfs values */
	count = gfs2_readi(sdp->md.statfs, buf, 0,
			   sdp->md.statfs->i_di.di_size);
	if (count != sizeof(struct gfs2_statfs_change)) {
		log_err(_("Failed to read statfs values (%d of %"PRIu64" read)\n"),
		        count, (uint64_t)sdp->md.statfs->i_di.di_size);
		return FSCK_ERROR;
	}
	gfs2_statfs_change_in(&sc, buf);
	/* Calculate the real values from the rgrp information */
	sdp->blks_total = 0;
	sdp->blks_alloced = 0;
	sdp->dinodes_alloced = 0;

	for (n = osi_first(&sdp->rgtree); n; n = next) {
		next = osi_next(n);
		rgd = (struct rgrp_tree *)n;
		ri = &rgd->ri;
		sdp->blks_total += ri->ri_data;
		sdp->blks_alloced += (ri->ri_data - rgd->rg.rg_free);
		sdp->dinodes_alloced += rgd->rg.rg_dinodes;
	}

	/* See if they match */
	if (sc.sc_total == sdp->blks_total &&
	    sc.sc_free == (sdp->blks_total - sdp->blks_alloced) &&
	    sc.sc_dinodes == sdp->dinodes_alloced) {
		log_info( _("The statfs file is accurate.\n"));
		return 0;
	}
	log_err( _("The statfs file is wrong:\n\n"));
	log_err( _("Current statfs values:\n"));
	log_err( _("blocks:  %lld (0x%llx)\n"),
		 (unsigned long long)sc.sc_total,
		 (unsigned long long)sc.sc_total);
	log_err( _("free:    %lld (0x%llx)\n"),
		 (unsigned long long)sc.sc_free,
		 (unsigned long long)sc.sc_free);
	log_err( _("dinodes: %lld (0x%llx)\n\n"),
		 (unsigned long long)sc.sc_dinodes,
		 (unsigned long long)sc.sc_dinodes);

	log_err( _("Calculated statfs values:\n"));
	log_err( _("blocks:  %lld (0x%llx)\n"),
		 (unsigned long long)sdp->blks_total,
		 (unsigned long long)sdp->blks_total);
	log_err( _("free:    %lld (0x%llx)\n"),
		 (unsigned long long)(sdp->blks_total - sdp->blks_alloced),
		 (unsigned long long)(sdp->blks_total - sdp->blks_alloced));
	log_err( _("dinodes: %lld (0x%llx)\n"),
		 (unsigned long long)sdp->dinodes_alloced,
		 (unsigned long long)sdp->dinodes_alloced);

	errors_found++;
	if (!query( _("Okay to fix the master statfs file? (y/n)"))) {
		log_err( _("The statfs file was not fixed.\n"));
		return 0;
	}

	do_init_statfs(sdp);
	log_err( _("The statfs file was fixed.\n"));
	errors_corrected++;
	return 0;
}

static const struct fsck_pass passes[] = {
	{ .name = "pass1",  .f = pass1 },
	{ .name = "pass1b", .f = pass1b },
	{ .name = "pass2",  .f = pass2 },
	{ .name = "pass3",  .f = pass3 },
	{ .name = "pass4",  .f = pass4 },
	{ .name = "check_statfs", .f = check_statfs },
	{ .name = NULL, }
};

static int fsck_pass(const struct fsck_pass *p, struct gfs2_sbd *sdp)
{
	int ret;
	struct timeval timer;

	if (fsck_abort)
		return FSCK_CANCELED;
	pass = p->name;

	log_notice( _("Starting %s\n"), p->name);
	gettimeofday(&timer, NULL);

	ret = p->f(sdp);
	if (ret)
		exit(ret);
	if (skip_this_pass || fsck_abort) {
		skip_this_pass = 0;
		log_notice( _("%s interrupted   \n"), p->name);
		return FSCK_CANCELED;
	}

	print_pass_duration(p->name, &timer);
	return 0;
}

static void exitlog(int status, void *unused)
{
	syslog(LOG_INFO, "exit: %d", status);
}

static void startlog(int argc, char **argv)
{
	int i;
	char *cmd, *p;
	size_t len;

	for (len = i = 0; i < argc; i++)
		len += strlen(argv[i]);
	len += argc; /* Add spaces and '\0' */

	cmd = malloc(len);
	if (cmd == NULL) {
		perror(argv[0]);
		exit(FSCK_ERROR);
	}
	p = cmd;
	for (i = 0; i < argc; i++, p++) {
		p = stpcpy(p, argv[i]);
		*p = ' ';
	}
	*(--p) = '\0';
	syslog(LOG_INFO, "started: %s", cmd);
	free(cmd);
}

int main(int argc, char **argv)
{
	struct gfs2_sbd sb;
	struct gfs2_sbd *sdp = &sb;
	int j;
	int i;
	int error = 0;
	int all_clean = 0;
	struct sigaction act = { .sa_handler = interrupt, };

	setlocale(LC_ALL, "");
	textdomain("gfs2-utils");

	openlog("fsck.gfs2", LOG_CONS|LOG_PID, LOG_USER);
	startlog(argc - 1, &argv[1]);
	on_exit(exitlog, NULL);

	memset(sdp, 0, sizeof(*sdp));

	if ((error = read_cmdline(argc, argv, &opts)))
		exit(error);
	setbuf(stdout, NULL);
	log_notice( _("Initializing fsck\n"));
	if ((error = initialize(sdp, force_check, preen, &all_clean)))
		exit(error);

	if (!force_check && all_clean && preen) {
		log_err( _("%s: clean.\n"), opts.device);
		destroy(sdp);
		exit(FSCK_OK);
	}

	sigaction(SIGINT, &act, NULL);

	for (i = 0; passes[i].name; i++)
		error = fsck_pass(passes + i, sdp);

	/* Free up our system inodes */
	if (!sdp->gfs1)
		inode_put(&sdp->md.inum);
	inode_put(&sdp->md.statfs);
	for (j = 0; j < sdp->md.journals; j++)
		inode_put(&sdp->md.journal[j]);
	free(sdp->md.journal);
	sdp->md.journal = NULL;
	inode_put(&sdp->md.jiinode);
	inode_put(&sdp->md.riinode);
	inode_put(&sdp->md.qinode);
	if (!sdp->gfs1)
		inode_put(&sdp->md.pinode);
	inode_put(&sdp->md.rooti);
	if (!sdp->gfs1)
		inode_put(&sdp->master_dir);
	if (lf_dip)
		inode_put(&lf_dip);

	if (!opts.no && errors_corrected)
		log_notice( _("Writing changes to disk\n"));
	fsync(sdp->device_fd);
	link1_destroy(&nlink1map);
	link1_destroy(&clink1map);
	destroy(sdp);
	if (sb_fixed)
		log_warn(_("Superblock was reset. Use tunegfs2 to manually "
		           "set lock table before mounting.\n"));
	log_notice( _("fsck.gfs2 complete\n"));

	if (!error) {
		if (!errors_found)
			error = FSCK_OK;
		else if (errors_found == errors_corrected)
			error = FSCK_NONDESTRUCT;
		else
			error = FSCK_UNCORRECTED;
	}
	exit(error);
}
