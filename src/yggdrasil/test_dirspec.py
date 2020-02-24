from dirinode import DirImpl
from dirspec import *

from yggdrasil import test


class DirRefinementTest(test.RefinementTest):
    def create_spec(self, mach):
        dirfn = FreshUFunction('dirfn', InoSort, NameSort, InoSort)
        direxists = FreshUFunction('direxists', InoSort, NameSort, BoolSort())
        datafn = FreshUFunction('datafn', InoSort, BitVecSort(32), BlockOffsetSort, SizeSort)
        ifree = FreshUFunction('ifreefn', InoSort, BoolSort())
        attrs = FreshAttributes()

        return DirSpec(mach, dirfn, direxists, datafn, ifree, attrs)

    def create_impl(self, mach):
        mappedarray = FreshDiskArray('mappedarray')
        attr_array = FreshDiskArray('attrs', domain=BitVecSort(32))
        diskarray = FreshDiskArray('diskarray')

        ifreearray = FreshDiskArray('ifreearray', domain=BitVecSort(32))
        orphans = FreshDiskArray('orphans')

        inode = InodeSpec(mach, [mappedarray, attr_array, diskarray, None, ifreearray, orphans])
        return DirImpl(inode, inode, Allocator32, BitmapSpec, DirLook)

    def equivalence(self, spec, impl, **kwargs):
        ino = FreshIno('ino-pre')
        blocknum = FreshBitVec('blocknum-pre', 32)
        off = FreshBitVec('off-pre', BlockOffsetSort.size())
        mapped_block = FreshBitVec('mapped-block-pre', 32)

        return ForAll([ino, mapped_block, blocknum, off], And(
                # The bsize of an inode should not overflow when multiplied by 4k
                ULT(spec._attrs.bsize(ino), 1048576),

                ULE(spec._attrs.fsize(ino), spec._attrs.bsize(ino) * 4096),

                Implies(ULT(0, spec._attrs.bsize(ino)),
                    ULT(spec._attrs.bsize(ino) * 4096 -  4096, spec._attrs.fsize(ino))),

                impl.is_valid(ino) == spec.is_valid(ino),

                # GCable
                Implies(impl.is_gcable(ino),
                    And(
                        spec._ifreefn(ino),
                        spec._attrs.bsize(ino) == 0,
                        spec._attrs.nlink(ino) == 0,
                        spec.read(ino, blocknum, off) == 0)),

                # General inode properties
                Implies(spec.is_valid(ino), And(
                    spec.get_iattr(ino) == impl.get_iattr(ino))),

                Implies(
                    impl.is_ifree(ino),
                    Not(impl._inode.is_mapped(Concat(ino, mapped_block)))),

                Implies(ULE(impl.get_iattr(ino).bsize, mapped_block),
                        Not(impl._inode.is_mapped(Concat(ino, mapped_block)))),

                # Non-directory properties
                Implies(Not(spec.is_dir(ino)), And(
                    Implies(Not(impl.is_gcable(ino)),
                        spec.read(ino, blocknum, off) == impl.read(ino, blocknum)[off]),
                    )),
                ))

    def call_mknod(self, spec, impl, args):
        i1 = spec.mknod(*args)
        i2, err2 = impl.mknod(*args)
        assertion(i1 != 0)
        assertion(i1 == i2)

    @test.z3_option(AUTO_CONFIG=False)
    def match_mknod(self):
        parent0 = FreshIno('parent')
        name0 = [FreshName('name-pre') for _ in range(16)]

        mode = FreshSize('mode')
        mtime = FreshSize('mtime')

        yield (parent0, name0, mode, mtime)

    # @test.z3_option(AUTO_CONFIG=True)
    # def match_write(self):
    #     ino0 = FreshIno('ino0-write')
    #     blocknum0 = BitVec('blocknum0-write', 32)
    #     v0 = FreshBlock('v0-write')
    #
    #     yield (ino0, blocknum0, v0)

    @test.z3_option(AUTO_CONFIG=False)
    def match_rename(self):
        oparent0 = FreshIno('oparent0-write')
        nparent0 = FreshIno('nparent0-write')

        oname0 = [FreshName('oname0-write') for _ in range(16)]
        nname0 = [FreshName('nname0-write') for _ in range(16)]

        yield (oparent0, oname0, nparent0, nname0)

    def call_unlink(self, spec, impl, args):
        i1 = spec.unlink(*args)
        i2 = impl.unlink(*args)
        assertion(i1 != 0)
        assertion(i1 == i2)

    @test.z3_option(AUTO_CONFIG=False)
    def match_unlink(self):
        parent0 = FreshIno('parent0')
        name0 = [FreshName('name0') for _ in range(16)]

        yield (parent0, name0)

    def call_rmdir(self, spec, impl, args):
        i1, err1 = spec.rmdir(*args)
        i2, err2 = impl.rmdir(*args)
        assertion(err1 == err2)
        assertion(Implies(err1 == 0, i1 != 0))
        assertion(i1 == i2)

    @test.z3_option(AUTO_CONFIG=False)
    def match_rmdir(self):
        parent0 = FreshIno('parent0')
        name0 = [FreshName('name0') for _ in range(16)]

        yield (parent0, name0)

    @test.z3_option(AUTO_CONFIG=False)
    def match_forget(self):
        ino0 = FreshIno('ino0')
        yield (ino0,)

    @test.z3_option(AUTO_CONFIG=False)
    def nop_gc1(self):
        orph_index0 = FreshIno('orph_index0')
        block_off0 = FreshIno('block_off')
        yield(orph_index0, block_off0)

    @test.z3_option(AUTO_CONFIG=False)
    def nop_gc2(self):
        orph_index0 = FreshIno('orph_index0')
        yield(orph_index0,)

    @test.z3_option(AUTO_CONFIG=False)
    def nop_gc3(self):
        yield ()


