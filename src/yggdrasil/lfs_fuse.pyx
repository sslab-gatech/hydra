import sys
import time
import traceback

from lfs import create_lfs

from llfuse cimport *
from diskimpl cimport Block

from libc.stdlib cimport malloc, calloc, free, abort, realloc
from libc.string cimport memcpy, memset, strlen, strcmp
from posix.fcntl cimport S_IFREG, S_IFDIR, S_IFLNK
from posix.types cimport ino_t

from libc.errno cimport ENOENT, ENOTDIR, EISDIR, EACCES, ENOMEM, ENAMETOOLONG


inode_obj = None
MAX_NAME_LENGTH = 8

#################


cdef int mkstat(fuse_ino_t ino, struct_stat *stbuf):
    print 'mkstat ino={}'.format(ino)
    stat = inode_obj.get_attr(ino)

    if ino == 1:
        stbuf.st_mode = 0755 | S_IFDIR
    else:
        stbuf.st_mode = stat.mode

    stbuf.st_ino = ino
    stbuf.st_nlink = 1
    stbuf.st_size = 0
    stbuf.st_mtime = stat.mtime

    return 0


cdef void ll_getattr(fuse_req_t req, fuse_ino_t ino, fuse_file_info *fi):
    print "getattr ino={}".format(ino)

    cdef struct_stat stbuf
    memset(&stbuf, 0, sizeof(stbuf))
    if mkstat(ino, &stbuf) == -1:
        fuse_reply_err(req, ENOENT)
    else:
        fuse_reply_attr(req, &stbuf, 1.0)


cdef void ll_lookup(fuse_req_t req, fuse_ino_t parent, const char *name):
    print "lookup parent={}, name={}".format(parent, name)

    cdef size_t namelen = strlen(name)
    
    if namelen > MAX_NAME_LENGTH:
        fuse_reply_err(req, ENOENT)
        return

    cdef fuse_entry_param e
    memset(&e, 0, sizeof(e))

    cdef uint64_t pname = 0
    memcpy(&pname, name, namelen)

    t = inode_obj.lookup(parent, pname)

    if t < 0:
        fuse_reply_err(req, ENOENT)
        return
    e.ino = t
    e.attr_timeout = 1.0
    e.entry_timeout = 1.0
    mkstat(e.ino, &e.attr)
    fuse_reply_entry(req, &e)


cdef struct dirbuf:
    char *p;
    size_t size


cdef void dirbuf_add(fuse_req_t req, dirbuf *b, const char *name, fuse_ino_t ino):
    cdef struct_stat stbuf
    cdef size_t oldsize = b.size
    b.size += fuse_add_direntry(req, NULL, 0, name, NULL, 0)
    cdef char *newp = <char*>realloc(<void*>b.p, b.size)
    if newp == NULL:
        print "memory error, could not realloc buffer"
        abort()
        
    b.p = newp
    memset(&stbuf, 0, sizeof(stbuf))
    stbuf.st_ino = ino
    fuse_add_direntry(req, b.p + oldsize, b.size - oldsize, name, &stbuf, b.size)


cdef int reply_buf_limited(fuse_req_t req, const char *buf, size_t bufsize, off_t off, size_t maxsize):
    if off < bufsize:
        return fuse_reply_buf(req, buf + off, min(bufsize - off, maxsize))
    else:
        fuse_reply_buf(req, NULL, 0)


cdef void ll_readdir(fuse_req_t req, fuse_ino_t ino,
        size_t size, off_t off, fuse_file_info* fi):
    print "readdir ino={} size={} off={}".format(ino, size, off)

    cdef dirbuf b
    cdef Block block
    cdef uint64_t i
    cdef uint64_t name = 0

    if ino != 1:
        fuse_reply_err(req, ENOTDIR)
    else:
        memset(&b, 0, sizeof(b))
        dirbuf_add(req, &b, ".", ino)
        dirbuf_add(req, &b, "..", ino)

        block = inode_obj.read(ino, 0)

        for i in range(32):
            fileino = block[4 + i * 2 + 1]
            if fileino != 0:
                name = block[4 + i * 2]
                dirbuf_add(req, &b, <char*>&name, fileino)

                if b.size > size + off:
                    break

        reply_buf_limited(req, b.p, b.size, off, size)
        free(b.p)

cdef mknod(fuse_ino_t parent, const char *name, mode_t mode, dev_t rdev):
    cdef size_t namelen = strlen(name)

    if namelen > MAX_NAME_LENGTH:
        return -ENAMETOOLONG

    cdef uint64_t pname = 0
    memcpy(&pname, name, namelen)

    return inode_obj.mknod(parent, pname, mode, int(time.time()))


cdef void ll_mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
        mode_t mode, dev_t rdev):
    print 'mknod parent={}, name={}, mode={}, rdev={}'.format(parent, name, mode, rdev)

    ino = mknod(parent, name, mode, rdev)

    if ino < 0:
        fuse_reply_err(req, -1 * ino)
        return

    cdef fuse_entry_param e
    memset(&e, 0, sizeof(e))
    e.ino = ino
    e.attr_timeout = 1.0
    e.entry_timeout = 1.0
    mkstat(e.ino, &e.attr)
    fuse_reply_entry(req, &e)



def main():
    global inode_obj

    if '--' in sys.argv:
        fargs = sys.argv[sys.argv.index('--') + 1:]
        sys.argv = sys.argv[:sys.argv.index('--')]
    else:
        fargs = []

    inode_obj = create_lfs(fargs)

    cdef int argc = len(sys.argv)
    cdef char** argv = <char**>malloc(argc * sizeof(char**))
    if argv == NULL:
        print "Malloc: Memory error. Could not allocate"
        abort()

    for i in range(argc):
        argv[i] = sys.argv[i]

    cmain(argc, argv)


cdef int cmain(int argc, char **argv):
    cdef fuse_args args
    args.argc = argc
    args.argv = argv
    args.allocated = 0

    cdef fuse_chan *ch
    cdef char* mountpoint = NULL
    cdef int err = -1

    cdef fuse_session *se

    cdef fuse_lowlevel_ops ops
    memset(<void*>&ops, 0, sizeof(ops))

    ops.getattr = &ll_getattr
    ops.lookup = &ll_lookup
    ops.readdir = &ll_readdir
    ops.mknod = &ll_mknod

    if fuse_parse_cmdline(&args, &mountpoint, NULL, NULL) != -1:
        ch = fuse_mount(mountpoint, &args)
        if ch != NULL:
            se = fuse_lowlevel_new(&args, &ops, sizeof(ops), NULL)

            if se != NULL:
                if fuse_set_signal_handlers(se) != -1:
                    fuse_session_add_chan(se, ch)
                    err = fuse_session_loop(se)
                    fuse_remove_signal_handlers(se)
                    fuse_session_remove_chan(ch)
                fuse_session_destroy(se)
            fuse_unmount(mountpoint, ch)

    fuse_opt_free_args(&args)

    return err
