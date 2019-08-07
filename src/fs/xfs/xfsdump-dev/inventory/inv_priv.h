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
#ifndef _INV_PRIV_H_
#define _INV_PRIV_H_

#include <sys/file.h> /* for flock */
#include "global.h"
#include "inventory.h"
#include "mlog.h"


/*----------------------------------------------------------------------*/
/* The Inventory stores its data in a hierarchical manner.              */
/*                                                                      */
/* At the first level is the Fstab, which maintains a table of unique   */
/* file system ids for all file systems that it knows of. From this fsid*/
/* we can derive the Inventory Index for that file system.              */
/*                                                                      */
/* Inventory Index contains references to all the Storage Objects that  */
/* carry data associated with (its) file system. The Storage Objects,   */
/* the third level (of indirection) are reserved for certain (variable) */
/* time periods, and the inv-index keeps track of those timeperiods     */
/* in its table. In other words, for a given time, there is a unique    */
/* stobj that exists to contain a session that occured at that time..   */
/*                                                                      */
/*----------------------------------------------------------------------*/

#define INV_LOCKFILE		inv_lockfile()
#define INVTSESS_COOKIE		"idbsess0"
#define INVT_STOBJ_MAXSESSIONS	5
#define INVT_MAX_INVINDICES	-1	/* unlimited */
#define FSTAB_UPDATED		1
#define NEW_INVINDEX		2

/* session flags ( seshdr.sh_flag ) */
#define INVT_PARTIAL		(uint)0x0001
#define INVT_RESUMED            (uint)0x0002

/* media file flags ( mfile.mf_flag ) */
#define INVT_MFILE_GOOD         (u_char)0x01
#define INVT_MFILE_INVDUMP      (u_char)0x02

#define INVT_ENDTIME		BOOL_FALSE
#define INVT_STARTTIME		BOOL_TRUE
#define INVT_DOLOCK		BOOL_TRUE
#define INVT_DONTLOCK		BOOL_FALSE

#define INVLOCK( fd, m )	flock( fd, m ) 

/* return values */
#define INV_OK			(int) 1
#define INV_ERR			(int) -1

#define I_DONE                  (int) -1
#define I_EMPTYINV              (int) -2

/* flags to oref_resolve_new_stobj() */
#define IS_EMPTY		BOOL_TRUE
#define IS_NOT_EMPTY		BOOL_FALSE

#define INV_PERMS               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

#define IS_WITHIN(ttpe, tm)	( tm >= (ttpe)->tp_start && \
				  tm <= (ttpe)->tp_end )
#define IDX_HDR_OFFSET(n) 	(off64_t) ( sizeof( invt_entry_t ) * \
					   (size_t) (n)\
					   + sizeof( invt_counter_t ) )
#define STOBJ_OFFSET( nhdrs, nsess ) (off64_t) ( \
			       sizeof( invt_sescounter_t ) + \
			       (size_t) nhdrs * sizeof( invt_seshdr_t ) + \
			       (size_t) nsess * sizeof( invt_session_t ) )

#define STREQL( n,m )		( strcmp((n),(m)) == 0 )
#define UUID_EQL( n,m,t )	( uuid_compare( n, m, t ) == 0 )
#define IS_PARTIAL_SESSION( h ) ( (h)->sh_flag & INVT_PARTIAL )
#define IS_RESUMED_SESSION( h ) ( (h)->sh_flag & INVT_RESUMED )
#define SC_EOF_INITIAL_POS	(off64_t) (sizeof( invt_sescounter_t ) + \
					 INVT_STOBJ_MAXSESSIONS * \
					 ( sizeof( invt_seshdr_t ) + \
					   sizeof( invt_session_t ) ) )

#define INV_PERROR(s)  	mlog( MLOG_NORMAL,"INV: %s %s\n", s,strerror(errno) )
#define INV_OFLAG(f)    ( f == INV_SEARCH_ONLY ) ? O_RDONLY: O_RDWR

/*----------------------------------------------------------------------*/
/* On Disk Data Structures                                              */
/*                                                                      */
/*----------------------------------------------------------------------*/

