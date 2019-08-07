#include "clusterautoconfig.h"
/**
 * glocktop.c - list/print the top GFS2 glock waiters
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <dirent.h>
#include <curses.h>
#include <term.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>
#include <libgfs2.h>

#define MAX_GLOCKS 20
#define MAX_LINES 6000
#define MAX_MOUNT_POINTS 100
#define MAX_FILES 512
#define MAX_CALLTRACE_LINES 4
#define TITLE1 "glocktop - GFS2 glock monitor"
#define TITLE2 "Press <ctrl-c> or <escape> to exit"

#define COLOR_TITLE     1
#define COLOR_NORMAL    2
#define COLOR_INVERSE   3
#define COLOR_SPECIAL   4
#define COLOR_HIGHLIGHT 5
#define COLOR_OFFSETS   6
#define COLOR_CONTENTS  7
#define COLOR_HELD      8

/*	init_pair(COLOR_TITLE, COLOR_BLACK,  COLOR_CYAN);
	init_pair(COLOR_INVERSE, COLOR_BLACK,  COLOR_WHITE);
	init_pair(COLOR_NORMAL, COLOR_WHITE,  COLOR_BLACK);
	init_pair(COLOR_SPECIAL, COLOR_MAGENTA, COLOR_WHITE);
	init_pair(COLOR_HIGHLIGHT, COLOR_WHITE, COLOR_BLUE);
	init_pair(COLOR_OFFSETS, COLOR_CYAN,   COLOR_WHITE);
	init_pair(COLOR_CONTENTS, COLOR_BLUE, COLOR_WHITE);
	init_pair(COLOR_HELD, COLOR_CYAN, COLOR_BLACK);
*/

#define STR_BLACK "[\033[0;30m]"
#define STR_RED "[\033[0;31m]"
#define STR_GREEN "[\033[0;32m]"
#define STR_YELLOW "[\033[0;33m]"
#define STR_BLUE "[\033[0;34m]"
#define STR_MAGENTA "[\033[0;35m]"
#define STR_CYAN "[\033[0;36m]"
#define STR_WHITE "[\033[0;37m]"

#define BOLD_WHITE "[\033[1;37m]"

#define BKG_CYAN "[\033[46m]"
#define BKG_WHITE "[\033[47m]"
#define BKG_BLUE "[\033[44m]"

#define REFRESH_TIME    30
#define COLORS_TITLE    \
	do {						   \
		if (termlines)				   \
			attrset(COLOR_PAIR(COLOR_TITLE));  \
		else 					   \
			printf(BKG_CYAN);		   \
	} while (0)
#define COLORS_NORMAL_BOLD    \
	do {						   \
		if (termlines) {			   \
			attrset(COLOR_PAIR(COLOR_NORMAL)); \
			attron(A_BOLD);			   \
		} else {				   \
			printf(BOLD_WHITE);		   \
		}					   \
	} while (0)
#define COLORS_NORMAL    \
	do {						   \
		if (termlines) {			   \
			attrset(COLOR_PAIR(COLOR_NORMAL)); \
		} else {				   \
			printf(STR_WHITE);		   \
		}					   \
	} while (0)
#define COLORS_INVERSE_BOLD   \
	do {						    \
		if (termlines) {			    \
			attrset(COLOR_PAIR(COLOR_INVERSE)); \
			attron(A_BOLD);			    \
		} else {				    \
			printf(BKG_WHITE);		    \
		}					    \
	} while (0)
#define COLORS_INVERSE   \
	do {						    \
		if (termlines) {			    \
			attrset(COLOR_PAIR(COLOR_INVERSE)); \
		} else {				    \
			printf(BKG_WHITE);		    \
		}					    \
	} while (0)
#define COLORS_HELD   \
	do {						    \
		if (termlines) {			    \
			attrset(COLOR_PAIR(COLOR_HELD));    \
		} else {				    \
			printf(STR_CYAN);		    \
		}					    \
	} while (0)
#define COLORS_HIGHLIGHT   \
	do {						    \
		if (termlines) {			    \
			attrset(COLOR_PAIR(COLOR_HIGHLIGHT));	\
		} else {				    \
			printf(BKG_BLUE);		    \
		}					    \
	} while (0)
#define DLM_DIRTBL "/sys/kernel/config/dlm/cluster/dirtbl_size"
#define DLM_RSBTBL "/sys/kernel/config/dlm/cluster/rsbtbl_size"
#define DLM_LKBTBL "/sys/kernel/config/dlm/cluster/lkbtbl_size"

#define GFS2_MAX_META_HEIGHT	10

#define DETAILS  0x00000001
#define FRIENDLY 0x00000002

enum summary_types {
	all = 0,
	locked = 1,
	held_ex = 2,
	held_sh = 3,
	held_df = 4,
	has_waiter = 5,
	tot_waiters = 6,
	stypes = 7,
};

char debugfs[PATH_MAX];
int termcols = 80, termlines = 30, done = 0;
unsigned glocks = 0;
const char *termtype;
WINDOW *wind;
int bufsize = 4 * 1024 * 1024;
char *glock[MAX_GLOCKS];
int iterations = 0, show_reservations = 0, iters_done = 0;
char devices[MAX_MOUNT_POINTS][80];
char mount_points[MAX_MOUNT_POINTS][80];
int fs_fd[MAX_MOUNT_POINTS];
int mounted = 0;
char dlmwlines[100][96]; /* waiters lines */
char dlmglines[MAX_LINES][97]; /* granted lines */
char contended_filenames[MAX_FILES][PATH_MAX];
unsigned long long contended_blocks[MAX_FILES];
int contended_count = 0;
int line = 0;
const char *prog_name;
char dlm_dirtbl_size[32], dlm_rsbtbl_size[32], dlm_lkbtbl_size[32];
int bsize = 0;
struct gfs2_sb sd_sb[MAX_MOUNT_POINTS];
int sd_diptrs = 0, sd_inptrs = 0;
uint64_t sd_heightsize[GFS2_MAX_META_HEIGHT];
uint64_t sd_jheightsize[GFS2_MAX_META_HEIGHT];
int sd_max_height, sd_max_jheight;
char print_dlm_grants = 1;
char *gbuf = NULL; /* glocks buffer */
char *gpos = NULL;
char *gnextpos = NULL;
int gmaxpos = 0;

char *dbuf = NULL; /* dlm locks buffer */
char *dpos = NULL;
char *dnextpos = NULL;
int dmaxpos = 0;
char hostname[256];

/*
 * init_colors
 */
static void init_colors(void)
{
	init_pair(COLOR_TITLE, COLOR_BLACK,  COLOR_CYAN);
	init_pair(COLOR_INVERSE, COLOR_BLACK,  COLOR_WHITE);
	init_pair(COLOR_NORMAL, COLOR_WHITE,  COLOR_BLACK);
	init_pair(COLOR_SPECIAL, COLOR_MAGENTA, COLOR_WHITE);
	init_pair(COLOR_HIGHLIGHT, COLOR_WHITE, COLOR_BLUE);
	init_pair(COLOR_OFFSETS, COLOR_CYAN,   COLOR_WHITE);
	init_pair(COLOR_CONTENTS, COLOR_BLUE, COLOR_WHITE);
	init_pair(COLOR_HELD, COLOR_CYAN, COLOR_BLACK);
}

