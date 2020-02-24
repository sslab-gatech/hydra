import sys
import time
import traceback
from xv6inode import create_fuse_inode

from llfuse cimport *
from diskimpl cimport Block, Stat, Concat32
from dirinode cimport DirImpl

from libc.stdint cimport uint64_t, int64_t
from libc.stdlib cimport malloc, calloc, free, abort, realloc
from libc.string cimport memcpy, memset, strlen, strcmp
from posix.fcntl cimport S_IFREG, S_IFDIR, S_IFLNK
from posix.types cimport ino_t

from libc.errno cimport ENOENT, ENOTDIR, EISDIR, EACCES, ENOMEM, ENAMETOOLONG, EFBIG


cdef DirImpl inode_obj = None


cdef uint64_t NUM_BLOCKS = 522
cdef uint64_t MAX_NAME_LENGTH = 120


cdef void update_block(Block b, size_t off, const char *buf, size_t size) nogil:
    cdef char *bbuf = <char*>b.buf

    off = off % 4096

    cdef size_t i = 0
    for i in range(size):
        bbuf[off + i] = buf[i]


#################

cdef void gc():
    for oidx in range(inode_obj._orphans.size()):
        ino = inode_obj._orphans.index(oidx)
        for i in range(inode_obj.get_iattr(ino).bsize + 1, -1, -1):
            # print 'gc-ing orphan', i, oidx, ino, inode_obj.get_iattr(ino).bsize
            inode_obj.gc1(oidx, i)
        inode_obj.gc2(oidx)
    inode_obj.gc3()


cdef int mkstat(fuse_ino_t ino, struct_stat *stbuf):
    cdef Stat attr = inode_obj.get_iattr(ino)
    # print 'mkstat ino={}, attr={}'.format(ino, attr)

    if attr.mode == 0:
        return -1

    stbuf.st_ino = ino
    if attr.mode & S_IFDIR != 0:
        stbuf.st_nlink = 2
    else:
        stbuf.st_nlink = 1
    stbuf.st_mode = attr.mode
    stbuf.st_size = attr.fsize
    if stbuf.st_size >= (10 * 4096):
        stbuf.st_size -= 4096
    stbuf.st_mtime = attr.mtime

    return 0


cdef void ll_getattr(fuse_req_t req, fuse_ino_t ino, fuse_file_info *fi):
    # print "getattr ino={}".format(ino)

    cdef struct_stat stbuf
    memset(&stbuf, 0, sizeof(stbuf))
    if mkstat(ino, &stbuf) == -1:
        fuse_reply_err(req, ENOENT)
    else:
        fuse_reply_attr(req, &stbuf, 1.0)


cdef void ll_setattr(fuse_req_t req, fuse_ino_t ino, struct_stat *attr,
                     int to_set, fuse_file_info *fi):
    # print "setattr ino={}, st_mode={}, st_size={}, st_mtime={}, oldattr={}".format(ino,
    #         attr[0].st_mode, 
    #         attr[0].st_size,
    #         attr[0].st_mtime,
    #         iattr)

    cdef Stat iattr = inode_obj.get_iattr(ino)
    cdef uint64_t off, startoff, endoff, oldsize, newsize

    if to_set & FUSE_SET_ATTR_SIZE:
        oldsize = iattr.fsize
        newsize = attr[0].st_size
        if newsize >= 4096 * 10:
            newsize += 4096
        if oldsize > newsize:
            inode_obj.truncate(ino, newsize)

        iattr = inode_obj.get_iattr(ino)

    if to_set & FUSE_SET_ATTR_MTIME:
        iattr.mtime = attr[0].st_mtime

    if to_set & FUSE_SET_ATTR_MODE:
        iattr.mode = attr[0].st_mode

    inode_obj.set_iattr(ino, iattr)

    cdef struct_stat stbuf
    memset(&stbuf, 0, sizeof(stbuf))
    mkstat(ino, &stbuf)
    fuse_reply_attr(req, &stbuf, 1.0)


cdef void ll_lookup(fuse_req_t req, fuse_ino_t parent, const char *name):
    # print "lookup parent={}, name={}".format(parent, name)

    cdef size_t namelen = strlen(name)
    
    if namelen > MAX_NAME_LENGTH:
        fuse_reply_err(req, ENOENT)
        return

    cdef fuse_entry_param e
    memset(&e, 0, sizeof(e))

    cdef uint64_t[15] pname
    memset(&pname, 0, sizeof(uint64_t) * 15)
    memcpy(&pname, name, namelen)

    t = inode_obj.lookup(parent, pname)

    if t is None:
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
    # print "readdir ino={} size={} off={}".format(ino, size, off)

    cdef dirbuf b
    cdef Block block
    cdef uint64_t i, ioff, boff, fileino
    cdef uint64_t[16] name
    cdef bint mapped
    memset(&name, 0, sizeof(uint64_t) * 16)

    boff = 0

    iattr = inode_obj.get_iattr(ino)

    if ino != 1 and iattr.mode & S_IFDIR == 0:
        fuse_reply_err(req, ENOTDIR)
    else:
        memset(&b, 0, sizeof(b))
        dirbuf_add(req, &b, ".", ino)
        dirbuf_add(req, &b, "..", ino)

        for ioff in range(522):
            if ioff == 10:
                continue
            is_mapped = inode_obj._inode.is_mapped(Concat32(ino, ioff))
            if not is_mapped:
                break
            block = inode_obj.read(ino, ioff)

            for i in range(0, 512, 16):
                fileino = block.get(i)
                if fileino != 0:
                    memcpy(&name, (<char*>block.buf) + (i + 1) * 8, MAX_NAME_LENGTH)

                    dirbuf_add(req, &b, <char*>&name, fileino)

                    if b.size > size + off:
                        break

        reply_buf_limited(req, b.p, b.size, off, size)
        free(b.p)