typedef struct invt_session {
	uuid_t 		 s_sesid;	/* this session's id: 16 bytes*/
	uuid_t		 s_fsid;	/* file system id */
	char		 s_label[INV_STRLEN];  /* session label */
	char		 s_mountpt[INV_STRLEN];/* path to the mount point */
	char		 s_devpath[INV_STRLEN];/* path to the device */
	uint		 s_cur_nstreams;/* number of streams created under
					   this session so far */
	uint		 s_max_nstreams;/* number of media streams in 
					   the session */
	char		 s_padding[16];
} invt_session_t;			   
 
typedef struct invt_seshdr {
	off64_t		sh_sess_off;    /* offset to rest of the sessioninfo */
	off64_t		sh_streams_off; /* offset to start of the set of 
					   stream hdrs */
	time32_t	sh_time;        /* time of the dump */
	uint32_t	sh_flag;        /* for misc flags */
	u_char		sh_level;       /* dump level */
	u_char		sh_pruned;      /* pruned by invutil flag */
	char		sh_padding[22];
} invt_seshdr_t;


/* Each session consists of a number of media streams. While it is given that
   there won't be multiple writesessions (ie. dumpsessions) concurrently,
   there can be multiple media streams operating on a single file system, 
   each writing media files within its own stream. Hence, we have a linked
   list of media files, that the stream keeps track of. */


typedef struct invt_breakpt {
	xfs_ino_t	ino;		/* the 64bit inumber */
	off64_t		offset;		/* offset into the file */
} invt_breakpt_t;

typedef struct invt_stream {
	/* duplicate info from mediafiles for speed */
	invt_breakpt_t	st_startino;	/* the starting pt */
	invt_breakpt_t	st_endino;	/* where we actually ended up. this
					   means we've written upto but not
					   including this breakpoint. */
	off64_t		st_firstmfile;	/*offsets to the start and end of*/
	off64_t		st_lastmfile;	/* .. linked list of mediafiles */
	char            st_cmdarg[INV_STRLEN]; /* drive path */
	uint		st_nmediafiles; /* number of mediafiles */
	bool_t		st_interrupted;	/* was this stream interrupted ? */
	char		st_padding[16];
} invt_stream_t;



typedef struct invt_mediafile {
	uuid_t		 mf_moid;	/* media object id */
	char		 mf_label[INV_STRLEN];	/* media file label */
	invt_breakpt_t	 mf_startino;	/* file that we started out with */
	invt_breakpt_t	 mf_endino;	/* the dump file we ended this 
					   media file with */
	off64_t		 mf_nextmf;	/* links to other mfiles */
	off64_t		 mf_prevmf;
	uint		 mf_mfileidx; 	/* index within the media object */
	u_char           mf_flag;       /* Currently MFILE_GOOD, INVDUMP */
	off64_t		 mf_size;       /* size of the media file */
	char		 mf_padding[15];
} invt_mediafile_t;



typedef struct invt_timeperiod {
	time32_t	tp_start;
	time32_t	tp_end;
} invt_timeperiod_t;

typedef struct invt_entry {
	invt_timeperiod_t 	ie_timeperiod;
	char			ie_filename[INV_STRLEN];
	char   			ie_padding[16];
} invt_entry_t;

/* Cheap Inheritance, and an attempt to avoid a nested type */
#define INVT_COUNTER_FIELDS  \
        uint32_t      ic_vernum;/* on disk version number for posterity */\
	uint	      ic_curnum;/* number of sessions/invindices recorded \
				   so far */				  \
	uint	      ic_maxnum;/* maximum number of sessions/inv_indices \
				   that we can record on this stobj */
#define INVT_COUNTER_FIELDS_SIZE 0xc

typedef struct invt_counter {
	INVT_COUNTER_FIELDS
	char	      ic_padding[0x20 - INVT_COUNTER_FIELDS_SIZE];
} invt_counter_t;

typedef struct invt_sescounter {
	INVT_COUNTER_FIELDS
	off64_t	       ic_eof;   /* current end of the file, where the next
				 media file or stream will be written to */
	char 	       ic_padding[0x20 - ( INVT_COUNTER_FIELDS_SIZE + 
					   sizeof( off64_t) )];
} invt_sescounter_t;


