# SPDX-License-Identifier: MIT

import common as c
import re
import binascii
import copy

BTRFS_ERRSTR = ""

def _print_hex(s):
    s = "\\x" + "\\x".join("{:02x}".format(ord(c)) for c in s)
    p = ""
    for i, e in enumerate(s):
        p += e
        if (i+1) % 16 == 0:
            p += "  "
        if (i+1) % 64 == 0:
            print p
            p = ""
    if len(s) < 64:
        print p
    elif p != "":
        print p


def check_child_exist(inode, inumlist, ret_on_err=0, print_err=1):
    global BTRFS_ERRSTR
    children = c.FT[(inode.id, inode.name[0])][0].children

    for child in children:
        child_id = child[0]
        child_name = child[1]
        if child_id not in inumlist:
            if ret_on_err:
                return 1
            else:
                errstr = "*** [META] missing child {0} of dir {1} ({2})".format(
                        child_name, inode.name, inode.id)
                if print_err:
                    print errstr
                if not c.REGTEST:
                    c.FP_LOG.write(errstr + "\n")


def check_meta(inode, entry, ret_on_err=0, print_err=1):
    global BTRFS_ERRSTR

    # print inode.id, inode.name, inode.u_name, inode.mode, inode.size
    # lcnt
    if inode.type != c.DIR:
        if inode.linkcnt != int(entry[c.IDX_NLINK]):
            if ret_on_err:
                return 1
            else:
                errstr = "*** [META] Link count mismatch in {0} {1}: em {2} vs ex {3}".format(
                        c.TYPESTR[inode.type], str(inode.name), inode.linkcnt, int(entry[c.IDX_NLINK]))
                if print_err:
                    print errstr
                BTRFS_ERRSTR += errstr + "\n"
                if not c.REGTEST:
                    c.FP_LOG.write(errstr + "\n")
                if c.FSTYPE == c.FSCQ or c.FSTYPE == c.YXV6:
                    return 1

    # mode
    if not c.FSTYPE == c.FSCQ:
        if inode.mode & 0777 != int(entry[c.IDX_MODE], 8) & 0777:
            if ret_on_err:
                return 1
            else:
                errstr = "*** [META] Mode mismatch in {0} {1}: em {2} vs ex {3}".format(
                        c.TYPESTR[inode.type], str(inode.name), oct(inode.mode), oct(int(entry[c.IDX_MODE], 8)))
                if print_err:
                    print errstr
                BTRFS_ERRSTR += errstr + "\n"
                if not c.REGTEST:
                    c.FP_LOG.write(errstr + "\n")

    if inode.type == c.FILE:
        # size
        if inode.size != int(entry[c.IDX_SIZE]):
            if ret_on_err:
                return 1
            else:
                errstr = "*** [META] Size mismatch in {0} {1}: em {2} vs ex {3}".format(
                        c.TYPESTR[inode.type], str(inode.name), inode.size, entry[c.IDX_SIZE])
                if print_err:
                    print errstr
                BTRFS_ERRSTR += errstr + "\n"
                if not c.REGTEST:
                    c.FP_LOG.write(errstr + "\n")
                if c.FSTYPE == c.FSCQ or c.FSTYPE == c.YXV6:
                    return 1
        """
        # blocks
        if inode.numblk != int(entry[c.IDX_BLOCKS]):
            if ret_on_err:
                errstr = "*** [META] Num blocks mismatch in {0} {1}: em {2} vs ex {3}".format(
                        c.TYPESTR[inode.type], str(inode.name), inode.numblk, entry[c.IDX_BLOCKS])
                print errstr
                BTRFS_ERRSTR += errstr + "\n"
                if not c.REGTEST:
                    c.FP_LOG.write(errstr + "\n")
            else:
                return 1
        """

    if c.FSTYPE == c.EXT4: # XXX: temporarily skip xattr check for ext4
        return 0

    if c.FSTYPE == c.YXV6: # yxv6 does not support xattr
        return 0

    # xattr
    xattr_crashed = entry[c.IDX_XATTR]
    pat_xattr = re.compile("(\w+[.]\w+): {1}(.*?), {1}", re.DOTALL)
    if pat_xattr.match(xattr_crashed):
        found = pat_xattr.findall(xattr_crashed)
        for tup in found:
            if tup[0] not in inode.xattr:
                if ret_on_err:
                    return 1
                else:
                    errstr = "*** [META] Missing xattr {0} in {1} {2}".format(
                            tup[0], c.TYPESTR[inode.type], str(inode.name))
                    if print_err:
                        print errstr
                    BTRFS_ERRSTR += errstr + "\n"
                    if not c.REGTEST:
                        c.FP_LOG.write(errstr + "\n")
            else:
                emul_xattr_val = inode.xattr[tup[0]]
                crash_xattr_val = tup[1]
                len_eval = len(emul_xattr_val)
                len_cval = len(crash_xattr_val)
                errflag = 0
                if len_eval > len_cval:
                    idx = len_cval
                elif len_eval < len_cval:
                    idx = len_eval
                else:
                    idx = len_eval

                for i in xrange(idx):
                    if ord(emul_xattr_val[i]) != ord(crash_xattr_val[i]):
                        errflag = 1
                        break

                if errflag:
                    if ret_on_err:
                        return 1
                    else:
                        errstr = u"*** [META] Wrong value for xattr {0} in {1} {2}".format(
                                tup[0], c.TYPESTR[inode.type], str(inode.name))
                        if print_err:
                            print errstr
                        BTRFS_ERRSTR += errstr + "\n"
                        if not c.REGTEST:
                            c.FP_LOG.write(errstr + "\n")
                        if c.verbose:
                            print "emulated xattr:"
                            _print_hex(emul_xattr_val)
                            print "recovered xattr:"
                            _print_hex(crash_xattr_val)
    return 0


