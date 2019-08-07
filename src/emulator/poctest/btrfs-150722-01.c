#include <sys/types.h>
#include <sys/mount.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <sys/syscall.h>

#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	unsigned char v0[8192];
	unsigned char v1[8192];
	char v2[] = ".";
	char v3[] = "foo";
	char v4[] = "foo/bar";
	char v5[] = "foo/bar/baz";
	char v6[] = "foo/bar/xattr";
	char v7[] = "foo/bar/acl";
	char v8[] = "foo/bar/æøå";
	char v9[] = "foo/bar/fifo";
	char v10[] = "foo/bar/hln";
	char v11[] = "foo/bar/sln";
	char v12[] = "./A";
	char v13[] = "./A/foo";
	char v14[] = "./A/bar";
	char v15[] = "./A/foo2";
	char v16[] = "./A/bar2";
	long v17;
	long v18;
	syscall(SYS_mkdir, (long)v12);
	v17 = syscall(SYS_open, (long)v13, 2, 0);
	v18 = syscall(SYS_open, (long)v14, 2, 0);
	syscall(SYS_sync);
	syscall(SYS_link, (long)v13, (long)v15);
	syscall(SYS_link, (long)v14, (long)v16);
	syscall(SYS_fsync, (long)v18);

	close(v17);
	close(v18);
	return 0;
}