/*
 * UpdateSize - screen size changed, so update it
 */
static void UpdateSize(int sig)
{
	static char term_buffer[2048];
	int rc;

	if (termlines) {
		termlines = 30;
		termtype = getenv("TERM");
		if (termtype == NULL)
			return;
		rc=tgetent(term_buffer,termtype);
		if (rc >= 0) {
			termlines = tgetnum((char *)"li");
			if (termlines < 10)
				termlines = 30;
			termcols = tgetnum((char *)"co");
			if (termcols < 80)
				termcols = 80;
		} else
			perror("Error: tgetent failed.");
		termlines--; /* last line is number of lines -1 */
	}
	signal(SIGWINCH, UpdateSize);
}

static void read_superblock(int fd, int mntpt)
{
	struct gfs2_sbd sbd = { .device_fd = fd, .bsize = GFS2_BASIC_BLOCK };
	struct gfs2_buffer_head *bh;
	int x;
	uint64_t space = 0;

	ioctl(fd, BLKFLSBUF, 0);
	bh = bread(&sbd, GFS2_SB_ADDR);
	gfs2_sb_in(&sd_sb[mntpt], bh->b_data);
	bsize = sd_sb[mntpt].sb_bsize;
	if (!bsize)
		bsize = 4096;
	sd_inptrs = (bsize - sizeof(struct gfs2_meta_header)) /
		sizeof(uint64_t);
	sd_diptrs = (bsize - sizeof(struct gfs2_dinode)) /
		sizeof(uint64_t);
	sd_heightsize[0] = bsize - sizeof(struct gfs2_dinode);
	sd_heightsize[1] = bsize * sd_diptrs;
	for (x = 2; ; x++) {
		space = sd_heightsize[x - 1] * sd_inptrs;
		if (space / sd_inptrs != sd_heightsize[x - 1] ||
		    space % sd_inptrs != 0)
			break;
		sd_heightsize[x] = space;
	}
	sd_jheightsize[0] = bsize - sizeof(struct gfs2_dinode);
	sd_jheightsize[1] = (bsize - sizeof(struct gfs2_meta_header)) *
		sd_diptrs;
	for (x = 2; ; x++){
		space = sd_jheightsize[x - 1] * sd_inptrs;
		if (space / sd_inptrs != sd_jheightsize[x - 1] ||
		    space % sd_inptrs != 0)
			break;
		sd_jheightsize[x] = space;
	}
	sd_max_jheight = x;
}

static int parse_mounts(void)
{
	char str[PATH_MAX], dev[PATH_MAX], mnt[PATH_MAX], mtype[PATH_MAX];
	char opts[PATH_MAX];
	FILE *fp;

	memset(debugfs, 0, sizeof(debugfs));
	memset(mount_points, 0, sizeof(mount_points));
	memset(devices, 0, sizeof(devices));

	fp = fopen("/proc/mounts", "rt");
	if (fp == NULL) {
		perror("/proc/mounts");
		return 1;
	}
	while (fgets(str, sizeof(str) - 1, fp)) {
		sscanf(str, "%s %s %s %s", dev, mnt, mtype, opts);
		if (!strcmp(mtype, "debugfs")) {
			strcpy(debugfs, mnt);
			continue;
		}
		if (strcmp(mtype, "gfs2")) /* if not gfs2 */
			continue;

		strncpy(mount_points[mounted], mnt, 79);
		mount_points[mounted][79] = '\0';
		strncpy(devices[mounted], dev, 79);
		devices[mounted][79] = '\0';

		/* Now find out the mount point's file system name */
		fs_fd[mounted] = open(dev, O_RDONLY);
		if (fs_fd[mounted])
			read_superblock(fs_fd[mounted], mounted);
		mounted++;
	}
	if (debugfs[0] == '\0') {
		if (mount("debugfs", "/sys/kernel/debug", "debugfs", 0, NULL)){
			fprintf(stderr, "Unable to mount debugfs.\n");
			fprintf(stderr, "Please mount it manually.\n");
			exit(-1);
		}
		strcpy(debugfs, "/sys/kernel/debug");
	}
	fclose(fp);
	return 0;
}

/*
 * display_title_lines
 */
static void display_title_lines(void)
{
	if (termlines) {
		clear(); /* don't use Erase */
		COLORS_TITLE;
		attron(A_BOLD);
		move(0, 0);
		printw("%-80s", TITLE1);
		move(termlines, 0);
		printw("%-79s", TITLE2);
		COLORS_NORMAL_BOLD;
		move(1, 0);
	} else {
		printf("\n");
	}
	line = 1;
}

/*
 * bobgets - get a string
 * returns: 1 if user exited by hitting enter
 *          0 if user exited by hitting escape
 */
static int bobgets(char string[], int x, int y, int sz, int *ch)
{
	int finished,runningy,rc;

	if (!termlines)
		return 0;
	move(x,y);
	finished=FALSE;
	COLORS_INVERSE_BOLD;
	move(x,y);
	addstr(string);
	move(x,y);
	curs_set(2);
	refresh();
	runningy=y;
	rc=0;
	while (!finished) {
		*ch = getch();

		if(*ch < 0x0100 && isprint(*ch)) {
			char *p=string+strlen(string); // end of the string

			*(p+1)='\0';
			string[runningy-y]=*ch;
			runningy++;
			move(x,y);
			addstr(string);
			if (runningy-y >= sz) {
				rc=1;
				*ch = KEY_RIGHT;
				finished = TRUE;
			}
		}
		else {
			// special character, is it one we recognize?
			switch(*ch)
			{
			case(KEY_ENTER):
			case('\n'):
			case('\r'):
				rc=1;
				finished=TRUE;
				string[runningy-y] = '\0';
				break;
			case(KEY_CANCEL):
			case(0x01B):
				rc=0;
				finished=TRUE;
				break;
			case(KEY_DC):
			case(0x07F):
				if (runningy>=y) {
					char *p;
					p = &string[runningy - y];
					while (*p) {
						*p = *(p + 1);
						p++;
					}
					*p = '\0';
					runningy--;
					// remove the character from the string
					move(x,y);
					addstr(string);
					COLORS_NORMAL_BOLD;
					addstr(" ");
					COLORS_INVERSE_BOLD;
					runningy++;
				}
				break;
			case(KEY_BACKSPACE):
				if (runningy>y) {
					char *p;

					p = &string[runningy - y - 1];
					while (*p) {
						*p = *(p + 1);
						p++;
					}
					*p='\0';
					runningy--;
					// remove the character from the string
					move(x,y);
					addstr(string);
					COLORS_NORMAL_BOLD;
					addstr(" ");
					COLORS_INVERSE_BOLD;
				}
				break;
			default:
				move(0,70);
				printw("%08x", *ch);
				// ignore all other characters
				break;
			} // end switch on non-printable character
		} // end non-printable character
		move(line, runningy);
		refresh();
	} // while !finished
	if (sz>0)
		string[sz]='\0';
	COLORS_NORMAL_BOLD;
	return rc;
}/* bobgets */

