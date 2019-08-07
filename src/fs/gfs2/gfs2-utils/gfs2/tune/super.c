#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <libintl.h>
#define _(String) gettext(String)
#include <linux_endian.h>
#include <linux/gfs2_ondisk.h>
#include "tunegfs2.h"

#ifdef GFS2_HAS_UUID
#include <uuid.h>
#endif

int read_super(struct tunegfs2 *tfs)
{
	void *block;
	int n;
       	tfs->sb_start = GFS2_SB_ADDR << GFS2_BASIC_BLOCK_SHIFT;
	block = malloc(sizeof(char) * GFS2_DEFAULT_BSIZE);
	if (!block) {
		perror("read_super: malloc");
		return EX_UNAVAILABLE;
	}
	n = pread(tfs->fd, block, GFS2_DEFAULT_BSIZE, tfs->sb_start);
	if (n < 0) {
		perror("read_super: pread");
		free(block);
		return EX_IOERR;
	}
	tfs->sb = block;
	if (be32_to_cpu(tfs->sb->sb_header.mh_magic) != GFS2_MAGIC) {
		fprintf(stderr, _("Device does not contain a GFS or GFS2 file system\n"));
		tfs->sb = NULL;
		free(block);
		return EX_IOERR;
	}
	/* Ensure that table and proto are NULL terminated */
	tfs->sb->sb_lockproto[GFS2_LOCKNAME_LEN - 1] = '\0';
	tfs->sb->sb_locktable[GFS2_LOCKNAME_LEN - 1] = '\0';
	return 0;
}

static int is_gfs2(const struct tunegfs2 *tfs)
{
	return be32_to_cpu(tfs->sb->sb_fs_format) == GFS2_FORMAT_FS;
}

int print_super(const struct tunegfs2 *tfs)
{
	printf(_("File system volume name: %s\n"), tfs->sb->sb_locktable);
#ifdef GFS2_HAS_UUID
	{
	char readable_uuid[36+1];

	uuid_unparse(tfs->sb->sb_uuid, readable_uuid);
	printf(_("File system UUID: %s\n"), readable_uuid);
	}
#endif
	printf( _("File system magic number: 0x%X\n"), be32_to_cpu(tfs->sb->sb_header.mh_magic));
	printf(_("Block size: %d\n"), be32_to_cpu(tfs->sb->sb_bsize));
	printf(_("Block shift: %d\n"), be32_to_cpu(tfs->sb->sb_bsize_shift));
	printf(_("Root inode: %llu\n"), (unsigned long long)be64_to_cpu(tfs->sb->sb_root_dir.no_addr));
	if (is_gfs2(tfs))
		printf(_("Master inode: %llu\n"), (unsigned long long)be64_to_cpu(tfs->sb->sb_master_dir.no_addr));
	printf(_("Lock protocol: %s\n"), tfs->sb->sb_lockproto);
	printf(_("Lock table: %s\n"), tfs->sb->sb_locktable);

	return 0;
}

int write_super(const struct tunegfs2 *tfs)
{
	int n;
	n = pwrite(tfs->fd, tfs->sb, GFS2_DEFAULT_BSIZE, tfs->sb_start);
	if (n < 0) {
		perror("write_super: pwrite");
		return EX_IOERR;
	}
	return 0;
}

int change_uuid(struct tunegfs2 *tfs, const char *str)
{
#ifdef GFS2_HAS_UUID
	uuid_t uuid;
	int status;

	status = uuid_parse(str, uuid);
	if (status == 0)
		uuid_copy(tfs->sb->sb_uuid, uuid);
	return status;
#else
	fprintf(stderr, _("UUID support unavailable in this build\n"));
	return 1;
#endif
}

int change_lockproto(struct tunegfs2 *tfs, const char *lockproto)
{
	int l = strlen(lockproto);

	if (l >= GFS2_LOCKNAME_LEN) {
		fprintf(stderr, _("Invalid lock protocol: %s\n"), _("too long"));
		return EX_DATAERR;
	}

	if (strncmp(lockproto, "lock_dlm", 8) &&
	    strncmp(lockproto, "lock_nolock", 11)) {
		fprintf(stderr, _("Invalid lock protocol: %s\n"), lockproto);
		return EX_DATAERR;
	}
	memset(tfs->sb->sb_lockproto, '\0', GFS2_LOCKNAME_LEN);
	strncpy(tfs->sb->sb_lockproto, lockproto, l);
	return 0;
}

int change_locktable(struct tunegfs2 *tfs, const char *locktable)
{
	const char *errpre = _("Invalid lock table:");

	if (strlen(locktable) >= GFS2_LOCKNAME_LEN) {
		fprintf(stderr, "%s %s\n", errpre, _("too long"));
		return EX_DATAERR;
	}

	if (strcmp(tfs->sb->sb_lockproto, "lock_dlm") == 0) {
		char *fsname = strchr(locktable, ':');
		if (fsname == NULL) {
			fprintf(stderr, "%s %s\n", errpre, _("missing colon"));
			return EX_DATAERR;
		}
		if (strlen(++fsname) > 30) {
			fprintf(stderr, "%s %s\n", errpre, _("file system name is too long"));
			return EX_DATAERR;
		}
		if (strchr(fsname, ':')) {
			fprintf(stderr, "%s %s\n", errpre, _("contains more than one colon"));
			return EX_DATAERR;
		}
	}

	strcpy(tfs->sb->sb_locktable, locktable);
	return 0;
}

