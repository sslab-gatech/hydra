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

    char dirName0[256] = "A";
    char dirName1[256] = "A/B";
    char dirName2[256] = "C";
    char fileName0[256] = "A/B/foo";
    char fileName1[256] = "C/foo";
    char fileName2[256] = "A/bar";

    mkdir(dirName0, 755);
    mkdir(dirName1, 755);
    mkdir(dirName2, 755);
    int fd0 = creat(fileName0, O_RDWR);
    sync();

    rename(fileName0, fileName1);
    int fd1 = creat(fileName2, O_RDWR);
    DIR* ds = opendir(dirName0);
    int fd2 = dirfd(ds);
    int ret = fsync(fd2);

    system("echo b > /proc/sysrq-trigger");
    return 0;
}

