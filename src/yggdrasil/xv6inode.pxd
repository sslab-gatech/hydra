from diskimpl cimport *
from bitmap cimport BitmapDisk
from waldisk cimport WALDisk
from inodepack cimport InodePackDisk
from dirinode cimport DirImpl

cdef class InodeDisk:
    cdef public uint64_t _INODEDATADISK

    cdef readonly uint64_t _NDIRECT

    cdef WALDisk _txndisk
    cdef object _Bitmap
    cdef object _Allocator
    cdef object _Inode
    cdef Allocator _allocator
    cdef readonly BitmapDisk _bitmap
    cdef InodePackDisk _inode

    cdef void begin_tx(self)
    cdef void commit_tx(self)

    cdef Stat get_iattr(self, uint64_t ino)
    cdef void set_iattr(self, uint64_t ino, Stat attr)

    cdef Block read(self, uint64_t lbn)
    cdef void write_tx(self, uint64_t lbn, Block data)

    cdef uint64_t mappingi(self, uint64_t vbn)
    cdef bint is_mapped(self, uint64_t vbn)
    cdef bint is_free(self, uint64_t vbn)
    cdef uint64_t alloc(self)
    cdef void free(self, uint64_t lbn)
    cdef uint64_t bmap(self, uint64_t vbn)
    cdef void bunmap(self, uint64_t vbn)

cdef class IndirectInodeDisk:
    cdef readonly uint64_t _NINDIRECT

    cdef readonly InodeDisk _idisk

    cdef void begin_tx(self)
    cdef void commit_tx(self)

    cdef Stat get_iattr(self, uint64_t ino)
    cdef void set_iattr(self, uint64_t ino, Stat attr)

    cdef Block read(self, uint64_t lbn)
    cdef void write_tx(self, uint64_t lbn, Block data)

    cdef uint64_t mappingi(self, uint64_t vbn)
    cdef bint is_mapped(self, uint64_t vbn)
    cdef bint is_free(self, uint64_t vbn)
    cdef uint64_t bmap(self, uint64_t vbn)
    cdef void bunmap(self, uint64_t vbn)
