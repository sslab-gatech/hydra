/*
 * Copyright (c) 2000-2001 Silicon Graphics, Inc.  All Rights Reserved.
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
#ifndef _INV_OREF_H__
#define _INV_OREF_H__

#include "inv_priv.h"


#define	INVT_OTYPE_INVIDX	0x0001
#define	INVT_OTYPE_FSTAB	0x0002
#define	INVT_OTYPE_STOBJ	0x0004

#define INVT_OTYPE_MASK		INVT_OTYPE_INVIDX | INVT_OTYPE_FSTAB | \
                                INVT_OTYPE_STOBJ

#define INVT_RES_COUNTERS	0x0010
#define INVT_RES_ENTRIES	0x0020

#define INVT_STOBJ_RES_HDRS	0x0040
#define INVT_STOBJ_RES_SESSIONS	0x0080
#define INVT_STOBJ_RES_STRMS	0x0100
#define INVT_STOBJ_RES_MFILES	0x0200

#define INVT_RES_MASK		0x0FF0

/* Indicates if either its child or its parent has been resolved.
   Only one kin per object can exist. i.e. a child cant have children.*/
#define INVT_RES_CHILD		0x1000
#define INVT_RES_PARENT		0x2000
#define INVT_RES_KIN		INVT_RES_PARENT | INVT_RES_CHILD



typedef uint32_t	invt_objtype_t;

#define INVT_LOCKED		0x0001

typedef struct invt_oref {
	int				 fd;  

	/* counters - contains curnum, maxnum, etc */
	union {
		invt_sescounter_t 	 *sescnt;
		invt_counter_t		 *cnt;
	} oref_cnt_u;
	
	/* entries pointing at other invt objects */
	union {
		invt_entry_t 		 *ent;
		invt_fstab_t		 *fstabent;
		struct {
			invt_seshdr_t	 *hdr;
			invt_session_t	 *ses;
			invt_stream_t	 *strm;
			invt_mediafile_t *mfile;
		} eu_sessent;
	} oref_ent_u;

	union {
		struct invt_oref	 *invidx;      	/* parent */
		struct invt_oref	 *stobj;	/* child */
	} oref_kin_u;

	/* indicates level of depth this has been resolved to */
	invt_objtype_t			 type;
	int			 lockflag;
	void				 *token;
} invt_oref_t;

#define	eu_hdr		oref_ent_u.eu_sessent.hdr
#define	eu_ses		oref_ent_u.eu_sessent.ses
#define	eu_strm		oref_ent_u.eu_sessent.strm
#define	eu_mfile	oref_ent_u.eu_sessent.mfile

#define cu_cnt		oref_cnt_u.cnt
#define cu_sescnt	oref_sescnt_u.sescnt

#define ku_invidx	oref_kin_u.invidx
#define ku_stobj	oref_kin_u.stobj

/*
 *	See if the object has been resolved to the given level. 
 */

#define OREF_ISRESOLVED(oref, to_this_type) \
	((oref)->type & (to_this_type))

#define OREF_INIT(oref) \
        { bzero((oref), sizeof(invt_oref_t)); \
	  (oref)->fd = -1; }

#define OREF_DESTROY(oref) \
        { if (OREF_ISRESOLVED(oref, INVT_RES_CHILD))\
		  _oref_free((oref)->ku_stobj, BOOL_TRUE); \
	  else if (OREF_ISRESOLVED(oref, INVT_RES_PARENT)) \
		  _oref_free((oref)->ku_invidx, BOOL_TRUE); \
          _oref_free(oref, BOOL_FALSE); }
	  
#define OREF_COPY(src_oref, dst_oref) \
	(bcopy((src_oref), (dst_oref), sizeof(invt_oref_t)))



/*
 * For locking/unlocking orefs - mode { LOCK_SH, LOCK_EX, LOCK_UN }
 */

#define OREF_LOCKMODE_EQL(oref, mode) ((oref)->lockflag == mode )
#define OREF_ISLOCKED(oref)	      ((oref)->lockflag == 0 ||
				       (oref)->lockflag == LOCK_UN)

#define OREF_LOCK(oref, mode) \
        { if (! OREF_LOCKMODE_EQL(oref,mode)) \
	      { INVLOCK((oref)->fd, mode); \
		(oref)->lockflag = mode; } \
	}
#define OREF_UNLOCK(oref)	OREF_LOCK(oref, LOCK_UN)

#define OREF_INVIDX_LOCK(oref, mode) \
        { if (OREF_ISRESOLVED(oref, INVT_OTYPE_INVIDX)) \
		OREF_LOCK(oref, mode); \
        else if (OREF_ISRESOLVED(oref, INVT_OTYPE_STOBJ)) \
		OREF_LOCK((oref)->invidx, mode); }

#define OREF_STOBJ_LOCK(oref, mode) \
        { if (OREF_ISRESOLVED(oref, INVT_OTYPE_STOBJ)) \
		OREF_LOCK(oref, mode); \
        else if (OREF_ISRESOLVED(oref, INVT_OTYPE_INVIDX)) \
		OREF_LOCK((oref)->stobj, mode); }




#define OREF_SET_TYPE(oref, type) \
	{ (oref)->type |= (type); }

#define OREF_UNSET_TYPE(oref, type) \
        { (oref)->type &= ~(type); }



#define OREF_SET_TOKEN(oref, tok) \
        { (oref)->token = tok; }

#define OREF_SET_CNT(oref, cnt) \
	{ assert (OREF_ISRESOLVED(oref, INVT_OTYPE_MASK)); \
	  ((oref)->type & INVT_OTYPE_STOBJ) ? \
	  (oref)->cu_sescnt = (cnt): (oref)->cu_cnt = (cnt); \
	  (oref)->type |= INVT_RES_COUNTERS; }

