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
#ifndef INVENTORY_H
#define INVENTORY_H

#include "types.h"

/* abstract interface to the inventory sub system of xfsdump
 *
 * the inventory contains a description of every xfsdump ever
 * done, unless explicitly deleted. It provides simple and specific
 * storage and query facilities.
 *
 * This was not an attempt to write a generic database. The inventory does have
 * knowledge of the functionalities, some abstractions, and even typical queries
 * of dump() and restore() and uses this knowledge in formulating its storage
 * structure on disk. All these things, of course, are completely abstract with
 * respect to the clients of the inventory. 
 * 
 */

#define INV_DIRPATH		inv_dirpath()
#define INV_TOKEN_NULL 		NULL
#define INV_FSTAB		inv_fstab()
#define INV_INVINDEX_PREFIX     ".InvIndex"
#define INV_STOBJ_PREFIX        ".StObj"

/* length of labels, mntpts, etc */
#define INV_STRLEN              GLOBAL_HDR_STRING_SZ

typedef uint32_t		inv_version_t;

/* This is the general inventory version. 
 */
#define INV_VERSION		(inv_version_t) 1	

/* This is the version of the on-tape (packed) inventory.
 * version 2 has an extra inv_version_t in it to make everything 
 * 64 bit aligned.
 */
#define PACKED_INV_VERSION_1	(inv_version_t) 1
#define PACKED_INV_VERSION_2	(inv_version_t) 2
#define PACKED_INV_VERSION	PACKED_INV_VERSION_2

/* Print [-I] suboption -depth */
#define PR_DEFAULT	0
#define PR_FSONLY 	1
#define PR_SESSONLY 	2
#define PR_STRMSONLY 	3
#define PR_ALL		4
#define PR_MAXDEPTH	5
#define PR_MAXLEVEL	255

/*----------------------------------------------------------------------*/
/* Inventory                                                            */
/*                                                                      */
/* Users are first supposed to call inventory_open() specifying what    */
/* type of handle s/he would like to supply. This can be BY_MOUNTPT,    */
/* BY_DEVPATH, or BY_UUID. The inventory maintains its own table of file*/
/* systems and their uuids, mountpts, and device paths.                 */
/* The current implementation requires that a uuid be present for all   */
/* its fs table entries. However, once the entry is there, caller can   */
/* query by any of the other handles.                                   */
/*                                                                      */
/* For a read-session, ie. just querying, like in restore(), the token  */
/* from inv_open() must always be passed.                               */
/*                                                                      */
/* This inventory has a hierarchical token scheme.                      */
/* For write-sessions, the caller should obtain a session_token         */
/* by calling inv_writesession_open() with the original token.  	*/
/* In order to start writing media files, the caller must    		*/
/* then obtain a stream-token via inv_stream_open() using that    	*/
/* session-token.                                                       */
/*                                                                      */
/* Once, done, stream_close(), writesession_close() and inv_close()     */
/* must be called in that order. To obtain all the data written during  */
/* the course of a single write-session, the user can call inv_get_     */
/* sessioninfo() while still holding the session-token. For inventory   */
/* reconstruction, inv_put_sessioninfo() may be used, and there is no   */
/* requirement to hold a token for that.                                */
/*                                                                      */
/* inv_delete_mediaobj() is, of course, for deletion of all the media   */
/* files contained in a media object.                                   */
/*                                                                      */
/*----------------------------------------------------------------------*/

/* Caller can open the inventory by any of these handles */
typedef enum {
	INV_BY_UUID,
	INV_BY_MOUNTPT,
	INV_BY_DEVPATH
} inv_predicate_t;

	
typedef enum {
	INV_SEARCH_ONLY,
	INV_SEARCH_N_MOD
} inv_oflag_t;

		     
typedef struct inv_mediafile {
	uuid_t		 m_moid;	/* media object id */
	uint		 m_mfile_index; /* index within the media object */
	xfs_ino_t	 m_startino;	/* file that we started out with */
	off64_t		 m_startino_off;
	xfs_ino_t	 m_endino;	/* the dump file we ended this .. */
	off64_t		 m_endino_off;	/* .. media file with. */
	off64_t		 m_size;	/* size of mediafile in bytes */
	bool_t           m_isgood;      /* distinguishes good mfiles */
	bool_t		 m_isinvdump;	/* is this the mfile that has the dump 
					   of the rest of the session? */
	char		 m_label[INV_STRLEN];	/* media file label */
} inv_mediafile_t;


typedef struct inv_stream {
	bool_t		st_interrupted;	/* was this stream interrupted ? */
	
	/* duplicate info from mediafiles for speed */
	xfs_ino_t	st_startino;	/* the starting pt */
	off64_t		st_startino_off;
	xfs_ino_t	st_endino;  	/* where we actually ended up. this 
					   means we've written upto but not 
					   including this breakpoint. */
	off64_t		st_endino_off;
	char            st_cmdarg[INV_STRLEN]; /* the driver path user entered */
	uint		st_nmediafiles; /* number of mediafiles */
	inv_mediafile_t *st_mediafiles; /* array of all media files */
} inv_stream_t;


