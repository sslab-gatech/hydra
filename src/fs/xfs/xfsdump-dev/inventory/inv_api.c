/*
 * Copyright (c) 2000-2002 Silicon Graphics, Inc.
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

#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>

#include "config.h"

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"
#include "getopt.h"


/*----------------------------------------------------------------------*/
/* inventory_open                                                       */
/*                                                                      */
/* INV_BY_MOUNTPT, INV_BY_UUID or INV_BY_DEVPATH                        */
/*----------------------------------------------------------------------*/

inv_idbtoken_t
inv_open( inv_predicate_t bywhat, inv_oflag_t forwhat, void *pred )
{
	int fd, stobjfd, num, retval;
	inv_idbtoken_t tok = INV_TOKEN_NULL;
	invt_sescounter_t *sescnt = 0;

	int index = 0;
	
	assert ( pred );
	fd = retval = init_idb ( pred, bywhat, forwhat, &tok );

	if ( retval == I_DONE ) 
		return tok;
	
	/* if we just want to search the db, all we need is the invidx.
	   at this point, we know that a tok wasnt created in init_idb() */
	if ( forwhat == INV_SEARCH_ONLY ) {
		/* fd == I_EMPTYINV or fd == valid fd */
		tok = get_token( fd, -1);
		tok->d_oflag = forwhat;
		return tok;
	}

	/* XXX also, see if it is too full. if so, make another and leave a
	   reference to the new file in the old one */

	stobjfd = idx_get_stobj( fd, forwhat, &index );
	if ( stobjfd < 0 ) {
		close( fd );
		return INV_TOKEN_NULL;
	}

	assert ( index > 0 );

	/* Now we need to make sure that this has enough space */
	INVLOCK( stobjfd, LOCK_SH );
	
	num = GET_SESCOUNTERS( stobjfd, &sescnt );
	if ( num < 0 ) {
		close( fd );
		INVLOCK( stobjfd, LOCK_UN );
		close( stobjfd );
		return INV_TOKEN_NULL;
	}

	/* create another storage object ( and, an inv_index entry for it 
	   too ) if we've filled this one up */

	if ( (uint) num >= sescnt->ic_maxnum ) {
		mlog( MLOG_DEBUG | MLOG_INV, "$ INV: creating a new storage obj & "
		      "index entry. \n" );
		INVLOCK( stobjfd, LOCK_UN );
		close (stobjfd);

		INVLOCK( fd, LOCK_EX );
		stobjfd = idx_create_entry( &tok, fd, BOOL_FALSE );
		INVLOCK( fd, LOCK_UN );

		free ( sescnt );		
		if ( stobjfd < 0 ) {
			close( fd );
			return INV_TOKEN_NULL;
		}
		return tok;
	}
	
	INVLOCK( stobjfd, LOCK_UN );

	free ( sescnt );
	tok = get_token( fd, stobjfd );
	tok->d_invindex_off = IDX_HDR_OFFSET( index - 1 );
	tok->d_oflag = forwhat;
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
	if ( tok->d_stobj_fd >= 0 ) 
		close ( tok->d_stobj_fd );
	destroy_token( tok );
	return BOOL_TRUE;
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
	bool_t		ispartial,
	bool_t		isresumed,
	u_char		level,
	uint		nstreams,
	time32_t	time,
	char		*mntpt,
	char		*devpath )
{
	invt_session_t  ses;
	int		fd;
	int	rval;
	invt_sescounter_t *sescnt = NULL;
	invt_seshdr_t  	hdr;
	inv_sestoken_t	sestok;
	inv_oflag_t     forwhat;

	assert ( tok != INV_TOKEN_NULL );
	assert ( sesid && fsid && mntpt && devpath );
	forwhat = tok->d_oflag;
	fd = tok->d_stobj_fd;
	assert ( forwhat != INV_SEARCH_ONLY );
	assert ( fd > 0 );

	if ( ! ( tok->d_update_flag & FSTAB_UPDATED ) ) {
		if ( fstab_put_entry( fsid, mntpt, devpath, forwhat ) < 0 ) {
		       mlog( MLOG_NORMAL | MLOG_INV, _(
				"INV: put_fstab_entry failed.\n") );
		       return INV_TOKEN_NULL;
		}
		tok->d_update_flag |= FSTAB_UPDATED;
	}


	/* copy the session information to store */
	memset( (void *)&ses, 0, sizeof( ses ) );	/* paranoia */
	memcpy( &ses.s_sesid, sesid, sizeof( uuid_t ) );
	memcpy( &ses.s_fsid, fsid, sizeof( uuid_t ) );
	strcpy( ses.s_label, label );	
	strcpy( ses.s_mountpt, mntpt );	
	strcpy( ses.s_devpath, devpath );	
	ses.s_max_nstreams = nstreams;

        hdr.sh_pruned = 0; /* session is not pruned by invutil */
	hdr.sh_time = time;
	hdr.sh_level = level;	
	hdr.sh_flag = (ispartial) ? INVT_PARTIAL: 0;
	hdr.sh_flag |= (isresumed) ? INVT_RESUMED : 0;
	/* sh_streams_off and sh_sess_off will be set in create_session() */
	
	sestok = get_sesstoken( tok );

	/* we need to put the new session in the appropriate place in 
	   storage object. So first find out howmany sessions are there */

	INVLOCK( fd, LOCK_EX );
	if ( GET_SESCOUNTERS( fd, &sescnt) < 0 ) {
		free ( sestok );
		INVLOCK( fd, LOCK_UN );
		return INV_TOKEN_NULL;
	}

	/* create the writesession, and get ready for the streams to come 
	   afterwards */
	rval = stobj_create_session( sestok, fd, sescnt, &ses, &hdr );
	assert (rval > 0);


	INVLOCK( fd, LOCK_UN );
 
	sestok->sd_sesstime = time;

	if ( tok->d_update_flag & NEW_INVINDEX ) {
		if ( idx_put_sesstime( sestok, INVT_STARTTIME ) < 0 ) {
			mlog( MLOG_NORMAL | MLOG_INV, _(
				"INV: put_starttime failed.\n") );
			return INV_TOKEN_NULL;
		}
		tok->d_update_flag &= ~(NEW_INVINDEX);
	}

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
	rval = idx_put_sesstime( tok, INVT_ENDTIME );

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
	inv_sestoken_t tok,
	char		*cmdarg )
{
	inv_stmtoken_t stok;
	invt_stream_t  stream;
	invt_session_t ses;
	invt_seshdr_t  seshdr;
	int fd;
	bool_t err = BOOL_FALSE;

	assert ( tok != INV_TOKEN_NULL );
	 
	/* this memset is needed as a dump interrupted/crashed very soon
	 * after starting results in an inventory with exteremely large
	 * starting/ending inodes or offsets. This can be misleading.
	 * See bug #463702 for an example.
	 */
	memset( (void *)&stream, 0 , sizeof(invt_stream_t) );

	stream.st_nmediafiles = 0;
	stream.st_interrupted = BOOL_TRUE; /* fix for 353197 */
	strcpy( stream.st_cmdarg, cmdarg );

	/* XXX yukk... make the token descriptors not pointers */
	stok = ( inv_stmtoken_t ) malloc( sizeof( invt_strdesc_entry_t ) );
	
	stok->md_sesstok = tok;
	stok->md_lastmfile = 0;
	
	/* get the session to find out where the stream is going to go */
	fd = tok->sd_invtok->d_stobj_fd; 

	INVLOCK( fd, LOCK_EX );

	/* get the session header and the session */
	if ( stobj_get_sessinfo( tok, &seshdr, &ses ) <= 0 ) 
		err = BOOL_TRUE;

	if ( ( ! err )  && ses.s_cur_nstreams < ses.s_max_nstreams ) {
		/* this is where this stream header will be written to */
		stok->md_stream_off = (off64_t) (sizeof( invt_stream_t ) * 
					         ses.s_cur_nstreams )
			                         + seshdr.sh_streams_off;
		ses.s_cur_nstreams++;
				
		/* write it back. */
		if ( PUT_REC_NOLOCK( fd, &ses, sizeof( ses ), 
				     tok->sd_session_off ) < 0 ) 
			err = BOOL_TRUE;
	} else if ( ! err ) {
		mlog ( MLOG_NORMAL, _(
		       "INV: cant create more than %d streams."
		       " Max'd out..\n"), ses.s_cur_nstreams );
		err = BOOL_TRUE;
	}

	if ( ! err ) { 
		stream.st_firstmfile = stream.st_lastmfile = 
			               stok->md_stream_off;
	
		/* now write the stream header on to the disk */
		if ( PUT_REC_NOLOCK( fd, &stream, sizeof( invt_stream_t ),
				    stok->md_stream_off ) > 0 ) {
			/* we're all set */
			INVLOCK( fd, LOCK_UN );
			return stok;
		}
	}

	/* error occured somewhere */
	free ( stok );
	INVLOCK( fd, LOCK_UN );
	return INV_TOKEN_NULL;
	
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
	
	rval = idx_put_sesstime( tok->md_sesstok, INVT_ENDTIME );
	if (rval < 0)
		mlog( MLOG_NORMAL | MLOG_INV, _(
		      "INV: idx_put_sesstime failed in "
		      "inv_stream_close() \n") );
	INVLOCK( fd, LOCK_EX );
	if ((rval = GET_REC_NOLOCK( fd, &strm, sizeof( invt_stream_t ), 
			       tok->md_stream_off )) > 0 ) {
	
		if ( strm.st_interrupted != wasinterrupted ) {
			strm.st_interrupted = wasinterrupted;
			dowrite = BOOL_TRUE;
		}

		/* get the last media file to figure out what our last 
		   ino was. we have a pointer to that in the stream token */
		if ( tok->md_lastmfile ){
			if ( strm.st_endino.ino != 
			      tok->md_lastmfile->mf_endino.ino ||
			     strm.st_endino.offset !=
			      tok->md_lastmfile->mf_endino.offset) {

			      mlog( MLOG_DEBUG | MLOG_INV, "INV: stream_close() "
				    " - endinos dont match ! \n");
			      dowrite = BOOL_TRUE;
			      strm.st_endino = tok->md_lastmfile->mf_endino;
			}
		}
			
		if (dowrite) {
			rval = PUT_REC_NOLOCK(fd, &strm, sizeof(invt_stream_t),
					      tok->md_stream_off);
		}
	}

	INVLOCK( fd, LOCK_UN );

	if ( tok->md_lastmfile ) {
		free ( tok->md_lastmfile );
	}
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
	uint		mfileindex,
	xfs_ino_t	startino,
	off64_t		startino_offset,
	xfs_ino_t	endino,
	off64_t		endino_offset,
	off64_t		size,
	bool_t          isgood,
	bool_t		isinvdump)
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
	mf->mf_mfileidx = mfileindex;
	mf->mf_startino.ino = startino;
	mf->mf_startino.offset = startino_offset;
	mf->mf_endino.ino = endino;
	mf->mf_endino.offset = endino_offset;
	mf->mf_size = size;
	mf->mf_flag = 0;
	if ( isgood ) 
		mf->mf_flag |= INVT_MFILE_GOOD;

	/* This flag is used to indicate the media file that contains the
	   dump of the sessioninfo structure that contains all but this
	   media file */
	if ( isinvdump )
		mf->mf_flag |= INVT_MFILE_INVDUMP;
	
	INVLOCK( tok->md_sesstok->sd_invtok->d_stobj_fd, LOCK_EX );
	rval = stobj_put_mediafile( tok, mf );
	INVLOCK( tok->md_sesstok->sd_invtok->d_stobj_fd, LOCK_UN );

	/* we dont free the mfile here. we always keep the last mfile
	   around, inside the inv_stmtoken, and when we add a new mfile,
	   we free the previous one. The last one is freed in stream_close()
	   */

	return ( rval < 0 ) ? BOOL_FALSE: BOOL_TRUE;

}

 



