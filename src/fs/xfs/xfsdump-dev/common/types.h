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
#ifndef TYPES_H
#define TYPES_H

/*
 * Pull in all the standard C types first.
 */
#include <stdint.h>

/*
 * type macros that were naively put into xfs/jdm.h, then used in places that
 * have nothing to do with file handle operations.
 */
#define sizeofmember( t, m )	sizeof( ( ( t * )0 )->m )
#define offsetofmember( t, m )	( ( size_t )( char * )&( ( ( t * )0 )->m ) )

#define XFSDUMP_DIRPATH	inv_basepath()

/*
 * Should be, but isn't, defined in uuid/uuid.h
 */
#define UUID_STR_LEN	36

/* fundamental page size - probably should not be hardwired, but
 * for now we will
 */
#define PGSZLOG2	12
#define PGSZ		( 1 << PGSZLOG2 )
#define PGMASK		( PGSZ - 1 )

/* integers
 */
typedef uint32_t size32_t;
typedef uint64_t size64_t;
typedef char char_t;
typedef unsigned char u_char_t;
typedef unsigned int uint;
typedef long long_t;
typedef unsigned long u_long_t;
typedef size_t ix_t;
typedef int32_t time32_t;

typedef uint64_t xfs_ino_t;

#define constpp        char * const *

/* limits
 */
#ifndef NBBY
#define NBBY		8
#endif

#define	MKMAX( t, s )	( ( t )						\
			  ( ( ( 1ull					\
			        <<					\
			        ( ( unsigned long long )sizeof( t )	\
				  *					\
				  ( unsigned long long )NBBY		\
			          -					\
			          ( s + 1ull )))			\
			      -						\
			      1ull )					\
			    *						\
			    2ull					\
			    +						\
			    1ull ))
#define MKSMAX( t )	MKMAX( t, 1ull )
#define MKUMAX( t )	MKMAX( t, 0ull )
#define INT32MAX	MKSMAX( int32_t )
#define UINT32MAX	MKUMAX( uint32_t )
#define SIZE32MAX	MKUMAX( size32_t )
#define INT64MAX	MKSMAX( int64_t )
#define UINT64MAX	MKUMAX( uint64_t )
#define SIZE64MAX	MKUMAX( size64_t )
#define INO64MAX	MKUMAX( xfs_ino_t )
#define OFF64MAX	MKSMAX( off64_t )
#define INTGENMAX	MKSMAX( int )
#define UINTGENMAX	MKUMAX( uint )
#define OFFMAX		MKSMAX( off_t )
#define SIZEMAX		MKUMAX( size_t )
#define IXMAX		MKUMAX( size_t )
#define INOMAX		MKUMAX( ino_t )
#define TIMEMAX		MKSMAX( time32_t )
#define INT16MAX	0x7fff
#define UINT16MAX	0xffff

/* boolean
 */
typedef int bool_t;
#define BOOL_TRUE	1
#define BOOL_FALSE	0
#define BOOL_UNKNOWN	( -1 )
#define BOOL_ERROR	( -2 )

/* useful return code scheme
 * NOTE: that there are macros and error messages in common/types.h that should
 * be updated when new error codes are added here.
 */
typedef enum { RV_OK,		/* mission accomplished */
	       RV_NOTOK,	/* media erase request denied */
	       RV_NOMORE,	/* no more work to do */
	       RV_EOD,		/* ran out of data */
	       RV_EOF,		/* hit end of media file */
	       RV_EOM,		/* hit end of media object */
	       RV_ERROR,	/* operator error or resource exhaustion */
	       RV_DONE,		/* return early because someone else did work */
	       RV_INTR,		/* cldmgr_stop_requested( ) */
	       RV_CORRUPT,	/* stopped because corrupt data encountered */
	       RV_QUIT,		/* stop using resource */
	       RV_DRIVE,	/* drive quit working */
	       RV_TIMEOUT,	/* operation timed out */
	       RV_MEDIA,	/* no media object in drive */
	       RV_PROTECTED,	/* want to write but write-protected */
	       RV_CORE,		/* really bad - dump core! */
	       RV_OPT,		/* bad command line option */
	       RV_INIT,		/* could not initialise subsystem */
	       RV_PERM,		/* insufficient privilege */
	       RV_COMPAT,	/* dump incompatible */
	       RV_INCOMPLETE,	/* some part of the dump is incomplete */
	       RV_KBD_INTR,	/* keyboard interrupt */
	       RV_INV,		/* session inventory error */
	       RV_USAGE,	/* print command usage only */
	       RV_EXISTS,	/* file or directory already exists */
	       RV_NONE,		/* no error code available */
	       RV_UNKNOWN,	/* there was an error but we don't know which one */
	       _RV_NUM		/* number of return codes */
} rv_t;

/* typedefs I'd like to see ...
 */
typedef struct stat stat_t;
typedef struct stat64 stat64_t;
typedef struct getbmapx getbmapx_t;
typedef struct fsdmidata fsdmidata_t;

/* flg definitions for preemptchk 
 */
#define PREEMPT_FULL		0
#define PREEMPT_PROGRESSONLY	1

#endif /* TYPES_H */
