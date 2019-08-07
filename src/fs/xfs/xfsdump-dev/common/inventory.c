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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <uuid/uuid.h>

#include "types.h"
#include "inventory_priv.h"

int sesslock_fd = -1;


/*----------------------------------------------------------------------*/
/* inventory_open                                                       */
/*                                                                      */
/* INV_BY_MOUNTPT, INV_BY_UUID or INV_BY_DEVPATH                        */
/*----------------------------------------------------------------------*/

inv_idbtoken_t
inv_open( inv_predicate_t bywhat, void *pred )
{

	int fd, stobjfd, num;
	char uuname[ INV_STRLEN ];
	inv_idbtoken_t tok = INV_TOKEN_NULL;
	invt_sescounter_t *sescnt = 0;
	
	int index = 0;
	
	assert ( pred );
	if ((fd = init_idb ( pred, bywhat, uuname, &tok )) < 0 )
		return tok;

	/* XXX also, see if it is too full. if so, make another and leave a
	   reference to the new file in the old one */

	stobjfd = get_storageobj( fd, &index );
	if ( stobjfd < 0 ) {
		close( fd );
		close( sesslock_fd );
		sesslock_fd = -1;
		return INV_TOKEN_NULL;
	}

	assert ( index > 0 );

	/* Now we need to make sure that this has enough space */
	num = GET_SESCOUNTERS( stobjfd, &sescnt );
	if ( num < 0 ) {
		close( fd );
		close( stobjfd );
		close( sesslock_fd );
		sesslock_fd = -1;
		return INV_TOKEN_NULL;
	}
	/* create another storage object ( and, an inv_index entry for it too )
	   if we've filled this one up */
	if ( (uint) num >= sescnt->ic_maxnum ) {
#ifdef INVT_DEBUG
		printf("$ creating a new storage obj & index entry. \n" );
#endif
		close (stobjfd);
		
		stobjfd = create_invindex_entry( &tok, fd, uuname, BOOL_FALSE );
		free ( sescnt );		
		if ( stobjfd < 0 ) {
			close( fd );
			close( sesslock_fd );
			sesslock_fd = -1;
			return INV_TOKEN_NULL;
		}
		return tok;
	}
	
	free ( sescnt );
	tok = get_token( fd, stobjfd );
	tok->d_invindex_off = INVINDEX_HDR_OFFSET( index - 1 );
	
	return tok;
	
}




/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/


bool_t
inv_close( inv_idbtoken_t tok )
{
	close ( tok->d_invindex_fd );
	close ( tok->d_stobj_fd );
	destroy_token( tok );
	close( sesslock_fd );
	sesslock_fd = -1;
	
	return BOOL_TRUE;
}




/*----------------------------------------------------------------------*/
/* inventory_lasttime_level_lessthan					*/
/*                                                                      */
/* Given a token that refers to a file system, and a level, this returns*/
/* the last time when a session of a lesser level was done.             */
/*                                                                      */
/* returns -1 on error.                                                 */
/*----------------------------------------------------------------------*/

bool_t
inv_lasttime_level_lessthan( 
	inv_idbtoken_t  tok,
	u_char level,
	time32_t **tm )
{
	int 	rval;
	assert ( tok != INV_TOKEN_NULL );

	rval =  search_invt( tok, level, (void **) tm,
			 (search_callback_t) tm_level_lessthan );

	return ( rval < 0) ? BOOL_FALSE: BOOL_TRUE;
}





/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

bool_t
inv_lastsession_level_lessthan( 
	inv_idbtoken_t 	tok,
	u_char		level,
	inv_session_t 	**ses )
{
	int 	rval;
	assert ( tok != INV_TOKEN_NULL );

	rval = search_invt( tok, level, (void **) ses, 
			  (search_callback_t) lastsess_level_lessthan );

	return ( rval < 0) ? BOOL_FALSE: BOOL_TRUE;

}




/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/* Return FALSE on an error, TRUE if not. If (*ses) is NULL, then the   */
/* search failed.                                                       */
/*----------------------------------------------------------------------*/


bool_t
inv_lastsession_level_equalto( 
	inv_idbtoken_t 	tok,			    
	u_char		level,
	inv_session_t	**ses )
{
	int 	rval;
	assert ( tok != INV_TOKEN_NULL );
	rval = search_invt( tok, level, (void **) ses, 
			  (search_callback_t) lastsess_level_equalto );

	return ( rval < 0) ? BOOL_FALSE: BOOL_TRUE;

}