class DirRefinement(test.RefinementTest):
    def create_spec(self, mach):
        dirfn = FreshUFunction('dirfn', InoSort, NameSort, InoSort)
        direxists = FreshUFunction('direxists', InoSort, NameSort, BoolSort())
        datafn = FreshUFunction('datafn', InoSort, BitVecSort(32), BlockOffsetSort, SizeSort)
        ifree = FreshUFunction('ifreefn', InoSort, BoolSort())
        attrs = FreshAttributes()

        return DirSpec(mach, dirfn, direxists, datafn, ifree, attrs)

    def create_impl(self, mach):
        mappedarray = FreshDiskArray('mappedarray')
        attr_array = FreshDiskArray('attrs', domain=BitVecSort(32))
        diskarray = FreshDiskArray('diskarray')

        ifreearray = FreshDiskArray('ifreearray', domain=BitVecSort(32))
        orphans = FreshDiskArray('orphans')

        inode = InodeSpec(mach, [mappedarray, attr_array, diskarray, None, ifreearray, orphans])
        return DirImpl(inode, inode, Allocator32, BitmapSpec, DirLook)

    def test_match_write(self):
        spec_mach = Machine()
        spec = self.create_spec(spec_mach)

        impl_mach = Machine()
        impl = self.create_impl(impl_mach)

        ino = FreshIno('ino-pre')

        blocknum = FreshBitVec('blocknum-pre', 32)
        off = FreshBitVec('off-pre', BlockOffsetSort.size())

        mapped_block = FreshBitVec('mapped-block-pre', 32)

        pre = ForAll([ino], And(
                # The bsize of an inode should not overflow when multiplied by 4k
                ULT(spec._attrs.bsize(ino), 1048576),

                ULE(spec._attrs.fsize(ino), spec._attrs.bsize(ino) * 4096),

                Implies(ULT(0, spec._attrs.bsize(ino)),
                    ULT(spec._attrs.bsize(ino) * 4096 -  4096, spec._attrs.fsize(ino))),

                spec.is_valid(ino) == impl.is_valid(ino),

                # GCable
                Implies(impl.is_gcable(ino),
                    And(
                        spec._ifreefn(ino),
                        spec._attrs.bsize(ino) == 0,
                        spec._attrs.nlink(ino) == 0,
                        spec.read(ino, blocknum, off) == 0)),

                # General inode properties
                Implies(spec.is_valid(ino), And(
                    spec.get_iattr(ino) == impl.get_iattr(ino))),

                ForAll([mapped_block], Implies(
                    impl.is_ifree(ino),
                    Not(impl._inode.is_mapped(Concat(ino, mapped_block))))),

                # Non-directory properties
                Implies(Not(spec.is_dir(ino)), And(
                    Implies(Not(impl.is_gcable(ino)), spec.read(ino, blocknum, off) == impl.read(ino, blocknum)[off]),

                    ForAll([mapped_block],
                        Implies(ULE(impl.get_iattr(ino).bsize, mapped_block),
                            Not(impl._inode.is_mapped(Concat(ino, mapped_block))))),
                )),

                # Directory properties
                Implies(spec.is_dir(ino), And(
                    # spec.lookup(ino, name) == impl.lookup(ino, name))),
                ))
        ))

        # self.show(And(*assertion.assertions), pre, AUTO_CONFIG=False)

        #########################

        ino0 = FreshIno('ino0-write')
        blocknum0 = BitVec('blocknum0-write', 32)
        size0 = BitVec('size0-write', 32)
        v0 = FreshBlock('v0-write')

        spec.write(ino0, blocknum0, v0, size0)
        impl.write(ino0, blocknum0, v0, size0)

        ################

        spec = spec.crash(Machine())
        impl = impl.crash(Machine())

        ###############

        # ino = FreshIno('ino-post')

        post = ForAll([ino], And(
                # The bsize of an inode should not overflow when multiplied by 4k
                ULT(spec._attrs.bsize(ino), 1048576),

                ULE(spec._attrs.fsize(ino), spec._attrs.bsize(ino) * 4096),

                Implies(ULT(0, spec._attrs.bsize(ino)),
                    ULT(spec._attrs.bsize(ino) * 4096 -  4096, spec._attrs.fsize(ino))),

                spec.is_valid(ino) == impl.is_valid(ino),

                # GCable
                Implies(impl.is_gcable(ino),
                    And(
                        spec._ifreefn(ino),
                        spec._attrs.bsize(ino) == 0,
                        spec._attrs.nlink(ino) == 0,
                        spec.read(ino, blocknum, off) == 0)),

                # General inode properties
                Implies(spec.is_valid(ino), And(
                    spec.get_iattr(ino) == impl.get_iattr(ino))),

                ForAll([mapped_block], Implies(
                    impl.is_ifree(ino),
                    Not(impl._inode.is_mapped(Concat(ino, mapped_block))))),

                # Non-directory properties
                Implies(Not(spec.is_dir(ino)), And(
                    Implies(Not(impl.is_gcable(ino)), spec.read(ino, blocknum, off) == impl.read(ino, blocknum)[off]),

                    ForAll([mapped_block],
                        Implies(ULE(impl.get_iattr(ino).bsize, mapped_block),
                        Not(impl._inode.is_mapped(Concat(ino, mapped_block))))),
                )),
        ))

        opt = {
            'AUTO_CONFIG': False,
        }

        # self.psolve(Not(Implies(pre, post)), **opt)

        assumption = And(spec_mach.assumption, impl_mach.assumption, *assertion.assertions)

        # print z3.simplify(assumption)

        model = self._solve(assumption,
            ForAll(spec_mach.control, Not(Implies(pre, post))), **opt)

        if model:
            print 'Spec read', model.eval(spec.read(ino, blocknum, off))
            print 'Impl read', model.eval(impl.read(ino, blocknum)[off])

        self.assertIsNone(model)


if __name__ == '__main__':
    test.main()
