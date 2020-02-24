from dirinode import DirImpl

from yggdrasil import test
from yggdrasil.dirspec import *


OffsetSort = BitVecSort(32)


class ISpec(object):
    def __init__(self, start, end):
        self._map = FreshUFunction('map',  SizeSort, SizeSort)
        self._revmap = FreshUFunction('revmap', SizeSort, SizeSort)
        self._freemap = FreshUFunction('free', SizeSort, BoolSort())

        self._datafn = FreshDiskArray('datafn')

        self._start = BitVecVal(start, 32)
        self._end = BitVecVal(end, 32)

    def alloc(self):
        block = FreshSize('alloc')
        assertion(self.is_free(block))
        self._freemap = self._freemap.update(block, BoolVal(False))
        return block

    def free(self, block, guard=BoolVal(True)):
        self._freemap = self._freemap.update(block, BoolVal(True), guard=guard)

    def is_free(self, block):
        return self._freemap(block)

    def inrange(self, off):
        return And(ULE(self._start, off), ULE(off, self._end))

    #############

    def is_mapped(self, ino, off):
        vbn = Concat(ino, off)
        block = self._map(vbn)
        return And(self.inrange(off), Not(self.is_free(block)), self._revmap(block) == vbn)

    #############

    def bmap(self, ino, off):
        if Or(self.is_mapped(ino, off), Not(self.inrange(off))):
            return

        vbn = Concat(ino, off)

        block = self.alloc()

        self._map = self._map.update(vbn, block)
        self._revmap = self._revmap.update(block, vbn)

        self._datafn = self._datafn.update(block, ConstBlock(0))

    def bunmap(self, ino, off):
        unmap = self.is_mapped(ino, off)
        block = self._map(Concat(ino, off))
        self.free(block, guard=unmap)

    def _read(self, block):
        return self._datafn(block)

    def read(self, ino, off):
        return If(self.is_mapped(ino, off),
                self._read(self._map(Concat(ino, off))), ConstBlock(0))

    def _write(self, block, value):
        self._datafn = self._datafn.update(block, value)

    def write(self, ino, off, value):
        if not self.is_mapped(ino, off):
            return
        self._write(self._map(Concat(ino, off)), value)


class IExtSpec(object):
    def __init__(self, dat, ext):
        self._dat = dat
        self._ext = ext

    def alloc(self, *args, **kwargs):
        return self._dat.alloc(*args, **kwargs)

    def free(self, *args, **kwargs):
        return self._dat.free(*args, **kwargs)

    def is_free(self, *args, **kwargs):
        return self._dat.is_free(*args, **kwargs)

    ############

    def bmap(self, *args, **kwargs):
        return self._dat.bmap(*args, **kwargs)

    def bunmap(self, *args, **kwargs):
        return self._dat.bunmap(*args, **kwargs)

    def _read(self, *args, **kwargs):
        return self._dat._read(*args, **kwargs)

    def _write(self, *args, **kwargs):
        return self._dat._write(*args, **kwargs)

    def read(self, *args, **kwargs):
        return self._dat.read(*args, **kwargs)

    def write(self, *args, **kwargs):
        return self._dat.write(*args, **kwargs)

    ###

    def ebmap(self, *args, **kwargs):
        return self._ext.bmap(*args, **kwargs)

    def ebunmap(self, *args, **kwargs):
        return self._ext.bunmap(*args, **kwargs)

    def eread(self, *args, **kwargs):
        return self._ext.read(*args, **kwargs)

    def ewrite(self, *args, **kwargs):
        return self._ext.write(*args, **kwargs)

    def crash(self, *args, **kwargs):
        return self


class BlockImpl(object):
    def __init__(self, idisk, freedisk, Bitmap, Allocator):
        self._idisk = idisk

    #########################

    def _read(self, block):
        return self._idisk._read(block)

    def _write(self, block, data):
        self._idisk._write(block, data)

    def read(self, ino, off):
        eoff = Extract(9 - 1, 0, off)

        # Off is direct
        if UGE(off, 512):
            return ConstBlock(0)

        imap = self._idisk.eread(ino, BitVecVal(0, 32))
        block = imap[eoff]
        if self._idisk.is_free(block):
            return ConstBlock(0)
        return self._read(block)

    def write(self, ino, off, value):
        eoff = Extract(9 - 1, 0, off)

        # Off is direct
        if UGE(off, 512):
            return ConstBlock(0)

        imap = self._idisk.eread(ino, BitVecVal(0, 32))
        block = imap[eoff]
        if self._idisk.is_free(block):
            return ConstBlock(0)
        self._write(block, value)


    def bmap(self, ino, off):
        eoff = Extract(9 - 1, 0, off)

        # Off is direct
        if UGE(off, 512):
            return

        imap = self._idisk.eread(ino, BitVecVal(0, 32))

        old_lbn = imap[eoff]

        # Off is not-mapped
        if old_lbn == 0:

            lbn = self._idisk.alloc()

            self.write(ino, off, ConstBlock(0))

            imap[eoff] = lbn

            self._idisk.ewrite(ino, BitVecVal(0, 32), imap)

    def crash(self, *args, **kwargs):
        return self


class ISpecTest(test.RefinementTest):
    def create_spec(self, mach):
        dat = ISpec(0, 511)
        ext = ISpec(0, 0)
        return IExtSpec(dat, ext)

    def create_impl(self, mach):
        dat = ISpec(0, 10)
        ext = ISpec(0, 2)
        inner = IExtSpec(dat, ext)
        freemaparray = FreshDiskArray('freemaparray')
        return BlockImpl(inner, AsyncDisk(mach, freemaparray), BitmapSpec, Allocator64)

    def equivalence(self, spec, impl, **kwargs):
        ino = FreshBitVec('ino', 32)
        off = FreshBitVec('off', 32)

        return ForAll([ino, off],
                spec.read(ino, off) == impl.read(ino, off))


    @test.z3_option(AUTO_CONFIG=False)
    def match_bmap(self):
        ino = FreshBitVec('ino', 32)
        off = FreshBitVec('off', 32)
        yield (ino, off)

    def test_create_spec(self):
        ISpec(0, 511)

    def test_spec_alloc(self):
        spec = ISpec(0, 511)

        block = spec.alloc()
        self.prove(Not(spec.is_free(block)))

    def test_spec_alloc_unalloc(self):
        spec = ISpec(0, 511)

        block = spec.alloc()
        spec.free(block)
        self.prove(spec.is_free(block))

    def test_spec_bmap_simple(self):
        spec = ISpec(0, 511)

        ino = FreshIno('ino')
        off = FreshBitVec('off', 32)

        spec.bunmap(ino, off)
        self.prove(Not(spec.is_mapped(ino, off)))

    def test_spec_bmap_simple2(self):
        spec = ISpec(0, 511)

        ino = FreshIno('ino')
        off = BitVecVal(0, 32)

        spec.bmap(ino, off)
        self.pprove(spec.is_mapped(ino, off))

    def test_spec_bmap(self):
        spec = ISpec(0, 511)

        ino = FreshIno('ino')
        off = FreshBitVec('off', 32)
        block = FreshBlock('block')

        assertion(ULT(off, 512))

        spec.bmap(ino, off)
        spec.write(ino, off, block)
        self.pprove(spec.read(ino, off) == block)


if __name__ == '__main__':
    test.main()
