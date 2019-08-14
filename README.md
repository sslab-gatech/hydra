1. Contents

2. Setup
```
$ cd src
```

```
// Install dependencies
./dep.sh
```

```
// Depending on the file system to test
$ make build-btrfs-imgwrp
$ make build-ext4-imgwrp
$ make build-f2fs-imgwrp
$ make build-xfs-imgwrp
```

```
// Setting environments
$ sudo ./prepare_fuzzing.sh
$ ./prepare_env.sh
```

```
// Create log directory under tmpfs
$ mkdir /tmp/mosbench/tmpfs-separate/4/log  // use free core (4 in this case)
```

```
// Run test (btrfs)
$ AFL_SKIP_BIN_CHECK=1 ./combined/afl-image-syscall/afl-fuzz -S fuzzer -b btrfs1 -s fs/btrfs/btrfs_wrapper.so -e samples/oracle/btrfs-10.image -y seed -i in -o out -u 10 -- lkl/tools/lkl/btrfs-combined-consistency -t btrfs -i samples/oracle/btrfs-10.image -e emulator/emulator.py -l /tmp/mostbench/tmpfs-separate/4/log -d "/tmp/mosbench/tmpfs-separate/4/" -r -p @@

```

