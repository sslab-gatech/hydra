from z3 import *
from util import *
from ufarray import *
import traceback
from collections import namedtuple


# Abstract machine model: multiple disks will share the same
# ordering constraints.  It captures two types of constraints:
# - a synced being true implies the current "on" being true;
# - the current "on" being true implies that all the previous
#   "on"s being true.
class Machine(object):
    def __init__(self):
        self._on = None
        self._control = []
        self._ordering = []
        self._stacks = {}

        self._ons = []
        self._flushes = []

    def __fresh_bool(self, name):
        b = Bool(fresh_name(name))

        self._stacks[b.sexpr()] = traceback.extract_stack()[:-2]

        self._control.append(b)
        if self._on is not None:
            self._ordering.append(Implies(b, self._on))
        return b

    # Create a sync bool variable, which is conditioned on "on".
    def create_synced(self):
        synced = self.__fresh_bool("synced")
        self._flushes.append(synced)
        return synced

    @property
    def on(self):
        if self._on:
            return self._on
        return BoolVal(True)

    # Create a new "on", which is conditioned on previous "on".
    # It also implies that pending synced's must be true.
    def create_on(self, syncs):
        self._on = self.__fresh_bool("on")
        self._ons.append(self._on)
        for s in syncs:
            self._ordering.append(Implies(self._on, s))
        return self._on

    def add_control(self, c):
        self._control.append(c)

    @property
    def control(self):
        return self._control

    @property
    def assumption(self):
        if len(self._ordering) == 0:
            return True
        return And(*self._ordering)

    def explain(self, model):
        print "<<<<<<<<<<<<<<<<<<"

        print "# Outstanding writes"
        for i in self._flushes:
            if not model.evaluate(i).eq(BoolVal(True)):
                print ' '.join(map(str, self._stacks[i.sexpr()][-2]))

        for i in self._ons:
            if not model.evaluate(i).eq(BoolVal(True)):
                print "# Crash point"
                for i in self._stacks[i.sexpr()]:
                    i = ' '.join(map(str, i))
                    if '/usr/lib/' in i:
                        continue
                    print i
                break
        print ">>>>>>>>>>>>>>>>>>"


class AsyncDisk(object):
    def __init__(self, mach, disk, cache=None):
        self._mach = mach
        # cache is a partial evaluation of _disk without any ites
        # Not required for correctness
        self._disk = disk
        self._dirty = []
        if cache is None:
            self._cache = disk
            self._docache = False
        else:
            self._cache = cache
            self._docache = True

    def _write(self, synced, bid, data, guard=BoolVal(True)):
        self._cache = self._cache.update(bid, data, guard)
        self._disk = self._disk.update(bid, data, synced, guard)
        self._dirty.append(synced)

    # cache is updated; disk is either updated or unchanged
    def write(self, bid, data, guard=BoolVal(True)):
        synced = self._mach.create_synced()
        self._write(synced, bid, data, guard)

    # flush is a full barrier
    def flush(self):
        on = self._mach.create_on(self._dirty)
        if self._docache:
            self._disk = If(on, self._disk, self._cache)
        self._dirty = []

    def read(self, bid):
        return self._cache(bid)

    # create a new copy of non-deterministic disk state
    # note that this captures all the previous states
    def crash(self, mach):
        return self.__class__(mach, self._disk)

    def domain(self):
        return self._disk.domain()


class VirtualAsyncDisk(AsyncDisk):
    def __init__(self, mach, mapped, disk):
        super(VirtualAsyncDisk, self).__init__(mach, disk)
        self._mapped = mapped
        self._mapped_cache = mapped

    def read(self, vbn):
        datablock = super(VirtualAsyncDisk, self).read(vbn)
        return If(self.is_mapped(vbn), datablock, ConstBlock(0))

    def is_mapped(self, vbn):
        return self._mapped_cache(vbn)[0] == 1

    def bmap(self, vbn, val=1):
        on = self._mach.create_on([])
        self._write(on, vbn, ConstBlock(0))
        self._mapped = self._mapped.update(vbn, ConstBlock(val), on)
        self._mapped_cache = self._mapped_cache.update(vbn, ConstBlock(val))

    def bunmap(self, bid):
        self.bmap(bid, 0)

    # create a new copy of non-deterministic disk state
    # note that this captures all the previous states
    def crash(self, mach):
        return self.__class__(mach, self._mapped, self._disk)


