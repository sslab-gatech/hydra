#!/usr/bin/python
# -*- coding: utf-8 -*-

# SPDX-License-Identifier: MIT

import os
import sys
import tempfile
import shutil
import errno
import argparse
import subprocess
import copy
import binascii
import hashlib

import common as c
import syscalls
import utils
import checker


def emulate(syscall, argv, ret):
    if syscall == "SYS_mkdir":
        syscalls.mkdir(argv)
    elif syscall == "SYS_rmdir":
        syscalls.rmdir(argv)
    elif syscall == "SYS_rename":
        syscalls.rename(argv)
    elif syscall == "SYS_truncate":
        syscalls.truncate(argv)
    elif syscall == "SYS_ftruncate":
        syscalls.ftruncate(argv)
    elif syscall == "SYS_link":
        syscalls.link(argv)
    elif syscall == "SYS_symlink":
        syscalls.symlink(argv)
    elif syscall == "SYS_unlink":
        syscalls.unlink(argv)
    elif syscall == "SYS_fsync":
        syscalls.fsync(argv)
    elif syscall == "SYS_sync":
        syscalls.sync()
    elif syscall == "SYS_fdatasync":
        syscalls.fdatasync(argv)
    elif syscall == "SYS_syncfs":
        syscalls.syncfs(argv)
    elif syscall == "SYS_open":
        syscalls.open(argv, ret)
    elif syscall == "SYS_write":
        syscalls.write(argv)
    elif syscall == "SYS_pwrite64":
        syscalls.pwrite64(argv)
    elif syscall == "SYS_read":
        syscalls.read(argv)
    elif syscall == "SYS_pread64":
        syscalls.pread64(argv)
    elif syscall == "SYS_lseek":
        syscalls.lseek(argv)
    elif syscall == "SYS_fallocate":
        syscalls.fallocate(argv)
    elif syscall == "SYS_utimes":
        syscalls.utimes(argv)
    elif syscall == "SYS_readlink":
        syscalls.readlink(argv)
    elif syscall == "SYS_chmod":
        syscalls.chmod(argv)
    elif syscall == "SYS_setxattr":
        syscalls.setxattr(argv)
    elif syscall == "SYS_removexattr":
        syscalls.removexattr(argv)
    elif syscall == "SYS_listxattr":
        syscalls.listxattr(argv)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", dest="img", required=True)
    parser.add_argument("-t", dest="type", required=True)
    parser.add_argument("-p", dest="prog", required=True)
    parser.add_argument("-c", dest="crashed")
    parser.add_argument("-r", dest="regtest", action="store_true")
    parser.add_argument("-v", "--verbose", action="store_true")
    parser.add_argument("-m", "--monochrome", action="store_true")
    parser.add_argument("-d", "--showdata", action="store_true")
    parser.add_argument("-f", "--usefifo", action="store_true")

    args = parser.parse_args()

    if args.verbose:
        c.verbose = 1
    if args.monochrome:
        c.color_yellow = ""
        c.color_green = ""
        c.color_red = ""
        c.color_blue = ""
        c.color_bold = ""
        c.color_end = ""
    if args.regtest:
        c.REGTEST = 1
    if args.monochrome:
        c.MONO = 1
    if args.showdata:
        c.SHOWDATA = 1

    if c.verbose:
        print "ARGS:", args

    image = args.img
    prog = args.prog
    if args.crashed is None: # for easier manual verification
        crash_state = prog.replace(".c", ".rec")
    else:
        crash_state = args.crashed

    t = args.type.lower()
    if t == "ext4":
        c.FSTYPE = c.EXT4
    elif t == "btrfs":
        c.FSTYPE = c.BTRFS
    elif t == "f2fs":
        c.FSTYPE = c.F2FS
    elif t == "xfs":
        c.FSTYPE = c.XFS
    elif t == "vfat":
        c.FSTYPE = c.VFAT
    elif t == "hfsplus":
        c.FSTYPE = c.HFSPLUS
    elif t == "gfs2":
        c.FSTYPE = c.GFS2
    elif t == "reiserfs":
        c.FSTYPE = c.REISERFS

    if c.verbose:
        print "===== MOUNT & RETRIEVE METADATA FROM CRASHED IMG ====="
        print "crashed:", crash_state
    tmpmetadata = []
    fp_imginfo = open(crash_state, "r")
    tmpmetadata = fp_imginfo.readlines()
    fp_imginfo.close()
    METADATA = []

    for entry in tmpmetadata:
        es = entry.rstrip("\n").split("\t")
        if len(es) > c.NUMITEM:
            es[c.IDX_XATTR] += "\x09" + "\x09".join(es[c.NUMITEM:])
            es = es[:c.NUMITEM]
        elif len(es) < c.NUMITEM:
            es[0] = "\x09".join(es)
            es = [es[0]]
        METADATA.append(es)

    for ei, entry in enumerate(METADATA):
        if len(entry) < c.NUMITEM:
            offset = 0
            cur_idx = ei
            while len(METADATA[cur_idx-offset]) < c.NUMITEM:
                offset += 1
            METADATA[cur_idx-offset][-1] += "\x0a" + "\x0a".join(entry)
            METADATA[cur_idx] = [""]


    for i in xrange(METADATA.count([""])):
        METADATA.remove([""])

    if c.verbose:
        print "====================== METADATA ======================"
        for entry in METADATA:
            print entry

    if c.verbose:
        print "===== PARSING prog ====="
    if args.usefifo:
        utils.init_img(fifofile=True)
    else:
        utils.init_img(fifofile=False)

    if not args.regtest:
        c.FP_LOG = open(crash_state, "a")
        c.FP_LOG.write("=====\n")


    # prog can be either
    # 1. c source code when testing
    # 2. serialized file (cur_input) when emulater is called by fuzzer
    with open(prog, "r") as fp:
        program_lines = fp.readlines()
    if len(program_lines) == 0:
        exit(-1)
    program = "".join(program_lines)
    # for case 1, just use source as is
    if not program_lines[0].startswith("#include"): # for case 2, deserialize and use
        path_this = os.path.dirname(os.path.realpath(__file__))
        program_show = os.path.join(path_this, "..", "combined", "program_show")
        program = subprocess.check_output([program_show, prog])
        program_lines = program.split("\n")

    if not args.regtest:
        c.FP_LOG.write(program + "\n=====\n") # write program to shared tmp file

    parse = 0
    for line in program_lines:
        if line.strip() == "{":
            parse = 1
            continue
        if line.strip() == "return 0;":
            parse = 0
            break
        if parse:
            utils.parse_wrapper(line)

    if c.verbose:
        print "===== EMULATING OPERATIONS ====="
        print "VARS:", c.VAR_STACK
        print "FD:", c.FD_STACK
        print "BUF_size:", c.BUF_SIZE
        if args.showdata:
            print "BUF_data:", c.BUF_DATA

    for si, syscall in enumerate(c.SYSCALL_STACK):
        # print "MEM:", c.MEM
        # print "DISK:", c.DISK
        #print "\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
        #print c.DENTRY
        #print syscall
        syscall_name = syscall[0]
        syscall_argv = syscall[1]
        syscall_ret = syscall[2]
        if c.verbose:
            print "\n", si, c.color_yellow + str(syscall) + c.color_end
        emulate(syscall_name, syscall_argv, syscall_ret)
        if c.verbose:
            c.FT.display(c.ROOT)
            print "MEM :", c.MEM.keys()
            print "DISK:", c.DISK.keys()


    if c.verbose:
        print "===== RESULT ====="

    # create ipath-iid map, where path is resolved
    imap_resolved = {}
    for id in c.DISK:
        namelist = c.DISK[id].name
        if len(namelist) == 1:
            name = namelist[0]
            for path in c.DENTRY:
                if name in path:
                    if c.DENTRY[path] == id:
                        imap_resolved[path] = id
        else:
            for name in namelist:
                for path in c.DENTRY:
                    if name in path:
                        if c.DENTRY[path] == id:
                            imap_resolved[path] = id

    from collections import OrderedDict
    sd = OrderedDict(sorted(imap_resolved.items(), key=lambda x: x[1]))

    list_inum_ondisk = []
    if c.verbose:
        print "===== TREE ====="
        c.FT.display(c.ROOT)
    if c.verbose:
        print ""
        print "===== MUST EXIST ====="
    for path in sd:
        inode = c.DISK[sd[path]]
        list_inum_ondisk.append(inode.id)
        if c.verbose:
            utils._show_inode_info(inode)
    list_inum_ondisk = list(set(list_inum_ondisk))

    if c.verbose:
        print "===== CHECK SYNC'ED FILES ====="

    META_MUT = copy.deepcopy(METADATA)
    if c.FSTYPE == c.BTRFS or c.FSTYPE == c.XFS:
        # Btrfs: use normal strategy (using inum as identifier)
        checker.test_fs_inum(list_inum_ondisk, META_MUT, sd)
    else:
        # ext4, f2fs
        checker.test_fs_history(list_inum_ondisk, META_MUT, sd)

    if c.verbose:
        print "===== ADDITIONAL CHECK ====="
    META_MUT = copy.deepcopy(METADATA)
    checker.test_self_consistency(META_MUT)

    if not args.regtest:
        c.FP_LOG.close()

    if c.verbose:
        print "===== UNSYNC'ED ====="
        for id in c.MEM:
            inode = c.MEM[id]
            utils._show_inode_info(inode)