def check_symlink(inode, entry, ret_on_err=0, print_err=1):
    global BTRFS_ERRSTR

    symlink_crashed = entry[c.IDX_SYMTARGET]
    if inode.target != symlink_crashed and inode.target != "./" + symlink_crashed:
        if ret_on_err:
            return 1
        else:
            errstr = "*** [META] Symlink target mismatch in {0}: em {1} vs ex {2}".format(
                    str(inode.name), inode.target, entry[c.IDX_SYMTARGET])
            if print_err:
                print errstr
            BTRFS_ERRSTR += errstr + "\n"
            if not c.REGTEST:
                c.FP_LOG.write(errstr + "\n")
            if c.FSTYPE == c.FSCQ or c.FSTYPE == c.YXV6:
                return 1
    return 0


def check_data(inode, entry, ret_on_err=0, print_err=1):
    global BTRFS_ERRSTR

    if int(entry[c.IDX_FTYPE]) == c.FILE:
        return 0
    if inode.size > len(inode.datablock.data):
        effective_data = inode.datablock.data + "\x00"*(len(inode.datablock.data) - inode.size)
    else:
        effective_data = inode.datablock.data

    crc_emul = binascii.crc32(effective_data) & 0xFFFFFFFF
    datahex = map(lambda c: hex(ord(c)), effective_data)
    try:
        crc_crash = int(entry[c.IDX_DATACHKSUM])
    except IndexError:
        crc_crash = 0 # XXX: ONLY FOR TESTING
    if crc_emul != crc_crash:
        if ret_on_err:
            return 1
        else:
            errstr = "*** [DATA] Inconsistency in {0} {1}: em {2} vs ex {3}".format(
                    c.TYPESTR[inode.type], str(inode.name), crc_emul, crc_crash)
            if print_err:
                print errstr
            BTRFS_ERRSTR += errstr + "\n"
            if not c.REGTEST:
                c.FP_LOG.write(errstr + "\n")
            if c.FSTYPE == c.FSCQ or c.FSTYPE == c.YXV6:
                return 1
    return 0


def _create_path(tuplist):
    namelist = []
    for tup in tuplist:
        namelist.append(tup[1])
    return "/".join(namelist)


