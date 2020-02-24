from z3 import *
import disk
from inodepack import InodePackDisk

from yggdrasil.ufarray import *
from yggdrasil.util import *
from yggdrasil.diskspec import *
from yggdrasil import test
from yggdrasil.test import *


def FreshAttr():
    size = FreshSize('bsize')
    mtime = FreshSize('mtime')
    mode = FreshSize('mode')
    nlink = FreshSize('nlink')
    return Stat(size, mtime, mode, nlink)


class InodePackDiskRefinement(RefinementTest):
    def create_spec(self, mach):
        dataarray = FreshDiskArray('dataarray', domain=BitVecSort(32))
        datadisk = AsyncDisk(mach, dataarray)
        metaarray = FreshDiskArray('metaarray', domain=BitVecSort(32))
        metadisk = AsyncDisk(mach, metaarray)
        return InodePackSpec(metadisk, datadisk)

    def create_impl(self, mach):
        array = FreshDiskArray('array', domain=BitVecSort(32))
        disk = AsyncDisk(mach, array)
        return InodePackDisk(disk, disk)

    def equivalence(self, spec, impl, **args):
        ino = FreshBitVec('ino', 32)
        ioff = FreshBitVec('ioff', 9)
        return ForAll([ino, ioff], Implies(ULT(ioff, 11), And(
                      spec.get_mapping(ino, ioff) == impl._get_mapping(ino, ioff),
                      spec.get_iattr(ino) == impl.get_iattr(ino)
                      )))

    def match_set_iattr(self):
        i0 = FreshBitVec('ino', 32)
        x0 = FreshAttr()
        yield (i0, x0)

    def match_set_mapping(self):
        i0 = FreshBitVec('ino', 32)
        off0 = FreshBitVec('off', 9)
        ptr0 = FreshBitVec('ptr', 64)
        yield (i0, off0, ptr0)


if __name__ == '__main__':
    test.main()
