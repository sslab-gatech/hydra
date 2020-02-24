from diskimpl cimport *


cdef class WALDisk:
    cdef readonly uint64_t LOG_BID_HEADER_BLOCK
    cdef readonly uint64_t LOG_DEV_HEADER_BLOCK
    cdef readonly uint64_t LOG_HEADER_BLOCK
    cdef readonly uint64_t PER_BLOCK

    cdef bint _osync
    cdef PartitionAsyncDisk _logdisk
    cdef list _datadisks
    cdef list _txn
    cdef Dict _cache

    cpdef void begin_tx(self)
    cpdef void write_tx(self, uint64_t dev, uint64_t bid, Block data)
    cpdef void flush(self)
    cpdef void commit_tx(self, bint force=*)
    cpdef void writev(self, list)
    cpdef void __commit(self)
    cpdef Block read(self, uint64_t dev, uint64_t bid)
