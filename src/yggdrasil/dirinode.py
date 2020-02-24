import cython
if not cython.compiled:
    import z3
    from disk import *

import errno
from stat import S_IFDIR
from collections import namedtuple


Disk = namedtuple('Disk', ['read', 'write'])


class Orphans(object):
    def __init__(self, orphandisk):
        self._orphandisk = orphandisk

    def size(self):
        return self._orphandisk.read(0)[0]

    def index(self, idx):
        orphanblock = self._orphandisk.read(0)
        n = orphanblock[0]

        assertion(0 <= n, "orphan index: n is negative")
        assertion(n < 511, "orphan index: n >= 511")

        np = Extract(8, 0, idx)

        return orphanblock[np + 1]

    def reset(self):
        self._orphandisk.write(0, ConstBlock(0))

    def clear(self, idx):
        orphanblock = self._orphandisk.read(0)
        np = Extract(8, 0, idx)
        orphanblock[np] = 0
        self._orphandisk.write(0, orphanblock)

    def append(self, value):
        orphanblock = self._orphandisk.read(0)
        n = orphanblock[0]

        assertion(0 <= n, "orphan index: n is negative")
        assertion(n < 511, "orphan index: n >= 511")

        np = Extract(8, 0, n)

        orphanblock[np + 1] = value
        orphanblock[0] = n + 1

        self._orphandisk.write(0, orphanblock)