#define OREF_SET_ENTRIES(oref, ents) \
        { assert ((oref)->type & (INVT_OTYPE_INVIDX | INVT_OTYPE_FSTAB));\
	  assert ((oref)->type & INVT_RES_COUNTERS); \
	  ((oref)->type & INVT_OTYPE_INVIDX) ? \
	  (oref)->eu_ent = ents : (oref)->eu_fstabent = ents; \
	  (oref)->type |= INVT_RES_ENTRIES; }

#define OREF_SET_HDRS(oref, hdrs) \
        {  assert ((oref)->type & INVT_OTYPE_STOBJ); \
	   assert ((oref)->type & INVT_RES_COUNTERS); \
	   (oref)->eu_hdr = hdrs; \
	   (oref)->type |= INVT_STOBJ_RES_HDRS; }

#define OREF_SET_SESSIONS(oref, ses) \
        {  assert ((oref)->type & INVT_OTYPE_STOBJ); \
	   assert ((oref)->type & INVT_RES_COUNTERS); \
	   (oref)->eu_ses = ses; \
	   (oref)->type |= INVT_STOBJ_RES_SESSIONS; }

#define OREF_SET_STRMS(oref, strms) \
        {  assert ((oref)->type & INVT_OTYPE_STOBJ); \
	   assert ((oref)->type & INVT_RES_COUNTERS); \
	   (oref)->eu_strm = strms; \
	   (oref)->type |= INVT_STOBJ_RES_STRMS; }

#define OREF_SET_CHILD(oref, stobjref) \
        {  assert (! OREF_ISRESOLVED(oref, INVT_RES_KIN)); \
	   (oref)->ku_child = stobjref; \
	   (oref)->type |= INVT_RES_CHILD; }

#define OREF_SET_PARENT(oref, invidxref) \
        {  assert (! OREF_ISRESOLVED(oref, INVT_RES_KIN)); \
	   (oref)->ku_parent = invidxref; \
	   (oref)->type |= INVT_RES_PARENT; }

#define OREF_UNRESOLVE_CHILD(oref) \
        {  assert (OREF_ISRESOLVED(oref, INVT_RES_CHILD)); \
	   close((oref)->ku_child->fd); \
	   OREF_DESTROY((oref)->ku_child); \
	   (oref)->ku_child = 0; \
	   OREF_UNSET_TYPE(oref, INVT_RES_CHILD); }

#define OREF_TYPE(oref) ((oref)->type)

#define OREF_CNT_CURNUM(oref) \
        ((oref)->type & INVT_OTYPE_STOBJ) ? \
	 (oref)->cu_sescnt->ic_curnum : (oref)->cu_cnt->ic_curnum

#define OREF_CNT_MAXNUM(oref) \
        ((oref)->type & INVT_OTYPE_STOBJ) ? \
	 (oref)->cu_sescnt->ic_maxnum : (oref)->cu_cnt->ic_maxnum

#define OREF_ENTRIES(oref) \
        ((oref)->type & INVT_OTYPE_INVIDX) ? \
	 (oref)->eu_ent : (oref)->eu_fstabent

#define OREF_HDRS(oref) 	((oref)->eu_hdr)
#define OREF_SESSIONS(oref) 	((oref)->eu_ses)
#define OREF_STRMS(oref) 	((oref)->eu_strm)
#define OREF_MEDIAFILES(oref)	((oref)->eu_mfile)
#define OREF_TOKEN(oref)	((oref)->token)

/*----------------------------------------------------------------------*/
/* Function Prototypes                                                  */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

int
oref_resolve(
	invt_oref_t	*invidx,
	inv_predicate_t bywhat,
	void 		*pred);

int
oref_resolve_upto(
	invt_oref_t 	*obj, 
	invt_objtype_t	type);

int
oref_resolve_entries(
	invt_oref_t 	*obj);

int
oref_resolve_counters(
	invt_oref_t 	*obj);

void
_oref_free(
	invt_oref_t 	*obj, 
	bool_t 		freethis);























#ifdef NOTDEF

extern int xlv_oref_resolve (
	xlv_oref_t *oref, xlv_tab_vol_entry_t *tab_vol_entry);

extern int xlv_oref_resolve_from_list (
	xlv_oref_t *oref, xlv_oref_t *oref_list);


/*
 *	Prints out the object referenced by an oref.
 */

extern void xlv_oref_print (xlv_oref_t *oref, int p_mode);

/*
 *	An iterator that applies a user-supplied function and arg
 *	to every volume element that makes up an object.
 *	The user-supplied function returns 0 to continue with the
 *	iteration. Return 1 to stop.
 */
typedef int (*XLV_OREF_PF) (xlv_oref_t *, void *);

extern void xlv_for_each_ve_in_obj (
        xlv_oref_t	*oref,
        XLV_OREF_PF	operation,
        void		*arg);

extern void xlv_for_each_ve_in_vol (
        xlv_oref_t	*oref,
        XLV_OREF_PF	operation,
        void		*arg);

extern int xlv_for_each_ve_in_subvol (
        xlv_oref_t	*oref,
        XLV_OREF_PF	operation,
        void		*arg);

extern int xlv_for_each_ve_in_plex (
        xlv_oref_t	*oref,
        XLV_OREF_PF	operation,
        void		*arg);


/*
 *	Returns 1 if the oref is a null reference; i.e., it has just
 *	been XLV_OREF_INIT'ed and no new fields have been set.
 */
extern int xlv_oref_is_null (xlv_oref_t *oref);
#endif /* NOTDEF */


#endif  /* _INV_OREF_H_ */