/*----------------------------------------------------------------------*/
/* inv_get_sessioninfo                                                  */
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
inv_get_sessioninfo(
	inv_sestoken_t		tok,
	void		      **bufpp,	/* buf to fill */
	size_t		       *bufszp )/* size of that buffer */
{
	invt_session_t 	ses;
	invt_seshdr_t	hdr;
	bool_t          rval;
	int		fd;


	assert( tok != INV_TOKEN_NULL );
	assert( tok->sd_invtok );
	*bufpp = NULL;
	*bufszp = 0;
	fd = tok->sd_invtok->d_stobj_fd;

	INVLOCK( fd, LOCK_SH );

	/* Next we get the session header, and the session information. Then
	   we can figure out how much space to allocate */
	if ( stobj_get_sessinfo( tok, &hdr, &ses ) <= 0 ) {
		INVLOCK( fd, LOCK_UN );
		return BOOL_FALSE;
	}

	rval = stobj_pack_sessinfo( fd, &ses, &hdr, bufpp, bufszp );
	INVLOCK( fd, LOCK_UN );


	return rval;
}




/*----------------------------------------------------------------------*/
/* inv_put_sessioninfo                                                  */
/*                                                                      */
/* This is used in reconstructing an inventory from dumped media objects*/
/*                                                                      */
/* Most importantly, note that this is not in anyway an alternative to  */
/* inv_open_writesession() - that is called while the dump is in progr- */
/* ess. This is really inv_put_dumpedsession(); We just didn't want to  */
/* be *that* dump-specific :)                                           */
/*----------------------------------------------------------------------*/

