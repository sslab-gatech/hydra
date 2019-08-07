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
    char fileName0[256] = "A/foo";
    char fileName1[256] = "A/bar";
    char fileName2[256] = "A/foo2";
    char fileName3[256] = "A/bar2";

    mkdir(dirName0, 755);
    int fd0 = creat(fileName0, O_RDWR);
    int fd1 = creat(fileName1, O_RDWR);

    sync();

    link(fileName0, fileName2);
    link(fileName1, fileName3);
    fsync(fd1);

    system("echo b > /proc/sysrq-trigger");
    return 0;
}

