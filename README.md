# Hydra: An Extensible Fuzzing Framework for Finding Semantic Bugs in File Systems


## Paper

* [Finding Semantic Bugs in File Systems with an Extensible Fuzzing Framework (SOSP 2019)](https://squizz617.github.io/pubs/hydra-sosp19.pdf) - original paper
* [Finding Bugs in File Systems with an Extensible Fuzzing Framework (ACM Transactions on Storage, Vol. 16, No. 2)](https://squizz617.github.io/pubs/hydra-tos20.pdf) - includes more details


## Overview

Hydra is a state-of-the-art fuzzing framework for file systems.
It provides building blocks for file system fuzzing,
including multi-dimensional input mutators, feedback engines,
a libOS-based executor, and a bug reproducer with test case minimizer.
Developers only need to focus on writing (or bringing in) a checker
which defines the core logic for finding the types of bugs
of their own interests.
Along with the framework,
this repository includes our in-house developed
crash consistency checker (SymC3),
with which 11 new crash consistency bugs
were revealed from `ext4`, `Btrfs`, `F2FS`,
and from two verified file systems: `FSCQ` and `Yxv6`.

This repository is provided under the terms of MIT license.


## Contents

* General code base
  - src/combined: Hydra input mutator
  - src/lkl/tools/lkl/{FS}-combined-consistency: Hydra LibOS-based Executor
    (will be downloaded and compiled during setup)

* Checkers
  - src/emulator: Hydra's in-house crash consistency checker, SymC3


## Setup

### 1. All setup should be done under `src`
```
$ cd src
```

### 2. Install dependencies
```
./dep.sh
```

### 3. Compile for each file system
```
$ make build-btrfs-imgwrp
```

* We can do the same for other file systems:
```
$ make build-ext4-imgwrp
$ make build-f2fs-imgwrp
$ make build-xfs-imgwrp
```

* (Skip if you want to test the latest kernel) To reproduce bugs presented in the SOSP'19 paper, do the following to back-port LKL to kernel 4.16.
```
$ cd lkl (pwd: proj_root/src/lkl) # assuming that you are in the src directory
$ make mrproper
$ git pull
$ git checkout v4.16-backport
$ ./compile -t btrfs
$ cd .. (pwd: proj_root/src)
```

### 4. Set up environments
```
$ sudo ./prepare_fuzzing.sh
$ ./prepare_env.sh
```

### 5. Run fuzzing (single / multiple instance)

* Single instance

```
$ ./run.py -t [fstype] -c [cpu_id] -l [tmpfs_id] -g [fuzz_group]

-t: choose from btrfs, f2fs, ext4, xfs
-c: cpu id to run this fuzzer instance
-l: tmpfs id to store logs (choose one from /tmp/mosbench/tmpfs-separate/)
-g: specify group id for parallel fuzzing, default: 0

e.g., ./run.py -t btrfs -c 4 -l 10 -g 1
Runs btrfs fuzzer, and pins the instance to Core #4.
Logs will be accumulated under /tmp/mosbench/tmpfs-separate/10/log/ .
```

* You can also run multiple fuzzers in parallel by doing:
```
[Terminal 1] ./run.py -t btrfs -c 1 -l 10 -g 1
[Terminal 2] ./run.py -t btrfs -c 2 -l 10 -g 1
[Terminal 3] ./run.py -t btrfs -c 3 -l 10 -g 1
[Terminal 4] ./run.py -t btrfs -c 4 -l 10 -g 1
// all btrfs bug logs will be under /tmp/mosbench/tmpfs-separate/10/log/

[Terminal 5] ./run.py -t f2fs -c 5 -l 11 -g 2
[Terminal 6] ./run.py -t f2fs -c 6 -l 11 -g 2
[Terminal 7] ./run.py -t f2fs -c 7 -l 11 -g 2
[Terminal 8] ./run.py -t f2fs -c 8 -l 11 -g 2
// all f2fs bug logs will be under /tmp/mosbench/tmpfs-separate/11/log/
```

### 6. Important note

It is highly encouraged that you use separate input, output, log directories for each file system, unless you are running fuzzers in parallel. If you reuse the same directories from previous testings of other file systems, it won't work properly.

### 7. Experiments

Please refer to EXPERIMENTS.md for detailed experiment information.


## Contacts

* Seulbae Kim (seulbae@gatech.edu)
* Meng Xu (meng.xu@gatech.edu)
* Sanidhya Kashyap (sanidhya@gatech.edu)
* Jungyeon Yoon (jungyeon@gatech.edu)
* Wen Xu (wen.xu@gatech.edu)
* Taesoo Kim (taesoo@gatech.edu)

