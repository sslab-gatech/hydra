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
#ifndef CONTENT_INODE_H
#define CONTENT_INODE_H

/* content_inode.h - inode-style content strategy-specific header structure
 */

/* dump startpoint: identifies dump stream boundaries
 * NOTE: sp_offset is not a position within a file; it is a quantity of
 * bytes to skip. holes do not count.
 */
struct startpt {
	xfs_ino_t sp_ino;		/* first inode to dump */
	off64_t sp_offset;	/* bytes to skip in file data fork */
	int32_t sp_flags;
	int32_t sp_pad1;
};

typedef struct startpt startpt_t;

#define STARTPT_FLAGS_END		( 1 << 0 )
		/* this startpt indicates that all extents of all files in
		 * the stream were completely dumped. the other fields
		 * are meaningless. this will appear only once per dump
		 * stream, even if the stream spans multiple media files.
		 *
		 * also used in the strategy-specific portion of the
		 * content header to qualify the ending point.
		 */
#define STARTPT_FLAGS_NULL		( 1 << 1 )
		/* used to detect if the null file header makes it onto
		 * media. only necessary after the last file in the stream,
		 * to allow the end-of-stream flag in the null header to
		 * be seen.
		 */


/* drange_t - describes a range of ino/offset values
 */
struct drange {
	startpt_t dr_begin;
	startpt_t dr_end;
};

typedef struct drange drange_t;


/* inode-style specific media file header section
 */
#define CONTENT_INODE_HDR_SZ  sizeofmember( content_hdr_t, ch_specific )

struct content_inode_hdr {
	int32_t cih_mediafiletype;			/*   4   4 */
		/* dump media file type: see #defines below */
	int32_t cih_dumpattr;				/*   4   8 */
		/* dump attributes: see #defines below */
	int32_t cih_level;				/*   4   c */
		/* dump level */
	char pad1[ 4 ];					/*   4  10 */
		/* alignment */
	time32_t cih_last_time;				/*   4  14 */
		/* if an incremental,time of previous dump at a lesser level */
	time32_t cih_resume_time;			/*   4  18 */
		/* if a resumed dump, time of interrupted dump */
	xfs_ino_t cih_rootino;				/*   8  20 */
		/* root inode number */
	uuid_t cih_last_id;				/*  10  30 */
		/* if an incremental, uuid of prev dump */
	uuid_t cih_resume_id;				/*  10  40 */
		/* if a resumed dump, uuid of interrupted dump */
	startpt_t cih_startpt;				/*  18  58 */
		/* starting point of media file contents */
	startpt_t cih_endpt;				/*  18  70 */
		/* starting point of next stream */
	uint64_t cih_inomap_hnkcnt;			/*   8  78 */

	uint64_t cih_inomap_segcnt;			/*   8  80 */

	uint64_t cih_inomap_dircnt;			/*   8  88 */

	uint64_t cih_inomap_nondircnt;			/*   8  90 */

	xfs_ino_t cih_inomap_firstino;			/*   8  98 */

	xfs_ino_t cih_inomap_lastino;			/*   8  a0 */

	uint64_t cih_inomap_datasz;			/*   8  a8 */
		/* bytes of non-metadata dumped */
	char cih_pad2[ CONTENT_INODE_HDR_SZ - 0xa8 ];	/*  18  c0 */
		/* padding */
};

typedef struct content_inode_hdr content_inode_hdr_t;

/* media file types
 */
#define CIH_MEDIAFILETYPE_DATA		0
#define CIH_MEDIAFILETYPE_INVENTORY	1
#define CIH_MEDIAFILETYPE_INDEX		2

/* dump attributes
 */
#define	CIH_DUMPATTR_SUBTREE			( 1 <<  0 )
#define	CIH_DUMPATTR_INDEX			( 1 <<  1 )
#define CIH_DUMPATTR_INVENTORY			( 1 <<  2 )
#define CIH_DUMPATTR_INCREMENTAL		( 1 <<  3 )
#define CIH_DUMPATTR_RETRY			( 1 <<  4 )
#define CIH_DUMPATTR_RESUME			( 1 <<  5 )
#define CIH_DUMPATTR_INOMAP			( 1 <<  6 )
#define CIH_DUMPATTR_DIRDUMP			( 1 <<  7 )
#define CIH_DUMPATTR_FILEHDR_CHECKSUM		( 1 <<  8 )
#define CIH_DUMPATTR_EXTENTHDR_CHECKSUM		( 1 <<  9 )
#define CIH_DUMPATTR_DIRENTHDR_CHECKSUM		( 1 << 10 )
#define CIH_DUMPATTR_DIRENTHDR_GEN		( 1 << 11 )
#define CIH_DUMPATTR_EXTATTR			( 1 << 12 )
#define CIH_DUMPATTR_EXTATTRHDR_CHECKSUM	( 1 << 13 )
#define CIH_DUMPATTR_NOTSELFCONTAINED		( 1 << 14 )


/* timestruct_t - time structure
 *
 * used in bstat_t below. derived from timestruc_t, to achieve independence
 * from changes to timestruc_t.
 */
