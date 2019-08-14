# Hydra - SOSP19 Artifact

1. Contents
* General code base
  - src/combined: Hydra input mutator
  - src/lkl/tools/lkl/{FS}-combined-consistency: Hydra LibOS-based Executor
    (will be downloaded and compiled during setup)

* Checkers
  - src/emulator: Hydra's in-house crash consistency checker, SymC3


2. Setup

All setup should be done under `src`
```
$ cd src
```

Install dependencies
```
./dep.sh
```

Run make for each file system
```
$ make build-btrfs-imgwrp
$ make build-ext4-imgwrp
$ make build-f2fs-imgwrp
$ make build-xfs-imgwrp
```

Set up environments
```
$ sudo ./prepare_fuzzing.sh
$ ./prepare_env.sh
```

Create log directory where crash consistency bug reports will be stored.
It is highly recommended that you use tmpfs.
```
$ mkdir /tmp/mosbench/tmpfs-separate/4/log  // use free core (4 in this case)
```

Run fuzzing!
```
$ AFL_SKIP_BIN_CHECK=1 ./combined/afl-image-syscall/afl-fuzz -S fuzzer -b btrfs1 -s fs/btrfs/btrfs_wrapper.so -e samples/oracle/btrfs-10.image -y seed -i in -o out -u 10 -- lkl/tools/lkl/btrfs-combined-consistency -t btrfs -i samples/oracle/btrfs-10.image -e emulator/emulator.py -l /tmp/mostbench/tmpfs-separate/4/log -d "/tmp/mosbench/tmpfs-separate/4/" -r -p @@
```

AFL args:
```
-S fuzzer00                      : Run AFL in slave mode
-b btrfs1                        : Name of fuzzer instance and shared memory buffer
-s fs/btrfs/btrfs_wrapper.so     : fs-specific parser (try ext4/f2fs/xfs)
-e samples/oracle/btrfs-10.image : Seed image file. Use {FS}-10.image for crash consistency testing, and {FS}-00.image for other bug types.
-y seed                          : Seed program (i.e., sequence of system calls) directory.
                                   You can create seed programs by running
                                   `$ ./combined/create_corpus_consistency istat/btrfs-10.istat seed`
-i in                            : Input directory (AFL will fill it up)
-o out                           : Output directory (Regular AFL output will be stored as is)
-u 10                            : CPU id to run this fuzzer instance
```

LKL emulator args:
```
-t btrfs
-i samples/oracle/btrfs-10.image      : Base image file. Should be the same as the -e arg for AFL
-e emulator/emulator.py               : SymC3 emulator path
-l /tmp/mosbench/tmpfs-separate/4/log : Log directory to save consistency bug reports
-d "/tmp/mosbench/tmpfs-separate/4"   : Set this if you are using tmpfs
-r                                    : (Optional) Set -r if you do not want to save crashed image file.
-p @@                                 : AFL sets @@ to the current test case
```

