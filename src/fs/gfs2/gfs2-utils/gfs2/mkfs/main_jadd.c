#include "clusterautoconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/vfs.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)

#include <linux/types.h>
#include <linux/fiemap.h>
#include <linux/fs.h>
#include "libgfs2.h"
#include "gfs2_mkfs.h"
#include "metafs.h"

#define RANDOM(values) ((values) * (random() / (RAND_MAX + 1.0)))

struct jadd_opts {
	char *path;
	char *new_inode;
	char *per_node;
	char *jindex;
	unsigned orig_journals;
	unsigned journals;
	unsigned quiet:1;
	unsigned debug:1;
};

#define JA_FL_SET   0
#define JA_FL_CLEAR 1
static void set_flags(int fd, int op, uint32_t flags)
{
        int err;
	uint32_t val;

        err = ioctl(fd, FS_IOC_GETFLAGS, &val);
        if (err) {
		perror("GETFLAGS");
		exit(EXIT_FAILURE);
	}

        if (op == JA_FL_SET)
                val |= flags;
        else if (op == JA_FL_CLEAR)
                val &= ~flags;

        err = ioctl(fd, FS_IOC_SETFLAGS, &val);
        if (err) {
		perror("SETFLAGS");
		exit(EXIT_FAILURE);
	}
}

static int rename2system(struct jadd_opts *opts, const char *new_dir, const char *new_name)
{
	char *newpath;
	int ret;

	if (asprintf(&newpath, "%s/%s", new_dir, new_name) < 0) {
		perror(_("Failed to allocate new path"));
		return -1;
	}

	ret = rename(opts->new_inode, newpath);
	free(newpath);
	return ret;
}

static int build_paths(const char *metafs_path, struct jadd_opts *opts)
{
	int i = 0;
	struct {
		char **path;
		const char *tail;
	} p[] = {
		{ &opts->new_inode, "new_inode" },
		{ &opts->per_node, "per_node" },
		{ &opts->jindex, "jindex" },
		{ NULL, NULL}
	};

	while (p[i].path != NULL) {
		if (asprintf(p[i].path, "%s/%s", metafs_path, p[i].tail) < 0) {
			while (i > 0)
				free(*p[--i].path);
			return 1;
		}
		if (opts->debug)
			printf("%d: %s\n", i, *p[i].path);
		i++;
	}
	return 0;
}

/**
 * print_usage - print out usage information
 * @prog_name: The name of this program
 */

static void print_usage(const char *prog_name)
{
	int i;
	const char *option, *param, *desc;
	const char *options[] = {
		/* Translators: This is a usage string printed with --help.
		   <size> and <number> here are the commandline parameters,
		   e.g. gfs2_jadd -j <number> /dev/sda */
		"-c", "<size>",   _("Size of quota change file, in megabytes"),
		"-D", NULL,       _("Enable debugging code"),
		"-h", NULL,       _("Display this help, then exit"),
		"-J", "<size>",   _("Size of journals, in megabytes"),
		"-j", "<number>", _("Number of journals"),
		"-q", NULL,       _("Don't print anything"),
		"-V", NULL,       _("Display version information, then exit"),
		NULL, NULL, NULL /* Must be kept at the end */
	};

	printf("%s\n", _("Usage:"));
	printf("%s [%s] <%s>\n\n", prog_name, _("options"), _("device"));
	printf(_("Adds journals to a GFS2 file system."));
	printf("\n\n%s\n", _("Options:"));

	for (i = 0; options[i] != NULL; i += 3) {
		option = options[i];
		param = options[i+1];
		desc = options[i+2];
		printf("%3s %-15s %s\n", option, param ? param : "", desc);
	}
}

/**
 * decode_arguments - decode command line arguments and fill in the struct gfs2_sbd
 * @argc:
 * @argv:
 * @sdp: the decoded command line arguments
 *
 */

