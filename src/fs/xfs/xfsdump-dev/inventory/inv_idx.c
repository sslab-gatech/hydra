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

#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <assert.h>
#include <string.h>

#include "config.h"

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"



/*----------------------------------------------------------------------*/
/* insert_invindexentry                                                 */
/*                                                                      */
/* given a time, find the invindex that has the time-period it can fit  */
/* into.                                                                */
/*----------------------------------------------------------------------*/
uint
idx_insert_newentry( int fd, /* kept locked EX by caller */
		     int *stobjfd, /* OUT */
		     invt_entry_t *iarr, invt_counter_t *icnt,
		     time32_t tm )
{
	uint i;
	inv_oflag_t forwhat = INV_SEARCH_N_MOD;
/*	invt_entry_t ient;
	ient.ie_timeperiod.tp_start = ient.ie_timeperiod.tp_end = tm; */
	
	/* If time period of the new entry is before our first invindex,
	   we have to insert a new invindex in the first slot */
	if ( iarr[0].ie_timeperiod.tp_start > tm ) {
		/* *stobjfd = idx_put_newentry( fd, 0, iarr, icnt, &ient );*/
		*stobjfd = open( iarr[0].ie_filename, INV_OFLAG(forwhat) );
		return 0;
	}

	for ( i = 0; i < icnt->ic_curnum; i++ ) {
		/* if our time is nicely within an existing entry's time
		   period, hellalujah */
		if ( IS_WITHIN( &iarr[i].ie_timeperiod, tm ) ) {
#ifdef INVT_DEBUG
			mlog( MLOG_DEBUG | MLOG_INV, "INV: is_within %d\n",i );
#endif
			*stobjfd = open( iarr[i].ie_filename, INV_OFLAG(forwhat) );
			return i;
		}
		if ( iarr[i].ie_timeperiod.tp_end == 0  && 
		     iarr[i].ie_timeperiod.tp_start  == 0 ) {
#ifdef INVT_DEBUG
			mlog( MLOG_DEBUG | MLOG_INV, "INV: end = start \n" );
			mlog( MLOG_DEBUG | MLOG_INV,"BEF: st %ld end %ld\n",
			     iarr[i].ie_timeperiod.tp_start,
			     iarr[i].ie_timeperiod.tp_end );
#endif
			
			iarr[i].ie_timeperiod.tp_start = 
				iarr[i].ie_timeperiod.tp_end = tm;
			PUT_REC_NOLOCK( fd, iarr, 
				       icnt->ic_curnum * sizeof(invt_entry_t),
				       (off64_t) sizeof( invt_counter_t ) );
#ifdef INVT_DEBUG
			mlog( MLOG_DEBUG | MLOG_INV,"AFT: st %ld end %ld\n",
			     iarr[i].ie_timeperiod.tp_start,
			     iarr[i].ie_timeperiod.tp_end );
#endif
			*stobjfd = open( iarr[i].ie_filename, INV_OFLAG(forwhat) );
			return i;
		}	



		/* if it is beyond the end of this timeperiod, see if we 
		   belong to a timeperiod that doesn't have an entry */
		if ( iarr[i].ie_timeperiod.tp_end < tm ) {
			/* see if we're the last entry here */
			if ( i == icnt->ic_curnum - 1 ) {
				/* our slot is (i+1)th entry. Make the 
				   timeperiod's the same as it was. As far
				   as I can see there is no way that 
				   timeperiods can overlap.
				
				   insert the new entry and write back 
				   icnt and invindex entry */
				/* *stobjfd = idx_put_newentry( fd, i+1, iarr, 
							     icnt, &ient );*/
			      *stobjfd = open( iarr[i].ie_filename, INV_OFLAG(forwhat) );
			      return i;
			}
			/* see if the next entry starts later than us */
			if ( iarr[i+1].ie_timeperiod.tp_start > tm ) {
				
				
				/* We have the option of pushing entries
				   after (i) forward by one slot, and 
				   taking the (i+1)th slot, OR just hooking
				   up with the next entry. 
				   We choose the former. */
				
				/* the timeperiods had better not overlap */
				assert(( tm > iarr[i].ie_timeperiod.tp_end ) &&
				       ( tm < iarr[i+1].ie_timeperiod.tp_start ));

				/* shift everything from (i+1) onwards by 
				   one. Then insert the new entry and write
				   back icnt and invindex entries */
				/* *stobjfd = idx_put_newentry( fd, i+1, iarr, 
							     icnt, &ient );*/
			      *stobjfd = open( iarr[i].ie_filename, INV_OFLAG(forwhat) );
				return i;
			}
		}	
	}		

	/* We couldnt find anything that fits */
	assert( 0 );	/* We can't get here ! */
	return -1;

	
}


