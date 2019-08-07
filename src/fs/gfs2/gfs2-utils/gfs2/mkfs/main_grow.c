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
#include <sys/mount.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <linux/types.h>
#include <blkid.h>
#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)

#include <logging.h>
#include "libgfs2.h"
#include "gfs2_mkfs.h"
#include "metafs.h"

#define BUF_SIZE 4096
#define MB (1024 * 1024)

static uint64_t override_device_size = 0;
static int test = 0;
static uint64_t fssize = 0, fsgrowth;
int print_level = MSG_NOTICE;

extern int create_new_inode(struct gfs2_sbd *sdp);
extern int rename2system(struct gfs2_sbd *sdp, char *new_dir, char *new_name);

#ifndef FALLOC_FL_KEEP_SIZE
#define FALLOC_FL_KEEP_SIZE 0x01
#endif
#ifndef BLKDISCARD
#define BLKDISCARD      _IO(0x12,119)
#endif

static int discard_blocks(int fd, uint64_t start, uint64_t len)
{
	__uint64_t range[2] = { start, len };

	if (ioctl(fd, BLKDISCARD, &range) < 0)
		return errno;
	return 0;
}

/**
 * usage - Print out the usage message
 *
 * This function does not include documentation for the -D option
 * since normal users have no use for it at all. The -D option is
 * only for developers. It intended use is in combination with the
 * -T flag to find out what the result would be of trying different
 * device sizes without actually having to try them manually.
 */

static void usage(void)
{
	int i;
	const char *option, *param, *desc;
	const char *options[] = {
		"-h", NULL, _("Display this usage information"),
		"-q", NULL, _("Quiet, reduce verbosity"),
		"-T", NULL, _("Do everything except update file system"),
		"-V", NULL, _("Display version information"),
		"-v", NULL, _("Increase verbosity"),
		NULL, NULL, NULL /* Must be kept at the end */
	};

	printf("%s\n", _("Usage:"));
	printf("    gfs2_grow [%s] <%s>\n\n", _("options"), _("device"));
	printf(_("Expands a GFS2 file system after the device containing the file system has been expanded"));
	printf("\n\n%s\n", _("Options:"));

	for (i = 0; options[i] != NULL; i += 3) {
		option = options[i];
		param = options[i+1];
		desc = options[i+2];
		printf("%3s %-15s %s\n", option, param ? param : "", desc);
	}
}