static char *bufgets(int fd, char *bigbuf, char **nextpos, char **pos,
		     int *maxpos)
{
	if (*nextpos == NULL) {
		*maxpos = read(fd, bigbuf, bufsize - 1);
		bigbuf[bufsize - 1] = '\0';
		if (*maxpos == 0)
			return NULL;
		*pos = bigbuf;
	} else
		*pos = *nextpos;

	*nextpos = memchr(*pos, '\n', (bigbuf + *maxpos) - *pos);
	while (*nextpos && (**nextpos == '\n' || **nextpos == '\r') &&
	       *nextpos < bigbuf + (bufsize - 1)) {
		**nextpos = '\0';
		(*nextpos)++;
	}
	if (*nextpos >= bigbuf + *maxpos)
		*nextpos = NULL;
	return *pos;
}

static char *glock_number(const char *str)
{
	const char *glockid;
	char *p;
	static char id[32];

	glockid = strchr(str, '/');
	if (glockid == NULL)
		return NULL;
	glockid++;
	strncpy(id, glockid, sizeof(id));
	id[31] = '\0';
	p = strchr(id, ' ');
	if (p)
		*p = '\0';
	return id;
}

static int this_glock_requested(const char *str)
{
	const char *glockid;
	int i;

	if (!glocks)
		return 0;

	glockid = glock_number(str);
	if (glockid == NULL)
		return 0;
	for (i = 0; i < glocks; i++)
		if (!strcmp(glockid, glock[i]))
			return 1;
	return 0;
}

static int is_iopen(const char *str)
{
	char *p;

	p = strchr(str, '/');
	if (p == NULL)
		return 0;
	p--;
	if (*p == '5')
		return 1;
	return 0;
}

static int this_lkb_requested(const char *str)
{
	int i;

	if (!glocks)
		return 1;

	for (i = 0; i < glocks; i++) {
		if (strstr(str, glock[i]))
			return 1;
	}
	return 0;
}

static void eol(int col) /* end of line */
{
	if (termlines) {
		line++;
		move(line, col);
	} else {
		printf("\n");
		for (; col > 0; col--)
			printf(" ");
	}
}

void print_it(const char *label, const char *fmt, const char *fmt2, ...)
{
	va_list args;
	char tmp_string[128];

	if (!termlines || line < termlines) {
		va_start(args, fmt2);
		vsnprintf(tmp_string, 127, fmt, args);
		tmp_string[127] = '\0';

		if (termlines) {
			printw("%s", tmp_string);
			refresh();
		} else {
			printf("%s", tmp_string);
			fflush(stdout);
		}
	}
	va_end(args);
}

static void display_filename(int fd, unsigned long long block,
			     unsigned long long dirarray[256], int subdepth)
{
	int i, subs;
	char *mntpt = NULL;
	char blk[32];
	DIR *dir = NULL;
	struct dirent *dent;

	for (i = 0; i < mounted; i++) {
		if (fd == fs_fd[i]) {
			mntpt = mount_points[i];
			break;
		}
	}
	if (i == mounted)
		return;
	for (i = 0; i < contended_count; i++) {
		if (contended_blocks[i] == block) {
			break;
		}
	}
	sprintf(blk, "%lld", block);
	if (i >= contended_count) {
		memset(contended_filenames[i], 0, PATH_MAX);
		strcat(contended_filenames[i], mntpt);
		for (subs = subdepth - 2; subs >= 0; subs--) {
			dir = opendir(contended_filenames[i]);
			while ((dent = readdir(dir))) {
				if (dent->d_ino == dirarray[subs]) {
					strcat(contended_filenames[i], "/");
					strcat(contended_filenames[i],
					       dent->d_name);
					break;
				}
			}
			closedir(dir);
		}
	}

	print_it(NULL, "%s", NULL, contended_filenames[i]);
	eol(0);
}

static const char *show_inode(const char *id, int fd, unsigned long long block)
{
	struct gfs2_inode *ip;
	const char *inode_type = NULL;
	struct gfs2_sbd sbd = { .device_fd = fd, .bsize = bsize };

	ip = lgfs2_inode_read(&sbd, block);
	if (S_ISDIR(ip->i_di.di_mode)) {
		struct gfs2_inode *parent;
		unsigned long long dirarray[256];
		int subdepth = 0, error;

		inode_type = "directory ";
		dirarray[0] = block;
		subdepth++;
		/* Backtrack the directory to its source */
		while (1) {
			error = gfs2_lookupi(ip, "..", 2, &parent);
			if (error)
				break;
			/* Stop at the root inode */
			if (ip->i_di.di_num.no_addr ==
			    parent->i_di.di_num.no_addr) {
				inode_put(&parent);
				break;
			}
			inode_put(&ip);
			ip = parent;
			dirarray[subdepth++] = parent->i_di.di_num.no_addr;
		}
		display_filename(fd, block, dirarray, subdepth);
	} else if (S_ISREG(ip->i_di.di_mode)) {
		inode_type = "file ";
	} else if (S_ISLNK(ip->i_di.di_mode)) {
		inode_type = "link ";
	} else if (S_ISCHR(ip->i_di.di_mode)) {
		inode_type = "char device ";
	} else if (S_ISBLK(ip->i_di.di_mode)) {
		inode_type = "block device ";
	} else if (S_ISFIFO(ip->i_di.di_mode)) {
		inode_type = "fifo ";
	} else if (S_ISSOCK(ip->i_di.di_mode)) {
		inode_type = "socket ";
	} else
		inode_type = "file? ";
	inode_put(&ip);
	return inode_type;
}

static const char *show_details(const char *id, const char *fsname, int btype,
				int trace_dir_path)
{
	int mnt_num;
	unsigned long long block = 0;
	const char *blk_type = NULL;
	FILE *dlmf;

	/* Figure out which mount point corresponds to this debugfs id */
	for (mnt_num = 0; mnt_num < mounted; mnt_num++) {
		char *p;

		p = strchr(sd_sb[mnt_num].sb_locktable, ':');
		if (!p)
			continue;
		p++;
		if (!strcmp(p, fsname))
			break;
	}
	memset(dlm_dirtbl_size, 0, sizeof(dlm_dirtbl_size));
	memset(dlm_rsbtbl_size, 0, sizeof(dlm_rsbtbl_size));
	memset(dlm_lkbtbl_size, 0, sizeof(dlm_lkbtbl_size));
	if (!strcmp(sd_sb[mnt_num].sb_lockproto, "lock_dlm")) {
		char *sp;
		char *p;

		dlmf = fopen(DLM_DIRTBL, "rt");
		if (dlmf) {
			sp = fgets(dlm_dirtbl_size, sizeof(dlm_dirtbl_size), dlmf);
			if (sp == NULL)
				goto out_err;
			p = strchr(dlm_dirtbl_size, '\n');
			if (p)
				*p = '\0';
			fclose(dlmf);
		} else {
			strcpy(dlm_dirtbl_size, " ");
		}
		dlmf = fopen(DLM_RSBTBL, "rt");
		if (dlmf) {
			sp = fgets(dlm_rsbtbl_size, sizeof(dlm_rsbtbl_size), dlmf);
			if (sp == NULL)
				goto out_err;
			p = strchr(dlm_rsbtbl_size, '\n');
			if (p)
				*p = '\0';
			fclose(dlmf);
		} else {
			strcpy(dlm_rsbtbl_size, " ");
		}
		dlmf = fopen(DLM_LKBTBL, "rt");
		if (dlmf) {
			sp = fgets(dlm_lkbtbl_size, sizeof(dlm_lkbtbl_size), dlmf);
			if (sp == NULL)
				goto out_err;
			p = strchr(dlm_lkbtbl_size, '\n');
			if (p)
				*p = '\0';
			fclose(dlmf);
		} else {
			strcpy(dlm_lkbtbl_size, " ");
		}
	} else {
		strcpy(dlm_dirtbl_size, "nolock");
		strcpy(dlm_lkbtbl_size, "nolock");
		strcpy(dlm_lkbtbl_size, "nolock");
	}

	if (mnt_num >= mounted) /* can't find the right superblock */
		return "unknown";

	/* Read the inode in so we can see its type. */
	sscanf(id, "%llx", &block);
	if (block) {
		if (btype == 2)
			if (trace_dir_path)
				blk_type = show_inode(id, fs_fd[mnt_num],
						      block);
			else
				blk_type = "";
		else
			blk_type = "";
	}
	return blk_type;
out_err:
	fclose(dlmf);
	return "error";
}

