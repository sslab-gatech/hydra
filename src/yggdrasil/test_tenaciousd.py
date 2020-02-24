import disk
from logspec import *
from tenaciousd import *

from yggdrasil.diskspec import *
from yggdrasil import test

class TenaciousdLogTest(test.DiskTest):
    def setUp(self):
        disk.native = False
        disk.assertion.assertions = []

    def test_append(self):
        spec_mach = Machine()
        datafn = Function(fresh_name('datafn'), SizeSort, SizeSort, BlockElementSort)
        sizefn = Function(fresh_name('sizefn'), SizeSort, SizeSort)
        nextfn = Function(fresh_name('nextfn'), SizeSort, SizeSort)
        tail = FreshSize('tailx')
        spec = LogSpec(spec_mach, datafn, sizefn, nextfn, tail)

        impl_mach = Machine()
        dataarray = FreshDiskArray('dataarray')
        impl = TenaciousdLog(SyncDisk(impl_mach, dataarray))

        i = FreshSize('i')
        off = FreshSize('off')

        spec_entry = spec[i]
        impl_entry = impl[i]

        pre = And(spec.tail == impl.tail, spec_entry[off] == impl_entry[off])

        spec_data = Function(fresh_name('spec_blk'), SizeSort, BlockElementSort)
        impl_data = [FreshBlock('impl_blk') for c in range(TenaciousdLog.LOG_MAX_ENTRY_BLOCKS)]
        n = FreshSize('n')

        x = FreshSize('x')
        inp = ForAll([x], LogSpecEntry(spec_data, n)[x] == LogImplEntry(impl_data)[x])

        spec.append(spec_data, n)
        impl.append(impl_data, n)

        spec = spec.crash(Machine())
        impl = impl.crash(Machine())

        spec_entry = spec[i]
        impl_entry = impl[i]

        post = And(spec.tail == impl.tail, spec_entry[off] == impl_entry[off])

        assumption = And(inp, ULT(0, i), ULE(off, 509), spec_mach.assumption, impl_mach.assumption, *disk.assertion.assertions)

        self.solve(assumption, ForAll(spec_mach.control, Not(Implies(pre, post))))

if __name__ == '__main__':
    test.main()
