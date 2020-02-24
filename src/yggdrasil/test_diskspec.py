from z3 import *
import disk

import unittest

from yggdrasil.util import *
from yggdrasil.diskspec import *
from yggdrasil import test


class AsyncDiskTest(test.DiskTest):
    def setUp(self):
        self._array = FreshDiskArray('array')

    def test_prefix(self):
        mach = Machine()
        d = AsyncDisk(mach, self._array)

        a = FreshBlock('block')
        b = FreshBlock('block')
        c = FreshBlock('block')

        d.write(5, a)
        d.write(6, b)
        d.write(7, c)

        d = d.crash(Machine())

        self.show(Implies(mach.assumption,
            And(d.read(5) == a, d.read(6) != b), d.read(7) != c))

    def test_reorder(self):
        mach = Machine()
        d = AsyncDisk(mach, self._array)

        a = FreshBlock('block')
        b = FreshBlock('block')
        c = FreshBlock('block')

        d.write(5, a)
        d.write(6, b)
        d.write(7, c)

        d = d.crash(Machine())

        self.show(Implies(mach.assumption,
                And(d.read(6) == a, d.read(5) != b)))

    def test_nocrash(self):
        mach = Machine()
        d = AsyncDisk(mach, self._array)

        a = FreshBlock('block')
        b = FreshBlock('block')
        c = FreshBlock('block')

        d.write(5, a)
        d.write(6, b)
        d.write(7, c)

        d = d.crash(Machine())

        self.show(Implies(mach.assumption,
                And(d.read(5) == a, d.read(6) == b, d.read(7) == c)))


    def test_write_two(self):
        mach = Machine()
        d = AsyncDisk(mach, self._array)
        i0 = FreshSize('i')
        i1 = FreshSize('i')
        x0 = FreshBlock('x')
        x1 = FreshBlock('x')
        assumption = i0 != i1

        oldv0 = d.read(i0)
        oldv1 = d.read(i1)

        d.write(i0, x0)
        d.write(i1, x1)

        newv0 = d.read(i0)
        newv1 = d.read(i1)
        self.prove(Implies(assumption, newv0 == x0))
        self.prove(Implies(assumption, newv1 == x1))

        d = d.crash(Machine())
        anyv0 = d.read(i0)
        anyv1 = d.read(i1)

        self.prove(Implies(assumption, And(
            Or(anyv0 == oldv0, anyv0 == x0),
            Or(anyv1 == oldv1, anyv1 == x1))))

    def test_flush(self):
        mach = Machine()
        d = AsyncDisk(mach, self._array)
        i0 = FreshSize('i')
        i1 = FreshSize('i')
        x0 = FreshBlock('x')
        x1 = FreshBlock('x')

        oldv0 = d.read(i0)
        oldv1 = d.read(i1)

        d.write(i0, x0)
        d.flush()
        d.write(i1, x1)

        d = d.crash(Machine())
        anyv0 = d.read(i0)
        anyv1 = d.read(i1)

        assumption = And(i0 != i1, x1 != oldv1, mach.assumption)
        self.prove(Implies(assumption, Implies(anyv1 == x1, anyv0 == x0)))


# implement SyncDisk on top of AsyncDisk
class FlushingDisk:
    def __init__(self, disk):
        self._disk = disk

    def write(self, bid, data):
        self._disk.write(bid, data)
        self._disk.flush()

    def flush(self):
        pass

    def read(self, bid):
        return self._disk.read(bid)

    def crash(self, mach):
        return self.__class__(self._disk.crash(mach))


class SyncDiskTest(test.DiskTest):
    # SyncDisk is an AsyncDisk
    def test_syncdisk_is_async(self):
        array = DiskSort(fresh_name('array'))
        bid = FreshSize('bid')
        data = FreshBlock('data')

        spec_mach = Machine()
        impl_mach = Machine()

        ad = AsyncDisk(spec_mach, array)
        ad.write(bid, data)

        sd = SyncDisk(impl_mach, array)
        sd.write(bid, data)

        self.prove(ForAll([array] + impl_mach.control,
                     Exists(spec_mach.control,
                            ForAll([bid], sd.read(bid) == ad.read(bid)))))

    # AsyncDisk+flush is a SyncDisk
    def test_flushingdisk_is_sync(self):
        array = DiskSort(fresh_name('array'))
        bid = FreshSize('bid')
        data = FreshBlock('data')

        spec_mach = Machine()
        impl_mach = Machine()

        sd = SyncDisk(spec_mach, array)
        sd.write(bid, data)

        fd = FlushingDisk(AsyncDisk(impl_mach, array))
        fd.write(bid, data)

        self.prove(ForAll([array] + impl_mach.control,
                     Exists(spec_mach.control,
                            ForAll([bid], sd.read(bid) == fd.read(bid)))))


