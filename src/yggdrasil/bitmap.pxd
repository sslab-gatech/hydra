from diskimpl cimport *
from libc.stdint cimport uint64_t

cdef class BitmapDisk:
    cdef object _disk

    cpdef bint is_set(self, uint64_t bit)
    cpdef void set_bit(self, uint64_t bit)
    cpdef void unset_bit(self, uint64_t bit)
