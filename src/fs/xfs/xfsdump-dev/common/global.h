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
#ifndef GLOBAL_H
#define GLOBAL_H

/* global_hdr_t - first page of every media file
 */
#define GLOBAL_HDR_SZ		PGSZ
#define GLOBAL_HDR_MAGIC	"xFSdump0"
#define GLOBAL_HDR_MAGIC_SZ	8

#define GLOBAL_HDR_VERSION_0	0
#define GLOBAL_HDR_VERSION_1	1
#define GLOBAL_HDR_VERSION_2	2
#define GLOBAL_HDR_VERSION_3	3
	/* version 3 uses the full 32-bit inode generation number in direnthdr_t.
	 * version 2 adds encoding of holes and a change to on-tape inventory format.
	 * version 1 adds extended file attribute dumping.
	 * version 0 xfsrestore can't handle media produced
	 * by version 1 xfsdump. 
	 */
#define GLOBAL_HDR_VERSION	GLOBAL_HDR_VERSION_3

#define GLOBAL_HDR_STRING_SZ	0x100
#define GLOBAL_HDR_TIME_SZ	4
#define GLOBAL_HDR_UUID_SZ	0x10

struct global_hdr {
	char gh_magic[ GLOBAL_HDR_MAGIC_SZ ];		/*   8    8 */
		/* unique signature of xfsdump */
	uint32_t gh_version;				/*   4    c */
		/* header version */
	uint32_t gh_checksum;				/*   4   10 */
		/* 32-bit unsigned additive inverse of entire header */
	time32_t gh_timestamp;				/*   4   14 */
		/* time32_t of dump */
	char gh_pad1[ 4 ];				/*   4   18 */
		/* alignment */
	uint64_t gh_ipaddr;				/*   8   20 */
		/* from gethostid(2), room for expansion */
	uuid_t gh_dumpid;				/*  10   30 */
		/* ID of dump session	 */
	char gh_pad2[ 0xd0 ];				/*  d0  100 */
		/* alignment */
	char gh_hostname[ GLOBAL_HDR_STRING_SZ ];	/* 100  200 */
		/* from gethostname(2) */
	char gh_dumplabel[ GLOBAL_HDR_STRING_SZ ];	/* 100  300 */
		/* label of dump session */
	char gh_pad3[ 0x100 ];				/* 100  400 */
		/* padding */
	char gh_upper[ GLOBAL_HDR_SZ - 0x400 ];		/* c00 1000 */
		/* header info private to upper software layers */
};

typedef struct global_hdr global_hdr_t;


/* used by main( ) to allocate and populate a global header template.
 * drive managers will copy this into the write header.
 */
extern global_hdr_t * global_hdr_alloc( int argc, char *argv[ ] );


/* used by main( ) to free the global header template after drive ini.
 */
extern void global_hdr_free( global_hdr_t *ghdrp );


/* global_hdr_checksum_set - fill in the global media file header checksum.
 * utility function for use by drive-specific strategies.
 */
extern void global_hdr_checksum_set( global_hdr_t *hdrp );


/* global_hdr_checksum_check - check the global media file header checksum.
 * utility function for use by drive-specific strategies.
 * returns BOOL_TRUE if ok, BOOL_FALSE if bad
 */
extern bool_t global_hdr_checksum_check( global_hdr_t *hdrp );

/* global_version_check - if we know this version number, return BOOL_TRUE 
 * else return BOOL_FALSE
 */

extern bool_t global_version_check( uint32_t version );


#endif /* GLOBAL_H */
