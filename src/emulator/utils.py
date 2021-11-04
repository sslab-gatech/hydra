# -*- coding: utf-8 -*-

# SPDX-License-Identifier: MIT

import common as c
import re


def init_img(fifofile=True):
    """
    For compatibility with JANUS..
    Call this method if we have to run fuzzer
    with the sample image (samples/oracle/btrfs-00.image)
    e.g., ./combined/afl-syscall/afl-fuzz
          -S slave1 -i i3 -o o3 -u 11 -m none
          -- lkl/tools/lkl/combined-consistency
          -t btrfs
          -i samples/oracle/btrfs-00.image
          -e emulator/emulator.py
          -p @@
    """
    if c.verbose:
        print "Initialize base img"

    inode = c.Inode(id=0, name=".", type=c.DIR, mode=0755, size=6)
    c.DISK[0] = inode
    c.DENTRY["."] = 0

    if c.FSTYPE == c.BTRFS:
        inum_start = 257
    else:
        inum_start = 4

    inum_foo   = inum_start
    inum_bar   = inum_start + 1
    inum_baz   = inum_start + 2
    inum_hln   = inum_baz
    inum_xattr = inum_baz + 1
    inum_acl   = inum_baz + 2
    inum_aoa   = inum_baz + 3
    inum_fifo  = inum_baz + 4
    inum_sln   = inum_baz + 5

    inode = c.Inode(id=inum_foo, name="foo", type=c.DIR, mode=0755, size=6)
    c.DISK[inode.id] = inode
    c.DENTRY["./foo"] = inode.id
    tup = (inode.id, "foo")
    tup_parent = (0, ".")
    c.DISK[0].add_child(tup)
    c.DISK[0].persist_children()
    c.FT.add_node(tup, tup_parent)

    inode = c.Inode(id=inum_bar, name="bar", type=c.DIR, mode=0755, size=54)
    c.DISK[inode.id] = inode
    c.DENTRY["./foo/bar"] = inode.id
    tup = (inode.id, "bar")
    tup_parent = (inum_foo, "foo")
    c.DISK[inum_foo].add_child(tup)
    c.DISK[inum_foo].persist_children()
    c.FT.add_node(tup, tup_parent)

    inode = c.Inode(id=inum_baz, name="baz", type=c.FILE, mode=0644, size=12)
    inode.datablock.data = "hello world\n"
    c.DISK[inode.id] = inode
    c.DENTRY["./foo/bar/baz"] = inode.id
    tup = (inode.id, "baz")
    tup_parent = (inum_bar, "bar")
    c.DISK[inum_bar].add_child(tup)
    c.FT.add_node(tup, tup_parent)

    if c.FSTYPE != c.VFAT:
        c.DISK[inode.id].add_hardlink("hln")
        c.DENTRY["./foo/bar/hln"] = inode.id
        tup = (inode.id, "hln")
        tup_parent = (inum_bar, "bar")
        c.DISK[inum_bar].add_child(tup)
        c.FT.add_node(tup, tup_parent)

    inode = c.Inode(id=inum_xattr, name="xattr", type=c.FILE, mode=0644, size=0)
    inode.xattr["user.mime_type"] = "text/plain"
    c.DISK[inode.id] = inode
    inode.datablock.data = ""
    c.DENTRY["./foo/bar/xattr"] = inode.id
    tup = (inode.id, "xattr")
    tup_parent = (inum_bar, "bar")
    c.DISK[inum_bar].add_child(tup)
    c.FT.add_node(tup, tup_parent)

    inode = c.Inode(id=inum_acl, name="acl", type=c.FILE, mode=0644, size=0)
    inode.xattr["system.posix_acl_access"] = "\x02"
    c.DISK[inode.id] = inode
    inode.datablock.data = ""
    c.DENTRY["./foo/bar/acl"] = inode.id
    tup = (inode.id, "acl")
    tup_parent = (inum_bar, "bar")
    c.DISK[inum_bar].add_child(tup)
    c.FT.add_node(tup, tup_parent)

    inode = c.Inode(id=inum_aoa, name="æøå", type=c.FILE, mode=0644, size=4)
    c.DISK[inode.id] = inode
    inode.datablock.data = "xyz\n"
    c.DENTRY["./foo/bar/æøå"] = inode.id
    tup = (inode.id, "æøå")
    tup_parent = (inum_bar, "bar")
    c.DISK[inum_bar].add_child(tup)
    c.FT.add_node(tup, tup_parent)

    if fifofile:
        inode = c.Inode(id=inum_fifo, name="fifo", type=c.FIFO, mode=0644, size=0)
        c.DISK[inode.id] = inode
        c.DENTRY["./foo/bar/fifo"] = inode.id
        tup = (inode.id, "fifo")
        tup_parent = (inum_bar, "bar")
        c.DISK[inum_bar].add_child(tup)
        c.FT.add_node(tup, tup_parent)
    else:
        if c.FSTYPE == c.BTRFS:
            inum_fifo = 265
        inode = c.Inode(id=inum_fifo, name="fifo", type=c.FILE, mode=0644, size=0)
        c.DISK[inode.id] = inode
        c.DENTRY["./foo/bar/fifo"] = inode.id
        tup = (inode.id, "fifo")
        tup_parent = (inum_bar, "bar")
        c.DISK[inum_bar].add_child(tup)
        c.FT.add_node(tup, tup_parent)

    if c.FSTYPE != c.VFAT:
        inode = c.Inode(id=inum_sln, name="sln", type=c.SYMLINK, mode=0777, size=15)
        c.DISK[inode.id] = inode
        inode.target = "mnt/foo/bar/baz"
        c.DENTRY["./foo/bar/sln"] = inode.id
        tup = (inode.id, "sln")
        tup_parent = (inum_bar, "bar")
        c.DISK[inum_bar].add_child(tup)
        c.DISK[inum_bar].persist_children()
        c.FT.add_node(tup, tup_parent)

    c.INODE_CNT = inum_sln
    c.INODE_CNT += 1
    if c.FSTYPE == c.BTRFS and not fifofile:
        c.INODE_CNT = 266

    # print c.DISK
    # for iid in c.DISK:
    #     print iid, c.DISK[iid].name, c.DISK[iid].children
    if c.verbose:
        print c.DENTRY
        c.FT.display((0, "."))


