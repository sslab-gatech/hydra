from disk import *

from yggdrasil.util import *
from yggdrasil.ufarray import *


# key: 64 bytes (512 bits)
KeySort = BitVecSort(64 * 8)

def FreshKey(prefix):
    return Const(fresh_name('key'), KeySort)

class KVSpec(object):
    def __init__(self, mach, fn_size, fn_data):
        self._mach = mach
        self._fn_size = fn_size
        self._fn_data = fn_data

    def get(self, key):
        m = (self._mach.on, BoolVal(True))
        size = substitute(self._fn_size(key), m)
        data = lambda off, fn=self._fn_data: substitute(fn(key, off), m)
        return String(size, data)

    def put(self, key, value):
        on = self._mach.create_on([])
        self._fn_size = lambda x, size=value.size(), oldfn=self._fn_size: If(And(on, x == key), size, oldfn(x))
        self._fn_data = lambda x, off, oldfn=self._fn_data: If(And(on, x == key), value[off], oldfn(x, off))

    def crash(self, mach):
        return self.__class__(mach, self._fn_size, self._fn_data)
