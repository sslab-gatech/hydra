#include <mntent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "Program.hpp"

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

void umountFSCQ(char *mpoint) {
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

	show_syscall(prog, scall);
	printf("ret: %ld\n", ret);
}

static void close_active_fds(Program *prog) {
	for (int64_t fd_index : prog->active_fds) {
		long fd = reinterpret_cast<long>(prog->variables[fd_index]->value);
		syscall(SYS_close, fd);
	}
}


int main(int argc, char *argv[]) {
    char *mpoint = argv[1];
    char *prog_path = argv[2];

    while (1) {

        while (!is_mounted(mpoint)) {
            usleep(5);
        }
        printf("mounting detected.\n");

        chdir(mpoint);
        Program *prog = Program::deserialize(prog_path, true);
        for (Syscall *scall : prog->syscalls) {
            exec_syscall(prog, scall);
        }
        close_active_fds(prog);
        delete prog;
        chdir("/");

        printf("execution done.\n");
  	    umountFSCQ(mpoint);  
    }

    return 0;
}


