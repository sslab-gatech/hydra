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

#ifndef REC_HDR_H
#define REC_HDR_H


/* this is the drive-specific portion of the drive header defined
 * in drive.h (dh_specific). the first record in a media file begins
 * with a global_hdr_t with this embedded in dh_specific. all subsequent
 * records begin with one of these. note that the first page (STAPE_HDR_SZ)
 * bytes are always reserved for the header; the user data begins at
 * record offset STAPE_HDR_SZ. Note also that the first record of each
 * media file is unused except for the header; user data begins STAPE_HDR_SZ
 * bytes into the second record.
 */

/*
 * This structure is used by both drive_minrmt and drive_scsi.  New
 * strategies should define their own here if they need a different
 * structure, and add a endian conversion function to arch_xlate.c 
 */
struct rec_hdr {
	uint64_t magic;			/*   8   8 */
		/* magic number STAPE_MAGIC (see above)
		 */
	int32_t version;			/*   4   c */
		/* version number STAPE_VERSION (see above)
		 */
	int32_t blksize;			/*   4  10 */
		/* size of tape block in bytes
		 */
	int32_t recsize;			/*   4  14 */
		/* size of this record in bytes, including 4k header
		 */
	int32_t capability;			/*   4  18 */
		/* tape drive capabilities (from drive.h)
		 */
	char pad1[ 8 ];				/*   8  20 */
		/* padding
		 */
	off64_t file_offset;			/*   8  28 */
		/* this record's byte offset into the media file.
		 * raw, includes header.
		 */
	off64_t first_mark_offset;		/*   8  30 */
		/* raw media file byte offset of first mark set
		 * in this record. set to -1 if no marks in record
		 */
	uint32_t rec_used;			/*   4  34 */
		/* portion of record containing user data plus rec hdr (bytes).
		 * normally set to record size. last record written may
		 * indicate smaller value. includes record header.
		 */
	int32_t checksum;			/*   4  38 */

	int32_t ischecksum;			/*   4  3c */

	uuid_t dump_uuid;			/*  10  4c */

	char pad2[ 0x1b4 ];			/* 1b4 200 */
};		/* pad to sizeof drive_hdr_t dh_specific */

typedef struct rec_hdr rec_hdr_t;

#endif /*  REC_HDR_H */