static void decode_arguments(int argc, char *argv[], struct gfs2_sbd *sdp)
{
	int opt;

	while ((opt = getopt(argc, argv, "VD:hqTv?")) != EOF) {
		switch (opt) {
		case 'D':	/* This option is for testing only */
			override_device_size = atoi(optarg);
			override_device_size <<= 20;
			break;
		case 'V':
			printf(_("%s %s (built %s %s)\n"), argv[0],
			       VERSION, __DATE__, __TIME__);
			printf(REDHAT_COPYRIGHT "\n");
			exit(0);
		case 'h':
			usage();
			exit(0);
		case 'q':
			decrease_verbosity();
			break;
		case 'T':
			printf( _("(Test mode - file system will not "
			       "be changed)\n"));
			test = 1;
			break;
		case 'v':
			increase_verbosity();
			break;
		case ':':
		case '?':
			/* Unknown flag */
			fprintf(stderr, _("Please use '-h' for help.\n"));
			exit(EXIT_FAILURE);
		default:
			fprintf(stderr, _("Invalid option '%c'\n"), opt);
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (optind == argc) {
		usage();
		exit(EXIT_FAILURE);
	}
}

static lgfs2_rgrps_t rgrps_init(struct gfs2_sbd *sdp)
{
	int ret;
	int error;
	uint64_t al_base = 0;
	uint64_t al_off = 0;
	struct stat st;
	blkid_probe pr = blkid_new_probe();
	if (pr == NULL || blkid_probe_set_device(pr, sdp->device_fd, 0, 0) != 0
	               || blkid_probe_enable_superblocks(pr, TRUE) != 0
	               || blkid_probe_enable_partitions(pr, TRUE) != 0) {
		fprintf(stderr, _("Failed to create probe\n"));
		return NULL;
	}

	error = fstat(sdp->device_fd, &st);
	if (error < 0) {
		fprintf(stderr, _("fstat failed\n"));
		return NULL;
	}

	if (!S_ISREG(st.st_mode) && blkid_probe_enable_topology(pr, TRUE) != 0) {
		fprintf(stderr, _("Failed to create probe\n"));
		return NULL;
	}

	ret = blkid_do_fullprobe(pr);
	if (ret == 0 && !S_ISREG(st.st_mode)) {
		blkid_topology tp = blkid_probe_get_topology(pr);
		if (tp != NULL) {
			unsigned long min_io_sz = blkid_topology_get_minimum_io_size(tp);
			unsigned long opt_io_sz = blkid_topology_get_optimal_io_size(tp);
			unsigned long phy_sector_sz = blkid_topology_get_physical_sector_size(tp);
			/* If optimal_io_size is not a multiple of minimum_io_size then
			   the values are not reliable swidth and sunit values, so don't
			   attempt rgrp alignment */
			if ((min_io_sz > phy_sector_sz) &&
			    (opt_io_sz > phy_sector_sz) &&
			    (opt_io_sz % min_io_sz == 0)) {
					al_base = opt_io_sz / sdp->bsize;
					al_off = min_io_sz / sdp->bsize;
			}

		}
	}

	blkid_free_probe(pr);
	return lgfs2_rgrps_init(sdp, al_base, al_off);
}

/**
 * Calculate the size of the filesystem
 * Reads the lists of resource groups in order to work out where the last block
 * of the filesystem is located.
 * Returns: The calculated size
 */
static uint64_t filesystem_size(lgfs2_rgrps_t rgs)
{
	lgfs2_rgrp_t rg = lgfs2_rgrp_last(rgs);
	const struct gfs2_rindex *ri = lgfs2_rgrp_index(rg);
	return ri->ri_data0 + ri->ri_data;
}

/**
 * Write the new rg information to disk.
 */
static unsigned initialize_new_portion(struct gfs2_sbd *sdp, lgfs2_rgrps_t rgs)
{
	unsigned rgcount = 0;
	uint64_t rgaddr = fssize;

	discard_blocks(sdp->device_fd, rgaddr * sdp->bsize, fsgrowth * sdp->bsize);
	/* Build the remaining resource groups */
	while (1) {
		int err = 0;
		lgfs2_rgrp_t rg;
		struct gfs2_rindex ri;
		uint64_t nextaddr;

		nextaddr = lgfs2_rindex_entry_new(rgs, &ri, rgaddr, 0);
		if (nextaddr == 0)
			break;
		rg = lgfs2_rgrps_append(rgs, &ri, nextaddr - rgaddr);
		if (rg == NULL) {
			perror(_("Failed to create resource group"));
			return 0;
		}
		rgaddr = nextaddr;
		if (metafs_interrupted)
			return 0;
		if (!test)
			err = lgfs2_rgrp_write(sdp->device_fd, rg);
		if (err != 0) {
			perror(_("Failed to write resource group"));
			return 0;
		}
		rgcount++;
	}
	fsync(sdp->device_fd);
	return rgcount;
}

static char *rindex_buffer(lgfs2_rgrps_t rgs, unsigned count)
{
	lgfs2_rgrp_t rg;
	unsigned i = 0;
	char *buf;

	buf = calloc(count, sizeof(struct gfs2_rindex));
	if (buf == NULL) {
		perror(__FUNCTION__);
		exit(EXIT_FAILURE);
	}
	for (rg = lgfs2_rgrp_first(rgs); rg; rg = lgfs2_rgrp_next(rg)) {
		const struct gfs2_rindex *ri = lgfs2_rgrp_index(rg);
		gfs2_rindex_out(ri, buf + (sizeof(*ri) * i));
		i++;
	}
	return buf;
}

/**
 * fix_rindex - Add the new entries to the end of the rindex file.
 */
static void fix_rindex(int rindex_fd, lgfs2_rgrps_t rgs, unsigned old_rg_count, unsigned rgcount)
{
	char *buf;
	ssize_t count;
	ssize_t writelen;
	off_t rindex_size;
	const size_t entrysize = sizeof(struct gfs2_rindex);

	log_info( _("%d new rindex entries.\n"), rgcount);
	buf = rindex_buffer(rgs, rgcount);
	writelen = rgcount * entrysize;

	if (test)
		goto out;

	rindex_size = lseek(rindex_fd, 0, SEEK_END);
	if (rindex_size != old_rg_count * entrysize) {
		log_crit(_("Incorrect rindex size. Want %ld (%d resource groups), have %ld\n"),
			 (long)(old_rg_count * entrysize), old_rg_count,
			 (long)rindex_size);
		goto out;
	}
	/* Write the first entry separately to ensure there's enough
	   space in the fs for the rest  */
	count = write(rindex_fd, buf, entrysize);
	if (count != entrysize) {
		log_crit(_("Error writing first new rindex entry; aborted.\n"));
		if (count > 0)
			goto trunc;
		else
			goto out;
	}
	count = write(rindex_fd, (buf + entrysize), (writelen - entrysize));
	if (count != (writelen - entrysize)) {
		log_crit(_("Error writing new rindex entries; aborted.\n"));
		if (count > 0)
			goto trunc;
		else
			goto out;
	}
	if (fallocate(rindex_fd, FALLOC_FL_KEEP_SIZE, (rindex_size + writelen), entrysize) != 0)
		perror("fallocate");
	fsync(rindex_fd);
out:
	free(buf);
	return;
trunc:
	count = (count / sizeof(struct gfs2_rindex)) + old_rg_count;
	log_crit(_("truncating rindex to %ld entries\n"),
		 (long)count * sizeof(struct gfs2_rindex));
	if (ftruncate(rindex_fd, (off_t)count * sizeof(struct gfs2_rindex)))
		log_crit(_("Could not truncate rindex: %s\n"), strerror(errno));
	free(buf);
}

/**
 * print_info - Print out various bits of (interesting?) information
 */
static void print_info(struct gfs2_sbd *sdp, char *device, char *mnt_path)
{
	log_notice("FS: %-25s%s\n", _("Mount point:"), mnt_path);
	log_notice("FS: %-25s%s\n", _("Device:"), device);
	log_notice("FS: %-25s%llu (0x%llx)\n", _("Size:"),
		   (unsigned long long)fssize, (unsigned long long)fssize);
	log_notice("DEV: %-24s%llu (0x%llx)\n", _("Length:"),
		   (unsigned long long)sdp->device.length,
		   (unsigned long long)sdp->device.length);
	log_notice(_("The file system will grow by %lluMB.\n"),
		   (unsigned long long)(fsgrowth * sdp->bsize) / MB);
}

static int open_rindex(char *metafs_path, int mode)
{
	char *path;
	int fd;

	if (asprintf(&path, "%s/rindex", metafs_path) < 0) {
		perror(_("Failed to open rindex"));
		return -1;
	}
	fd = open(path, (mode | O_CLOEXEC));
	if (fd < 0) {
		perror(path);
		fprintf(stderr, _("Please run fsck.gfs2\n"));
	}
	free(path);
	return fd;
}

int main(int argc, char *argv[])
{
	struct gfs2_sbd sbd, *sdp = &sbd;
	int rindex_fd;
	int error = EXIT_SUCCESS;
	int devflags = (test ? O_RDONLY : O_RDWR) | O_CLOEXEC;

	setlocale(LC_ALL, "");
	textdomain("gfs2-utils");
	srandom(time(NULL) ^ getpid());

	memset(sdp, 0, sizeof(struct gfs2_sbd));
	sdp->bsize = GFS2_DEFAULT_BSIZE;
	sdp->rgsize = -1;
	sdp->jsize = GFS2_DEFAULT_JSIZE;
	sdp->qcsize = GFS2_DEFAULT_QCSIZE;
	sdp->md.journals = 1;
	decode_arguments(argc, argv, sdp);

	for(; (argc - optind) > 0; optind++) {
		struct metafs mfs = {0};
		struct mntent *mnt;
		unsigned rgcount;
		unsigned old_rg_count;
		lgfs2_rgrps_t rgs;

		error = lgfs2_open_mnt(argv[optind], O_RDONLY|O_CLOEXEC, &sdp->path_fd,
		                                       devflags, &sdp->device_fd, &mnt);
		if (error != 0) {
			fprintf(stderr, _("Error looking up mount '%s': %s\n"), argv[optind], strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (mnt == NULL) {
			fprintf(stderr, _("%s: not a mounted gfs2 file system\n"), argv[optind]);
			continue;
		}

		if (lgfs2_get_dev_info(sdp->device_fd, &sdp->dinfo) < 0) {
			perror(mnt->mnt_fsname);
			exit(EXIT_FAILURE);
		}
		sdp->sd_sb.sb_bsize = GFS2_DEFAULT_BSIZE;
		sdp->bsize = sdp->sd_sb.sb_bsize;
		if (compute_constants(sdp)) {
			log_crit("%s\n", _("Failed to compute file system constants"));
			exit(EXIT_FAILURE);
		}
		if (read_sb(sdp) < 0) {
			fprintf(stderr, _("Error reading superblock.\n"));
			exit(EXIT_FAILURE);
		}
		if (sdp->gfs1) {
			fprintf(stderr, _("cannot grow gfs1 filesystem\n"));
			exit(EXIT_FAILURE);
		}
		fix_device_geometry(sdp);
		mfs.context = copy_context_opt(mnt);
		if (mount_gfs2_meta(&mfs, mnt->mnt_dir, (print_level > MSG_NOTICE))) {
			perror(_("Failed to mount GFS2 meta file system"));
			exit(EXIT_FAILURE);
		}
		rindex_fd = open_rindex(mfs.path, (test ? O_RDONLY : O_RDWR));
		if (rindex_fd < 0) {
			cleanup_metafs(&mfs);
			exit(EXIT_FAILURE);
		}
		/* Get master dinode */
		sdp->master_dir = lgfs2_inode_read(sdp, sdp->sd_sb.sb_master_dir.no_addr);
		if (sdp->master_dir == NULL) {
			perror(_("Could not read master directory"));
			exit(EXIT_FAILURE);
		}
		rgs = rgrps_init(sdp);
		if (rgs == NULL) {
			perror(_("Could not initialise resource groups"));
			error = -1;
			goto out;
		}
		/* Fetch the rindex from disk.  We aren't using gfs2 here,  */
		/* which means that the bitmaps will most likely be cached  */
		/* and therefore out of date.  It shouldn't matter because  */
		/* we're only going to write out new RG information after   */
		/* the existing RGs, and only write to the index at EOF.    */
		log_info(_("Gathering resource group information for %s\n"), argv[optind]);
		old_rg_count = lgfs2_rindex_read_fd(rindex_fd, rgs);
		if (old_rg_count == 0) {
			perror(_("Failed to scan existing resource groups"));
			error = -EXIT_FAILURE;
			goto out;
		}
		if (metafs_interrupted)
			goto out;
		fssize = lgfs2_rgrp_align_addr(rgs, filesystem_size(rgs) + 1);
		/* We're done with the old rgs now that we have the fssize and rg count */
		lgfs2_rgrps_free(&rgs);
		/* Now lets set up the new ones with alignment and all */
		rgs = rgrps_init(sdp);
		if (rgs == NULL) {
			perror(_("Could not initialise new resource groups"));
			error = -1;
			goto out;
		}
		fsgrowth = (sdp->device.length - fssize);
		rgcount = lgfs2_rgrps_plan(rgs, fsgrowth, ((GFS2_MAX_RGSIZE << 20) / sdp->bsize));
		if (rgcount == 0) {
			log_err( _("The calculated resource group size is too small.\n"));
			log_err( _("%s has not grown.\n"), argv[optind]);
			error = -1;
			goto out;
		}
		print_info(sdp, mnt->mnt_fsname, mnt->mnt_dir);
		rgcount = initialize_new_portion(sdp, rgs);
		if (rgcount == 0 || metafs_interrupted)
			goto out;
		fsync(sdp->device_fd);
		fix_rindex(rindex_fd, rgs, old_rg_count, rgcount);
	out:
		lgfs2_rgrps_free(&rgs);
		close(rindex_fd);
		cleanup_metafs(&mfs);
		close(sdp->device_fd);

		if (metafs_interrupted)
			break;
	}
	close(sdp->path_fd);
	sync();
	if (metafs_interrupted) {
		log_notice( _("gfs2_grow interrupted.\n"));
		exit(1);
	}
	log_notice( _("gfs2_grow complete.\n"));
	return error;
}
