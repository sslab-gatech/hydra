# EXPERIMENTS

Reproducing key results of the paper.

## Reproducing Bugs Found (§5.1) and Fuzzing Speed (§5.2)

### 1. Crash inconsistency

- On the **master** branch, follow the instructions in README.md
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

### 3. Specification violation

### 4. Memory-safety bugs

* To reproduce memory-safety bugs,
  please refer to [Janus](https://github.com/sslab-gatech/janus),
  which is a use case of Hydra
  that specializes in finding memory bugs.

## Code Coverage (§5.3)

## Hydra Framework Services

