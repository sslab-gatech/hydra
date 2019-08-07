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

#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>

#include "config.h"

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"


/*----------------------------------------------------------------------*/
/*  init_idb                                                            */
/*                                                                      */
/*  Returns -1 if we are done with initialization, the fd if not.	*/
/*  The idb_token indicates whether there was an error or not, if the 	*/
/*  return value is -1.                                                 */
/*----------------------------------------------------------------------*/

int
init_idb( void *pred, inv_predicate_t bywhat, inv_oflag_t forwhat,
	  inv_idbtoken_t *tok )
{
	char fname[ INV_STRLEN ];
	char uuname[ INV_STRLEN ];
	int fd;

	*tok = INV_TOKEN_NULL;
	/* make sure INV_DIRPATH exists, and is writable */
	if ( make_invdirectory( forwhat ) < 0 )
		return I_DONE;

	/* come up with the unique file suffix that refers to this
	   filesystem */
	if ( fstab_get_fname( pred, uuname, bywhat, forwhat ) < 0 ) {
		return I_DONE;
	}

	( void )strcpy( fname, uuname );
	strcat ( fname, INV_INVINDEX_PREFIX );

	/* first check if the inv_index file exists: if not create it */
	if ( ( fd = open( fname, INV_OFLAG(forwhat) ) ) == -1 ) {
		if (errno != ENOENT) {
			INV_PERROR ( fname );
		} else if (forwhat == INV_SEARCH_N_MOD) {
			*tok = idx_create( fname, forwhat ); 
		} else {
			/* this happens when the inv is empty and the user
			   wants to do a search. this is legal - not an error */
			return I_EMPTYINV;
		}
		return I_DONE; /* we are done whether token's NULL or not */
	}
	
	/* we've got to do more housekeeping stuff. so signal that */
	return fd;
}





inv_idbtoken_t
get_token( int invfd, int stobjfd  )
{
	invt_desc_entry_t *desc;

	desc = (invt_desc_entry_t *) malloc
		( sizeof( invt_desc_entry_t ) );
	
	desc->d_invindex_fd = invfd;
	desc->d_stobj_fd  = stobjfd;
	desc->d_update_flag = 0;
	desc->d_invindex_off = -1;

	return (inv_idbtoken_t) desc; /* yukky, but ok for the time being */
}






void
destroy_token( inv_idbtoken_t tok )
{
	free ( (invt_desc_entry_t *) tok );
}



inv_sestoken_t
get_sesstoken( inv_idbtoken_t tok )
{
	inv_sestoken_t stok;

	stok = (inv_sestoken_t) malloc( sizeof( invt_sesdesc_entry_t ) );
	stok->sd_invtok = tok;
	stok->sd_session_off = stok->sd_sesshdr_off = -1;
	stok->sd_sesstime = (time32_t) 0;
	return stok;
}