static int is_dlm_waiting(int dlmwaiters, int locktype, char *id)
{
	int i;
	int dlmid, wait_type, nodeid, type;
	char locknum[32];

	for (i = 0; i < dlmwaiters && i < 100; i++) {
		sscanf(dlmwlines[i], "%x %d %d        %d         %s",
		       &dlmid, &wait_type, &nodeid, &type, locknum);
		if ((type == locktype) && (!strcmp(locknum, id)))
			return 1;
	}
	return 0;
}

static const char *friendly_state(const char *glock_line, const char *search)
{
	const char *p;

	p = strstr(glock_line, search);

	if (p == NULL)
		return "Dazed";

	p += 2;
	if (*p == 'E')
		return "Exclusive";
	else if (*p == 'S')
		return "Shared";
	else if (*p == 'U')
		return "Unlocked";
	else if (*p == 'D')
		return "Deferred";
	else
		return "Confused";
}

static const char *friendly_gflags(const char *glock_line)
{
	static char flagout[PATH_MAX];
	const char *p;

	memset(flagout, 0, sizeof(flagout));

	p = strstr(glock_line, "f:");
	if (!p)
		return " ";
	p += 2;
	strcpy(flagout, "[");
	while (*p != ' ') {
		switch (*p) {
		case 'l':
			/*strcat(flagout, "Locked");*/
			break;
		case 'D':
			strcat(flagout, "Demoting");
			break;
		case 'd':
			strcat(flagout, "Demote pending");
			break;
		case 'p':
			strcat(flagout, "Demote in progress");
			break;
		case 'y':
			strcat(flagout, "Dirty");
			break;
		case 'f':
			strcat(flagout, "Flush");
			break;
		case 'i':
			strcat(flagout, "Invalidating");
			break;
		case 'r':
			strcat(flagout, "Reply pending");
			break;
		case 'I':
			/*strcat(flagout, "Initial");*/
			break;
		case 'F':
			strcat(flagout, "Frozen");
			break;
		case 'q':
			strcat(flagout, "Queued");
			break;
		case 'L':
			strcat(flagout, "LRU");
			break;
		case 'o':
			/*strcat(flagout, "Object present");*/
			break;
		case 'b':
			strcat(flagout, "Blocking");
			break;
		default:
			strcat(flagout, "Unknown");
			break;
		}
		if ((strlen(flagout)) > 1 && (!strchr(" lIo", *(p + 1))))
			strcat(flagout, ", ");
		p++;
	}
	strcat(flagout, "]");
	return flagout;
}

static const char *friendly_glock(const char *glock_line, char prefix)
{
	static char gline[PATH_MAX];

	if (prefix == 'W')
		sprintf(gline, "Is:%s, Want:%s   %s",
			friendly_state(glock_line, "s:"),
			friendly_state(glock_line, "t:"),
			friendly_gflags(glock_line));
	else
		sprintf(gline, "Held:%s   %s",
			friendly_state(glock_line, "s:"),
			friendly_gflags(glock_line));
	return gline;
}

static const char *dlm_grtype(int grmode)
{
	const char *dlm_types[8] = {"NL", "CR", "CW", "PR", "PW", "EX",
				    "NA", "NA"};

	if (grmode < 0)
		return "-1";
	return dlm_types[grmode & 0x07];
}

static const char *dlm_status(int status)
{
	const char *dlm_statuses[4] = {"Unknown", "Waiting", "Granted",
				       "Converting"};
	if (status < 0)
		return "unknown";
	return dlm_statuses[status & 0x03];
}

static const char *dlm_nodeid(int lkbnodeid)
{
	static char nodeid[16];

	if (lkbnodeid == 0)
		return "this node";
	sprintf(nodeid, "node %d", lkbnodeid);
	return nodeid;
}

static const char *getprocname(int ownpid)
{
	char fn[1024];
	static char str[80];
	const char *procname;
	FILE *fp;

	sprintf(fn, "/proc/%d/status", ownpid);
	fp = fopen(fn, "r");
	if (fp == NULL)
		return "ended";

	if (fgets(str, 80, fp) != NULL) {
		char *p;

		procname = str + 6;
		p = strchr(procname, '\n');
		if (p)
			*p = '\0';
	} else
		procname = "unknown";

	fclose(fp);
	return procname;
}

static void show_dlm_grants(int locktype, const char *g_line, int dlmgrants,
			    int summary)
{
	int i;
	char dlm_resid[75];
	unsigned int lkb_id, lkbnodeid, remid, ownpid, exflags, flags, status;
	unsigned int grmode, rqmode, nodeid, length;
	unsigned long long xid, us;
	char trgt_res_name[64], res_name[64], *p1, *p2;
	const char *procname;

	p1 = strchr(g_line, '/');
	if (!p1)
		return;
	p1++;
	p2 = strchr(p1, ' ');
	if (!p2)
		return;
	memset(trgt_res_name, 0, sizeof(trgt_res_name));
	memcpy(trgt_res_name, p1, p2 - p1);
	sprintf(dlm_resid, "%8d%16s", locktype, trgt_res_name);
	for (i = 0; i < dlmgrants; i++) {
/*
lkb_id  n   remid  pid x e f s g rq u n ln res_name 1234567890123456
1100003 1 2ae0006 8954 0 0 0 2 5 -1 0 1 24 "       2           102ab"
2a20001 1 30d0001 8934 0 0 0 2 3 -1 0 1 24 "       5           102ab"
  b0001 2  860001 8868 0 0 10000 2 3 -1 0 0 24 "       1               2"
2450001 2 1be0002 8962 0 0 10000 1 -1 5 12214 0 24 "       2           102ab"
*/
		p1 = strchr(dlmglines[i], '\"');
		if (!p1)
			continue;
		p1++;
		if (strncmp(dlm_resid, p1, 24))
			continue;

		sscanf(dlmglines[i], "%x %d %x %u %llu %x %x %d %d %d %llu "
		       "%u %d \"%24s\"\n",
		       &lkb_id, &lkbnodeid, &remid, &ownpid, &xid, &exflags,
		       &flags, &status, &grmode, &rqmode, &us, &nodeid,
		       &length, res_name);
		if (status == 1) { /* Waiting */
			if (!lkbnodeid)
				procname = getprocname(ownpid);
			else
				procname = "";
			if (summary)
				print_it(NULL, " (", NULL);
			else
				print_it(NULL, "  D: ", NULL);
			print_it(NULL, "%s for %s, pid %d %s", NULL,
				 dlm_status(status), dlm_nodeid(lkbnodeid),
				 ownpid, procname);
			if (summary)
				print_it(NULL, ")", NULL);
		} else if (grmode == 0) {
			continue; /* ignore "D: Granted NL on node X" */
		} else {
			procname = getprocname(ownpid);
			if (summary)
				print_it(NULL, " (", NULL);
			else
				print_it(NULL, "  D: ", NULL);
			print_it(NULL, "%s %s on %s to pid %d %s", NULL,
				 dlm_status(status), dlm_grtype(grmode),
				 dlm_nodeid(lkbnodeid), ownpid, procname);
			if (summary)
				print_it(NULL, ")", NULL);
		}
		if (!summary)
			eol(0);
	}
}