static void decode_arguments(int argc, char *argv[], struct gfs2_sbd *sdp, struct jadd_opts *opts)
{
	int cont = TRUE;
	int optchar;

	while (cont) {
		optchar = getopt(argc, argv, "c:DhJ:j:qu:V");

		switch (optchar) {
		case 'c':
			sdp->qcsize = atoi(optarg);
			break;
		case 'D':
			opts->debug = 1;
			lgfs2_set_debug(1);
			break;
		case 'h':
			print_usage(argv[0]);
			exit(0);
			break;
		case 'J':
			sdp->jsize = atoi(optarg);
			break;
		case 'j':
			opts->journals = atoi(optarg);
			break;
		case 'q':
			opts->quiet = 1;
			break;
		case 'V':
			printf("gfs2_jadd %s (built %s %s)\n", VERSION,
			       __DATE__, __TIME__);
			printf(REDHAT_COPYRIGHT "\n");
			exit(0);
			break;
		case ':':
		case '?':
			fprintf(stderr, _("Please use '-h' for help.\n"));
			exit(EXIT_FAILURE);
			break;
		case EOF:
			cont = FALSE;
			break;
		default:
			die( _("Invalid option: %c\n"), optchar);
			break;
		};
	}

	if (optind < argc) {
		opts->path = argv[optind];
		optind++;
	} else
		die( _("no path specified (try -h for help)\n"));

	if (optind < argc)
		die( _("Unrecognized argument: %s\n"), argv[optind]);

	if (opts->debug) {
		printf( _("Command line arguments:\n"));
		printf("  qcsize = %u\n", sdp->qcsize);
		printf("  jsize = %u\n", sdp->jsize);
		printf("  journals = %u\n", sdp->md.journals);
		printf("  quiet = %u\n", opts->quiet);
		printf("  path = %s\n", opts->path);
	}
}

static void verify_arguments(struct gfs2_sbd *sdp, struct jadd_opts *opts)
{
	if (!opts->journals)
		die( _("no journals specified\n"));
	if (sdp->jsize < 32 || sdp->jsize > 1024)
		die( _("bad journal size\n"));
	if (!sdp->qcsize || sdp->qcsize > 64)
		die( _("bad quota change size\n"));
}

static void print_results(struct jadd_opts *opts)
{
	if (opts->debug)
		printf("\n");
	else if (opts->quiet)
		return;

	printf( _("Filesystem: %s\n"), opts->path);
	printf( _("Old journals: %u\n"), opts->orig_journals);
	printf( _("New journals: %u\n"), opts->journals);
}

