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

    char dir0[256] = "A";
    char file0[256] = "A/foo";
    char file1[256] = "A/bar";

    mkdir(dir0, 755);
    int fd0 = creat(file0, O_RDWR);
    DIR* ds = opendir(dir0);
    int fd1 = dirfd(ds);
    fsync(fd0);
    fsync(fd1);

    rename(file0, file1);
    int fd2 = creat(file0, O_RDWR);
    int ret = fsync(fd2);

    system("echo b > /proc/sysrq-trigger");
    return 0;
}

