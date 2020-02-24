import errno
import stat

from yggdrasil.util import *
from yggdrasil.diskspec import *
from disk import *


InoSort = BitVecSort(32)
NameSort = SizeSort # todo


def FreshIno(name):
    return FreshBitVec(name, InoSort.size())

def FreshName(name):
    return FreshBitVec(name, NameSort.size())


class DirLook(object):
    def __init__(self, inode):
        pass

    def locate_empty_slot(self, block):
        return FreshBitVec('off', 9)

    def locate_dentry(self, block, name):
        return FreshBitVec('off', 9)

    def locate_empty_slot_ino(self, ino):
        return FreshBitVec('ioff', 32), FreshBitVec('off', 9)

    def locate_dentry_ino(self, ino, name):
        return FreshBitVec('ioff', 32), FreshBitVec('off', 9)


class Attributes(object):
    def __init__(self, bsizefn, fsizefn, mtimefn, modefn, nlinkfn):
        self._bsizefn = bsizefn
        self._fsizefn = fsizefn
        self._mtimefn = mtimefn
        self._modefn = modefn
        self._nlinkfn = nlinkfn

    def set_bsize(self, ino, bsize, guard=BoolVal(True)):
        self._bsizefn = self._bsizefn.update(ino, bsize, guard)

    def set_fsize(self, ino, fsize, guard=BoolVal(True)):
        self._fsizefn = self._fsizefn.update(ino, fsize, guard)

    def set_mtime(self, ino, mtime, guard=BoolVal(True)):
        self._mtimefn = self._mtimefn.update(ino, mtime, guard)

    def set_mode(self, ino, mode, guard=BoolVal(True)):
        self._modefn = self._modefn.update(ino, mode, guard)

    def set_nlink(self, ino, nlink, guard=BoolVal(True)):
        self._nlinkfn = self._nlinkfn.update(ino, nlink, guard)

    def bsize(self, ino):
        return self._bsizefn(ino)

    def fsize(self, ino):
        return self._fsizefn(ino)

    def mtime(self, ino):
        return self._mtimefn(ino)

    def mode(self, ino):
        return self._modefn(ino)

    def nlink(self, ino):
        return self._nlinkfn(ino)

    def to_stat(self, ino):
        return Stat(size=Concat(self.bsize(ino), self.fsize(ino)),
                    mtime=self.mtime(ino),
                    mode=self.mode(ino),
                    nlink=self.nlink(ino))


def FreshAttributes():
    bsizefn = FreshUFunction('bsizefn', InoSort, BitVecSort(32))
    fsizefn = FreshUFunction('fsizefn', InoSort, BitVecSort(32))
    mtimefn = FreshUFunction('mtimefn', InoSort, SizeSort)
    modefn = FreshUFunction('modefn', InoSort, SizeSort)
    nlinkfn = FreshUFunction('nlinkfn', InoSort, SizeSort)
    return Attributes(bsizefn, fsizefn, mtimefn, modefn, nlinkfn)