def _dfs_traverse(tup, pers, stack, entry_list, level=0):
    if level > c.MAX_DFS_DEPTH: # escape from unbound recursion
        return entry_list
    # perform dfs on all possible inodes
    stack.append(tup)
    stackcpy = copy.deepcopy(stack)

    inum = tup[0]
    try:
        inode = c.MEM[inum]
        meta = 0
    except KeyError:
        inode = c.DISK[inum]
        meta = 1

    if c.verbose:
        print "\t"*level, "digging", tup, pers, meta, stackcpy
    # pers: if current tuple's parent persisted it
    # meta: if current tuple's metadata has been persisted
    newlist = [stackcpy, pers, meta, inode]
    entry_list.append(newlist)

    children = inode.p_children + inode.children + inode.d_children

    for ci, ch in enumerate(children):
        if c.verbose:
            print "\t"*level, "ch {0}/{1}: {2}".format(ci+1, len(children), ch)
        if ch in inode.p_children:
            pers = 1
        else:
            pers = 0
        _dfs_traverse(ch, pers, stack, entry_list, level=level+1)
        try:
            stack.remove(ch)
        except ValueError:
            pass # can happen when escaping from unbound recursion

    return entry_list


def test_fs_history(list_inum_ondisk, METADATA, sd):
    """
    This test strategy works for the file systems that do not assign
    inode numbers in sequence (e.g., ext4, f2fs).
    """
    name_meta = []
    name_nometa = []
    noname_meta = []
    noname_nometa = []

    deps = []
    entry_list = []
    _dfs_traverse(c.ROOT, 0, [], entry_list)
    # entry: [pathlist, name_persisted?, meta_persisted?, inode_instance]
    # pathlist: [(inum1, name1), (inum2, name2), (inum3, name3)]
    # path = _create_path(pathlist)
    # -> name1/name2/name3
    for entry in entry_list:
        pathlist = entry[0]
        name_persisted = entry[1]
        meta_persisted = entry[2]
        if len(pathlist) == 1: # skip root inode
            continue
        cur_tup = pathlist[-1]
        inum = cur_tup[0]
        name = cur_tup[1]
        inode = entry[3]

        if name_persisted: # if name is persisted -> continue
            continue

        # if not persisted by its parent,
        # find all dependencies and add in list
        cur_path = _create_path(pathlist)
        if c.verbose:
            print "Finding dependencies of", inum, cur_path,  ":", inode.u_name
        for hist in inode.u_name:
            if hist[1] == name:
                idx = 1
            elif hist[0] == name:
                idx = 0
            else:
                continue

            hist_tup = (inum, hist[~idx])

            flag = 0
            for dep in deps:
                if cur_path in dep:
                    flag = 1
            if flag:
                continue

            # rename order matters
            if not idx:
                cur_path_dep_list = [(cur_path, inum)]
            else:
                cur_path_dep_list = []
            for e in entry_list:
                e_inum = e[0][-1][0]
                if e[0][-1] == hist_tup:
                    dep_path = _create_path(e[0])
                    cur_path_dep_list.append((dep_path, e_inum))
            if idx:
                cur_path_dep_list.append((cur_path, inum))

            deps.append(cur_path_dep_list)
        if inode.u_name == []:
            # special case (this path could exist or not)
            # nevermind. will handle this in later steps.
            pass

    for dep in deps:
        if deps.count(dep) > 1:
            deps.remove(dep)
    """
    # double check for duplicated dependency
    import itertools
    for dep in deps:
        perms = list(itertools.permutations(dep, len(dep)))
        for perm in perms[1:]:
            perm = list(perm)
            if perm in deps:
                deps.remove(perm)
    """
    if c.verbose:
        print "DEPS:", deps

    # only ONE of the directory entries in each list element should exist.
    # we're gonna check that first in the recovered image metadata
    # If it is confirmed that one path exists in the recovered metadata,
    # the other paths are put in abandon_list, and all states that include
    # these paths will be removed.
    abandon_list = []
    for dep in deps:
        for tup in dep:
            abandon_list.append(tup)
            for img_entry in METADATA:
                if img_entry[c.IDX_PATH] == tup[0]:# and img_entry[c.IDX_FTYPE] == c.DIR:
                    try:
                        abandon_list.remove(tup)
                    except ValueError:
                        pass

    # For directories whose names are not persisted by their parent dirs,
    # look for other possible names of this inode (same inum).
    # If any of the other names have been persisted, it means
    # the renamed name is successfully persisted, and thus
    # this entry should not exist anymore. Put in the abandon_list.
    for entry in entry_list:
        if len(entry[0]) == 1:
            continue
        inode = entry[3]
        inum = entry[0][-1][0]
        if inode.type == c.DIR:
            if not entry[1]:
                for entry2 in entry_list:
                    if entry == entry2:
                        continue
                    entry2_inum = entry2[0][-1][0]
                    if entry2_inum == inum:
                        path = _create_path(entry[0])
                        abandon_list.append((path, inum))

    # Dealing with renamed regular files
    # base: 1 A, 2 A/foo, 3 A/bar
    # op  : rename(A/foo, A/bar)
    # result can be either:
    # - 1 A 2 foo 3 bar
    # - 1 A 2 bar
    # If everything in bar's rename history (foo & bar) exist in image
    # that means we have a previous version.
    # If not, we have a persisted version. That solves constraints.
    for dep in deps:
        cnt = 0
        for tup in dep:
            for img_entry in METADATA:
                if img_entry[c.IDX_PATH] == tup[0]:
                    cnt += 1
        if cnt == len(dep):
            abandon_list.append(dep[-1])

    entry_list_final = []
    for entry in entry_list:
        entry_list_final.append(entry)
        names = []
        for tup in entry[0]:
            names.append(tup[1])
        path = "/".join(names)
        for (apath, inum) in abandon_list:
            if path.startswith(apath):
                try:
                    entry_list_final.remove(entry)
                except ValueError:
                    pass

    if c.verbose:
        print "FINAL LIST OF ENTRIES"
    for entry in entry_list_final:
        if c.verbose:
            print entry
        inum = entry[0][-1][0]
        name = entry[0][-1][1]
        try:
            inode = c.DISK[inum]
        except KeyError:
            inode = c.MEM[inum]

        if entry[1] and entry[2]:
            name_meta.append(entry)
        if entry[1] and not entry[2]:
            name_nometa.append(entry)
        if not entry[1] and entry[2]:
            noname_meta.append(entry)
        if not entry[1] and not entry[2]:
            noname_nometa.append(entry)

    found_bug = 0

    if c.verbose:
        print "Testing case 1: name and meta persisted"
    # case 1: both name and meta persisted
    # Check name's existence and its metadata consistency
    # - Name should be in the image with correct metadata
    for entry in name_meta:
        pathlist = entry[0]
        path = _create_path(pathlist)
        if c.verbose:
            print path
        inode = entry[3]

        flag = 0
        for img_entry in METADATA:
            if img_entry[0] == path:
                flag = 1
                check_meta(inode, img_entry)
                if inode.type == c.SYMLINK:
                    check_symlink(inode, img_entry)
                if inode.type == c.FILE:
                    check_data(inode, img_entry)
        if not flag:
            if inode.type == c.DIR:
                typestr = "directory"
            else:
                typestr = "file"
            errstr = "*** [META] Missing {0}: {1}".format(typestr, path)
            print errstr
            found_bug = 1
            if not c.REGTEST:
                c.FP_LOG.write(errstr + "\n")

    if c.verbose:
        print "Testing case 2: only name persisted"
    # Case 2: only name persisted
    # Check name's existence but don't need to check metadata inconsistency
    # - If name is not found, that is a bug
    for entry in name_nometa:
        pathlist = entry[0]
        path = _create_path(pathlist)
        if c.verbose:
            print path
        inode = entry[3]

        flag = 0
        for img_entry in METADATA:
            if img_entry[0] == path:
                flag = 1
        if not flag:
            if inode.type == c.DIR:
                typestr = "directory"
            else:
                typestr = "file"
            errstr = "*** [META] Missing {0}: {1}".format(typestr, path)
            print errstr
            found_bug = 1
            if not c.REGTEST:
                c.FP_LOG.write(errstr + "\n")

    # do this prior to checking case 3 entries
    paths = []
    path_entry_map = {}
    noname_all = noname_meta + noname_nometa
    for entry in noname_all:
        pathlist = entry[0]
        path =_create_path(pathlist)
        paths.append(path)
        try:
            path_entry_map[path].append(entry)
        except KeyError:
            path_entry_map[path] = [entry]
    dup_path_list = list(set([x for x in paths if paths.count(x) > 1]))
    dup_grp = {}

    if c.verbose:
        print "Testing case 3: only metadata persisted"
    # Case 3: only metadata persisted
    # Search name first.
    # - If name is found in the image, then check metadata
    # - If name is not found, that's fine. just move on.
    for entry in noname_all:
        pathlist = entry[0]
        path = _create_path(pathlist)
        if c.verbose:
            print path
        inode = entry[3]
        perm = entry[1]
        meta = entry[2]

        if path in dup_path_list:
            ret = 0
            for img_entry in METADATA:
                if img_entry[0] == path:
                    if meta:
                        ret += check_meta(inode, img_entry, ret_on_err=1)
                        if inode.type == c.SYMLINK:
                            ret += check_symlink(inode, img_entry, ret_on_err=1)
                        if inode.type == c.FILE:
                            ret += check_data(inode, img_entry, ret_on_err=1)
                    else:
                        ret = 0
            try:
                dup_grp[dup_path_list.index(path)].append(ret)
            except KeyError:
                dup_grp[dup_path_list.index(path)] = [ret]
        else:
            if meta:
                for img_entry in METADATA:
                    if img_entry[0] == path:
                        ret = 0
                        ret += check_meta(inode, img_entry)
                        if inode.type == c.SYMLINK:
                            ret += check_symlink(inode, img_entry)
                        if inode.type == c.FILE:
                            ret += check_data(inode, img_entry)
                        if ret > 0:
                            found_bug = 1

    for gid in dup_grp:
        if 0 not in dup_grp[gid]:
            print "Entry", dup_path_list[gid], "is not consistent:"
            found_bug = 1
            entries = path_entry_map[dup_path_list[gid]]

            for ei, entry in enumerate(entries):
                pathlist = entry[0]
                path = _create_path(pathlist)
                print "Case {0}".format(ei+1, path)
                inode = entry[3]
                for img_entry in METADATA:
                    if img_entry[0] == path:
                        check_meta(inode, img_entry)
                        if inode.type == c.SYMLINK:
                            check_symlink(inode, img_entry)
                        if inode.type == c.FILE:
                            check_data(inode, img_entry)

    # Case 4: nothing's persisted
    # No test needed for these entries.

    return found_bug


