#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <sysexits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <libintl.h>
#define _(String) gettext(String)

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "tunegfs2.h"

struct tunegfs2 tunegfs2_struct;
struct tunegfs2 *tfs = &tunegfs2_struct;


static void parse_mount_options(char *arg)
{
	struct opt_map *m;
	char *s, *c;
	int l;
	struct opt_map {
		const char *tag;
		int *flag;
		char **val;
	} map[]= {
		{ "lockproto=", &tfs->opt_proto, &tfs->proto },
		{ "locktable=", &tfs->opt_table, &tfs->table },
		{ NULL, 0, NULL }
	};

	s = arg;
	for (m = &map[0]; m->tag; m++) {
		l = strlen(m->tag);
		if (!strncmp(s, m->tag, l)) {
			*(m->flag) = 1;
			*(m->val) = s + l;
			c = strchr(*(m->val), ',');
			if (!c)
				break;
			*c='\0';
			s = c+1;
		}
	}
}

static void usage(char *name)
{
	printf("%s %s [-hlV] [-L <%s>] [-U <%s>] [-o <%s>] <%s>\n",
	       _("Usage:"), basename(name), _("label"), _("UUID"),
	       _("mount options"), _("device"));
}

static void version(void)
{
	printf("tunegfs2 (%s %s)\n", __DATE__, __TIME__);
}

int main(int argc, char **argv)
{
	int c, status;

	memset(tfs, 0, sizeof(struct tunegfs2));
	while((c = getopt(argc, argv, "hL:U:lo:V")) != -1) {
		switch(c) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'L':
			tfs->opt_label = 1;
			tfs->label = optarg;
			break;
		case 'U':
			tfs->opt_uuid = 1;
			tfs->uuid = optarg;
			break;
		case 'l':
			tfs->opt_list = 1;
			break;
		case 'o':
			parse_mount_options(optarg);
			break;
		case 'V':
			version();
			return 0;
		default:
			fprintf(stderr, _("Invalid option: %c\n"), c);
			usage(argv[0]);
			return EX_USAGE;
		}
	}

	if ((argc - optind) != 1) {
		fprintf(stderr, _("Incorrect number of arguments\n"));
		usage(argv[0]);
		return EX_USAGE;
	}

	if (tfs->opt_label && tfs->opt_table) {
		fprintf(stderr, _("The -L argument cannot be used with the 'locktable' option\n"));
		usage(argv[0]);
		return EX_USAGE;
	}

	tfs->devicename = argv[optind];
	tfs->fd = open(tfs->devicename, O_RDWR); 

	if (tfs->fd < 0) {
		fprintf(stderr, _("Unable to open device %s\n"),
				tfs->devicename);
		return EX_IOERR;
	}

	status = read_super(tfs);
	if (status)
		goto out;

	if (tfs->opt_uuid) {
		status = change_uuid(tfs, tfs->uuid);
		if (status)
			goto out;
	}

	if (tfs->opt_proto) {
		status = change_lockproto(tfs, tfs->proto);
		if (status)
			goto out;
	}

	if (tfs->opt_label) {
		status = change_locktable(tfs, tfs->label);
		if (status)
			goto out;
	}

	if (tfs->opt_table) {
		status = change_locktable(tfs, tfs->table);
		if (status)
			goto out;
	}

	if (tfs->opt_label || tfs->opt_uuid || tfs->opt_table ||
	    tfs->opt_proto) {
		status = write_super(tfs);
		if (status)
			goto out;
	}

	if (tfs->opt_list) {
		version();
		print_super(tfs);
	}

	close(tfs->fd);
out:
	return status;
}
