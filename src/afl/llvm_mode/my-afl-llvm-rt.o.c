#include "../config.h"
#include "../types.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <fcntl.h>
#include <signal.h>

#define _RET_IP_ (unsigned long)__builtin_return_address(0)

/* Globals needed by the injected instrumentation. The __afl_area_initial region
   is used for instrumentation output before __afl_map_shm() has a chance to run.
   I will end up as .comm, so it shouldn't be too wasteful. */

u8  __afl_area_initial[MAP_SIZE];
u8* __afl_area_ptr = __afl_area_initial;
unsigned long __afl_prev_loc;

int __afl_forkserver_pid = 0;
int uml_status = 0;
int status_reported = 0;

/* SHM setup. */

static void __afl_map_shm(void) {

  u8 *id_str = getenv(SHM_ENV_VAR);

  /* If we're running under AFL, attach to the appropriate region, replacing the
     early-stage __afl_area_initial region that is needed to allow some really
     hacky .init code to work correctly in projects such as OpenSSL. */

  if (id_str) {

    u32 shm_id = atoi(id_str);

    __afl_area_ptr = shmat(shm_id, NULL, 0);

    /* Whooooops. */

    if (__afl_area_ptr == (void *)-1) _exit(1);

    /* Write something into the bitmap so that even with low AFL_INST_RATIO,
       our parent doesn't give up on us. */

    __afl_area_ptr[0] = 1;

  }

}

void __afl_printf(char *s) {
  printf("str: %s\n", s);
  FILE *fp = fopen("log", "a");
  fprintf(fp, "%s\n", s);
  fclose(fp);
  return;
}

void __afl_printf_int(int i) {
  printf("int: %d\n", i);
  FILE *fp = fopen("log", "a");
  fprintf(fp, "%d\n", i);
  fclose(fp);
  return;
}

void __afl_halt() {
  // printf("HALT: %d\n", status_reported);
  // fflush(stdout);

  if (!status_reported) {
    kill(__afl_forkserver_pid, SIGUSR1);
    status_reported = 1;
  }
}

void halt_handler(int num) {
  uml_status = 1;
}

void __afl_panic() {
  // printf("PANIC: %d\n", status_reported);
  // fflush(stdout);

  if (!status_reported) {
    kill(__afl_forkserver_pid, SIGUSR2);
    status_reported = 1;
  }
}

void panic_handler(int num) {
  uml_status = 2;
}

void __afl_test_forkserver(void) {
  s32 child_pid;
  static u8 tmp[4];

   __afl_forkserver_pid = getpid();
  
  signal(SIGUSR1, halt_handler);
  signal(SIGUSR2, panic_handler);
  signal(SIGTERM, SIG_IGN);

  if (write(FORKSRV_FD + 1, tmp, 4) != 4) return;

  while (1) {
    u32 was_killed;
    int status, real_status;
    
    uml_status = 0;

    if (read(FORKSRV_FD, &was_killed, 4) != 4)
      _exit(1);

    child_pid = fork();

    if (child_pid < 0) _exit(1);

    if (!child_pid) {
      close(FORKSRV_FD);
      close(FORKSRV_FD + 1);
      signal(SIGTERM, SIG_DFL);
      return; 
    }

    if (write(FORKSRV_FD + 1, &child_pid, 4) != 4)
      _exit(1);

    while (uml_status == 0) {
      usleep(10);
    }
    
    if (uml_status == 1) {
      status = 0;
    } else if (uml_status == 2) {
      status = 139;
    }

    if (waitpid(child_pid, &real_status, 0) < 0)
      _exit(1);

    s32 ret;

    // printf("sending status: %d\n", uml_status);
    // fflush(stdout);

    if ((ret = write(FORKSRV_FD + 1, &status, 4)) != 4)
      _exit(1);

  }
}

/* This one can be called from user code when deferred forkserver mode
    is enabled. */

void __afl_manual_init(void) {

  static u8 init_done;

  if (!init_done) {

    __afl_map_shm();
    __afl_test_forkserver();
    init_done = 1;

  }

}

void __sanitizer_cov_trace_pc() {
  unsigned long location = _RET_IP_;
  ++__afl_area_ptr[(location ^ __afl_prev_loc) & (MAP_SIZE - 1)];
  __afl_prev_loc = location >> 1;
}
