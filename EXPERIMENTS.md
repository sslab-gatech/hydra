# EXPERIMENTS

This document explains how to reproduce the key results we reported in our
SOSP'19 paper.

## Reproducing the detected bugs

### 1. Crash inconsistency

#### [On the host machine]

1. On the **master** branch, follow the instructions in `README.md` to run the
   fuzzer.
2. Test cases that trigger crash consistency bugs are stored under
   the specified `log` directory (e.g., `/tmp/mosbench/tmpfs-separate/10/log`).
3. Execute the post-processor to retrieve the **code** of the buggy testcase:
   (e.g., `./src/utils/symc3-process.py path/to/test_case_file`).
4. Copy the retrieved test case code (e.g., `test_case_file.c`)
   and the base image (e.g., `src/samples/oracle/btrfs-10.image`) into
   a VM instance running Linux kernel v5.0 (or any kernel that you've tested).

#### [On the VM]

5. Create a directory to mount (`mkdir test`).
6. Mount the copied image (`sudo mount -t {filesystem_name} /dev/sdb test`)
7. Compile and execute the copied test case:
   ```
   $ gcc test_case_file.c
   $ sudo ./a.out
   ```
   (It requires `sudo` for SysRq).
8. Right after the last syscall, the VM will reboot.
9. Now mount the crashed file system image, and check crash inconsistency.

- For more information, please refer to our
  [bugzilla report](https://bugzilla.kernel.org/show_bug.cgi?id=202843)
  with the attached PoC of one of the `Btrfs` bugs Hydra found.

- In addition, a live demo is available in the
  [SOSP'19 presentation video](https://www.youtube.com/watch?v=3gj9YH8AyqE).
  At the beginning of the presentation (00:12), I ran Hydra against `f2fs`.
  At the end (14:23), I went over the steps explained above to reproduce one
  of the bugs fuzzer reported during the talk, and verified that it indeed was
  an actual bug.

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

