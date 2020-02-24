from posix.stat cimport struct_stat
from posix.fcntl cimport flock
from posix.types cimport off_t, mode_t, dev_t
from libc.stdint cimport uint64_t


cdef extern from "sys/types.h":
    ctypedef unsigned long fsblkcnt_t
    ctypedef unsigned long fsfilcnt_t


cdef extern from "sys/statvfs.h":
    cdef struct statvfs:
        unsigned long  f_bsize    # file system block size */
        unsigned long  f_frsize   # fragment size */
        fsblkcnt_t     f_blocks   # size of fs in f_frsize units */
        fsblkcnt_t     f_bfree    # # free blocks */
        fsblkcnt_t     f_bavail   # # free blocks for unprivileged users */
        fsfilcnt_t     f_files    # # inodes */
        fsfilcnt_t     f_ffree    # # free inodes */
        fsfilcnt_t     f_favail   # # free inodes for unprivileged users */
        unsigned long  f_fsid     # file system ID */
        unsigned long  f_flag     # mount flags */
        unsigned long  f_namemax  # maximum filename length */

cdef extern from "sys/uio.h":
    cdef struct iovec:
        void *iov_base
        size_t iov_len

cdef extern from "fuse_lowlevel.h":
    cdef int FUSE_SET_ATTR_MODE               # (1 << 0)
    cdef int FUSE_SET_ATTR_UID                # (1 << 1)
    cdef int FUSE_SET_ATTR_GID                # (1 << 2)
    cdef int FUSE_SET_ATTR_SIZE               # (1 << 3)
    cdef int FUSE_SET_ATTR_ATIME              # (1 << 4)
    cdef int FUSE_SET_ATTR_MTIME              # (1 << 5)

    cdef int FUSE_SET_ATTR_ATIME_NOW # hint   # (1 << 7)
    cdef int FUSE_SET_ATTR_MTIME_NOW # hint   # (1 << 8)

    cdef int FUSE_SET_ATTR_CTIME              # (1 << 10)


    ctypedef fuse_req fuse_req_t
    ctypedef uint64_t fuse_ino_t

    cdef enum fuse_buf_copy_flags:
        # Buffer contains a file descriptor
        #
        # If this flag is set, the .fd field is valid, otherwise the
        # .mem fields is valid.
        #
        FUSE_BUF_IS_FD = (1 << 1),

        #
        # Seek on the file descriptor
        #
        # If this flag is set then the .pos field is valid and is
        # used to seek to the given offset before performing
        # operation on file descriptor.
        #
        FUSE_BUF_FD_SEEK = (1 << 2),

        # 
        # Retry operation on file descriptor
        #
        # If this flag is set then retry operation on file descriptor
        # until .size bytes have been copied or an error or EOF is
        # detected.
        # 
        FUSE_BUF_FD_RETRY = (1 << 3),


    cdef struct fuse_conn_info:
        pass

    # Directory entry parameters supplied to fuse_reply_entry()
    cdef struct fuse_entry_param:
        #  Unique inode number
        #
        # In lookup, zero means negative entry (from version 2.5)
        # Returning ENOENT also means negative entry, but by setting zero
        # ino the kernel may cache negative entries for entry_timeout
        # seconds.
        # 
        fuse_ino_t ino;

        #  Generation number for this entry.
        #
        # If the file system will be exported over NFS, the
        # ino/generation pairs need to be unique over the file
        # system's lifetime (rather than just the mount time). So if
        # the file system reuses an inode after it has been deleted,
        # it must assign a new, previously unused generation number
        # to the inode at the same time.
        #
        # The generation must be non-zero, otherwise FUSE will treat
        # it as an error.
        #
        # 
        uint64_t generation;

        #  Inode attributes.
        # 
        # Even if attr_timeout == 0, attr must be correct. For example,
        # for open(), FUSE uses attr.st_size from lookup() to determine
        # how many bytes to request. If this value is not correct,
        # incorrect data will be returned.
        #
        struct_stat attr;

        # Validity timeout (in seconds) for the attributes */
        double attr_timeout;

        # Validity timeout (in seconds) for the name */
        double entry_timeout;


    cdef struct fuse_session:
        pass

    cdef struct fuse_chan:
        pass

    cdef struct fuse_args:
        int argc
        char **argv
        int allocated

    cdef struct fuse_ino:
        pass

    cdef struct fuse_req:
        pass

    cdef struct fuse_forget_data:
        pass

    cdef struct fuse_bufvec:
        pass

    cdef struct fuse_pollhandle:
        pass

    cdef struct fuse_file_info:
       int flags
       unsigned int writepage     # : 1
       unsigned int direct_io     # : 1
       unsigned int keep_cache    # : 1
       unsigned int flush         # : 1
       unsigned int nonseekable   # : 1
       unsigned int flock_release # : 1
       unsigned int padding       # : 27

       uint64_t fh
       uint64_t lock_owner
       uint64_t poll_events

    cdef struct fuse_lowlevel_ops:
        #   
        #   Initialize filesystem
        #  
        #   Called before any other filesystem method
        #  
        #   There's no reply to this function
        #  
        #   @param userdata the user data passed to fuse_lowlevel_new()
        #   
        void (*init) (void *userdata, fuse_conn_info *conn)

        #   
        #   Clean up filesystem
        #  
        #   Called on filesystem exit
        #  
        #   There's no reply to this function
        #  
        #   @param userdata the user data passed to fuse_lowlevel_new()
        #   
        void (*destroy) (void *userdata)

        #  
        #   Look up a directory entry by name and get its attributes.
        #  
        #   Valid replies:
        #     fuse_reply_entry
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param parent inode number of the parent directory
        #   @param name the name to look up
        #   
        void (*lookup) (fuse_req_t req, fuse_ino_t parent, const char *name)

        #  
        #   Forget about an inode
        #  
        #   This function is called when the kernel removes an inode
        #   from its internal caches.
        #  
        #   The inode's lookup count increases by one for every call to
        #   fuse_reply_entry and fuse_reply_create. The nlookup parameter
        #   indicates by how much the lookup count should be decreased.
        #  
        #   Inodes with a non-zero lookup count may receive request from
        #   the kernel even after calls to unlink, rmdir or (when
        #   overwriting an existing file) rename. Filesystems must handle
        #   such requests properly and it is recommended to defer removal
        #   of the inode until the lookup count reaches zero. Calls to
        #   unlink, remdir or rename will be followed closely by forget
        #   unless the file or directory is open, in which case the
        #   kernel issues forget only after the release or releasedir
        #   calls.
        #  
        #   Note that if a file system will be exported over NFS the
        #   inodes lifetime must extend even beyond forget. See the
        #   generation field in struct fuse_entry_param above.
        #  
        #   On unmount the lookup count for all inodes implicitly drops
        #   to zero. It is not guaranteed that the file system will
        #   receive corresponding forget messages for the affected
        #   inodes.
        #  
        #   Valid replies:
        #     fuse_reply_none
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param nlookup the number of lookups to forget
        #   
        void (*forget) (fuse_req_t req, fuse_ino_t ino, uint64_t nlookup)

        #  
        #   Get file attributes
        #  
        #   Valid replies:
        #     fuse_reply_attr
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi for future use, currently always NULL
        #   
        void (*getattr) (fuse_req_t req, fuse_ino_t ino,
                         fuse_file_info *fi)

        #  
        #   Set file attributes
        #  
        #   In the 'attr' argument only members indicated by the 'to_set'
        #   bitmask contain valid values.  Other members contain undefined
        #   values.
        #  
        #   If the setattr was invoked from the ftruncate() system call
        #   under Linux kernel versions 2.6.15 or later, the fi->fh will
        #   contain the value set by the open method or will be undefined
        #   if the open method didn't set any value.  Otherwise (not
        #   ftruncate call, or kernel version earlier than 2.6.15) the fi
        #   parameter will be NULL.
        #  
        #   Valid replies:
        #     fuse_reply_attr
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param attr the attributes
        #   @param to_set bit mask of attributes which should be set
        #   @param fi file information, or NULL
        #  
        #   Changed in version 2.5:
        #       file information filled in for ftruncate
        #   
        void (*setattr) (fuse_req_t req, fuse_ino_t ino, struct_stat *attr,
                         int to_set, fuse_file_info *fi)

        #  
        #   Read symbolic link
        #  
        #   Valid replies:
        #     fuse_reply_readlink
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   
        void (*readlink) (fuse_req_t req, fuse_ino_t ino)

        #  
        #   Create file node
        #  
        #   Create a regular file, character device, block device, fifo or
        #   socket node.
        #  
        #   Valid replies:
        #     fuse_reply_entry
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param parent inode number of the parent directory
        #   @param name to create
        #   @param mode file type and mode with which to create the new file
        #   @param rdev the device number (only valid if created file is a device)
        #   
        void (*mknod) (fuse_req_t req, fuse_ino_t parent, const char *name,
                       mode_t mode, dev_t rdev)

        #  
        #   Create a directory
        #  
        #   Valid replies:
        #     fuse_reply_entry
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param parent inode number of the parent directory
        #   @param name to create
        #   @param mode with which to create the new file
        #   
        void (*mkdir) (fuse_req_t req, fuse_ino_t parent, const char *name,
                       mode_t mode)

        #  
        #   Remove a file
        #  
        #   If the file's inode's lookup count is non-zero, the file
        #   system is expected to postpone any removal of the inode
        #   until the lookup count reaches zero (see description of the
        #   forget function).
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param parent inode number of the parent directory
        #   @param name to remove
        #   
        void (*unlink) (fuse_req_t req, fuse_ino_t parent, const char *name)

        #  
        #   Remove a directory
        #  
        #   If the directory's inode's lookup count is non-zero, the
        #   file system is expected to postpone any removal of the
        #   inode until the lookup count reaches zero (see description
        #   of the forget function).
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param parent inode number of the parent directory
        #   @param name to remove
        #   
        void (*rmdir) (fuse_req_t req, fuse_ino_t parent, const char *name)

        #  
        #   Create a symbolic link
        #  
        #   Valid replies:
        #     fuse_reply_entry
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param link the contents of the symbolic link
        #   @param parent inode number of the parent directory
        #   @param name to create
        #   
        void (*symlink) (fuse_req_t req, const char *link, fuse_ino_t parent,
                         const char *name)

        #   Rename a file
        #  
        #   If the target exists it should be atomically replaced. If
        #   the target's inode's lookup count is non-zero, the file
        #   system is expected to postpone any removal of the inode
        #   until the lookup count reaches zero (see description of the
        #   forget function).
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param parent inode number of the old parent directory
        #   @param name old name
        #   @param newparent inode number of the new parent directory
        #   @param newname new name
        #   
        void (*rename) (fuse_req_t req, fuse_ino_t parent, const char *name,
                        fuse_ino_t newparent, const char *newname)

        #  
        #   Create a hard link
        #  
        #   Valid replies:
        #     fuse_reply_entry
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the old inode number
        #   @param newparent inode number of the new parent directory
        #   @param newname new name to create
        #   
        void (*link) (fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent,
                      const char *newname)

        #  
        #   Open a file
        #  
        #   Open flags (with the exception of O_CREAT, O_EXCL, O_NOCTTY and
        #   O_TRUNC) are available in fi->flags.
        #  
        #   Filesystem may store an arbitrary file handle (pointer, index,
        #   etc) in fi->fh, and use this in other all other file operations
        #   (read, write, flush, release, fsync).
        #  
        #   Filesystem may also implement stateless file I/O and not store
        #   anything in fi->fh.
        #  
        #   There are also some flags (direct_io, keep_cache) which the
        #   filesystem may set in fi, to change the way the file is opened.
        #   See fuse_file_info structure in <fuse_common.h> for more details.
        #  
        #   Valid replies:
        #     fuse_reply_open
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   
        void (*open) (fuse_req_t req, fuse_ino_t ino,
                      fuse_file_info *fi)

        #  
        #   Read data
        #  
        #   Read should send exactly the number of bytes requested except
        #   on EOF or error, otherwise the rest of the data will be
        #   substituted with zeroes.  An exception to this is when the file
        #   has been opened in 'direct_io' mode, in which case the return
        #   value of the read system call will reflect the return value of
        #   this operation.
        #  
        #   fi->fh will contain the value set by the open method, or will
        #   be undefined if the open method didn't set any value.
        #  
        #   Valid replies:
        #     fuse_reply_buf
        #     fuse_reply_iov
        #     fuse_reply_data
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param size number of bytes to read
        #   @param off offset to read from
        #   @param fi file information
        #   
        void (*read) (fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                      fuse_file_info *fi)

        #  
        #   Write data
        #  
        #   Write should return exactly the number of bytes requested
        #   except on error.  An exception to this is when the file has
        #   been opened in 'direct_io' mode, in which case the return value
        #   of the write system call will reflect the return value of this
        #   operation.
        #  
        #   fi->fh will contain the value set by the open method, or will
        #   be undefined if the open method didn't set any value.
        #  
        #   Valid replies:
        #     fuse_reply_write
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param buf data to write
        #   @param size number of bytes to write
        #   @param off offset to write to
        #   @param fi file information
        #   
        void (*write) (fuse_req_t req, fuse_ino_t ino, const char *buf,
                       size_t size, off_t off, fuse_file_info *fi)

        #  
        #   Flush method
        #  
        #   This is called on each close() of the opened file.
        #  
        #   Since file descriptors can be duplicated (dup, dup2, fork), for
        #   one open call there may be many flush calls.
        #  
        #   Filesystems shouldn't assume that flush will always be called
        #   after some writes, or that if will be called at all.
        #  
        #   fi->fh will contain the value set by the open method, or will
        #   be undefined if the open method didn't set any value.
        #  
        #   NOTE: the name of the method is misleading, since (unlike
        #   fsync) the filesystem is not forced to flush pending writes.
        #   One reason to flush data, is if the filesystem wants to return
        #   write errors.
        #  
        #   If the filesystem supports file locking operations (setlk,
        #   getlk) it should remove all locks belonging to 'fi->owner'.
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   
        void (*flush) (fuse_req_t req, fuse_ino_t ino,
                       fuse_file_info *fi)

        #  
        #   Release an open file
        #  
        #   Release is called when there are no more references to an open
        #   file: all file descriptors are closed and all memory mappings
        #   are unmapped.
        #  
        #   For every open call there will be exactly one release call.
        #  
        #   The filesystem may reply with an error, but error values are
        #   not returned to close() or munmap() which triggered the
        #   release.
        #  
        #   fi->fh will contain the value set by the open method, or will
        #   be undefined if the open method didn't set any value.
        #   fi->flags will contain the same flags as for open.
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   
        void (*release) (fuse_req_t req, fuse_ino_t ino,
                         fuse_file_info *fi)

        #  
        #   Synchronize file contents
        #  
        #   If the datasync parameter is non-zero, then only the user data
        #   should be flushed, not the meta data.
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param datasync flag indicating if only data should be flushed
        #   @param fi file information
        #   
        void (*fsync) (fuse_req_t req, fuse_ino_t ino, int datasync,
                       fuse_file_info *fi)

        #  
        #   Open a directory
        #  
        #   Filesystem may store an arbitrary file handle (pointer, index,
        #   etc) in fi->fh, and use this in other all other directory
        #   stream operations (readdir, releasedir, fsyncdir).
        #  
        #   Filesystem may also implement stateless directory I/O and not
        #   store anything in fi->fh, though that makes it impossible to
        #   implement standard conforming directory stream operations in
        #   case the contents of the directory can change between opendir
        #   and releasedir.
        #  
        #   Valid replies:
        #     fuse_reply_open
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   
        void (*opendir) (fuse_req_t req, fuse_ino_t ino,
                         fuse_file_info *fi)

        #  
        #   Read directory
        #  
        #   Send a buffer filled using fuse_add_direntry(), with size not
        #   exceeding the requested size.  Send an empty buffer on end of
        #   stream.
        #  
        #   fi->fh will contain the value set by the opendir method, or
        #   will be undefined if the opendir method didn't set any value.
        #  
        #   Returning a directory entry from readdir() does not affect
        #   its lookup count.
        #  
        #   Valid replies:
        #     fuse_reply_buf
        #     fuse_reply_data
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param size maximum number of bytes to send
        #   @param off offset to continue reading the directory stream
        #   @param fi file information
        #   
        void (*readdir) (fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                         fuse_file_info *fi)

        #  
        #   Release an open directory
        #  
        #   For every opendir call there will be exactly one releasedir
        #   call.
        #  
        #   fi->fh will contain the value set by the opendir method, or
        #   will be undefined if the opendir method didn't set any value.
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   
        void (*releasedir) (fuse_req_t req, fuse_ino_t ino,
                            fuse_file_info *fi)

        #  
        #   Synchronize directory contents
        #  
        #   If the datasync parameter is non-zero, then only the directory
        #   contents should be flushed, not the meta data.
        #  
        #   fi->fh will contain the value set by the opendir method, or
        #   will be undefined if the opendir method didn't set any value.
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param datasync flag indicating if only data should be flushed
        #   @param fi file information
        #   
        void (*fsyncdir) (fuse_req_t req, fuse_ino_t ino, int datasync,
                          fuse_file_info *fi)

        #  
        #   Get file system statistics
        #  
        #   Valid replies:
        #     fuse_reply_statfs
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number, zero means "undefined"
        #   
        void (*statfs) (fuse_req_t req, fuse_ino_t ino)

        #  
        #   Set an extended attribute
        #  
        #   Valid replies:
        #     fuse_reply_err
        #   
        void (*setxattr) (fuse_req_t req, fuse_ino_t ino, const char *name,
                          const char *value, size_t size, int flags)

        #  
        #   Get an extended attribute
        #  
        #   If size is zero, the size of the value should be sent with
        #   fuse_reply_xattr.
        #  
        #   If the size is non-zero, and the value fits in the buffer, the
        #   value should be sent with fuse_reply_buf.
        #  
        #   If the size is too small for the value, the ERANGE error should
        #   be sent.
        #  
        #   Valid replies:
        #     fuse_reply_buf
        #     fuse_reply_data
        #     fuse_reply_xattr
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param name of the extended attribute
        #   @param size maximum size of the value to send
        #   
        void (*getxattr) (fuse_req_t req, fuse_ino_t ino, const char *name, size_t size)

        #  
        #   List extended attribute names
        #  
        #   If size is zero, the total size of the attribute list should be
        #   sent with fuse_reply_xattr.
        #  
        #   If the size is non-zero, and the null character separated
        #   attribute list fits in the buffer, the list should be sent with
        #   fuse_reply_buf.
        #  
        #   If the size is too small for the list, the ERANGE error should
        #   be sent.
        #  
        #   Valid replies:
        #     fuse_reply_buf
        #     fuse_reply_data
        #     fuse_reply_xattr
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param size maximum size of the list to send
        #   
        void (*listxattr) (fuse_req_t req, fuse_ino_t ino, size_t size)

        #  
        #   Remove an extended attribute
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param name of the extended attribute
        #   
        void (*removexattr) (fuse_req_t req, fuse_ino_t ino, const char *name)

        #  
        #   Check file access permissions
        #  
        #   This will be called for the access() system call.  If the
        #   'default_permissions' mount option is given, this method is not
        #   called.
        #  
        #   This method is not called under Linux kernel versions 2.4.x
        #  
        #   Introduced in version 2.5
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param mask requested access mode
        #   
        void (*access) (fuse_req_t req, fuse_ino_t ino, int mask)

        #  
        #   Create and open a file
        #  
        #   If the file does not exist, first create it with the specified
        #   mode, and then open it.
        #  
        #   Open flags (with the exception of O_NOCTTY) are available in
        #   fi->flags.
        #  
        #   Filesystem may store an arbitrary file handle (pointer, index,
        #   etc) in fi->fh, and use this in other all other file operations
        #   (read, write, flush, release, fsync).
        #  
        #   There are also some flags (direct_io, keep_cache) which the
        #   filesystem may set in fi, to change the way the file is opened.
        #   See fuse_file_info structure in <fuse_common.h> for more details.
        #  
        #   If this method is not implemented or under Linux kernel
        #   versions earlier than 2.6.15, the mknod() and open() methods
        #   will be called instead.
        #  
        #   Introduced in version 2.5
        #  
        #   Valid replies:
        #     fuse_reply_create
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param parent inode number of the parent directory
        #   @param name to create
        #   @param mode file type and mode with which to create the new file
        #   @param fi file information
        #   
        void (*create) (fuse_req_t req, fuse_ino_t parent, const char *name,
                        mode_t mode, fuse_file_info *fi)

        #  
        #   Test for a POSIX file lock
        #  
        #   Introduced in version 2.6
        #  
        #   Valid replies:
        #     fuse_reply_lock
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   @param lock the region/type to test
        #   
        void (*getlk) (fuse_req_t req, fuse_ino_t ino,
                       fuse_file_info *fi, flock *lock)

        #  
        #   Acquire, modify or release a POSIX file lock
        #  
        #   For POSIX threads (NPTL) there's a 1-1 relation between pid and
        #   owner, but otherwise this is not always the case.  For checking
        #   lock ownership, 'fi->owner' must be used.  The l_pid field in
        #   'struct flock' should only be used to fill in this field in
        #   getlk().
        #  
        #   Note: if the locking methods are not implemented, the kernel
        #   will still allow file locking to work locally.  Hence these are
        #   only interesting for network filesystems and similar.
        #  
        #   Introduced in version 2.6
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   @param lock the region/type to set
        #   @param sleep locking operation may sleep
        #   
        void (*setlk) (fuse_req_t req, fuse_ino_t ino,
                       fuse_file_info *fi,
                       flock *lock, int sleep)

        #  
        #   Map block index within file to block index within device
        #  
        #   Note: This makes sense only for block device backed filesystems
        #   mounted with the 'blkdev' option
        #  
        #   Introduced in version 2.6
        #  
        #   Valid replies:
        #     fuse_reply_bmap
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param blocksize unit of block index
        #   @param idx block index within file
        #   
        void (*bmap) (fuse_req_t req, fuse_ino_t ino, size_t blocksize,
                      uint64_t idx)

        #  
        #   Ioctl
        #  
        #   Note: For unrestricted ioctls (not allowed for FUSE
        #   servers), data in and out areas can be discovered by giving
        #   iovs and setting FUSE_IOCTL_RETRY in @flags.  For
        #   restricted ioctls, kernel prepares in/out data area
        #   according to the information encoded in cmd.
        #  
        #   Introduced in version 2.8
        #  
        #   Valid replies:
        #     fuse_reply_ioctl_retry
        #     fuse_reply_ioctl
        #     fuse_reply_ioctl_iov
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param cmd ioctl command
        #   @param arg ioctl argument
        #   @param fi file information
        #   @param flags for FUSE_IOCTL_* flags
        #   @param in_buf data fetched from the caller
        #   @param in_bufsz number of fetched bytes
        #   @param out_bufsz maximum size of output data
        #   
        void (*ioctl) (fuse_req_t req, fuse_ino_t ino, int cmd, void *arg,
                       fuse_file_info *fi, unsigned flags,
                       const void *in_buf, size_t in_bufsz, size_t out_bufsz)

        #  
        #   Poll for IO readiness
        #  
        #   Introduced in version 2.8
        #  
        #   Note: If ph is non-NULL, the client should notify
        #   when IO readiness events occur by calling
        #   fuse_lowelevel_notify_poll() with the specified ph.
        #  
        #   Regardless of the number of times poll with a non-NULL ph
        #   is received, single notification is enough to clear all.
        #   Notifying more times incurs overhead but doesn't harm
        #   correctness.
        #  
        #   The callee is responsible for destroying ph with
        #   fuse_pollhandle_destroy() when no longer in use.
        #  
        #   Valid replies:
        #     fuse_reply_poll
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   @param ph poll handle to be used for notification
        #   
        void (*poll) (fuse_req_t req, fuse_ino_t ino, fuse_file_info *fi,
                      fuse_pollhandle *ph)

        #  
        #   Write data made available in a buffer
        #  
        #   This is a more generic version of the ->write() method.  If
        #   FUSE_CAP_SPLICE_READ is set in fuse_conn_info.want and the
        #   kernel supports splicing from the fuse device, then the
        #   data will be made available in pipe for supporting zero
        #   copy data transfer.
        #  
        #   buf->count is guaranteed to be one (and thus buf->idx is
        #   always zero). The write_buf handler must ensure that
        #   bufv->off is correctly updated (reflecting the number of
        #   bytes read from bufv->buf[0]).
        #  
        #   Introduced in version 2.9
        #  
        #   Valid replies:
        #     fuse_reply_write
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param bufv buffer containing the data
        #   @param off offset to write to
        #   @param fi file information
        #   
        void (*write_buf) (fuse_req_t req, fuse_ino_t ino,
                           fuse_bufvec *bufv, off_t off,
                           fuse_file_info *fi)

        #  
        #   Callback function for the retrieve request
        #  
        #   Introduced in version 2.9
        #  
        #   Valid replies:
        # *        fuse_reply_none
        #  
        #   @param req request handle
        #   @param cookie user data supplied to fuse_lowlevel_notify_retrieve()
        #   @param ino the inode number supplied to fuse_lowlevel_notify_retrieve()
        #   @param offset the offset supplied to fuse_lowlevel_notify_retrieve()
        #   @param bufv the buffer containing the returned data
        #   
        void (*retrieve_reply) (fuse_req_t req, void *cookie, fuse_ino_t ino,
                                off_t offset, fuse_bufvec *bufv)

        #  
        #   Forget about multiple inodes
        #  
        #   See description of the forget function for more
        #   information.
        #  
        #   Introduced in version 2.9
        #  
        #   Valid replies:
        #     fuse_reply_none
        #  
        #   @param req request handle
        #   
        void (*forget_multi) (fuse_req_t req, size_t count,
                              fuse_forget_data *forgets)

        #  
        #   Acquire, modify or release a BSD file lock
        #  
        #   Note: if the locking methods are not implemented, the kernel
        #   will still allow file locking to work locally.  Hence these are
        #   only interesting for network filesystems and similar.
        #  
        #   Introduced in version 2.9
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param fi file information
        #   @param op the locking operation, see flock(2)
        #   
        void (*flock) (fuse_req_t req, fuse_ino_t ino,
                       fuse_file_info *fi, int op)

        #  
        #   Allocate requested space. If this function returns success then
        #   subsequent writes to the specified range shall not fail due to the lack
        #   of free space on the file system storage media.
        #  
        #   Introduced in version 2.9
        #  
        #   Valid replies:
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param offset starting point for allocated region
        #   @param length size of allocated region
        #   @param mode determines the operation to be performed on the given range,
        #               see fallocate(2)
        #   
        void (*fallocate) (fuse_req_t req, fuse_ino_t ino, int mode,
                       off_t offset, off_t length, fuse_file_info *fi)

        #  
        #   Read directory with attributes
        #  
        #   Send a buffer filled using fuse_add_direntry_plus(), with size not
        #   exceeding the requested size.  Send an empty buffer on end of
        #   stream.
        #  
        #   fi->fh will contain the value set by the opendir method, or
        #   will be undefined if the opendir method didn't set any value.
        #  
        #   In contrast to readdir() (which does not affect the lookup counts),
        #   the lookup count of every entry returned by readdirplus(), except "."
        #   and "..", is incremented by one.
        #  
        #   Introduced in version 3.0
        #  
        #   Valid replies:
        #     fuse_reply_buf
        #     fuse_reply_data
        #     fuse_reply_err
        #  
        #   @param req request handle
        #   @param ino the inode number
        #   @param size maximum number of bytes to send
        #   @param off offset to continue reading the directory stream
        #   @param fi file information
        #   
        void (*readdirplus) (fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                             fuse_file_info *fi)


    int fuse_parse_cmdline(fuse_args *args, char **mountpoint,
                           int *multithreaded, int *foreground) nogil

    fuse_chan* fuse_mount(const char *mountpoint, fuse_args *args) nogil

    fuse_session* fuse_lowlevel_new(fuse_args *args,
                                    const fuse_lowlevel_ops *ops,
                                    size_t op_size, void *userdata) nogil

    void fuse_session_add_chan(fuse_session *se, fuse_chan *ch) nogil
    int fuse_session_loop(fuse_session *se) nogil
    void fuse_session_destroy(fuse_session *se) nogil
    void fuse_unmount(const char *mountpoint, fuse_chan *ch) nogil
    void fuse_opt_free_args(fuse_args *args) nogil

    void fuse_remove_signal_handlers(fuse_session *se) nogil
    void fuse_session_remove_chan(fuse_chan *ch) nogil
    int fuse_set_signal_handlers(fuse_session *se) nogil

    int fuse_reply_err(fuse_req_t req, int err) nogil
    void fuse_reply_none(fuse_req_t req) nogil
    int fuse_reply_entry(fuse_req_t req, const fuse_entry_param *e) nogil
    int fuse_reply_create(fuse_req_t req, const fuse_entry_param *e,
                      const fuse_file_info *fi) nogil
    int fuse_reply_attr(fuse_req_t req, const struct_stat *attr,
                    double attr_timeout) nogil
    int fuse_reply_readlink(fuse_req_t req, const char *link) nogil
    int fuse_reply_open(fuse_req_t req, const fuse_file_info *fi) nogil
    int fuse_reply_write(fuse_req_t req, size_t count) nogil
    int fuse_reply_buf(fuse_req_t req, const char *buf, size_t size) nogil
    # int fuse_reply_data(fuse_req_t req, fuse_bufvec *bufv, fuse_buf_copy_flags flags) nogil
    int fuse_reply_iov(fuse_req_t req, const iovec *iov, int count) nogil
    int fuse_reply_statfs(fuse_req_t req, const statvfs *stbuf) nogil
    int fuse_reply_xattr(fuse_req_t req, size_t count) nogil
    int fuse_reply_lock(fuse_req_t req, const flock *lock) nogil
    int fuse_reply_bmap(fuse_req_t req, uint64_t idx) nogil
    
    size_t fuse_add_direntry(fuse_req_t req, char *buf, size_t bufsize,
                             const char *name, const struct_stat *stbuf,
                             off_t off) nogil

    size_t fuse_add_direntry_plus(fuse_req_t req, char *buf, size_t bufsize,
                              const char *name,
                              const fuse_entry_param *e, off_t off) nogil

    int fuse_reply_ioctl_retry(fuse_req_t req,
                           const iovec *in_iov, size_t in_count,
                           const iovec *out_iov, size_t out_count) nogil

    int fuse_reply_ioctl(fuse_req_t req, int result, const void *buf, size_t size) nogil


    int fuse_reply_ioctl_iov(fuse_req_t req, int result, const iovec *iov,
                             int count) nogil

    int fuse_reply_poll(fuse_req_t req, unsigned revents) nogil
