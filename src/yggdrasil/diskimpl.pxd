from libc.stdint cimport uint64_t
from libc.string cimport memcpy

# Extract returns the same size type..
cpdef inline uint64_t Extract(int hi, int lo, uint64_t val):
    return val >> lo & ((<uint64_t>1 << (hi - lo + 1)) - <uint64_t>1)

cpdef inline uint64_t USub(uint64_t a, uint64_t b):
    return a - b

cpdef inline uint64_t Concat32(uint64_t a, uint64_t b):
    return a << 32 | b

cpdef inline bint ULE(uint64_t a, uint64_t b):
    return a <= b

cpdef inline bint ULT(uint64_t a, uint64_t b):
    return a < b

cpdef inline bint UGE(uint64_t a, uint64_t b):
    return a >= b

cpdef inline bint UGT(uint64_t a, uint64_t b):
    return a > b

cpdef inline uint64_t URem(uint64_t a, uint64_t b):
    return a % b

cpdef inline uint64_t UDiv(uint64_t a, uint64_t b):
    return a / b

cpdef inline uint64_t LShR(uint64_t a, uint64_t b):
    return a >> b

cpdef inline uint64_t Extend(uint64_t val, uint64_t size):
    assert size == 64
    return val

cpdef inline uint64_t BitVecVal(uint64_t val, uint64_t size):
    assert size <= 64
    return val

cpdef inline bint Not(bint cond):
    return not cond

cpdef inline bint Or(bint a, bint b):
    return (a or b)

cpdef bint And(bint a=*, bint b=*, bint c=*)
cpdef Block ConstBlock(uint64_t c)

cpdef inline If(bint cond, a, b):
    if cond:
        return a
    return b


cpdef void assertion(bint b, object msg=*)

cdef class Stat:
    cdef public uint64_t size
    cdef public uint64_t mtime
    cdef public uint64_t mode
    cdef public uint64_t nlink


cdef class Block:
    cdef uint64_t *buf
    cdef readonly int size # buf size

    cpdef Block copy(self)
    cdef void set(self, uint64_t, uint64_t) nogil
    cdef uint64_t get(self, uint64_t) nogil


cdef class AsyncDisk:
    cdef char* fn
    cdef int fd

    cpdef void write(self, uint64_t blknum, Block block, bint cond=*)
    cpdef Block read(self, uint64_t blknum)
    cpdef void flush(self)


cdef class Dict(object):
    cdef dict _map

    cpdef get(self, gkey, dresult)
    cpdef has_key(self, gkey)


cdef class PartitionAsyncDisk:
    cdef AsyncDisk adisk
    cdef uint64_t start
    cdef uint64_t end
    cdef bint debug

    cpdef void write(self, uint64_t blknum, Block block, bint cond=*)
    cpdef Block read(self, uint64_t blknum)
    cpdef void flush(self)


cdef class Allocator:
    cdef readfn
    cdef uint64_t start
    cdef uint64_t end

    cdef uint64_t _alloc(self, uint64_t i, Block block) nogil
    cpdef uint64_t alloc(self)


cdef class DentryLookup:
    cdef object _inode

    cdef int locate_dentry(self, Block block, uint64_t[15] name) nogil
    cdef int locate_empty_slot(self, Block block) nogil

    cdef tuple locate_dentry_ino(self, uint64_t ino, uint64_t[15] name)
    cdef tuple locate_empty_slot_ino(self, uint64_t ino)
