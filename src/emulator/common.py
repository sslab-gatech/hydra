#!/usr/bin/python
# -*- coding: utf-8 -*-

# SPDX-License-Identifier: MIT

import os
import common as c

(_ROOT, _DEPTH, _BREADTH) = range(3)

class Node:
    '''
    tup: (inode id, inode name)
    '''
    def __init__(self, tup, parent):
        self.__tup = tup
        self.__parent = parent
        self.__children = []
        self.__old_children = []

    @property
    def tup(self):
        return self.__tup

    @property
    def parent(self):
        return self.__parent

    @property
    def children(self):
        return self.__children

    @property
    def old_children(self):
        return self.__old_children

    def add_child(self, tup):
        self.children.append(tup)

    def remove_child(self, tup):
        self.children.remove(tup)
        self.old_children.append(tup)

    def get_rev_children(self):
        rc = self.children
        rc.reverse()
        return rc


class FileTree:
    def __init__(self):
        self.__nodes = {}

    @property
    def nodes(self):
        return self.__nodes

    def add_node(self, tup, parent=None):
        node = Node(tup, parent)
        try:
            node_list = self[tup]
        except KeyError:
            node_list = []
        node_list.append(node)
        # Map each key (tuple) to list of node instances
        # to handle cases like link(A/yyy, yyy) -> unlink(yyy)
        self[tup] = node_list
        # Each node instance can be differentiated by its parent's tup!

        if parent is not None:
            # self[parent] is a list of nodes
            for p_node in self[parent]:
                # find parent node instance which has the given parent tup
                if p_node.tup == parent:
                    p_node.add_child(tup) # add child tuple there
                    break

        return node

    def remove_node(self, tup, parent=None):
        # removes node from its parent's children
        # delete node object from self[tup] list
        if parent is not None:
            self[parent][0].remove_child(tup)

        for node in self[tup]:
            if node.parent == parent:
                self[tup].remove(node)

    def modify_node(self, tup_old, tup_new, parent_old=None, parent_new=None):
        # 1. parent_old's children list -> remove tup_old
        # 2. parent_new's children list -> add tup_new
        # 3. copy tup_old's children to tup_new's children
        # 4. change tup_old's children's parent to tup_new
        # 5. delete tup_old node object
        # 6. add new node object to tree

        if parent_old is not None: # 1
            self[parent_old][0].remove_child(tup_old)
        if parent_new is not None: # 2
            self[parent_new][0].add_child(tup_new)

        node_old = None
        for node in self[tup_old]:
            if node.parent == parent_old:
                node_old = node
                break

        node_new = Node(tup_new, parent_new)
        node_new.children = node_old.children # 3

        for child_tup in node_old.children: # 4
            child_nodelist = self[child_tup]
            cn = None
            for child_node in child_nodelist:
                if child_node.parent == tup_old:
                    cn = child_node
                    break
            cn.parent = tup_new

        self[tup_old].remove(node_old) #5

        try:
            node_list = self[tup_new]
        except KeyError:
            node_list = []
        node_list.append(node_new)
        self[tup_new] = node_list #6

        return node_new

    def display(self, tup, parent=None, depth=_ROOT):
        line = ""
        if depth != _ROOT:
            line = "   "*(depth-1) + "+" + "--"

        try:
            inode = MEM[tup[0]]
        except KeyError:
            inode = DISK[tup[0]]

        if inode.type == FILE:
            fgcolor = color_green
        elif inode.type == DIR:
            fgcolor = color_bold + color_blue
        elif inode.type == SYMLINK:
            fgcolor = color_red
        elif inode.type == FIFO:
            fgcolor = color_yellow

        line += "{0}: {1} {2}{3}{4}".format(
                tup[0],
                oct(inode.mode),
                fgcolor, tup[1], color_end)

        if inode.type == FILE:
            line += " ({0} bytes, offset: {1})".format(len(inode.datablock.data), str(inode.offset))
        elif inode.type == SYMLINK:
            line += " -> {0}".format(inode.target)

        print line

        depth += 1
        cur_tup_nodelist = self[tup]
        for node in cur_tup_nodelist:
            if node.parent == parent:
                children = node.children
                break
        for child in children:
            self.display(child, tup, depth)

    def traverse(self, tup, mode=_DEPTH):
        yield tup
        queue = self[tup].children
        while queue:
            yield queue[0]
            expansion = self[queue[0]].children
            if mode == _DEPTH:
                queue = expansion + queue[1:]
            elif mode == _BREADTH:
                queue = queue[1:] + expansion

    def get_path(self, start, goal, goaldir):
        nodes = []
        stack = [(start, [start])]
        visited = set()
        while stack:
            (vertex, path_tup_list) = stack.pop()
            if vertex not in visited:
                flag = 0
                if vertex == goal:
                    path_str = ""
                    for node in path_tup_list:
                        path_str = os.path.join(path_str, node[1])
                    if goaldir == ROOT or goaldir in visited:
                        return path_tup_list, path_str
                    else:
                        flag = 1
                if not flag:
                    visited.add(vertex)

                leafnode = self[vertex][0] # take node instance if unique
                if len(self[vertex]) > 1:  # if not, find one that has the correct parent
                    for node in self[vertex]:
                        if path_tup_list[-2] == node.parent:
                            leafnode = node
                            break
                for child in leafnode.get_rev_children():
                    stack.insert(0, (child, path_tup_list + [child]))
        return None, None

    def __getitem__(self, key):
        return self.__nodes[key]

    def __setitem__(self, key, item):
        self.__nodes[key] = item

    def __delitem__(self, key):
        self.__nodes.pop(key, None)