bool_t
inv_put_sessioninfo( invt_sessinfo_t *s )
{
	static bool_t invdir_ok = BOOL_FALSE;
	
	if ( !invdir_ok ) {
		if ( make_invdirectory( INV_SEARCH_N_MOD ) < 0 )
			return BOOL_FALSE;
		else
			invdir_ok = BOOL_TRUE;
	} 

      	return insert_session( s );

}



		   
/*----------------------------------------------------------------------*/
/* inv_free_session							*/
/* 									*/ 
/* free the inv_session structure allocated as a result of calls to     */
/* inv_get_session_byuuid, etc.						*/
/*----------------------------------------------------------------------*/
void
inv_free_session(
	inv_session_t **ses)
{
	uint i;
	
	assert(ses);
	assert(*ses);

	for ( i = 0; i < (*ses)->s_nstreams; i++ ) {
		/* the array of mediafiles is contiguous */
		free ((*ses)->s_streams[i].st_mediafiles);
	}
	
	/* all streams are contiguous too */
	free ((*ses)->s_streams);
      
	free (*ses);
	*ses = NULL;
}


/*----------------------------------------------------------------------*/
/* inv_lasttime_level_lessthan						*/
/*									*/
/* Given a file system uuid, token that refers to a file system, and a	*/
/* level, tm is populated with last time when a session of a lesser	*/
/* level was done.							*/
/*									*/
/* Returns TRUE on success.						*/
/*----------------------------------------------------------------------*/

