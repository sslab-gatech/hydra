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
    char dir1[256] = "B";
    char file0[256] = "A/foo";
    char dir2[256] = "B/C";
    char file1[256] = "B/foo2";
    char file2[256] = "A/bar";
    char dir3[256] = "A/C";
    char file3[256] = "A/foo2";

    mkdir(dir0, 755);
    mkdir(dir1, 755);
    int fd0 = creat(file0, O_RDWR);
    mkdir(dir2, 755);
    int fd1 = creat(file1, O_RDWR);
    sync();

    link(file0, file2);
    rename(dir2, dir3);
    rename(file1, file3);
    int ret = fsync(fd0);

    system("echo b > /proc/sysrq-trigger");
    return 0;
}

