// SPDX-License-Identifier: MIT

#ifndef _CONSTANTS_HPP
#define _CONSTANTS_HPP

/************** syscall numbers **************/
#include <sys/syscall.h>
extern const char *syscall_name[];

/************** syscall arguments **************/
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FALLOC_FL_INSERT_RANGE		0x20
#define FALLOC_FL_UNSHARE_RANGE		0x40

/************** misc numbers **************/
#define PAGE_SIZE 	4096
/*
enum syscall_number : uint32_t {
	nr_read = 0,
	nr_write,
	nr_open,
	nr_close,
	nr_newstat,
	nr_newfstat,
	nr_newlstat,
	nr_poll,
	nr_lseek,
	nr_mmap,
	nr_mprotect,
	nr_munmap,
	nr_brk,
	nr_rt_sigaction,
	nr_rt_sigprocmask,
	nr_rt_sigreturn,
	nr_ioctl,
	nr_pread64,
	nr_pwrite64,
	nr_readv,
	nr_writev,
	nr_access,
	nr_pipe,
	nr_select,
	nr_sched_yield,
	nr_mremap,
	nr_msync,
	nr_mincore,
	nr_madvise,
	nr_shmget,
	nr_shmat,
	nr_shmctl,
	nr_dup,
	nr_dup2,
	nr_pause,
	nr_nanosleep,
	nr_getitimer,
	nr_alarm,
	nr_setitimer,
	nr_getpid,
	nr_sendfile64,
	nr_socket,
	nr_connect,
	nr_accept,
	nr_sendto,
	nr_recvfrom,
	nr_sendmsg,
	nr_recvmsg,
	nr_shutdown,
	nr_bind,
	nr_listen,
	nr_getsockname,
	nr_getpeername,
	nr_socketpair,
	nr_setsockopt,
	nr_getsockopt,
	nr_clone,
	nr_fork,
	nr_vfork,
	nr_execve,
	nr_exit,
	nr_wait4,
	nr_kill,
	nr_newuname,
	nr_semget,
	nr_semop,
	nr_semctl,
	nr_shmdt,
	nr_msgget,
	nr_msgsnd,
	nr_msgrcv,
	nr_msgctl,
	nr_fcntl,
	nr_flock,
	nr_fsync,
	nr_fdatasync,
	nr_truncate,
	nr_ftruncate,
	nr_getdents,
	nr_getcwd,
	nr_chdir,
	nr_fchdir,
	nr_rename,
	nr_mkdir,
	nr_rmdir,
	nr_creat,
	nr_link,
	nr_unlink,
	nr_symlink,
	nr_readlink,
	nr_chmod,
	nr_fchmod,
	nr_chown,
	nr_fchown,
	nr_lchown,
	nr_umask,
	nr_gettimeofday,
	nr_getrlimit,
	nr_getrusage,
	nr_sysinfo,
	nr_times,
	nr_ptrace,
	nr_getuid,
	nr_syslog,
	nr_getgid,
	nr_setuid,
	nr_setgid,
	nr_geteuid,
	nr_getegid,
	nr_setpgid,
	nr_getppid,
	nr_getpgrp,
	nr_setsid,
	nr_setreuid,
	nr_setregid,
	nr_getgroups,
	nr_setgroups,
	nr_setresuid,
	nr_getresuid,
	nr_setresgid,
	nr_getresgid,
	nr_getpgid,
	nr_setfsuid,
	nr_setfsgid,
	nr_getsid,
	nr_capget,
	nr_capset,
	nr_rt_sigpending,
	nr_rt_sigtimedwait,
	nr_rt_sigqueueinfo,
	nr_rt_sigsuspend,
	nr_sigaltstack,
	nr_utime,
	nr_mknod,
	nr_obsolete_0,
	nr_personality,
	nr_ustat,
	nr_statfs,
	nr_fstatfs,
	nr_sysfs,
	nr_getpriority,
	nr_setpriority,
	nr_sched_setparam,
	nr_sched_getparam,
	nr_sched_setscheduler,
	nr_sched_getscheduler,
	nr_sched_get_priority_max,
	nr_sched_get_priority_min,
	nr_sched_rr_get_interval,
	nr_mlock,
	nr_munlock,
	nr_mlockall,
	nr_munlockall,
	nr_vhangup,
	nr_modify_ldt,
	nr_pivot_root,
	nr_sysctl,
	nr_prctl,
	nr_arch_prctl,
	nr_adjtimex,
	nr_setrlimit,
	nr_chroot,
	nr_sync,
	nr_acct,
	nr_settimeofday,
	nr_mount,
	nr_umount,
	nr_swapon,
	nr_swapoff,
	nr_reboot,
	nr_sethostname,
	nr_setdomainname,
	nr_iopl,
	nr_ioperm,
	nr_obsolete_1,
	nr_init_module,
	nr_delete_module,
	nr_obsolete_2,
	nr_obsolete_3,
	nr_quotactl,
	nr_obsolete_4,
	nr_obsolete_5,
	nr_obsolete_6,
	nr_obsolete_7,
	nr_obsolete_8,
	nr_obsolete_9,
	nr_gettid,
	nr_readahead,
	nr_setxattr,
	nr_lsetxattr,
	nr_fsetxattr,
	nr_getxattr,
	nr_lgetxattr,
	nr_fgetxattr,
	nr_listxattr,
	nr_llistxattr,
	nr_flistxattr,
	nr_removexattr,
	nr_lremovexattr,
	nr_fremovexattr,
	nr_tkill,
	nr_time,
	nr_futex,
	nr_sched_setaffinity,
	nr_sched_getaffinity,
	nr_obsolete_10,
	nr_io_setup,
	nr_io_destroy,
	nr_io_getevents,
	nr_io_submit,
	nr_io_cancel,
	nr_obsolete_11,
	nr_lookup_dcookie,
	nr_epoll_create,
	nr_obsolete_12,
	nr_obsolete_13,
	nr_remap_file_pages,
	nr_getdents64,
	nr_set_tid_address,
	nr_restart_syscall,
	nr_semtimedop,
	nr_fadvise64,
	nr_timer_create,
	nr_timer_settime,
	nr_timer_gettime,
	nr_timer_getoverrun,
	nr_timer_delete,
	nr_clock_settime,
	nr_clock_gettime,
	nr_clock_getres,
	nr_clock_nanosleep,
	nr_exit_group,
	nr_epoll_wait,
	nr_epoll_ctl,
	nr_tgkill,
	nr_utimes,
	nr_obsolete_14,
	nr_mbind,
	nr_set_mempolicy,
	nr_get_mempolicy,
	nr_mq_open,
	nr_mq_unlink,
	nr_mq_timedsend,
	nr_mq_timedreceive,
	nr_mq_notify,
	nr_mq_getsetattr,
	nr_kexec_load,
	nr_waitid,
	nr_add_key,
	nr_request_key,
	nr_keyctl,
	nr_ioprio_set,
	nr_ioprio_get,
	nr_inotify_init,
	nr_inotify_add_watch,
	nr_inotify_rm_watch,
	nr_migrate_pages,
	nr_openat,
	nr_mkdirat,
	nr_mknodat,
	nr_fchownat,
	nr_futimesat,
	nr_newfstatat,
	nr_unlinkat,
	nr_renameat,
	nr_linkat,
	nr_symlinkat,
	nr_readlinkat,
	nr_fchmodat,
	nr_faccessat,
	nr_pselect6,
	nr_ppoll,
	nr_unshare,
	nr_set_robust_list,
	nr_get_robust_list,
	nr_splice,
	nr_tee,
	nr_sync_file_range,
	nr_vmsplice,
	nr_move_pages,
	nr_utimensat,
	nr_epoll_pwait,
	nr_signalfd,
	nr_timerfd_create,
	nr_eventfd,
	nr_fallocate,
	nr_timerfd_settime,
	nr_timerfd_gettime,
	nr_accept4,
	nr_signalfd4,
	nr_eventfd2,
	nr_epoll_create1,
	nr_dup3,
	nr_pipe2,
	nr_inotify_init1,
	nr_preadv,
	nr_pwritev,
	nr_rt_tgsigqueueinfo,
	nr_perf_event_open,
	nr_recvmmsg,
	nr_fanotify_init,
	nr_fanotify_mark,
	nr_prlimit64,
	nr_name_to_handle_at,
	nr_open_by_handle_at,
	nr_clock_adjtime,
	nr_syncfs,
	nr_sendmmsg,
	nr_setns,
	nr_getcpu,
	nr_process_vm_readv,
	nr_process_vm_writev,
	nr_kcmp,
	nr_finit_module,
	nr_sched_setattr,
	nr_sched_getattr,
	nr_renameat2,
	nr_seccomp,
	nr_getrandom,
	nr_memfd_create,
	nr_kexec_file_load,
	nr_bpf,
	nr_execveat,
	nr_userfaultfd,
	nr_membarrier,
	nr_mlock2,
	nr_copy_file_range,
	nr_preadv2,
	nr_pwritev2,
	nr_pkey_mprotect,
	nr_pkey_alloc,
	nr_pkey_free,
	nr_statx
};
*/

/************** useful constants ************/
// xattr
#define NUM_DEFAULT_XATTR (10)
#define NUM_DEFAULT_XATTR_PREFIX (5)
extern const char *default_xattr[NUM_DEFAULT_XATTR];
extern const char *default_xattr_prefix[NUM_DEFAULT_XATTR_PREFIX];

#endif