bool_t
inv_lasttime_level_lessthan( 
	uuid_t		*fsidp,
	inv_idbtoken_t	tok,
	u_char		level,
	time32_t	**tm)
{
	int 	rval;
	if ( tok != INV_TOKEN_NULL ) {
		rval =  search_invt(fsidp, tok->d_invindex_fd, &level,
				    (void **)tm,
				    (search_callback_t)tm_level_lessthan);

		return ( rval < 0) ? BOOL_FALSE: BOOL_TRUE;
	}
	
	return invmgr_query_all_sessions(fsidp,		 /* fs uuid ptr */
					 (void *)&level, /* in */
					 (void **)tm,	 /* out */
			       (search_callback_t)tm_level_lessthan);
}

/*----------------------------------------------------------------------*/
/* inv_lastsession_level_lessthan					*/
/*									*/
/* Given a file system uuid, token that refers to a file system, and a	*/
/* level, ses is populated with a session of lesser than the level	*/
/* passed in.								*/
/*									*/
/* Returns FALSE on an error, TRUE if not. If (*ses) is NULL, then the	*/
/* search failed.                                                       */
/*----------------------------------------------------------------------*/

bool_t
inv_lastsession_level_lessthan( 
	uuid_t		*fsidp,
	inv_idbtoken_t	tok,
	u_char		level,
	inv_session_t	**ses)
{
	int 	rval;
	if ( tok != INV_TOKEN_NULL ) {
		rval = search_invt(fsidp, tok->d_invindex_fd, &level,
				   (void **)ses,
				   (search_callback_t)lastsess_level_lessthan);

		return ( rval < 0) ? BOOL_FALSE: BOOL_TRUE;
	}

	return invmgr_query_all_sessions(fsidp,		 /* fs uuid */
					 (void *)&level, /* in */
					 (void **)ses,	 /* out */
			       (search_callback_t)lastsess_level_lessthan);

}

