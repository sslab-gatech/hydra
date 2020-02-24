from z3 import *
from util import *
import copy
import warnings


# immutable string: an array of u64s (represented using UF)
StringOffsetSort = SizeSort
StringElementSort = BlockElementSort


class String(object):
    def __init__(self, size, data):
        self._size = size
        self._data = data

    def __eq__(self, other):
        off = FreshSize('off', domain=StringOffsetSort)
        size = self.size()
        return And(size == other.size(),
                   ForAll([off], Implies(ULT(off, size), self[off] == other[off])))

    def __ne__(self, other):
        return Not(self == other)

    def size(self):
        return self._size

    def __getitem__(self, off):
        return self._data(off)

    def ite(self, other, cond):
        data = lambda off: If(cond, self[off], other[off])
        return String(self._size, data)


# mutable - easy to extract to C
class Block(object):
    def __init__(self, fn):
        self._fn = fn

    def __getitem__(self, key):
        if type(key) is int:
            key = BitVecVal(key, BlockOffsetSort)

        assert key.size() == BlockOffsetSort.size()

        return self._fn(key)

    def get(self, bid):
        return self[bid]

    def set(self, bid, val):
        self[bid] = val

    def ite(self, other, cond):
        # If cond then self else other
        fn = lambda off, oldfn=self._fn: If(cond,
                oldfn(off),
                other._fn(off))
        return Block(fn)

    def __setitem__(self, key, val):
        if type(key) is int:
            key = BitVecVal(key, BlockOffsetSort)
        if type(val) is int:
            val = BitVecVal(val, BlockElementSort)

        if val.size() < BlockElementSort.size():
            warnings.warn("Block element %s (%s) smaller then expected (%s)." %
                    (val, val.size(), BlockElementSort.size()))
            val = ZeroExt(BlockElementSort.size() - val.size(), val)

        assert key.size() == BlockOffsetSort.size()
        assert val.size() == BlockElementSort.size()

        self._fn = lambda off, oldfn=self._fn: If(off == key, val, oldfn(off))

    def set(self, key, val):
        self[key] = val

    def getrange(self, start, end=None):
        if type(start) is int:
            start = BitVecVal(start, BlockOffsetSort.size())
        else:
            assert start.size() == BlockOffsetSort.size()
        if end is None:
            end = BlockSize.size() / BlockElementSort.size()
        else:
            assert end.size() == BlockOffsetSort.size()
        size = If(ULE(start, end - 1), end - start, 0)
        zdiff = StringOffsetSort.size() - BlockOffsetSort.size()
        size = ZeroExt(zdiff, size)
        start = ZeroExt(zdiff, start)
        data = lambda off, fn=self._fn: If(ULT(off, size), fn(Extract(BlockOffsetSort.size() - 1, 0, off + start)), BitVecVal(0, StringElementSort.size()))
        return String(size, data)

    def setrange(self, start, s):
        if type(start) is int:
            start = BitVecVal(start, BlockOffsetSort.size())
        else:
            assert start.size() == BlockOffsetSort.size()
        zdiff = StringOffsetSort.size() - BlockOffsetSort.size()
        size = s.size()
        self._fn = lambda off, oldfn=self._fn: If(And(ULE(start, off), ULT(ZeroExt(zdiff, off - start), size)), s[ZeroExt(zdiff, off - start)], oldfn(off))

    def __eq__(self, other):
        off = Const(fresh_name('off'), BlockOffsetSort)
        return ForAll([off], self[off] == other[off])

    def __ne__(self, other):
        return Not(self.__eq__(other))

    def _print(self, num=None):
        if num is None:
            num = self.size
        for i in range(num):
            print simplify(self[i])

    @property
    def size(self):
        return 2 ** BlockOffsetSort.size()


