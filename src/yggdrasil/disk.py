import z3
import diskimpl
import traceback
import symbolicmap

from yggdrasil import diskspec
from yggdrasil import ufarray
from yggdrasil import util


native = True


def _native(a, b):
    def inner(*args, **kwargs):
        if native:
            return a(*args, **kwargs)
        return b(*args, **kwargs)
    return inner


def AsyncDiskWrap(*args, **kwargs):
    mach = diskspec.Machine()
    array = ufarray.ConstDiskArray(ConstBlock(0))
    return diskspec.AsyncDisk(mach, array)


def ZConcat32(a, b):
    # Z3 concat of 32 bit numbers
    # Since the implementation only uses uint64's
    # we have to know the the sizes for the numbers being concated.
    assert a.size() == 32
    assert b.size() == 32
    return z3.Concat(a, b)

Block = diskimpl.Block

# TODO: More efficient native stat
Stat = _native(diskspec.Stat, diskspec.Stat)
AsyncDisk = _native(diskimpl.AsyncDisk, AsyncDiskWrap)
PartitionAsyncDisk = diskimpl.PartitionAsyncDisk
ConstBlock = _native(diskimpl.ConstBlock, ufarray.ConstBlock)
Extract = _native(diskimpl.Extract, z3.Extract)
ULE = _native(diskimpl.ULE, z3.ULE)
ULT = _native(diskimpl.ULT, z3.ULT)
UGT = _native(diskimpl.UGT, z3.UGT)
UGE = _native(diskimpl.UGE, z3.UGE)
If = _native(diskimpl.If, util.If)
Allocator32 = _native(diskimpl.Allocator, diskspec.Allocator32)
Allocator64 = _native(diskimpl.Allocator, diskspec.Allocator64)
Extend = _native(diskimpl.Extend, util.Extend)
BitVecVal = _native(diskimpl.BitVecVal, z3.BitVecVal)
Concat32 = _native(diskimpl.Concat32, ZConcat32)
Not = _native(diskimpl.Not, z3.Not)
And = _native(diskimpl.And, z3.And)
Or = _native(diskimpl.Or, z3.Or)
USub = _native(diskimpl.USub, lambda a, b: a - b)
URem = _native(diskimpl.URem, z3.URem)
UDiv = _native(diskimpl.UDiv, z3.UDiv)
LShR = _native(diskimpl.LShR, z3.LShR)
Dict = _native(diskimpl.Dict, symbolicmap.SymbolicMap)
UMax = _native(None, util.UMax)

def debug(msg, *var):
    if not native:
        if not hasattr(debug, 'debugs'):
            debug.debugs = []
        debug.debugs.append((msg, map(z3.simplify, var)))

def assertion(cond, msg=None):
    if native:
        if not cond:
            if msg:
                print "Assertion failure:", msg
            traceback.print_stack()
        assert cond
    else:
        if not hasattr(assertion, 'assertions'):
            assertion.assertions = []
        assertion.assertions.append(cond)
