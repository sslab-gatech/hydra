import errno
from lfs import LFS
from disk import assertion, debug, Stat

from yggdrasil.diskspec import *
from yggdrasil import test


class LFSSpec(object):
    def __init__(self, mach, dirfn, parentfn, modefn, mtimefn):
        self._mach = mach
        self._dirfn = dirfn
        self._modefn = modefn
        self._mtimefn = mtimefn
        self._parentfn = parentfn

    def lookup(self, parent, name):
        ino = self._dirfn(parent, name)
        return If(0 < ino, ino, -errno.ENOENT)

    def get_attr(self, ino):
        return Stat(bsize=0,
                    mode=self._modefn(ino),
                    mtime=self._mtimefn(ino))

    def mknod(self, parent, name, mode, mtime):
        if 0 < self.lookup(parent, name):
            return BitVecVal(-errno.EEXIST, 64)

        on = self._mach.create_on([])

        ino = FreshBitVec('ino', 64)
        assertion(0 < ino)
        assertion(Not(0 < self._parentfn(ino)))

        self._dirfn = self._dirfn.update((parent, name), ino, guard=on)
        self._modefn = self._modefn.update(ino, mode, guard=on)
        self._mtimefn = self._mtimefn.update(ino, mtime, guard=on)
        self._parentfn = self._parentfn.update(ino, parent, guard=on)

        return ino

    def crash(self, mach):
        return self.__class__(mach, self._dirfn, self._parentfn, self._modefn, self._mtimefn)


class LFSRefinement(test.RefinementTest):
    def create_spec(self, mach):
        dirfn =  FreshUFunction('dirfn', SizeSort, SizeSort, SizeSort)
        parentfn =  FreshUFunction('parentfn', SizeSort, SizeSort)
        modefn =  FreshUFunction('modefn', SizeSort, SizeSort)
        mtimefn =  FreshUFunction('mtimefn', SizeSort, SizeSort)
        return LFSSpec(mach, dirfn, parentfn, modefn, mtimefn)

    def create_impl(self, mach):
        array = FreshDiskArray('disk')
        disk = AsyncDisk(mach, array)
        return LFS(disk)

    def pre_post(self, spec, impl, **kwargs):
        name = FreshBitVec('name.pre', 64)
        parent = BitVecVal(1, 64)

        sb = impl._disk.read(0)
        imap = impl._disk.read(sb[2])
        off = FreshBitVec('off', 9)

        pre = ForAll([name], Implies(name != 0, And(
            Implies(0 < spec._dirfn(parent, name),
                parent == spec._parentfn(spec._dirfn(parent, name))),

            Implies(0 < impl.lookup(parent, name),
                And(impl.lookup(parent, name) < sb[1],
                    spec.get_attr(spec.lookup(parent, name)) == impl.get_attr(impl.lookup(parent, name)))),
            spec.lookup(parent, name) == impl.lookup(parent, name))))

        pre = And(pre,
                ForAll([off], Implies(ZeroExt(64 - off.size(), off) < sb[1], And(0 < imap[off], imap[off] < sb[0]))))

        pre = And(pre,
                # allocated blocks are in range ]0..allocator[
                0 < sb[2], sb[2] < sb[0],
                0 < imap[1], imap[1] < sb[0],

                # root dir inode has been allocated
                1 < sb[1],
                )

        (spec, impl, (_, name0, _, _), (sino, iino)) = yield pre

        self.show(pre)

        if iino < 0:
            iino = impl.lookup(parent, name0)

        if self._solve(sino == iino):
            assertion(sino == iino)

        sb = impl._disk.read(0)
        imap = impl._disk.read(sb[2])

        post = ForAll([name], Implies(name != 0, And(
            Implies(0 < spec._dirfn(parent, name),
                parent == spec._parentfn(spec._dirfn(parent, name))),

            Implies(0 < impl.lookup(parent, name),
                And(impl.lookup(parent, name) < sb[1],
                    spec.get_attr(spec.lookup(parent, name)) == impl.get_attr(impl.lookup(parent, name)))),
            spec.lookup(parent, name) == impl.lookup(parent, name))))

        post = And(post,
                ForAll([off], Implies(ZeroExt(64 - off.size(), off) < sb[1], And(0 < imap[off], imap[off] < sb[0]))))

        post = And(post,
                # allocated blocks are in range ]0..allocator[
                0 < sb[2], sb[2] < sb[0],
                0 < imap[1], imap[1] < sb[0],

                # root dir inode has been allocated
                1 < sb[1],
                )

        yield post

    def match_mknod(self):
        parent = BitVecVal(1, 64)
        name = FreshBitVec('name', 64)
        mode = FreshBitVec('mode', 64)
        mtime = FreshBitVec('mtime', 64)
        assertion(name != 0)
        yield (parent, name, mode, mtime)

    # test

    # def test_foo(self):
    #     mach = Machine()
    #     impl = self.create_impl(mach)
    #
    #     parent = BitVecVal(1, 64)
    #     name = BitVecVal(0xdeadbeef, 64)
    #     mode = BitVecVal(0x1337, 64)
    #
    #     sb = impl._disk.read(0)
    #     imap = impl._disk.read(sb[2])
    #
    #     name0 = FreshSize('name')
    #
    #     pre = And(
    #             # inode alloc
    #             0 < sb[1], sb[1] < 512,
    #
    #             # allocated blocks are in range ]0..allocator[
    #             0 < sb[2], sb[2] < sb[0],
    #             0 < imap[1], imap[1] < sb[0],
    #             0 < imap[1], sb[2] < imap[1],
    #
    #             # root dir inode has been allocated
    #             1 < sb[1],
    #
    #             ForAll([name0],
    #                 Implies(0 < impl.lookup(parent, name0),
    #                     And(
    #                         impl.lookup(parent, name0) < sb[1],
    #                         imap[Extract(8, 0, impl.lookup(parent, name0))] < sb[0]))))
    #
    #     res = impl.mknod(parent, name, mode)
    #     if res < 0:
    #         pass
    #     else:
    #         ino = impl.lookup(parent, name)
    #         v = impl.get_attr(ino)
            # self.psolve(pre, v != mode)




if __name__ == '__main__':
    test.main()