static int create_new_inode(struct jadd_opts *opts, uint64_t *addr)
{
	char *name = opts->new_inode;
	int fd;
	int error;

	for (;;) {
		fd = open(name, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
		if (fd >= 0)
			break;
		if (errno == EEXIST) {
			error = unlink(name);
			if (error){
				perror("unlink");
				exit(EXIT_FAILURE);
			}
		} else{
			perror("create");
			exit(EXIT_FAILURE);
		}
	}
	if (addr != NULL) {
		struct stat st;

		fstat(fd, &st);
		*addr = st.st_ino;
	}

	return fd;
}

static void add_ir(struct jadd_opts *opts)
{
	int fd;
	char new_name[256];
	int error;

	fd = create_new_inode(opts, NULL);

	{
		struct gfs2_inum_range ir;

		set_flags(fd, JA_FL_SET, FS_JOURNAL_DATA_FL);
		memset(&ir, 0, sizeof(struct gfs2_inum_range));
		if (write(fd, (void*)&ir, sizeof(struct gfs2_inum_range)) !=
		    sizeof(struct gfs2_inum_range)) {
			perror("add_ir");
			exit(EXIT_FAILURE);
		}
	}

	close(fd);

	sprintf(new_name, "inum_range%u", opts->journals);
	error = rename2system(opts, opts->per_node, new_name);
	if (error < 0 && errno != EEXIST){
		perror("add_ir rename2system");
		exit(EXIT_FAILURE);
	}
}

static void add_sc(struct jadd_opts *opts)
{
	int fd;
	char new_name[256];
	int error;

	fd = create_new_inode(opts, NULL);

	{
		struct gfs2_statfs_change sc;
		set_flags(fd, JA_FL_SET, FS_JOURNAL_DATA_FL);

		memset(&sc, 0, sizeof(struct gfs2_statfs_change));
		if (write(fd, (void*)&sc, sizeof(struct gfs2_statfs_change)) !=
		    sizeof(struct gfs2_statfs_change)) {
			perror("add_sc");
			exit(EXIT_FAILURE);
		}
	}

	close(fd);

	sprintf(new_name, "statfs_change%u", opts->journals);
	error = rename2system(opts, opts->per_node, new_name);
	if (error < 0 && errno != EEXIST){
		perror("add_sc rename2system");
		exit(EXIT_FAILURE);
	}
}

static void add_qc(struct gfs2_sbd *sdp, struct jadd_opts *opts)
{
	int fd;
	char new_name[256];
	int error;

	fd = create_new_inode(opts, NULL);

	{
		char buf[sdp->bsize];
		unsigned int blocks =
			sdp->qcsize << (20 - sdp->sd_sb.sb_bsize_shift);
		unsigned int x;
		struct gfs2_meta_header mh;

		set_flags(fd, JA_FL_CLEAR, FS_JOURNAL_DATA_FL);
		memset(buf, 0, sdp->bsize);

		for (x=0; x<blocks; x++) {
			if (write(fd, buf, sdp->bsize) != sdp->bsize) {
				perror("add_qc");
				exit(EXIT_FAILURE);
			}
		}

		lseek(fd, 0, SEEK_SET);

		memset(&mh, 0, sizeof(struct gfs2_meta_header));
		mh.mh_magic = GFS2_MAGIC;
		mh.mh_type = GFS2_METATYPE_QC;
		mh.mh_format = GFS2_FORMAT_QC;
		gfs2_meta_header_out(&mh, buf);

		for (x=0; x<blocks; x++) {
			if (write(fd, buf, sdp->bsize) != sdp->bsize) {
				perror("add_qc");
				exit(EXIT_FAILURE);
			}
		}

		error = fsync(fd);
		if (error){
			perror("add_qc fsync");
			exit(EXIT_FAILURE);
		}
	}

	close(fd);

	sprintf(new_name, "quota_change%u", opts->journals);
	error = rename2system(opts, opts->per_node, new_name);
	if (error < 0 && errno != EEXIST){
		perror("add_qc rename2system");
		exit(EXIT_FAILURE);
	}
}

static void gather_info(struct gfs2_sbd *sdp, struct jadd_opts *opts)
{
	struct statfs statbuf;
	if (statfs(opts->path, &statbuf) < 0) {
		perror(opts->path);
		exit(EXIT_FAILURE);
	}
	sdp->bsize = statbuf.f_bsize;
}

static void find_current_journals(struct jadd_opts *opts)
{
	struct dirent *dp;
	DIR *dirp;
	unsigned existing_journals = 0;

	dirp = opendir(opts->jindex);
	if (!dirp) {
		perror("jindex");
		exit(EXIT_FAILURE);
	}
	while (dirp) {
		if ((dp = readdir(dirp)) != NULL) {
			if (strncmp(dp->d_name, "journal", 7) == 0)
				existing_journals++;
		} else
			goto close;
	}
close:
	closedir(dirp);
	if (existing_journals == 0) {
		die( _("No journals found. Did you run mkfs.gfs2 correctly?\n"));
	}

	opts->orig_journals = existing_journals;
}

#ifdef GFS2_HAS_LH_V2
static uint64_t find_block_address(int fd, off_t offset, unsigned bsize)
{
	struct {
		struct fiemap fm;
		struct fiemap_extent fe;
	} fme;
	int ret;

	fme.fm.fm_start = offset;
	fme.fm.fm_length = 1;
	fme.fm.fm_flags = FIEMAP_FLAG_SYNC;
	fme.fm.fm_extent_count = 1;

	ret = ioctl(fd, FS_IOC_FIEMAP, &fme.fm);
	if (ret != 0 || fme.fm.fm_mapped_extents != 1) {
		fprintf(stderr, "Failed to find log header block address\n");
		return 0;
	}
	return fme.fe.fe_physical / bsize;
}
#endif

static void add_j(struct gfs2_sbd *sdp, struct jadd_opts *opts)
{
	int fd;
	char new_name[256];
	int error;
	uint64_t addr;

	fd = create_new_inode(opts, &addr);

	{
		char buf[sdp->bsize];
		unsigned int blocks =
			sdp->jsize << (20 - sdp->sd_sb.sb_bsize_shift);
		unsigned int x;
		struct gfs2_log_header lh;
		uint64_t seq = RANDOM(blocks);
		off_t off = 0;

		set_flags(fd, JA_FL_CLEAR, FS_JOURNAL_DATA_FL);
		memset(buf, 0, sdp->bsize);
		for (x=0; x<blocks; x++) {
			if (write(fd, buf, sdp->bsize) != sdp->bsize) {
				perror("add_j");
				exit(EXIT_FAILURE);
			}
		}

		lseek(fd, 0, SEEK_SET);

		memset(&lh, 0, sizeof(struct gfs2_log_header));
		lh.lh_header.mh_magic = GFS2_MAGIC;
		lh.lh_header.mh_type = GFS2_METATYPE_LH;
		lh.lh_header.mh_format = GFS2_FORMAT_LH;
		lh.lh_flags = GFS2_LOG_HEAD_UNMOUNT;
#ifdef GFS2_HAS_LH_V2
		lh.lh_flags |= GFS2_LOG_HEAD_USERSPACE;
		lh.lh_jinode = addr;
#endif
		for (x=0; x<blocks; x++) {
			uint32_t hash;

			lh.lh_sequence = seq;
			lh.lh_blkno = x;
			gfs2_log_header_out(&lh, buf);
			hash = lgfs2_log_header_hash(buf);
			((struct gfs2_log_header *)buf)->lh_hash = cpu_to_be32(hash);
#ifdef GFS2_HAS_LH_V2
			((struct gfs2_log_header *)buf)->lh_addr = cpu_to_be64(
			                           find_block_address(fd, off, sdp->bsize));
			hash = lgfs2_log_header_crc(buf, sdp->bsize);
			((struct gfs2_log_header *)buf)->lh_crc = cpu_to_be32(hash);
#endif
			if (write(fd, buf, sdp->bsize) != sdp->bsize) {
				perror("add_j");
				exit(EXIT_FAILURE);
			}

			if (++seq == blocks)
				seq = 0;
			off += sdp->bsize;
		}

		error = fsync(fd);
		if (error){
			perror("add_j fsync");
			exit(EXIT_FAILURE);
		}
	}

	close(fd);

	sprintf(new_name, "journal%u", opts->journals);
	error = rename2system(opts, opts->jindex, new_name);
	if (error < 0 && errno != EEXIST){
		perror("add_j rename2system");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	struct jadd_opts opts = {0};
	struct gfs2_sbd sbd, *sdp = &sbd;
	struct metafs mfs = {0};
	struct mntent *mnt;
	unsigned int total;

	setlocale(LC_ALL, "");
	textdomain("gfs2-utils");
	srandom(time(NULL) ^ getpid());

	memset(sdp, 0, sizeof(struct gfs2_sbd));
	sdp->jsize = GFS2_DEFAULT_JSIZE;
	sdp->qcsize = GFS2_DEFAULT_QCSIZE;
	opts.journals = 1;

	decode_arguments(argc, argv, sdp, &opts);
	verify_arguments(sdp, &opts);

	sbd.path_fd = lgfs2_open_mnt_dir(opts.path, O_RDONLY|O_CLOEXEC, &mnt);
	if (sbd.path_fd < 0) {
		fprintf(stderr, _("Error looking up mount '%s': %s\n"), opts.path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (mnt == NULL) {
		fprintf(stderr, _("%s: not a mounted gfs2 file system\n"), opts.path);
		exit(EXIT_FAILURE);
	}
	gather_info(sdp, &opts);
	mfs.context = copy_context_opt(mnt);
	if (mount_gfs2_meta(&mfs, mnt->mnt_dir, opts.debug)) {
		perror("GFS2 metafs");
		exit(EXIT_FAILURE);
	}

	if (build_paths(mfs.path, &opts)) {
		perror(_("Failed to build paths"));
		exit(EXIT_FAILURE);
	}

	if (compute_constants(sdp)) {
		perror(_("Failed to compute file system constants"));
		exit(EXIT_FAILURE);
	}
	find_current_journals(&opts);

	total = opts.orig_journals + opts.journals;
	for (opts.journals = opts.orig_journals;
	     opts.journals < total;
	     opts.journals++) {
		if (metafs_interrupted) {
			cleanup_metafs(&mfs);
			exit(130);
		}
		add_ir(&opts);
		add_sc(&opts);
		add_qc(sdp, &opts);
		add_j(sdp, &opts);
	}

	free(opts.new_inode);
	free(opts.per_node);
	free(opts.jindex);
	close(sdp->path_fd);
	cleanup_metafs(&mfs);
	sync();
	print_results(&opts);

	return 0;
}