/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
bool_t
invmgr_query_all_sessions (
	uuid_t		  *fsidp,
	void 		  *inarg,
	void 		  **outarg,
	search_callback_t func)
{
	invt_counter_t *cnt;
	invt_fstab_t *arr;
	int numfs, i, fd, invfd;
	char fname[INV_STRLEN];
	int result;
	inv_oflag_t forwhat = INV_SEARCH_ONLY;
	void *objectfound;
	bool_t ret = BOOL_FALSE;

	/* if on return, this is still null, the search failed */
	*outarg = NULL; 
	assert(inarg);

	fd = fstab_getall( &arr, &cnt, &numfs, forwhat );
	/* special case missing file: ok, outarg says zero */
	if ( fd < 0 && errno == ENOENT ) {
		return BOOL_TRUE;
	}
	if ( fd < 0 || numfs <= 0 ) {
		mlog( MLOG_NORMAL | MLOG_INV, _("INV: Error in fstab\n") );
		return ret;
	}
	
	close( fd );

	for ( i = 0; i < numfs; i++) {
		if ( fstab_get_fname( &arr[i].ft_uuid, fname, 
				     (inv_predicate_t)INV_BY_UUID,
				     forwhat) < 0 ) {
			mlog( MLOG_NORMAL | MLOG_INV, _(
			     "INV: Cant get inv-name for uuid\n")
			     );
			continue;
		}
		strcat( fname, INV_INVINDEX_PREFIX );
		invfd = open( fname, INV_OFLAG(forwhat) );
		if ( invfd < 0 ) {
			mlog( MLOG_NORMAL | MLOG_INV, _(
			     "INV: Open failed on %s\n"),
			     fname
			     );
			continue;
		}
		result = search_invt(fsidp, invfd, inarg, &objectfound, func);
		close(invfd);		

		/* if error return BOOL_FALSE */
		if (result < 0) {
			return BOOL_FALSE;
		} else if ((result == 1) && *outarg) {
			/* multiple entries found,  more info needed */
			*outarg = NULL;
			return BOOL_TRUE;
		} else if (result == 1) {
			*outarg = objectfound;
			ret = BOOL_TRUE;
		}
	}
	
	/* return val indicates if there was an error or not. *buf
	   says whether the search was successful */
	return ret;
}




/*----------------------------------------------------------------------*/
/* search_invt                                                          */
/*                                                                      */
/* Used by the toplevel (inv layer ) to do common searches on the inven-*/
/* tory. Caller supplies a callback routine that performs the real      */
/* comparison/check.                                                    */
/*----------------------------------------------------------------------*/

int
search_invt( 
	uuid_t			*fsidp,
	int			invfd,
	void			*arg,
	void			**buf,
	search_callback_t	do_chkcriteria)
{

	int 		fd, i;
	invt_entry_t	*iarr = NULL;
	invt_counter_t	*icnt = NULL;
	int	     	nindices;
	

	if (invfd == I_EMPTYINV)
		return -1;

	/* set the caller's buffer pointer to NULL initially.
	 * if no session found, the caller will expect to see
	 * NULL.
	 */
	*( char ** )buf = NULL;

	if ( ( nindices = GET_ALLHDRS_N_CNTS( invfd, (void **)&iarr, 
					      (void **)&icnt, 
					      sizeof( invt_entry_t ),
					      sizeof( invt_counter_t )) 
	      ) <= 0 ) {
		return -1;
	}

	free( icnt );

	/* we need to get all the invindex headers and seshdrs in reverse
	   order */
	for (i = nindices - 1; i >= 0; i--) {
		int			nsess, j;
		invt_sescounter_t 	*scnt = NULL;
		invt_seshdr_t		*harr = NULL;
		bool_t                  found;

		fd = open (iarr[i].ie_filename, O_RDONLY );
		if (fd < 0) {
			INV_PERROR( iarr[i].ie_filename );
			continue;
		}
		INVLOCK( fd, LOCK_SH );

		/* Now see if we can find the session we're looking for */
		if (( nsess = GET_ALLHDRS_N_CNTS_NOLOCK( fd, (void **)&harr, 
						  (void **)&scnt, 
						  sizeof( invt_seshdr_t ),
						 sizeof( invt_sescounter_t ))
		     ) < 0 ) {
			INV_PERROR( iarr[i].ie_filename );
			INVLOCK( fd, LOCK_UN );
			close( fd );
			continue;
		}
		free ( scnt );

		for (j = nsess - 1; j >= 0; j--) {
			invt_session_t ses;

			/* fd is kept locked until we return from the 
			   callback routine */

			/* Check to see if this session has been pruned 
			 * by xfsinvutil before checking it. 
			 */
			if (harr[j].sh_pruned) {
				continue;
			}

			/* if we need to check the fs uuid's and they don't
			 * match or we fail to get the session record,
			 * then keep looking
			 */
			if (fsidp) {
				int ret = GET_REC_NOLOCK(fd, &ses,
						         sizeof(invt_session_t),
						         harr[j].sh_sess_off);
				if (ret < 0)
					return ret;
				if (uuid_compare(ses.s_fsid, *fsidp))
					continue;
			}

			found = (* do_chkcriteria)(fd, &harr[j], arg, buf);
			if (! found ) continue;
			
			/* we found what we need; just return */
			INVLOCK( fd, LOCK_UN );
			close( fd );
			free( harr );

			return found; /* == -1 or 1 */
		}
		
		INVLOCK( fd, LOCK_UN );
		close( fd );
	}
	
	return 0;
}



