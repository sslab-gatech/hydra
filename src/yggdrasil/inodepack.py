import cython
if not cython.compiled:
    from disk import *
from collections import namedtuple


# A class that packs multiple inodes together into a single block


class InodePackDisk(object):
    # Field index for meta-data
    SIZE = 0
    MTIME = 1
    MODE = 2
    NLINK = 3
    _UNUSED = 4
    OFF = 5

    def __init__(self, metadisk, datadisk):
        self._disk = metadisk

    def read(self, ino):
        return self._disk.read(LShR(ino, 5))

    @cython.locals(off='uint64_t')
    @cython.locals(bid='uint64_t')
    def set_iattr(self, ino, attr, block=None):
        off = Extract(8, 0, ino * 16)
        bid = LShR(ino, 5) # UDiv(ino, 32)

        if block is None:
            inode = self._disk.read(bid)
        else:
            inode = block

        inode[self.SIZE + off] = attr.size
        inode[self.MTIME + off] = attr.mtime
        inode[self.MODE + off] = attr.mode
        inode[self.NLINK + off] = attr.nlink
        self._disk.write(bid, inode)

    @cython.locals(off='uint64_t')
    @cython.locals(bid='uint64_t')
    def get_iattr(self, ino, block=None):
        off = Extract(8, 0, ino * 16)
        bid = LShR(ino, 5) # UDiv(ino, 32)

        if block is None:
            inode = self._disk.read(bid)
        else:
            inode = block
        return Stat(
                inode[off + self.SIZE],
                inode[off + self.MTIME],
                inode[off + self.MODE],
                inode[off + self.NLINK])

    @cython.locals(ioff='uint64_t')
    @cython.locals(bid='uint64_t')
    def set_mapping(self, ino, off, ptr, block=None):
        assertion(ULT(off, 11))

        ioff = Extract(8, 0, ino * 16)
        bid = LShR(ino, 5) # UDiv(ino, 32)

        if block is None:
            old = self._disk.read(bid)
        else:
            old = block

        old[off + ioff + self.OFF] = ptr
        self._disk.write(bid, old)

    def get_mapping(self, ino, off, block=None):
        if off >= 11:
            return 0
        return self._get_mapping(ino, off, block)

    @cython.locals(ioff='uint64_t')
    @cython.locals(bid='uint64_t')
    def _get_mapping(self, ino, off, block=None):
        ioff = Extract(8, 0, ino * 16)
        bid = LShR(ino, 5) # UDiv(ino, 32)

        if block is None:
            block = self._disk.read(bid)

        return block[off + ioff + self.OFF]

    def crash(self, mach):
        return self.__class__(self._disk.crash(mach),
                self._disk.crash(mach))