/* 
 * inventory_session_t 
 * all the information that is kept on a single dump session of a single
 * file system in the inventory.
 * 
 */

typedef struct inv_session {
	uuid_t		 s_fsid;	/* file system */
	uuid_t 		 s_sesid;	/* this session's id:16 bytes*/
	time32_t	 s_time;   	/* time of the dump */
	bool_t		 s_ispartial;	/* was this a partial (dump) ? */
	bool_t           s_isresumed;   /* was this a resumed (dump) ? */
	u_char		 s_level;  	/* dump level */
	char		 s_label[INV_STRLEN];  /* session label */
	char		 s_mountpt[INV_STRLEN];/* path to the mount point */
	char		 s_devpath[INV_STRLEN];/* path to the device */
	uint		 s_nstreams;	/* num of media streams recorded */
	inv_stream_t	*s_streams;	/* array of streams */
        uint            s_refnum;      /* storage location dependent ref.
					   used in displaying the session and 
					   nowhere else */
					   
} inv_session_t;
 


struct invt_desc_entry;
struct invt_sesdesc_entry;
struct invt_strdesc_entry;

/* The three kinds of access tokens for the inventory */
typedef struct invt_desc_entry		*inv_idbtoken_t;
typedef struct invt_sesdesc_entry	*inv_sestoken_t;
typedef struct invt_strdesc_entry	*inv_stmtoken_t;




/* inventory_open - initializes access to the inventory
 */
extern inv_idbtoken_t
inv_open( 
	 inv_predicate_t bywhat, /* BY_UUID, BY_MOUNTPT, BY_DEVPATH */	
	 inv_oflag_t      forwhat,/* SEARCH_ONLY, SEARCH_N_MOD */
	 void 		 *pred );/* uuid_t *,char * mntpt, or char *dev */


extern bool_t
inv_close( 
	inv_idbtoken_t	tok );


extern inv_sestoken_t
inv_writesession_open(
	inv_idbtoken_t 	tok, 	/* token obtained by inventory_init() */
	uuid_t		*fsid,
	uuid_t		*sesid,
	char		*label,
	bool_t		ispartial,
        bool_t          isresumed,
	u_char		level,
	uint		nstreams,
	time32_t	time,
	char		*mntpt,
	char		*devpath );	      

extern bool_t
inv_writesession_close( 
	inv_sestoken_t  tok );

extern inv_stmtoken_t
inv_stream_open(
	inv_sestoken_t 	tok,
	char		*cmdarg );	

extern bool_t
inv_stream_close(
	inv_stmtoken_t	tok,
	bool_t 		wasinterrupted );

extern bool_t
inv_put_mediafile( 
	inv_stmtoken_t 	tok, 
	uuid_t 		*moid, 
	char 		*label,
	uint		mfileindex,
	xfs_ino_t	startino,
	off64_t		startino_offset,
	xfs_ino_t	endino,
	off64_t		endino_offset,
	off64_t		size,	  /* size of the mediafile in bytes */
	bool_t          isgood,   /* to distinguish unimportant media
				     files from others */
	bool_t		isinv_dump);

/* lasttime_level_lessthan - finds the time of the last dump of the
 * specified file system at a level less than the specified level.
 * if never dumped below the current level, *time is set to NULL.
 * 
 */
extern bool_t
inv_lasttime_level_lessthan( 
	uuid_t			*fsidp,
	inv_idbtoken_t 		tok,
	u_char			level,
	time32_t		**time); /* out */

extern bool_t
inv_lastsession_level_lessthan( 
	uuid_t			*fsidp,
	inv_idbtoken_t 		tok,			     
	u_char  		level,
	inv_session_t		**ses); /* out */

extern bool_t
inv_lastsession_level_equalto( 
	uuid_t			*fsidp,
	inv_idbtoken_t 		tok,			     
	u_char  		level,
	inv_session_t		**ses); /* out */

/* Given a uuid of a session, return the session structure.*/
extern bool_t
inv_get_session_byuuid(
	uuid_t		*fsidp,
	uuid_t		*sesid,
	inv_session_t	**ses);

extern bool_t
inv_get_session_bylabel(
	uuid_t		*fsidp,
	char		*session_label,
	inv_session_t	**ses);


/* on return, *ses is NULL */
extern void
inv_free_session(
	inv_session_t **ses);


/* For dumping the inventory once a dump is done. */
extern bool_t
inv_get_sessioninfo(
	inv_sestoken_t		tok,
	void		      **bufpp,		/* out */
	size_t		       *bufszp );	/* out */


extern bool_t
inv_delete_mediaobj( uuid_t *moid );

extern bool_t
inv_DEBUG_print( int argc, char **argv );

extern int
inv_setup_base( void );

extern char *
inv_dirpath( void );

extern char *
inv_fstab( void );

extern char *
inv_lockfile( void );

extern char *
inv_basepath( void );


#endif /* INVENTORY_H */