/*----------------------------------------------------------------------*/
/* idx_put_newentry                                                     */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int
idx_put_newentry( 
	invt_idxinfo_t *idx, 
	invt_entry_t *ient )
{
	invt_entry_t	*idxarr;
	int stobjfd;
	
	int fd = idx->invfd; 	/* kept locked EX by caller */
	uint index = idx->index + 1;
	invt_entry_t *iarr = idx->iarr;
	invt_counter_t *icnt = idx->icnt;

	stobj_makefname( ient->ie_filename );
	if ( ( stobjfd = stobj_create( ient->ie_filename ) ) < 0 )
		return -1;

	icnt->ic_curnum++; /* there is no maximum */

	idxarr = ( invt_entry_t * ) calloc ( icnt->ic_curnum, 
					     sizeof( invt_entry_t ) );
	memcpy( idxarr, iarr, ( size_t ) ( sizeof( invt_entry_t ) * index ) );

	/* shift everything from (i+1) onwards by one */
	if ( index <  icnt->ic_curnum - 1 ) 
		memcpy( &idxarr[ index + 1 ], &iarr[ index ], 
		       ( size_t ) ( ( icnt->ic_curnum - index - 1 ) *
				    sizeof( invt_entry_t ) ) );
	/* insert the new entry */
	memcpy( &idxarr[ index ], ient, sizeof( invt_entry_t ) );

	
	if ( ( PUT_COUNTERS( fd, icnt ) < 0 ) ||
		( PUT_REC_NOLOCK( fd, idxarr, 
				  icnt->ic_curnum * sizeof( invt_entry_t ), 
				  sizeof( invt_counter_t ) ) < 0 ) ) {
			/* XXX delete the stobj that we just created */
			
			memset( ient->ie_filename, 0 , INV_STRLEN );
			free( idxarr );
			return -1;
		}

	free( iarr );
	idx->iarr = idxarr;
	return stobjfd;
	
}




/*----------------------------------------------------------------------*/
/* idx_find_stobj                                                       */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/


int
idx_find_stobj( invt_idxinfo_t *idx,
	        time32_t tm )
{

	int 		stobjfd;

	/* since sessions can be inserted in random order, the invindex
	   table can contain time-periods that don't have corresponding
	   entries for */
	if ( GET_ALLHDRS_N_CNTS_NOLOCK( idx->invfd, (void **)&idx->iarr, 
						     (void **)&idx->icnt, 
						     sizeof( invt_entry_t ),
				sizeof( invt_counter_t ) ) < 0 ) {
		return -1;
	}

#ifdef INVT_DEBUG
	printf( "idx_find_stobj Time: %ld\n", tm );
	idx_DEBUG_printinvindices( idx->iarr, idx->icnt->ic_curnum );
#endif

	/* Now figure out where we are going to insert this stobj among the
	   invindices and put it there */
	idx->index = idx_insert_newentry( idx->invfd, &stobjfd, idx->iarr, 
						 idx->icnt, tm );

	return stobjfd;
}






/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

inv_idbtoken_t
idx_create( char *fname, inv_oflag_t forwhat )
{
	int stobjfd, fd;
	inv_idbtoken_t tok;

	/* This is not to be called when the user wants to open
	   the db for SEARCH_ONLY. */
	assert( forwhat != INV_SEARCH_ONLY );

	if ((fd = open ( fname , INV_OFLAG(forwhat) | O_CREAT, S_IRUSR|S_IWUSR ) ) < 0 ) {
		INV_PERROR ( fname );
		return INV_TOKEN_NULL;
	}
	
	INVLOCK( fd, LOCK_EX );
	fchmod( fd, INV_PERMS );

#ifdef INVT_DEBUG
	mlog( MLOG_NITTY | MLOG_INV, "creating InvIndex %s\n", fname);
#endif
	
	/* create the first entry in the new inv_index */
	stobjfd = idx_create_entry( &tok, fd, BOOL_TRUE );
	
	INVLOCK( fd, LOCK_UN );

	if ( stobjfd < 0 )
		return INV_TOKEN_NULL;
	return tok;
}