typedef struct invt_fstab {
	uuid_t	ft_uuid;
	char	ft_mountpt[INV_STRLEN];
	char	ft_devpath[INV_STRLEN];
	char 	ft_padding[16];
} invt_fstab_t;



/*----------------------------------------------------------------------*/
/* The Tokens                                                           */
/*                                                                      */
/*----------------------------------------------------------------------*/
typedef struct invt_desc_entry {
	int	d_invindex_fd;	/* open file descriptor of inv index */
	int	d_stobj_fd;	/* fd of storage object */
	u_char	d_update_flag;  /* indicates whether fstab was updated with
				   this file system or not and also if
				   we had to create a new invindex file */
	inv_oflag_t d_oflag;    /* the open mode (SEARCH_ONLY, SEARCH_N_MOD) */
	off64_t	d_invindex_off; /* for every session, we need a reference 
				   to its invindex entry, so that when we
				   close a session, we know which one */
} invt_desc_entry_t;


typedef struct invt_sesdesc_entry {
	invt_desc_entry_t *sd_invtok;	/* generic inventory token */
	off64_t		   sd_session_off;
	off64_t		   sd_sesshdr_off;
	time32_t	   sd_sesstime;	/* time that session started. 
					   needed for closing the session */
} invt_sesdesc_entry_t;
	
struct invt_mediafile;

typedef struct invt_strdesc_entry {
	invt_sesdesc_entry_t  *md_sesstok;   /* the session token */
	off64_t		       md_stream_off;/* offset to the media stream 
						header */	
	struct invt_mediafile *md_lastmfile; /* just so that we dont have
						to get it back from disk
						when we add the next mfile
						to the linked list */

} invt_strdesc_entry_t;
	



/*----------------------------------------------------------------------*/
/* Info Structures - simple logical containers for sessions and indices */
/*                                                                      */
/*----------------------------------------------------------------------*/

typedef struct invt_sessinfo {
	int 		  stobjfd;
	invt_seshdr_t    *seshdr; 
	invt_session_t   *ses;
	invt_stream_t    *strms;
	invt_mediafile_t *mfiles;
} invt_sessinfo_t;

typedef struct invt_idxinfo {
	int 		invfd;
	invt_counter_t	*icnt;
	invt_entry_t	*iarr;
	uint		index;
}invt_idxinfo_t;

#define INVT_MOID	1
#define INVT_LABEL	2
#define INVT_NULLTYPE	-1
typedef struct invt_mobjinfo {
	int 		type;
	void		*value;
} invt_mobjinfo_t;


typedef struct invt_pr_ctx {
	int 			index;
	int			depth;
	bool_t			fstab;
	bool_t			invidx;
	bool_t			invcheck;
	invt_mobjinfo_t		mobj;
	u_char			level;
} invt_pr_ctx_t;

typedef bool_t (*search_callback_t) (int, invt_seshdr_t *, void *, void *);


#define GET_REC( fd, buf, sz, off )  \
                 get_invtrecord( fd, buf, sz, off, INVT_DOLOCK )

#define GET_REC_NOLOCK( fd, buf, sz, off )  \
                 get_invtrecord( fd, buf, sz, off, INVT_DONTLOCK )

#define GET_ALLHDRS_N_CNTS( fd, h, c, hsz, csz ) \
                 get_headerinfo( fd, h, c, hsz, csz, INVT_DOLOCK )

#define GET_ALLHDRS_N_CNTS_NOLOCK( fd, h, c, hsz, csz ) \
                 get_headerinfo( fd, h, c, hsz, csz, INVT_DONTLOCK )

#define PUT_REC( fd, buf, sz, off )  \
                 put_invtrecord( fd, buf, sz, off, INVT_DOLOCK )

#define PUT_REC_NOLOCK( fd, buf, sz, off )  \
                 put_invtrecord( fd, buf, sz, off, INVT_DONTLOCK )


#define GET_COUNTERS( fd, cnt ) get_counters( fd, (void **)(cnt), \
					      sizeof(invt_counter_t) )
#define GET_SESCOUNTERS( fd, cnt ) get_counters( fd, (void **)(cnt), \
						sizeof(invt_sescounter_t) )