# Transactional disk - writev is atomic and persistent (sequential).
class TxnDisk(object):
    def __init__(self, mach, disk):
        self._mach = mach
        self._cache = disk
        self._disk = disk
        self._txn = None

    def begin_tx(self):
        assert self._txn is None
        self._txn = []

    def write_tx(self, lba, data):
        # write_tx updates the cache
        self._cache = self._cache.update(lba, data)
        self._txn.append((lba, data))

    def commit_tx(self):
        self.writev(self._txn)
        self._txn = None

    def writev(self, iov):
        on = self._mach.create_on([])
        disk = self._disk
        for lba, data in iov:
            self._cache = self._cache.update(lba, data)
            self._disk = self._disk.update(lba, data, on)

    def read(self, bid):
        return self._cache(bid)

    def crash(self, mach):
        return self.__class__(mach, self._disk)

    def domain(self):
        return self._disk.domain()


class MultiTxnDisk(object):
    def __init__(self, mach, disks):
        self._mach = mach
        self._caches = disks[::]
        self._disks = disks[::]
        self._txn = None

    def begin_tx(self):
        assert self._txn is None
        self._txn = []

    def write_tx(self, dev, lba, data):
        self._caches[dev] = self._caches[dev].update(lba, data)
        self._txn.append((dev, lba, data))

    def commit_tx(self):
        self.writev(self._txn)
        self._txn = None

    def writev(self, iov):
        on = self._mach.create_on([])
        for dev, lba, data in iov:
            if isinstance(dev, int):
                self._caches[dev] = self._caches[dev].update(lba, data)
                self._disks[dev] = self._disks[dev].update(lba, data, on)
            else:
                for idev in range(len(self._disks)):
                    self._caches[idev] = self._caches[idev].update(lba, data, dev==idev)
                    self._disks[idev] = self._disks[idev].update(lba, data, on, dev==idev)

    def read(self, dev, bid):
        return self._caches[dev](bid)

    def crash(self, mach):
        return self.__class__(mach, self._disks)

    def domain(self, dev):
        return self._disks[dev].domain()

    def range(self, dev):
        return self._disks[dev].range()


class Stat(object):
    def __init__(self, size, mtime, mode, nlink=0):
        self.size = size
        self.mtime = mtime
        self.mode = mode
        self.nlink = nlink

    @property
    def bsize(self):
        return Extract(63, 32, self.size)

    @property
    def fsize(self):
        return Extract(31, 0, self.size)

    def __str__(self):
        return "Stat(size=%s, mtime=%s, mode=%s, nlink=%s)" % (self.size, self.mtime, self.mode, self.nlink)

    def __getitem__(self, idx):
        if idx == 0:
            return self.size
        if idx == 1:
            return self.mtime
        if idx == 2:
            return self.mode
        if idx == 3:
            return self.nlink
        raise IndexError("")

    def __eq__(self, other):
        return And(self.size == other.size,
                   self.mtime == other.mtime,
                   self.mode == other.mode,
                   self.nlink == other.nlink)


