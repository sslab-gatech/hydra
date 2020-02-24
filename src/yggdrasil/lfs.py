import cython
from disk import *
import errno
import time
import z3


class LFS(object):
    SUPERBLOCK = 0

    SB_OFF_BALLOC = 0
    SB_OFF_IALLOC = 1
    SB_OFF_IMAP = 2

    I_OFF_MTIME = 0
    I_OFF_MODE = 1
    I_OFF_DATA = 4
    
    def __init__(self, disk):
        self._disk = disk

        self._sb = None
        self._imap = None

    def read(self, ino, block):
        self._begin()
        bid = self._imap[ino]
        r = self._disk.read(bid)
        self._commit(False)
        return r

    def _begin(self):
        assert self._sb is None
        assert self._imap is None

        self._sb = self._disk.read(self.SUPERBLOCK)
        self._imap = self._disk.read(self._sb[self.SB_OFF_IMAP])

    def _balloc(self):
        a = self._sb[self.SB_OFF_BALLOC]
        self._sb[self.SB_OFF_BALLOC] += 1

        # Allocator returned a new block
        assertion(0 < (a + 1))

        return a

    def _ialloc(self):
        a = self._sb[self.SB_OFF_IALLOC]
        self._sb[self.SB_OFF_IALLOC] += 1

        # we have a free inode..
        assertion(a < 512)

        return a

    def _commit(self, write=True):
        assert self._sb is not None
        assert self._imap is not None

        if write:
            a = self._balloc()
            self._disk.write(a, self._imap)
            self._disk.flush()
            self._sb[self.SB_OFF_IMAP] = a
            self._disk.write(self.SUPERBLOCK, self._sb)
            self._disk.flush()

        self._sb = None
        self._imap = None

    def _set_map(self, ino, bid):
        self._imap[Extract(8, 0, ino)] = bid

    def _get_map(self, ino):
        return self._imap[Extract(8, 0, ino)]

    ########

    def dir_lookup(self, blk, name):
        res = -errno.ENOENT
        for i in range(2):
            oname = blk[self.I_OFF_DATA + i * 2]
            oino = blk[self.I_OFF_DATA + i * 2 + 1]

            res = If(And(oname == name, 0 < oino), oino, res)
        return res

    def dir_find_empty(self, blk):
        res = BitVecVal(-errno.ENOSPC, 64)
        for i in range(2):
            res = If(blk[self.I_OFF_DATA + i * 2] == 0, i, res)
        return res

    def get_attr(self, ino):
        s = Stat(0, 0, 0)

        self._begin()

        blk = self._get_map(ino)
        blk = self._disk.read(blk)
        s.bsize = 0
        s.mode = blk[self.I_OFF_MODE]
        s.mtime = blk[self.I_OFF_MTIME]

        self._commit(False)

        return s

    def lookup(self, parent, name):
        self._begin()

        parent_blkno = self._get_map(parent)
        parent_blk = self._disk.read(parent_blkno)

        ino = self.dir_lookup(parent_blk, name)
        self._commit(False)
        return ino

    def exists(self, parent, name):
        return 0 < self.lookup(parent, name)

    def mknod(self, parent, name, mode, mtime):
        # check if the file exists
        if self.exists(parent, name):
            assertion(False)
            return BitVecVal(-errno.EEXIST, 64)

        self._begin()

        parent_blkno = self._get_map(parent)
        parent_blk = self._disk.read(parent_blkno)

        ino = self._ialloc()
        blkno = self._balloc()

        eoff = self.dir_find_empty(parent_blk)

        if eoff < 0:
            self._commit(False)
            return eoff

        # write new inode
        inodeblk = ConstBlock(0)

        inodeblk[self.I_OFF_MTIME] = mtime
        inodeblk[self.I_OFF_MODE] = mode
        self._disk.write(blkno, inodeblk)

        # update parent directory
        parent_blk[self.I_OFF_DATA + 2 * Extract(8, 0, eoff)] = name
        parent_blk[self.I_OFF_DATA + 2 * Extract(8, 0, eoff) + 1] = ino

        new_parent_blkno = self._balloc()

        self._disk.write(new_parent_blkno, parent_blk)

        # update the imap
        self._set_map(ino, blkno)
        self._set_map(parent, new_parent_blkno)

        self._commit()

        return ino

    def crash(self, mach):
        return self.__class__(self._disk.crash(mach))


def mkfs(disk):
    sb = disk._disk.read(0)
    if sb[0] == 0:
        sb[LFS.SB_OFF_BALLOC] = 3
        sb[LFS.SB_OFF_IALLOC] = 2
        sb[LFS.SB_OFF_IMAP] = 1
        disk._disk.write(0, sb)

        imap = ConstBlock(0)
        imap[1] = 2
        disk._disk.write(1, imap)


def create_lfs(*args):
    disk = AsyncDisk('/tmp/foo.img')
    lfs = LFS(disk)
    mkfs(lfs)

    return lfs

if __name__ == '__main__':
    lfs = create_lfs()

    print lfs.lookup(1, 16)
    print lfs.get_attr(4)
    print lfs.mknod(1, 20, 2000, 2000)
    print lfs.lookup(1, 20)
    print lfs.get_attr(4)
