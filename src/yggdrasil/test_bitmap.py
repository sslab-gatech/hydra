from z3 import *
from bitmap import *
import disk

from yggdrasil.util import *
from yggdrasil.diskspec import *
from yggdrasil import test


class TestBitmapDisk(test.RefinementTest):
    def create_impl(self, mach):
        array = FreshDiskArray('impl',
                domain=BitVecSort(SizeSort.size() - LogBlockSize))
        return BitmapDisk(AsyncDisk(mach, array))

    def create_spec(self, mach):
        array = FreshDiskArray('spec')
        return BitmapSpec(AsyncDisk(mach, array))

    def equivalence(self, spec, impl, *args, **kwargs):
        i = BitVec(fresh_name('i'), SizeSort.size())
        return ForAll([i], spec.is_set(i) == impl.is_set(i))

    @test.z3_option(AUTO_CONFIG=True)
    def match_set_bit(self):
        bid = BitVec(fresh_name('bid'), SizeSort.size())
        yield (bid,)

    @test.z3_option(AUTO_CONFIG=True)
    def match_unset_bit(self):
        bid = BitVec(fresh_name('bid'), SizeSort.size())
        yield (bid,)


if __name__ == '__main__':
    test.main()