/*----------------------------------------------------------------------*/
/* idx_recons_time                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/
int
idx_recons_time( time32_t tm, invt_idxinfo_t *idx )
{
	invt_timeperiod_t *tp = &idx->iarr[idx->index].ie_timeperiod;
	if ( tp->tp_start && IS_WITHIN( tp, tm ) )
		return 1;

	if ( tm > tp->tp_end || tp->tp_end == 0 ) 
		tp->tp_end =  tm;
	if ( tm < tp->tp_start || tp->tp_start == 0 )
		tp->tp_start = tm;
	PUT_REC_NOLOCK( idx->invfd,  &idx->iarr[idx->index], 
		        sizeof( invt_entry_t ), IDX_HDR_OFFSET(idx->index) );
	return 1;
}




/*----------------------------------------------------------------------*/
/* put_sesstime                                                         */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int
idx_put_sesstime( inv_sestoken_t tok, bool_t whichtime)
{
	int rval;
	invt_entry_t ent;
	int fd = tok->sd_invtok->d_invindex_fd;

	INVLOCK( fd, LOCK_EX );

	rval = GET_REC_NOLOCK( fd, &ent, sizeof( invt_entry_t ),
			        tok->sd_invtok->d_invindex_off);
	if ( rval < 0 ) {
		INVLOCK( fd, LOCK_UN );
		return -1;
	}
	ent.ie_timeperiod.tp_end = tok->sd_sesstime;
	
	if ( whichtime == INVT_STARTTIME || ent.ie_timeperiod.tp_start == 0 ) {
		ent.ie_timeperiod.tp_start = tok->sd_sesstime;
	}
#ifdef INVT_DEBUG
	mlog( MLOG_DEBUG | MLOG_INV,"Putsestime: st %ld end %ld\n",
			      ent.ie_timeperiod.tp_start,
			      ent.ie_timeperiod.tp_end );
#endif
	rval = PUT_REC_NOLOCK(fd, &ent, sizeof(invt_entry_t),
			      tok->sd_invtok->d_invindex_off);
	
#ifdef INVT_DEBUG
	{
		int nindices;
		invt_entry_t *iarr = NULL;
		invt_counter_t *icnt = NULL;
		if ( ( nindices = GET_ALLHDRS_N_CNTS_NOLOCK( fd, 
						     (void **)&iarr, 
						     (void **)&icnt, 
						sizeof( invt_entry_t ),
				sizeof( invt_counter_t ))) < 0 ) {
			return -1;		 
		}
		idx_DEBUG_printinvindices( iarr, (uint) nindices );
		free( iarr );
		free( icnt );
	}
#endif

	INVLOCK( fd, LOCK_UN );
	return rval;
}



/* an inventory index entry keeps track of a single storage object;
   it knows about its name (ie filename) and the timeperiod that the
   it contains dump sessions for.
   note that each file system has its own (set of) inventory indices.
*/