#define TIMESTRUCT_SZ	8

struct timestruct {			/*		     bytes accum */
	int32_t		tv_sec;		/* seconds		 4     4 */
	int32_t		tv_nsec;	/* and nanoseconds	 4     8 */
};

typedef struct timestruct timestruct_t;


/* bstat_t - bulk stat structure
 *
 * used in filehdr_t below. derived from xfs_bstat_t, to achieve independence
 * from changes to xfs_bstat_t. 
 */
#define BSTAT_SZ	128
#define MODE_SZ		4

struct bstat {				/*		     bytes accum */
	xfs_ino_t	bs_ino;		/* inode number		 8     8 */
	uint32_t	bs_mode;	/* type and mode	 4     c */
	uint32_t	bs_nlink;	/* number of links	 4    10 */
	int32_t		bs_uid;		/* user id		 4    14 */
	int32_t		bs_gid;		/* group id		 4    18 */
	uint32_t	bs_rdev;	/* device value		 4    1c */
	int32_t		bs_blksize;	/* block size		 4    20 */
	off64_t		bs_size;	/* file size		 8    28 */
	timestruct_t	bs_atime;	/* access time		 8    30 */
	timestruct_t	bs_mtime;	/* modify time		 8    38 */
	timestruct_t	bs_ctime;	/* inode change time	 8    40 */
	int64_t		bs_blocks;	/* number of blocks	 8    48 */
	uint32_t	bs_xflags;	/* extended flags	 4    4c */
	int32_t		bs_extsize;	/* extent size		 4    50 */
	int32_t		bs_extents;	/* number of extents	 4    54 */
	uint32_t	bs_gen;		/* generation count	 4    58 */
	uint16_t	bs_projid_lo;	/* low 16 of project id	 2    5a */
	uint16_t	bs_forkoff;	/* inode fork offset	 2    5c */
	uint16_t	bs_projid_hi;	/* hi 16 of project id	 2    5e */
	char		bs_pad[ 10 ];	/* for expansion	 e    68 */
	uint32_t	bs_dmevmask;	/* DMI event mask        4    6c */
	uint16_t	bs_dmstate;	/* DMI state info        2    6e */
	char		bs_pad1[ 18 ];	/* for expansion        12    80 */
					/* NOTE: old dumps didn't always
					 * zero first 2 bytes of bs_pad1 */
};

typedef struct bstat bstat_t;

/*
 * Project quota id helpers (previously projid was 16bit only
 * and using two 16bit values to hold new 32bit projid was choosen
 * to retain compatibility with "old" filesystems).
 */
static inline uint32_t
bstat_projid(struct bstat *bs)
{
        return (uint32_t)bs->bs_projid_hi << 16 | bs->bs_projid_lo;
}


/* extended inode flags that can only be set after all data
 * has been restored to a file.
 */
#define	POST_DATA_XFLAGS	( XFS_XFLAG_IMMUTABLE |		\
				  XFS_XFLAG_APPEND |		\
				  XFS_XFLAG_SYNC )

/* filehdr_t - header placed at the beginning of every dumped file.
 *
 * each fs file placed on dump media begins with a FILEHDR_SZ-byte header.
 * following that are one or more variable-length content extents.
 * the content extents contain the actual data associated with the fs file. 
 */
#define FILEHDR_SZ	256

struct filehdr {
	int64_t fh_offset;
	int32_t fh_flags;
	uint32_t fh_checksum;
	bstat_t fh_stat;
	char fh_pad2[ FILEHDR_SZ
		      - sizeof( int64_t )
		      - sizeof( int32_t )
		      - sizeof( uint32_t )
		      - sizeof( bstat_t ) ];
};

typedef struct filehdr filehdr_t;

#define FILEHDR_FLAGS_NULL	( 1 << 0 )
		/* identifies a dummy file header. every media file
		 * is terminated with a dummy file header, unless
		 * terminated by end of media.
		 */
#define FILEHDR_FLAGS_CHECKSUM	( 1 << 1 )
		/* indicates the header checksum is valid
		 */
#define FILEHDR_FLAGS_END	( 1 << 2 )
		/* the last media file in the stream is always terminated by
		 * a dummy file header, with this flag set.
		 */
#define FILEHDR_FLAGS_EXTATTR	( 1 << 3 )
		/* special file header followed by one file's (dir or nondir)
		 * extended attributes.
		 */


/* extenthdr_t - a header placed at the beginning of every dumped
 * content extent.
 *
 * a dumped file consists of a filehdr_t followed by zero or more
 * extents. for regular files the last extent is always of type LAST.
 * for symbolic link files, only one extent is dumped, and it contains the
 * symlink path.
 */
#define EXTENTHDR_SZ	32

struct extenthdr {
	off64_t eh_sz; /* length of extent, NOT including header, in bytes */
	off64_t eh_offset;
	int32_t eh_type;
	int32_t eh_flags;
	uint32_t eh_checksum;
	char eh_pad[ 4 ];
};

typedef struct extenthdr extenthdr_t;

#define EXTENTHDR_TYPE_LAST	0
		/* always the last extent. sz is always 0
		 */
