#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <libgfs2.h>

const char *prog_name = "nukerg";

static void usage(void)
{
	printf("%s zeroes a list of gfs2 resource groups or rindex entries.\n", prog_name);
	printf("\n");
	printf("Usage:\n");
	printf("    %s -r <num_list>|-i <num_list> /dev/your/device\n", prog_name);
	printf("\n");
	printf("      -r: Destroy resource groups\n");
	printf("      -i: Destroy resource group index (rindex) entries\n");
	printf("\n");
	printf("num_list: A comma- or space-separated list of resource group or rindex\n");
	printf("          numbers (0-indexed) to be destroyed. Use '*' for all.\n");
	printf("\n");
	printf("Both -r and -i can be specified. Resource groups will necessarily\n");
	printf("be destroyed before rindex entries.\n");
}

#define RGNUMS_SIZE (256)

struct opts {
	const char *device;
	unsigned rgnums[RGNUMS_SIZE];
	size_t rgnum_count;
	unsigned rinums[RGNUMS_SIZE];
	size_t rinum_count;

	unsigned got_help:1;
	unsigned got_device:1;
	unsigned got_rgnums:1;
	unsigned got_rinums:1;
};

static int parse_uint(char *str, unsigned *uint)
{
	long long tmpll;
	char *endptr;

	if (str == NULL || *str == '\0')
		return 1;

	errno = 0;
	tmpll = strtoll(str, &endptr, 10);
	if (errno || tmpll < 0 || tmpll > UINT_MAX || *endptr != '\0')
		return 1;

	*uint = (unsigned)tmpll;
	return 0;
}

#define ALL_RGS ((unsigned)-1)

static size_t parse_uint_list(char *str, unsigned *list, char **tokp, size_t nleft)
{
	const char *delim = " ,";
	size_t i;

	for (i = 0, *tokp = strsep(&str, delim); *tokp != NULL;
	       i++, *tokp = strsep(&str, delim))
	{
		int ret;

		if (i >= nleft)
			return (size_t)-1; /* List would overflow */

		/* Allow * to denote "all" */
		if (strcmp(*tokp, "*") == 0) {
			list[0] = ALL_RGS;
			return 1;
		}
		ret = parse_uint(*tokp, &list[i]);
		if (ret != 0)
			return 0; /* Invalid token, *tokp points to it */
	}
	return i; /* Will be 0 if str is NULL or empty (*tokp is NULL in that case) */
}

static int parse_ri_list(char *str, struct opts *opts)
{
	unsigned *rinums = opts->rinums + opts->rinum_count;
	size_t nleft = RGNUMS_SIZE - opts->rinum_count;
	char *errtok = NULL;
	size_t n;

	n = parse_uint_list(str, rinums, &errtok, nleft);
	if (n == 0) {
		if (errtok == NULL)
			fprintf(stderr, "No rindex entries given\n");
		else
			fprintf(stderr, "Invalid rindex entry: '%s'\n", errtok);
		return 1;
	}
	else if (n == (size_t)-1) {
		fprintf(stderr, "Too many rindex entries\n");
		return 1;
	}
	opts->rinum_count += n;
	opts->got_rinums = 1;
	return 0;
}

static int parse_rg_list(char *str, struct opts *opts)
{
	unsigned *rgnums = opts->rgnums + opts->rgnum_count;
	size_t nleft = RGNUMS_SIZE - opts->rgnum_count;
	char *errtok = NULL;
	size_t n;

	n = parse_uint_list(optarg, rgnums, &errtok, nleft);
	if (n == 0) {
		if (errtok == NULL)
			fprintf(stderr, "No resource groups given\n");
		else
			fprintf(stderr, "Invalid resource group number: '%s'\n", errtok);
		return 1;
	} else if (n == (size_t)-1) {
		fprintf(stderr, "Too many resource group numbers\n");
		return 1;
	}
	opts->rgnum_count += n;
	opts->got_rgnums = 1;
	return 0;
}

static int opts_get(int argc, char *argv[], struct opts *opts)
{
	int c;

	memset(opts, 0, sizeof(*opts));

	while (1) {
		c = getopt(argc, argv, "-hi:r:");
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			opts->got_help = 1;
			usage();
			return 0;
		case 'i':
			if (parse_ri_list(optarg, opts))
				return 1;
			break;
		case 'r':
			if (parse_rg_list(optarg, opts))
				return 1;
			break;
		case 1:
			if (opts->got_device) {
				fprintf(stderr, "More than one device specified. ");
				fprintf(stderr, "Try -h for help.\n");
				return 1;
			}
			opts->device = optarg;
			opts->got_device = 1;
			break;
		case '?':
		default:
			usage();
			return 1;
		}
	}
	return 0;
}

