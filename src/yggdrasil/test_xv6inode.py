import disk
from xv6inode import *
import unittest

from yggdrasil.diskspec import *
from yggdrasil.util import *
from yggdrasil import test


def FreshAttr():
    bsize = FreshSize('bsize')
    mtime = FreshSize('mtime')
    mode = FreshSize('mode')
    nlink = FreshSize('nlink')
    return Stat(bsize, mtime, mode, nlink)


def attr_eq(a, b):
    return And(a.size == b.size,
               a.mtime == b.mtime,
               a.mode == b.mode)


class InodeDiskRefinement(test.RefinementTest):
    def create_spec(self, mach):
        mappedarray = FreshDiskArray('mappedarray')
        attr_array = FreshDiskArray('spec_attr', domain=BitVecSort(32))
        diskarray = FreshDiskArray('diskarray')
        return InodeSpec(mach, [mappedarray, attr_array, diskarray])

    def create_impl(self, mach):
        freemaparray = FreshDiskArray('freemaparray')
        inodemetaarray = FreshDiskArray('inodemetaarray', domain=BitVecSort(32))
        inodedataarray = FreshDiskArray('inodedataarray', domain=BitVecSort(32))
        diskarray = FreshDiskArray('diskarray')
        txndisk = MultiTxnDisk(mach, [freemaparray, inodemetaarray, inodedataarray, diskarray])
        return InodeDisk(txndisk, Allocator64, BitmapSpec, InodePackSpec)

    def pre_post(self, spec, impl, fnargs, *args, **kwargs):
        pre_reverse_map = Function(fresh_name('reverse_map'), SizeSort, SizeSort)

        bid = BitVec(fresh_name('bid'), 64)

        ino1 = BitVec(fresh_name('ino'), 32)
        off1 = BitVec(fresh_name('off'), 32)

        pre = ForAll([bid],
                And(
                    Or(impl.is_free(bid), Exists([ino1, off1], impl.mappingi(Concat(ino1, off1)) == bid)),

                    # mappings should be the same
                    spec.is_mapped(bid) == impl.is_mapped(bid),

                    # Contents of mapped blocks should be the same
                    Implies(impl.is_mapped(bid),
                        impl.read(impl.mappingi(bid)) == spec.read(spec.mappingi(bid))),

                    attr_eq(impl.get_iattr(Extract(63, 32, bid)), spec.get_iattr(Extract(63, 32, bid))),

                    # Mapping should be injective
                    Implies(impl.is_mapped(bid),
                        pre_reverse_map(impl.mappingi(bid)) == bid),

                    # Mapped blocks should be allocated
                    Implies(impl.is_mapped(bid), Not(impl.is_free(impl.mappingi(bid))))))

        pre = And(pre, ULT(fnargs[0], InodeDisk.NDIRECT))

        (spec, impl, args, (_, lbn)) = yield pre

        if len(args) == 1 and lbn is not None:
            post_reverse_map = lambda x: If(x == lbn, args[0], pre_reverse_map(x))
        else:
            post_reverse_map = pre_reverse_map

        post = ForAll([bid],
                And(
                    Or(impl.is_free(bid), Exists([ino1, off1], impl.mappingi(Concat(ino1, off1)) == bid)),

                    # mappings should be the same
                    spec.is_mapped(bid) == impl.is_mapped(bid),

                    # Contents of mapped blocks should be the same
                    Implies(impl.is_mapped(bid),
                        impl.read(impl.mappingi(bid)) == spec.read(spec.mappingi(bid))),

                    attr_eq(impl.get_iattr(Extract(63, 32, bid)), spec.get_iattr(Extract(63, 32, bid))),

                    # Mapping should be injective
                    Implies(impl.is_mapped(bid),
                        post_reverse_map(impl.mappingi(bid)) == bid),

                    # Mapped blocks should be allocated
                    Implies(impl.is_mapped(bid), Not(impl.is_free(impl.mappingi(bid))))))

        yield post

    def match_set_iattr(self):
        ino = BitVec(fresh_name('ino'), 32)
        attr = FreshAttr()
        yield (ino, attr)

    def _create_bid(self):
        bid = BitVec(fresh_name('ino'), 64)
        yield (bid,)

    match_bunmap = _create_bid
    match_bmap = _create_bid

    # Some regular tests below

    def test_bmap(self):
        mach = Machine()
        impl = self.create_impl(mach)

        ino = BitVec(fresh_name('ino'), 32)
        off = BitVec(fresh_name('off'), 32)
        vbn = Concat(ino, off)

        mapped = impl.is_mapped(vbn)

        pre = Not(mapped)
        pre = And(pre, ULT(off, InodeDisk.NDIRECT))

        impl.begin_tx()
        impl.bmap(vbn)
        impl.commit_tx()

        post = impl.is_mapped(vbn)

        pre = And(pre, *disk.assertion.assertions)

        self.prove(Implies(pre, post))

    def test_bunmap(self):
        mach = Machine()

        impl = self.create_impl(mach)

        ino = BitVec(fresh_name('ino'), 32)
        off = BitVec(fresh_name('off'), 32)
        vbn = Concat(ino, off)

        mapped = impl.is_mapped(vbn)

        pre = mapped
        pre = And(pre, ULT(off, InodeDisk.NDIRECT))

        impl.begin_tx()
        impl.bunmap(vbn)
        impl.commit_tx()

        post = Not(impl.is_mapped(vbn))

        pre = And(pre, *disk.assertion.assertions)

        self.prove(Implies(pre, post))