# implement SyncDisk on top of AsyncDisk
class FlushingDisk:
    def __init__(self, disk):
        self._disk = disk

    def write(self, bid, data):
        self._disk.write(bid, data)
        self._disk.flush()

    def flush(self):
        pass

    def read(self, bid):
        return self._disk.read(bid)

    def crash(self, mach):
        return self.__class__(self._disk.crash(mach))


class SyncDiskTest(test.DiskTest):
    # SyncDisk is an AsyncDisk
    def test_syncdisk_is_async(self):
        array = FreshDiskArray('array')
        bid = FreshSize('bid')
        data = FreshBlock('data')

        spec_mach = Machine()
        impl_mach = Machine()

        ad = AsyncDisk(spec_mach, array)
        ad.write(bid, data)

        sd = SyncDisk(impl_mach, array)
        sd.write(bid, data)

        self.solve(ForAll(spec_mach.control, Not(
                            ForAll([bid], sd.read(bid) == ad.read(bid)))))

    # AsyncDisk+flush is a SyncDisk
    def test_flushingdisk_is_sync(self):
        array = FreshDiskArray('array')
        bid = FreshSize('bid')
        data = FreshBlock('data')

        spec_mach = Machine()
        impl_mach = Machine()

        sd = SyncDisk(spec_mach, array)
        sd.write(bid, data)

        fd = FlushingDisk(AsyncDisk(impl_mach, array))
        fd.write(bid, data)

        self.solve(ForAll(spec_mach.control, Not(
                   ForAll([bid], sd.read(bid) == fd.read(bid)))))


class VirtualAsyncDiskTest(test.RefinementTest):
    def create_spec(self, mach):
        array = FreshDiskArray('array')
        return AsyncDisk(mach, array)
       
    def create_impl(self, mach):
        array = FreshDiskArray('array')
        mapped = FreshDiskArray('mapped')
        return VirtualAsyncDisk(mach, mapped, array)

    def equivalence(self, spec, impl, **kwargs):
        vbn = Const(fresh_name('i'), spec.domain())
        return ForAll([vbn],
                Implies(impl.is_mapped(vbn),
                    impl.read(vbn) == spec.read(vbn)))

    def match_write(self, spec=None, impl=None):
        i0 = FreshSize('i')
        x0 = FreshBlock('x')
        yield (i0, x0)


class InodeSpecTest(test.DiskTest):
    def setUp(self):
        disk.native = False
        disk.assertion.assertions = []
        self._array = FreshDiskArray('array')
        self._attr_array = FreshDiskArray('attrs')
        self._mapped = FreshDiskArray('mapped')

    def test_compare_to_async(self):
        d1_mach = Machine()
        d1 = AsyncDisk(d1_mach, self._array)

        d2_mach = Machine()
        d2 = InodeSpec(d2_mach, [self._mapped, self._attr_array, self._array])

        vbn = Const(fresh_name('i'), d1.domain())

        i0 = Const(fresh_name('i'), d1.domain())
        x0 = FreshBlock('x')

        pre = ForAll([vbn], Implies(d2.is_mapped(vbn), d1.read(vbn) == d2.read(vbn)))

        d1.write(i0, x0)
        d2.begin_tx()
        d2.write(i0, x0)
        d2.commit_tx()

        d1 = d1.crash(Machine())
        d2 = d2.crash(Machine())

        assumption = And(d1_mach.assumption, d2_mach.assumption)

        post = ForAll([vbn], Implies(d2.is_mapped(vbn), d1.read(vbn) == d2.read(vbn)))

        C1 = d1_mach.control

        self.solve(
                ForAll(C1, Not(
                Implies(assumption, Implies(pre, post)))))

    def test_bmap(self):
        self.enable_symbolic_execution()

        mach = Machine()
        d2 = InodeSpec(mach, [self._mapped, self._attr_array, self._array])

        vbn = FreshSize('vbn')

        mapped = d2.is_mapped(vbn)

        d2.begin_tx()
        d2.bmap(vbn)
        d2.commit_tx()

        self.prove(Implies(And(*disk.assertion.assertions), Implies(Not(mapped), d2.is_mapped(vbn))))

    def test_bunmap(self):
        mach = Machine()
        d2 = InodeSpec(mach, [self._mapped, self._attr_array, self._array])

        vbn = FreshSize('vbn')

        mapped = d2.is_mapped(vbn)

        d2.begin_tx()
        d2.bunmap(vbn)
        d2.commit_tx()

        self.prove(Implies(mapped, Not(d2.is_mapped(vbn))))


if __name__ == '__main__':
    test.main()
