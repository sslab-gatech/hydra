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
#include <assert.h>

#include "config.h"

#include "inv_priv.h"
#include "inv_oref.h"


/*
 * Resolve a stobj, invidx or fstab
 */
int
oref_resolve_(
	invt_oref_t 	*obj,
	invt_objtype_t	type)
{
	int rval;

	type &= INVT_OTYPE_MASK;
	assert(type);
	assert(! OREF_ISRESOLVED(obj, INVT_OTYPE_MASK));

	switch (type) {
	      case INVT_OTYPE_INVIDX:
		rval = oref_resolve_invidx(obj);
		break;

	      case INVT_OTYPE_STOBJ:
		rval = oref_resolve_stobj(obj);
		break;

	      case INVT_OTYPE_FSTAB:
		rval = oref_resolve_fstab(obj);
		break;

	      default:
		assert(0);
		break;
	}
	
	return rval;
}





/*
 * Resolve an object reference upto a specified point 
 */

int
oref_resolve_upto(
	invt_oref_t 	*obj, 
	invt_objtype_t	type)
{
	int rval = INV_OK;

	assert (OREF_ISRESOLVED(obj, INVT_OTYPE_MASK));
	assert (OREF_ISLOCKED(obj));

	/* we arent interested in anything else */
	type &= INVT_RES_MASK;
	assert(type);

	if (type >= INVT_RES_COUNTERS) {
		rval = oref_resolve_counters(obj);
	}
	if (type >= INVT_RES_ENTRIES && rval != INV_ERR) {
		rval = oref_resolve_entries(obj);
	}
	if (type >= INVT_RES_HDRS && rval != INV_ERR) {
		rval = oref_resolve_hdrs(obj);
	}
	if (type >= INVT_RES_SESSIONS && rval != INV_ERR) {
		rval = oref_resolve_sessions(obj);
	}
	if (type >= INVT_RES_STRMS && rval != INV_ERR) {
		rval = oref_resolve_strms(obj);
	}
	if (type >= INVT_RES_MFILES && rval != INV_ERR) {
		rval = oref_resolve_mfiles(obj);
	}
	
	return rval;
}



int
oref_resolve_entries(
	invt_oref_t 	*obj)
{
	if (OREF_ISRESOLVED(obj, INVT_RES_ENTRIES))
		return INV_OK;

	assert(! OREF_ISRESOLVED(INVT_OTYPE_STOBJ));

	if (OREF_ISRESOLVED(INVT_OTYPE_INVIDX)) {
		invt_entry_t *ent;
		assert(OREF_CNT_CURNUM(obj));

		if (GET_ENTRIES(obj->fd, &ent, OREF_CNT_CURNUM(obj),
				sizeof(invt_entry_t)) < 0){
			return INV_ERR;
		}
		obj->eu_ent = ent;
	}
	else {
		invt_fstab_t *ent;
		assert(OREF_CNT_CURNUM(obj));
		if (GET_ENTRIES(obj->fd, &ent, OREF_CNT_CURNUM(obj),
				sizeof(invt_fstab_t)) < 0){
			return INV_ERR;
		}
		obj->eu_fstabent = ent;
	}

	OREF_SET_TYPE(obj, INVT_RES_COUNTERS);
		
	return INV_OK;
}






int
oref_resolve_counters(
	invt_oref_t 	*obj)
{
	if (OREF_ISRESOLVED(obj, INVT_RES_COUNTERS))
		return INV_OK;

	if (OREF_ISRESOLVED(INVT_OTYPE_STOBJ)) {
		invt_sescounter_t *sescnt;
		sescnt = calloc(1, sizeof(invt_sescounter_t));
		if (GET_SESCOUNTERS(obj->fd, sescnt) < 0){
			free(sescnt);
			return INV_ERR;
		}
		obj->cu_sescnt = sescnt;
	}
	else {
		invt_counter_t *cnt;
		cnt = calloc(1, sizeof(invt_counter_t));
		if (GET_COUNTERS(obj->fd, cnt) < 0){
			free(cnt);
			return INV_ERR;
		}
		obj->cu_cnt = cnt;
	}

	OREF_SET_TYPE(obj, INVT_RES_COUNTERS);
		
	return INV_OK;
}



int
oref_sync(
	invt_oref_t 	*obj, 
	invt_objtype_t	type)
{
	int rval;

	type &= INVT_RES_MASK;
	assert(type);
	assert(OREF_ISRESOLVED(obj, INVT_OTYPE_MASK));
	assert(OREF_ISRESOLVED(obj, type));
	assert(OREF_ISLOCKED(obj));

	switch (type) {
	      case INVT_RES_COUNTERS:
		rval = PUT_REC_NOLOCK(obj->fd, 
				      OREF_CNT(obj), 
				      sizeof(*OREF_CNT(obj)),
				      (off64_t) 0);
				      
		break;

	      case INVT_RES_ENTRIES:
		assert(! OREF_ISRESOLVED(obj, INVT_OTYPE_STOBJ));

		rval = PUT_REC_NOLOCK(obj->fd, 
				      OREF_ENTRIES(obj), 
				      sizeof(*OREF_ENTRIES(obj)),
				      (off64_t) sizeof(*OREF_CNT(obj)));
		
		break;

	      default:
		assert(0);
		break;
	}
	
	return rval;
}