/*----------------------------------------------------------------------*/
/* inv_lastsession_level_equalto					*/
/*									*/
/* Given a file system uuid, token that refers to a file system, and a	*/
/* level, this populates ses with last time when a session of a lesser	*/
/* level was done.							*/
/*									*/
/* Return FALSE on an error, TRUE if not. If (*ses) is NULL, then the   */
/* search failed.                                                       */
/*----------------------------------------------------------------------*/


bool_t
inv_lastsession_level_equalto( 
	uuid_t		*fsidp,
	inv_idbtoken_t	tok,
	u_char		level,
	inv_session_t	**ses )
{
	int 	rval;
	if ( tok != INV_TOKEN_NULL ) {
		rval = search_invt(fsidp, tok->d_invindex_fd, &level,
				   (void **)ses,
				   (search_callback_t)lastsess_level_equalto);

		return ( rval < 0) ? BOOL_FALSE: BOOL_TRUE;
	}
	
	return invmgr_query_all_sessions(fsidp,		 /* fs uuid */
					 (void *)&level, /* in */
					 (void **)ses,	 /* out */
			       (search_callback_t)lastsess_level_equalto);

}


/*----------------------------------------------------------------------*/
/* inv_getsession_byuuid                                                */
/*                                                                      */
/* Given a file system uuid and a session uuid , ses is populated with	*/
/* the session that contains the matching system uuid.			*/
/*									*/
/* Returns FALSE on an error, TRUE if the session was found.		*/
/*----------------------------------------------------------------------*/

bool_t
inv_get_session_byuuid(
	uuid_t		*fsidp,
	uuid_t		*sesid,
	inv_session_t	**ses)
{

	return invmgr_query_all_sessions(fsidp,		/* fs uuid */
					 (void *)sesid, /* in */
					 (void **)ses,	/* out */
			       (search_callback_t)stobj_getsession_byuuid);
}

/*----------------------------------------------------------------------*/
/* inv_getsession_bylabel						*/
/*                                                                      */
/* Given a file system uuid and a session uuid, ses is populated with	*/
/* the session that contains the matching system label.			*/
/*									*/
/* Returns FALSE on an error, TRUE if the session was found.		*/
/*----------------------------------------------------------------------*/

bool_t
inv_get_session_bylabel(
	uuid_t		*fsidp,
	char		*session_label,
	inv_session_t	**ses)
{

	return invmgr_query_all_sessions(fsidp,			/* fs uuid */
					 (void *)session_label, /* in */
					 (void **)ses,		/* out */
			       (search_callback_t)stobj_getsession_bylabel);
}