/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

inv_sestoken_t
inv_writesession_open( 
	inv_idbtoken_t tok, 	/* token obtained by inventory_open() */
	uuid_t		*fsid,
	uuid_t		*sesid,
	char		*label,
	u_char		level,
	uint		nstreams,
	time32_t	time,
	char		*mntpt,
	char		*devpath )
{
	invt_session_t *ses;
	int		fd;
	int	rval;
	invt_sescounter_t *sescnt = NULL;
	invt_seshdr_t  	hdr;
	inv_sestoken_t	sestok;

	assert ( tok != INV_TOKEN_NULL );
	assert ( sesid && fsid && mntpt && devpath );

	if ( ! ( tok->d_update_flag & FSTAB_UPDATED ) ) {
		if ( put_fstab_entry( fsid, mntpt, devpath ) < 0 ) {
			printf ("put_fstab_entry :(\n");
			return INV_TOKEN_NULL;
		}
		tok->d_update_flag |= FSTAB_UPDATED;
	}



	ses = (invt_session_t *) calloc( 1, sizeof( invt_session_t ) );

	/* copy the session information to store */
	memcpy( &ses->s_sesid, sesid, sizeof( uuid_t ) );
	memcpy( &ses->s_fsid, fsid, sizeof( uuid_t ) );
	strcpy( ses->s_label, label );	
	strcpy( ses->s_mountpt, mntpt );	
	strcpy( ses->s_devpath, devpath );	
	ses->s_max_nstreams = nstreams;


	/* s_curstream_off will be set in create_session() */

	fd = tok->d_stobj_fd;
	
	assert ( fd > 0 );

	hdr.sh_time = time;
	hdr.sh_level = level;	
	/* sh_streams_off and sh_sess_off will be set in create_session() */
	
	sestok = get_sesstoken( tok );

	/* we need to put the new session in the appropriate place in 
	   storage object. So first find out howmany sessions are there */

	INVLOCK( fd, LOCK_EX );
	if ( GET_SESCOUNTERS( fd, &sescnt) < 0 ) {
		free ( ses );
		free ( sestok );
		INVLOCK( fd, LOCK_UN );
		return INV_TOKEN_NULL;
	}

	/* create the writesession, and get ready for the streams to come 
	   afterwards */
	rval = create_session( sestok, fd, sescnt, ses, &hdr );
	assert (rval > 0);


	INVLOCK( fd, LOCK_UN );
 
	sestok->sd_sesstime = time;

	if ( tok->d_update_flag & NEW_INVINDEX ) {
		if ( put_sesstime( sestok, INVT_STARTTIME ) < 0 ) {
			printf ("put_starttime :(\n");
			return INV_TOKEN_NULL;
		}
		tok->d_update_flag &= ~(NEW_INVINDEX);
	}

	free ( ses );
	free ( sescnt );


	return ( rval < 0 )? INV_TOKEN_NULL: sestok;
}





/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/


bool_t
inv_writesession_close( inv_sestoken_t tok )
{
	int		rval;
	
	assert ( tok != INV_TOKEN_NULL );

	/* now update end_time in the inv index header */
	rval = put_sesstime( tok, INVT_ENDTIME );

	memset( tok, 0, sizeof( invt_sesdesc_entry_t ) );

	free ( tok );

	return ( rval < 0 ) ? BOOL_FALSE: BOOL_TRUE;

}