# immutable
class DiskArray(object):
    def __init__(self, fn, domain=SizeSort):
        self._domain = domain
        self._fn = fn

    def __call__(self, key):
        if type(key) is int:
            key = BitVecVal(key, self._domain)

        if key.size() > self._domain.size():
            warnings.warn("Disk key %s (%s) larger then expected (%s)." %
                    (key, key.size(), self._domain.size()))
            key = Extract(self._domain.size() - 1, 0, key)

        if key.size() != self._domain.size():
            print "key.size() = {} != self._domain.size() = {}".format(key.size(),
                                                                       self._domain.size())
            assert key.size() == self._domain.size()

        return Block(lambda off, fn=self._fn: fn(key, off))

    def update(self, key, val, *guard):
        if type(key) is int:
            key = BitVecVal(key, self._domain)

        if key.size() > self._domain.size():
            warnings.warn("Disk key %s (%s) larger then expected (%s)." %
                    (key, key.size(), self._domain.size()))
            key = Extract(self._domain.size() - 1, 0, key)

        if key.size() != self._domain.size():
            print "key.size() = {} != self._domain.size() = {}".format(key.size(),
                                                                       self._domain.size())
            assert key.size() == self._domain.size()
        assert isinstance(val, Block)

        # filter out None
        guard = [g for g in guard if g is not None]
        # make a copy of the block; a shallow copy is fine
        # as _fn is the only field
        val = copy.copy(val)
        fn = lambda bid, off, oldfn=self._fn: If(And(bid == key, *guard), val[off], oldfn(bid, off))
        return DiskArray(fn, domain=self._domain)

    def ite(self, other, cond):
        # If cond then self else other
        # NB: don't eval here to match our C code - the result is _mutable_
        fn = lambda bid, off: If(cond, self._fn(bid, off), other._fn(bid, off))
        return DiskArray(fn, domain=self._domain)

    def domain(self):
        return self._domain


def FreshSize(prefix, domain=SizeSort):
    return Const(fresh_name(prefix), domain)


def FreshDiskArray(prefix, domain=SizeSort):
    return DiskArray(Function(fresh_name(prefix), domain, BlockOffsetSort, BlockElementSort), domain=domain)


def ConstDiskArray(blk, domain=SizeSort):
    return DiskArray(lambda bid, off: blk[off], domain=domain)


def FreshBlock(prefix):
    return Block(Function(fresh_name(prefix), BlockOffsetSort, BlockElementSort))


def ConstBlock(v):
    if type(v) is int:
        v = BitVecVal(v, BlockElementSort.size())

    assert v.size() == BlockElementSort.size()

    return Block(lambda off: v)


def FreshString(prefix, size=None):
    if size is None:
        size = FreshSize(prefix + '.size', domain=StringOffsetSort)
    elif type(size) is int:
        size = BitVecVal(size, SizeSort.size())
    data = Function(fresh_name(prefix + '.data'), StringOffsetSort, StringElementSort)
    return String(size, data)


def ConstString(size, v):
    return String(size, lambda off: v)


def EmptyString():
    return ConstString(BitVecVal(0, StringOffsetSort.size()), 0)


def tup_eq(t1, t2):
    res = BoolVal(True)
    for a, b in zip(t1, t2):
        res = And(a == b, res)
    return res


# A wrapper around an uninterpreted function that supports easily updating it
# at a particular point.
class UFunction(object):
    def __init__(self, name, *args, **kwargs):
        self._name = name
        fn = kwargs.pop('fn', None)

        self._domain = args[:-1]
        self._range = args[-1]
        if fn:
            self._fn = fn
        else:
            self._fn = Function(name, *args)

    def __call__(self, *key):
        assert len(key) == len(self._domain)

        key = list(key)

        for n in range(len(key)):
            if isinstance(key[n], int):
                key[n] = BitVecVal(key[n], self._domain[n])

            assert key[n].size() == self._domain[n].size()

        return self._fn(*key)

    def update(self, key, val, guard=True):
        if not isinstance(key, tuple) and not isinstance(key, list):
            key = [key]

        assert len(key) == len(self._domain)

        key = list(key)

        for n in range(len(key)):
            if isinstance(key[n], int):
                key[n] = BitVecVal(key[n], self._domain[n])

            assert key[n].size() == self._domain[n].size()

        pfn = self._fn

        fn = lambda *args: If(And(guard, tup_eq(args, key)), val, pfn(*args))

        args = list(self._domain) + [self._range]

        return UFunction(self._name, *args, fn=fn)


def FreshUFunction(name, *args, **kwargs):
    return UFunction(fresh_name(name), *args, **kwargs)