/*----------------------------------------------------------------------*/
/* inv_delete_mediaobj                                                  */
/*                                                                      */
/* We delete all the media files that are stored in the given mobj, and */
/* delete them one by one. We delete the session information only when  */
/* all its mediafiles are deleted. This is because a single session can */
/* be striped across multiple mediaobjects.                             */
/*----------------------------------------------------------------------*/

bool_t
inv_delete_mediaobj( uuid_t *moid )
{
	inv_oflag_t forwhat = INV_SEARCH_N_MOD;

	/* forall fsids (fs) in fstab {
	       open invindex table (t)
	       forall indices (i) in t {
	          open stobj (st)
		  forall sessions (s) in st {
		      forall streams (strm) in s {
		         forall mediafiles (m) in strm {
			     if (m.mediaobj == moid) {
			     // delete m
			     if ( --strm.nmediafiles == 0 )
			        if ( --s.nstreams == 0 )
			            delete-session (s)
			     }
			 } 
		      }
		 }
	      }
	*/
	
	invt_counter_t *cnt;
	invt_fstab_t *arr;
	int numfs, i, fd, invfd;
	char fname[INV_STRLEN];

	fd = fstab_getall( &arr, &cnt, &numfs, forwhat );
	if ( fd < 0 || numfs <= 0 ) {
		mlog( MLOG_NORMAL | MLOG_INV, _("INV: Error in fstab\n") );
		return BOOL_FALSE;
	}
	
	close( fd );

	for ( i = 0; i < numfs; i++) {
		if ( fstab_get_fname( &arr[i].ft_uuid, 
				      fname, 
				      (inv_predicate_t)INV_BY_UUID,
				      forwhat
				     )
		     < 0 ) {
			mlog( MLOG_NORMAL | MLOG_INV, _(
			      "INV: Cant get inv-name for uuid\n") );
			return BOOL_FALSE;
		}
		strcat( fname, INV_INVINDEX_PREFIX );
		invfd = open( fname, INV_OFLAG(forwhat));
		if ( invfd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_INV, _(
			     "INV: Open failed on %s\n"), 
			     fname );
			return BOOL_FALSE;
		}

		if (search_invt(&arr[i].ft_uuid, invfd, NULL, (void **)&moid,
				(search_callback_t)stobj_delete_mobj)
		    < 0 )
			return BOOL_FALSE;
		/* we have to delete the session, etc */
		close( invfd );	
	}
	
	return BOOL_TRUE;
}



#define I_IFOUND	0x01
#define I_IDONE		0x02
#define I_IERR		0x04


static const char *myopts[] = { 
#define OPT_MNT		0
	"mnt", 

#define OPT_FSID	1
	"fsid", 

#define OPT_DEV		2
	"dev", 

#define OPT_DEPTH	3
	"depth", 

#define OPT_MOBJID	4
	"mobjid",

#define OPT_MOBJLABEL	5
	"mobjlabel", 

#define OPT_FSTAB	6
	"fstab",

#define OPT_INVIDX	7
	"invidx",

#define OPT_INVCHECK	8
	"check",

#define OPT_INVLEVEL	9
	"level",

	NULL
};