class InodeSpec(MultiTxnDisk):
    MAPPED = 0
    ATTRS = 1
    DATA = 2

    def set_iattr(self, ino, attr):
        data = ConstBlock(0)
        data[0] = attr[0]
        data[1] = attr[1]
        data[2] = attr[2]
        data[3] = attr[3]
        self.write_tx(self.ATTRS, ino, data)

    def get_iattr(self, ino):
        data = self._read(self.ATTRS, ino)
        return Stat(data[0], data[1], data[2], data[3])

    def is_mapped(self, vbn):
        return self._read(self.MAPPED, vbn)[0] == 1

    def mappingi(self, vbn):
        return vbn

    def _read(self, *args, **kwargs):
        return super(InodeSpec, self).read(*args, **kwargs)

    def read(self, lbn):
        return self._read(self.DATA, lbn)

    def write(self, lbn, data):
        return self.write_tx(self.DATA, lbn, data)

    def bmap(self, vbn):
        if self.is_mapped(vbn):
            return vbn

        self.write(vbn, ConstBlock(0))
        self.write_tx(self.MAPPED, vbn, ConstBlock(1))

        return vbn

    def bunmap(self, vbn):
        self.write_tx(self.MAPPED, vbn, ConstBlock(0))


class RangeVirtualTxnDisk(MultiTxnDisk):
    MAPPED = 0
    DATA = 1

    def __init__(self, start, end, *args, **kwargs):
        self._start = start
        self._end = end
        super(RangeVirtualTxnDisk, self).__init__(*args, **kwargs)

    def is_mapped(self, vbn):
        # start <= vbn < end
        return And(ULE(self._start, vbn), ULT(vbn, self._end), self.read(self.MAPPED, vbn)[0] == 1)

    def readm(self, vbn):
        return If(self.is_mapped(vbn), self.read(self.DATA, vbn), ConstBlock(0))

    def writem(self, vbn, data):
        self.write_tx(self.MAPPED, vbn, ConstBlock(1))
        self.write_tx(self.DATA, vbn, data)

    def bmap(self, vbn):
        if self.is_mapped(vbn):
            return vbn

        self.write_tx(self.MAPPED, vbn, ConstBlock(1))
        self.write_tx(self.DATA, vbn, ConstBlock(0))

    def bunmap(self, vbn):
        self.write_tx(self.MAPPED, vbn, ConstBlock(0))

    def crash(self, mach):
        return self.__class__(self._start, self._end, mach, self._disks)


class SyncDisk:
    def __init__(self, mach, disk):
        self._mach = mach
        self._cache = disk
        self._disk = disk

    def write(self, bid, data):
        self._cache = self._cache.update(bid, data)
        on = self._mach.create_on([])
        self._disk = self._disk.update(bid, data, on)

    def flush(self):
        pass

    def read(self, bid):
        return self._cache(bid)

    def crash(self, mach):
        return self.__class__(mach, self._disk)


class BitmapSpec(object):
    def __init__(self, disk):
        self._disk = disk

    def is_set(self, bit):
        return self._disk.read(bit)[0] == 1

    def set_bit(self, bit):
        return self._disk.write(bit, ConstBlock(1))

    def unset_bit(self, bit):
        return self._disk.write(bit, ConstBlock(0))

    def crash(self, mach):
        return BitmapSpec(self._disk.crash(mach))


class InodePackSpec(object):
    def __init__(self, metadisk, datadisk):
        self._metadisk = metadisk
        self._datadisk = datadisk

    def read(self, bid):
        return None

    def set_mapping(self, ino, off, ptr, block=None):
        block = self._datadisk.read(ino)
        block[off] = ptr
        self._datadisk.write(ino, block)

    def get_mapping(self, ino, off, block=None):
        return self._datadisk.read(ino)[off]

    def set_iattr(self, ino, attr, block=None):
        data = ConstBlock(0)
        data[0] = attr[0]
        data[1] = attr[1]
        data[2] = attr[2]
        data[3] = attr[3]
        self._metadisk.write(ino, data)

    def get_iattr(self, ino, block=None):
        data = self._metadisk.read(ino)
        return Stat(data[0], data[1], data[2], data[3])

    def crash(self, mach):
        return self.__class__(self._metadisk.crash(mach), self._datadisk.crash(mach))


class Allocator64(object):
    def __init__(self, _readfn, _start, _end):
        pass

    def alloc(self):
        return BitVec(fresh_name('alloc'), 64)


class Allocator32(object):
    def __init__(self, _readfn, _start, _end):
        pass

    def alloc(self):
        return BitVec(fresh_name('alloc'), 32)

