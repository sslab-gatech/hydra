# cython: cdivision=True
from libc.stdint cimport uint64_t
from libc.stdlib cimport calloc, free
from libc.string cimport memcpy
from posix.fcntl cimport open, O_RDWR
from posix.stat cimport mode_t
from posix.unistd cimport close, pread, pwrite, fsync, _exit

import traceback


cdef int BLOCKSIZE = 4096


cpdef inline bint And(bint a=1, bint b=1, bint c=1):
    return (a and b) and c


cpdef inline Block ConstBlock(uint64_t c):
    block = Block(BLOCKSIZE)
    if c == 0:
        return block
    for i in range(block.size):
        block.buf[i] = c
    return block


cdef class Stat:
    def __init__(self,  uint64_t size, uint64_t mtime, uint64_t mode, uint64_t nlink):
        self.size = size
        self.mtime = mtime
        self.mode = mode
        self.nlink = nlink

    @property
    def bsize(self):
        return Extract(63, 32, self.size)

    @property
    def fsize(self):
        return Extract(31, 0, self.size)

    def __str__(self):
        return "size={}, mtime={}, mode={}, nlink={}".format(self.size, self.mtime, self.mode, self.nlink)


cpdef void assertion(bint b, object msg=None):
    if not b:
        traceback.print_stack()
        print "Assertion failure:", msg
        _exit(1)


# mutable block
# manages a buffer
cdef class Block:
    # size in bytes
    def __cinit__(self, int size):
        assert size % 8 == 0, "block size is not a multiple of 8"

        self.buf = <uint64_t*>calloc(size, sizeof(char));
        if self.buf == NULL:
            raise MemoryError('could not calloc')

        self.size = size / sizeof(uint64_t)

    cpdef Block copy(self):
        cdef Block other = Block(self.size * sizeof(uint64_t))
        memcpy(other.buf, self.buf, self.size * sizeof(uint64_t))
        return other

    def __dealloc__(self):
        if self.buf != NULL:
            free(self.buf)
            self.buf = NULL

    # This is a python function..
    # Remove this indirection by calling
    # block.buf[x] instead of block[x]
    def __getitem__(self, int v):
        if v >= self.size:
            # print "WARN", v, self.size
            v = v % self.size
        # assert v < self.size
        return self.buf[v]

    def __setitem__(self, int v, uint64_t k):
        assert v < self.size, "block index out of range"
        self.buf[v] = k

    cdef void set(self, uint64_t v, uint64_t k) nogil:
        self.buf[v] = k

    cdef uint64_t get(self, uint64_t v) nogil:
        return self.buf[v]

    def _print(self):
        for i in range(self.size):
            print self[i],
            if (i + 1) % 64 == 0:
                print


cdef class AsyncDisk:
    def __cinit__(self, char* fn, mode_t mode=O_RDWR):
        self.fn = fn
        self.fd = open(fn, mode, 0666)

    def __dealloc__(self):
        close(self.fd)
        self.fd = 0

    cpdef void write(self, uint64_t blknum, Block block, bint cond=1):
        if not cond:
            return
        assert block.size == BLOCKSIZE / sizeof(uint64_t), "async disk: writing a block with invalid size"
        cdef ssize_t nbytes = pwrite(self.fd, block.buf, BLOCKSIZE, blknum * BLOCKSIZE)
        assert nbytes == BLOCKSIZE, "async disk: could not write entire nbytes"

    cpdef Block read(self, uint64_t blknum):
        cdef Block block = Block(BLOCKSIZE)
        cdef char* buf = <char*>block.buf
        cdef ssize_t nbytes = pread(self.fd, buf, BLOCKSIZE, blknum * BLOCKSIZE)
        assert nbytes == BLOCKSIZE, "async disk: could not read entire blocksize"
        return block

    cpdef void flush(self):
        fsync(self.fd)


cdef class Dict(object):
    def __init__(self):
        self._map = dict()

    cpdef get(self, gkey, dresult):
        if self._map.has_key(gkey):
            return self._map[gkey]
        return dresult

    cpdef has_key(self, gkey):
        return self._map.has_key(gkey)

    def __setitem__(self, key, value):
        self._map[key] = value


cdef class PartitionAsyncDisk:
    def __cinit__(self, AsyncDisk adisk, uint64_t start, uint64_t end, bint debug):
        self.start = start
        self.end = end
        self.adisk = adisk
        self.debug = debug

    cpdef void write(self, uint64_t blknum, Block block, bint cond=1):
        if not cond:
            return
        if self.debug:
            print 'write(%d)' % (blknum + self.start,)
        assert blknum < self.end - self.start, "partition async disk: write blknum out of range"
        self.adisk.write(blknum + self.start, block, cond)

    cpdef Block read(self, uint64_t blknum):
        assert blknum < self.end - self.start, "partition async disk: read blknum out of range: %s %s" % (self.start, self.end)
        return self.adisk.read(blknum + self.start)

    cpdef void flush(self):
        self.adisk.flush()

# Unverified bitmap allocator.
# It searches in the range [startblock, endblock] for an 'unset' bit.
# and returns it's index (uint64_t)
cdef class Allocator:
    def __init__(self, readfn, startblock, endblock):
        self.readfn = readfn
        self.start = startblock
        self.end = endblock # inclusive

    cdef uint64_t _alloc(self, uint64_t i, Block block) nogil:
        cdef uint64_t j, k

        for j in range(block.size):
            for k in range(sizeof(uint64_t) * 8):
                if block.get(j) & ((<uint64_t>1) << k) == 0:
                    return i * BLOCKSIZE * 8 + j * sizeof(uint64_t) * 8 + k
        return 0

    cpdef uint64_t alloc(self):
        cdef uint64_t i
        cdef uint64_t a
        cdef Block block
        for i in range(self.start, self.end + 1):
            block = self.readfn(i)
            a = self._alloc(i, block)
            if a != 0:
                return a
        return 0


cdef class DentryLookup:
    def __init__(self, inode):
        self._inode = inode

    cdef int locate_dentry(self, Block block, uint64_t[15] name) nogil:
        cdef int i
        cdef uint64_t ino, n, v
        for i in range(0, 512, 16):
            ino = block.get(i)
            if ino == 0:
                continue
            for n in range(15):
                v = name[n]
                if block.get(i + n + 1) != v:
                    break
            else:
                return i
        return -1

    cdef int locate_empty_slot(self, Block block) nogil:
        cdef int i
        cdef uint64_t ino
        for i in range(0, 512, 16):
            ino = block.get(i)
            if ino == 0:
                return i
        return -1

    cdef tuple locate_dentry_ino(self, uint64_t ino, uint64_t[15] name):
        cdef Block block
        cdef uint64_t i, bid
        cdef int res

        for i in range(522):
            if i == 10:
                continue
            if not self._inode.is_mapped(Concat32(ino, i)):
                continue
            bid = self._inode.mappingi(Concat32(ino, i))
            block = self._inode.read(bid)
            res = self.locate_dentry(block, name)
            if res >= 0:
                return i, res
        return 0, -1

    cdef tuple locate_empty_slot_ino(self, uint64_t ino):
        cdef Block block
        cdef uint64_t i, bid
        cdef int res

        for i in range(522):
            if i == 10:
                continue
            bid = self._inode.bmap(Concat32(ino, i))
            block = self._inode.read(bid)
            res = self.locate_empty_slot(block)
            if res >= 0:
                return i, res
        return 0, -1