class DirImpl(object):
    NBLOCKS = 522

    IFREEDISK =  4
    ORPHANS =  5

    @cython.locals(inode='IndirectInodeDisk')
    def __init__(self, txndisk, inode, Allocator, Bitmap, DirLookup):
        self._txndisk = txndisk
        self._inode = inode

        self._Allocator = Allocator
        self._Bitmap = Bitmap
        self._DirLookup = DirLookup

        PIno = namedtuple('Inode', ['is_mapped', 'mappingi', 'read', 'bmap'])

        self._dirlook = DirLookup(PIno(
            is_mapped=lambda vbn, inode=inode: inode.is_mapped(vbn),
            mappingi=lambda vbn, inode=inode: inode.mappingi(vbn),
            read=lambda bid, inode=inode: inode.read(bid),
            bmap=lambda bid, inode=inode: inode.bmap(bid),
        ))

        self._ifree = Disk(
            write=lambda bid, data: self._txndisk.write_tx(self.IFREEDISK, bid, data),
            read=lambda bid: self._txndisk._read(self.IFREEDISK, bid))

        orphandisk = Disk(
            write=lambda bid, data: self._txndisk.write_tx(self.ORPHANS, bid, data),
            read=lambda bid: self._txndisk._read(self.ORPHANS, bid))

        self._iallocator = Allocator(
                lambda n: self._ifree.read(n),
                0, 1024)

        self._ibitmap = Bitmap(self._ifree)
        self._orphans = Orphans(orphandisk)

    def locate_dentry_ino(self, ino, name):
        ioff, off = self._dirlook.locate_dentry_ino(ino, name)
        assertion(ULT(ioff, 522), "locate_dentry_ino: invalid ioff")
        assertion(ioff != 10, "locate_dentry_ino: invalid ioff")
        bid = self._inode.bmap(Concat32(ino, ioff))
        block = self._inode.read(bid)
        valid = And(bid != 0, off % 16 == 0, Extract(31, 0, block[off]) != 0)
        for i in range(15):
            valid = And(valid, block[off + i + 1] == name[i])
        return block, bid, off, valid

    def locate_empty_dentry_slot_ino(self, ino):
        ioff, off = self._dirlook.locate_empty_slot_ino(ino)
        assertion(ULT(ioff, 522), "locate_empty_dentry_slot: invalid ioff")
        assertion(ioff != 10, "locate_empty_dentry_slot: invalid ioff")
        bid = self._inode.bmap(Concat32(ino, ioff))
        block = self._inode.read(bid)
        assertion(bid != 0, "locate_empty_dentry_slot: invalid bid")
        assertion(off % 16 == 0, "locate_empty_dentry_slot: invalid offset")
        assertion(block[off] == 0, "locate_empty_dentry_slot: slot not empty")
        return block, bid, off

    def locate_empty_dentry_slot_err_ino(self, ino):
        ioff, off = self._dirlook.locate_empty_slot_ino(ino)
        assertion(ULT(ioff, 522), "locate_dentry_ino: invalid ioff")
        assertion(ioff != 10, "locate_dentry_ino: invalid ioff")
        bid = self._inode.bmap(Concat32(ino, ioff))
        block = self._inode.read(bid)
        return block, bid, off, And(bid != 0, off % 16 == 0, block[off] == 0)

    def write_dentry(self, block, off, ino, name):
        block[off] = ino
        for i in range(15):
            block[off + i + 1] = name[i]

    def clear_dentry(self, block, off):
        for i in range(16):
            block[off + i] = 0

    def ialloc(self):
        # black box allocator returns a vbn
        ino = self._iallocator.alloc()
        # Validation
        assertion(ino != 0, "ialloc: inode is 0")
        assertion(self.is_ifree(ino), "ialloc: ino is not free")
        self._ibitmap.set_bit(ino)
        return ino

    def is_ifree(self, ino):
        return Not(self._ibitmap.is_set(ino))

    def is_valid(self, ino):
        return And(ino != 0, self._ibitmap.is_set(ino), UGT(self.get_iattr(ino).nlink, 0))

    def is_gcable(self, ino):
        return And(ino != 0, self._ibitmap.is_set(ino), self.get_iattr(ino).nlink == 0)

    def is_dir(self, ino):
        attr = self._inode.get_iattr(ino)
        return And(self.is_valid(ino),
                   attr.mode & S_IFDIR != 0)

    def is_regular(self, ino):
        attr = self._inode.get_iattr(ino)
        return And(self.is_valid(ino),
                   attr.mode & S_IFDIR == 0)

    ###

    def get_iattr(self, ino):
        return self._inode.get_iattr(ino)

    def set_iattr(self, ino, attr):
        self._inode.begin_tx()
        self._inode.set_iattr(ino, attr)
        self._inode.commit_tx()

    def read(self, ino, blocknum):
        attr = self.get_iattr(ino)
        bsize = attr.bsize

        is_mapped = self._inode.is_mapped(Concat32(ino, blocknum))
        lbn = self._inode.mappingi(Concat32(ino, blocknum))
        res = self._inode.read(lbn)
        res = If(And(is_mapped, ULT(blocknum, bsize)), res, ConstBlock(0))
        return res

    def truncate(self, ino, fsize):

        target_bsize = fsize / 4096 + (fsize % 4096 != 0)

        # Update the size

        attr = self._inode.get_iattr(ino)

        while attr.bsize > target_bsize:
            self._inode.begin_tx()
            self._inode.bunmap(Concat32(ino, attr.bsize - 1))
            attr.size = Concat32(attr.bsize - 1, fsize)
            self._inode.set_iattr(ino, attr)
            self._inode.commit_tx()

        if attr.fsize > fsize:
            self._inode.begin_tx()
            attr.size = Concat32(attr.bsize, fsize)
            self._inode.set_iattr(ino, attr)
            self._inode.commit_tx()

    def write(self, ino, blocknum, v, size=BitVecVal(4096, 32)):
        # Implementation support only a small number of blocknums.
        assertion(ULT(blocknum, 522), "write: blocknum to large")
        assertion(ULT(BitVecVal(0, 32), size), "write: size is 0")
        assertion(ULE(size, BitVecVal(4096, 32)), "write: size to large")
        assertion(self.is_regular(ino), "write: writing to a non-regular inode")

        self._inode.begin_tx()

        bid = self._inode.bmap(Concat32(ino, blocknum))
        self._inode.write(bid, v)

        attr = self._inode.get_iattr(ino)

        nsize = Concat32(blocknum + 1, blocknum * 4096 + size)
        update = ULE(attr.fsize, blocknum * 4096 + size)
        attr.size = If(update, nsize, attr.size)

        self._inode.set_iattr(ino, attr)

        self._inode.commit_tx()

        return size

    def lookup(self, parent, name):
        assertion(self.is_dir(parent), "lookup: parent is not dir")

        self._inode.begin_tx()
        parent_block, _, off, valid = self.locate_dentry_ino(parent, name)
        self._inode.commit_tx()

        return If(valid, Extract(31, 0, parent_block[off]), 0)

    def mknod(self, parent, name, mode, mtime):
        assertion(self.is_dir(parent), "mknod: parent is not a directory")
        assertion(name[0] != 0, "mknod: name is null")

        self._inode.begin_tx()

        parent_block, parent_bid, off, valid = self.locate_empty_dentry_slot_err_ino(parent)
        if Not(valid):
            self._inode.commit_tx()
            return 0, errno.ENOSPC

        ino = self.ialloc()

        attr = Stat(size=0, mtime=mtime, mode=mode, nlink=2)

        self._inode.set_iattr(ino, attr)

        attr = self._inode.get_iattr(parent)
        assertion(ULE(attr.bsize, 522), "mknod: bsize is larger than 522")
        attr.size = Concat32(BitVecVal(522, 32), BitVecVal(4096 * 522, 32))
        assertion(ULT(attr.nlink, attr.nlink + 1), "mknod: nlink overflow")
        attr.nlink += 1

        self._inode.set_iattr(parent, attr)

        self.write_dentry(parent_block, off, ino, name)
        parent_block[off] = ino

        self._inode.write(parent_bid, parent_block)

        self._inode.commit_tx()

        return ino, 0

    def unlink(self, parent, name):
        assertion(self.is_dir(parent), "unlink: not a dir")
        assertion(name[0] != 0, "unlink: name is null")

        self._inode.begin_tx()

        parent_block, parent_bid, off, valid = self.locate_dentry_ino(parent, name)

        assertion(valid, "unlink: not valid")

        attr = self._inode.get_iattr(parent)
        assertion(UGE(attr.nlink, 2), "unlink: nlink is not greater than 1: " + str(attr.nlink))
        attr.nlink -= 1
        self._inode.set_iattr(parent, attr)

        ino = Extract(31, 0, parent_block[off])

        attr = self._inode.get_iattr(ino)
        attr.nlink = 1
        self._inode.set_iattr(ino, attr)

        self.clear_dentry(parent_block, off)

        self._inode.write(parent_bid, parent_block)

        # append the inode to the orphan list
        self._orphans.append(Extend(ino, 64))

        self._inode.commit_tx()

        return ino

    def rmdir(self, parent, name):
        assertion(self.is_dir(parent), "rmdir: parent is not a directory")
        assertion(name[0] != 0, "rmdir: name is null")

        self._inode.begin_tx()
        parent_block, parent_bid, off, valid = self.locate_dentry_ino(parent, name)
        if Not(valid):
            self._inode.commit_tx()
            return 0, errno.ENOENT

        assertion(valid, "rmdir: dentry off not valid")

        ino = Extract(31, 0, parent_block[off])
        if Not(self.is_dir(ino)):
            self._inode.commit_tx()
            return 0, errno.ENOTDIR

        assertion(self.is_dir(ino), "rmdir: ino is not dir")

        attr = self._inode.get_iattr(ino)
        if UGT(attr.nlink, 2):
            self._inode.commit_tx()
            return BitVecVal(0, 32), errno.ENOTEMPTY

        attr = self._inode.get_iattr(parent)
        assertion(UGE(attr.nlink, 2), "rmdir: nlink is not greater than 1: " + str(attr.nlink))
        attr.nlink -= 1
        self._inode.set_iattr(parent, attr)

        self.clear_dentry(parent_block, off)
        self._inode.write(parent_bid, parent_block)

        attr = self._inode.get_iattr(ino)
        attr.nlink = 1
        self._inode.set_iattr(ino, attr)

        # append the inode to the orphan list
        self._orphans.append(Extend(ino, 64))

        self._inode.commit_tx()

        return ino, 0

    def forget(self, ino):
        if Or(self.get_iattr(ino).mode & S_IFDIR != 0, self.get_iattr(ino).nlink != 1):
            return

        assertion(self.is_regular(ino), "forget: ino is not regular")

        self._inode.begin_tx()
        attr = self._inode.get_iattr(ino)
        attr.nlink = 0
        self._inode.set_iattr(ino, attr)
        self._inode.commit_tx()

    def rename(self, oparent, oname, nparent, nname):
        assertion(self.is_dir(oparent), "rename: oparent is not dir")
        assertion(self.is_dir(nparent), "rename: nparent is not dir")

        assertion(oname[0] != 0, "rename: oname is null")
        assertion(nname[0] != 0, "rename: nname is null")

        self._inode.begin_tx()

        attr = self._inode.get_iattr(oparent)
        assertion(UGE(attr.nlink, 2), "rename: nlink is not greater than 1: " + str(attr.nlink))
        attr.nlink -= 1
        self._inode.set_iattr(oparent, attr)

        attr = self._inode.get_iattr(nparent)
        assertion(ULE(attr.bsize, 522), "rename: bsize is larger than 522")
        attr.size = Concat32(BitVecVal(522, 32), BitVecVal(4096 * 522, 32))
        assertion(ULT(attr.nlink, attr.nlink + 1), "rename: nlink overflow")
        attr.nlink += 1
        self._inode.set_iattr(nparent, attr)

        # Find target and wipe from parent block
        oparent_block, oparent_bid, ooff, ovalid  = self.locate_dentry_ino(oparent, oname)
        assertion(ovalid, "rename: ooff is not valid")
        ino = oparent_block[ooff]
        self.clear_dentry(oparent_block, ooff)
        self._inode.write(oparent_bid, oparent_block)

        # Check if target exists
        nparent_block, nparent_bid, noff, nvalid = self.locate_dentry_ino(nparent, nname)

        if nvalid:
            # append the dst inode to the orphan list
            self._orphans.append(nparent_block[noff])
            self.clear_dentry(nparent_block, noff)

        nparent_block, nparent_bid, noff = self.locate_empty_dentry_slot_ino(nparent)
        self.write_dentry(nparent_block, noff, ino, nname)

        self._inode.write(nparent_bid, nparent_block)

        self._inode.commit_tx()

        return 0

    def fsync(self):
        self._txndisk.flush()

    def gc1(self, orph_index, off):
        ino = Extract(31, 0, self._orphans.index(orph_index))
        if not self.is_gcable(ino):
            return
        # Wipe data

        self._inode.begin_tx()
        self._inode.bunmap(Concat32(ino, off))

        nsize = off

        attr = self._inode.get_iattr(ino)
        if attr.bsize == nsize + 1:
            attr.size = Concat32(nsize, nsize * 4096)
            self._inode.set_iattr(ino, attr)

        self._inode.commit_tx()

    # If the inode is in the orphan list, is gc-able *and* 
    # its size is 0 we can safely mark it as 'free'
    def gc2(self, orph_index):
        ino = Extract(31, 0, self._orphans.index(orph_index))
        if not self.is_gcable(ino):
            return

        if self._inode.get_iattr(ino).size == 0:
            self._inode.begin_tx()
            self._orphans.clear(orph_index)
            self._ibitmap.unset_bit(ino)
            self._inode.commit_tx()

    def gc3(self):
        self._inode.begin_tx()
        self._orphans.reset()
        self._inode.commit_tx()

    def crash(self, mach):
        return self.__class__(self._txndisk.crash(mach), self._inode.crash(mach), self._Allocator, self._Bitmap, self._DirLookup)