static void print_call_trace(const char *hline)
{
	char *p, *pid, tmp[32], stackfn[64], str[96];
	FILE *fp;
	int i;

	p = strchr(hline, 'p');
	if (!p)
		return;
	pid = p + 2;
	p = strchr(pid, ' ');
	if (!p)
		return;
	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, pid, p - pid);
	sprintf(stackfn, "/proc/%s/stack", tmp);
	fp = fopen(stackfn, "rt");
	if (fp == NULL)
		return;
	for (i = 0; i < MAX_CALLTRACE_LINES; i++) {
		if (fgets(str, sizeof(str) - 1, fp) == NULL)
			break;
		if (strstr(str, "gfs2_glock_")) { /* skip lines we don't
						      care about*/
			i--;
			continue;
		}
		p = strchr(str, '\n');
		if (p)
			*p = '\0';
		p = strchr(str, ']');
		if (p)
			p += 2;
		else
			p = str;
		print_it(NULL, "  C:              %s ", NULL, p);
		eol(0);
	}
	fclose(fp);
}

static int is_ex(const char *hline)
{
	if (strncmp(hline, " H: s:EX ", 9) == 0)
		return 1;
	return 0;
}

static int has_holder_flag(const char *hline, char flag)
{
	const char *p;

	p = strchr(hline, 'f');
	if (p == NULL)
		return 0;
	p++;
	if (*p != ':')
		return 0;
	p++;
	while (*p != '\0') {
		if (*p == ' ')
			return 0;
		if (*p == flag)
			return 1;
		p++;
	}
	return 0;
}

static int is_holder(const char *hline)
{
	return has_holder_flag(hline, 'H');
}

static int is_waiter(const char *hline)
{
	return has_holder_flag(hline, 'W');
}

static int get_lock_type(const char *str)
{
	const char *p;

	p = strchr(str, '/');
	return (p ? (*(p - 1)) - '0' : 0);
}

static long long get_demote_time(const char *str)
{
	char *p;
	char tmp[80];

	p = strchr(str, '/');
	if (p == NULL)
		return 0;
	p++;
	p = strchr(p, '/');
	if (p == NULL)
		return 0;
	p++;
	strncpy(tmp, p, 79);
	tmp[79] = '\0';
	p = strchr(tmp, ' ');
	if (p == NULL)
		return 0;
	*p = '\0';
	return atoll(tmp);
}

static const char *pid_string(const char *str)
{
	char *p;
	static char pidstr[80];

	memset(pidstr, 0, sizeof(pidstr));
	p = strchr(str, 'p');
	if (p) {
		strncpy(pidstr, p + 2, sizeof(pidstr));
		pidstr[79] = '\0';
		p = strchr(pidstr, ']');
		if (p) {
			p++;
			*p = '\0';
		}
	}
	return pidstr;
}

/* If this glock is relevant, return 0, else the reason it's irrelevant */
static int irrelevant(const char *holder, const char *glockstr)
{
	int lock_type = get_lock_type(glockstr);

	/* Exclude shared and locks */
	if (!is_ex(holder))
		return 1;
	/* Exclude locks held at mount time: statfs*/
	if (strstr(holder, "init_per_node"))
		return 2;
	if (strstr(holder, "init_journal"))
		return 3;
	if (strstr(holder, "init_inodes"))
		return 4;
	if (strstr(holder, "fill_super"))
		return 5;
	if (lock_type == 9) /* Exclude journal locks */
		return 6;
	return 0;
}

static const char *reason(int why)
{
	const char *reasons[] = {"(N/A:------)",  /* 0 */
				 "(N/A:Not EX)",  /* 1 */
				 "(N/A:System)",  /* 2 */
				 "(N/A:journl)",  /* 3 */
				 "(N/A:System)",  /* 4 */
				 "(N/A:System)",  /* 5 */
				 "(N/A:Journl)"}; /* 6 */

	return reasons[why];
}

static void print_friendly_prefix(char one_glocks_lines[MAX_LINES][97])
{
	int why = irrelevant(one_glocks_lines[1], one_glocks_lines[0]);

	if (why)
		print_it(NULL, "  U: %s ", NULL, reason(why));
	else
		print_it(NULL, "  U: ", NULL);
}

static void show_glock(char one_glocks_lines[MAX_LINES][97], int gline,
		       const char *fsname, int dlmwaiters, int dlmgrants,
		       int trace_dir_path, int prev_had_waiter, int flags,
		       int summary)
{
	int i, locktype = 0;
	char id[33], *p;
	char extras[80], prefix = '\0';
	long long demote_time = 0;
	const char *ltype[] = {"N/A", "non-disk", "inode", "rgrp", "meta",
			       "i_open", "flock", "posix lock", "quota",
			       "journal"};

	if (termlines) {
		if (irrelevant(one_glocks_lines[1], one_glocks_lines[0]))
			COLORS_HELD;
		else
			COLORS_NORMAL;
	}
	if (!gline)
		return;

	memset(extras, 0, sizeof(extras));
	p = strchr(one_glocks_lines[0], '/');
	memset(id, 0, sizeof(id));

	if (p) {
		locktype = get_lock_type(one_glocks_lines[0]);
		demote_time = get_demote_time(one_glocks_lines[0]);
		p++;
		strncpy(id, p, sizeof(id) - 1);
		id[sizeof(id) - 1] = '\0';
		p = strchr(id, ' ');
		if (p)
			*p = '\0';

		if (locktype != 2) {
			strncpy(extras, ltype[locktype], 79);
			extras[79] = '\0';
		} else {
			const char *i_type = show_details(id, fsname, 2,
							  trace_dir_path);
			sprintf(extras, "%sinode", i_type);
		}
	}
	if (flags & DETAILS) {
		print_it(NULL, " %s ", NULL, one_glocks_lines[0]);
		print_it(NULL, "(%s)", NULL, extras);
		if (demote_time)
			print_it(NULL, " ** demote time is greater than 0 **",
				 NULL);
		eol(0);
		if (dlmgrants)
			show_dlm_grants(locktype, one_glocks_lines[0],
					dlmgrants, 0);
	}
	if (flags & FRIENDLY) {
		print_friendly_prefix(one_glocks_lines);
		for (i = 1; i < gline; i++) {
			if (one_glocks_lines[i][0] == ' ' &&
			    one_glocks_lines[i][1] == 'H' &&
			    prefix != 'W')
				prefix = (is_holder(one_glocks_lines[i]) ?
					  'H' : 'W');
		}
		print_it(NULL, " %c %-10.10s %-9.9s %s", NULL, prefix,
			 extras, id, friendly_glock(one_glocks_lines[0],
						    prefix));
		eol(0);
	}
	for (i = 1; i < gline; i++) {
		if (!show_reservations &&
		    one_glocks_lines[i][0] == ' ' &&
		    one_glocks_lines[i][2] == 'B' &&
		    one_glocks_lines[i][3] == ':')
			continue;

		if (flags & DETAILS) {
			print_it(NULL, " %-80.80s", NULL, one_glocks_lines[i]);
			eol(0);
			continue;
		}
		if ((flags & FRIENDLY) &&
		    one_glocks_lines[i][1] == 'H')
			print_friendly_prefix(one_glocks_lines);

		if (one_glocks_lines[i][0] == ' ' &&
		    one_glocks_lines[i][1] == 'H') {
			print_it(NULL, " %c ---> %s pid %s ", NULL,
				 prefix, (is_holder(one_glocks_lines[i]) ?
					  "held by" : "waiting"),
				 pid_string(one_glocks_lines[i]));
			if (demote_time)
				print_it(NULL, "** demote time is non-"
					 "zero ** ", NULL);
			if (is_dlm_waiting(dlmwaiters, locktype, id)) {
				print_it(NULL, "***** DLM is in a "
					 "comm wait for this lock "
					 "***** ", NULL);
			}
			show_dlm_grants(locktype, one_glocks_lines[0],
					dlmgrants, 1);
			eol(0);
			print_call_trace(one_glocks_lines[i]);
		}
	}
}