def init_mutated_img(statfile):
    inode = c.Inode(id=0, name=".", type=c.DIR, mode=0755, size=6)
    c.DISK[0] = inode
    c.DENTRY["."] = 0

    fp_imgstat = open(statfile, "r")
    tmpstat = fp_imgstat.readlines()
    fp_imgstat.close()
    STAT = []

    # redundant (emulator.py:142-162)
    for entry in tmpstat:
        es = entry.rstrip("\n").split("\t")
        if len(es) > c.NUMITEM:
            es[c.IDX_XATTR] += "\x09" + "\x09".join(es[c.NUMITEM:])
            es = es[:c.NUMITEM]
        elif len(es) < c.NUMITEM:
            es[0] = "\x09".join(es)
            es = [es[0]]
        STAT.append(es)

    for ei, entry in enumerate(STAT):
        if len(entry) < c.NUMITEM:
            offset = 0
            cur_idx = ei
            while len(STAT[cur_idx-offset]) < c.NUMITEM:
                offset += 1
            STAT[cur_idx-offset][-1] += "\x0a" + "\x0a".join(entry)
            STAT[cur_idx] = [""]

    for i in xrange(STAT.count([""])):
        STAT.remove([""])

    # initialize data structure using STAT info!
    for entry in STAT:
        print entry


p_var_str = re.compile("(v[0-9]+)\[\].*\"(.*)\"")
p_var_fd = re.compile("(v[0-9]+);")
p_var_buf = re.compile("(v[0-9]+)\[([0-9]+)\]")
p_arg = re.compile("\((.*)\)")


def parse_var_assignment(line):
    # print "var:", line
    m_var = p_var_str.search(line)
    var_name = m_var.group(1)
    var_value = m_var.group(2)
    c.VAR_STACK[var_name] = var_value


def parse_buf(line):
    # print "buf:", line
    m_var = p_var_buf.search(line)
    var_name = m_var.group(1)
    var_size = m_var.group(2)
    c.BUF_SIZE[var_name] = var_size
    c.BUF_DATA[var_name] = "\x00" * int(var_size)


def parse_fd(line):
    # print "fd:", line
    m_var = p_var_fd.search(line)
    var_name = m_var.group(1)
    c.FD_STACK[var_name] = ""