#define EXTENTHDR_TYPE_ALIGN	1
		/* used for alignment. sz is the number of bytes to skip,
		 * in addition to the hdr
		 */
#define EXTENTHDR_TYPE_DATA	4
		/* heads an extent of ordinary file data. sz is the data
		 * extent length in bytes, NOT including the hdr. offset
		 * indicates the byte position of the extent within the file.
		 * also heads the path in a symlink dump.
		 */
#define EXTENTHDR_TYPE_HOLE	8
		/* used to encode a hole extent. Offset indicates the byte
		 * position of the hole within the file and sz is the
		 * hole extent length.
		 */

#define EXTENTHDR_FLAGS_CHECKSUM	( 1 << 0 )


/* direnthdr_t - placed at the beginning of every dumped directory entry.
 * a directory entry consists of the fixed size header followed by a variable
 * length NULL-terminated name string, followed by enough padding to make the
 * overall record size a multiple of 8 bytes. the name string begins
 * at the dh_name field. the total number of bytes occupied by
 * each directory entry on media is dh_sz.
 *
 * a sequence of directory entries is always terminated with a null direnthdr_t.
 * this is detected by looking for a zero ino.
 */
typedef uint32_t gen_t;

#define DIRENTHDR_ALIGN	8

#define DIRENTHDR_SZ	24

struct direnthdr {
	xfs_ino_t dh_ino;
	gen_t dh_gen;
	uint32_t dh_checksum;
	uint16_t dh_sz; /* overall size of record */
	char dh_name[ 6 ];
};

typedef struct direnthdr direnthdr_t;

/* the old direnthdr truncated the inode generation number
 * to the low 12 bits.
 */

struct direnthdr_v1 {
	xfs_ino_t dh_ino;
	uint16_t dh_gen; /* generation count & DENTGENMASK of ref'ed inode */
	uint16_t dh_sz; /* overall size of record */
	uint32_t dh_checksum;
	char dh_name[ 8 ];
};

typedef struct direnthdr_v1 direnthdr_v1_t;

/* truncated generation count
 */
#define DENTGENSZ		12	/* leave 4 bits for future flags */
#define DENTGENMASK		(( 1 << DENTGENSZ ) - 1 )
#define BIGGEN2GEN( bg )	( ( gen_t )( bg & DENTGENMASK ))



/* symbolic links will be dumped using an extent header. the extent
 * will always be a multiple of SYMLINK_ALIGN bytes. the symlink character
 * string will always be null-terminated.
 */
#define SYMLINK_ALIGN	8

/* extattr_hdr_t - hdr for an extended attribute. the first byte of the
 * attribute name immediately follows the header. the name is terminated
 * with a NULL byte. Each extattr record will be aligned; there may
 * be padding at the end of each record to achieve this. NOTE: ah_sz
 * includes the hdr sz; adding ah_sz to the offset of one extattr
 * gives the offset of the next.
 */
#define EXTATTRHDR_SZ		16
#define EXTATTRHDR_ALIGN	8

struct extattrhdr {
	uint32_t ah_sz; /* overall size of extended attribute record */
	uint16_t ah_valoff; /* byte offset within record of value */
	uint16_t ah_flags; /* see EXTATTRHDR_FLAGS_... below */
	uint32_t ah_valsz; /* size of value */
	uint32_t ah_checksum; /* hdr checksum */
};

typedef struct extattrhdr extattrhdr_t;

#define EXTATTRHDR_FLAGS_ROOT		( 1 << 0 )
	/* a "root" mode attribute
	 */
#define EXTATTRHDR_FLAGS_NULL		( 1 << 1 )
	/* marks the end of the attributes associated with the leading filehdr_t
	 */
#define EXTATTRHDR_FLAGS_OLD_CHECKSUM	( 1 << 2 )
	/* old xfsdumps used this flag to indicate a checksum is present,
	 * but the checksum was not calculated properly. the presence of
	 * this flag now indicates a checksum that cannot be verified.
	 */
#define EXTATTRHDR_FLAGS_SECURE		( 1 << 3 )
	/* a linux "secure" mode attribute
	 */
#define EXTATTRHDR_FLAGS_CHECKSUM	( 1 << 4 )
	/* checksum is present.
	 */

/* Routines for calculating and validating checksums on xfsdump headers.
 * The header length must be an integral number of uint32_t's.
 */
static inline uint32_t
calc_checksum(void *bufp, size_t len)
{
	uint32_t sum = 0;
	uint32_t *sump = bufp;
	uint32_t *endp = sump + len / sizeof(uint32_t);
	assert(len % sizeof(uint32_t) == 0);
	while (sump < endp)
		sum += *sump++;
	return ~sum + 1;
}

static inline bool_t
is_checksum_valid(void *bufp, size_t len)
{
	uint32_t sum = 0;
	uint32_t *sump = bufp;
	uint32_t *endp = sump + len / sizeof(uint32_t);
	assert(len % sizeof(uint32_t) == 0);
	while (sump < endp)
		sum += *sump++;
	return sum == 0 ? BOOL_TRUE : BOOL_FALSE;
}

#endif /* CONTENT_INODE_H */