static int parse_dlm_waiters(FILE *dlm, const char *fsname)
{
	int dlml = 0;

	memset(dlmwlines, 0, sizeof(dlmwlines));
	while (fgets(dlmwlines[dlml], 80, dlm))
		dlml++;

	return dlml;
}

static int parse_dlm_grants(int dlmfd, const char *fsname)
{
	int dlml = 0;
	char *dlmline;

	memset(dlmglines, 0, sizeof(dlmglines));
	dnextpos = NULL;
	while ((dlmline = bufgets(dlmfd, dbuf, &dnextpos, &dpos, &dmaxpos))) {
		if (!this_lkb_requested(dlmline))
			continue;
		strncpy(dlmglines[dlml], dlmline, 96);
		dlmglines[dlml][96] = '\0';
		dlml++;
		if (dlml >= MAX_LINES)
			break;
	}
	return dlml;
}

static void print_summary(int total_glocks[11][stypes], int dlmwaiters)
{
	int i;
	int total_unlocked = 0;
	const struct {
		const char *name;
		const int width;
	} column[] = {
		{ "unknown", 7 }, { "nondisk", 7}, { "inode", 8 }, { "rgrp", 7 },
		{ "meta", 4 }, { "iopen", 7 }, { "flock", 7 }, { "p", 1 },
		{ "quota", 5 }, { "jrnl", 4 }, { "Total", 8 }
	};
	const int ncols = sizeof(column) / sizeof(column[0]);

	/* Print column headers */
	print_it(NULL, "S     glocks ", NULL);
	for (i = 1; i < ncols; i++)
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*s ", NULL, column[i].width, column[i].name);
	eol(0);
	print_it(NULL, "S  --------- ", NULL);
	for (i = 1; i < ncols; i++)
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*s ", NULL, column[i].width, "--------");
	eol(0);

	/* Print rows */
	print_it(NULL, "S  Unlocked: ", NULL);
	for (i = 1; i < (ncols - 1); i++) {
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*d ", NULL, column[i].width,
				 total_glocks[i][all] - total_glocks[i][locked]);
		total_unlocked += total_glocks[i][all] -
			total_glocks[i][locked];
	}
	print_it(NULL, "%*d ", NULL, column[i].width, total_unlocked);
	eol(0);
	print_it(NULL, "S    Locked: ", NULL);
	for (i = 1; i < ncols; i++) {
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*d ", NULL, column[i].width,
				 total_glocks[i][locked]);
		total_glocks[10][locked] += total_glocks[i][locked];
	}
	eol(0);
	print_it(NULL, "S     Total: ", NULL);
	for (i = 1; i < ncols; i++) {
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*d ", NULL, column[i].width,
				 total_glocks[i][all]);
		total_glocks[10][all] += total_glocks[i][all];
	}
	eol(0);
	print_it(NULL, "S", NULL);
	eol(0);
	print_it(NULL, "S   Held EX: ", NULL);
	for (i = 1; i < ncols; i++) {
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*d ", NULL, column[i].width,
				 total_glocks[i][held_ex]);
		total_glocks[10][held_ex] += total_glocks[i][held_ex];
	}
	eol(0);
	print_it(NULL, "S   Held SH: ", NULL);
	for (i = 1; i < ncols; i++) {
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*d ", NULL, column[i].width,
				 total_glocks[i][held_sh]);
		total_glocks[10][held_sh] += total_glocks[i][held_sh];
	}
	eol(0);
	print_it(NULL, "S   Held DF: ", NULL);
	for (i = 1; i < ncols; i++) {
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*d ", NULL, column[i].width,
				 total_glocks[i][held_df]);
		total_glocks[10][held_df] += total_glocks[i][held_df];
	}
	eol(0);
	print_it(NULL, "S G Waiting: ", NULL);
	for (i = 1; i < ncols; i++) {
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*d ", NULL, column[i].width,
				 total_glocks[i][has_waiter]);
		total_glocks[10][has_waiter] += total_glocks[i][has_waiter];
	}
	eol(0);
	print_it(NULL, "S P Waiting: ", NULL);
	for (i = 1; i < ncols; i++) {
		if (i != 7 && i != 4) /* Ignore plock and meta */
			print_it(NULL, "%*d ", NULL, column[i].width,
				 total_glocks[i][tot_waiters]);
		total_glocks[10][tot_waiters] += total_glocks[i][tot_waiters];
	}
	eol(0);
	print_it(NULL, "S  DLM wait: %7d", NULL, dlmwaiters);
	eol(0);
	eol(0);
}

