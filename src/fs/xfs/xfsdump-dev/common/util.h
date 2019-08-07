/*
 * Copyright (c) 2000-2001 Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef UTIL_H
#define UTIL_H


/* util.[hc] - generally useful functions
 */

/* write_buf - converts the normal manager write method into something simpler
 *
 * the managers present a somewhat complicated write interface, for performance
 * reasons, which eliminates buffer copying. however, there are times when
 * the data to be written is already buffered, such as when writing out
 * header structures. This function takes care of the details of writing
 * the contents of such a buffer using the manager operators.
 *
 * if bufp is null, writes bufsz zeros.
 */
typedef char * ( * gwbfp_t )( void *contextp, size_t wantedsz, size_t *szp);
typedef int ( * wfp_t )( void *contextp, char *bufp, size_t bufsz );

extern int write_buf( char *bufp,
			   size_t bufsz,
			   void *contextp,
			   gwbfp_t get_write_buf_funcp,
			   wfp_t write_funcp );


/* read_buf - converts the normal manager read method into something simpler
 *
 * the managers present a somewhat complicated read interface, for performance
 * reasons, which eliminates buffer copying. however, there are times when
 * the caller wants his buffer to be filled  completely, such as when reading
 * header structures. This function takes care of the details of filling
 * such a buffer using the manager operators.
 *
 * if bufp is null, discards read data.
 *
 * returns number of bytes successfully read. returns by reference the
 * status of the first failure of the read funcp. if no read failures occur,
 * *statp will be zero.
 */
typedef char * ( *rfp_t )( void *contextp, size_t wantedsz, size_t *szp, int *statp );
typedef void ( * rrbfp_t )( void *contextp, char *bufp, size_t bufsz );

extern int read_buf( char *bufp,
			  size_t bufsz, 
			  void *contextp,
			  rfp_t read_funcp,
			  rrbfp_t return_read_buf_funcp,
			  int *statp );



/* strncpyterm - like strncpy, but guarantees the destination is null-terminated
 */
extern char *strncpyterm( char *s1, char *s2, size_t n );

/* bigstat - efficient file status gatherer. presents an iterative
 * callback interface, invoking the caller's callback for each in-use
 * inode. the caller can specify the first ino, and can limit the callbacks
 * to just dirs, just non-dirs, or both. if the callback returns non-zero,
 * aborts the iteration and sets stat to the callback's return; otherwise,
 * stat is set to zero. return value set to errno if the system call fails,
 * or EINTR if optional pre-emption func returns TRUE.
 */
#define BIGSTAT_ITER_DIR	( 1 << 0 )
#define BIGSTAT_ITER_NONDIR	( 1 << 1 )
#define BIGSTAT_ITER_ALL	( ~0 )

typedef int (*bstat_cbfp_t)(void *arg1,
				 jdm_fshandle_t *fshandlep,
				 int fsfd,
				 xfs_bstat_t *statp );

typedef xfs_ino_t (*bstat_seekfp_t)(void *arg1,
				    xfs_ino_t lastino);

extern int bigstat_iter( jdm_fshandle_t *fshandlep,
			      int fsfd,
			      int selector,
			      xfs_ino_t start_ino,
			      bstat_cbfp_t fp,
			      void * cb_arg1,
			      bstat_seekfp_t seekfp,
			      void * seek_arg1,
			      int *statp,
			      bool_t ( pfp )( int ), /* preemption chk func */
			      xfs_bstat_t *buf,
			      size_t buflen );

extern int bigstat_one( int fsfd,
			     xfs_ino_t ino,
			     xfs_bstat_t *statp );

extern int inogrp_iter( int fsfd,
			     int ( * fp )( void *arg1,
				     		int fsfd,
						xfs_inogrp_t *inogrp ),
			     void * arg1,
			     int *statp );

/* calls the callback for every entry in the directory specified
 * by the stat buffer. supplies the callback with a file system
 * handler and a stat buffer, and the name from the dirent.
 *
 * NOTE: does NOT invoke callback for "." or ".."!
 *
 * caller may supply getdents buffer. size must be >= sizeof( dirent_t )
 * + MAXPATHLEN. if not supplied (usrgdp NULL), one will be malloc()ed.
 *
 * if the callback returns non-zero, returns 1 with cbrval set to the
 * callback's return value. if syscall fails, returns -1 with errno set.
 * otherwise returns 0.
 */
extern int diriter( jdm_fshandle_t *fshandlep,
			 int fsfd,
			 xfs_bstat_t *statp,
			 int ( *cbfp )( void *arg1,
					     jdm_fshandle_t *fshandlep,
					     int fsfd,
					     xfs_bstat_t *statp,
					     char *namep ),
			 void *arg1,
			 int *cbrvalp,
			 char *usrgdp,
			 size_t usrgdsz );



/* macro to copy uuid structures
 */
#define COPY_LABEL( lab1, lab2) ( bcopy( lab1, lab2, GLOBAL_HDR_STRING_SZ) )

/*
 * Align pointer up to alignment
 */
#define	ALIGN_PTR(p,a)	\
	(((intptr_t)(p) & ((a)-1)) ? \
		((void *)(((intptr_t)(p) + ((a)-1)) & ~((a)-1))) : \
		((void *)(p)))

#endif /* UTIL_H */