int
inv_getopt(int argc, char **argv, invt_pr_ctx_t *prctx) 
{
	int rval = 0;
	void *fs = 0;
	char *options, *value;
	extern char *optarg;
	extern int optind, opterr;
	
	inv_predicate_t bywhat = -1;
	int c, d;
	uuid_t fsid;
	int npreds = 0;
	int npreds2 = 0;
	char invoptstring[128], *rptr, *wptr;
	optind = 1;
	opterr = 0;     
	
	/* 
	 * getopt doesn't handle both '-I' and '-I w/subopts' so...
	 * First set I_IFOUND if -I is set at all (with or without 
	 * any suboptions).  Do this by taking out the ':' so getopt
	 * accepts it.  Later, reset opts and go through again to 
	 * pick off any subopts.
	 */
	strcpy(invoptstring, GETOPT_CMDSTRING);
	wptr = strchr(invoptstring, 'I');
	if (wptr != NULL) {
		wptr++;
		rptr = wptr + 1;
		while (*wptr != '\0')
			*wptr++ = *rptr++;
		while ( ( c = getopt( argc, argv, invoptstring)) != EOF ) {
			switch ( c ) {
			case GETOPT_INVPRINT:
				prctx->depth = 0;
				rval |= I_IFOUND ;
				break;
			}
		}
		optind = 1;
		opterr = 0;
		optarg = NULL;
	}

	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
		case GETOPT_INVPRINT:
			rval |= I_IFOUND ;
			if ((options = optarg) == NULL) 
				break;

			while (*options != '\0') {
				d = getsubopt(&options,(constpp)myopts,&value);
				if (value == NULL && d != OPT_FSTAB &&
				     d != OPT_INVIDX && d != OPT_INVCHECK)
					continue;
				switch( d ) {
					/* process mntpt option */
				      case OPT_MNT: 
					bywhat = (inv_predicate_t) INV_BY_MOUNTPT;
					fs = value;
					npreds++;
					break;
					
					/* process fsid option */
				      case OPT_FSID: 
					bywhat = (inv_predicate_t) INV_BY_UUID;
					npreds++;
					
					uuid_parse(value, fsid);
					fs = fsid;
					break;

				      case OPT_DEV: /* process dev option */
					bywhat = (inv_predicate_t) INV_BY_DEVPATH;
					fs = value;   
					npreds++;
					break;

				      case OPT_DEPTH:
					prctx->depth = atoi(value);   
					if (prctx->depth < 0 || 
					    prctx->depth > 4 )
						prctx->depth = 0;
					break;
					
				      case OPT_MOBJID:
					{
					uuid_t *u;
					u = malloc ( sizeof( uuid_t ) );
					uuid_parse(value, *u);
					prctx->mobj.type = INVT_MOID;
					prctx->mobj.value = (void *)u;
				        }
					npreds2++;
					break;

				      case OPT_MOBJLABEL:
					prctx->mobj.type = INVT_LABEL;
					prctx->mobj.value = (void *)value;
					npreds2++;
					break;
						
				      case OPT_FSTAB:
					prctx->fstab = BOOL_TRUE;
					break;

				      case OPT_INVIDX:
					prctx->invidx = BOOL_TRUE;
					break;

				      case OPT_INVCHECK:
					prctx->invcheck = BOOL_TRUE;
					break;

				      case OPT_INVLEVEL:
					prctx->level = atoi(value);
					break;

				      default:
					if ( strlen(value) == 1 &&
					     atoi(value) < PR_MAXDEPTH )
						prctx->depth = atoi(value);
					else {
						mlog( MLOG_NORMAL | MLOG_INV, _(
					       "INV: invalid sub-option %s"
					       " for -I option\n"), value );
						rval |= I_IERR;
					}
					break;
				}
			}
			break; /* case GETOPT_INVPRINT */
		}

	}
	
	if (npreds > 1) {
		mlog( MLOG_NORMAL | MLOG_INV, _(
		     "INV: Only one of mnt=,dev= and fsid=value can be used.\n")
		     );
		rval |= I_IERR;
	}
	else if (npreds2 > 1) {
		mlog( MLOG_NORMAL | MLOG_INV, _(
		     "INV: Only one of mobjid= and mobjlabel= can be used.\n")
		     );
		rval |= I_IERR;
	}
	else if ( (rval & I_IFOUND) && !(rval & I_IERR) && fs 
		 && ! prctx->fstab && ! prctx->invcheck) {
		inv_idbtoken_t tok;

		/* A filesystem could be backed up, mkfs'ed then restored
		 * to a new UUID value.  Therefore, we can't simply stop
		 * when we find the first matching mount point (pv564234).
		 * This code loops through all filesystems and does the 
		 * comparison by hand. */
		if (bywhat == INV_BY_MOUNTPT) {
			int fd, numfs, i;
			invt_fstab_t *arr = NULL;
			invt_counter_t *cnt = NULL;
			inv_oflag_t forwhat = INV_SEARCH_ONLY;

			fd = fstab_getall( &arr, &cnt, &numfs, forwhat );
			free( cnt );

			rval |= I_IERR; /* Cleared if successful */

			if ( fd >= 0 ) {
				for ( i = 0; i < numfs; i++ ) {
					tok = inv_open( 
						(inv_predicate_t )INV_BY_UUID,
						INV_SEARCH_ONLY,
						&arr[i].ft_uuid );
					if ( tok == INV_TOKEN_NULL )
						break;
					if ( STREQL( arr[i].ft_mountpt, fs) ) {
						prctx->index = i;
						invmgr_inv_print( 
						          tok->d_invindex_fd,
							  prctx );
						rval &= ~(I_IERR);
					}
					inv_close( tok );
				}
				free ( arr );
				rval |= I_IDONE;
			}
			if ( (rval&I_IERR) ) {
				mlog( MLOG_NORMAL | MLOG_INV, _(
				    "INV: open failed on mount point \"%s\"\n"),
				     fs);
			}
			return rval;
		}

		/* We have to print only one file system to print by UUID */
		tok = inv_open( bywhat, INV_SEARCH_ONLY, fs);
		if ( tok != INV_TOKEN_NULL ) {
			prctx->index = 0;
			invmgr_inv_print(tok->d_invindex_fd, prctx);	
			inv_close( tok );
			rval |= I_IDONE;
		} else {
			mlog( MLOG_NORMAL | MLOG_INV, _(
			     "INV: open failed on file system id \"%s\"\n"),
			     fs);
			rval |= I_IERR;
		}
	}

	return rval;
}

