import z3

from z3 import *
import collections
import os, sys, subprocess
import types

# Stupid thing required by a recent-ish z3 update
Z3_LIBRARY_PATH = os.environ.get('Z3_LIBRARY_PATH')
if Z3_LIBRARY_PATH:
    z3.init(Z3_LIBRARY_PATH)

sys.setrecursionlimit(9999)


def nop(*args, **kwargs):
    pass
setattr(z3.AstRef, '__del__', nop)


# block number (64 bits)
SizeSort  = BitVecSort(64)

# block data 32Kib
BlockSize = 32 * 1024
LogBlockSize = BlockSize.bit_length() - 1
BlockElementSort = BitVecSort(64)
BlockOffsetSort = BitVecSort(LogBlockSize - (BlockElementSort.size().bit_length() - 1))
BlockSort = ArraySort(BitVecSort(LogBlockSize - 6), BitVecSort(64))

# disk: block number -> block data
DevSort = BitVecSort(64)

PartitionedSizeSort = BitVecSort(SizeSort.size() + DevSort.size())

# Zero = BitVecVal(0, BlockSort.size())
#zero = K(BitVecSort(9), BitVecVal(0, 64))

def FreshBitVec(name, size):
    return BitVec(fresh_name(name), size)


def FreshBool(b):
    return Bool(fresh_name(b))


def If(cond, a, b):
    if hasattr(a, 'ite'):
        assert type(a) == type(b)
        return a.ite(b, cond)
    return z3.If(cond, a, b)


def bbf(p, start, end):
    return Extract(end - 1, start, p)


def Extend(x, size):
    return ZeroExt(size - x.size(), x)


def BitwiseBitField(p, start, end):
    start = Extend(start, p.size())
    end = Extend(end, p.size())
    return p >> start & ((1 << (end - start)) - 1)


def block_extend(d):
    return Store(zero, 0, d)
    # return ZeroExt(BlockSort.size() - d.size(), d)


# Unsigned max
def UMax(a, *args):
    if len(args) == 0:
        return a
    if len(args) == 1:
        b = args[0]
        return If(ULE(a, b), b, a)


def UMin(a, *args):
    if len(args) == 0:
        return a
    if len(args) == 1:
        b = args[0]
        return If(ULE(b, a), b, a)


def fresh_name(name):
    if not hasattr(fresh_name, "idx"):
        fresh_name.idx = {}
    n = fresh_name.idx.get(name, 0)
    fresh_name.idx[name] = n + 1
    return name + "." + str(n)


# def prove(claim, **keywords):
#     return solve(Not(claim), **keywords)


def solve(*args, **keywords):
    s = Solver()
    s.set(**keywords)
    s.add(*args)
    if keywords.get('show', False):
        print(s)
    smt = os.getenv('SMT')
    if smt:
        PIPE = subprocess.PIPE
        p = subprocess.Popen(smt.split(), stdin=PIPE, stdout=PIPE, stderr=PIPE)
        stdout, stderr = p.communicate(s.to_smt2())
        stdout = stdout.strip()
        if stdout == 'unsat':
            r = unsat
        else:
            r = unknown
    else:
        r = s.check()
    if r == unsat:
        print("no solution")
    elif r == unknown:
        print("failed to solve")
        try:
            print(s.model())
        except Z3Exception:
            return
    else:
        return s.model()

def pack_list(*p):
    return Concat(*p)


def unpack_list(p, size):
    out = []
    k = p.size() / size 
    while k > 0:
        out.append(Extract(size * k - 1, size * (k - 1), p))
        k -= 1

    return tuple(out)