static int nuke_rgs(struct gfs2_sbd *sdp, lgfs2_rgrps_t rgs, unsigned *rgnums, size_t count)
{
	struct gfs2_rgrp blankrg;
	lgfs2_rgrp_t rg;
	unsigned i;

	memset(&blankrg, 0, sizeof(blankrg));

	for (rg = lgfs2_rgrp_first(rgs), i = 0; rg; rg = lgfs2_rgrp_next(rg), i++) {
		const struct gfs2_rindex *ri = lgfs2_rgrp_index(rg);
		unsigned j;

		for (j = 0; j < count; j++) {
			uint64_t addr = ri->ri_addr;
			off_t off = addr * sdp->bsize;
			ssize_t bytes;

			if (i != rgnums[j] && rgnums[j] != ALL_RGS)
				continue;

			printf("Nuking rg #%u at block 0x%"PRIx64"\n", i, addr);

			bytes = pwrite(sdp->device_fd, &blankrg, sizeof(blankrg), off);
			if (bytes != sizeof(blankrg)) {
				fprintf(stderr, "Write failed (%u bytes): %s",
				             (unsigned)bytes, strerror(errno));
				return 1;
			}
		}
	}
	return 0;
}

static int nuke_ris(struct gfs2_sbd *sdp, lgfs2_rgrps_t rgs, unsigned *rinums, size_t count)
{
	struct gfs2_rindex blankri;
	lgfs2_rgrp_t rg;
	unsigned i;

	memset(&blankri, 0, sizeof(blankri));

	for (rg = lgfs2_rgrp_first(rgs), i = 0; rg; rg = lgfs2_rgrp_next(rg), i++) {
		unsigned j;

		for (j = 0; j < count; j++) {
			int bytes = 0;
			uint64_t off;

			if (i != rinums[j] && rinums[j] != ALL_RGS)
				continue;

			printf("Nuking rindex entry %u.\n", i);

			off = i * sizeof(struct gfs2_rindex);
			bytes = gfs2_writei(sdp->md.riinode, &blankri, off,
						sizeof(struct gfs2_rindex));
			if (bytes != sizeof(struct gfs2_rindex)) {
				fprintf(stderr, "Write failed (%d bytes): %s",
						       bytes, strerror(errno));
				return 1;
			}
		}
	}
	return 0;
}

static lgfs2_rgrps_t read_rindex(struct gfs2_sbd *sdp)
{
	lgfs2_rgrps_t rgs;
	unsigned rgcount;
	unsigned i;

	gfs2_lookupi(sdp->master_dir, "rindex", 6, &sdp->md.riinode);
	if (sdp->md.riinode == NULL) {
		perror("Failed to look up rindex");
		return NULL;
	}
	rgs = lgfs2_rgrps_init(sdp, 0, 0);
	if (rgs == NULL) {
		fprintf(stderr, "Failed to initialize resource group set: %s\n",
		                                                strerror(errno));
		return NULL;
	}
	rgcount = sdp->md.riinode->i_di.di_size / sizeof(struct gfs2_rindex);
	for (i = 0; i < rgcount; i++) {
		const struct gfs2_rindex *ri;

		ri = lgfs2_rindex_read_one(sdp->md.riinode, rgs, i);
		if (ri == NULL) {
			fprintf(stderr, "Failed to read rindex entry %u: %s\n",
			                                    i, strerror(errno));
			return NULL;
		}
	}
	printf("%u rindex entries found.\n", i);
	return rgs;
}

static int fill_super_block(struct gfs2_sbd *sdp)
{
	sdp->sd_sb.sb_bsize = GFS2_BASIC_BLOCK;
	sdp->bsize = sdp->sd_sb.sb_bsize;

	if (compute_constants(sdp) != 0) {
		fprintf(stderr, "Failed to compute file system constants.\n");
		return 1;
	}
	if (read_sb(sdp) != 0) {
		perror("Failed to read superblock\n");
		return 1;
	}
	sdp->master_dir = lgfs2_inode_read(sdp, sdp->sd_sb.sb_master_dir.no_addr);
	if (sdp->master_dir == NULL) {
		fprintf(stderr, "Failed to read master directory inode.\n");
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct gfs2_sbd sbd;
	lgfs2_rgrps_t rgs;
	struct opts opts;
	int ret;

	memset(&sbd, 0, sizeof(sbd));

	ret = opts_get(argc, argv, &opts);
	if (ret != 0 || opts.got_help)
		exit(ret);

	if (!opts.got_device) {
		fprintf(stderr, "No device specified.\n");
		usage();
		exit(1);
	}
	if (!opts.got_rgnums && !opts.got_rinums) {
		fprintf(stderr, "No resource groups or rindex entries specified.\n");
		usage();
		exit(1);
	}
	if ((sbd.device_fd = open(opts.device, O_RDWR)) < 0) {
		perror(opts.device);
		exit(1);
	}
	if (fill_super_block(&sbd) != 0)
		exit(1);

	rgs = read_rindex(&sbd);
	if (rgs == NULL)
		exit(1);

	if (opts.got_rgnums && nuke_rgs(&sbd, rgs, opts.rgnums, opts.rgnum_count) != 0)
		exit(1);

	if (opts.got_rinums && nuke_ris(&sbd, rgs, opts.rinums, opts.rinum_count) != 0)
		exit(1);

	inode_put(&sbd.md.riinode);
	inode_put(&sbd.master_dir);
	lgfs2_rgrps_free(&rgs);
	fsync(sbd.device_fd);
	close(sbd.device_fd);
	exit(0);
}

/* This function is for libgfs2's sake. */
void print_it(const char *label, const char *fmt, const char *fmt2, ...) {}
