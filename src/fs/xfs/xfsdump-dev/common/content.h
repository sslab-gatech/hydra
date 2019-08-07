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
#ifndef CONTENT_H
#define CONTENT_H

/* content.[hc] - dump/restore content strategy abstraction
 */


/* content_hdr_t - content file header
 *
 * This header structure will be placed at the beginning of the
 * content media files by the media manager mo_begin_write() operator,
 * and will be extracted from the beginning of the media object files by
 * the mo_begin_read() operator. A content header_t has three parts: generally
 * useful info, content strategy-specific info, and upper layer info. The hdr
 * argument of the co_begin_write() operator will be stuffed into the
 * upper layer info, and extracted for the upper layer by co_begin_read().
 */
#define CONTENT_HDR_SZ		sizeofmember( media_hdr_t, mh_upper )
#define CONTENT_HDR_FSTYPE_SZ	16
#define CONTENT_STATSZ		160 /* must match dlog.h DLOG_MULTI_STATSZ */

struct content_hdr {
	char ch_mntpnt[ GLOBAL_HDR_STRING_SZ ];		/* 100  100 */
		/* full pathname of fs mount point */
	char ch_fsdevice[ GLOBAL_HDR_STRING_SZ ];	/* 100  200 */
		/* full pathname of char device containing fs */
	char  ch_pad1[GLOBAL_HDR_STRING_SZ];		/* 100  300 */
		/* in case another label is needed */
	char ch_fstype[ CONTENT_HDR_FSTYPE_SZ ];	/*  10  310 */
		/* from fsid.h */
	uuid_t ch_fsid;					/*  10  320 */
		/* file system uuid */
	char  ch_pad2[ GLOBAL_HDR_UUID_SZ ];		/*  10  330 */
		/* in case another id is needed */
	char ch_pad3[ 8 ];				/*   8  338 */
		/* padding */
	int32_t ch_strategyid;				/*   4  33c */
		/* ID of the content strategy used to produce this dump */
	char ch_pad4[ 4 ];				/*   4  340 */
		/* alignment */
	char ch_specific[ 0xc0 ];			/*  c0  400 */
		/* content strategy-specific info */
};

typedef struct content_hdr content_hdr_t;	

#define CONTENT_QUOTAFILE	"xfsdump_quotas"
#define CONTENT_PQUOTAFILE	"xfsdump_quotas_proj"
#define CONTENT_GQUOTAFILE	"xfsdump_quotas_group"

#ifdef DUMP

struct quota_info {
	char *	desc;		/* Quotas type (user, project, etc) */
	bool_t	savequotas;	/* Quotas saved OK */
	char *	quotafile;	/* Filename quota info is stored in */
	char	quotapath[MAXPATHLEN]; /* Full path to quotafile */
	char *	repquotaargs;	/* Args to repquota to create this quotafile */
	int	statflag;	/* quota stats flag for this type */
	ino_t	quotaino;	/* ino of the quota file */
};

typedef struct quota_info quota_info_t;

extern bool_t is_quota_file(ino_t ino);

#endif /* DUMP */

#ifdef DUMP
extern bool_t content_init( int argc,
			    char *argv[ ],
			    global_hdr_t *gwhdrtemplatep );
	/* prepares for multi-stream dump
	 */

extern int content_stream_dump( ix_t strmix );
	/* does stream dump
	 */

#endif /* DUMP */
#ifdef RESTORE
extern size_t perssz;

extern bool_t content_init( int argc, char *argv[ ], size64_t vmsz );
	/* prepares for multi-thread restore
	 */

extern int content_stream_restore( ix_t thrdix );
	/* does thread restore
	 */

extern bool_t content_overwrite_ok( char *path,
				    int32_t ctime,
				    int32_t mtime,
				    char **reasonstrp,
				    bool_t *exists );
	/* called by tree to ask if ok to overwrite file
	 */

extern void content_showinv( void );
	/* displays inventory of dump session being restored,
	 * in the context of a second-level dialog
	 */

extern void content_showremainingobjects( void );
	/* displays info on media objects remaining to be restored.
	 */
#endif /* RESTORE */

extern bool_t content_complete( void );
	/* cleanup: called from main thread. returns TRUE if complete
	 */

extern size_t content_statline( char **lines[ ] );
	/* supplies status line for main keyboard intr dialog
	 * returns by ref an array of character strings, and the length
	 * of the array is returned by value.
	 */

extern bool_t content_media_change_needed;
	/* queried by main thread to decide if interupt dialog needed
	 * for media change confirmation.
	 */
extern char *content_mediachange_query( void );
	/* invoked by main thread sigint dialog to allow operator to
	 * confirm media changes and ask what media objects remain
	 */

#endif /* CONTENT_H */
