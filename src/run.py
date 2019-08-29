#!/usr/bin/python

import os
import sys
import argparse

tmpfs_base = "/tmp/mosbench/tmpfs-separate"
FSLIST = ["btrfs", "f2fs", "ext4", "xfs"]

def usage():
    print "USAGE: ./run.py -t [fstype] -c [cpu_id] -l [log_cpu_id]"

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-t', dest='fstype')
    parser.add_argument('-c', dest='cpu_id')
    parser.add_argument('-l', dest='log_id')

    args = parser.parse_args()

    fail = 0
    if args.fstype is None:
        print "specify fs type by -t [fsname]"
        fail = 1
    elif args.fstype not in FSLIST:
        print "you can test {0}".format(str(FSLIST))
        fail = 1
    elif args.cpu_id is None:
        print "specify core id to run fuzzing instance on by -c [cpu_id]"
        fail = 1
    elif args.log_id is None:
        print "specify core for tmpfs to store log in by -l [log_id]"
        fail = 1
    if fail:
        usage()
        sys.exit(1)

    in_dir  = "in-{0}".format(args.fstype)
    out_dir = "out-{0}".format(args.fstype)
    log_base = os.path.join(tmpfs_base, args.log_id)
    log_dir = os.path.join(log_base, "log")
    tmp_dir = os.path.join(log_base, "tmp")
    try:
        os.mkdir(tmp_dir)
    except OSError:
        print "{0} already exists".format(tmp_dir)
    try:
        os.mkdir(log_dir)
    except OSError:
        print "{0} already exists".format(log_dir)
    try:
        os.mkdir(in_dir)
    except OSError:
        print "{0} already exists".format(in_dir)
    try:
        os.mkdir(out_dir)
    except OSError:
        print "{0} already exists".format(out_dir)

    instance_name = "fuzzer_{0}-cpu{1}log{2}".format(args.fstype, args.cpu_id, args.log_id)
    shm_name = "shm_{0}-{1}".format(args.fstype, args.cpu_id)

    cmd = 'AFL_SKIP_BIN_CHECK=1 ./combined/afl-image-syscall/afl-fuzz -S {0} -b {1} -s fs/{2}/{2}_wrapper.so -e samples/oracle/{2}-10.image -y seed -i {3} -o {4} -u {5} -- lkl/tools/lkl/{2}-combined-consistency -t {2} -i samples/oracle/{2}-10.image -e emulator/emulator.py -l /tmp/mosbench/tmpfs-separate/{6}/log -d "/tmp/mosbench/tmpfs-separate/{6}/" -r -p @@'.format(instance_name, shm_name, args.fstype, in_dir, out_dir, args.cpu_id, args.log_id)

    os.system(cmd)