/* flags = DETAILS || FRIENDLY or both */
static void glock_details(int fd, const char *fsname, int dlmwaiters,
			  int dlmgrants, int trace_dir_path, int show_held,
			  int summary)
{
	char *ln, *p;
	char one_glocks_lines[MAX_LINES][97];
	int gline = 0;
	int show_prev_glock = 0, prev_had_waiter = 0;
	int total_glocks[11][stypes], locktype = 0;
	int holders_this_glock_ex = 0;
	int holders_this_glock_sh = 0;
	int holders_this_glock_df = 0;
	int waiters_this_glock = 0;

	memset(total_glocks, 0, sizeof(total_glocks));
	gnextpos = NULL;
	while ((ln = bufgets(fd, gbuf, &gnextpos, &gpos, &gmaxpos))) {
		if (ln[0] == ' ' && ln[1] == ' ' && ln[2] == ' ')
			continue;
		if (ln[0] == 'G') {
			/* Summary stuff------------------------------------ */
			if (waiters_this_glock) {
				total_glocks[locktype][tot_waiters] +=
					waiters_this_glock;
				total_glocks[locktype][has_waiter]++;
			}
			if (holders_this_glock_ex)
				total_glocks[locktype][held_ex]++;
			if (holders_this_glock_sh)
				total_glocks[locktype][held_sh]++;
			if (holders_this_glock_df)
				total_glocks[locktype][held_df]++;
			locktype = get_lock_type(ln);
			p = ln + 6;
			if (*p != 'U' || *(p + 1) != 'N')
				total_glocks[locktype][locked]++;
			total_glocks[locktype][all]++;
			holders_this_glock_ex = 0;
			holders_this_glock_sh = 0;
			holders_this_glock_df = 0;
			waiters_this_glock = 0;
			/* Detail stuff------------------------------------- */
			if (show_prev_glock) {
				show_glock(one_glocks_lines, gline, fsname,
					   dlmwaiters, dlmgrants,
					   trace_dir_path, prev_had_waiter,
					   DETAILS, summary);
				show_glock(one_glocks_lines, gline, fsname,
					   dlmwaiters, dlmgrants,
					   trace_dir_path, prev_had_waiter,
					   FRIENDLY, summary);
				memset(one_glocks_lines, 0,
				       sizeof(one_glocks_lines));
				show_prev_glock = 0;
			}
			prev_had_waiter = 0;
			gline = 0;
			if (this_glock_requested(ln))
				show_prev_glock = 1;
		} else if (ln[0] == ' ' && ln[1] == 'H') {
			char *flag = strchr(ln, 'f');
			char *mode = strchr(ln, 's');

			/* Summary stuff------------------------------------ */
			while (flag) {
				flag++;
				switch (*flag) {
				case ':':
					break;
				case 'W':
					waiters_this_glock++;
					flag = NULL;
					break;
				case 'H':
					flag = NULL;
					if (mode == NULL)
						holders_this_glock_df++;
					else if (*(mode + 1) == ':' &&
						 *(mode + 2) == 'E' &&
						 *(mode + 3) == 'X')
						holders_this_glock_ex++;
					else if (*(mode + 1) == ':' &&
						 *(mode + 2) == 'S' &&
						 *(mode + 3) == 'H')
						holders_this_glock_sh++;
					else
						holders_this_glock_df++;
					break;
				case ' ':
					flag = NULL;
					break;
				default:
					break;
				};
			}
			/* Detail stuff------------------------------------- */
			if (!glocks) {
				int haswaiter = is_waiter(ln);

				if (haswaiter) {
					show_prev_glock = 1;
					prev_had_waiter = 1;
				} else if (show_held && is_holder(ln) &&
					   !is_iopen(one_glocks_lines[0])) {
					show_prev_glock = 1;
				} else if (!irrelevant(ln, one_glocks_lines[0])) {
					show_prev_glock = 1;
				}
			}
		}
		/* Detail stuff--------------------------------------------- */
		strncpy(one_glocks_lines[gline], ln, 96);
		one_glocks_lines[gline][96] = '\0';
		gline++;
		if (gline >= MAX_LINES)
			break;
		if (termlines && line >= termlines)
			break;
	}
	/* Detail stuff----------------------------------------------------- */
	if (show_prev_glock && gline < MAX_LINES &&
	    (!termlines || line < termlines)) {
		show_glock(one_glocks_lines, gline, fsname, dlmwaiters,
			   dlmgrants, trace_dir_path, prev_had_waiter,
			   DETAILS, summary);
		show_glock(one_glocks_lines, gline, fsname, dlmwaiters,
			   dlmgrants, trace_dir_path, prev_had_waiter,
			   FRIENDLY, summary);
	}
	if (!summary || ((iters_done % summary) != 0))
		return;

	print_summary(total_glocks, dlmwaiters);
}

static void show_help(int help)
{
	if (help == 1) {
		COLORS_NORMAL;
		eol(0);
		print_it(NULL, "  Glock flags:                 ", NULL);
		eol(0);
		print_it(NULL, "    l - Locked                 ", NULL);
		print_it(NULL, "    r - Reply pending          ", NULL);
		eol(0);
		print_it(NULL, "    d - Demote pending         ", NULL);
		print_it(NULL, "    I - Initial                ", NULL);
		eol(0);
		print_it(NULL, "    D - Demote requested       ", NULL);
		print_it(NULL, "    F - Frozen                 ", NULL);
		eol(0);
		print_it(NULL, "    p - Demote in progress     ", NULL);
		print_it(NULL, "    q - Queued holder          ", NULL);
		eol(0);
		print_it(NULL, "    y - Dirty data             ", NULL);
		print_it(NULL, "    L - LRU                    ", NULL);
		eol(0);
		print_it(NULL, "    f - Flush                  ", NULL);
		print_it(NULL, "    o - Object present         ", NULL);
		eol(0);
		print_it(NULL, "    i - Invalidating           ", NULL);
		print_it(NULL, "    b - Blocking request       ", NULL);
		eol(0);
	} else if (help == 2) {
		COLORS_NORMAL;
		eol(0);
		print_it(NULL, "  Holder flags:                ", NULL);
		eol(0);
		print_it(NULL, "    t - Try (non-blocking)     ", NULL);
		print_it(NULL, "    E - Exact lock             ", NULL);
		eol(0);
		print_it(NULL, "    T - Try with callback      ", NULL);
		print_it(NULL, "    c - No Cache lock          ", NULL);
		eol(0);
		print_it(NULL, "    e - No exp                 ", NULL);
		print_it(NULL, "    H - Held (locked)          ", NULL);
		eol(0);
		print_it(NULL, "    A - Any lock               ", NULL);
		print_it(NULL, "    W - Waiting for lock       ", NULL);
		eol(0);
		print_it(NULL, "    p - Priority lock          ", NULL);
		print_it(NULL, "    a - Asynchronous lock      ", NULL);
		eol(0);
		print_it(NULL, "    F - First                  ", NULL);
		eol(0);
	}
}

/* flags = DETAILS || FRIENDLY or both */
static void parse_glocks_file(int fd, const char *fsname, int dlmwaiters,
			      int dlmgrants, int trace_dir_path,
			      int show_held, int help, int summary)
{
	char fstitle[96], fsdlm[105];
	char ctimestr[64];
	time_t t;
	int i;

	tzset();
	t = time(NULL);
	strftime(ctimestr, 64, "%a %b %d %T %Y", localtime(&t));
	ctimestr[63] = '\0';
	memset(fstitle, 0, sizeof(fstitle));
	memset(fsdlm, 0, sizeof(fsdlm));
	sprintf(fstitle, "@ %s       %s ", fsname, ctimestr);
	if (dlmwaiters) {
		sprintf(fsdlm, "dlm: %s/%s/%s [", dlm_dirtbl_size,
			dlm_rsbtbl_size, dlm_lkbtbl_size);
		for (i = 0; i < dlmwaiters; i++)
			strcat(fsdlm, "*");
		for (; i < 10; i++)
			strcat(fsdlm, " ");
		strcat(fsdlm, "]");
	}
	attron(A_BOLD);
	print_it(NULL, "%s @%s %s", NULL, fstitle, hostname, fsdlm);
	eol(0);
	attroff(A_BOLD);
	glock_details(fd, fsname, dlmwaiters, dlmgrants, trace_dir_path,
		      show_held, summary);

	show_help(help);
	if (termlines)
		refresh();
}