int
oref_sync_append(
	invt_oref_t 	*obj, 
	invt_objtype_t	type,
	void		*entry,
	size_t		entsz)
{
	int rval;

	type &= INVT_RES_MASK;
	assert(type);
	assert(OREF_ISRESOLVED(obj, INVT_OTYPE_MASK));
	assert(OREF_ISLOCKED(obj));

	switch (type) {
	      case INVT_RES_ENTRIES:
		assert(! OREF_ISRESOLVED(obj, INVT_OTYPE_STOBJ));
		
		rval = PUT_REC_NOLOCK(obj->fd, 
				      entry,
				      entsz,
				      IDX_HDR_OFFSET(OREF_CNT_CURNUM(obj) - 1));
		if (OREF_ISRESOLVED(obj, INVT_RES_ENTRIES)) 
			free((oref)->eu_ent);
		OREF_UNSET_TYPE(obj, INVT_RES_ENTRIES);
		
		break;

	      default:
		assert(0);
		break;
	}
	
	return rval;
}



void
_oref_free(
	invt_oref_t 	*obj, 
	bool_t 		freethis)
{
	/* 
	 * Unlock the object *if* it is locked.
	 * We dont want to actually close the fd because,
	 * the tokens still need it.
	 */
	OREF_UNLOCK(obj);	

	if (OREF_ISRESOLVED(obj, INVT_OTYPE_STOBJ) ){
		if (OREF_ISRESOLVED(obj, INVT_RES_COUNTERS)) 
			free((oref)->cu_sescnt);
		if (OREF_ISRESOLVED(obj, INVT_RES_HDRS))
			free((oref)->eu_hdr);
		if (OREF_ISRESOLVED(obj, INVT_RES_SESSIONS))
			free((oref)->eu_ses);
		if (OREF_ISRESOLVED(obj, INVT_RES_STRMS))
			free((oref)->eu_strm);
		if (OREF_ISRESOLVED(obj, INVT_RES_MFILES))
			free((oref)->eu_mfile);
	}
	else if (OREF_ISRESOLVED(obj, INVT_OTYPE_INVIDX)) {
		if (OREF_ISRESOLVED(obj, INVT_RES_COUNTERS)) 
			free((oref)->cu_cnt);
		if (OREF_ISRESOLVED(obj, INVT_RES_ENTRIES)) 
			free((oref)->eu_ent);
	}
	else if (OREF_ISRESOLVED(obj, INVT_OTYPE_FSTAB)) {
		if (OREF_ISRESOLVED(obj, INVT_RES_COUNTERS)) 
			free((oref)->cu_cnt);
		if (OREF_ISRESOLVED(obj, INVT_RES_ENTRIES)) 
			free((oref)->eu_fstabent);
	}
	OREF_INIT(obj);
	if (freethis)
		free(obj);
}



/*
 * Resolve the invidx and a suitable stobj.
 * Also resolves an idb_token as a side effect.
 */

int
oref_resolve(
	invt_oref_t	*invidx,
	inv_predicate_t bywhat,
	void 		*pred)
{
	char 		fname[ INV_STRLEN ];
	char 		uuname[ INV_STRLEN ];
	int 		fd;
	invt_oref_t	*stobj;
	int		index;

	assert(! OREF_ISRESOLVED(invidx, INVT_OTYPE_MASK));
	
	OREF_SET_TYPE(invidx, INVT_OTYPE_INVIDX);

	/* come up with the unique file suffix that refers to this
	   filesystem */
	if (fstab_get_fname(pred, uuname, bywhat) < 0) {
		return INV_ERR;
	}
	(void) strcpy(fname, uuname);
	strcat(fname, INV_INVINDEX_PREFIX);

	/* first check if the inv_index file exists: if not create it */
	if ((fd = open(fname, O_RDWR)) == -1) {
		if (errno != ENOENT) {
			INV_PERROR (fname);
			return INV_ERR;
		}
		/* create the invidx */
		return oref_resolve_new_invidx(invidx, fname);	
	}

	invidx->fd = fd;
	OREF_LOCK(invidx, LOCK_EX);

	if (oref_resolve_child(invidx, &index) == INV_ERR) {
		OREF_UNLOCK(invidx);
		return INV_ERR;
	}

	stobj = OREF_CHILD(invidx);

	/* Now we need to make sure that this has enough space */
	OREF_LOCK(stobj, LOCK_SH);
	
	if (oref_resolve_upto(stobj, INVT_RES_COUNTERS) == INV_ERR) {
		OREF_UNLOCK(stobj);
		OREF_UNLOCK(invidx);
		return INV_ERR;
	}

	/* create another storage object ( and, an inv_index entry for it 
	   too ) if we've filled this one up */
	if (OREF_CNT_CURNUM(stobj) >= OREF_CNT_MAXNUM(stobj)) {
		int 	rval;
#ifdef INVT_DEBUG
		mlog( MLOG_DEBUG | MLOG_INV, "$ INV: creating a new storage obj & "
		      "index entry. \n" );
#endif	
		/* Close(), Destroy() and mark unresolved */
		OREF_UNRESOLVE_CHILD(invidx);
		rval = oref_resolve_new_stobj(invidx, BOOL_FALSE);
		/* rval = oref_idx_create_entry(invidx, BOOL_FALSE); */
		OREF_UNLOCK(invidx);
		
		return rval;
	}

	OREF_UNLOCK(stobj);
	OREF_UNLOCK(invidx);

	tok = get_token(invidx->fd, stobj->fd);
	tok->d_invindex_off = IDX_HDR_OFFSET(index - 1);
	
	OREF_SET_TOKEN(tok);
	return INV_OK;

}

