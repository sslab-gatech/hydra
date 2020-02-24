from z3 import *
import unittest

from yggdrasil.util import *
from yggdrasil.ufarray import *
from yggdrasil import test


def StoreIn(arr, bid, off, field):
    i = arr[bid]
    i = Store(i, off, field)
    return Store(arr, bid, i)


def UpdateIn(arr, bid, off, field):
    block = arr(bid)
    block[off] = field
    return arr.update(bid, block)


class ArrayTest(test.DiskTest):
    def setUp(self):
        pass

    # Update a whole fresh block
    def test_array_update(self):
        spec = Array('spec', BitVecSort(64), ArraySort(BitVecSort(9), BitVecSort(64)))
        impl = FreshDiskArray('impl')


        bid = BitVec('bid', 64)
        off = BitVec('off', 9)

        bid0 = BitVec('bid0', 64)

        dataimpl = FreshBlock('blockimpl')
        dataspec = Array('dataspec', BitVecSort(9), BitVecSort(64))

        dataoff = BitVec('off0', 9)

        pre = And(
                ForAll([dataoff], dataimpl[dataoff] == dataspec[dataoff]),
                ForAll([bid, off], spec[bid][off] == impl(bid)[off]))

        impl = impl.update(bid0, dataimpl)

        spec = Store(spec, bid0, dataspec)

        post = ForAll([bid, off], spec[bid][off] == impl(bid)[off])
        self.prove(Implies(pre, post))

    # Update a field within a block, write block back
    def test_block_update(self):
        spec = Array('spec', BitVecSort(64), ArraySort(BitVecSort(9), BitVecSort(64)))
        impl = FreshDiskArray('impl')

        bid = BitVec('bid', 64)
        off = BitVec('off', 9)

        bid0 = BitVec('bid0', 64)
        off0 = BitVec('off0', 9)
        field0 = BitVec('field', 64)

        pre = ForAll([bid, off], spec[bid][off] == impl(bid)[off])

        spec = StoreIn(spec, bid0, off0, field0)
        impl = UpdateIn(impl, bid0, off0, field0)

        post = ForAll([bid, off], spec[bid][off] == impl(bid)[off])

        self.prove(Implies(pre, post))


if __name__ == '__main__':
    test.main()
