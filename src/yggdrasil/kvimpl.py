from disk import *
from yggdrasil.util import *
from yggdrasil.ufarray import *


# TODO: implement the FNV-1a hash as the default scheme
# TODO: add support for transactions
class KVImpl(object):
    KEY_SIZE = 8
    OFF_VALUE_SIZE = KEY_SIZE
    OFF_VALUE_DATA = OFF_VALUE_SIZE + 1
    VALUE_SIZE_LIMIT = (BlockSize / 64) - OFF_VALUE_DATA

    def __init__(self, disk, hashfn):
        self._disk = disk
        self._hashfn = hashfn

    # TODO: linear probing for 8 locations if collision
    def get(self, key):
        addr = self._hashfn(key)
        data = self._disk.read(addr)
        # check if key matches
        eq = And([key[i] == data[i] for i in range(self.KEY_SIZE)])
        size = data[self.OFF_VALUE_SIZE]
        # check value size
        cond = And(eq, ULE(size, self.VALUE_SIZE_LIMIT))
        end = Extract(BlockOffsetSort.size() - 1, 0, self.OFF_VALUE_DATA + size)
        return If(cond, data.getrange(self.OFF_VALUE_DATA, end), EmptyString())

    # TODO: should check if the addr is usable (unmapped)
    #       this might get tricky with delete
    def put(self, key, value):
        addr = self._hashfn(key)
        data = ConstBlock(0)
        for i in range(self.KEY_SIZE):
            data[i] = key[i]
        data[self.OFF_VALUE_SIZE] = value.size()
        data.setrange(self.OFF_VALUE_DATA, value)
        self._disk.write(addr, data)

    # TODO: bumap the key; maybe need to try to move another key
    #       to the slot first
    def delete(self, key):
        pass

    def crash(self, mach):
        return self.__class__(self._disk.crash(mach))