def test_fs_inum(list_inum_ondisk, METADATA, sd):
    global BTRFS_ERRSTR
    inumlist = []
    for e in METADATA:
        inumlist.append(int(e[c.IDX_INUM]))

    for iid in list(set(list_inum_ondisk)):
        if iid == 0: # skip root inode
            continue
        found = 0
        if c.verbose:
            print iid, c.DISK[iid].name

        for entry in METADATA:
            if int(entry[c.IDX_INUM]) == iid:
                found = 1
                inode_disk = c.DISK[iid]
                check_meta(inode_disk, entry, print_err=0)
                if inode_disk.type == c.FILE:
                    check_data(inode_disk, entry, print_err=0)
                elif inode_disk.type == c.SYMLINK:
                    check_symlink(inode_disk, entry, print_err=0)
                elif inode_disk.type == c.DIR:
                    check_child_exist(inode_disk, inumlist, print_err=0)
                break
        """
        if not found:
            errstr = "*** [META] missing inode {0} {1}".format(
                    iid, c.DISK[iid].name)
            BTRFS_ERRSTR += errstr + "\n"
            if not c.REGTEST:
                c.FP_LOG.write(errstr + "\n")
        """

    BTRFS_ERRSTR = BTRFS_ERRSTR.rstrip()
    if len(BTRFS_ERRSTR) > 0:
        # secondary check for self-consistency situation
        errlist = BTRFS_ERRSTR.split("\n")
        cnt = 0
        for err in errlist:
            if "Link count" in err:
                cnt += 1
        if cnt == len(errlist):
            if test_self_consistency(METADATA, print_err=0):
                print BTRFS_ERRSTR
        else:
            print BTRFS_ERRSTR


def test_self_consistency(METADATA, print_err=1):
    id_list = []
    err_cnt = 0
    for entry in METADATA:
        id_list.append(entry[c.IDX_INUM])
    for entry in METADATA:
        if entry[c.IDX_FTYPE] == "2": # skip additional check if directory
            continue
        if int(entry[c.IDX_NLINK]) != id_list.count(entry[c.IDX_INUM]):
            errstr = "Failed self-consistency test:\n"
            errstr += "*** [META] Incorrect metadata: {0} id:{1} lcnt:{2}".format(
                    entry[c.IDX_PATH], entry[c.IDX_INUM], entry[c.IDX_NLINK])
            err_cnt += 1

            if print_err:
                print errstr

            if not c.REGTEST:
                c.FP_LOG.write(errstr + "\n")
    return err_cnt