/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/


int
invmgr_inv_print(
	int 		invfd, 
	invt_pr_ctx_t 	*prctx)
{

	int 		fd, i;
	invt_entry_t	*iarr = NULL;
	invt_counter_t	*icnt = NULL;
	int	     	nindices;
	uint           ref = 0;

	if (invfd == I_EMPTYINV)
		return 0;
	
		
	if ( ( nindices = GET_ALLHDRS_N_CNTS( invfd, (void **)&iarr, 
					      (void **)&icnt, 
					      sizeof( invt_entry_t ),
					      sizeof( invt_counter_t )) 
	      ) <= 0 ) {
		return -1;
	}
	
	free( icnt );

	if (prctx->invidx) {
		idx_DEBUG_printinvindices( iarr, (uint) nindices );
		free(iarr);
		return (0);
	}



	for ( i = 0; i < nindices; i++ ) {
		int 			nsess;
		invt_sescounter_t 	*scnt = NULL;
		invt_seshdr_t		*harr = NULL;
		int                     s;

		fd = open (iarr[i].ie_filename, O_RDONLY );
		if (fd < 0) {
			INV_PERROR( iarr[i].ie_filename );
			continue;
		}
		INVLOCK( fd, LOCK_SH );

		/* Now see if we can find the session we're looking for */
		if (( nsess = GET_ALLHDRS_N_CNTS_NOLOCK( fd, (void **)&harr, 
						  (void **)&scnt, 
						  sizeof( invt_seshdr_t ),
						 sizeof( invt_sescounter_t ))
		     ) < 0 ) {
			INV_PERROR( iarr[i].ie_filename );
			INVLOCK( fd, LOCK_UN );
			close( fd );
			continue;
		}
		free ( scnt );
		for( s = 0; s < nsess; s++ ) {
			/* fd is kept locked until we return from the 
			   callback routine */

			/* Check to see if this session has been pruned 
			 * by xfsinvutil before returning it. 
			 */
			if ( harr[s].sh_pruned ) {
				continue;
			}

			(void)DEBUG_displayallsessions( fd, &harr[ s ],
						        ref++, prctx);
		}
			
		INVLOCK( fd, LOCK_UN );
		close( fd );
	}
	
	free (iarr);
	return 0;
}



/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/