class DirSpec(object):
    def __init__(self, mach, dirfn, direxists, datafn, ifreefn, attrs):
        self._mach = mach

        self._dirfn = dirfn
        self._direxists = direxists

        self._datafn = datafn
        self._ifreefn = ifreefn

        self._attrs = attrs

    def ialloc(self, on):
        ino = FreshIno('spec-alloc-ino')
        # allocator assumptions
        assertion(self.is_ifree(ino))
        assertion(ino != 0)

        self._attrs.set_bsize(ino, 0, guard=on)
        self._attrs.set_fsize(ino, 0, guard=on)
        self._attrs.set_mtime(ino, 0, guard=on)
        self._attrs.set_mode(ino, 0, guard=on)
        self._attrs.set_nlink(ino, 1, guard=on)

        # mark the new inode as being used
        self._ifreefn = self._ifreefn.update(ino, BoolVal(False), guard=on)

        return ino

    def is_ifree(self, ino):
        return self._ifreefn(ino)

    def is_valid(self, ino):
        return And(ino != 0, Not(self._ifreefn(ino)), UGT(self._attrs.nlink(ino), 0))

    def is_dir(self, ino):
        return And(self.is_valid(ino),
            self._attrs.mode(ino) & stat.S_IFDIR != 0)

    def is_regular(self, ino):
        return And(self.is_valid(ino),
                   self._attrs.mode(ino) & stat.S_IFDIR == 0)

    ##

    def read(self, ino, blocknum, off):
        res = self._datafn(ino, blocknum, off)
        return res

    # Truncate a file down to block `bnum`.
    # truncate(ino, 0) will wipe the entire file.
    def truncate(self, ino, size):
        assertion(ULT(BitVecVal(0, 32), size))
        assertion(ULE(size, BitVecVal(4096, 32)))
        assertion(self.is_regular(ino))

        on = self._mach.create_on([])

        self._attrs.set_fsize(ino, size, on)


    def lookup(self, parent, name):
        return If(self._direxists(parent, name[0]), self._dirfn(parent, name[0]), 0)

    def get_iattr(self, ino):
        return self._attrs.to_stat(ino)

    def write(self, ino, blocknum, v, size=4096):
        assertion(self.is_regular(ino))

        _fn = self._datafn._fn
        _bfn = v._fn

        on = self._mach.create_on([])

        fn = lambda i, b, o: If(
                And(on, ino == i, blocknum == b), _bfn(o), _fn(i, b, o))
        self._datafn._fn = fn

        assertion(ULE(blocknum, 1048574))

        update = ULE(self._attrs.fsize(ino), blocknum * 4096 + size)
        self._attrs.set_bsize(ino, blocknum + 1, And(update, on))
        self._attrs.set_fsize(ino, blocknum * 4096 + size, And(update, on))

    def rename(self, oparent, oname, nparent, nname):
        # old parent and new parent are directories
        assertion(self.is_dir(oparent))
        assertion(self.is_dir(nparent))

        # destination does not exist
        assertion(Not(self._direxists(nparent, nname[0])))

        on = self._mach.create_on([])

        ino = self._dirfn(oparent, oname[0])

        assertion(ULT(self._attrs.nlink(nparent), self._attrs.nlink(nparent) + 1))
        self._attrs.set_nlink(nparent, self._attrs.nlink(nparent) + 1, guard=on)
        self._attrs.set_bsize(nparent, 522, guard=on)
        self._attrs.set_fsize(nparent, 4096 * 522, guard=on)

        assertion(ULT(self._attrs.nlink(nparent) - 1, self._attrs.nlink(nparent)))
        self._attrs.set_nlink(oparent, self._attrs.nlink(oparent) - 1, guard=on)

        # remove oname from oparent
        self._direxists = self._direxists.update((oparent, oname[0]),
                BoolVal(False), guard=on)

        # associate newparent x newname -> ino
        self._dirfn = self._dirfn.update((nparent, nname[0]), ino, guard=on)
        self._direxists = self._direxists.update((nparent, nname[0]),
                BoolVal(True), guard=on)

    def mknod(self, parent, name, mode, mtime):
        on = self._mach.create_on([])

        # Parent is a directory
        assertion(self.is_dir(parent))

        # A file with this name does not exist in the directory
        assertion(Not(self._direxists(parent, name[0])))

        # Get a fresh inode number.. It must be non-zero and unique
        ino = self.ialloc(on)

        # Set attribute for parent directory
        self._attrs.set_nlink(parent, self._attrs.nlink(parent) + 1, guard=on)
        self._attrs.set_bsize(parent, 522, guard=on)
        self._attrs.set_fsize(parent, 4096 * 522, guard=on)

        # Set attributes for new inode
        self._attrs.set_bsize(ino, 0, guard=on)
        self._attrs.set_fsize(ino, 0, guard=on)
        self._attrs.set_mtime(ino, mtime, guard=on)
        self._attrs.set_mode(ino, mode, guard=on)
        self._attrs.set_nlink(ino, 2, guard=on)

        # associate parent x name -> ino
        self._dirfn = self._dirfn.update((parent, name[0]), ino, guard=on)
        self._direxists = self._direxists.update((parent, name[0]), BoolVal(True), guard=on)

        return ino

    def forget(self, ino):
        if Or(self._attrs.nlink(ino) != 1, self.is_dir(ino)):
            return

        assertion(self.is_valid(ino))
        on = self._mach.create_on([])

        # Wipe data
        _fn = self._datafn._fn
        fn = lambda i, b, o: If( And(on, ino == i), BitVecVal(0, 64), _fn(i, b, o))
        self._datafn._fn = fn

        self._attrs.set_bsize(ino, 0, guard=on)
        self._attrs.set_fsize(ino, 0, guard=on)
        self._attrs.set_nlink(ino, 0, guard=on)
        self._ifreefn = self._ifreefn.update(ino, BoolVal(True), guard=on)

    def unlink(self, parent, name):
        # parent is a directory
        assertion(self.is_dir(parent))

        on = self._mach.create_on([])

        # TODO: Failure?
        # Decrement nlink
        assertion(UGT(self._attrs.nlink(parent), 1))
        self._attrs.set_nlink(parent, self._attrs.nlink(parent) - 1, guard=on)

        ino = self._dirfn(parent, name[0])

        assertion(UGT(self._attrs.nlink(ino), 1))

        self._attrs.set_nlink(ino, 1, guard=on)

        # remove ino from parent
        self._direxists = self._direxists.update((parent, name[0]), BoolVal(False), guard=on)

        return ino

    def rmdir(self, parent, name):
        # parent is a directory
        if Not(self.is_dir(parent)):
            return 0, errno.ENOTDIR

        on = self._mach.create_on([])

        ino = self._dirfn(parent, name[0])
        assertion(self.is_dir(ino))

        if UGT(self._attrs.nlink(ino), 2):
            return BitVecVal(0, 32), errno.ENOTEMPTY

        # Wipe data
        _fn = self._datafn._fn
        fn = lambda i, b, o: If( And(on, ino == i), BitVecVal(0, 64), _fn(i, b, o))
        self._datafn._fn = fn

        self._attrs.set_nlink(parent, self._attrs.nlink(parent) - 1, guard=on)

        self._attrs.set_nlink(ino, 1, guard=on)

        return ino, BitVecVal(0, 32)

    def crash(self, mach):
        return self.__class__(mach, self._dirfn, self._direxists, self._datafn, self._ifreefn, self._attrs)
