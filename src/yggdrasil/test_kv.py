import disk

from kvspec import *
from kvimpl import *

from yggdrasil.diskspec import *
from yggdrasil import test


class KVTest(test.DiskTest):
    def setUp(self):
        disk.native = False
        disk.assertion.assertions = []

    def test_kvspec(self):
        mach = Machine()
        fn_size = Function(fresh_name('kvspec.size'), KeySort, StringOffsetSort)
        fn_data = Function(fresh_name('kvspec.data'), KeySort, StringOffsetSort, StringElementSort)
        db = KVSpec(mach, fn_size, fn_data)
        k = FreshKey('k')
        v = FreshString('v')
        db.put(k, v)
        self.prove(db.get(k) == v)

    def test_kvimpl(self):
        mach = Machine()
        disk = AsyncDisk(mach, FreshDiskArray('array'))
        sig = [StringElementSort] * KVImpl.KEY_SIZE + [SizeSort]
        f = Function(fresh_name('hash'), *sig)
        hashfn = lambda key: f(*[key[i] for i in range(KVImpl.KEY_SIZE)])
        db = KVImpl(disk, hashfn)
        k = FreshString('k', size=KVImpl.KEY_SIZE)
        v = FreshString('v')
        assumption = ULE(v.size(), KVImpl.VALUE_SIZE_LIMIT)
        assumption = And(mach.assumption, assumption)
        db.put(k, v)
        self.prove(Implies(assumption, db.get(k) == v))

if __name__ == '__main__':
    test.main()