static void usage(void)
{
	printf("Usage:\n");
	printf("glocktop [-i] [-d <delay sec>] [-n <iter>] [-sX] [-c] [-D] [-H] [-r] [-t]\n");
	printf("\n");
	printf("-i : Runs glocktop in interactive mode.\n");
	printf("-d : delay between refreshes, in seconds (default: %d).\n", REFRESH_TIME);
	printf("-n : stop after <iter> refreshes.\n");
	printf("-H : don't show Held glocks, even if not waited on, excluding "
	       "iopen\n");
	printf("-r : show reservations when rgrp glocks are displayed\n");
	printf("-s : show glock summary information every X iterations\n");
	printf("-t : trace directory glocks back\n");
	printf("-D : don't show DLM lock status\n");
	printf("\n");
	fflush(stdout);
	exit(0);
}

int main(int argc, char **argv)
{
	int fd;
	DIR *dir = NULL;
	char *fn;
	struct dirent *dent;
	int retval;
	int refresh_time = REFRESH_TIME;
	fd_set readfds;
	char string[96];
	int ch, i, dlmwaiters = 0, dlmgrants = 0;
	int cont = TRUE, optchar;
	int trace_dir_path = 0;
	int show_held = 1, help = 0;
	int interactive = 0;
	int summary = 10;
	int nfds = STDIN_FILENO + 1;

	prog_name = argv[0];
	memset(glock, 0, sizeof(glock));
	memset(contended_filenames, 0, sizeof(contended_filenames));
	memset(contended_blocks, 0, sizeof(contended_blocks));
	UpdateSize(0);
	/* decode command line arguments */
	while (cont) {
		optchar = getopt(argc, argv, "-d:Dn:rs:thHi");

		switch (optchar) {
		case 'd':
			refresh_time = atoi(optarg);
			if (refresh_time < 1) {
				fprintf(stderr, "Error: delay %d too small; "
					"must be at least 1\n", refresh_time);
				exit(-1);
			}
			break;
		case 'D':
			print_dlm_grants = 0;
			break;
		case 'n':
			iterations = atoi(optarg);
			break;
		case 'r':
			show_reservations = 1;
			break;
		case 's':
			summary = atoi(optarg);
			break;
		case 't':
			trace_dir_path = 1;
			break;
		case 'h':
			usage();
			break;
		case 'H':
			show_held = 0; /* held, but not iopen held */
			break;
		case 'i':
			interactive = 1;
			break;
		case EOF:
			cont = FALSE;
			break;
		case 1:
			if  (optarg && glocks < MAX_GLOCKS)
				glock[glocks++] = optarg;
			break;

		default:
			fprintf(stderr, "unknown option: %c\n", optchar);
			exit(-1);
		};
	}

	if (interactive) {
		printf("Initializing. Please wait...");
		fflush(stdout);
	}
	if (gethostname(hostname, sizeof(hostname))) {
		fprintf(stderr, "Error: unable to determine host name.\n");
		exit(-1);
	}
	if (parse_mounts())
		exit(-1);

	if (interactive && (wind = initscr()) == NULL) {
		fprintf(stderr, "Error: unable to initialize screen.\n");
		exit(-1);
	}

	if (interactive) {
		/* Do our initial screen stuff: */
		signal(SIGWINCH, UpdateSize); /* handle term resize signal */
		UpdateSize(0); /* update screen size based on term settings */
		clear(); /* don't use Erase */
		start_color();
		noecho();
		keypad(stdscr, TRUE);
		raw();
		curs_set(0);
		init_colors();
	} else {
		termlines = 0;
	}
	while (!gbuf) {
		gbuf = malloc(bufsize);
		if (gbuf) {
			/*printf("bufsize=%dK\n", bufsize / 1024);*/
			break;
		}
		bufsize /= 2;
	}
	while (!dbuf) {
		dbuf = malloc(bufsize);
		if (dbuf) {
			/*printf("bufsize=%dK\n", bufsize / 1024);*/
			break;
		}
		bufsize /= 2;
	}

	while (!done) {
		struct timeval tv;

		if (asprintf(&fn, "%s/gfs2/", debugfs) == -1) {
			perror(argv[0]);
			exit(-1);
		}
		dir = opendir(fn);
		free(fn);

		if (!dir) {
			if (interactive) {
				refresh();
				endwin();
			}
			fprintf(stderr, "Unable to open gfs2 debugfs directory.\n");
			fprintf(stderr, "Check if debugfs and gfs2 are mounted.\n");
			exit(-1);
		}
		display_title_lines();
		while ((dent = readdir(dir))) {
			const char *fsname;
			char dlm_fn[PATH_MAX+5+8]; /* "/dlm/" and "_waiters" */
			FILE *dlmf;
			int dlmfd;

			if (!strcmp(dent->d_name, "."))
				continue;
			if (!strcmp(dent->d_name, ".."))
				continue;

			fsname = strchr(dent->d_name, ':');
			if (fsname)
				fsname++;
			else
				fsname = dent->d_name;

			memset(dlm_fn, 0, sizeof(dlm_fn));
			sprintf(dlm_fn, "%s/dlm/%s_waiters", debugfs, fsname);
			dlmf = fopen(dlm_fn, "rt");
			if (dlmf) {
				dlmwaiters = parse_dlm_waiters(dlmf, fsname);
				fclose(dlmf);
			}

			if (print_dlm_grants) {
				memset(dlm_fn, 0, sizeof(dlm_fn));
				sprintf(dlm_fn, "%s/dlm/%s_locks", debugfs,
					fsname);
				dlmfd = open(dlm_fn, O_RDONLY);
				if (dlmfd > 0) {
					dlmgrants = parse_dlm_grants(dlmfd,
								     fsname);
					close(dlmfd);
				}
			}

			if (asprintf(&fn, "%s/gfs2/%s/glocks", debugfs, dent->d_name) == -1) {
				perror(argv[0]);
				exit(-1);
			}
			fd = open(fn, O_RDONLY);
			if (fd < 0) {
				if (interactive) {
					refresh();
					endwin();
				}
				perror(fn);
				free(fn);
				exit(-1);
			}
			free(fn);
			parse_glocks_file(fd, fsname, dlmwaiters, dlmgrants,
					  trace_dir_path, show_held, help,
					  summary);
			close(fd);
		}
		closedir(dir);
		tv.tv_sec = refresh_time;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		if (nfds != 0)
			FD_SET(STDIN_FILENO, &readfds);
		retval = select(nfds, &readfds, NULL, NULL, &tv);
		if (retval) {
			if (interactive)
				ch = getch();
			else
				ch = getchar();
			switch (ch) {
			case 0x1b: /* mount wheel? */
			case 0x03:
			case 'q':
				done = 1;
				break;
			case 'h':
				help = (help + 1) % 3;
				break;
			case 's':
				if (!interactive)
					break;
				move(1, 0);
				printw("Change delay from %d to: ",
				       refresh_time);
				if (bobgets(string, 1, 25, 5, &ch) == 1)
					refresh_time = atoi(string);
				if (refresh_time < 1)
					refresh_time = 1;
				break;
			/* When we get EOF on stdin, remove it from the fd_set
			   to avoid shorting out the select() */
			case EOF:
				nfds = 0;
				break;
			}
		}
		iters_done++;
		if (iterations && iters_done >= iterations)
			break;
	}
	for (i = 0; i < mounted; i++)
		close(fs_fd[i]);
	free(gbuf);
	free(dbuf);
	if (interactive) {
		refresh();
		endwin();
	}
	exit(0);
}