def parse_memcpy(line):
    in_par = line[line.find("(")+1:line.rfind(")")]
    in_par_s = in_par.split(", ")
    var_name = in_par_s[0]
    var_size = int(in_par_s[-1])
    var_string = in_par[in_par.find("\"")+1:in_par.rfind("\"")]

    """
    We need a special way of interpreting the string,
    because Program::deserialize() does not care about how the result string
    should be escaped. When parsing, only the non-printable bytes
    (\x00~\x1f and \x7f~\xff) should be escaped.
    """
    # pat_escape = re.compile(r"\\x[0-9a-f]{2}")
    pat_escape = re.compile(r"\\x[0189a-f][0-9a-f]|\\x7f")
    esclist = pat_escape.findall(var_string)
    for esc in esclist:
        var_string = var_string.replace(esc, esc.decode("unicode-escape"))

    c.BUF_DATA[var_name] = var_string[:var_size]


def parse_call(line):
    # print "call:", line
    m_arg = p_arg.search(line)
    arglist = m_arg.group(1).split(", ")
    syscall_name = arglist[0]
    syscall_args = arglist[1:]
    if syscall_name == "SYS_open":
        syscall_ret = line.split(" = ")[0]
    else:
        syscall_ret = ""
    c.SYSCALL_STACK.append([syscall_name, syscall_args, syscall_ret])


def parse_wrapper(line):
    line = line.strip()
    if c.verbose:
        if c.SHOWDATA:
            print line
        elif "memcpy" not in line:
            print line
        elif "memcpy" in line:
            print line.split(",")[0] + ", \"DATA (-d to see)\");"
    if "syscall" in line:
        parse_call(line)
    elif line.endswith(";"):
        if "=" in line and "{ 0, }" not in line and "memcpy" not in line:
            parse_var_assignment(line)
        elif "memcpy" in line:
            parse_memcpy(line)
        elif "[" and "]" in line:
            parse_buf(line)
        elif "v" in line and "close" not in line:
            parse_fd(line)


def _pretty_print_data(data):
    ni = []
    di = []
    ddict = {}
    isnull = 0
    for bi, byte in enumerate(data):
        if bi == 0:
            if byte != "\x00":
                di.append(0)
        if byte == "\x00":
            if not isnull:
                isnull = 1
                ni.append(bi)
                try:
                    ddict[di[-1]] = bi - di[-1]
                except IndexError:
                    pass
        else:
            if isnull:
                isnull = 0
                di.append(bi)
                try:
                    ddict[ni[-1]] = bi - ni[-1]
                except IndexError:
                    pass
        if bi == len(data)-1:
            if isnull:
                try:
                    ddict[ni[-1]] = bi - ni[-1] + 1
                except IndexError:
                    ddict[ni[0]] = bi
            else:
                try:
                    ddict[di[-1]] = bi - di[-1] + 1
                except IndexError:
                    ddict[di[0]] = bi

    for key in sorted(ddict):
        if key in ni:
            print "\\x00*{0}".format(ddict[key]),
        else:
            print data[key:key+ddict[key]],
    print ""


def _show_inode_info(inode):
    print "inode id:", inode.id
    print "- name  :", inode.name
    print "- hist  :", inode.u_name
    if inode.type == c.FILE:
        typestr = "Regular file"
    elif inode.type == c.DIR:
        typestr = "Directory"
    elif inode.type == c.SYMLINK:
        typestr = "Symbolic link"
    elif inode.type == c.FIFO:
        typestr = "FIFO file"
    print "- type  :", typestr
    print "- isize :", inode.size
    print "- link  :", inode.linkcnt
    print "- mode  :", oct(inode.mode)
    if inode.xattr:
        print "- xattr :", inode.xattr
    if inode.type == c.FILE:
        print "- blocks: {0}".format(inode.numblk)
        print "- data  : {0} bytes".format(len(inode.datablock.data))
        if c.SHOWDATA:
            _pretty_print_data(inode.datablock.data)

    elif inode.type == c.DIR:
        try:
            print "- child :", c.FT[(inode.id, inode.name[0])][0].children
            print "- old_child :", c.FT[(inode.id, inode.name[0])][0].old_children
        except:
            print "(removed)"
    elif inode.type == c.SYMLINK:
        print "- symlink target :", inode.target
    print ""

