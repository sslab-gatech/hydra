from libc.stdint cimport uint64_t

from diskimpl cimport *
from waldisk cimport WALDisk
from xv6inode cimport IndirectInodeDisk


cdef class DirImpl:
    cdef WALDisk _txndisk
    cdef IndirectInodeDisk _inode

    cdef object _Allocator
    cdef object _iallocator
    cdef object _ifree

    cdef object _Bitmap
    cdef object _ibitmap

    cdef object _DirLookup
    cdef DentryLookup _dirlook

    cdef object _orphans

    cdef object locate_dentry_ino(self, uint64_t ino, uint64_t[15] name)
    cdef object locate_empty_dentry_slot_ino(self, uint64_t ino)
    cdef object locate_empty_dentry_slot_err_ino(self, uint64_t ino)

    cdef void write_dentry(self, Block block, uint64_t off, uint64_t ino, uint64_t[15] name)
    cdef void clear_dentry(self, Block block, uint64_t off)

    cdef uint64_t lookup(self, uint64_t parent, uint64_t[15] name)
    cdef uint64_t unlink(self, uint64_t parent, uint64_t[15] name)
    cdef tuple rmdir(self, uint64_t parent, uint64_t[15] name)
    cdef uint64_t rename(self, uint64_t oparent, uint64_t[15] oname, uint64_t nparent, uint64_t[15] nname)
    cdef tuple mknod(self, uint64_t parent, uint64_t[16] name, uint64_t mode, uint64_t mtime)
