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
#ifndef MEDIA_H
#define MEDIA_H

/* media.[hc] - media abstraction
 */

/* media_hdr_t - media file header
 *
 * This header structure will be placed at the beginning of the
 * media object files by the drive manager do_begin_write() operator,
 * and will be extracted from the beginnining of the media object files by
 * the do_begin_read() operator. A media header_t has three parts: generally
 * useful info, media strategy-specific info, and upper layer info. The hdr
 * argument of the mo_begin_write() operator will be stuffed into the
 * upper layer info, and extracted for the upper layer by mo_begin_read().
 */
#define MEDIA_HDR_SZ		sizeofmember( drive_hdr_t, dh_upper )

struct media_hdr {
	char mh_medialabel[ GLOBAL_HDR_STRING_SZ ];	/* 100  100 */
		/* label of media object containing file */
	char mh_prevmedialabel[ GLOBAL_HDR_STRING_SZ ];	/* 100  200 */
		/* label of upstream media object */
	char mh_pad1[ GLOBAL_HDR_STRING_SZ ];		/* 100  300 */
		/* in case more labels needed */
	uuid_t mh_mediaid;				/*  10  310 */
		/* ID of media object 	*/
	uuid_t mh_prevmediaid;				/*  10  320 */
		/* ID of upstream media object */
	char mh_pad2[ GLOBAL_HDR_UUID_SZ ];		/*  10  330 */
		/* in case more IDs needed */
	uint32_t mh_mediaix;				/*   4  334 */
		/* 0-based index of this media object within the dump stream */
	uint32_t mh_mediafileix;			/*   4  338 */
		/* 0-based index of this file within this media object */
	uint32_t mh_dumpfileix;			/*   4  33c */
		/* 0-based index of this file within this dump stream */
	uint32_t mh_dumpmediafileix;			/*   4  340 */
		/* 0-based index of file within dump stream and media object */
	uint32_t mh_dumpmediaix;			/*   4  344 */
		/* 0-based index of this dump within the media object */
	int32_t mh_strategyid;				/*   4  348 */
		/* ID of the media strategy used to produce this dump */
	char mh_pad3[ 0x38 ];				/*  38  380 */
		/* padding */
	char mh_specific[ 0x80 ];			/*  80  400 */
		/* media strategy-specific info */
	char mh_upper[ MEDIA_HDR_SZ - 0x400 ];		/* 400  800 */
		/* header info private to upper software layers */
};

typedef struct media_hdr media_hdr_t;

/* macros to mark a media file as a terminator. artifact of original
 * media_rmvtape media strategy
 */
#define MEDIA_TERMINATOR_CHK( mrhp )	( mrhp->mh_specific[ 0 ] & 1 )
#define MEDIA_TERMINATOR_SET( mwhp )	( mwhp->mh_specific[ 0 ] |= 1 )

/* media strategy IDs. artifactis of first version of xfsdump
 */
#define MEDIA_STRATEGY_SIMPLE	0
#define MEDIA_STRATEGY_RMVTAPE	1

#endif /* MEDIA_H */
