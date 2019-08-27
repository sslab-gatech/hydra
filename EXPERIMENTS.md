# EXPERIMENTS

Reproducing key results of the paper.

## Reproducing Bugs Found (§5.1) and Fuzzing Speed (§5.2)

### 1. Crash inconsistency

- On the **master** branch, follow the instructions in README.md
- To reproduce bugs presented in the SOSP'19 paper,
  do the following:
```
$ cd src
$ cd lkl (pwd: proj_root/src/lkl)
$ make mrproper
$ git pull
$ git checkout v4.16-backport
$ ./compile -t btrfs
$ ./compile -t ext4
$ ./compile -t f2fs
$ ./compile -t xfs
$ cd .. (pwd: proj_root/src)
```
Then, follow steps 4~ in README.md

- Test cases that trigger crash consistency bugs are stored under
  the specified `log` directory.
- When a bug is found, copy the test case (e.g., `log_dir/tesecaseid.c`)
  and the base image (e.g., `samples/oracle/btrfs-10.image`) into
  a VM instance running Linux kernel v5.0.
- To simulate a crash, insert the following line after all syscalls.
```
system("echo b > /proc/sysrq-trigger");
```
- Compile the test case
- Mount the copied base image, and execute the compiled test case.
  (It requires `sudo` for SysRq).
- After the last syscall, VM will reboot.
- Re-mounting the file system image, you will see crash inconsistency.
- For more information, please refer to our
  [bugzilla report](https://bugzilla.kernel.org/show_bug.cgi?id=202843)
  with the attached PoC of one of the `Btrfs` bugs Hydra found.

### 2. Logic bugs

- Checkout **logicbug** branch.
- Enable kernel checkers in LKL by running the provided script:
```
$ cd src
$ ./setup_logicbug.sh
```
- Re-compile Executors:
```
# Assume we're already in src/
$ cd lkl
$ make mrproper
$ ./compile -t btrfs # You can replace btrfs with ext4 / f2fs / xfs
```
- Run fuzzing.
```
// Make sure you are not reusing the input and output directories from another experiments.
// If so, create new directories and use them instead!
$ AFL_SKIP_BIN_CHECK=1 ./combined/afl-image-syscall/afl-fuzz -S fuzzer-lb -b btrfs-lb -s fs/btrfs/btrfs_wrapper.so -e samples/oracle/btrfs-00.image -y seed-lb -i in-lb -o out-lb -u 11 -- lkl/tools/lkl/btrfs-combined -t btrfs -p @@
```
- The bugs will be stored under `out-lb/fuzzer-lb/crashes`.

### 3. Specification violation

### 4. Memory-safety bugs

* To reproduce memory-safety bugs,
  please refer to [Janus](https://github.com/sslab-gatech/janus),
  which is a use case of Hydra
  that specializes in finding memory bugs.

