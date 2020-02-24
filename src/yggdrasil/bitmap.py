import cython
if not cython.compiled:
    from disk import *

__all__ = ['BitmapDisk']


# Implementation of a bitmap on disk
class BitmapDisk(object):
    def __init__(self, disk):
        self._disk = disk

    # Check if a bit is set
    @cython.locals(mapbit='uint64_t')
    @cython.locals(mapfield='uint64_t')
    @cython.locals(mapbid='uint64_t')
    @cython.locals(block='Block')
    @cython.locals(field='uint64_t')
    def is_set(self, bit):
        # Define bit as Concat(.., ..)
        mapbit = Extract(6 - 1, 0, bit)
        mapfield = Extract(6 + 9 - 1, 6, bit)
        mapbid = Extract(64 - 1, 6 + 9, bit)

        block = self._disk.read(mapbid)
        field = block[mapfield]

        return Extract(0, 0, field >> Extend(mapbit, 64)) == 1

    @cython.locals(mapbit='uint64_t')
    @cython.locals(mapfield='uint64_t')
    @cython.locals(mapbid='uint64_t')
    @cython.locals(block='Block')
    @cython.locals(field='uint64_t')
    @cython.locals(new_field='uint64_t')
    def set_bit(self, bit):
        mapbit = Extract(6 - 1, 0, bit)
        mapfield = Extract(6 + 9 - 1, 6, bit)
        mapbid = Extract(64 - 1, 6 + 9, bit)

        block = self._disk.read(mapbid)

        field = block[mapfield]
        new_field = field | BitVecVal(1, 64) << Extend(mapbit, 64)

        block[mapfield] = new_field
        self._disk.write(mapbid, block)

    @cython.locals(mapbit='uint64_t')
    @cython.locals(mapfield='uint64_t')
    @cython.locals(mapbid='uint64_t')
    @cython.locals(block='Block')
    @cython.locals(field='uint64_t')
    @cython.locals(new_field='uint64_t')
    def unset_bit(self, bit):
        mapbit = Extract(6 - 1, 0, bit)
        mapfield = Extract(6 + 9 - 1, 6, bit)
        mapbid = Extract(64 - 1, 6 + 9, bit)

        block = self._disk.read(mapbid)

        field = block[mapfield]
        new_field = field & (~(BitVecVal(1, 64) << Extend(mapbit, 64)))

        block[mapfield] = new_field
        self._disk.write(mapbid, block)

    def crash(self, mach):
        return self.__class__(self._disk.crash(mach))
