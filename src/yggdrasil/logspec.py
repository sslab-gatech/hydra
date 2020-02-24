from disk import *
from yggdrasil.util import *

class LogSpecEntry(object):
    def __init__(self, data, size):
        self._data = data
        self._size = size

    def __getitem__(self, off):
        return self._data(off)

    @property
    def size(self):
        return self._size


class LogSpec(object):
    def __init__(self, mach, datafn, sizefn, nextfn, tail):
        self._mach = mach
        self._datafn = datafn
        self._sizefn = sizefn
        self._nextfn = nextfn
        self._tail = tail

    def append(self, data, size):
        on = self._mach.create_on([])
        tail = self._tail
        self._datafn = lambda x, off, oldfn=self._datafn: If(And(on, x == tail), data(off), oldfn(x, off))
        self._sizefn = lambda x, oldfn=self._sizefn: If(And(on, x == tail), size, oldfn(x))
        newtail = Const(fresh_name("tail"), SizeSort)
        self._mach.add_control(newtail)
        assertion(ULT(tail, newtail))
        self._nextfn = lambda x, oldfn=self._nextfn: If(And(on, x == tail), newtail, oldfn(x))
        self._tail = If(on, newtail, tail)

    def __getitem__(self, k):
        m = [(c, BoolVal(True)) for c in self._mach.control]
        data = lambda off, datafn=self._datafn: substitute(datafn(k, off), *m)
        size = substitute(self._sizefn(k), *m)
        return LogSpecEntry(data, size)

    @property
    def tail(self):
        m = [(c, BoolVal(True)) for c in self._mach.control]
        return substitute(self._tail, *m)

    def crash(self, mach):
        return self.__class__(mach, self._datafn, self._sizefn, self._nextfn, self._tail)