/*----------------------------------------------------------------------*/
/* inventory_stream_open                                                */
/*                                                                      */
/* Opens a stream for mediafiles to be put in.                          */
/*----------------------------------------------------------------------*/
inv_stmtoken_t
inv_stream_open(
	inv_sestoken_t tok )
{
	inv_stmtoken_t stok;
	invt_stream_t  stream;
	invt_session_t ses;
	invt_seshdr_t  seshdr;
	int fd;

	assert ( tok != INV_TOKEN_NULL );
	 
	stream.st_nmediafiles = 0;
	stream.st_interrupted = BOOL_FALSE;

	
	/* XXX yukk... make the token descriptors not pointers */
	stok = ( inv_stmtoken_t ) malloc( sizeof( invt_strdesc_entry_t ) );
	
	stok->md_sesstok = tok;
	stok->md_lastmfile = 0;
	
	/* get the session to find out where the stream is going to go */
	fd = tok->sd_invtok->d_stobj_fd; 

	sess_lock();
	
	INVLOCK( fd, LOCK_SH );
	/* get the session header first */
	if ( GET_REC_NOLOCK( fd, &seshdr, sizeof( invt_seshdr_t ),
			     tok->sd_sesshdr_off ) <= 0 ) {
		free ( stok );
		INVLOCK( fd, LOCK_UN );
		sess_unlock();
		return INV_TOKEN_NULL;
	}



	/* XXX Have one func that gives both seshdr and session */
	if ( GET_REC_NOLOCK( fd, &ses, sizeof( invt_session_t ),
			     tok->sd_session_off ) <= 0 ) {
		free ( stok );
		INVLOCK( fd, LOCK_UN );
		sess_unlock();
		return INV_TOKEN_NULL;
	}
	INVLOCK( fd, LOCK_UN );

	if ( ses.s_cur_nstreams < ses.s_max_nstreams ) {
		/* this is where this stream header will be written to */
		stok->md_stream_off = (off64_t) (sizeof( invt_stream_t ) * 
					         ses.s_cur_nstreams )
			                         + seshdr.sh_streams_off;
		ses.s_cur_nstreams++;
				
		/* write it back. this locks and unlocks fd EXclusively */
		if ( PUT_REC( fd, &ses, sizeof( ses ), 
			      tok->sd_session_off ) < 0 ) {
			free ( stok );
			sess_unlock();
			return INV_TOKEN_NULL;
		}
	} else {
		fprintf(stderr, "Cant create more than %d streams. Max'd out..\n",
			ses.s_cur_nstreams );
		free ( stok );
		sess_unlock();
		return INV_TOKEN_NULL;
	}
	sess_unlock();

	stream.st_firstmfile = stream.st_lastmfile = stok->md_stream_off;
	
	/* now put the stream header on to the disk */
	if ( PUT_REC( fd, &stream, sizeof( invt_stream_t ),
		      stok->md_stream_off ) < 0 ) {
		free ( stok );
		return INV_TOKEN_NULL;
	}

	return stok;
}




/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

bool_t
inv_stream_close(
		inv_stmtoken_t	tok,
		bool_t wasinterrupted )
{
	invt_stream_t strm;
	int fd = tok->md_sesstok->sd_invtok->d_stobj_fd;
	int rval;
	bool_t dowrite = BOOL_FALSE;

	INVLOCK( fd, LOCK_EX );
	if ((rval = GET_REC_NOLOCK( fd, &strm, sizeof( invt_stream_t ), 
			       tok->md_stream_off )) < 0 )
		goto end;   /* eek :-)   */
	
	if ( strm.st_interrupted != wasinterrupted ) {
		strm.st_interrupted = wasinterrupted;
		dowrite = BOOL_TRUE;
	}

	/* get the last media file to figure out what our last ino was. 
	   we have a pointer to that in the stream token */
	if ( tok->md_lastmfile ){
		if ( strm.st_endino.ino != tok->md_lastmfile->mf_endino.ino ||
		     strm.st_endino.offset != tok->md_lastmfile->mf_endino.offset){
			printf("Warning: endinos dont match ! \n");
			dowrite = BOOL_TRUE;
			strm.st_endino = tok->md_lastmfile->mf_endino;
		}
	}
			
	if (dowrite) {
		rval = PUT_REC_NOLOCK(fd, &strm, sizeof(invt_stream_t),
				      tok->md_stream_off);
	}
 end:
	INVLOCK( fd, LOCK_UN );

	free ( tok->md_lastmfile );
	memset( tok, 0, sizeof( invt_strdesc_entry_t ) );
	free ( tok );

	return ( rval < 0 ) ? BOOL_FALSE: BOOL_TRUE;
}
 



/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