int
invmgr_inv_check(
	int invfd)
{

	int 		fd, i;
	invt_entry_t	*iarr = NULL;
	invt_counter_t	*icnt = NULL;
	int	     	nindices;

	if (invfd == I_EMPTYINV)
		return 0;
	
	if ( ( nindices = GET_ALLHDRS_N_CNTS( invfd, (void **)&iarr, 
					      (void **)&icnt, 
					      sizeof( invt_entry_t ),
					      sizeof( invt_counter_t )) 
	      ) <= 0 ) {
		return -1;
	}

	free( icnt );


	for ( i = 0; i < nindices; i++ ) {
		int 			nsess;
		invt_sescounter_t 	*scnt = NULL;
		invt_seshdr_t		*harr = NULL;
		int                     s;

		fd = open (iarr[i].ie_filename, O_RDONLY );
		if (fd < 0) {
			INV_PERROR( iarr[i].ie_filename );
			continue;
		}
		INVLOCK( fd, LOCK_SH );

		/* Now see if we can find the session we're looking for */
		if (( nsess = GET_ALLHDRS_N_CNTS_NOLOCK( fd, (void **)&harr, 
						  (void **)&scnt, 
						  sizeof( invt_seshdr_t ),
						 sizeof( invt_sescounter_t ))
		     ) < 0 ) {
			INV_PERROR( iarr[i].ie_filename );
			INVLOCK( fd, LOCK_UN );
			close( fd );
			continue;
		}
		free ( scnt );
		
		if ((iarr[i].ie_timeperiod.tp_start != harr[0].sh_time) ||
		    (iarr[i].ie_timeperiod.tp_end != harr[nsess-1].sh_time)) {
			printf(_("INV: Check %d failed.\n"), i+1);
			printf(_("invidx (%d)\t%d - %d\n"),
			       i+1,
			       iarr[i].ie_timeperiod.tp_start,
			       iarr[i].ie_timeperiod.tp_end);
			for( s = 0; s < nsess; s++ ) {
				printf(_("tm (%d)\t%d\n"), s, harr[s].sh_time);
			}
		}
		else {
			printf(_("INV: Check %d out of %d succeeded\n"),
			       i+1, nindices);
		}
		INVLOCK( fd, LOCK_UN );
		close( fd );
	}
	
	return 0;
}


/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

/* ARGSUSED */
bool_t
tm_level_lessthan( int fd, invt_seshdr_t *hdr, void *arg,
		   void **tm )
{
	u_char level = *(u_char *)arg;
	*tm = NULL;
	if ( IS_PARTIAL_SESSION( hdr ) )
		return 0;
	if (hdr->sh_level < level ) {
#ifdef INVT_DEBUG
		mlog( MLOG_DEBUG | MLOG_INV, "$ found level %d < %d\n", hdr->sh_level, 
		     level );
#endif
		*tm = calloc( 1, sizeof( time32_t ) );
		memcpy( *tm, &hdr->sh_time, sizeof( time32_t ) );
		return 1;
	}
	return 0;
}




/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

bool_t
lastsess_level_lessthan( int fd, invt_seshdr_t *hdr, void *arg,
			 void **buf )
{
	u_char level = *(u_char *)arg;
	*buf = NULL;
	if ( IS_PARTIAL_SESSION( hdr ) )
		return 0;
	if (hdr->sh_level < level ) {
#ifdef INVT_DEBUG
		mlog( MLOG_DEBUG | MLOG_INV, "$ found (ses) level %d < %d \n",
		     hdr->sh_level, level );
#endif
		return stobj_make_invsess( fd, (inv_session_t **) buf, hdr );
	}
	return 0;

}



/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

bool_t
lastsess_level_equalto( int fd, invt_seshdr_t *hdr,
		       void *arg, void **buf )
{
	u_char level = *(u_char *)arg;
	*buf = NULL;
	if ( IS_PARTIAL_SESSION( hdr ) )
		return 0;
	if (hdr->sh_level == level ) {
#ifdef INVT_DEBUG
	mlog( MLOG_DEBUG | MLOG_INV, "$ found (ses) level %d == %d \n", hdr->sh_level,
	     level );
#endif
		return stobj_make_invsess( fd, (inv_session_t **) buf, hdr );
	}
	return 0;


}






