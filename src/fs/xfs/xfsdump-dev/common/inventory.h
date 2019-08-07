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
#define INV_STRLEN              128      /* length of labels, mntpts, etc */


/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/* Users are first supposed to call inventory_open() specifying what    */
/* type of handle s/he would like to supply. This can be BY_MOUNTPT,    */
/* BY_DEVPATH, or BY_UUID. The inventory maintains its own table of file*/
/* systems and their uuids, mountpts, and device paths. (XXX host).     */
/* The current implementation requires that a uuid be present for all   */
/* its fs table entries. However, once the entry is there, caller can   */
/* query by any of the other handles.                                   */
/*                                                                      */
/* For a read-session, ie. just querying, like in restore(), the token  */
/* from inventory_open() must always be passed.                         */
/*                                                                      */
/* This inventory has a hierarchical token scheme.                      */
/* For write-sessions, the caller should obtain a session_token         */
/* by calling inventory_session_open() with the original token.         */
/* In order to start writing media files, the caller must    		*/
/* then obtain a stream_token via inventory_stream_open() using that    */
/* session_token.                                                       */
/*                                                                      */
/* Once, done, stream_close(), session_close() and inventory_close()    */
/* must be called in that order.                                        */
/*                                                                      */
/*----------------------------------------------------------------------*/

/* Caller can open the inventory by any of these handles */
typedef enum {
	INV_BY_UUID,
	INV_BY_MOUNTPT,
	INV_BY_DEVPATH
} inv_predicate_t;


typedef struct inv_stream {
	bool_t		st_interrupted;	/* was this stream interrupted ? */
	
	/* duplicate info from mediafiles for speed */
	xfs_ino_t	st_startino;	/* the starting pt */
	off64_t		st_startino_off;
	xfs_ino_t	st_endino;	/* where we actually ended up. this means
					   we've written upto but not including
					   this breakpoint. */
	off64_t		st_endino_off;
	int		st_nmediafiles; /* number of mediafiles */
} inv_stream_t;


/* 
 * inventory_session_t 
 * all the information that is kept on a single dump session of a single
 * file system in the inventory.
 * 
 */

typedef struct inv_session {
	uuid_t		 s_fsid;	/* file system */
	uuid_t 		 s_sesid;	/* this dump session's id: 16 bytes*/
	uint		 s_nstreams;	/* number of media streams recorded */
	inv_stream_t	*s_streams;	/* array of streams */
	time32_t	 s_time;   	/* time of the dump */
	u_char		 s_level;  	/* dump level */
	char		 s_label[INV_STRLEN];  /* session label, assigned by the
						  operator */
	char		 s_mountpt[INV_STRLEN];/* path to the mount point */
	char		 s_devpath[INV_STRLEN];/* path to the device */
} inv_session_t;
 
	
/* Is there anything else that you need here, Chuck? */
typedef struct inv_mediafile {
	uuid_t		 m_moid;	/* media object id */
	xfs_ino_t	 m_startino;	/* file that we started out with */
	off64_t		 m_startino_off;
	xfs_ino_t	 m_endino;	/* the dump file we ended this .. */
	off64_t		 m_endino_off;	/* .. media file with. */
	char		 m_label[INV_STRLEN];	/* media file label */
} inv_mediafile_t;


/* the list of inos returned by inv_get_inolist() */
typedef struct inolist {
	xfs_ino_t		 i_ino;
	struct inolist	*i_next;	/* NULL if last element */
} inv_inolist_t;


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
	 void 		 *pred );/* uuid_t *,char * mntpt, or char *dev */


extern bool_t
inv_close( 
	inv_idbtoken_t tok );


extern inv_sestoken_t
inv_writesession_open(
	inv_idbtoken_t 	tok, 	/* token obtained by inventory_init() */
	uuid_t		*fsid,
	uuid_t		*sesid,
	char		*label,
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
	inv_sestoken_t 	tok );

extern bool_t
inv_stream_close(
	inv_stmtoken_t	tok,
	bool_t 		wasinterrupted );

extern bool_t
inv_put_mediafile( 
	inv_stmtoken_t 	tok, 
	uuid_t 		*moid, 
	char 		*label,
	xfs_ino_t	startino,
	off64_t		startino_offset,
	xfs_ino_t	endino,
	off64_t		endino_offset );

/* lasttime_level_lessthan - finds the time of the last dump of the
 * specified file system at a level less than the specified level.
 * if never dumped below the current level, *time is set to NULL.
 * 
 */
extern bool_t
inv_lasttime_level_lessthan( 
	inv_idbtoken_t 		tok,
	u_char  		level,
	time32_t		**time );/* out */

extern bool_t
inv_lastsession_level_lessthan( 
	inv_idbtoken_t 		tok,			     
	u_char  		level,
	inv_session_t		**ses );/* out */

extern bool_t
inv_lastsession_level_equalto( 
	inv_idbtoken_t 		tok,			     
	u_char  		level,
	inv_session_t		**ses );/* out */

extern bool_t
inv_get_inolist(
	inv_inolist_t 		**inolist );

/* For dumping the inventory once a dump is done. */
extern bool_t
inv_get_session(
	inv_sestoken_t		tok,
	void		      **bufpp,		/* out */
	size_t		       *bufszp );	/* out */

/* To reconstruct a compelete inventory from dumped inventories */
extern bool_t
inv_put_session(
	inv_idbtoken_t		tok,
	void		       *bufp,
	size_t		        bufsz );

#ifdef DEBUG

bool_t
inv_DEBUG_printallsessions( 
	inv_idbtoken_t 	tok,			    
	inv_session_t	**ses );

#endif /* ifdef DEBUG */

extern int
inv_setup_base( void );

extern char *
inv_dirpath( void );

extern char *
inv_fstab( void );

extern char *
inv_lockfile( void );


#endif /* INVENTORY_H */