cdef void ll_open(fuse_req_t req, fuse_ino_t ino, fuse_file_info *fi):
    iattr = inode_obj.get_iattr(ino)
    # print "open ino={} attr={}".format(ino, iattr)

    if iattr.mode & S_IFDIR != 0:
        fuse_reply_err(req, EISDIR)
    # TODO: Check permission?
    # else ...:
    #     fuse_reply_err(req, EACCES)
    else:
        fuse_reply_open(req, fi)


cdef void ll_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, fuse_file_info *fi):
    print 'read ino={}, size={}, off={}'.format(ino, size, off)

    cdef Stat iattr = inode_obj.get_iattr(ino)
    cdef uint64_t bufsize, fsize = iattr.fsize
    if fsize / 4096 == off / 4096:
        bufsize = fsize % 4096
    else:
        bufsize = 4096

    bufsize = min(bufsize, size)

    cdef uint64_t boff = off / 4096
    if boff >= 10:
        boff += 1

    cdef Block block = inode_obj.read(ino, boff)
    reply_buf_limited(req, <char*>block.buf, bufsize, 0, bufsize)


cdef (uint64_t, uint64_t) mknod(fuse_ino_t parent, const char *name, mode_t mode, dev_t rdev):
    cdef size_t namelen = strlen(name)

    if namelen > MAX_NAME_LENGTH:
        return (0, ENAMETOOLONG)

    cdef uint64_t[15] pname
    memset(&pname, 0, sizeof(uint64_t) * 15)
    memcpy(&pname, name, namelen)

    return inode_obj.mknod(parent, pname, mode, int(time.time()))


cdef void ll_mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
        mode_t mode, dev_t rdev):
    # print 'mknod parent={}, name={}, mode={}, rdev={}'.format(parent, name, mode, rdev)

    v = mknod(parent, name, mode, rdev)
    cdef int64_t ino, err
    ino = v[0]
    err = v[1]

    if err != 0:
        fuse_reply_err(req, err)
        return

    cdef fuse_entry_param e
    memset(&e, 0, sizeof(e))
    e.ino = ino
    e.attr_timeout = 1.0
    e.entry_timeout = 1.0
    mkstat(e.ino, &e.attr)
    fuse_reply_entry(req, &e)


cdef void ll_rename(fuse_req_t req, fuse_ino_t oldparent, const char *oldname,
                    fuse_ino_t newparent, const char *newname):
    # print 'rename oldparent={}, oldname={}, newparent={}, newname={}'.format(
    #         oldparent, oldname, newparent, newname)

    cdef size_t oldnamelen = strlen(oldname)
    cdef size_t newnamelen = strlen(newname)

    if oldnamelen > MAX_NAME_LENGTH:
        fuse_reply_err(req, ENOENT)
        return

    if newnamelen > MAX_NAME_LENGTH:
        fuse_reply_err(req, ENAMETOOLONG)
        return

    cdef uint64_t[15] poldname
    cdef uint64_t[15] pnewname

    memset(&poldname, 0, sizeof(uint64_t) * 15)
    memcpy(&poldname, oldname, oldnamelen)

    memset(&pnewname, 0, sizeof(uint64_t) * 15)
    memcpy(&pnewname, newname, newnamelen)

    ret = inode_obj.rename(oldparent, poldname, newparent, pnewname)
    fuse_reply_err(req, ret)


cdef void ll_write(fuse_req_t req, fuse_ino_t ino, const char *buf,
        size_t size, off_t off, fuse_file_info *fi):
    # print "write off={}, size={}".format(off, size)
    # print (off % 4096) + size

    cdef uint64_t boff = off / 4096;
    if boff >= 10:
        boff += 1

    cdef Block b = inode_obj.read(ino, boff)
    update_block(b, off, buf, size)

    if boff >= NUM_BLOCKS:
        fuse_reply_err(req, EFBIG)
        return

    cdef size_t written = inode_obj.write(ino, boff, b,  off % 4096 + size)

    if written == 0:
        fuse_reply_write(req, 0)
        return
    else:
        fuse_reply_write(req, size)
        return


cdef void ll_mkdir(fuse_req_t req, fuse_ino_t parent,
        const char *name, mode_t mode):
    # print 'mkdir parent={}, name={}, mode={}'.format(parent, name, parent)
    ll_mknod(req, parent, name, S_IFDIR | mode, 0)