#define PUT_COUNTERS( fd, cnt ) PUT_REC_NOLOCK( fd, (void *)(cnt), \
					     sizeof( invt_counter_t ), \
					     (off64_t) 0 )

#define PUT_SESCOUNTERS( fd, cnt )  PUT_REC_NOLOCK( fd, (void *)(cnt), \
					     sizeof( invt_sescounter_t ), \
					     (off64_t) 0 )

#define GET_SESHEADERS( fd, hdrs, n ) get_headers( fd, (void**)(hdrs), \
				   (size_t) ( n * sizeof(invt_seshdr_t ) ),\
				          sizeof( invt_sescounter_t )  )
						   
#define GET_ENTRIES(fd, hdrs, n, sz) get_headers(fd, (void**)(hdrs), \
				   (size_t) (n * sz), sizeof(invt_counter_t))

/*----------------------------------------------------------------------*/
/* Function Prototypes							*/
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
inv_idbtoken_t
idx_create( char *fname, inv_oflag_t forwhat );

int
idx_create_entry( inv_idbtoken_t *tok, int invfd, bool_t firstentry );

int
idx_put_sesstime( inv_sestoken_t tok, bool_t whichtime);


int
idx_find_stobj( invt_idxinfo_t *idx, time32_t tm );

uint
idx_insert_newentry( int fd, int *stobjfd, invt_entry_t *iarr, 
		     invt_counter_t *icnt,
		     time32_t tm );
int
idx_put_newentry( invt_idxinfo_t *idx, invt_entry_t *ient );

int
idx_get_stobj( int invfd, inv_oflag_t forwhat, int *index );

int
idx_recons_time( time32_t tm, invt_idxinfo_t *idx );

int
idx_DEBUG_printinvindices( invt_entry_t *iarr, uint num );

int
idx_DEBUG_print ( int fd );

/*----------------------------------------------------------------------*/

int
stobj_create( char *fname );

int
stobj_create_session( inv_sestoken_t tok, int fd, invt_sescounter_t *sescnt, 
		      invt_session_t *ses, invt_seshdr_t *hdr );

int
stobj_put_mediafile( inv_stmtoken_t tok, invt_mediafile_t *mf );

off64_t
stobj_put_session( 
	int fd, 
	invt_sescounter_t *sescnt, 
	invt_session_t *ses, 
	invt_seshdr_t *hdr,
	invt_stream_t *strms,
	invt_mediafile_t *mfiles );

int
stobj_put_streams( int fd, invt_seshdr_t *hdr, invt_session_t *ses, 
		   invt_stream_t *strms,
		   invt_mediafile_t *mfiles );

int
stobj_hdrcmp( const void *h1, const void *h2 );

int
stobj_sortheaders( int fd, uint num );

uint
stobj_find_splitpoint( int fd, invt_seshdr_t *harr, uint ns, time32_t tm );

int
stobj_split( invt_idxinfo_t *idx, int fd, invt_sescounter_t *sescnt, 
	     invt_sessinfo_t *newsess );
bool_t
stobj_replace_session( int fd, invt_sescounter_t *sescnt, invt_session_t *ses,
		       invt_seshdr_t *hdr, invt_sessinfo_t *newsess );

int
stobj_delete_mfile( int fd, inv_stream_t *strm, invt_mediafile_t *mf,
		    off64_t  mfileoff );

bool_t
stobj_pack_sessinfo( int fd, invt_session_t *ses, invt_seshdr_t *hdr,
		     void  **bufpp, size_t *bufszp );

bool_t
stobj_unpack_sessinfo(  
        void              *bufp,
        size_t             bufsz,
	invt_sessinfo_t   *s );

bool_t
stobj_delete_sessinfo( int fd, invt_sescounter_t *sescnt,
		       invt_session_t *ses, invt_seshdr_t *hdr );

bool_t
stobj_delete_mobj( int fd, invt_seshdr_t *hdr, void *arg,
		   void **buf );

int
stobj_get_sessinfo ( inv_sestoken_t tok, invt_seshdr_t *hdr, invt_session_t *ses );

void
stobj_makefname( char *fname );

