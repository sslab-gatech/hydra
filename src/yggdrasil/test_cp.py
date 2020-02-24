from disk import *

from yggdrasil.util import *
from yggdrasil.diskspec import *
from yggdrasil import test


class CPSpec(object):
    def __init__(self, ds):
        self._ds = ds

    def cp(self, sparent, sname, dparent, dname):
        # sparent/sname exists
        assertion(self._ds._direxists(sparent, sname))

        # dparent/dname does not exist
        assertion(Not(self._ds._direxists(dparent, dname)))

        # source inode
        sino = self._ds._dirfn(sparent, sname)

        assertion(self._ds.is_regular(sino))

        # destination inode
        dino = self._ds.ialloc(True)

        debug('sino', sino)
        debug('sino valid?', self._ds.is_valid(sino))
        debug('sino-mode', self._ds._attrs.mode(sino))

        # Set attributes for new inode
        self._ds._attrs.set_bsize(dino, 0)
        self._ds._attrs.set_mtime(dino, 0) #self._ds._attrs.mtime(sino))
        self._ds._attrs.set_mode(dino, 0) #self._ds._attrs.mode(sino))
        #
        # # associate parent x name -> ino
        self._ds._dirfn = self._ds._dirfn.update((dparent, dname), dino)
        self._ds._direxists = self._ds._direxists.update((dparent, dname), BoolVal(True))

        # self.write(dino, 0, 0, self._datafn(sino, 0, 0))

        return dino


class CPImpl(object):
    def __init__(self, di):
        self._di = di

    def cp(self, sparent, sname, dparent, dname):
        # sparent/sname exists
        assertion(self._di._direxists(sparent, sname))

        # dparent/dname does not exist
        assertion(Not(self._di._direxists(dparent, dname)))

        sino = self._di.lookup(sparent, sname)
        if sino == 0:
            return sino

        assertion(self._di.is_regular(sino))

        sattr = self._di.get_iattr(sino)

        dino = self._di.mknod(dparent, dname, 0, 0) #, sattr.mode, sattr.mtime)
        if dino < 0:
            return dino

        fsize = self._di.get_iattr(sino).bsize
        if fsize == 0:
            return dino
        else:
            assertion(False)
            return dino
            # dat = self.read(sino, 0, 0)
            #
            # b = ConstBlock(0)
            # b[0] = dat

            # self.write(dino, 0, 0, b)


class CPRefinement(test.RefinementTest):
    def _create_dir(self, mach):
        dirfn = FreshUFunction('dirfn', InoSort, NameSort, InoSort)
        direxists = FreshUFunction('direxists', InoSort, NameSort, BoolSort())
        datafn = FreshUFunction('datafn', InoSort, BitVecSort(32), BlockOffsetSort, SizeSort)
        ifree = FreshUFunction('ifreefn', InoSort, BoolSort())
        attrs = FreshAttributes()
        return DirSpec(mach, dirfn, direxists, datafn, ifree, attrs)

    def create_spec(self, mach):
        dspec = self._create_dir(mach)
        return dspec, CPSpec(dspec)

    def create_impl(self, mach):
        dspec = self._create_dir(mach)
        return dspec, CPImpl(dspec)

    def test_cp(self):
        spec_mach = Machine()
        spec, cp_spec = self.create_spec(spec_mach)

        impl_mach = Machine()
        impl, cp_impl = self.create_impl(impl_mach)

        ##

        ino = FreshIno('ino-pre')

        blocknum = FreshBitVec('blocknum-pre', 32)
        off = FreshBitVec('off-pre', BlockOffsetSort.size())

        pre = ForAll([ino], And(
                spec.is_valid(ino) == impl.is_valid(ino),

                # General inode properties
                Implies(spec.is_valid(ino), And(
                    spec.get_iattr(ino) == impl.get_iattr(ino))),

                # Non-directory properties
                Implies(Not(spec.is_dir(ino)), And(
                    spec.read(ino, blocknum, off) == impl.read(ino, blocknum, off),
                )),

                # Directory properties
                Implies(spec.is_dir(ino), And(
                    # spec.lookup(ino, name) == impl.lookup(ino, name))),
                ))
        ))

        self.show(And(*assertion.assertions), pre)

        ########################

        sparent0 = FreshIno('sparent0')
        sname0 = FreshName('sname0')

        dparent0 = FreshIno('dparent0')
        dname0 = FreshName('dname0')

        sino = cp_spec.cp(sparent0, sname0, dparent0, dname0)
        iino = cp_impl.cp(sparent0, sname0, dparent0, dname0)

        assertion(sino == iino)

        #########################

        ino = FreshIno('ino-post')

        post = ForAll([ino], And(
                spec.is_valid(ino) == impl.is_valid(ino),

                # General inode properties
                Implies(spec.is_valid(ino), And(
                    spec.get_iattr(ino) == impl.get_iattr(ino))),

                Implies(Not(spec.is_dir(ino)), And(
                    spec.read(ino, blocknum, off) == impl.read(ino, blocknum, off),
                )),

                # Directory properties
                Implies(spec.is_dir(ino), And(
                    # spec.lookup(ino, name) == impl.lookup(ino, name)
                ))
        ))

        self.psolve(Not(Implies(pre, post)))


if __name__ == '__main__':
    test.main()