FT = FileTree()
ROOT = (0, ".")
FT.add_node(ROOT)

class Inode:
    id = None
    name = None
    u_name = None
    type = None
    mode = None
    u_mode = None
    uid = None
    u_uid = None
    gid = None
    g_gid = None
    size = None # in bytes
    u_size = None
    linkcnt = None
    u_linkcnt = None
    # ctime = None
    # mtime = None
    # atime = None
    # blocksize = 4096
    checksum = None
    u_checksum = None
    xattr = {}
    u_xattr = {}
    p_children = [] # persisted dirents
    children = []   # added/modified, but not persisted dirents
    d_children = [] # removed dirents
    removed = 0
    offset = {}
    datablock = None
    target = ""
    opt_dict = {}
    numblk = None
    # pending_ops = None


    def __init__(self, id, name, type, mode, size, linkcnt=1, uid=0, gid=0, numblk=0):
        global INODE_CNT
        self.id = id
        self.name = [name]
        self.u_name = []
        self.name_hist = []
        self.type = type
        self.mode = mode
        self.u_mode = []
        self.uid = uid
        self.u_uid = []
        self.gid = gid
        self.g_gid = []
        self.size = size
        self.u_size = []
        self.linkcnt = linkcnt
        self.xattr = {}
        self.u_xattr = {}
        self.p_children = []
        self.children = [] # only for directory
        self.d_children = []
        self.removed = 0
        self.offset = {}
        self.numblk = numblk
        if type == FILE:
            self.datablock = DataBlock("", "\x00"*65536)
            bs = size / BLKSIZE
            if size % BLKSIZE:
                bs += 1
            self.numblk = bs * BLKSIZE / 512
        elif type == SYMLINK:
            self.target = ""
        INODE_CNT += 1 # increment global inode counter whenever new inode is created

        if FSTYPE == FSCQ:
            self.unlinked_children = []

    def add_name(self, name):
        self.name.append(name)

    def remove_name(self, name_old, name_new=""):
        self.name.remove(name_old)
        self.u_name.append((name_old, name_new)) # store (old, new) tuple

    def add_child(self, (id, name)):
        tup_children = (id, name)
        self.children.append(tup_children)

    def remove_child(self, (id, name), unlink=0):
        tup_children = (id, name)
        try:
            self.children.remove(tup_children)
        except ValueError:
            self.p_children.remove(tup_children)
        self.d_children.append(tup_children)

        if FSTYPE == FSCQ and unlink:
            try:
                inode_child = c.MEM[tup_children[0]]
            except KeyError:
                inode_child = c.DISK[tup_children[0]]
            if inode_child.opt_dict != {}:
                # if child inode has a non-empty opt_dict, that's an open file
                self.unlinked_children.append(tup_children)


    def persist_children(self):
        # clear relevant rename history from children's u_name
        for child in self.children: # only for new names
            inum = child[0]
            name = child[1]
            try:
                inode = c.DISK[inum]
            except KeyError:
                inode = c.MEM[inum]
            for history in inode.u_name:
                old_name = history[0]
                new_name = history[1]
                if new_name == name:
                    inode.u_name.remove(history)
        for child in self.d_children: # only for removed names
            inum = child[0]
            name = child[1]
            try:
                inode = c.DISK[inum]
            except KeyError:
                inode = c.MEM[inum]
            for history in inode.u_name:
                old_name = history[0]
                new_name = history[1]
                if old_name == name:
                    inode.u_name.remove(history)

        # move tuples in children and d_children to p_children
        self.p_children.extend(self.children)
        self.p_children = list(set(self.p_children))
        self.children = []
        self.d_children = []

    def discard_children(self):
        self.p_children.extend(self.children)
        self.p_children = list(set(self.p_children))
        self.children = []
        self.d_children = []

    def add_hardlink(self, name):
        self.linkcnt += 1
        self.name.append(name)