/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int
idx_create_entry(  
	inv_idbtoken_t *tok, 
	int invfd, 	/* kept locked EX  by caller */
	bool_t firstentry )
{
	invt_entry_t   	ent;
	int	      	fd;
	off64_t 	hoff;


	memset ( &ent, 0, sizeof( ent ) );
	
	/* initialize the start and end times to be the same */
	ent.ie_timeperiod.tp_start = ent.ie_timeperiod.tp_end = (time32_t)0;
	stobj_makefname( ent.ie_filename );

	if ( firstentry ) {
		invt_counter_t cnt;

		cnt.ic_maxnum = INVT_MAX_INVINDICES;
		cnt.ic_curnum = 1;
		cnt.ic_vernum = INV_VERSION;

		fd = stobj_create( ent.ie_filename );
		if ( fd < 0 ) {
			return -1;
		}

		if ( PUT_REC_NOLOCK( invfd, &cnt, sizeof(cnt), (off64_t)0 ) < 0 )
			return -1;

		hoff = sizeof( invt_counter_t );

		if ( PUT_REC_NOLOCK( invfd, &ent, sizeof( ent ), hoff ) < 0)
			return -1;
	} else {
		invt_counter_t *cnt = NULL;

		if ( GET_COUNTERS( invfd, &cnt )  < 0 ) {
			return -1;
		}
		
		/* XXX check if there are too many indices. if so, create 
		   another and leave a pointer to that in here */
		
		/* create the new storage object */
		fd = stobj_create( ent.ie_filename );
		if ( fd < 0 ) {
			return -1;
		}
		++(cnt->ic_curnum);
		if ( PUT_COUNTERS( invfd, cnt ) < 0 ) {
			return -1;
		}

		/* add the new index entry to the array, at the end */

		hoff = IDX_HDR_OFFSET( cnt->ic_curnum - 1 );
		free (cnt);
#ifdef INVT_DEBUG
		mlog( MLOG_NITTY | MLOG_INV, "new stobj name %s @ offset %d\n",
		      ent.ie_filename,(int)hoff );
#endif
		if (PUT_REC_NOLOCK( invfd, &ent, sizeof( ent ), hoff) < 0 )
			return -1;
		
	}

	*tok = get_token( invfd, fd );
	(*tok)->d_invindex_off = hoff;
	(*tok)->d_update_flag |= NEW_INVINDEX;
	(*tok)->d_oflag = INV_SEARCH_N_MOD;
	return fd;

}






/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int
idx_get_stobj( int invfd, inv_oflag_t forwhat, int *index )
{
	invt_entry_t 	*ent = 0;
	int	     	 fd;

	/* if there's space anywhere at all, then it must be in the last
	   entry. get_lastheader() does the locking */
	
	if ((*index = get_lastheader( invfd, (void **)&ent, 
				       sizeof(invt_entry_t),
				       sizeof(invt_counter_t) ) ) < 0 )
		return -1;
	/* at this point we know that there should be at least one invindex
	   entry present */
	assert ( ent != NULL );	
	assert ( ent->ie_filename );

	fd = open( ent->ie_filename, INV_OFLAG(forwhat) );
	if ( fd < 0 )
		INV_PERROR( ent->ie_filename );
	free ( ent );

	return fd;
}


int
idx_DEBUG_printinvindices( invt_entry_t *iarr, uint num )
{
	uint i;
	uint k;

	char s[9];
	printf( "\n ==================================\n"
	        " InvIndex\n # StObjs\t%d\n", num );
#define INV_UUID_STR_LEN	36 /* not exported via uuid.h */
	for ( i = 0; i < num; i++ ) {
		k = strlen( iarr[i].ie_filename );
		strncpy( s, (char *) iarr[i].ie_filename + k -
			( INV_UUID_STR_LEN + strlen(INV_STOBJ_PREFIX)), 8 );
		s[8]= 0;
		printf("%d. %s \t( %d - %d )\n", i, s, 
		       iarr[i].ie_timeperiod.tp_start,
		       iarr[i].ie_timeperiod.tp_end );
	}
#undef INV_UUID_STR_LEN
	printf( "\n ==================================\n");
	return 1;
	
}

int
idx_DEBUG_print ( int fd )
{
	int nindices;
	invt_entry_t *iarr = NULL;
	invt_counter_t *icnt = NULL;
	if ( ( nindices = GET_ALLHDRS_N_CNTS_NOLOCK( fd, 
						    (void **)&iarr, 
						    (void **)&icnt, 
						    sizeof( invt_entry_t ),
				         sizeof( invt_counter_t ))) < 0 ) {
		return -1;		 
	}
	idx_DEBUG_printinvindices( iarr, (uint) nindices );
	free( iarr );
	free( icnt );

	return 1;
}



int
DEBUG_displayallsessions( int fd, invt_seshdr_t *hdr, uint ref,
			  invt_pr_ctx_t *prctx)
{
	inv_session_t *ses;
	if ( stobj_make_invsess( fd, &ses, hdr ) < 1 )
		return -1;

	DEBUG_sessionprint( ses, ref, prctx);
	free( ses->s_streams );
	free( ses );
	
	return 0;
}

