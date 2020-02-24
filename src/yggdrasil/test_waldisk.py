from z3 import *
import disk
from waldisk import *
import unittest
import itertools

from yggdrasil.ufarray import *
from yggdrasil.util import *
from yggdrasil.diskspec import *
from yggdrasil import test


class WALDiskTestRefinement(test.RefinementTest):
    def create_spec(self, mach):
        dataarray1 = FreshDiskArray('dataarray')
        dataarray2 = FreshDiskArray('dataarray')
        return MultiTxnDisk(mach, [dataarray1, dataarray2])

    def create_impl(self, mach, logarray=None):
        if logarray is None:
            logarray = ConstDiskArray(ConstBlock(0))
        dataarray1 = FreshDiskArray('dataarray')
        dataarray2 = FreshDiskArray('dataarray')
        logdisk = AsyncDisk(mach, logarray)
        datadisk1 = AsyncDisk(mach, dataarray1)
        datadisk2 = AsyncDisk(mach, dataarray2)
        return WALDisk(logdisk, [datadisk1, datadisk2], osync=False)

    def equivalence_volatile(self, spec, impl, **kwargs):
        bid = FreshSize('bid')
        return ForAll([bid], And(
            spec.read(0, bid) == impl.read(0, bid),
            spec.read(1, bid) == impl.read(1, bid)))

    def equivalence_durable(self, spec, impl, **kwargs):
        bid = FreshSize('bid')
        return ForAll([bid], And(
            spec._disks[0](bid) == impl._datadisks[0].read(bid),
            spec._disks[1](bid) == impl._datadisks[1].read(bid)))

    equivalence = equivalence_volatile

    def call_write_tx_nocommit(self, spec, impl, args):
        impl.begin_tx()
        spec.begin_tx()

        for arg in args[0]:
            impl.write_tx(*arg)
            spec.write_tx(*arg)

    def call_write_tx(self, spec, impl, args):
        impl.begin_tx()
        spec.begin_tx()

        for arg in args[0]:
            impl.write_tx(*arg)
            spec.write_tx(*arg)

        impl.commit_tx()
        spec.commit_tx()

    def _gen_iov(self, *args, **kwargs):
        for n in range(WALDisk.LOG_MAX_ENTRIES + 1):
            iov = []
            for i in range(n):
                iov.append((1, FreshSize('i'), FreshBlock('x')))
            yield (iov,)

    # Verify writev
    match_writev = _gen_iov
    match_write_tx = _gen_iov
    match_write_tx_nocommit = lambda self, *args, **kwargs: self._gen_iov(*args, **kwargs)
    match_write_tx_nocommit.nocrash = True

    # recover-full(recover-partial(d)) = recover-full(d)
    def test_idempotent_recovery(self):
        mach = Machine()
        logarray = FreshDiskArray('logarray')
        d = self.create_impl(mach, logarray=logarray)
        assumption = mach.assumption

        # Recovery post-condition..
        self.solve(assumption, mach._on,
                d._logdisk._disk(0)[0] != 0)

        # block i after a full recovery
        i = FreshSize('i')
        x = d.read(0, i)

        # block i after a partial recovery followed by a full recovery
        mach = Machine()
        y = d.crash(mach).read(0, i)
        assumption = And(assumption, mach.assumption)

        self.solve(assumption, Not(x == y))

    def test_atomic(self):
        for i in range(2):
            self.__test_atomic(i + 1)

    def __test_atomic(self, n):
        mach = Machine()
        d = self.create_impl(mach)

        bids = [FreshSize('i') for i in range(n)]
        xs = [FreshBlock('x') for i in range(n)]
        iov = zip(itertools.repeat(0), bids, xs)

        oldvs = [d.read(0, bid) for bid in bids]
        d.writev(iov)

        assumption = And(Distinct(*bids), mach.assumption)

        # reboot & recovery
        mach = Machine()
        d = d.crash(mach)
        anyvs = [d.read(0, bid) for bid in bids]

        self.prove(Implies(assumption, Or(
            And(*[anyv == oldv for anyv, oldv in zip(anyvs, oldvs)]),
            And(*[anyv == x for anyv, x in zip(anyvs, xs)]))))


if __name__ == '__main__':
    test.main()