class DataBlock():
    def __init__(self, data, hole):
        self.data = data
        self.hole = hole
        self.synced = 0


# GLOBAL VARIABLES
verbose = 0
FSTYPE = 0
SHOWDATA = 0
MONO = 0
REGTEST = 0
cnt_recursion = 0
MAX_SYMLINK = 40 # fs/namei.c:1166 (nd->total_link_count >= 40)
BLKSIZE = 4096 # default block size
UMASK = 022 # default umask value
FP_LOG = None
MAX_DFS_DEPTH = 50 # ad-hoc

# GLOBAL DATA STRUCTURES
VAR_STACK = {} # {filename_var_name: assigned_string_for_filename}
BUF_SIZE = {} # {buffer_var_name: size}
BUF_DATA = {}  # {buffer_var_name: actual_data}
FD_STACK = {}  # {fd_var_name: ""}, becomes {fd_var_name: path} upon open()
SYSCALL_STACK = []

# SERIALIZED METADATA INDEX INFO
IDX_PATH = 0
IDX_FTYPE = 1
IDX_INUM = 2
IDX_NLINK = 3
IDX_SIZE = 4
IDX_BLKSIZE = 5
IDX_BLOCKS = 6
IDX_MODE = 7
IDX_DATACHKSUM = 8
IDX_SYMTARGET = 9
IDX_XATTR = 10
NUMITEM = IDX_XATTR + 1

# GLOBAL COLORS
color_yellow = "\033[33m"
color_green = "\033[92m"
color_red = "\033[31m"
color_blue = "\033[34m"
color_bold = "\033[1m"
color_end = "\033[0m"

# DENTRY maps path to inode id
DENTRY = {".": 0}

# MEM and DISK maps inode id to inode object
MEM = {}
DISK = {}

# Keeps track of unsynced inode
UNSYNCED = []

# INODE INFO
INODE_CNT     = 1    # root node always has inode id 0

# TYPES
FILE    = 0x01
DIR     = 0x02
SYMLINK = 0x03
FIFO    = 0x04
TYPESTR = [None, "regular file", "directory", "symbolic link", "fifo file"]

# FSTYPE
EXT4      = 0
BTRFS     = 1
F2FS      = 2
XFS       = 3
VFAT      = 4
HFSPLUS   = 5
GFS2      = 6
REISERFS  = 7
FSCQ      = 8
YXV6      = 9

# MODE
S_IRWXU =  0700
S_IRUSR =  0400
S_IWUSR =  0200
S_IXUSR =  0100
S_IWRXG =   070
S_IRGRP =   040
S_IWGRP =   020
S_IXGRP =   010
S_IRWXO =    07
S_IROTH =    04
S_IWOTH =    02
S_IXOTH =    01
S_ISUID = 04000
S_ISGID = 02000
S_ISVTX = 01000

# OPEN FLAGS
O_RDONLY    = 00000000
O_WRONLY    = 00000001
O_RDWR      = 00000002
O_ACCMODE   = 00000003 # non-standard
O_CREAT     = 00000100
O_EXCL      = 00000200
O_NOCTTY    = 00000400
O_TRUNC     = 00001000
O_APPEND    = 00002000
O_NONBLOCK  = 00004000
O_DSYNC     = 00010000
FASYNC      = 00020000
O_DIRECT    = 00040000
O_LARGEFILE = 00100000
O_DIRECTORY = 00200000
O_NOFOLLOW  = 00400000
O_NOATIME   = 01000000
O_CLOEXEC   = 02000000
O_SYNC      = 04000000
