#!/usr/bin/python

# SPDX-License-Identifier: MIT

import os
import string
import random
import subprocess
import stat
import binascii
import copy
import time
import sys
import argparse

import common as c
import utils
import syscalls
import checker
from emulator import emulate


def _visit(path, type, write=1):
    stat = os.stat(path)
    chksum = 0
    if type == c.FILE:
        with open(path, "r") as fp:
            content = fp.read()
        chksum = binascii.crc32(content) & 0xFFFFFFFF

    blksize = 0
    numblk = 0
    xattr = ""
    metastr = path + "\t" + str(type) + '\t' + str(stat.st_ino) + '\t'
    metastr += str(stat.st_nlink) + '\t' + str(stat.st_size) + '\t'
    metastr += str(blksize) + '\t' + str(numblk) + '\t'
    metastr += str(oct(stat.st_mode))[-3:] + '\t' + str(chksum) + '\t' + "" + '\t'
    metastr += xattr + '\n'

    if write and not c.REGTEST:
        c.FP_LOG.write(metastr)
    return [path, str(type), str(stat.st_ino), str(stat.st_nlink), str(stat.st_size), str(blksize), str(numblk), str(oct(stat.st_mode))[-3:], str(chksum), "", xattr]



def _traverse_crashed(mnt_point, write=1):
    METADATA = []
    os.chdir(mnt_point)
    for dirname, subdirlist, filelist in os.walk("."):
        METADATA.append(_visit(dirname, c.DIR, write))
        for fname in filelist:
            fpath = os.path.join(dirname, fname)
            METADATA.append(_visit(fpath, c.FILE, write))

    if write and not c.REGTEST:
        c.FP_LOG.write("\n=====\n")
    return METADATA


def _parse_program(prog):
    if prog.endswith(".c"):
        with open(prog, "r") as fp:
            program_lines = fp.readlines()
    else:
        program_show = os.path.join(path_this, "..", "combined", "program_show")
        program = subprocess.check_output([program_show, prog])
        program_lines = program.split("\n")

    if not c.REGTEST:
        c.FP_LOG.write(program + "\n=====\n")

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
        if 0:
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


def _read_rec_file(prog):
    with open(prog.replace(".c", ".rec"), "r") as fp:
        tmpmetadata = fp.readlines()

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

    return METADATA


def _init_fscq_img():
    inode = c.Inode(id=0, name=".", type=c.DIR, mode=0755, size=4096)
    c.DISK[0] = inode
    c.DENTRY["."] = 0

    inode = c.Inode(id=1, name="foo", type=c.DIR, mode=0755, size=4096)
    c.DISK[inode.id] = inode
    c.DENTRY["./foo"] = inode.id
    tup = (inode.id, "foo")
    tup_parent = (0, ".")
    c.DISK[0].add_child(tup)
    c.DISK[0].persist_children()
    c.FT.add_node(tup, tup_parent)

    inode = c.Inode(id=2, name="bar", type=c.DIR, mode=0755, size=4096)
    c.DISK[inode.id] = inode
    c.DENTRY["./foo/bar"] = inode.id
    tup = (inode.id, "bar")
    tup_parent = (1, "foo")
    c.DISK[1].add_child(tup)
    c.DISK[1].persist_children()
    c.FT.add_node(tup, tup_parent)

    inode = c.Inode(id=3, name="baz", type=c.FILE, mode=0777, size=12)
    inode.datablock.data = "hello world\n"
    c.DISK[inode.id] = inode
    c.DENTRY["./foo/bar/baz"] = inode.id
    tup = (inode.id, "baz")
    tup_parent = (2, "bar")
    c.DISK[2].add_child(tup)
    c.FT.add_node(tup, tup_parent)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", dest="prog")
    parser.add_argument("-d", dest="mnt_point")
    parser.add_argument("-r", dest="regtest", action="store_true")
    parser.add_argument("-v", dest="verbose", action="store_true")
    parser.add_argument("-m", "--monochrome", action="store_true")
    args = parser.parse_args()

    if args.verbose:
        c.verbose = 1
    if args.regtest:
        c.REGTEST = 1
    if args.prog:
        prog = args.prog # will be given by fscq-cc
    else:
        prog = "/tmp/prog-prev" # for offline testing
    if args.monochrome:
        c.color_yellow = ""
        c.color_green = ""
        c.color_red = ""
        c.color_blue = ""
        c.color_bold = ""
        c.color_end = ""
    if args.mnt_point:
        mnt_point = args.mnt_point # will be given by fscq-cc
    else:
        mnt_point = "/tmp/crashed" # for offline testing

    logdir = "/tmp/fscqlog"
    tmpname = "file" + ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(6))
    logname = os.path.join(logdir, tmpname)
    if c.verbose:
        print "===== FSCQ EMULATOR BEGINS ====="
        print "LOG:", logname

    c.FSTYPE = c.FSCQ
    if not args.regtest:
        c.FP_LOG = open(os.path.join(logdir, logname), "a")
    _init_fscq_img()

    path_this = os.path.dirname(os.path.realpath(__file__))
    if args.prog and not args.mnt_point:
        METADATA = _read_rec_file(args.prog)
    else:
        METADATA = _traverse_crashed(mnt_point)

    if not args.prog:
        os.chdir(path_this)

    if c.verbose:
        print "===== METADATA ====="
        for entry in METADATA:
            print entry
    _parse_program(prog)

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
    found = checker.test_fs_history(list_inum_ondisk, META_MUT, sd)

    if c.verbose:
        print "===== UNSYNC'ED ====="
        for id in c.MEM:
            inode = c.MEM[id]
            utils._show_inode_info(inode)

    if not args.regtest:
        c.FP_LOG.close()
    if not found:
        print "NO BUG FOUND"
        if not args.regtest:
            os.remove(logname)
    else:
        print "BUG!"
        # print "MAYBE BUG?"
        # # sometimes fscq just fails to properly show dirs and files
        # # if emulator reports a bug, we must re-traverse the mnt point
        # # few times and see if the result is consistent...
        # meta2 = _traverse_crashed(mnt_point, write=0)
        # time.sleep(0.5)
        # meta3 = _traverse_crashed(mnt_point, write=0)
        # time.sleep(0.5)
        # meta4 = _traverse_crashed(mnt_point, write=0)
        # time.sleep(0.5)
        # if len(METADATA) == len(meta2) == len(meta3) == len(meta4):
            # print "BUG!"
        # else:
            # print "NO BUG FOUND"
            # os.remove(logname)