cdef void ll_create(fuse_req_t req, fuse_ino_t parent,
        const char *name, mode_t mode, fuse_file_info *fi):
    # print 'create parent={}, name={}, mode={}'.format(parent, name, parent)

    cdef (uint64_t, uint64_t) res = mknod(parent, name, mode, 0)

    if res[1] != 0:
        fuse_reply_err(req, res[1])
        return

    cdef fuse_entry_param e
    memset(&e, 0, sizeof(e))
    e.ino = res[0]
    e.attr_timeout = 1.0
    e.entry_timeout = 1.0
    mkstat(e.ino, &e.attr)
    fuse_reply_create(req, &e, fi)


cdef void ll_unlink(fuse_req_t req, fuse_ino_t parent, const char *name):
    # print 'unlink parent={}, name={}'.format(parent, name)

    cdef size_t namelen = strlen(name)

    if namelen > MAX_NAME_LENGTH:
        fuse_reply_err(req, ENOENT)
        return

    cdef uint64_t[15] pname

    memset(&pname, 0, sizeof(uint64_t) * 15)
    memcpy(&pname, name, namelen)

    ret = inode_obj.unlink(parent, pname)

    fuse_reply_err(req, 0)


cdef void ll_rmdir(fuse_req_t req, fuse_ino_t parent, const char *name):
    # print 'rmdir parent={}, name={}'.format(parent, name)

    cdef size_t namelen = strlen(name)

    if namelen > MAX_NAME_LENGTH:
        fuse_reply_err(req, ENOENT)
        return

    cdef uint64_t[15] pname

    memset(&pname, 0, sizeof(uint64_t) * 15)
    memcpy(&pname, name, namelen)

    cdef (uint64_t, uint64_t) res = inode_obj.rmdir(parent, pname)
    fuse_reply_err(req, res[1])

cdef void ll_forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup):
    print 'forget ino={}, nlookup={}'.format(ino, nlookup)

    inode_obj.forget(ino)
    gc()
    fuse_reply_none(req)


cdef void ll_symlink(fuse_req_t req, const char *link, fuse_ino_t parent,
        const char *name):
    # print 'symlink link={}, parent={}, name={}'.format(link, parent, name)

    v = mknod(parent, name, 0777 | S_IFLNK, 0)

    cdef uint64_t ino = v[0]
    cdef uint64_t err = v[1]

    if err != 0:
        fuse_reply_err(req, err)
        return

    # inode_obj.bunmap(ino, 0)
    cdef Block b = inode_obj.read(ino, 0)
    update_block(b, 0, link, strlen(link))
    inode_obj.write(ino, 0, b, strlen(link))

    cdef fuse_entry_param e
    memset(&e, 0, sizeof(e))
    e.ino = ino
    e.attr_timeout = 1.0
    e.entry_timeout = 1.0
    mkstat(e.ino, &e.attr)
    fuse_reply_entry(req, &e)


cdef void ll_readlink(fuse_req_t req, fuse_ino_t ino):
    # print 'readlink ino={}'.format(ino)

    cdef Block b = inode_obj.read(ino, 0)
    fuse_reply_readlink(req, <char*>b.buf)

cdef void ll_fsync(fuse_req_t req, fuse_ino_t ino,
                   int datasync, fuse_file_info *fi):
    # print "fsync ino={} datasync={}".format(ino, datasync)
    inode_obj.fsync()
    fuse_reply_err(req, 0)

cdef void ll_fsyncdir(fuse_req_t req, fuse_ino_t ino,
                      int datasync, fuse_file_info *fi):
    # print "fsyncdir ino={} datasync={}".format(ino, datasync)
    inode_obj.fsync()
    fuse_reply_err(req, 0)


def main():
    global inode_obj

    farg_symbol = "-i" # use -i instead of -- for compatibility with Hydra
    if farg_symbol in sys.argv:
        fargs = sys.argv[sys.argv.index(farg_symbol) + 1:]
        sys.argv = sys.argv[:sys.argv.index(farg_symbol)]
    else:
        fargs = []

    inode_obj = create_fuse_inode(fargs)

    cdef int argc = len(sys.argv)
    cdef char** argv = <char**>malloc(argc * sizeof(char**))
    if argv == NULL:
        print "Malloc: Memory error. Could not allocate"
        abort()

    for i in range(argc):
        argv[i] = sys.argv[i]

    cmain(argc, argv)


cdef void ll_init(void *userdata, fuse_conn_info *conn):
    pass


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

    # ops.init = &ll_init
    ops.lookup = &ll_lookup
    ops.getattr = &ll_getattr
    ops.setattr = &ll_setattr
    ops.readdir = &ll_readdir
    ops.open = &ll_open
    ops.read = &ll_read
    ops.write = &ll_write
    ops.mknod = &ll_mknod
    ops.create = &ll_create
    ops.mkdir = &ll_mkdir
    ops.rename = &ll_rename
    ops.unlink = &ll_unlink
    ops.rmdir = &ll_rmdir
    ops.forget = &ll_forget

    ops.fsync = &ll_fsync
    ops.fsyncdir = &ll_fsyncdir

    ops.symlink = &ll_symlink
    ops.readlink = &ll_readlink

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