int
stobj_insert_session( invt_idxinfo_t *idx, int fd,
		      invt_sessinfo_t *s );

int
stobj_make_invsess( int fd, inv_session_t **buf, invt_seshdr_t *hdr );

int
stobj_copy_invsess( int fd, invt_seshdr_t *hdr, invt_session_t *ses, 
		    inv_session_t **buf);

void
DEBUG_sessionprint( inv_session_t *ses, uint ref, invt_pr_ctx_t *prctx);

void
DEBUG_sessprint( invt_session_t *ses );

bool_t
stobj_getsession_byuuid(int fd, invt_seshdr_t *hdr, void *sesid, void **buf);

bool_t
stobj_getsession_bylabel(int fd, invt_seshdr_t *hdr, void *label, void **buf);


void
stobj_convert_session(inv_session_t *ises, invt_session_t *ses, 
		      invt_seshdr_t *hdr);

void
stobj_convert_strm(inv_stream_t *expstrm, invt_stream_t *strm);

void
stobj_convert_mfile(inv_mediafile_t *expmf, invt_mediafile_t *mf);

void
stobj_convert_sessinfo(inv_session_t **buf, invt_sessinfo_t *sinfo);

/*----------------------------------------------------------------------*/

int
fstab_get_fname( void *pred, char *fname, inv_predicate_t bywhat, 
		 inv_oflag_t forwhat );

int
fstab_put_entry( uuid_t *fsidp, char *mntpt, char *dev, inv_oflag_t forwhat );

int
fstab_getall( invt_fstab_t **arr, invt_counter_t **cnt, int *numfs, 
	      inv_oflag_t forwhat );

void
fstab_DEBUG_print( invt_fstab_t *arr, int num );


/*----------------------------------------------------------------------*/

int
get_invtentry( char *fname, time32_t tm, invt_entry_t *buf, size_t bufsz );

int
get_invtrecord( int fd, void *buf, size_t bufsz, off64_t off, bool_t dolock );

int
put_invtrecord( int fd, void *buf, size_t bufsz, off64_t off, bool_t dolock );

inv_idbtoken_t
get_token( int fd, int objfd );

void
destroy_token( inv_idbtoken_t tok );


int
get_headers( int fd, void **hdrs, size_t bufsz, size_t cntsz );

int
get_counters( int fd, void **cntpp, size_t sz );

int
get_sescounters( int fd, invt_sescounter_t **cntpp );

int
get_lastheader( int fd, void **ent, size_t hdrsz,  size_t cntsz );


inv_sestoken_t
get_sesstoken( inv_idbtoken_t tok );

int
get_headerinfo( int fd, void **hdrs, void **cnt,
	        size_t hdrsz, size_t cntsz, bool_t doblock );

bool_t
invmgr_query_all_sessions(uuid_t *fsidp, void *inarg, void **outarg,
			  search_callback_t func);

int
search_invt(uuid_t *fsidp, int invfd, void *arg, void **buf,
	    search_callback_t do_chkcriteria);
int
invmgr_inv_print( int invfd, invt_pr_ctx_t *prctx);

int
invmgr_inv_check( int invfd );

bool_t
tm_level_lessthan( int fd, invt_seshdr_t *hdr, void *arg,
		   void **tm );

bool_t
lastsess_level_lessthan( int fd, invt_seshdr_t *hdr,  void *arg,
			 void **buf );

bool_t
lastsess_level_equalto( int fd, invt_seshdr_t *hdr,  void *arg, void **buf );

int
DEBUG_displayallsessions( int fd, invt_seshdr_t *hdr, uint ref, 
			  invt_pr_ctx_t *prctx);

int
make_invdirectory( inv_oflag_t forwhat );

bool_t
init_idb( void *pred, inv_predicate_t bywhat, inv_oflag_t forwhat, 
	 inv_idbtoken_t *tok );

int
inv_getopt( int argc, char **argv, invt_pr_ctx_t *prctx);

bool_t
insert_session( invt_sessinfo_t *s);

/* To reconstruct a complete inventory from dumped inventories */
extern bool_t
inv_put_sessioninfo( invt_sessinfo_t *s );
	        

#endif
