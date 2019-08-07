#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>


int main(int argc, char* argv[])
{
    if (argc < 2){
        printf("usage: %s path_to_mounted_dir\n", argv[0]);
        return 0;
    }
    chdir(argv[1]);

    mkdir("A", 0755);
    sync();
    int fd0 = creat("foo1", O_RDWR);
    symlink("foo1", "A/bar1");

    DIR* ds = opendir("A");
    int fd1 = dirfd(ds);
    fsync(fd1);

    system("echo b > /proc/sysrq-trigger");
    return 0;
}