class IndirectInodeDiskRefinement(test.RefinementTest):
    def create_spec(self, mach):
        mappedarray = FreshDiskArray('mappedarray')
        attr_array = FreshDiskArray('spec_attr', domain=BitVecSort(32))
        diskarray = FreshDiskArray('diskarray')
        return InodeSpec(mach, [mappedarray, attr_array, diskarray])

    def create_impl(self, mach):
        freemaparray = FreshDiskArray('freemaparray')
        inodemetaarray = FreshDiskArray('inodemetaarray', domain=BitVecSort(32))
        inodedataarray = FreshDiskArray('inodedataarray', domain=BitVecSort(32))
        diskarray = FreshDiskArray('diskarray')
        txndisk = MultiTxnDisk(mach, [freemaparray, inodemetaarray, inodedataarray, diskarray])
        inodedisk = InodeDisk(txndisk, Allocator64, BitmapSpec, InodePackSpec)
        return IndirectInodeDisk(inodedisk)

    def pre_post(self, spec, impl, fnargs, *args, **kwargs):
        pre_reverse_map = Function(fresh_name('reverse_map'), SizeSort, SizeSort)

        # bid = BitVec(fresh_name('bid'), 64)
        # ino = Extract(63, 32, bid)
        # off = Extract(31, 0, bid)
        ino = FreshBitVec('ino', 32)
        off = FreshBitVec('off', 32)
        bid = Concat(ino, off)

        pre = ForAll([ino, off],
                And(
                    # Mapped blocks should be allocated
                    Implies(impl.is_mapped(bid), Not(impl.is_free(impl.mappingi(bid)))),
                    # Mapping should be injective
                    Implies(impl.is_mapped(bid), pre_reverse_map(impl.mappingi(bid)) == bid),

                    attr_eq(impl.get_iattr(ino), spec.get_iattr(ino)),

                    # mappings should be the same
                    Implies(off != InodeDisk.NDIRECT - 1,
                        And(
                            Implies(impl.is_mapped(bid),
                                impl.read(impl.mappingi(bid)) == spec.read(spec.mappingi(bid))),
                    
                            # mapping should be the same
                            spec.is_mapped(bid) == impl.is_mapped(bid)))))

        pre = And(pre, ULT(fnargs[0], InodeDisk.NDIRECT + IndirectInodeDisk.NINDIRECT))
        # pre = And(pre, ULT(fnargs[0], InodeDisk.NDIRECT))
        pre = And(pre, fnargs[0] != InodeDisk.NDIRECT - 1)

        if kwargs['fname'] == 'bmap' and kwargs['crash']:
            self.show(pre)
        elif kwargs['fname'] == 'bunmap':
            self.show(pre)

        (spec, impl, args, (_, lbn)) = yield pre

        lbn_indirect = impl.mappingi(Concat(ino, BitVecVal(InodeDisk.NDIRECT - 1, 32)))

        if len(args) == 1 and lbn is not None:
            post_reverse_map = lambda x: If(x == lbn,
                                            args[0],
                                            If(x == lbn_indirect,
                                                Concat(ino, BitVecVal(InodeDisk.NDIRECT - 1, 32)),
                                                pre_reverse_map(x)))
        else:
            post_reverse_map = pre_reverse_map

        post = ForAll([ino, off],
                And(
                    # Mapped blocks should be allocated
                    Implies(impl.is_mapped(bid), Not(impl.is_free(impl.mappingi(bid)))),
                    # # Mapping should be injective
                    Implies(impl.is_mapped(bid), post_reverse_map(impl.mappingi(bid)) == bid),

                    attr_eq(impl.get_iattr(ino), spec.get_iattr(ino)),

                    # mappings should be the same
                    Implies(off != InodeDisk.NDIRECT - 1,
                        And(
                            Implies(impl.is_mapped(bid),
                                impl.read(impl.mappingi(bid)) == spec.read(spec.mappingi(bid))),

                            # mapping should be the same
                            spec.is_mapped(bid) == impl.is_mapped(bid)
                            ))))

        yield post

    @test.z3_option(AUTO_CONFIG=False)
    def match_set_iattr(self):
        ino = BitVec(fresh_name('ino'), 32)
        attr = FreshAttr()
        yield (ino, attr)

    @test.z3_option(AUTO_CONFIG=False)
    def _create_bid(self):
        # bid = BitVec(fresh_name('ino'), 64)
        ino = FreshBitVec('ino', 32)
        off = FreshBitVec('off', 32)
        bid = Concat(ino, off)
        yield (bid,)

    match_bunmap = _create_bid
    # match_bunmap.debug = True
    match_bmap = _create_bid
    # match_bmap.debug = True


if __name__ == '__main__':
    test.main()