/*
 * Resolve the invidx entirely, and open the StObj.
 * Invidx is kept locked by caller
 */
int
oref_resolve_child(
	invt_oref_t 	*invidx,
	int		*index)
{
	invt_entry_t 	*ent;
	assert(OREF_IS_LOCKED(invidx));

	if (oref_resolve_upto(invidx, INVT_RES_ENTRIES) == INV_ERR)	
		return INV_ERR;
	
	ent = OREF_ENTRIES(invidx);
	
	/* at this point we know that there should be at least one invindex
	   entry present */
	assert ( ent != NULL );	
	assert ( ent->ie_filename );

	fd = open( ent->ie_filename, O_RDWR );
	if ( fd < 0 ) {
		INV_PERROR( ent->ie_filename );
		return INV_ERR;
	}

	stobj = calloc(1, sizeof(invt_oref_t));
	OREF_SET_CHILD(invidx, stobj);
	stobj->fd = fd;

	return INV_OK;
}



/* used to be idx_create */
int
oref_resolve_new_invidx(
	invt_oref_t 	*invidx,
	char		*fname)
{
	int stobjfd, fd;
	inv_idbtoken_t tok;

	if ((fd = open ( fname , O_RDWR | O_CREAT, S_IRUSR|S_IWUSR ) ) < 0 ) {
		INV_PERROR ( fname );
		return INV_ERR;
	}
	invidx->fd = fd;
	OREF_LOCK(invidx, LOCK_EX);
	fchmod(fd, INV_PERMS);

#ifdef INVT_DEBUG
	mlog( MLOG_NITTY | MLOG_INV, "creating InvIndex %s\n", fname);
#endif	
	/* create the new stobj as its first entry */
	rval = oref_resolve_new_stobj(invidx, IS_EMPTY);

	OREF_UNLOCK(invidx);
	return rval;

}



/* formerly idx_create_entry() */
int
oref_resolve_new_stobj(
	invt_oref_t	*invidx,
	bool_t		firstentry)
{
	invt_entry_t   	ent;
	int	      	fd;
	off64_t 	hoff;
	invt_oref_t	*stobj;
	inv_idbtoken_t	tok;

	assert(OREF_ISLOCKED(invidx));

	memset ( &ent, 0, sizeof( ent ) );
	stobj = calloc(1, sizeof(invt_oref_t));
	OREF_SET_CHILD(invidx, stobj);
	
	/* initialize the start and end times to be the same */
	ent.ie_timeperiod.tp_start = ent.ie_timeperiod.tp_end = (time32_t)0;
	stobj_makefname( ent.ie_filename );

	if ( firstentry ) {
		invt_counter_t *cnt;
		cnt = malloc(sizeof(invt_counter_t));

		cnt->ic_maxnum = INVT_MAX_INVINDICES;
		cnt->ic_curnum = 1;
		cnt->ic_vernum = INV_VERSION;
		
		fd = stobj_create( ent.ie_filename );
		if ( fd < 0 ) {
			free(cnt);
			return INV_ERR;
		}
		stobj->fd = fd;
		OREF_SET_COUNTERS(invidx, cnt);
		OREF_SET_ENTRIES(invidx, &ent);
		if ((oref_sync(invidx, INVT_RES_COUNTERS) == INV_ERR)
		    ||
		    (oref_sync(invidx, INVT_RES_ENTRIES) == INV_ERR))
			return INV_ERR;
	}
	else {
		if (oref_resolve_upto(invidx, INVT_RES_COUNTERS) == INV_ERR)
			return INV_ERR;
		
		/* create the new storage object */
		fd = stobj_create( ent.ie_filename );
		if ( fd < 0 ) {
			return -1;
		}

		stobj->fd = fd;
		OREF_CNT_CURNUM(invidx)++;
		
		if ((oref_sync(invidx, INVT_RES_COUNTERS) == INV_ERR)
		    ||
		    (oref_sync_append(invidx, INVT_RES_ENTRIES, 
				      &ent, sizeof(ent)) == INV_ERR))
			return INV_ERR;
		
	}
	tok = get_token( invfd, fd );
	tok->d_invindex_off = IDX_HDR_OFFSET(OREF_CNT_CURNUM(invidx) - 1);
	tok->d_update_flag |= NEW_INVINDEX;
	
	OREF_SET_TOKEN(invidx, tok);
	return INV_OK;
	
}


