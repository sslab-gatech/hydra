# Hydra - SOSP19 Artifact

## Contents

* General code base
  - src/combined: Hydra input mutator
  - src/lkl/tools/lkl/{FS}-combined-consistency: Hydra LibOS-based Executor
    (will be downloaded and compiled during setup)

* Checkers
  - src/emulator: Hydra's in-house crash consistency checker, SymC3


## Setup

1. All setup should be done under `src`
```
$ cd src
```

2. Install dependencies
```
./dep.sh
```

3. Run make for each file system
```
$ make build-btrfs-imgwrp
$ make build-ext4-imgwrp
$ make build-f2fs-imgwrp
$ make build-xfs-imgwrp
```

* To reproduce bugs presented in the SOSP'19 paper, do the following:
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

4. Set up environments
```
$ sudo ./prepare_fuzzing.sh
$ ./prepare_env.sh
```

5. Run fuzzing (single instance)
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

You can also run multiple fuzzers in parallel by doing:
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

6. Important note

It is highly encouraged that you use separate input, output, log directories for each file system, unless you are running fuzzers in parallel. If you reuse the same directories from previous testings of other file systems, it won't work properly.

7. Experiments

Please refer to EXPERIMENTS.md for detailed experiment information.

