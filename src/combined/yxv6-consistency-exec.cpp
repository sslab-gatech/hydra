// SPDX-License-Identifier: MIT

#include <mntent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/limits.h>

#include <string>
#include <iostream>

#include "Program.hpp"

#define VERBOSE 0

using namespace std;

static string yxv6_handler;

int is_mounted (char * dev_path) {
    FILE * mtab = NULL;
    struct mntent * part = NULL;
    int is_mounted = 0;

    if ( ( mtab = setmntent ("/etc/mtab", "r") ) != NULL) {
        while ( ( part = getmntent ( mtab) ) != NULL) {
            if ( ( part->mnt_dir != NULL ) && ( strcmp ( part->mnt_dir, dev_path ) ) == 0 ) {
                is_mounted = 1;
            }
        }
        endmntent ( mtab);
    }
    return is_mounted;
}

void umount_yxv6(char *mpoint) {
    int status, child_pid;
    child_pid = fork();
    if (child_pid < 0) {
        printf("fork() failed.");
        exit(1);
    }
    if (!child_pid) {
        char *args[4];
        args[0] = const_cast<char*>("fusermount");
        args[1] = const_cast<char*>("-u");
        args[2] = mpoint;
        args[3] = NULL;
        execvp(args[0], args);
    }
    waitpid(child_pid, &status, 0);
}

static void exec_syscall(Program *prog, Syscall *scall) {

    long params[6];
    long ret;
    int cnt = 0;

    for (Arg *arg : scall->args) {
        if (!arg->is_variable)
            params[cnt] = arg->value;
        else {
            Variable *v = prog->variables[arg->index];
            if (v->is_pointer() && v->value == 0)
                v->value = static_cast<uint8_t*>(malloc(v->size));
            params[cnt] = reinterpret_cast<long>(v->value);
        }
        cnt++;
    }

    ret = syscall(scall->nr, params[0], params[1], params[2], params[3], params[4], params[5]);
    if (scall->ret_index != -1)
        prog->variables[scall->ret_index]->value = reinterpret_cast<uint8_t*>(ret);

    if (VERBOSE) {
        show_syscall(prog, scall);
        printf("ret: %ld\n", ret);
    }
}

static void close_active_fds(Program *prog) {
    for (int64_t fd_index : prog->active_fds) {
        long fd = reinterpret_cast<long>(prog->variables[fd_index]->value);
        syscall(SYS_close, fd);
    }
}

void mount_crashed(char* crashed_img, char *crashed_mpoint) {
    int status, child_pid;
    child_pid = fork();
    if (child_pid < 0) {
        printf("fork() failed.");
        exit(1);
    }
    if (!child_pid) {
        if (VERBOSE)
            printf("child mounting crashed img\n");
        char *args[11];
        args[0] = const_cast<char*>("python");
        args[1] = const_cast<char*>(yxv6_handler.c_str());
        args[2] = const_cast<char*>("-o");
        args[3] = const_cast<char*>("max_read=4096");
        args[4] = const_cast<char*>("-o");
        args[5] = const_cast<char*>("max_write=4096");
        args[6] = const_cast<char*>("-s");
        args[7] = const_cast<char*>(crashed_mpoint);
        args[8] = const_cast<char*>("-i");
        args[9] = const_cast<char*>(crashed_img);
        args[10] = NULL;
        execvp(args[0], args);
    }
    waitpid(child_pid, &status, WNOHANG);
}

int main(int argc, char *argv[]) {
    char *mpoint = argv[1];
    char *prog_path = argv[2]; // /tmp/prog-[NUM]
    char *emul_path = argv[3];
    char *img_path = argv[4];

    if (argc < 5) {
        printf("Usage: ./yxv6-cc mnt_point prog_path emulator_path img_path\n");
        printf("** Make sure the arguments are aligned with the AFL instance **\n");
        printf("e.g.,\n");
        printf("AFL: AFL_NO_FORKSRV=1 AFL_FAST_CAL=1\n");
        printf("     ./combined/afl-yxv6-syscall/afl-fuzz -t 1000000 -m none\n");
        printf("     -f /tmp/prog-1 -i ./seed-yxv6 -o output -k\n");
        printf("     -e /tmp/mosbench/tmpfs-separate/0/disk.img\n");
        printf("     -p /tmp/mosbench/tmpfs-separate/0/tmp-1.img -u 1\n");
        printf("     -- python yggdrasil/yav_xv6_main.py\n");
        printf("     -o max_read=4096 -o max_write=4096 -s /tmp/yggmnt\n");
        printf("      -i /tmp/mosbench/tmpfs-separate/0/tmp-1.img\n");
        printf("EXE: ./yxv6-cc /tmp/mnt-1 /tmp/prog-1 ./emulator/yxv6.py /tmp/tmp-1.img\n");
        return 1;
    }

    string mpoint_str = mpoint;
    string crashed_mpoint = mpoint_str + "-crashed";

    string prog_path_str = prog_path;
    string cp_prog_cmd = "cp " + prog_path_str + " " + prog_path_str + "-prev";

    string img_path_str = img_path;
    string s = img_path_str;
    size_t pos = s.rfind("tmp");
    s.replace(pos, 3, "crashed");
    string crashed_img_path = s;
    string cp_img_cmd = "cp " + img_path_str + " " + crashed_img_path;

    string emul_path_str = emul_path;
    string emul_cmd = emul_path_str + " -d " + crashed_mpoint + " -p " + prog_path_str + "-prev";

    string umount_cmd = "fusermount -u " + crashed_mpoint;

    char cwd_buf[PATH_MAX];
    getcwd(cwd_buf, PATH_MAX);
    std::string cwd_buf_str(cwd_buf);
    yxv6_handler = cwd_buf_str + "/yggdrasil/yav_xv6_main.py";

    if (VERBOSE)
        printf("init done\n");

    while (1) {
        while (!is_mounted(mpoint)) {
            usleep(5);
        }
        if (VERBOSE) {
            printf("========================\n");
            printf("first mounting detected.\n");
        }


        chdir(mpoint);
        Program *prog = Program::deserialize(prog_path, true);
        for (Syscall *scall : prog->syscalls) {
            exec_syscall(prog, scall);
        }
        close_active_fds(prog);
        delete prog;

        system(cp_img_cmd.c_str());
        system(cp_prog_cmd.c_str());
        system("sync");

        if (VERBOSE)
            printf("first execution done.\n");
        chdir("/");
        while (is_mounted(mpoint)) {
            usleep(5);
            if (VERBOSE)
                printf("trying to unmount /tmp/mnt\n");
            umount_yxv6(mpoint);
        }

        mount_crashed((char*)crashed_img_path.c_str(), (char*)crashed_mpoint.c_str());
        if (VERBOSE)
            printf("parent checking crashed\n");
        while (!is_mounted((char*)crashed_mpoint.c_str())) {
            usleep(5);
        }
        if (VERBOSE)
            printf("crashed img mounting detected.\n");

        chdir(crashed_mpoint.c_str());
        system(emul_cmd.c_str());

        chdir("/");
        if (VERBOSE)
            printf("second execution done.\n");
        system(umount_cmd.c_str());

    }

    return 0;
}