/*----------------------------------------------------------------------*/
/* insert_session                                                       */
/*                                                                      */
/* insert a session that may or may not be already in the inventory.    */
/* this is used in reconstructing the database.                         */
/*----------------------------------------------------------------------*/
bool_t
insert_session( invt_sessinfo_t *s)
{	
	inv_idbtoken_t tok = INV_TOKEN_NULL;
	int invfd, stobjfd = -1;
	invt_idxinfo_t	  idx;
	bool_t		  ret = BOOL_FALSE;
	inv_oflag_t       forwhat = INV_SEARCH_N_MOD;

	/* initialize the inventory */
	if ( ( invfd = init_idb ( (void *) s->ses->s_fsid, 
				  (inv_predicate_t) INV_BY_UUID,
 				  forwhat,
				  &tok ) ) < 0 ) {
		if ( tok == INV_TOKEN_NULL ) {
#ifdef INVT_DEBUG
			mlog( MLOG_DEBUG | MLOG_INV, "INV: insert_session: init_db "
			      "failed\n" );
#endif
			return BOOL_FALSE;
		}
		
		invfd = tok->d_invindex_fd;
		close( tok->d_stobj_fd );
		destroy_token( tok );
	}
	
	/* at this point we know that invindex has at least one entry
	
	   First we need to find out if this session is in the inventory
	   already. To find the storage object that can possibly
	   contain this session, it suffices to sequentially search the
	   inventory indices of this filesystem for the particular invt-entry
	 */
	INVLOCK( invfd, LOCK_EX );
	idx.invfd = invfd;
	stobjfd = idx_find_stobj( &idx, s->seshdr->sh_time );
	if (stobjfd < 0) {
		INVLOCK( invfd, LOCK_UN );
		free( idx.icnt );
		free( idx.iarr );
		return BOOL_FALSE;
	}

	/* Now put the session in the storage-object */
	INVLOCK( stobjfd, LOCK_EX );
	if ( ( stobj_insert_session( &idx, stobjfd, s ) < 0 ) ||
		( idx_recons_time ( s->seshdr->sh_time, &idx ) < 0 ) )
			ret = BOOL_TRUE;
		
	INVLOCK( stobjfd, LOCK_UN );
	INVLOCK( invfd, LOCK_UN );

	free( idx.icnt );
	free( idx.iarr );

	if (ret) return BOOL_FALSE;

	/* make sure the fstab is uptodate too */
	if ( fstab_put_entry( &s->ses->s_fsid, s->ses->s_mountpt, s->ses->s_devpath,
			      forwhat ) < 0 )
		return BOOL_FALSE;

	/* and we are done */
	return BOOL_TRUE;
	
}



/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int
make_invdirectory( inv_oflag_t forwhat )
{
	struct stat64 st;
	char path[PATH_MAX];
	char *p;

	p = strcpy( path, INV_DIRPATH );

	if ( stat64( path, &st ) == 0 )
		return 1;

	if ( forwhat == INV_SEARCH_ONLY || errno != ENOENT )
		return -1;

	do {
		p++;
		if ( *p == '/' ) {
			*p = '\0';
			if ( mkdir( path, (mode_t)0755 ) < 0 ) {
				if ( errno != EEXIST ) {
					INV_PERROR( path );
					return -1;
				}
			}
			*p = '/';
		}
	} while ( *p );

	if ( mkdir( path, (mode_t)0755 ) < 0 ) {
		if ( errno != EEXIST ) {
			INV_PERROR( path );
			return -1;
		}
	}

	mlog( MLOG_VERBOSE | MLOG_INV, _("%s created\n"), path );
	return 1;
}

#ifdef NOTDEF

bool_t
invmgr_lockinit( void )
{
	if ( invlock_fd == -1 ) {
		if (( invlock_fd = open( INV_LOCKFILE, 
					O_RDONLY | O_CREAT, S_IRUSR|S_IWUSR )) < 0 ) {
			INV_PERROR( INV_LOCKFILE );
			return BOOL_FALSE;
		}
		fchmod ( invlock_fd, INV_PERMS );
	}
	return BOOL_TRUE;
}
	
	
bool_t
invmgr_trylock( invt_mode_t mode )
{
	int md;
	assert( invlock_fd >= 0 );
	
	md = (mode == INVT_RECONSTRUCT) ? LOCK_EX: LOCK_SH;

	if (INVLOCK( invlock_fd, md | LOCK_NB ) < 0)
		return BOOL_FALSE;

	return BOOL_TRUE;
}

void
invmgr_unlock( void )
{
	assert( invlock_fd >= 0 );
	
	INVLOCK( invlock_fd, LOCK_UN );	

}

#endif

