from partition import Partition
import disk

from yggdrasil.diskspec import *
from yggdrasil import test


class PartitionRefinement(test.RefinementTest):
    def equivalence(self, spec, impl, **kwargs):
        bid = FreshSize('eq-bid')
        return ForAll([bid], And(
            Implies(impl[0].valid(bid), spec[0].read(bid) == impl[0].read(bid)),
            Implies(impl[1].valid(bid), spec[1].read(bid) == impl[1].read(bid)),
        ))

    def crash(self, p):
        return [p[0].crash(Machine()), p[1].crash(Machine())]

    crash_impl = crash
    crash_spec = crash

    def create_spec(self, mach):
        a1 = FreshDiskArray('spec-array-1')
        a2 = FreshDiskArray('spec-array-2')
        d1 = AsyncDisk(mach, a1)
        d2 = AsyncDisk(mach, a2)
        return [d1, d2]

    def create_impl(self, mach):
        a1 = FreshDiskArray('impl-array')
        d1 = AsyncDisk(mach, a1)
        a = FreshSize('start')
        b = FreshSize('mid')
        c = FreshSize('end')

        disk.assertion(ULT(a, b))
        disk.assertion(ULT(b, c))

        p1 = Partition(d1, a, b)
        p2 = Partition(d1, b, c)

        return [p1, p2]

    def call_write(self, spec, impl, args):
        dev, bid, block = args

        if dev == 0:
            spec[0].write(bid, block)
            impl[0].write(bid, block)
        else:
            spec[1].write(bid, block)
            impl[1].write(bid, block)

    @test.z3_option(AUTO_CONFIG=False)
    def match_write(self):
        dev = FreshSize('dev')
        bid = FreshSize('bid')
        block = FreshBlock('block')

        disk.assertion(ULT(dev, 2))
 
        yield (dev, bid, block)
    match_write.debug = True


if __name__ == '__main__':
    test.main()
