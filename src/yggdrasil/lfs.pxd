from diskimpl cimport Block, AsyncDisk
from libc.stdint cimport uint64_t

cdef class LFS:
    cdef AsyncDisk _disk
    cdef Block _sb
    cdef Block _imap