bool_t
inv_put_mediafile( 
	inv_stmtoken_t 	tok, 
	uuid_t 		*moid, 
	char 		*label,
	xfs_ino_t	startino,
	off64_t		startino_offset,
	xfs_ino_t	endino,
	off64_t		endino_offset )
{
	invt_mediafile_t *mf;
	int 		 rval;


	assert ( tok != INV_TOKEN_NULL );
	assert ( tok->md_sesstok->sd_invtok->d_update_flag & FSTAB_UPDATED );
	assert ( tok->md_sesstok->sd_invtok->d_stobj_fd >= 0 );

	mf = (invt_mediafile_t *) calloc( 1, sizeof( invt_mediafile_t ) );
	
	/* copy the media file information */
	memcpy( &mf->mf_moid, moid, sizeof( uuid_t ) );
	strcpy( mf->mf_label, label );	
	mf->mf_startino.ino = startino;
	mf->mf_startino.offset = startino_offset;
	mf->mf_endino.ino = endino;
	mf->mf_endino.offset = endino_offset;
	
	INVLOCK( tok->md_sesstok->sd_invtok->d_stobj_fd, LOCK_EX );
	rval = put_mediafile( tok, mf );
	INVLOCK( tok->md_sesstok->sd_invtok->d_stobj_fd, LOCK_UN );

	/* we dont free the mfile here. we always keep the last mfile
	   around, inside the inv_stmtoken, and when we add a new mfile,
	   we free the previous one. The last one is freed in stream_close()
	   */

	return ( rval < 0 ) ? BOOL_FALSE: BOOL_TRUE;

}



/*----------------------------------------------------------------------*/
/* inv_get_inolist                                                      */
/*                                                                      */
/* This returns all the database files that belong to the inventory, so */
/* that the client (dump) knows that these shouldn't be dumped alongwith*/
/* regular files.                                                       */
/*                                                                      */
/* foreach ( fs in fstab )                                              */
/* 	foreach ( index in InvIndex )                                 */
/*          get                                                         */
/*----------------------------------------------------------------------*/

int
inv_get_inolist(
	inv_inolist_t 	**inolist )
{
	invt_entry_t	*iarr = NULL;
	invt_counter_t	*icnt = NULL;
	int	     	nindices, i;
	struct stat64	statbuf;	
	inv_inolist_t 	*curitem;

#ifdef NOTDEF	
	*inolist = NULL;
	curitem = malloc( sizeof( inv_inolist_t ) );
	
	/* get the array of all indices in the invindex */
	if ( ( nindices = GET_ALLHDRS_N_CNTS( invfd, (void **)&iarr, 
					      (void **)&icnt, 
					      sizeof( invt_entry_t ),
					      sizeof( invt_counter_t )) 
	      ) <= 0 ) {
		return -1;
	}
	free( icnt );
	
	/* attach all the StObjs */
	for (i = nindices - 1; i >= 0; i--) {
		if ( stat64( iarr[i].ie_filename, &statbuf ) < 0 ) {
			perror( iarr[i].ie_filename );
			return -1;
		}
		
		create_inolist_item( curitem, statbuf.st_ino );
	}
	/* The inventory index */
	if ( fstat64( invfd, &statbuf ) ){
		perror( "InvIndex file" );
		return -1;
	}
	create_inolist_item( curitem, statbuf.st_ino );

	/* fstab */
	if ( stat64( INV_FSTAB, &statbuf ) < 0 ) {
			perror( INV_FSTAB );
			return -1;
		}
	create_inolist_item( curitem, statbuf.st_ino );
	
	/* sesslock file */
	if ( stat64( SESSLOCK_FILE, &statbuf ) < 0 ) {
			perror( SESSLOCK_FILE );
			return -1;
		}
	create_inolist_item( curitem, statbuf.st_ino );
#endif

	return 1;
		
}





/*----------------------------------------------------------------------*/
/* inv_get_session                                                      */
/*                                                                      */
/* This is to be called after a write-session is complete, but before it*/
/* is closed. ie. the token must still be valid, and all the streams    */
/* and their mediafiles put in the inventory.                           */
/*                                                                      */
/* On return, the buffer will be filled with all the data pertinent to  */
/* the session referred to by the session token. The application of this*/
/* is to dump the inventory of a session to a media object.             */
/*----------------------------------------------------------------------*/

bool_t
inv_get_session(
	inv_sestoken_t		tok,
	void		      **bufpp,	/* buf to fill */
	size_t		       *bufszp )/* size of that buffer */
{
	assert( tok != INV_TOKEN_NULL );
	assert( tok->sd_invtok );

	/* First get the session header, and the session information. Then
	   we can figure out how much space to allocate */
	
}




#ifdef DEBUG

/* This prints out all the sessions of a filesystem that are in the inventory */
bool_t
inv_DEBUG_printallsessions( 
	inv_idbtoken_t 	tok,			    
	inv_session_t	**ses )
{
	int 	rval;
	rval = search_invt( tok, 0, (void **) ses, 
			  (search_callback_t) DEBUG_displayallsessions );

	return ( rval < 0) ? BOOL_FALSE: BOOL_TRUE;

}

#endif