/* This prints out all the sessions of a filesystem that are in the inventory */
bool_t
inv_DEBUG_print( int argc, char **argv ) 
{
	invt_counter_t *cnt = NULL;
	invt_fstab_t *arr = NULL;
	int fd, numfs, i;
	inv_idbtoken_t tok;
	int rval;
	invt_pr_ctx_t prctx;
	inv_oflag_t forwhat = INV_SEARCH_ONLY;
	prctx.mobj.type = INVT_NULLTYPE;
	prctx.fstab = prctx.invidx = prctx.invcheck = BOOL_FALSE;
	prctx.level = PR_MAXLEVEL;

	/* If user didnt indicate -i option, we can't do anything */
	rval = inv_getopt( argc, argv, &prctx );

	if (!prctx.invcheck && ! prctx.fstab) {
		if (! (rval & I_IFOUND)) {
			return BOOL_TRUE;
		} else if ( rval & I_IERR || rval & I_IDONE ) {
			return BOOL_FALSE;
		}
	}

	fd = fstab_getall( &arr, &cnt, &numfs, forwhat );
	free( cnt );

	if ( fd >= 0 ) {
		 if (prctx.fstab) {
			 fstab_DEBUG_print( arr, numfs );
			 if (! prctx.invidx)
				 return BOOL_FALSE;
		 }
		
		for ( i = 0; i < numfs; i++ ) {
			tok = inv_open( ( inv_predicate_t )INV_BY_UUID,
					forwhat,
				        &arr[i].ft_uuid );
			if ( tok == INV_TOKEN_NULL ) {
				free ( arr );
				return BOOL_FALSE;
			}

			if (prctx.invcheck) {
				mlog( MLOG_VERBOSE | MLOG_INV, _(
				     "INV: checking fs \"%s\"\n"),
				     &arr[i].ft_mountpt
				     );
				invmgr_inv_check(tok->d_invindex_fd);
			}
			else {
				prctx.index = i;
				invmgr_inv_print( tok->d_invindex_fd, 
						 &prctx );
			}
			inv_close( tok );
		}
	}

	return BOOL_FALSE;
}

#undef I_IFOUND
#undef I_IDONE
#undef I_IERR
