from diskimpl cimport *
from libc.stdint cimport uint64_t
from diskimpl cimport Block

cdef class InodePackDisk:
    cdef object _disk

    cpdef Block read(self, uint64_t ino)
    cpdef void set_iattr(self, uint64_t ino, Stat attr, Block block=*)
    cpdef Stat get_iattr(self, uint64_t ino, Block block=*)
    cpdef set_mapping(self, uint64_t ino, uint64_t off, uint64_t ptr, Block block=*)
    cpdef get_mapping(self, uint64_t ino, uint64_t off, Block block=*)
