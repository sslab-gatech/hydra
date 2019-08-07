#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <mntent.h>
#include <libintl.h>
#include <locale.h>
#define _(String) gettext(String)

#include "metafs.h"

int metafs_interrupted;

static int lock_for_admin(struct metafs *mfs, int debug)
{
	int error;

	if (debug)
		printf(_("Trying to get admin lock..."));

	mfs->fd = open(mfs->path, O_RDONLY | O_NOFOLLOW);
	if (mfs->fd < 0)
		return -1;

	error = flock(mfs->fd, LOCK_EX);
	if (error) {
		close(mfs->fd);
		return -1;
	}
	if (debug)
		printf(_("locked.\n"));
	return 0;
}

static void sighandler(int error)
{
	metafs_interrupted = 1;
}

static void setsigs(void (*handler)(int))
{
	struct sigaction sa = {	.sa_handler = handler };

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGILL, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGCONT, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
}

int mount_gfs2_meta(struct metafs *mfs, const char *path, int debug)
{
	int ret;

	mfs->path = strdup("/tmp/.gfs2meta.XXXXXX");
	if (mfs->path == NULL)
		return -1;

	if(!mkdtemp(mfs->path))
		goto err_free;

	setsigs(sighandler);

	ret = mount(path, mfs->path, "gfs2meta", 0, mfs->context);
	if (ret)
		goto err_rmdir;

	if (lock_for_admin(mfs, debug))
		goto err_umount;

	return 0;

err_umount:
	if (umount(mfs->path))
		/* Translators: the first %s here is a path, the second is an error message */
		fprintf(stderr, _("Could not unmount %s: %s\n"),
		                    mfs->path, strerror(errno));
	setsigs(SIG_DFL);
err_rmdir:
	rmdir(mfs->path);
err_free:
	free(mfs->path);
	mfs->path = NULL;
	return -1;
}

void cleanup_metafs(struct metafs *mfs)
{
	int ret;

	if (mfs->fd <= 0)
		return;

	fsync(mfs->fd);
	close(mfs->fd);
	ret = umount(mfs->path);
	if (ret)
		/* Translators: the first %s here is a path, the second is an error message */
		fprintf(stderr, "Could not unmount %s : %s\n",
			mfs->path, strerror(errno));
	else
		rmdir(mfs->path);

	setsigs(SIG_DFL);
	metafs_interrupted = 0;

	free(mfs->path);
	mfs->path = NULL;
	free(mfs->context);
	mfs->context = NULL;
}

/**
 * Returns a duplicate of the 'context' mount option, or NULL if not found.
 */
char *copy_context_opt(struct mntent *mnt)
{
	char *ctx, *end;

	ctx = hasmntopt(mnt, "context");
	if (ctx == NULL)
		return NULL;

	end = strchr(ctx, ',');
	if (end == NULL)
		return NULL;

	return strndup(ctx, end - ctx);
}
