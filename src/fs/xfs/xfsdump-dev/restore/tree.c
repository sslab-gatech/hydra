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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/param.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <utime.h>
#include <limits.h>
#include <time.h>
#include <xfs/handle.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>
#include <xfs/xfs.h>

#include "config.h"

#include "types.h"
#include "exit.h"
#include "cldmgr.h"
#include "path.h"
#include "openutil.h"
#include "getopt.h"
#include "stream.h"
#include "mlog.h"
#include "dlog.h"
#include "global.h"
#include "drive.h"
#include "media.h"
#include "content.h"
#include "content_inode.h"
#include "inomap.h"
#include "namreg.h"
#include "dirattr.h"
#include "bag.h"
#include "node.h"
#include "tree.h"
#include "libgen.h"
#include "mmap.h"
#include "exit.h"

/* structure definitions used locally ****************************************/

/* name of persistent state file 
 */
#define PERS_NAME	"tree"

/* orphanage specifics. ino must be otherwise unused in the dump source fs.
 * zero works.
 */
#define ORPH_INO	0
#define ORPH_NAME	"orphanage"


/* VM budgeting - give hash array one sixteenth, rest goes to node array
 */
#define HASHSZ_PERVM	16

/* reserve the first page for persistent state
 */
struct treePersStorage {
	xfs_ino_t p_rootino;
		/* ino of root
		 */
	nh_t p_rooth;
		/* handle of root node
		 */
	nh_t p_orphh;
		/* handle to orphanage node
		 */
	size64_t p_hashsz;
		/* size of hash array (private to hash abstraction)
		 */
	size_t p_hashmask;
		/* hash mask (private to hash abstraction)
		 */
	bool_t p_ownerpr;
		/* restore directory owner/group attributes
		 */
	bool_t p_fullpr;
		/* restoring a full level 0 non-resumed dump (can skip
		 * some steps)
		 */
	bool_t p_ignoreorphpr;
		/* set if positive subtree or interactive
		 */
	bool_t p_restoredmpr;
		/* restore DMI event settings
		 */
	bool_t p_truncategenpr;
		/* truncate inode generation number (for compatibility
		 * with xfsdump format 2 and earlier)
		 */
};

typedef struct treePersStorage treepers_t;

#define PERSSZ	perssz


/* interactive dialog transient state
 */
#define INTER_ARGMAX	10	/* max number of args to interactive cmds */
struct inter {
	size_t i_argc;
	char *i_argv[ INTER_ARGMAX ];
	nh_t i_cwdh;
	char i_name[ NAME_MAX + 1 ];
};

typedef struct inter inter_t;

/* transient state
 */
struct tran {
	bool_t t_toconlypr;
		/* just display table of contents; don't restore files
		 */
	char *t_hkdir;
		/* full absolute pathname of housekeeping directory
		 */
	char *t_dstdir;
		/* full absolute pathname of destination directory
		 */
	bool_t t_dstdirisxfspr;
		/* destination directory is an xfs filesystem; xfs-specific
		 * calls can be made when needed.
		 */
	char *t_orphdir;
		/* full absolute pathname of orphanage directory
		 */
	char *t_hksubtree;
		/* if non-NULL, is path of hkdir relative to dstdir.
		 * don't restore there.
		 */
	int t_persfd;
		/* file descriptor of the persistent state file
		 */
	nh_t *t_hashp;
		/* pointer to mapped hash array (private to hash abstraction)
		 */
	char t_namebuf[ NAME_MAX + 1 ];
		/* to hold names temporarily retrieved from name registry
		 */
	inter_t t_inter;
		/* context for interactive subtree selection
		 */
};

typedef struct tran tran_t;


/* node structure. each node represents a directory entry
 */
#define NODESZ	56

struct node {
	xfs_ino_t n_ino;	/* 8  8 ino */
	nrh_t n_nrh;		/* 8 16 handle to name in name registry */
	dah_t n_dah;		/* 4 20 handle to directory attributes */
	nh_t n_hashh;		/* 4 24 hash array */
	nh_t n_parh;		/* 4 28 parent */
	nh_t n_sibh;		/* 4 32 sibling list */
	nh_t n_sibprevh;	/* 4 36 prev sibling list - dbl link list */
	nh_t n_cldh;		/* 4 40 children list */
	nh_t n_lnkh;		/* 4 44 hard link list */
	gen_t n_gen;		/* 4 48 generation count mod 0x10000 */
	u_char_t n_flags;	/* 1 49 action and state flags */
	u_char_t n_nodehkbyte;	/* 1 50 given to node abstraction */
	char n_pad[6];		/* 6 56 */
};

typedef struct node node_t;

#define NF_REAL		( 1 << 0 )
	/* set when the corresponding file/dir has been created in
	 * the restore destination.
	 */
#define NF_SUBTREE	( 1 << 1 )
	/* marks nodes in the selected subtrees.
	 */
#define NF_REFED	( 1 << 2 )
	/* indicates node is still referenced according to incremental/resumed
	 * dump. used to detect dirents no longer used. prior to restoring
	 * a dump session, this flag is cleared in all nodes. during the dirent
	 * restoral, it is set. it may also be set during the adjustment
	 * for referenced but undumped directories. NOTE: nodes in the
	 * orphanage NEVER have this flag set.
	 */
#define NF_WRITTEN	( 1 << 3 )
	/* set as soon as a non-dir node restore is begun. allows
	 * overwrite inhibit options to work with segmented files
	 */
#define NF_ISDIR	( 1 << 4 )
	/* indicates this node is a directory. set when a directory is taken
	 * from the dirdump.
	 */
#define NF_DUMPEDDIR	( 1 << 5 )
	/* indicates this node is a directory present in the current dirdump.
	 * at beginning of session, this flag is cleared in all nodes.
	 * then as each directory dump is read from media, the flag
	 * is set from the corresponding node. this allows adjustments to
	 * the NF_REFED flag: if a directory was not dumped, either it no
	 * longer exists or it has not changed. if it is referenced, we assume
	 * it exists, in which case if it is not dumped then all of its entries
	 * are referenced as well.
	 */ 
#define NF_NEWORPH	( 1 << 6 )
	/* cleared from all nodes in the orphanage before a dump is applied.
	 * set if a dir is seen in the dirdump but no node exists for it.
	 * cleared if that dir is adopted subsequently during the dirdump.
	 * set if a nondir is seen in the nondir dump but no node exists for
	 * it. in either case a node is created and placed in the orphanage.
	 * during rmdir/unlink processing, nodes so marked are left alone.
	 * since the flag is cleared at the beginning of the next increment,
	 * old orphans had better be adopted, otherwise they will be unlinked.
	 */

/* link list iterator context
 */
struct link_iter_context {
	nh_t li_headh;          /* head of hard link list */
	nh_t li_prevh;          /* next to last node returned by _next() */
	nh_t li_lasth;          /* last node returned by _next() */
	bool_t li_donepr;       /* set as soon as last.next null */
};
typedef struct link_iter_context link_iter_context_t;

/* used for caching parent pathname from previous Node2path result
 */
struct path_cache {
	nh_t nh;
	int len;
	char buf[MAXPATHLEN];
};
typedef struct path_cache path_cache_t;

/* declarations of externally defined global symbols *************************/

extern void usage( void );
extern size_t pgsz;
extern size_t pgmask;
extern bool_t restore_rootdir_permissions;

/* forward declarations of locally defined static functions ******************/

static nh_t Node_alloc( xfs_ino_t ino,
			gen_t gen,
			nrh_t nrh,
			dah_t dah,
			size_t flags );
static void Node_free( nh_t *nhp );
static node_t * Node_map( nh_t nh );
static void Node_unmap( nh_t nh, node_t **npp );
static int Node2path_recurse( nh_t nh, char *buf,
				   int bufsz, int level );
static void adopt( nh_t parh, nh_t cldh, nrh_t nrh );
static nrh_t disown( nh_t cldh );
static void selsubtree( nh_t nh, bool_t sensepr );
static void selsubtree_recurse_down( nh_t nh, bool_t sensepr );
static nh_t link_hardh( xfs_ino_t ino, gen_t gen );
static nh_t link_nexth( nh_t nh );
static nh_t link_matchh( nh_t hardh, nh_t parh, char *name );
static void link_in( nh_t nh );
static void link_out( nh_t nh );
static void link_headiter( bool_t ( * cbfp )( void *contextp, nh_t hardh ),
			   void *contextp );
static void link_iter_init( link_iter_context_t *link_iter_contextp,
			    nh_t hardheadh );
static nh_t link_iter_next( link_iter_context_t *link_iter_contextp );
void link_iter_unlink( link_iter_context_t *link_iter_contextp, nh_t nh );
static bool_t hash_init( size64_t vmsz,
			 size64_t dircnt,
			 size64_t nondircnt,
			 char *perspath );
static bool_t hash_sync( char *perspath );
static inline size_t hash_val(xfs_ino_t ino, size_t hashmask);
static void hash_in( nh_t nh );
static void hash_out( nh_t nh );
static nh_t hash_find( xfs_ino_t ino, gen_t gen );
static void hash_iter( bool_t ( * cbfp )( void *contextp, nh_t hashh ),
		       void *contextp );
static void setdirattr( dah_t dah, char *path );
static bool_t tsi_walkpath( char *arg, nh_t rooth, nh_t cwdh,
			    dlog_pcbp_t pcb, void *pctxp,
			    nh_t *namedhp, nh_t *parhp, nh_t *cldhp,
			    xfs_ino_t *inop, bool_t *isdirprp, bool_t *isselpr );
static bool_t Node2path( nh_t nh, char *path, char *errmsg );
static bool_t tree_setattr_recurse( nh_t parh, char *path );
static void tsi_cmd_pwd_recurse( void *ctxp,
				 dlog_pcbp_t pcb,
				 void *pctxp,
				 nh_t nh );
static int mkdir_r(char *path);
#ifdef TREE_CHK
static bool_t Node_chk( nh_t nh, nh_t *nexthashhp, nh_t *nextlnkhp );
static bool_t tree_chk2( void );
#endif /* TREE_CHK */

/* definition of locally defined global variables ****************************/


/* definition of locally defined static variables *****************************/

static treepers_t *persp = 0;
static tran_t *tranp = 0;
static char *persname = PERS_NAME;
static char *orphname = ORPH_NAME;
static xfs_ino_t orphino = ORPH_INO;


/* definition of locally defined global functions ****************************/

/* ARGSUSED */
bool_t
tree_init( char *hkdir,
	   char *dstdir,
	   bool_t toconlypr,
	   bool_t ownerpr,
	   xfs_ino_t rootino,
	   xfs_ino_t firstino,
	   xfs_ino_t lastino,
	   size64_t dircnt,
	   size64_t nondircnt,
	   size64_t vmsz,
	   bool_t fullpr,
	   bool_t restoredmpr,
	   bool_t dstdirisxfspr,
	   uint32_t dumpformat,
	   bool_t truncategenpr )
{
	off64_t nodeoff;
	char *perspath;
	bool_t ok;
	int rval;

	/* sanity checks
	 */
	assert( ! ( PERSSZ % pgsz ));
	assert( sizeof( persp ) <= PERSSZ );
	assert( sizeof( node_t ) <= NODESZ );
	assert( ! persp );
	assert( ! tranp );

	/* allocate transient state
	 */
	tranp = ( tran_t * )calloc( 1, sizeof( tran_t ));
	assert( tranp );

	tranp->t_toconlypr = toconlypr;
	tranp->t_hkdir = hkdir;
	tranp->t_dstdir = dstdir;
	tranp->t_dstdirisxfspr = dstdirisxfspr;

	/* allocate a char string buffer to hold the abs. pathname
	 * of the orphanage directory file. load it with the pathname.
	 */
	tranp->t_orphdir = open_pathalloc( tranp->t_dstdir,
					   orphname,
					   0 );

	/* determine if housekeeping dir is within the destination.
	 * generate a relative path containing the difference,
	 * else null. will not restore into there.
	 */
	if ( strcmp( dstdir, "." )) {
		tranp->t_hksubtree = path_diff( hkdir, dstdir );
	} else {
		tranp->t_hksubtree = 0;
	}

	/* create an orphanage, if it already exists, complain.
	 * not needed if just a table-of-contents restore.
	 */
	if ( ! tranp->t_toconlypr ) {
		rval = mkdir( tranp->t_orphdir, S_IRWXU );
		if ( rval ) {
			if ( errno == EEXIST ) {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
				      "%s already exists: "
				      "rm -rf prior to initating restore\n"),
				      tranp->t_orphdir );
			} else {
				mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
				      "unable to create %s: %s\n"),
				      tranp->t_orphdir,
				      strerror( errno ));
			}
			return BOOL_FALSE;
		}
	}

	/* build a full pathname to pers. state file
	 */
	perspath = open_pathalloc( tranp->t_hkdir, persname, 0 );

	/* create the persistent state file
	 */
	( void )unlink( perspath );
	tranp->t_persfd = open( perspath,
				O_RDWR | O_CREAT,
				S_IRUSR | S_IWUSR );
	if ( tranp->t_persfd < 0 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
		      "could not open %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* mmap the persistent state
	 */
	assert( ! ( PERSSZ % pgsz ));
	persp = ( treepers_t * ) mmap_autogrow(
				     PERSSZ,
				     tranp->t_persfd,
				     ( off64_t )0 );
	if ( persp == ( treepers_t * )-1 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
		      "unable to map %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* create the hash abstraction. it will map more of the
	 * persistent state file.
	 */
	ok = hash_init( vmsz / HASHSZ_PERVM, dircnt, nondircnt, perspath );
	if ( ! ok ) {
		return BOOL_FALSE;
	}

	/* initialize the node abstraction. let it's use of backing store
	 * begin immediately after the hash abstraction. give it the remainder
	 * of vm.
	 */
	assert( persp->p_hashsz <= ( size64_t )( OFF64MAX - ( off64_t )PERSSZ));
	nodeoff = ( off64_t )PERSSZ + ( off64_t )persp->p_hashsz;
	assert( vmsz > ( size64_t )nodeoff );
	ok = node_init( tranp->t_persfd, 
		        nodeoff,
		        NODESZ,
			 ( ix_t )offsetofmember( node_t, n_nodehkbyte ),
		        sizeof( size64_t ), /* node alignment */
		        vmsz - ( size64_t )nodeoff,
			dircnt + nondircnt );
	if ( ! ok ) {
		return BOOL_FALSE;
	}

	/* extract the root ino and allocate a node for
	 * the root and for the orphanage. place both nodes
	 * in the hash list. make the orphanage a child of
	 * root, and indicate he is real.
	 *
	 * Note that we assume that the root inode always
	 * has a generation count of zero - which is true.
	 */
	persp->p_rootino = rootino;
	persp->p_rooth = Node_alloc( rootino,
				     0, /* gen cnt */
				     NRH_NULL,
				     DAH_NULL,
				     NF_ISDIR | NF_REAL );
	if (persp->p_rooth == NH_NULL)
		return BOOL_FALSE;
	link_in( persp->p_rooth );
	persp->p_orphh = Node_alloc( orphino,
				     0, /* gen cnt */
				     namreg_add( orphname, strlen( orphname )),
				     DAH_NULL,
				     NF_ISDIR | NF_REAL );
	if (persp->p_orphh == NH_NULL)
		return BOOL_FALSE;
	link_in( persp->p_orphh );
	adopt( persp->p_rooth, persp->p_orphh, NRH_NULL );

	/* record if we should attempt to restore original owner/group
	 */
	persp->p_ownerpr = ownerpr;

	/* record if this is a full dump. can skip link processing if so
	 */
	persp->p_fullpr = fullpr;

	/* record if DMI event settings should be restored
	 */
	persp->p_restoredmpr = restoredmpr;

	/* record if truncated generation numbers are required
	 */
	if ( dumpformat < GLOBAL_HDR_VERSION_3 ) {
		persp->p_truncategenpr = BOOL_TRUE;
		mlog( MLOG_NORMAL | MLOG_DEBUG | MLOG_TREE, _(
		      "dump format version %u used truncated inode generation numbers\n"),
			dumpformat );
	} else if ( truncategenpr ) {
		persp->p_truncategenpr = BOOL_TRUE;
		mlog( MLOG_NORMAL | MLOG_DEBUG | MLOG_TREE, _(
		      "forcing use of truncated inode generation numbers\n"));
	} else {
		persp->p_truncategenpr = BOOL_FALSE;
	}

	return BOOL_TRUE;
}

bool_t
tree_sync( char *hkdir,
	   char *dstdir,
	   bool_t toconlypr,
	   bool_t fullpr,
	   bool_t dstdirisxfspr )
{
	off64_t nodeoff;
	char *perspath;
	bool_t ok;
	int rval;

	if ( persp ) {
		return BOOL_TRUE;
	}

	/* sanity checks
	 */
	assert( ! ( PERSSZ % pgsz ));
	assert( sizeof( persp ) <= PERSSZ );
	assert( sizeof( node_t ) <= NODESZ );
	assert( ! persp );
	assert( ! tranp );

	/* allocate transient state
	 */
	tranp = ( tran_t * )calloc( 1, sizeof( tran_t ));
	assert( tranp );

	tranp->t_toconlypr = toconlypr;
	tranp->t_hkdir = hkdir;
	tranp->t_dstdir = dstdir;
	tranp->t_dstdirisxfspr = dstdirisxfspr;

	/* allocate a char string buffer to hold the abs. pathname
	 * of the orphanage directory file. load it with the pathname.
	 */
	tranp->t_orphdir = open_pathalloc( tranp->t_dstdir,
					   orphname,
					   0 );

	/* determine if housekeeping dir is within the destination.
	 * generate a relative path containing the difference,
	 * else null. will not restore into there.
	 */
	if ( strcmp( dstdir, "." )) {
		tranp->t_hksubtree = path_diff( hkdir, dstdir );
	} else {
		tranp->t_hksubtree = 0;
	}

	/* re-create the orphanage (in case someone rmdir'ed it)
	 */
	rval = mkdir( tranp->t_orphdir, S_IRWXU );
	if ( rval && errno != EEXIST ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
		      "unable to recreate %s: %s\n"),
		      tranp->t_orphdir,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* build a full pathname to pers. state file
	 */
	perspath = open_pathalloc( tranp->t_hkdir, persname, 0 );

	/* re-open the persistent state file
	 */
	tranp->t_persfd = open( perspath, O_RDWR );
	if ( tranp->t_persfd < 0 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
		      "could not open %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* mmap the persistent state
	 */
	assert( ! ( PERSSZ % pgsz ));
	persp = ( treepers_t * ) mmap_autogrow(
				     PERSSZ,
				     tranp->t_persfd,
				     ( off64_t )0 );
	if ( persp == ( treepers_t * )-1 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
		      "unable to map %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* update the fullpr field of the persistent state to match
	 * the input of our caller.
	 */
	persp->p_fullpr = fullpr;

	/* regardless of the format of this dump, if the previously applied
	 * dump used truncated generation numbers, then we need to as well.
	 */
	if ( persp->p_truncategenpr ) {
		mlog( MLOG_NORMAL | MLOG_DEBUG | MLOG_TREE, _(
		      "using truncated inode generation numbers for "
		      "compatibility with previously applied restore\n") );
	}

	/* rsynchronize with the hash abstraction. it will map more of the
	 * persistent state file.
	 */
	ok = hash_sync( perspath );
	if ( ! ok ) {
		return BOOL_FALSE;
	}

	/* synchronize with the node abstraction.
	 */
	assert( persp->p_hashsz <= ( size64_t )( OFF64MAX - ( off64_t )PERSSZ));
	nodeoff = ( off64_t )PERSSZ + ( off64_t )persp->p_hashsz;
	ok = node_sync( tranp->t_persfd, nodeoff );
	if ( ! ok ) {
		return BOOL_FALSE;
	}

	/* extract the root ino and allocate a node for
	 * the root and for the orphanage. place both nodes
	 * in the hash list. make the orphanage a child of
	 * root, and indicate he is real.
	 */
	return BOOL_TRUE;
}

bool_t
tree_check_dump_format( uint32_t dumpformat )
{
	if ( dumpformat < GLOBAL_HDR_VERSION_3 && !persp->p_truncategenpr ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
		      "encountered dump format %d after a "
		      "restore of format %d or newer\n"),
			dumpformat, GLOBAL_HDR_VERSION_3 );
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
		      "to restore this series of dumps, use the -%c "
		      "option on the first restore\n"),
			GETOPT_FMT2COMPAT );
		return BOOL_FALSE;
	}

	return BOOL_TRUE;
}

/* recursively descend the tree clearing REFED and DIRDUMPED and NEWORPH
 * flags. force the orphanage to be refed and dumped, so we won't try
 * to orphan it, and so things added to it won't look like they are
 * referenced during ref adj. also null dirattr handles, since they are
 * not retained across dump session restores.
 */
static void tree_marknoref_recurse( nh_t parh );

void
tree_marknoref( void )
{
	tree_marknoref_recurse( persp->p_rooth );

	{
		node_t *orphp;
		orphp = Node_map( persp->p_orphh );
		orphp->n_flags |= ( NF_REFED | NF_DUMPEDDIR );
		assert( orphp->n_dah == DAH_NULL );
		Node_unmap( persp->p_orphh, &orphp );
	}
}

static void
tree_marknoref_recurse( nh_t parh )
{
	nh_t cldh;

	{
		node_t *parp;
		parp = Node_map( parh );
		parp->n_flags &= ~( NF_REFED | NF_DUMPEDDIR | NF_NEWORPH );

		parp->n_dah = DAH_NULL;
		cldh = parp->n_cldh;
		Node_unmap( parh, &parp  );
	}
	while ( cldh != NH_NULL ) {
		node_t *cldp;
		nh_t nextcldh;
		tree_marknoref_recurse( cldh ); /* RECURSION */
		cldp = Node_map( cldh );
		nextcldh = cldp->n_sibh;
		Node_unmap( cldh, &cldp  );
		cldh = nextcldh;
	}
}

void
tree_markallsubtree( bool_t sensepr )
{
	if ( ! sensepr ) {
		persp->p_ignoreorphpr = BOOL_TRUE;
	}
	selsubtree( persp->p_rooth, sensepr );
}

nh_t
tree_begindir( filehdr_t *fhdrp, dah_t *dahp )
{
	nh_t hardh;
	xfs_ino_t ino = fhdrp->fh_stat.bs_ino;
	gen_t gen = fhdrp->fh_stat.bs_gen;
	dah_t dah;

	if ( persp->p_truncategenpr ) {
		gen = BIGGEN2GEN( gen );
	}

	/* sanity check - orphino is supposed to be an unused ino!
	 */
	assert( ino != orphino );

	/* lookup head of hardlink list
	 */
	hardh = link_hardh( ino, gen );
	assert( ino != persp->p_rootino || hardh == persp->p_rooth );

	/* already present
	 */
	if ( hardh != NH_NULL ) {
		node_t *hardp;
		hardp = Node_map( hardh );
		if ( ! ( hardp->n_flags & NF_ISDIR )) {
			/* case 1: previously seen as dirent, now know is dir
			 */
			mlog( MLOG_TRACE | MLOG_TREE,
			      "directory %llu %u (%u): "
			      "upgrading to dir\n",
			      ino,
			      gen,
			      fhdrp->fh_stat.bs_gen );
			if ( ! tranp->t_toconlypr ) {
				assert( hardp->n_dah == DAH_NULL );
				hardp->n_dah = dirattr_add( fhdrp );
			}
		} else if ( ! tranp->t_toconlypr && hardp->n_dah == DAH_NULL ) {
			/* case 2: node is a dir, but had thrown away dirattr
			 */
			mlog( MLOG_TRACE | MLOG_TREE,
			      "directory %llu %u (%u): "
			      "updating\n",
			      ino,
			      gen,
			      fhdrp->fh_stat.bs_gen );
			hardp->n_dah = dirattr_add( fhdrp );
		} else {
			/* case 3: already has dirattr; must be restart
			 */
			mlog( MLOG_TRACE | MLOG_TREE,
			      "directory %llu %u (%u): "
			      "retaining\n",
			      ino,
			      gen,
			      fhdrp->fh_stat.bs_gen );
		}
		hardp->n_flags |= NF_ISDIR;
		hardp->n_flags |= NF_DUMPEDDIR;
		*dahp = hardp->n_dah;
		Node_unmap( hardh, &hardp );
	} else {
		/* case 4: first time seen
		 */
		mlog( MLOG_TRACE | MLOG_TREE,
		      "directory %llu %u (%u): "
		      "new\n",
		      ino,
		      gen,
		      fhdrp->fh_stat.bs_gen );
		if ( ! tranp->t_toconlypr ) {
			dah = dirattr_add( fhdrp );
		} else {
			dah = DAH_NULL;
		}
		hardh = Node_alloc( ino,
				    gen,
				    NRH_NULL,
				    dah,
				    NF_ISDIR | NF_NEWORPH );
		if (hardh == NH_NULL)
			return NH_NULL;
		link_in( hardh );
		adopt( persp->p_orphh, hardh, NRH_NULL );
		*dahp = dah;
	}

	return hardh;
}

rv_t
tree_addent( nh_t parh, xfs_ino_t ino, gen_t gen, char *name, size_t namelen )
{
	nh_t hardh;

	if ( persp->p_truncategenpr ) {
		gen = BIGGEN2GEN( gen );
	}

	/* sanity check - orphino is supposed to be an unused ino!
	 */
	assert( ino != orphino );

	/* don't allow entries named "orphanage" under root to be added
	 */
	if ( parh == persp->p_rooth && !strcmp( name, orphname )) {
		mlog( MLOG_DEBUG | MLOG_TREE,
		      "dirent %s %llu %u: "
		      "skipping (reserved)\n",
		      name,
		      ino,
		      gen );
		return RV_OK;
	}

	/* if the parent is null, just ignore
	 */
	if ( parh == NH_NULL ) {
		mlog( MLOG_DEBUG | MLOG_TREE,
		      "dirent %s %llu %u: "
		      "skipping (null parent)\n",
		      name,
		      ino,
		      gen );
		return RV_OK;
	}

	/* see if one or more links to this ino already exist.
	 */
	hardh = link_hardh( ino, gen );

	if ( hardh != NH_NULL ) {
		node_t *hardp;
		hardp = Node_map( hardh );
		if ( hardp->n_flags & NF_ISDIR ) {
			nh_t renameh;
			node_t *renamep;
			/* REFERENCED */
			int namebuflen; 

			hardp->n_flags |= NF_REFED;
			if ( hardp->n_parh == persp->p_orphh ) {
				/* dir now seen as entry
				 * if in orph but REAL, must be pending rename
				 */
				if ( ( hardp->n_flags & NF_REAL )
				     &&
				     hardp->n_lnkh == NH_NULL ) {
					hardp->n_lnkh = Node_alloc( ino,
								    gen,
								    NRH_NULL,
								    DAH_NULL,
								    0 );
					if (hardp->n_lnkh == NH_NULL)
						return RV_ERROR;
				}
				if ( hardp->n_lnkh != NH_NULL ) {
					assert( hardp->n_flags & NF_REAL );
					renameh = hardp->n_lnkh;
					renamep = Node_map( renameh );
					if ( renamep->n_parh == NH_NULL ) {
						mlog( MLOG_DEBUG | MLOG_TREE,
						      "dirent %s %llu %u: "
						      "adopting (dir par)\n",
						      name,
						      ino,
						      gen );
						renamep->n_parh = parh;
					}
					if ( renamep->n_parh != parh ) {
						mlog( MLOG_DEBUG | MLOG_TREE,
						      "dirent %s %llu %u: "
						      "re-adopting (dir par)\n",
						      name,
						      ino,
						      gen );
						renamep->n_parh = parh;
					}
					if ( renamep->n_nrh != NRH_NULL ) {
						namebuflen
						=
						namreg_get( renamep->n_nrh,
							    tranp->t_namebuf,
						    sizeof( tranp->t_namebuf ));
						assert( namebuflen > 0 );
						if ( strcmp( name,
							   tranp->t_namebuf )) {
							mlog( MLOG_DEBUG
							      |
							      MLOG_TREE,
							      "dirent %s "
							      "%llu %u: "
							      "re-adopting "
							      "(dir name): "
							      "was %s\n",
							      name,
							      ino,
							      gen,
							     tranp->t_namebuf );
							namreg_del(
							       renamep->n_nrh );
							renamep->n_nrh =
								       NRH_NULL;
						}
					}
					if ( renamep->n_nrh == NRH_NULL ) {
						renamep->n_nrh
						=
						namreg_add( name, namelen );
						renamep->n_parh = parh;
					}
					Node_unmap( renameh, &renamep );
				} else {
					nrh_t nrh;

					hardp->n_flags &= ~NF_NEWORPH;
					assert( hardp->n_nrh == NRH_NULL );
					assert( hardp->n_parh != NH_NULL );
					nrh = disown( hardh );
					assert( nrh == NRH_NULL );
					nrh = namreg_add( name, namelen );
					adopt( parh, hardh, nrh );
					mlog( MLOG_DEBUG | MLOG_TREE,
					      "dirent %s %llu %u: "
					      "updating (dir)\n",
					      name,
					      ino,
					      gen );
				}
			} else {
				assert( hardp->n_nrh != NRH_NULL );
				namebuflen
				=
				namreg_get( hardp->n_nrh,
					    tranp->t_namebuf,
					    sizeof( tranp->t_namebuf ));
				assert( namebuflen > 0 );
				if ( hardp->n_parh == parh
				     &&
				     ! strcmp( tranp->t_namebuf, name )) {
					/* dir seen as entry again
					 */
					if ( hardp->n_lnkh != NH_NULL ) {
						/* rescind rename
						 */
						mlog( MLOG_DEBUG | MLOG_TREE,
						      "dirent %s %llu %u: "
						      "rescind rename (dir)\n",
						      name,
						      ino,
						      gen );
						Node_free( &hardp->n_lnkh );
					} else {
						mlog( MLOG_DEBUG | MLOG_TREE,
						      "dirent %s %llu %u: "
						      "retaining (dir)\n",
						      name,
						      ino,
						      gen );
					}
				} else {
					/* dir rename
					 */
					nh_t renameh;
					node_t *renamep;
					if ( hardp->n_lnkh == NH_NULL ) {
						renameh = Node_alloc( ino,
								      gen,
								      NRH_NULL,
								      DAH_NULL,
								      0 );
						if (renameh == NH_NULL)
							return RV_ERROR;
						hardp->n_lnkh = renameh;
					} else {
						mlog( MLOG_DEBUG | MLOG_TREE,
						      "dirent %s %llu %u: "
						      "re-renaming\n",
						      name,
						      ino,
						      gen );
						renameh = hardp->n_lnkh;
						renamep = Node_map( renameh );
						renamep->n_parh = NH_NULL;
						if ( renamep->n_nrh
						     !=
						     NRH_NULL ) {
						   namreg_del( renamep->n_nrh );
						}
						renamep->n_nrh = NRH_NULL;
						Node_unmap( renameh, &renamep );
					}
					renamep = Node_map( renameh );
					assert( hardp->n_parh != NH_NULL );
					if ( hardp->n_parh != parh ) {
						/* different parent
						 */
						renamep->n_parh = parh;
						mlog( MLOG_DEBUG | MLOG_TREE,
						      "dirent %s %llu %u: "
						      "renaming (parent)\n",
						      name,
						      ino,
						      gen );
					}
					if ( strcmp( tranp->t_namebuf, name )) {
						/* different name
						 */
						renamep->n_nrh =
						    namreg_add( name, namelen );
						mlog( MLOG_DEBUG | MLOG_TREE,
						      "dirent %s %llu %u: "
						      "renaming (name)\n",
						      name,
						      ino,
						      gen );
					}
					Node_unmap( renameh, &renamep );
				}
			}
		} else {
			nh_t matchh;
			matchh = link_matchh( hardh, parh, name );
			if ( matchh != NH_NULL ) {
				/* entry already exists
				 */
				node_t *matchp;
				matchp = Node_map( matchh );
				matchp->n_flags |= NF_REFED;
				Node_unmap( matchh, &matchp );
				mlog( MLOG_DEBUG | MLOG_TREE,
				      "dirent %s %llu %u: "
				      "retaining (nondir)\n",
				      name,
				      ino,
				      gen );
			} else {
				/* case 5: new hard link
				 */
				nrh_t nrh;
				nh_t linkh;
				nrh = namreg_add( name, namelen );
				linkh = Node_alloc( ino,
						    gen,
						    NRH_NULL,
						    DAH_NULL,
						    NF_REFED );
				if (linkh == NH_NULL)
					return RV_ERROR;
				link_in( linkh );
				adopt( parh, linkh, nrh );
				mlog( MLOG_DEBUG | MLOG_TREE,
				      "dirent %s %llu %u: "
				      "adding (link)\n",
				      name,
				      ino,
				      gen );
			}
		}
		Node_unmap( hardh, &hardp );
	} else {
		/* case 6: new entry
		 */
		nrh_t nrh;
		nrh = namreg_add( name, namelen );
		hardh = Node_alloc( ino,
				    gen,
				    NRH_NULL,
				    DAH_NULL,
				    NF_REFED );
		if (hardh == NH_NULL)
			return RV_ERROR;
		link_in( hardh );
		adopt( parh, hardh, nrh );
		mlog( MLOG_DEBUG | MLOG_TREE,
		      "dirent %s %llu %u: "
		      "adding (new)\n",
		      name,
		      ino,
		      gen );
	}
	return RV_OK;
}

/* ARGSUSED */
void
tree_enddir( nh_t dirh )
{
}

bool_t
tree_subtree_parse( bool_t sensepr, char *path )
{
	nh_t namedh;
	nh_t parh;
	nh_t cldh;
	xfs_ino_t ino;
	bool_t isdirpr;
	bool_t isselpr;
	bool_t ok;

	/* walk the tree down this relative pathname from the root.
	 */
	ok = tsi_walkpath( path,
			   NH_NULL,
			   persp->p_rooth,
			   0,
			   0,
			   &namedh,
			   &parh,
			   &cldh,
			   &ino,
			   &isdirpr,
			   &isselpr );
	if ( ! ok ) {
		return BOOL_FALSE;
	}

	/* set or clear flag in this node and all of its children,
	 * and ajust parentage flags.
	 */
	selsubtree( namedh, sensepr );

	return BOOL_TRUE;
}

/* tree_post - called after the dirdump has been applied.
 * first phase is to eliminate all unreferenced dirents.
 * done by recursive depth-wise descent of the tree. on the way
 * up, unlink or orphan unreferenced nondirs, unlink unreferenced
 * dirs, orphan dirs to be renamed. if a dir is unreferenced, but
 * contains referenced dirents, orphan those dirents. orphan unreferenced
 * nondirs if they are the last link to an inode referenced but not real
 * somewhere else in the tree. next, make new directories. then rename
 * directories. finally, create hardlinks from orphanage.
 */
static bool_t noref_elim_recurse( nh_t parh,
				  nh_t cldh,
				  bool_t parrefpr,
				  char *path1,
				  char *path2 );

static bool_t mkdirs_recurse( nh_t parh,
			      nh_t cldh,
			      char *path );

static bool_t rename_dirs( nh_t cldh,
			   char *path1,
			   char *path2 );

static bool_t proc_hardlinks( char *path1,
			      char *path2 );

bool_t
tree_post( char *path1, char *path2 )
{
	node_t *rootp;
	node_t *orphp;
	nh_t cldh;
	bool_t ok;

	/* eliminate unreferenced dirents
	 */
	if ( ! persp->p_fullpr ) {
		mlog( MLOG_DEBUG | MLOG_TREE,
		      "eliminating unreferenced directory entries\n" );
		rootp = Node_map( persp->p_rooth );
		cldh = rootp->n_cldh;
		Node_unmap( persp->p_rooth, &rootp );
		ok = noref_elim_recurse( persp->p_rooth,
					 cldh,
					 BOOL_TRUE,
					 path1,
					 path2 );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
	}

	/* make new directories
	 */
	mlog( MLOG_DEBUG | MLOG_TREE,
	      "making new directories\n" );
	rootp = Node_map( persp->p_rooth );
	cldh = rootp->n_cldh;
	Node_unmap( persp->p_rooth, &rootp );
	ok = mkdirs_recurse( persp->p_rooth, cldh, path1 );
	if ( ! ok ) {
		return BOOL_FALSE;
	}

#ifdef TREE_CHK
	assert( tree_chk( ));
#endif /* TREE_CHK */

	/* rename directories
	 */
	if ( ! persp->p_fullpr ) {
		mlog( MLOG_DEBUG | MLOG_TREE,
		      "performing directory renames\n" );
		orphp = Node_map( persp->p_orphh );
		cldh = orphp->n_cldh;
		Node_unmap( persp->p_orphh, &orphp );
		ok = rename_dirs( cldh, path1, path2 );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
	}

#ifdef TREE_CHK
	assert( tree_chk( ));
#endif /* TREE_CHK */

	/* process hard links
	 */
	if ( ! persp->p_fullpr ) {
		mlog( MLOG_DEBUG | MLOG_TREE,
		      "processing hard links\n" );
		ok = proc_hardlinks( path1, path2 );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
	}

	return BOOL_TRUE;
}

/* ARGSUSED */
static bool_t
noref_elim_recurse( nh_t parh,
		    nh_t cldh,
		    bool_t parrefpr,
		    char *path1,
		    char *path2 )
{
	while ( cldh != NH_NULL ) {
		node_t *cldp;
		xfs_ino_t ino;
		gen_t gen;
		bool_t inorphanagepr;
		bool_t isdirpr;
		bool_t isrealpr;
		bool_t isrefpr;
		bool_t isrenamepr;
		nh_t renameh;
		nh_t grandcldh;
		nh_t nextcldh;
		int rval;
		bool_t ok;

		cldp = Node_map( cldh );
		ino = cldp->n_ino;
		gen = cldp->n_gen;
		inorphanagepr = cldp->n_parh == persp->p_orphh;
		isdirpr = ( cldp->n_flags & NF_ISDIR );
		isrealpr = ( cldp->n_flags & NF_REAL );
		isrefpr = ( cldp->n_flags & NF_REFED );
		isrenamepr = ( isdirpr && cldp->n_lnkh != NH_NULL );
		renameh = cldp->n_lnkh;
		grandcldh = cldp->n_cldh;
		nextcldh = cldp->n_sibh;

#ifdef TREE_DEBUG
		ok = Node2path( cldh, path1, _("noref debug") );
		mlog( MLOG_TRACE | MLOG_TREE,
		      "noref: %s: isdir = %d, isreal = %d, isref = %d, "
		      "isrenamedir = %d\n",
		      path1, isdirpr, isrealpr, isrefpr, isrenamepr );
#endif

		Node_unmap( cldh, &cldp );

		if ( isdirpr ) {
			bool_t ok;

			ok = noref_elim_recurse( cldh,
						 grandcldh,
						 isrefpr,
						 path1,
						 path2 );
						/* RECURSION */
			if ( ! ok ) {
				return BOOL_FALSE;
			}

			if ( inorphanagepr ) {
				cldh = nextcldh;
				continue;
			}

			if ( ! isrefpr ) {
				nrh_t nrh;

				assert( ! isrenamepr );
				if ( isrealpr ) {
					ok = Node2path( cldh, path1, _("rmdir") );
					if ( ! ok ) {
						cldh = nextcldh;
						continue;
					}
					mlog( MLOG_TRACE | MLOG_TREE,
					      "rmdir %s\n",
					      path1 );
					rval = rmdir( path1 );
					if ( rval ) {
						mlog( MLOG_NORMAL
						      |
						      MLOG_WARNING
						      |
						      MLOG_TREE, _(
						    "unable to rmdir %s: %s\n"),
						     path1,
						     strerror( errno ));
						cldh = nextcldh;
						continue;
					}
				}
				nrh = disown( cldh );
				assert( nrh != NRH_NULL );
				namreg_del( nrh );
				link_out( cldh );
				Node_free( &cldh );
			}

			if ( isrenamepr ) {
				nrh_t nrh;
				node_t *renamep;

				assert( isrefpr );
				assert( isrealpr );
				ok = Node2path( cldh,
						path1,
						_("tmp dir rename src") );
				if ( ! ok ) {
					cldh = nextcldh;
					continue;
				}
				nrh = disown( cldh );
				assert( nrh != NRH_NULL );
				adopt( persp->p_orphh, cldh, NRH_NULL );
				ok = Node2path( cldh,
						path2,
						_("tmp dir rename dst") );
				if ( ! ok ) {
					/* REFERENCED */
					nrh_t dummynrh;
					dummynrh = disown( cldh );
					assert( dummynrh == NRH_NULL );
					adopt( parh, cldh, nrh );
					cldh = nextcldh;
					continue;
				}
				mlog( MLOG_TRACE | MLOG_TREE,
				      "rename dir %s to %s\n",
				      path1,
				      path2 );
				rval = rename( path1, path2 );
				if ( rval ) {
					/* REFERENCED */
					nrh_t dummynrh;
					mlog( MLOG_NORMAL | MLOG_WARNING, _(
					      "unable to rename dir %s "
					      "to dir %s: %s\n"),
					      path1,
					      path2,
					      strerror( errno ));
					dummynrh = disown( cldh );
					assert( dummynrh == NRH_NULL );
					adopt( parh, cldh, nrh );
					cldh = nextcldh;
					continue;
				}
				cldp = Node_map( cldh );
				renamep = Node_map( renameh );
				if ( renamep->n_nrh == NRH_NULL ) {
					renamep->n_nrh = nrh;
				} else {
					namreg_del( nrh );
				}
				if ( renamep->n_parh == NH_NULL ) {
					renamep->n_parh = parh;
				}
				cldp->n_flags |= NF_NEWORPH;
				Node_unmap( renameh, &renamep );
				Node_unmap( cldh, &cldp );
			}
		} else  {
			/* determine if we can unlink this node.
			 * if its not real, and not refed, simple.
			 * if real and not refed and there is at least
			 * one unreal refed node and no other real
			 * nodes around, must put this one in orphanage
			 * rather than unlinking it.
			 */
			bool_t mustorphpr;
			bool_t canunlinkpr;

			if ( inorphanagepr ) {
				cldh = nextcldh;
				continue;
			}

			mustorphpr = BOOL_FALSE;
			canunlinkpr = ! isrefpr && ! isrealpr;
			if ( ! isrefpr && isrealpr ) {
				nh_t hardh;
				bool_t neededpr;
				hardh = link_hardh( ino, gen );
				assert( hardh != NH_NULL );
				canunlinkpr = BOOL_FALSE;
				neededpr = BOOL_FALSE;
				/* tes@sgi.com:
				 * This loop seems to assume that
				 * REAL files come before NON-REALs
				 * so that we will break out first
				 * if we find a REAL file.
				 */
				while ( hardh != NH_NULL ) {
					node_t *hardp;
					bool_t hardisrefpr;
					bool_t hardisrealpr;
					nh_t nexthardh;

					hardp = Node_map( hardh );
					hardisrefpr = hardp->n_flags & NF_REFED;
					hardisrealpr = hardp->n_flags & NF_REAL;
					nexthardh = hardp->n_lnkh;
					Node_unmap( hardh, &hardp );
					if ( hardh != cldh && hardisrealpr ) {
						break;
					}
					if ( hardisrefpr && ! hardisrealpr ) {
						neededpr = BOOL_TRUE;
					}
					hardh = nexthardh;
				}
				if ( neededpr ) {
					mustorphpr = BOOL_TRUE;
				}
				else {
					canunlinkpr = BOOL_TRUE;
				}
			}

			if ( mustorphpr ) {
				/* rename file to orphanage */
				nrh_t nrh;
				assert( ! canunlinkpr );
				ok = Node2path( cldh,
						path1,
						_("tmp nondir rename src") );
				if ( ! ok ) {
					cldh = nextcldh;
					continue;
				}
				nrh = disown( cldh );
				assert( nrh != NRH_NULL );
				adopt( persp->p_orphh, cldh, NRH_NULL );
				ok = Node2path( cldh,
						path2,
						_("tmp nondir rename dst") );
				if ( ! ok ) {
					/* REFERENCED */
					nrh_t dummynrh;
					dummynrh = disown( cldh );
					assert( dummynrh == NRH_NULL );
					adopt( parh, cldh, nrh );
					cldh = nextcldh;
					continue;
				}
				mlog( MLOG_TRACE | MLOG_TREE,
				      "rename nondir %s to %s\n",
				      path1,
				      path2 );
				rval = rename( path1, path2 );
				if ( rval ) {
					/* REFERENCED */
					nrh_t dummynrh;
					mlog( MLOG_NORMAL | MLOG_WARNING, _(
					      "unable to rename nondir %s "
					      "to nondir %s: %s\n"),
					      path1,
					      path2,
					      strerror( errno ));
					dummynrh = disown( cldh );
					assert( dummynrh == NRH_NULL );
					adopt( parh, cldh, nrh );
					cldh = nextcldh;
					continue;
				}
				namreg_del( nrh );
				cldp = Node_map( cldh );
				cldp->n_flags |= NF_NEWORPH;
				Node_unmap( cldh, &cldp );
			}
			if ( canunlinkpr ) {
				/* REFERENCED */
				nrh_t nrh;

				assert( ! mustorphpr );
				if ( isrealpr ) {
					ok = Node2path( cldh, path1, _("rmdir") );
					if ( ! ok ) {
						cldh = nextcldh;
						continue;
					}
					mlog( MLOG_TRACE | MLOG_TREE,
					      "unlink %s\n",
					      path1 );
					rval = unlink( path1 );
					if ( rval ) {
						mlog( MLOG_NORMAL
						      |
						      MLOG_WARNING
						      |
						      MLOG_TREE, _(
						     "unable to unlink nondir "
						     "%s: %s\n"),
						     path1,
						     strerror( errno ));
						cldh = nextcldh;
						continue;
					}
				}
				nrh = disown( cldh );
				assert( nrh != NRH_NULL );
				link_out( cldh );
				Node_free( &cldh );
			}
		}

		/* next!
		 */
		cldh = nextcldh;
	}

	return BOOL_TRUE;
}

/* ARGSUSED */
static bool_t
mkdirs_recurse( nh_t parh, nh_t cldh, char *path )
{
	while ( cldh != NH_NULL ) {
		node_t *cldp;
		bool_t isdirpr;
		bool_t isselpr;
		bool_t isrealpr;
		bool_t isrefpr;
		nh_t grandcldh;
		nh_t nextcldh;

		cldp = Node_map( cldh );
		isdirpr = ( cldp->n_flags & NF_ISDIR );
		isrealpr = ( cldp->n_flags & NF_REAL );
		isrefpr = ( cldp->n_flags & NF_REFED );
		isselpr = ( cldp->n_flags & NF_SUBTREE );
		grandcldh = cldp->n_cldh;
		nextcldh = cldp->n_sibh;
		Node_unmap( cldh, &cldp );

		/* if needed, create a directory and update real flag
		 */
		if ( isdirpr && ! isrealpr && isrefpr && isselpr ) {
			int rval;

			if ( ! Node2path( cldh, path, _("makedir") )) {
				cldh = nextcldh;
				continue;
			}
			if ( tranp->t_toconlypr ) {
				rval = 0;
			} else {
				mlog( MLOG_TRACE | MLOG_TREE,
				      "mkdir %s\n",
				      path );
				rval = mkdir( path, S_IRWXU );
			}
			if ( rval && errno != EEXIST ) {
				mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
				      "mkdir %s failed: %s\n"),
				      path,
				      strerror( errno ));
				
			} else {
				cldp = Node_map( cldh );
				cldp->n_flags |= NF_REAL;
				Node_unmap( cldh, &cldp );
				isrealpr = BOOL_TRUE;
			}
		}

		/* if a real selected directory, recurse
		 */
		if ( isdirpr && isrealpr && isselpr ) {
			bool_t ok;
			ok = mkdirs_recurse( cldh, grandcldh, path );
							/* RECURSION */
			if ( ! ok ) {
				return BOOL_FALSE;
			}
		}

		/* next!
		 */
		cldh = nextcldh;
	}
	
	return BOOL_TRUE;
}

static bool_t
rename_dirs( nh_t cldh,
	     char *path1,
	     char *path2 )
{
	while ( cldh != NH_NULL ) {
		node_t *cldp;
		/* REFERENCED */
		nh_t parh;
		bool_t isdirpr;
		nh_t renameh;
		bool_t isrenamepr;
		nh_t nextcldh;
		nh_t newparh;
		nh_t newnrh;

		cldp = Node_map( cldh );
		parh = cldp->n_parh;
		isdirpr = cldp->n_flags & NF_ISDIR;
		renameh = cldp->n_lnkh;
		isrenamepr = isdirpr && renameh != NH_NULL;
		nextcldh = cldp->n_sibh;
		Node_unmap( cldh, &cldp );
		assert( parh == persp->p_orphh );

		if ( isrenamepr ) {
			node_t *renamep;
			int rval;
			/* REFERENCED */
			nrh_t dummynrh;
			bool_t ok;

			renamep = Node_map( renameh );
			newparh = renamep->n_parh;
			newnrh = renamep->n_nrh;
			Node_unmap( renameh, &renamep );
			ok = Node2path( cldh, path1, _("rename dir") );
			if ( ! ok ) {
				cldh = nextcldh;
				continue;
			}
			dummynrh = disown( cldh );
			assert( dummynrh == NRH_NULL );
			adopt( newparh, cldh, newnrh );
			ok = Node2path( cldh, path2, _("rename dir") );
			if ( ! ok ) {
				dummynrh = disown( cldh );
				assert( dummynrh == newnrh );
				adopt( persp->p_orphh, cldh, NRH_NULL );
				cldp = Node_map( cldh );
				cldp->n_nrh = NRH_NULL;
				Node_unmap( cldh, &cldp );
				cldh = nextcldh;
				continue;
			}
			mlog( MLOG_TRACE | MLOG_TREE,
			      "rename dir %s to %s\n",
			      path1,
			      path2 );
			rval = rename( path1, path2 );
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_WARNING, _(
				      "unable to rename dir %s "
				      "to dir %s: %s\n"),
				      path1,
				      path2,
				      strerror( errno ));
				dummynrh = disown( cldh );
				assert( dummynrh == newnrh );
				adopt( persp->p_orphh, cldh, NRH_NULL );
				cldh = nextcldh;
				continue;
			}
			cldp = Node_map( cldh );
			cldp->n_flags &= ~NF_NEWORPH;
			cldp->n_lnkh = NH_NULL;
			Node_unmap( cldh, &cldp );
			Node_free( &renameh );
		}

		/* next!
		 */
		cldh = nextcldh;
	}

	return BOOL_TRUE;
}

/* will call funcp for all links to be created. will abort if funcp
 * ever returns FALSE;
 */
rv_t
tree_cb_links( xfs_ino_t ino,
	       gen_t gen,
	       int32_t ctime,
	       int32_t mtime,
	       bool_t ( * funcp )( void *contextp,
				   bool_t linkpr,
				   char *path1,
				   char *path2 ),
	       void *contextp,
	       char *path1,
	       char *path2 )
{
	nh_t hardh;
	nh_t nh;
	char *path;
	bool_t ok;
	int  rval;

	if ( persp->p_truncategenpr ) {
		gen = BIGGEN2GEN( gen );
	}

	/* find the hardhead
	 */
	hardh = link_hardh( ino, gen );

	/* loop through all hard links, attempting to restore/link
	 */
	path = path1;
	for ( nh = hardh ; nh != NH_NULL ; nh = link_nexth( nh )) {
		node_t *np;
		u_char_t flags;
		char *reasonstr;

		/* get the node flags
		 */
		np = Node_map( nh );
		flags = np->n_flags;
		Node_unmap( nh, &np );

		/* build a pathname
		 */
		ok = Node2path( nh, path, _("restore") );
		if ( ! ok ) {
			continue;
		}

		/* skip if not in selected subtree
		 */
		if ( ! ( flags & NF_SUBTREE )) {
			mlog( (MLOG_NITTY + 1) | MLOG_TREE,
			      "skipping %s (ino %llu gen %u): %s\n",
			      path,
			      ino,
			      gen,
			      "not in selected subtree" );
			continue;
		}

		/* don't restore into the housekeeping directory
		 */
		if ( path_beginswith( path, tranp->t_hkdir )) {
			mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
			      "cannot restore %s (ino %llu gen %u): "
			      "pathname contains %s\n"),
			      path,
			      ino,
			      gen,
			      tranp->t_hkdir );
			continue;
		}

		/* check if ok to overwrite: don't check if we've already
		 * been here and decided overwrite ok. if ok, set flag
		 * so we won't check again. in fact, can't check again
		 * since restore changes the answer.
		 */
		if ( ! ( flags & NF_WRITTEN )) {
			bool_t exists;
			if ( ! content_overwrite_ok( path,
						     ctime,
						     mtime,
						     &reasonstr,
						     &exists )) {
				mlog( MLOG_TRACE | MLOG_TREE,
				      "skipping %s (ino %llu gen %u): %s\n",
				      path,
				      ino,
				      gen,
				      reasonstr );
				continue;
			} else {
				np = Node_map( nh );
				np->n_flags |= NF_WRITTEN;
				Node_unmap( nh, &np );

				/*
				 * We're the first stream to restore this file.
				 * Unlink it first to remove extended attributes
				 * that may have been set since the dump was
				 * taken.
				 */
				if ( ! tranp->t_toconlypr && exists ) {
					rval = unlink( path );
					if ( rval && errno != ENOENT ) {
						mlog( MLOG_NORMAL | 
						      MLOG_WARNING, _(
						      "unable to unlink "
						      "current file prior to "
						      "restore: "
						      "%s: %s: discarding\n"),
						      path,
						      strerror( errno ));
						continue;
					}
				}
			}
		}

		/* call callback to restore file / create hard link.
		 * returns !ok if should abort.
		 */
		if ( path == path2 ) {
			mlog( MLOG_TRACE | MLOG_TREE,
			      "link %s to %s (%llu %u)\n",
			      path1,
			      path2,
			      ino,
			      gen );
		} else {
			mlog( MLOG_TRACE | MLOG_TREE,
			      "restoring %s (%llu %u)\n",
			      path1,
			      ino,
			      gen );
		}
		ok = ( * funcp )( contextp, path == path2, path1, path2 );
		if ( ! ok ) {
			return RV_NOTOK;
		}

		/* set flag, indicating node is now real
		 */
		np = Node_map( nh );
		np->n_flags |= NF_REAL;
		Node_unmap( nh, &np );

		/* switch to second path buffer, for link paths
		 */
		path = path2;
	}


	/* if not yet peeled from tape, do so: place in orphanage if
	 * no references found (i.e., hard link list empty).
	 */
	if ( path == path1 ) {
		if ( hardh != NH_NULL || persp->p_ignoreorphpr ) {
			/*
			 * The file is referenced so the path is
			 * valid.  This means that if path begins
			 * with 'orphanage' the file is one of two
			 * things:
			 * 1) It's a file that really is an
			 * orphanage file from a previous restore 
			 * that has now ended up on this dump tape.
			 * We don't really want to restore this file
			 * but, it's harmless to do so, it should 
			 * happen rarely, and the path name is 
			 * indistinguishable from ...
			 * 2) A file whose name was never resolved
			 * from root because of file corruption.
			 * Some granparent dir (parent dir of it's 
			 * parent dir) was corrupted so the code that 
			 * walks the trees was thus never able to set 
			 * the NF_SUBTREE flag.  It then ends up here
			 * with a non-resolved name but a valid
			 * hard reference.  We really need to give
			 * the admin the opportunity to get this
			 * data since one corrupted dirnode would
			 * make the whole tree of data
			 * unreachable.  pv698761
			 */
			if ( persp->p_ignoreorphpr || (strncmp(ORPH_NAME, path, 
					    strlen(ORPH_NAME)) != 0)) {
				mlog( MLOG_DEBUG | MLOG_TREE,
				      "discarding %llu %u\n",
				      ino,
				      gen );
				ok = ( * funcp )( contextp, BOOL_FALSE, 0, 0 );
				if ( ! ok ) {
					return RV_NOTOK;
				}
			} else {

				if ( ! tranp->t_toconlypr ) {
					char *dir;
					char tmp[PATH_MAX];

					strcpy(tmp, path);
					dir = dirname(tmp);
					mkdir_r(dir);
				}

				mlog (MLOG_VERBOSE | MLOG_NOTE | MLOG_TREE, _(
				      "ino %llu salvaging file,"
				      " placing in %s\n"), ino, path1);
				ok = ( * funcp )( contextp, path == path2, 
					path1, path2 );
				if ( ! ok ) {
					return RV_NOTOK;
				}
			} 
		} else {
			mlog( MLOG_VERBOSE | MLOG_NOTE | MLOG_TREE, _(
			      "ino %llu gen %u not referenced: "
			      "placing in orphanage\n"),
			      ino,
			      gen );
			nh = Node_alloc( ino,
					 gen,
					 NRH_NULL,
					 DAH_NULL,
					 NF_REAL | NF_NEWORPH );
			if (nh == NH_NULL) {
				mlog( MLOG_ERROR | MLOG_TREE, _(
				"node allocation failed when placing ino %llu"
				" in orphanage\n"), ino); 
				return RV_ERROR; /* allocation failed */
			}
			link_in( nh );
			adopt( persp->p_orphh, nh, NRH_NULL );
			ok = Node2path( nh, path1, _("orphan") );
			assert( ok );
			( void )( * funcp )( contextp, BOOL_FALSE, path1,path2);
		}
	}
	return RV_OK;
}

/* uses flags cleared during directory restore (NF_DUMPEDDIR and NF_REFED )
 * to determine what directory entries are no longer needed. this can
 * be done because whenever a directory chenges, it and all of its current
 * entries are dumped. so if an entry is dumped which is a dir, but that
 * dir is not dumped, all of that directories entries are needed and so must
 * have their REFED flag cleared.
 *
 * does a recursive depth-wise descent of the tree, following this rule to
 * clear the REFED flags.
 */

static void tree_adjref_recurse( nh_t cldh,
				 bool_t pardumpedpr,
				 bool_t parrefedpr );

bool_t
tree_adjref( void )
{
	tree_adjref_recurse( persp->p_rooth, BOOL_FALSE, BOOL_TRUE );
	return BOOL_TRUE;
}

static void
tree_adjref_recurse( nh_t cldh,
		       bool_t pardumpedpr,
		       bool_t parrefedpr )
{
	nh_t grandcldh;
	bool_t clddumpedpr;
	bool_t cldrefedpr;
	{
		node_t *cldp;
		cldp = Node_map( cldh );
		if ( ! pardumpedpr && parrefedpr ) {
			cldp->n_flags |= NF_REFED;
		}
		clddumpedpr = ( int )cldp->n_flags & NF_DUMPEDDIR;
		cldrefedpr = ( int )cldp->n_flags & NF_REFED;
		grandcldh = cldp->n_cldh;
		Node_unmap( cldh, &cldp  );
	}
	while ( grandcldh != NH_NULL ) {
		node_t *grandcldp;
		nh_t nextgrandcldh;
		tree_adjref_recurse( grandcldh, clddumpedpr, cldrefedpr );
								/* RECURSION */
		grandcldp = Node_map( grandcldh );
		nextgrandcldh = grandcldp->n_sibh;
		Node_unmap( grandcldh, &grandcldp  );
		grandcldh = nextgrandcldh;
	}
}

static bool_t tree_extattr_recurse( nh_t parh,
				    nh_t cldh,
				    bool_t ( * cbfunc )( char *path,
							 dah_t dah ),
				    char *path );

bool_t
tree_extattr( bool_t ( * cbfunc )( char *path, dah_t dah ), char *path )
{
	node_t *rootp;
	nh_t cldh;
	bool_t ok;

	rootp = Node_map( persp->p_rooth );
	cldh = rootp->n_cldh;
	Node_unmap( persp->p_rooth, &rootp );
	ok = tree_extattr_recurse( persp->p_rooth, cldh, cbfunc, path );

	return ok;
}

static bool_t
tree_extattr_recurse( nh_t parh,
		      nh_t cldh,
		      bool_t ( * cbfunc )( char *path, dah_t dah ),
		      char *path )
{
	node_t *parp;
	dah_t dah;
	bool_t ok;

	/* first update all children
	 */
	while ( cldh != NH_NULL ) {
		node_t *cldp;
		bool_t isdirpr;
		bool_t isselpr;
		bool_t isrealpr;
		nh_t grandcldh;
		nh_t nextcldh;

		cldp = Node_map( cldh );
		isdirpr = ( cldp->n_flags & NF_ISDIR );
		isrealpr = ( cldp->n_flags & NF_REAL );
		isselpr = ( cldp->n_flags & NF_SUBTREE );
		grandcldh = cldp->n_cldh;
		nextcldh = cldp->n_sibh;
		Node_unmap( cldh, &cldp );

		/* if a real selected directory, recurse
		 */
		if ( isdirpr && isrealpr && isselpr ) {
			bool_t ok;
			ok = tree_extattr_recurse( cldh,
						   grandcldh,
						   cbfunc,
						   path );
							/* RECURSION */
			if ( ! ok ) {
				return BOOL_FALSE;
			}
		}

		/* next!
		 */
		cldh = nextcldh;
	}

	/* now update self
	 */
	parp = Node_map( parh );
	dah = parp->n_dah;
	Node_unmap( parh, &parp );
	if ( ! Node2path( parh, path, _("set dir extattr") )) {
		mlog (MLOG_NORMAL | MLOG_WARNING | MLOG_TREE,  _(
		      "tree_extattr_recurse: Could not convert node to "
		      "path for %s\n"), path);
		return BOOL_TRUE;
	}
	if ( dah != DAH_NULL ) {
		ok = ( * cbfunc )( path, dah );
	} else {
		ok = BOOL_TRUE;
	}
	
	return ok;
}

struct phcb {
	char *path1;
	char *path2;
	bool_t ok;
};

typedef struct phcb phcb_t;

static bool_t proc_hardlinks_cb( void *contextp, nh_t hardheadh );

static bool_t
proc_hardlinks( char *path1, char *path2 )
{
	phcb_t phcb;

	/* have callback invoked for all hardheads
	 */
	phcb.path1 = path1;
	phcb.path2 = path2;
	phcb.ok = BOOL_TRUE;
	link_headiter( proc_hardlinks_cb, ( void * )&phcb );
	return phcb.ok;
}

/* called for every hard head
 *
 * tes@sgi.com:
 * This code processes the hardlinks, extracting out a 
 * src_list - real & !ref
 * dest_list - !real & ref
 * The src_list are the entries to delete and the dst_list
 * are the new_entries to create.
 * 1. If we have a src and a dst then we can do a rename.
 * 2. If we have extra src nodes then we can unlink them.
 * 3. If we have extra dst nodes then we can link them.
 * HOWEVER, the linking in of the new nodes is actually handled
 * in the restore_file_cb() code which on restoral creates
 * the file and all its hardlinks.
 * ALSO, I can not see how the src_list can have more than
 * 1 node in it, since in noref_elim_recurse() it unlinks all
 * extra deleted entries (real & !ref).
 * Thus, steps 2 and 3 are handled in noref_elim_recurse().
 */
static bool_t
proc_hardlinks_cb( void *contextp, nh_t hardheadh )
{
	phcb_t *phcbp = ( phcb_t * )contextp;
	node_t *hardheadp;
	xfs_ino_t ino;
	gen_t gen;
	bool_t isdirpr;
	nh_t rnsrcheadh;
	nh_t rndstheadh;
	nh_t lnsrch;
	nh_t nh;
	link_iter_context_t link_iter_context;
	bool_t ok;
	int rval;

	/* skip directories
	 */
	hardheadp = Node_map( hardheadh );
	ino = hardheadp->n_ino;
	gen = hardheadp->n_gen;
	isdirpr = hardheadp->n_flags & NF_ISDIR;
	Node_unmap( hardheadh, &hardheadp );
	if ( isdirpr ) {
		return BOOL_TRUE;
	}

	mlog( MLOG_DEBUG | MLOG_TREE,
	      "processing hardlinks to %llu %u\n",
	      ino,
	      gen );

	/* first pass through hard link list: for each node, leave on
	 * list, unlink and place on rename src list, unlink and place on
	 * rename dst list, or unlink and discard. note a node available
	 * to link from, in case we need it.
	 */
	rnsrcheadh = NH_NULL;
	rndstheadh = NH_NULL;
	lnsrch = NH_NULL;
	link_iter_init( &link_iter_context, hardheadh );
	while ( ( nh = link_iter_next( &link_iter_context )) != NH_NULL ) {

		node_t *np = Node_map( nh );
		bool_t isrealpr = np->n_flags & NF_REAL;
		bool_t isrefpr = np->n_flags & NF_REFED;
		bool_t isselpr = np->n_flags & NF_SUBTREE;

		/* if unrefed, unreal, free node etc. (sel doesn't matter)
		 */
		if ( ! isrealpr && ! isrefpr ) {
			mlog( MLOG_NITTY | MLOG_TREE,
			      "freeing node %x: not real, not referenced\n",
			      nh );
			link_iter_unlink( &link_iter_context, nh );
			Node_unmap( nh, &np );
			( void )disown( nh );
			Node_free( &nh );
			continue;
		}
		
		/* not real, refed, but not selected, can't help
		 */
		if ( ! isrealpr &&   isrefpr && ! isselpr ) {
			mlog( MLOG_NITTY | MLOG_TREE,
			      "skipping node %x: not selected\n",
			      nh );
			Node_unmap( nh, &np );
			continue;
		}

		/* if unreal, refed, sel, add to dst list,
		 */
		if ( ! isrealpr &&   isrefpr &&   isselpr ) {
			mlog( MLOG_NITTY | MLOG_TREE,
			      "making node %x dst: "
			      "not real, refed, sel\n",
			      nh );
			link_iter_unlink( &link_iter_context, nh );
			np->n_lnkh = rndstheadh;
			rndstheadh = nh;
			Node_unmap( nh, &np );
			continue;
		}

		/* if real, unrefed, sel, add to src list
		 */
		if (   isrealpr && ! isrefpr &&   isselpr ) {
			mlog( MLOG_NITTY | MLOG_TREE,
			      "making node %x src: real, not refed, sel\n",
			      nh );
			link_iter_unlink( &link_iter_context, nh );
			np->n_lnkh = rnsrcheadh;
			rnsrcheadh = nh;
			Node_unmap( nh, &np );
			continue;
		}

		/* would like to get rid of but not selected, or
		 * real and referenced, leave alone (sel doesn't matter).
		 * consider as a lnk src, since real and not going away.
		 */
		if (   isrealpr && ( isrefpr || !isselpr ) ) {
			mlog( MLOG_NITTY | MLOG_TREE,
			      "skipping node %x: %s\n",
			      nh,
			      isselpr ? "real and ref" : "real and not sel" );
			Node_unmap( nh, &np );
			if ( lnsrch == NH_NULL ) {
				mlog( MLOG_NITTY | MLOG_TREE,
				      "node %x will be link src\n",
				      nh );
				lnsrch = nh;
			}
			continue;
		}
		assert( 0 );
	}

	/* now pass through dst list, doing renames if src list not empty,
	 * otherwise links if a lnk src available, otherwise put back in link
	 * list
	 */
	while ( rndstheadh != NH_NULL ) {
		nh_t dsth;
		node_t *dstp;
		bool_t successpr;

		dsth = rndstheadh;
		dstp = Node_map( dsth );
		rndstheadh = dstp->n_lnkh;
		dstp->n_lnkh = NH_NULL;
		Node_unmap( dsth, &dstp );

		/* build pathname to dst
		 */
		ok = Node2path( dsth, phcbp->path2, _("rename to") );
		if ( ! ok ) {
			link_in( dsth );
			continue;
		}

		successpr = BOOL_FALSE;
		while ( ! successpr && rnsrcheadh != NH_NULL ) {
			nh_t srch;
			nrh_t nrh;
			node_t *srcp;

			srch = rnsrcheadh;
			srcp = Node_map( srch );
			rnsrcheadh = srcp->n_lnkh;
			srcp->n_lnkh = NH_NULL;
			Node_unmap( srch, &srcp );

			/* build a path to src
			 */
			ok = Node2path( srch, phcbp->path1, _("rename from") );
			if ( ! ok ) {
				link_in( srch );
				continue;
			}

			if ( tranp->t_toconlypr ) {
				rval = 0;
			} else {
				mlog( MLOG_TRACE | MLOG_TREE,
				      "rename nondir %s to %s\n",
				      phcbp->path1,
				      phcbp->path2 );
				rval = rename( phcbp->path1,
					       phcbp->path2 );
			}
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
				      "unable to rename nondir "
				      "%s to %s: %s\n"),
				      phcbp->path1,
				      phcbp->path2,
				      strerror( errno ));
				link_in( srch );
				continue;
			}

			nrh = disown( srch );
			if ( nrh != NRH_NULL ) {
				namreg_del( nrh );
			}
			Node_free( &srch );

			successpr = BOOL_TRUE;
		}

		/* tes@sgi.com: note: loop of one iteration only 
		 */
		while ( ! successpr && lnsrch != NH_NULL ) {
			ok = Node2path( lnsrch, phcbp->path1, _("link") );

			if ( ! ok ) {
				mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
				      "unable to use %s "
				      "as a hard link source\n"),
				      phcbp->path1 );
				lnsrch = NH_NULL;
				continue;
			}

			if ( tranp->t_toconlypr ) {
				rval = 0;
			} else {
				mlog( MLOG_TRACE | MLOG_TREE,
				      "link nondir %s to %s\n",
				      phcbp->path1,
				      phcbp->path2 );
				rval = link( phcbp->path1,
					     phcbp->path2 );
			}
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE,
				      "unable to link nondir "
				      "%s to %s: %s\n",
				      phcbp->path1,
				      phcbp->path2,
				      strerror( errno ));
				break;
			}
			successpr = BOOL_TRUE;
		}

		if ( ! successpr ) {
			mlog( MLOG_NITTY | MLOG_TREE,
			      "no link src for node %x\n",
			      dsth );
		} else {
			dstp = Node_map( dsth );
			dstp->n_flags |= NF_REAL;
			Node_unmap( dsth, &dstp );
		}

		link_in( dsth );
	}

	/* finally, pass through remaining src list, unlinking/disowning.
	 * tes@sgi.com: don't believe this will happen as this step
	 * should now be done in noref_elim_recurse().
	 */
	while ( rnsrcheadh != NH_NULL ) {
		nh_t srch;
		node_t *srcp;
		bool_t ok;

		srch = rnsrcheadh;
		srcp = Node_map( srch );
		rnsrcheadh = srcp->n_lnkh;
		srcp->n_lnkh = NH_NULL;
		Node_unmap( srch, &srcp );

		ok = Node2path( srch, phcbp->path1, _("unlink") );
		if ( ! ok ) {
			link_in( srch );
			continue;
		}

		if ( tranp->t_toconlypr ) {
			rval = 0;
		} else {
			mlog( MLOG_TRACE | MLOG_TREE,
			      "unlink nondir %s\n",
			      phcbp->path1 );
			rval = unlink( phcbp->path1 );
		}
		if ( rval ) {
			mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
			      "unable to unlink %s: %s\n"),
			      phcbp->path1,
			      strerror( errno ));
			link_in( srch );
		} else {
			nrh_t nrh;
			nrh = disown( srch );
			if ( nrh != NRH_NULL ) {
				namreg_del( nrh );
			}
			Node_free( &srch );
		}
	}

	return BOOL_TRUE;
}

/* traverse tree depth-wise bottom-up for dirs no longer referenced.
 * if non-empty, move children to orphanage
 */
bool_t
tree_setattr( char *path )
{
	bool_t ok;
	node_t *rootp;

	ok = tree_setattr_recurse( persp->p_rooth, path );

	if ( restore_rootdir_permissions && ok ) {
		rootp = Node_map( persp->p_rooth );
		/* "." is cwd which is the destination dir */
		setdirattr( rootp->n_dah, "." );
		Node_unmap( persp->p_rooth, &rootp );
	}

	return ok;
}

static bool_t
tree_setattr_recurse( nh_t parh, char *path )
{
	node_t *parp = Node_map( parh );
	nh_t cldh = parp->n_cldh;
	Node_unmap( parh, &parp );
	while ( cldh != NH_NULL ) {
		nh_t nextcldh;

		/* get the node attributes
		 */
		node_t *cldp = Node_map( cldh );
		bool_t isdirpr = ( cldp->n_flags & NF_ISDIR );
		bool_t isselpr = ( cldp->n_flags & NF_SUBTREE );
		bool_t isrealpr = ( cldp->n_flags & NF_REAL );
		dah_t dah = cldp->n_dah;

		/* get next cld
		 */
		nextcldh = cldp->n_sibh;
		Node_unmap( cldh, &cldp );

		/* if is a real selected dir, go ahead.
		 */
		if ( isdirpr && isselpr && isrealpr ) {
			bool_t ok;
			ok = tree_setattr_recurse( cldh, path ); /* RECURSION */
			if ( ! ok ) {
				Node_unmap( cldh, &cldp );
				return BOOL_FALSE;
			}
			if ( dah != DAH_NULL ) {
				bool_t ok;
				ok = Node2path( cldh, path, _("set dirattr") );
				if ( ok ) {
					setdirattr( dah, path );
				}
			}
		}

		cldh = nextcldh;
	}

	return BOOL_TRUE;
}

static void
setdirattr( dah_t dah, char *path )
{
	mode_t mode;
	struct utimbuf utimbuf;
	struct fsxattr fsxattr;
	int rval;
	size_t	hlen;
	void	*hanp;
	int fd = -1;

	if ( dah == DAH_NULL )
		return;

	if ( tranp->t_dstdirisxfspr ) {
		if (path_to_handle(path, &hanp, &hlen)) {
			mlog( MLOG_NORMAL | MLOG_WARNING,
				_("path_to_handle of %s failed:%s\n"),
				path, strerror( errno ));
		} else {
			fd = open_by_handle(hanp, hlen, O_RDONLY);
			if (fd < 0) {
				mlog( MLOG_NORMAL | MLOG_WARNING,
					_("open_by_handle of %s failed:%s\n"),
					path, strerror( errno ));
			}
			free_handle(hanp, hlen);
		}
	}

	if ( tranp->t_dstdirisxfspr && persp->p_restoredmpr ) {
		fsdmidata_t fssetdm;

		fssetdm.fsd_dmevmask = dirattr_get_dmevmask( dah );
		fssetdm.fsd_padding = 0;	/* not used */
		fssetdm.fsd_dmstate = ( uint16_t )dirattr_get_dmstate( dah );

		/* restore DMAPI event settings etc.
		 */
		rval = ioctl( fd,
			      XFS_IOC_FSSETDM,
			      ( void * )&fssetdm );
		if ( rval ) {
			mlog( errno == EINVAL
			      ?
			      ( MLOG_NITTY + 1 ) | MLOG_TREE
			      :
			      MLOG_NITTY | MLOG_TREE,
			      "set DMI attributes"
			      " of %s failed: %s\n",
			      path,
			      strerror( errno ));
		}
	}

	utimbuf.actime = dirattr_get_atime( dah );
	utimbuf.modtime = dirattr_get_mtime( dah );
	rval = utime( path, &utimbuf );
	if ( rval ) {
		mlog( MLOG_VERBOSE | MLOG_TREE, _(
		      "could not set access and modification times"
		      " of %s: %s\n"),
		      path,
		      strerror( errno ));
	}
	mode = dirattr_get_mode( dah );
	if ( persp->p_ownerpr  ) {
		rval = chown( path,
			      dirattr_get_uid( dah ),
			      dirattr_get_gid( dah ));
		if ( rval ) {
			mlog( MLOG_NORMAL | MLOG_TREE, _(
			      "chown (uid=%d, gid=%d) %s failed: %s\n"),
			      dirattr_get_uid( dah ),
			      dirattr_get_gid( dah ),
			      path,
			      strerror( errno ));
		}
	}
	rval = chmod( path, mode );
	if ( rval ) {
		mlog( MLOG_NORMAL | MLOG_TREE, _(
		      "chmod %s failed: %s\n"),
		      path,
		      strerror( errno ));
	}

	/* set the extended inode flags
	 */
	if ( !tranp->t_dstdirisxfspr )
		return;

	memset((void *)&fsxattr, 0, sizeof( fsxattr ));
	fsxattr.fsx_xflags = dirattr_get_xflags( dah );
	fsxattr.fsx_extsize = dirattr_get_extsize( dah );
	fsxattr.fsx_projid = dirattr_get_projid( dah );

	rval = ioctl( fd,
		      XFS_IOC_FSSETXATTR,
		      (void *)&fsxattr);
	if ( rval < 0 ) {
		mlog(MLOG_NORMAL | MLOG_WARNING,
		     _("attempt to set "
		       "extended attributes "
		       "(xflags 0x%x, "
		       "extsize = 0x%x, "
		       "projid = 0x%x) "
		       "of %s failed: "
		       "%s\n"),
		     fsxattr.fsx_xflags,
		     fsxattr.fsx_extsize,
		     fsxattr.fsx_projid,
		     path,
		     strerror(errno));
	}
	( void )close( fd );
}

/* deletes orphanage if empty, else warns
 */
bool_t
tree_delorph( void )
{
	int rval;

	rval = rmdir( tranp->t_orphdir );
	if ( rval ) {
		if ( errno == EEXIST ) {
			mlog(MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
			     "unable to rmdir %s: not empty\n"),
			     tranp->t_orphdir );
		} else {
			mlog(MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
			     "unable to rmdir %s: %s\n"),
			     tranp->t_orphdir,
			     strerror( errno ));
		}
	}

	return BOOL_TRUE;
}


/* definition of locally defined static functions ****************************/


/* interactive subtree abstraction *******************************************/

static void tsi_cmd_inst( void *, dlog_pcbp_t, void * );
static void tsi_cmd_pwd( void *, dlog_pcbp_t, void * );
static void tsi_cmd_ls( void *, dlog_pcbp_t, void * );
static void tsi_cmd_cd( void *, dlog_pcbp_t, void * );
static void tsi_cmd_add( void *, dlog_pcbp_t, void * );
static void tsi_cmd_delete( void *, dlog_pcbp_t, void * );
static void tsi_cmd_extract( void *, dlog_pcbp_t, void * );
static void tsi_cmd_quit( void *, dlog_pcbp_t, void * );
static void tsi_cmd_help( void *, dlog_pcbp_t, void * );
static void tsi_cmd_parse( char * );
static dlog_ucbp_t tsi_cmd_match( void );

#define PREAMBLEMAX	3
#define ACKMAX		3
#define POSTAMBLEMAX	3

bool_t
tree_subtree_inter( void )
{
	fold_t fold;
	char *preamblestr[ PREAMBLEMAX ];
	size_t preamblecnt;
	char *ackstr[ ACKMAX ];
	size_t ackcnt;
	char *postamblestr[ POSTAMBLEMAX ];
	size_t postamblecnt;

	dlog_ucbp_t cmdp;

restart:
	/* make the current working directory the root of the fs
	 */
	tranp->t_inter.i_cwdh = persp->p_rooth;

	/* begin the dialog
	 */
	preamblecnt = 0;
	fold_init( fold, _("subtree selection dialog"), '=' );
	preamblestr[ preamblecnt++ ] = "\n";
	preamblestr[ preamblecnt++ ] = fold;
	preamblestr[ preamblecnt++ ] = "\n\n";
	assert( preamblecnt <= PREAMBLEMAX );
	dlog_begin( preamblestr, preamblecnt );

	/* execute commands until time to extract or quit. always begin with
	 * an implicit instructions command. if see SIGINT, give main thread
	 * dialog a chance to decide if a session interrupt is wanted.
	 */
	cmdp = tsi_cmd_inst;
	do {
		char buf[ MAXPATHLEN ];
		const ix_t abortix = 1;
		const ix_t sigintix = 2;
		const ix_t okix = 3;
		ix_t responseix;

		/* execute command and get response
		 */
		responseix = dlog_string_query( cmdp,
					        0,
					        buf,
					        sizeof( buf ),
					        0,
					        IXMAX,    /* timeout ix */
					        sigintix, /* sigint ix */
					        abortix,  /* sighup ix */
						abortix,  /* sigquit ix */
						okix );   /* ok ix */

		/* ack the response
		 */
		ackcnt = 0;
		if ( responseix == sigintix ) {
			ackstr[ ackcnt++ ] = _("keyboard interrupt\n");
		} else if ( responseix == abortix ) {
			ackstr[ ackcnt++ ] = _("abort\n");
		} else {
			assert( responseix == okix );
		}
		assert( ackcnt <= ACKMAX );
		dlog_string_ack( ackstr,
				 ackcnt );

		/* exception handling
		 */
		if ( responseix != okix ) {
			/* if exception, end the dialog. may restart
			 * if operator decidesd not to intr.
			 */
			postamblecnt = 0;
			fold_init( fold, _("end dialog"), '-' );
			postamblestr[ postamblecnt++ ] = "\n";
			postamblestr[ postamblecnt++ ] = fold;
			postamblestr[ postamblecnt++ ] = "\n\n";
			assert( postamblecnt <= POSTAMBLEMAX );
			dlog_end( postamblestr, postamblecnt );

			/* if sighup or sigquit, immediately quit
			 */
			if ( responseix == abortix ) {
				return BOOL_FALSE;
			}

			/* if sigint, allow main thread to decide if
			 * operator really wants to quit
			 */
			assert( responseix == sigintix );
			if ( cldmgr_stop_requested( )) {
				return BOOL_FALSE;
			}
			sleep( 1 ); /* to allow main thread to begin dialog */
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_TREE,
			      "" ); /* block until main thread dialog done */
			sleep( 1 ); /* let main thread ask children to die */
			if ( cldmgr_stop_requested( )) {
				return BOOL_FALSE;
			}
			mlog( MLOG_DEBUG | MLOG_TREE,
			      "retrying interactive subtree selection dialog\n" );
			goto restart;
		}


		tsi_cmd_parse( buf );
		cmdp = tsi_cmd_match( );
		if ( ! cmdp ) {
			cmdp = tsi_cmd_help;
		}
	} while ( cmdp != tsi_cmd_quit && cmdp != tsi_cmd_extract );

	postamblecnt = 0;
	fold_init( fold, _("end dialog"), '-' );
	postamblestr[ postamblecnt++ ] = "\n";
	postamblestr[ postamblecnt++ ] = fold;
	postamblestr[ postamblecnt++ ] = "\n\n";
	assert( postamblecnt <= POSTAMBLEMAX );
	dlog_end( postamblestr, postamblecnt );

	/* pv 773569 - quit is not a reason to consider session
         * to be interrupted (we haven't started yet) so just unmark
         * any selected directories and return */
	if ( cmdp == tsi_cmd_quit ) {
		mlog( MLOG_NORMAL, _("Unmark and quit\n") );
		selsubtree( persp->p_rooth , BOOL_FALSE );
	}
	    
	return BOOL_TRUE;
}

static void
tsi_cmd_inst( void *ctxp,
	      dlog_pcbp_t pcb,
	      void *pctxp )
{
	tsi_cmd_help( ctxp, pcb, pctxp );
}

static void
tsi_cmd_pwd( void *ctxp,
	     dlog_pcbp_t pcb,
	     void *pctxp )
{
	/* special case root
	 */
	if ( tranp->t_inter.i_cwdh == persp->p_rooth ) {
		( * pcb )( pctxp, "cwd is fs root\n" );
		return;
	}

	/* ascend tree recursively, print path on way back
	 */
	tsi_cmd_pwd_recurse( ctxp, pcb, pctxp, tranp->t_inter.i_cwdh );
	( * pcb )( pctxp, "\n" );
}

static void
tsi_cmd_pwd_recurse( void *ctxp,
		     dlog_pcbp_t pcb,
		     void *pctxp,
		     nh_t nh )
{
	node_t *np;
	register nh_t parh;
	/* REFERENCED */
	register int namelen;
	nrh_t nrh;

	assert( nh != NH_NULL );

	np = Node_map( nh );
	nrh = np->n_nrh;
	parh = np->n_parh;
	Node_unmap( nh, &np );
	if ( parh != persp->p_rooth ) {
		tsi_cmd_pwd_recurse( ctxp, pcb, pctxp, parh );
			/* RECURSION */
		( * pcb )( pctxp, "/" );
	}
	assert( nrh != NRH_NULL );
	namelen = namreg_get( nrh,
			      tranp->t_inter.i_name,
			      sizeof( tranp->t_inter.i_name ));
	assert( namelen > 0 );
	( * pcb )( pctxp, tranp->t_inter.i_name );
}

/* ARGSUSED */
static void
tsi_cmd_ls( void *ctxp,
	    dlog_pcbp_t pcb,
	    void *pctxp )
{
	size_t argc = tranp->t_inter.i_argc;
	char *arg = ( argc > 1 ) ? tranp->t_inter.i_argv[ 1 ] : 0;
	bool_t ok;

	nh_t cldh;
	nh_t parh;
	nh_t namedh;
	xfs_ino_t ino;
	bool_t isdirpr;
	bool_t isselpr;

	/* walk the tree according to the path argument, to get
	 * the named node.
	 */
	ok = tsi_walkpath( arg,
			   persp->p_rooth,
			   tranp->t_inter.i_cwdh,
			   pcb,
			   pctxp,
			   &namedh,
			   &parh,
			   &cldh,
			   &ino,
			   &isdirpr,
			   &isselpr );
	if ( ! ok ) {
		return;
	}

	/* if named is not a dir, just display named
	 */
	if ( ! isdirpr ) {
		( * pcb )( pctxp,
			   "    %s %10llu %s%s\n",
			   isselpr ? "*" : " ",
			   ino,
			   tranp->t_inter.i_name,
			   isdirpr ? "/" : " " );

		return;
	}

	/* iterate through the directory, printing all matching entries.
	 * hide the orphanage.
	 */
	while ( cldh != NH_NULL ) {
		node_t *cldp;
		nrh_t nrh;
		nh_t nextcldh;
		cldp = Node_map( cldh );
		nrh = cldp->n_nrh;
		nextcldh = cldp->n_sibh;
		isdirpr = ( cldp->n_flags & NF_ISDIR );
		isselpr = ( cldp->n_flags & NF_SUBTREE );
		ino = cldp->n_ino;
		Node_unmap( cldh, &cldp );
		if ( cldh != persp->p_orphh ) {
			/* REFERENCED */
			int namelen;
			namelen = namreg_get( nrh,
					      tranp->t_inter.i_name,
					      sizeof( tranp->t_inter.i_name ));
			assert( namelen > 0 );
			( * pcb )( pctxp,
				   "    %s %10llu %s%s\n",
				   isselpr ? "*" : " ",
				   ino,
				   tranp->t_inter.i_name,
				   isdirpr ? "/" : " " );
		}
		cldh = nextcldh;
	}
}

/* ARGSUSED */
static void
tsi_cmd_cd( void *ctxp,
	    dlog_pcbp_t pcb,
	    void *pctxp )
{
	size_t argc = tranp->t_inter.i_argc;
	char *arg = ( argc > 1 ) ? tranp->t_inter.i_argv[ 1 ] : 0;

	nh_t cldh;
	nh_t parh;
	nh_t namedh;
	xfs_ino_t ino;
	bool_t isdirpr;
	bool_t isselpr;
	bool_t ok;

	/* walk the tree according to the path argument, to get
	 * the named node.
	 */
	ok = tsi_walkpath( arg,
			   persp->p_rooth,
			   tranp->t_inter.i_cwdh,
			   pcb,
			   pctxp,
			   &namedh,
			   &parh,
			   &cldh,
			   &ino,
			   &isdirpr,
			   &isselpr );
	if ( ! ok ) {
		return;
	}

	/* if named is not a dir, complain
	 */
	if ( ! isdirpr ) {
		assert( arg );
		( * pcb )( pctxp,
			   _("%s is not a directory\n"),
			   arg );

		return;
	}

	/* change the current working directory
	 */
	tranp->t_inter.i_cwdh = namedh;
}

/* ARGSUSED */
static void
tsi_cmd_add( void *ctxp,
	     dlog_pcbp_t pcb,
	     void *pctxp )
{
	size_t argc = tranp->t_inter.i_argc;
	char *arg = ( argc > 1 ) ? tranp->t_inter.i_argv[ 1 ] : 0;

	nh_t cldh;
	nh_t parh;
	nh_t namedh;
	xfs_ino_t ino;
	bool_t isdirpr;
	bool_t isselpr;
	bool_t ok;

	/* walk the tree according to the path argument, to get
	 * the named node.
	 */
	ok = tsi_walkpath( arg,
			   persp->p_rooth,
			   tranp->t_inter.i_cwdh,
			   pcb,
			   pctxp,
			   &namedh,
			   &parh,
			   &cldh,
			   &ino,
			   &isdirpr,
			   &isselpr );
	if ( ! ok ) {
		return;
	}

	selsubtree( namedh, BOOL_TRUE );
}

/* ARGSUSED */
static void
tsi_cmd_delete( void *ctxp,
		dlog_pcbp_t pcb,
		void *pctxp )
{
	size_t argc = tranp->t_inter.i_argc;
	char *arg = ( argc > 1 ) ? tranp->t_inter.i_argv[ 1 ] : 0;

	nh_t cldh;
	nh_t parh;
	nh_t namedh;
	xfs_ino_t ino;
	bool_t isdirpr;
	bool_t isselpr;
	bool_t ok;

	/* walk the tree according to the path argument, to get
	 * the named node.
	 */
	ok = tsi_walkpath( arg,
			   persp->p_rooth,
			   tranp->t_inter.i_cwdh,
			   pcb,
			   pctxp,
			   &namedh,
			   &parh,
			   &cldh,
			   &ino,
			   &isdirpr,
			   &isselpr );
	if ( ! ok ) {
		return;
	}

	selsubtree( namedh, BOOL_FALSE );
}

/* ARGSUSED */
static void
tsi_cmd_extract( void *ctxp,
		 dlog_pcbp_t pcb,
		 void *pctxp )
{
}

/* ARGSUSED */
static void
tsi_cmd_quit( void *ctxp,
	      dlog_pcbp_t pcb,
	      void *pctxp )
{
}

static int parse( int slotcnt, char **slotbuf, char *string );

static void
tsi_cmd_parse( char *buf )
{
	int wordcnt;

	if ( ! buf ) {
		tranp->t_inter.i_argc = 0;
		return;
	}

	wordcnt = parse( INTER_ARGMAX, tranp->t_inter.i_argv, buf );

	tranp->t_inter.i_argc = ( size_t )min( max( 0, wordcnt ), INTER_ARGMAX );
}

struct tsi_cmd_tbl {
	char *tct_pattern;
	char *tct_help;
	dlog_ucbp_t tct_ucb;
	size_t tct_argcmin;
	size_t tct_argcmax;
};

typedef struct tsi_cmd_tbl tsi_cmd_tbl_t;

static tsi_cmd_tbl_t tsi_cmd_tbl[] = {
	{ "pwd",	"",				tsi_cmd_pwd,	1, 1 },
	{ "ls",		"[ <path> ]",			tsi_cmd_ls,	1, 2 },
	{ "cd",		"[ <path> ]",			tsi_cmd_cd,	1, 2 },
	{ "add",	"[ <path> ]",			tsi_cmd_add,	1, 2 },
	{ "delete",	"[ <path> ]",			tsi_cmd_delete,	1, 2 },
	{ "extract",	"",				tsi_cmd_extract,1, 1 },
	{ "quit",	"",				tsi_cmd_quit,	1, 1 },
	{ "help",	"",				tsi_cmd_help,	1, 1 },
};

static dlog_ucbp_t
tsi_cmd_match( void )
{
	tsi_cmd_tbl_t *tblp = tsi_cmd_tbl;
	tsi_cmd_tbl_t *tblendp = tsi_cmd_tbl
				 +
				 sizeof( tsi_cmd_tbl )
				 /
				 sizeof( tsi_cmd_tbl[ 0 ] );

	if ( tranp->t_inter.i_argc == 0 ) {
		return 0;
	}

	for ( ; tblp < tblendp ; tblp++ ) {
		if ( ! strncmp( tranp->t_inter.i_argv[ 0 ],
				tblp->tct_pattern,
				strlen( tranp->t_inter.i_argv[ 0 ] ))) {
			break;
		}
	}

	if ( tblp == tblendp ) {
		return 0;
	}

	assert( tblp->tct_argcmin != 0 );
	if ( tranp->t_inter.i_argc < tblp->tct_argcmin ) {
		return 0;
	}

	if ( tranp->t_inter.i_argc > tblp->tct_argcmax ) {
		return 0;
	}

	return tblp->tct_ucb;
}

/* ARGSUSED */
static void
tsi_cmd_help( void *ctxp,
	      dlog_pcbp_t pcb,
	      void *pctxp )
{
	tsi_cmd_tbl_t *tblp = tsi_cmd_tbl;
	tsi_cmd_tbl_t *tblendp = tsi_cmd_tbl
				 +
				 sizeof( tsi_cmd_tbl )
				 /
				 sizeof( tsi_cmd_tbl[ 0 ] );

	( * pcb )( pctxp, _("the following commands are available:\n") );
	for ( ; tblp < tblendp ; tblp++ ) {
		( * pcb )( pctxp,
			   "\t%s %s\n",
			   tblp->tct_pattern,
			   tblp->tct_help );
	}
}

/* walks the tree following the given path.
 * returns FALSE if syntax error encountered.
 * returns by reference handles to the named node, its parent,
 * the first child in its cld list, its ino, if it is a directory,
 * and if it is selected.
 * optionally given a dlog print func and context, to be used for diag output.
 */
static bool_t
tsi_walkpath( char *arg, nh_t rooth, nh_t cwdh,
	      dlog_pcbp_t pcb, void *pctxp,
	      nh_t *namedhp, nh_t *parhp, nh_t *cldhp,
	      xfs_ino_t *inop, bool_t *isdirprp, bool_t *isselprp )
{
	nh_t namedh;
	nh_t parh;
	nh_t cldh;
	node_t *namedp;
	char *path;
	char nbuf[ NAME_MAX + 1 ];
	xfs_ino_t ino;
	bool_t isdirpr;
	bool_t isselpr;


	/* correct arg if ends with slash (if arg is "/", ok)
	 */
	if ( arg && strlen( arg ) > 1 && arg[ strlen( arg ) - 1 ] == '/' ) {
		arg[ strlen( arg ) - 1 ] = 0;
	}

	/* use path to walk down the path argument
	 */
	path = arg;

	/* walk the tree beginning either at the root node
	 * or at the current working directory
	 */
	if ( path && *path == '/' ) {
		assert( rooth != NH_NULL );
		namedh = rooth;
		path++;
	} else {
		assert( cwdh != NH_NULL );
		namedh = cwdh;
	}

	/* get the parent of the starting point, and its cld list
	 */
	namedp = Node_map( namedh );
	parh = namedp->n_parh;
	cldh = namedp->n_cldh;
	ino = namedp->n_ino;
	isselpr = ( namedp->n_flags & NF_SUBTREE );
	assert( namedp->n_flags & NF_ISDIR );
	Node_unmap( namedh, &namedp );
	isdirpr = BOOL_TRUE;

	/* walk the tree from the starting point following the path arg.
	 * on leaving this loop, the following will be valid:
	 *	namedh  - the handle of the node named by path arg;
	 *	parh - the parent of the named node;
	 *	isdirpr - TRUE if named node is a directory;
	 *	cldh - the first child in the named node's cld list.
	 */
	for ( ; ; ) {
		size_t namelen;
		char *strpatchp;
		nh_t sibh;

		/* if no path arg, break
		 */
		if ( ! path ) {
			break;
		}

		/* clean out leading slashes. these can occur if the
		 * path arg is ".../////..." or "////..."
		 */
		while ( *path == '/' ) {
			path++;
		}

		/* if empty path arg, break
		 */
		if ( ! strlen( path )) {
			break;
		}

		/* copy the first name from the path, and advance
		 * the path pointer.
		 */
		namelen = strcspn( path, "/" );
		assert( namelen < sizeof( nbuf ));
		strncpy( nbuf, path, namelen );
		nbuf[ namelen ] = 0;
		path += namelen;
		if ( *path ) {
			assert( *path == '/' );
			strpatchp = path;
			*strpatchp = 0;
			path++;
		} else {
			strpatchp = 0;
		}

		/* be sure the named node is a dir
		 */
		if ( ! isdirpr ) {
			if ( pcb ) {
				( * pcb )( pctxp, _(
					   "parent of %s is not a directory\n"),
					   arg );
			}
			return BOOL_FALSE;
		}

		/* special case "."
		 */
		if ( ! strcmp( nbuf, "." )) {
			if ( strpatchp ) {
				*strpatchp = '/';
			}
			continue;
		}

		/* special case ".."
		 */
		if ( ! strcmp( nbuf, ".." )) {
			if ( parh == NH_NULL ) {
				if ( pcb ) {
					( * pcb )( pctxp, _(
						   "%s above root\n"),
						   arg );
				}
				return BOOL_FALSE;
			}
			namedh = parh;
			namedp = Node_map( namedh );
			parh = namedp->n_parh;
			cldh = namedp->n_cldh;
			ino = namedp->n_ino;
			isselpr = ( namedp->n_flags & NF_SUBTREE );
			Node_unmap( namedh, &namedp );
			if ( strpatchp ) {
				*strpatchp = '/';
			}
			continue;
		}

		/* look for child with right name
		 */
		sibh = cldh;
		while ( sibh != NH_NULL ) {
			node_t *sibp;
			nh_t nextsibh;
			nrh_t nrh;
			/* REFERENCED */
			int siblen;

			sibp = Node_map( sibh );
			nrh = sibp->n_nrh;
			nextsibh = sibp->n_sibh;
			cldh = sibp->n_cldh;
			ino = sibp->n_ino;
			isselpr = ( sibp->n_flags & NF_SUBTREE );
			isdirpr = ( sibp->n_flags & NF_ISDIR );
			Node_unmap( sibh, &sibp );
			assert( nrh != NRH_NULL || sibh == persp->p_orphh );
			if ( nrh != NRH_NULL ) {
				siblen = namreg_get( nrh,
						     tranp->t_inter.i_name,
					       sizeof( tranp->t_inter.i_name ));
				assert( siblen > 0 );
				if ( ! strcmp( nbuf, tranp->t_inter.i_name )) {
					break;
				}
			}
			sibh = nextsibh;
		}

		/* if no match, complain
		 */
		if ( sibh == NH_NULL ) {
			if ( pcb ) {
				( * pcb )( pctxp, _(
					   "%s not found\n"),
					   arg );
			}
			return BOOL_FALSE;
		}

		/* continue search. cldh, ino and isdirpr
		 * set in inner loop above
		 */
		parh = namedh;
		namedh = sibh;
		if ( strpatchp ) {
			*strpatchp = '/';
		}
	}
	*namedhp = namedh;
	*parhp = parh;
	*cldhp = cldh;
	*inop = ino;
	*isdirprp = isdirpr;
	*isselprp = isselpr;

	return BOOL_TRUE;
}

/* Node abstraction *********************************************************/

static nh_t
Node_alloc( xfs_ino_t ino, gen_t gen, nrh_t nrh, dah_t dah, size_t flags )
{
	nh_t nh;
	node_t *np;

	nh = node_alloc( );
	if (nh == NH_NULL)
	    return NH_NULL;
	np = Node_map( nh );
	np->n_ino = ino;
	np->n_nrh = nrh;
	np->n_dah = dah;
	np->n_hashh = NH_NULL;
	np->n_parh = NH_NULL;
	np->n_sibh = NH_NULL;
	np->n_sibprevh = NH_NULL;
	np->n_cldh = NH_NULL;
	np->n_lnkh = NH_NULL;
	np->n_gen = gen;
	np->n_flags = ( u_char_t )flags;
	memset(np->n_pad, 0, sizeof(np->n_pad));
	Node_unmap( nh, &np  );
	return nh;
}

static void
Node_free( nh_t *nhp )
{
	node_t *np;
	np = Node_map( *nhp );
	np->n_ino = 0;
	np->n_gen = 0;
	if ( np->n_nrh != NRH_NULL ) {
		namreg_del( np->n_nrh );
		np->n_nrh = NRH_NULL;
	}
	if ( np->n_dah != DAH_NULL ) {
		dirattr_del( np->n_dah );
		np->n_dah = DAH_NULL;
	}
	np->n_flags = 0;
	np->n_parh = NH_NULL;
	np->n_sibh = NH_NULL;
	np->n_sibprevh = NH_NULL;
	np->n_cldh = NH_NULL;
	np->n_lnkh = NH_NULL;
	Node_unmap( *nhp, &np  );
	node_free( nhp );
}

/*
 * NOTE: Does error handling here and exits.
 */
static node_t *
Node_map( nh_t nh )
{
	node_t *n = ( node_t * )node_map( nh );
	if ( n == NULL ) {
		mlog( MLOG_ERROR | MLOG_TREE, _(
			"failed to map in node (node handle: %u)\n"), nh);
		exit(EXIT_ERROR);
	}
	return n;
}

static void
Node_unmap( nh_t nh, node_t **npp )
{
	node_unmap( nh, ( void ** )npp );
}

/* builds a pathname for the specified node, relative to root
 * returns FALSE if pathname too long
 */
static bool_t
Node2path( nh_t nh, char *path, char *errmsg )
{
	int remainingcnt;
	strcpy(path, "."); /* in case root node passed in */
	remainingcnt = Node2path_recurse( nh, path, MAXPATHLEN, 0 );
	if ( remainingcnt <= 0 ) {
		node_t *np = Node_map( nh );
		xfs_ino_t ino = np->n_ino;
		gen_t gen = np->n_gen;
		Node_unmap( nh, &np );
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
		      "unable %s ino %llu gen %u: "
		      "relative pathname too long (partial %s)\n"),
		      errmsg,
		      ino,
		      gen,
		      path );
		return BOOL_FALSE;
	} else {
		return BOOL_TRUE;
	}
}

/* returns how much of the buffer remains, assuming the buffer size is
 * MAXPATHLEN. always null-terminates, but null char not counted in return.
 * works because the buffer size is secretly 2 * MAXPATHLEN.
 */
static int
Node2path_recurse( nh_t nh, char *buf, int bufsz, int level )
{
	static __thread path_cache_t cache = { NH_NULL, 0, "" };
	node_t *np;
	nh_t parh;
	xfs_ino_t ino;
	gen_t gen;
	nrh_t nrh;
	char *oldbuf;
	int oldbufsz;
	int namelen;

	/* recursion termination
	 */
	if ( nh == persp->p_rooth ) {
		return bufsz;
	}

	/* if we have a cache hit, no need to recurse any further
	 */
	if ( nh == cache.nh ) {
		assert( bufsz > cache.len );
		strcpy( buf, cache.buf );
		return bufsz - cache.len;
	}

	/* extract useful node members
	 */
	np = Node_map( nh );
	parh = np->n_parh;
	ino = np->n_ino;
	gen = np->n_gen;
	nrh = np->n_nrh;
	Node_unmap( nh, &np );

	/* build path to parent
	 */
	oldbuf = buf;
	oldbufsz = bufsz;
	bufsz = Node2path_recurse( parh, buf, bufsz, level+1 ); /* RECURSION */
	if ( bufsz <= 0 ) {
		return bufsz;
	}
	buf += oldbufsz - bufsz;

	/* insert slash if parent not root
	 */
	if ( parh != persp->p_rooth ) {
		assert( bufsz + MAXPATHLEN >= 2 );
		*buf++ = '/';
		*( buf + 1 ) = 0;
		bufsz--;
		if ( bufsz <= 0 ) {
			return bufsz;
		}
	}

	/* append entry name: special case if in orphanage
	 */
	if ( parh == persp->p_orphh ) {
		namelen = sprintf( buf, "%llu.%u", (unsigned long long)ino, gen );
	} else if ( nh == persp->p_orphh ) {
		namelen = sprintf( buf, "%s", orphname );
	} else {
		assert( nrh != NRH_NULL );
		namelen = namreg_get( nrh, buf, ( size_t )bufsz + MAXPATHLEN );
		assert( namelen > 0 );
	}

	/* update remaining buffer size
	 */
	bufsz -= namelen;
	assert( bufsz + MAXPATHLEN > 0 );

	/* update the cache if we're the target's parent
	 * (and the pathname is not too long)
	 */
	if ( level == 1 && bufsz > 0 ) {
		cache.nh = nh;
		strcpy( cache.buf, oldbuf );
		cache.len = oldbufsz - bufsz;
	}

	/* return remaining buffer size
	 */
	return bufsz;
}

/* family abstraction *********************************************************/

static void
adopt( nh_t parh, nh_t cldh, nrh_t nrh )
{
	node_t *parp;
	node_t *cldp;
	node_t *sibp;

#ifdef TREE_DEBUG
	mlog(MLOG_DEBUG | MLOG_TREE,
	   "adopt(parent=%llu,child=%llu,node=%llu): \n",
	   parh, cldh, nrh);
#endif

	/* fix up our child - put at front of child list */
	cldp = Node_map( cldh );
	cldp->n_parh = parh;
	cldp->n_nrh = nrh;
	parp = Node_map( parh );
	cldp->n_sibh = parp->n_cldh;
	cldp->n_sibprevh = NH_NULL;
	Node_unmap( cldh, &cldp  );

	/* fix up old first child i.e. child's new sibling */
	if ( parp->n_cldh != NH_NULL ) { /* if parent has a child */
	    sibp = Node_map( parp->n_cldh );
	    sibp->n_sibprevh = cldh;
	    Node_unmap( parp->n_cldh, &sibp );
	}

        /* fix up parent */
	parp->n_cldh = cldh;
	Node_unmap( parh, &parp  );
}

static nrh_t
disown( nh_t cldh )
{
	node_t *cldp;
	nrh_t nrh;
	nh_t parh;
	node_t *parp;

	cldp = Node_map( cldh );

	nrh = cldp->n_nrh;

	parh = cldp->n_parh;
	assert( parh != NH_NULL );
	if ( parh == NH_NULL ) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
		      "attempt to disown child "
		      "which has no parent!\n") );
		return nrh;
	}
	parp = Node_map( parh );
	assert( parp->n_cldh != NH_NULL );
	if ( parp->n_cldh == NH_NULL ) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
		      "attempt to disown child "
		      "when parent has no children!\n") );
		return nrh;
	}
	if ( parp->n_cldh == cldh ) {
		/* child is the first one in the child list */
		parp->n_cldh = cldp->n_sibh;
		if ( cldp->n_sibh != NH_NULL ) {
			node_t *sibp = Node_map( cldp->n_sibh );
			sibp->n_sibprevh = NH_NULL; 
			Node_unmap( cldp->n_sibh, &sibp );
		}
	} else {
		/* child is further down the child list */
		/* use double link list to find previous link */
		nh_t prevcldh = cldp->n_sibprevh;
		node_t *prevcldp;

		assert(prevcldh != NH_NULL); /* must be a previous */
		prevcldp = Node_map( prevcldh );

		/* fix up previous */
		prevcldp->n_sibh = cldp->n_sibh; 
		Node_unmap( prevcldh, &prevcldp  );

		/* fix up next */
		if ( cldp->n_sibh != NH_NULL ) {
			node_t *sibp = Node_map( cldp->n_sibh );
			sibp->n_sibprevh = prevcldh; 
			Node_unmap( cldp->n_sibh, &sibp );
		}
	}
	Node_unmap( parh, &parp  );
	cldp->n_parh = NH_NULL;
	cldp->n_sibh = NH_NULL;
	cldp->n_sibprevh = NH_NULL;
	Node_unmap( cldh, &cldp  );

	return nrh;
}

/* recursively marks all nodes in subtree as selected or not selected
 * for subtree restoral. adjusts ancestors flags accordingly. also adjusts
 * inomap, which will be used by content.c to see if a media file contains
 * any nondirs which might need to be restored.
 */
static void
selsubtree( nh_t nh, bool_t sensepr )
{
	node_t *np;
	nh_t parh;

	/* first mark the subtree
	 */
	selsubtree_recurse_down( nh, sensepr );

	/* get parent
	 */
	np = Node_map( nh );
	parh = np->n_parh;
	Node_unmap( nh, &np );

	/* next adjust ancestory
	 */
	while ( parh != NH_NULL ) {
		node_t *parp;
		nh_t newparh;

		parp = Node_map( parh );
		if ( sensepr ) {
			parp->n_flags |= NF_SUBTREE;
		} else {
			bool_t atleastonechildselpr = BOOL_FALSE;
			nh_t cldh = parp->n_cldh;
			while ( cldh != NH_NULL ) {
				node_t *cldp;
				nh_t nextcldh;
				bool_t cldsensepr;
				cldp = Node_map( cldh );
				cldsensepr = ( cldp->n_flags & NF_SUBTREE )
					     ?
					     BOOL_TRUE
					     :
					     BOOL_FALSE;
				nextcldh = cldp->n_sibh;
				Node_unmap( cldh, &cldp );
				if ( cldsensepr ) {
					atleastonechildselpr = BOOL_TRUE;
					break;
				}
				cldh = nextcldh;
			}
			if ( ! atleastonechildselpr ) {
				parp->n_flags &= ~NF_SUBTREE;
				/* DBG could break out here (remember to unmap!)
				 */
			}
		}
		newparh = parp->n_parh;
		Node_unmap( parh, &parp );
		parh = newparh;
	}
}

static void
selsubtree_recurse_down( nh_t nh, bool_t sensepr )
{
	nh_t cldh;

	/* first mark the node indicated, and get head of cld list
	 */
	{
		node_t *np;
		bool_t isdirpr;
		xfs_ino_t ino;
		gen_t gen;

		np = Node_map( nh );
		if ( sensepr ) {
			np->n_flags |= NF_SUBTREE;
		} else {
			np->n_flags &= ~NF_SUBTREE;
		}
		cldh = np->n_cldh;
		ino = np->n_ino;
		gen = np->n_gen;
		isdirpr = ( np->n_flags & NF_ISDIR );
		Node_unmap( nh, &np  );
		if ( ! isdirpr ) {
			if ( sensepr ) {
				inomap_rst_add( ino );
			} else {
				/* check hardlist: don't del unless none needed
				 */
				nh_t nh;
				bool_t neededpr = BOOL_FALSE;
				for ( nh = link_hardh( ino, gen )
				      ;
				      nh != NH_NULL
				      ;
				      nh = link_nexth( nh )) {
					node_t *np;
					u_char_t flags;
					np = Node_map( nh );
					flags = np->n_flags;
					Node_unmap( nh, &np  );
					if ( flags & NF_SUBTREE ) {
						neededpr = BOOL_TRUE;
						break;
					}
				}
				if ( ! neededpr ) {
					inomap_rst_del( ino );
				}
			}
		}
	}

	/* then mark all of its children. be sure to skip the orphanage!!!
	 */
	while ( cldh != NH_NULL ) {
		node_t *cldp;
		nh_t nextcldh;

		if ( cldh != persp->p_orphh ) {
			selsubtree_recurse_down( cldh, sensepr );
		}
		cldp = Node_map( cldh );
		nextcldh = cldp->n_sibh;
		Node_unmap( cldh, &cldp  );
		cldh = nextcldh;
	}
}


/* link abstraction *********************************************************/

/* returns handle to head of hard link list
 */
static nh_t
link_hardh( xfs_ino_t ino, gen_t gen )
{
	return hash_find( ino, gen );
}

/* returns following node in hard link list
 */
static nh_t
link_nexth( nh_t nh )
{
	node_t *np;
	nh_t nexth;

	np = Node_map( nh );
	nexth = np->n_lnkh;
	Node_unmap( nh, &np );
	return nexth;
}

/* searches hard link list for exact match.
 * returns hard link list head
 */
static nh_t
link_matchh( nh_t hardh, nh_t parh, char *name )
{
	while ( hardh != NH_NULL ) {
		node_t *np;
		nh_t nexth;
		np = Node_map( hardh );
		if ( np->n_parh == parh ) {
			/* REFERENCED */
			int namelen;
			namelen = namreg_get( np->n_nrh,
					      tranp->t_namebuf,
					      sizeof( tranp->t_namebuf ));
			assert( namelen > 0 );
			if ( ! strcmp( name, tranp->t_namebuf )) {
				Node_unmap( hardh, &np );
				break;
			}
		}
		nexth = np->n_lnkh;
		Node_unmap( hardh, &np );
		hardh = nexth;
	}
	return hardh;
}

static void
link_in( nh_t nh )
{
	node_t *np;
	xfs_ino_t ino;
	gen_t gen;
	nh_t hardh;

#ifdef TREE_DEBUG
	mlog(MLOG_DEBUG | MLOG_TREE,
	    "link_in(%llu): map in node\n", nh);
#endif

	/* map in the node to read ino and gen
	 */
	np = Node_map( nh );
	ino = np->n_ino;
	gen = np->n_gen;

	/* see if one or more links already hashed.
	 */
	hardh = hash_find( ino, gen );

	/* if not hashed, just hash it. otherwise put at end
	 * of hard link (lnk) list.
	 */
	if ( hardh == NH_NULL ) {
#ifdef TREE_DEBUG
		mlog(MLOG_DEBUG | MLOG_TREE,
		    "link_in(): hash node in for ino %llu\n", ino);
#endif
		hash_in( nh );
	} else {
		nh_t prevh = hardh;
		node_t *prevp = Node_map( prevh );
#ifdef TREE_DEBUG
		mlog(MLOG_DEBUG | MLOG_TREE,
		    "link_in(): put at end of hash list\n");
#endif
		while ( prevp->n_lnkh != NH_NULL ) {
			nh_t nexth = prevp->n_lnkh;
			Node_unmap( prevh, &prevp  );
			prevh = nexth;
			prevp = Node_map( prevh );
		}
		prevp->n_lnkh = nh;
		Node_unmap( prevh, &prevp  );
	}

	/* since always put at end of hard link list, make node's
	 * lnk member terminate list.
	 */
	np->n_lnkh = NH_NULL;
	Node_unmap( nh, &np  );
#ifdef TREE_DEBUG
	mlog(MLOG_DEBUG | MLOG_TREE,
	    "link_in(%llu): UNmap in node\n", nh);
#endif
}

static void
link_out( nh_t nh )
{
	node_t *np;
	xfs_ino_t ino;
	gen_t gen;
	nh_t hardh;

	/* map in the node to read ino and gen
	 */
	np = Node_map( nh );
	ino = np->n_ino;
	gen = np->n_gen;

	/* get head of hard link list
	 */
	hardh = hash_find( ino, gen );
	assert( hardh != NH_NULL );

	/* if node is at head of hard link list, hash it out and
	 * hash in the following node in link list, if there is one.
	 * otherwise, unlink from hardlink list.
	 */
	if ( nh == hardh ) {
		hash_out( nh );
		if ( np->n_lnkh != NH_NULL ) {
			hash_in( np->n_lnkh );
		}
	} else {
		nh_t prevh = hardh;
		node_t *prevp = Node_map( prevh );
		while ( prevp->n_lnkh != nh ) {
			nh_t nexth = prevp->n_lnkh;
			Node_unmap( prevh, &prevp  );
			prevh = nexth;
			assert( prevh != NH_NULL );
			prevp = Node_map( prevh );
		}
		prevp->n_lnkh = np->n_lnkh;
		Node_unmap( prevh, &prevp  );
	}
	np->n_lnkh = NH_NULL;

	/* release the mapping
	 */
	Node_unmap( nh, &np  );
}

/* invokes callback for all hardheads
 * iteration aborted if callback returns FALSE
 */
static void
link_headiter( bool_t ( * cbfp )( void *contextp, nh_t hardh ), void *contextp )
{
	hash_iter( cbfp, contextp );
}

/* iterator for a hard link list. allows deletion of the last node
 * returned.
 */
static void
link_iter_init( link_iter_context_t *link_iter_contextp, nh_t hardheadh )
{
	link_iter_contextp->li_headh = hardheadh;
	link_iter_contextp->li_prevh = NH_NULL;
	link_iter_contextp->li_lasth = NH_NULL;
	link_iter_contextp->li_donepr = BOOL_FALSE;
}

static nh_t
link_iter_next( link_iter_context_t *link_iter_contextp )
{
	node_t *lastp;
	nh_t tmplasth;

	/* if already done, return
	 */
	if ( link_iter_contextp->li_donepr == BOOL_TRUE ) {
		return NH_NULL;
	}
		
	/* if no hardhead, done
	 */
	if ( link_iter_contextp->li_headh == NH_NULL ) {
		link_iter_contextp->li_donepr = BOOL_TRUE;
		return NH_NULL;
	}

	/* make tmp copy of last
	 */
	tmplasth = link_iter_contextp->li_lasth;

	/* if no last, must be first call
	 */
	if ( tmplasth == NH_NULL ) {
		assert( link_iter_contextp->li_prevh == NH_NULL );
		link_iter_contextp->li_lasth = link_iter_contextp->li_headh;
		return link_iter_contextp->li_lasth;
	}

	/* slide last into prev
	 */
	link_iter_contextp->li_prevh = tmplasth;
	lastp = Node_map( tmplasth );
	link_iter_contextp->li_lasth = lastp->n_lnkh;
	Node_unmap( tmplasth, &lastp );

	/* if NULL, flag done
	 */
	if ( link_iter_contextp->li_lasth == NH_NULL ) {
		link_iter_contextp->li_donepr = BOOL_TRUE;
	}

	/* return the last handle
	 */
	return link_iter_contextp->li_lasth;
}

/* ARGSUSED */
void
link_iter_unlink( link_iter_context_t *link_iter_contextp, nh_t nh )
{
	node_t *lastp;
	nh_t nexth;

	/* sanity checks
	 */
	assert( link_iter_contextp->li_lasth != NH_NULL );
	assert( nh == link_iter_contextp->li_lasth );

	/* get the next node in list
	 */
	lastp = Node_map( link_iter_contextp->li_lasth );
	nexth = lastp->n_lnkh;
	lastp->n_lnkh = NH_NULL;
	Node_unmap( link_iter_contextp->li_lasth, &lastp );

	if ( link_iter_contextp->li_lasth == link_iter_contextp->li_headh ) {
		assert( link_iter_contextp->li_prevh == NH_NULL );
		hash_out( link_iter_contextp->li_headh );
		link_iter_contextp->li_headh = nexth;
		if ( nexth != NH_NULL ) {
			hash_in( nexth );
		}
	} else {
		node_t *prevp;
		assert( link_iter_contextp->li_prevh != NH_NULL );
		prevp = Node_map( link_iter_contextp->li_prevh );
		prevp->n_lnkh = nexth;
		Node_unmap( link_iter_contextp->li_prevh, &prevp );
	}
	link_iter_contextp->li_lasth = link_iter_contextp->li_prevh;
	link_iter_contextp->li_prevh = NH_NULL;
}


/* hash abstraction *********************************************************/

#define HASHLEN_MIN	( pgsz / sizeof( nh_t ))

static bool_t
hash_init( size64_t vmsz,
	   size64_t dircnt,
	   size64_t nondircnt,
	   char *perspath )
{
	size64_t hashlen;
	size64_t loghashlen;
	size64_t vmlen;
	size64_t hashlenmax;
	ix_t hix;

	/* sanity checks
	 */
	assert( pgsz % sizeof( nh_t ) == 0 );

	/* calculate the size of the hash array. must be a power of two,
	 * and a multiple of the page size. don't use more than the available
	 * vm. but enforce a minimum.
	 */
	vmlen = vmsz / sizeof( nh_t );
	hashlenmax = min( vmlen, SIZEMAX );
	hashlen = ( dircnt + nondircnt );
	hashlen = max( hashlen, ( size64_t )HASHLEN_MIN );
	hashlen = min( hashlen, hashlenmax );

	for ( loghashlen = 0
	      ;
	      ( ( size64_t )1 << loghashlen ) <= hashlen
	      ;
	      loghashlen++ )
		;
	assert( loghashlen > 0 );
	hashlen = ( size64_t )1 << loghashlen;
	if (hashlen > hashlenmax)
		hashlen >>= 1;
	assert( hashlen <= hashlenmax );

	/* record hash size in persistent state
	 */
	persp->p_hashsz = hashlen * sizeof( nh_t );

	/* map the hash array just after the persistent state header
	 */
	assert( persp->p_hashsz <= SIZEMAX );
	assert( ! ( persp->p_hashsz % ( size64_t )pgsz ));
	assert( ! ( PERSSZ % pgsz ));
	tranp->t_hashp = ( nh_t * ) mmap_autogrow(
					    ( size_t )persp->p_hashsz,
					    tranp->t_persfd,
					    ( off64_t )PERSSZ );
	if ( tranp->t_hashp == ( nh_t * )-1 ) {
		mlog( MLOG_NORMAL | MLOG_TREE, _(
		      "unable to mmap hash array into %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* initialize the hash array to all NULL node handles
	 */
	for ( hix = 0 ; hix < ( ix_t )hashlen ; hix++ ) {
		tranp->t_hashp[ hix ] = NH_NULL;
	}

	/* build a hash mask. this works because hashlen is a power of two.
	 * record in persistent state.
	 */
	assert( hashlen - 1 <= SIZEMAX );
	persp->p_hashmask = ( size_t )( hashlen - 1 );

	return BOOL_TRUE;
}

static bool_t
hash_sync( char *perspath )
{
	size64_t hashsz;

	/* sanity checks
	 */
	assert( pgsz % sizeof( nh_t ) == 0 );

	/* retrieve the hash size from the persistent state
	 */
	hashsz = persp->p_hashsz;
	assert( ! ( hashsz % sizeof( nh_t )));

	/* map the hash array just after the persistent state header
	 */
	assert( hashsz <= SIZEMAX );
	assert( ! ( hashsz % ( size64_t )pgsz ));
	assert( ! ( PERSSZ % pgsz ));
	tranp->t_hashp = ( nh_t * ) mmap_autogrow(
					    ( size_t )hashsz,
					    tranp->t_persfd,
					    ( off64_t )PERSSZ );
	if ( tranp->t_hashp == ( nh_t * )-1 ) {
		mlog( MLOG_NORMAL | MLOG_TREE, _(
		      "unable to mmap hash array into %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	return BOOL_TRUE;
}

static inline size_t
hash_val(xfs_ino_t ino, size_t hashmask)
{
	ino += ~(ino << 15);
	ino ^=  (ino >> 10);
	ino +=  (ino << 3);
	ino ^=  (ino >> 6);
	ino += ~(ino << 11);
	ino ^=  (ino >> 16);
	return (size_t)ino & hashmask;
}

static void
hash_in( nh_t nh )
{
	node_t *np;
	xfs_ino_t ino;
	size_t hix;
	nh_t *entryp;

	/* get a mapping to the node
	 */
	np = Node_map( nh );

	/* get ino from node
	 */
	ino = np->n_ino;
	
	/* assert not already in
	 */
	assert( hash_find( np->n_ino, np->n_gen ) == NH_NULL );

	/* calculate the hash index
	 */
	hix = hash_val(ino, persp->p_hashmask);

	/* get a pointer to the indexed hash array entry
	 */
	entryp = &tranp->t_hashp[ hix ];

	/* insert into the list, at the head
	 */
	assert( np->n_hashh == NH_NULL );
	np->n_hashh = *entryp;
	*entryp = nh;

	/* release the mapping
	 */
	Node_unmap( nh, &np  );
}

static void
hash_out( nh_t nh )
{
	node_t *np;
	xfs_ino_t ino;
	nh_t hashheadh;
	size_t hix;
	nh_t *entryp;

	/* get a mapping to the node
	 */
	np = Node_map( nh );

	/* get the ino
	 */
	ino = np->n_ino;

	/* get a pointer to the hash array entry
	 */
	hix = hash_val(ino, persp->p_hashmask);
	entryp = &tranp->t_hashp[ hix ];

	/* get the handle of the first node in the appropriate hash array
	 */
	hashheadh = *entryp;
	assert( hashheadh != NH_NULL );
	
	/* if node is first in list, replace entry with following node.
	 * otherwise, walk the list until found.
	 */
	if ( hashheadh == nh ) {
		*entryp = np->n_hashh;
	} else {
		nh_t prevh = hashheadh;
		node_t *prevp = Node_map( prevh );
		while ( prevp->n_hashh != nh ) {
			nh_t nexth = prevp->n_hashh;
			Node_unmap( prevh, &prevp  );
			prevh = nexth;
			assert( prevh != NH_NULL );
			prevp = Node_map( prevh );
		}
		prevp->n_hashh = np->n_hashh;
		Node_unmap( prevh, &prevp  );
	}
	np->n_hashh = NH_NULL;

	/* release the mapping
	 */
	Node_unmap( nh, &np  );
}

static nh_t
hash_find( xfs_ino_t ino, gen_t gen )
{
	nh_t nh;
	node_t *np;
	size_t hix;

	/* get handle to first node in appropriate hash array
	 */
	hix = hash_val(ino, persp->p_hashmask);
	nh = tranp->t_hashp[ hix ];

	/* if list empty, return null handle
	 */
	if ( nh == NH_NULL ) {
		return NH_NULL;
	}

#ifdef TREE_DEBUG
	mlog(MLOG_DEBUG | MLOG_TREE,
	     "hash_find(%llu,%u): traversing hash link list\n",
		ino, gen); 
#endif

	/* walk the list until found.
	 */
	np = Node_map( nh );
	while ( np->n_ino != ino || np->n_gen != gen ) {
		nh_t nextnh = np->n_hashh;
		Node_unmap( nh, &np  );
		nh = nextnh;
		if ( nh == NH_NULL ) {
			return NH_NULL;
		}
		np = Node_map( nh );
	}
	Node_unmap( nh, &np  );

#ifdef TREE_DEBUG
	mlog(MLOG_DEBUG | MLOG_TREE,
	    "hash_find(): return nh = %llu\n", nh);
#endif
	return nh;
}

/* invokes callback for all hashed nodes
 * iteration aborted if callback returns FALSE
 * call back may hash out and free the node, so
 * must figure next node prior to calling callback.
 */
static void
hash_iter( bool_t ( * cbfp )( void *contextp, nh_t hashh ), void *contextp )
{
	ix_t hix;
	size64_t hashlen = persp->p_hashsz / sizeof( nh_t );

	for ( hix = 0 ; hix < ( ix_t )hashlen ; hix++ ) {
		nh_t nh = tranp->t_hashp[ hix ];

		while ( nh != NH_NULL ) {
			node_t *np;
			nh_t nexth;
			bool_t ok;

			np = Node_map( nh );
			nexth = np->n_hashh;
			Node_unmap( nh, &np );

			ok = ( * cbfp )( contextp, nh );
			if ( ! ok ) {
				return;
			}

			nh = nexth;
		}
	}
}

/* misc static functions *****************************************************/

#ifdef TREE_CHK
/* use hash array to iterate through all nodes. check
 * each node's hash, hardlink, namreg, dirattr, parent,
 * and sibling handles.
 */
static bool_t
Node_chk( nh_t nh, nh_t *nexthashhp, nh_t *nextlnkhp )
{
	node_t *np;
	node_t n;
	char nambuf[ NAME_MAX + 1 ];
	bool_t okaccum;

	mlog( MLOG_NITTY + 1 | MLOG_TREE,
	      "checking node nh == 0x%x\n",
	      nh );

	okaccum = BOOL_TRUE;

	if ( nexthashhp ) {
		*nexthashhp = NH_NULL;
	}

	assert( nextlnkhp );
	*nextlnkhp = NH_NULL;

	np = Node_map( nh );
	assert( np );
	n = *np;
	Node_unmap( nh, &np );

	if ( ! nexthashhp && n.n_hashh != NH_NULL ) {
		mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_TREE, _(
		      "nh 0x%x np 0x%x hash link not null\n"),
		      nh,
		      np );
		okaccum = BOOL_FALSE;
	}

	if ( n.n_hashh != NH_NULL ) {
		np = Node_map( n.n_hashh );
		Node_unmap( n.n_hashh, &np );
	}

	if ( n.n_lnkh != NH_NULL ) {
		np = Node_map( n.n_lnkh );
		Node_unmap( n.n_lnkh, &np );
	}

	if ( n.n_parh != NH_NULL ) {
		np = Node_map( n.n_parh );
		Node_unmap( n.n_parh, &np );
	}

	if ( n.n_cldh != NH_NULL ) {
		np = Node_map( n.n_cldh );
		Node_unmap( n.n_cldh, &np );
	}

	if ( n.n_sibh != NH_NULL ) {
		np = Node_map( n.n_sibh );
		Node_unmap( n.n_sibh, &np );
	}

	if ( n.n_nrh != NRH_NULL ) {
		int rval;
		rval = namreg_get( n.n_nrh, nambuf, sizeof( nambuf ));
		assert( rval >= 0 );
	}

	if ( n.n_dah != DAH_NULL ) {
		( void )dirattr_get_mode( n.n_dah );
	}

	if ( nexthashhp ) {
		*nexthashhp = n.n_hashh;
	}

	*nextlnkhp = n.n_lnkh;

	return okaccum;
}

bool_t
tree_chk( void )
{
	ix_t hix;
	size64_t hashlen = persp->p_hashsz / sizeof( nh_t );
	bool_t ok;
	bool_t okaccum;

	okaccum = BOOL_TRUE;

	for ( hix = 0 ; hix < ( ix_t )hashlen ; hix++ ) {
		nh_t hashh = tranp->t_hashp[ hix ];

		mlog( MLOG_NITTY + 1 | MLOG_TREE,
		      "checking hix %u\n",
		      hix );
		while ( hashh != NH_NULL ) {
			nh_t lnkh;

			ok = Node_chk( hashh, &hashh, &lnkh );
			if ( ! ok ) {
				okaccum = BOOL_FALSE;
			}

			while ( lnkh != NH_NULL ) {
				ok = Node_chk( lnkh, 0, &lnkh );
				if ( ! ok ) {
					okaccum = BOOL_FALSE;
				}
			}
		}
	}

	ok = tree_chk2( );
	if ( ! ok ) {
		okaccum = BOOL_FALSE;
	}

	return okaccum;
}

static bool_t tree_chk2_recurse( nh_t cldh, nh_t parh );

static bool_t
tree_chk2( void )
{
	node_t *rootp;
	nh_t cldh;
	bool_t ok;

	mlog( MLOG_DEBUG | MLOG_TREE,
	      "tree hierarchy check\n" );

	rootp = Node_map( persp->p_rooth );
	cldh = rootp->n_cldh;
	Node_unmap( persp->p_rooth, &rootp );

	ok = tree_chk2_recurse( cldh, persp->p_rooth );

	return ok;
}

static bool_t
tree_chk2_recurse( nh_t cldh, nh_t parh )
{
	bool_t okaccum = BOOL_TRUE;

	assert( parh != NH_NULL );

	while ( cldh != NH_NULL ) {
		node_t *cldp;
		xfs_ino_t ino;
		gen_t gen;
		nh_t nodeparh;
		nrh_t nrh;
		nh_t grandcldh;
		nh_t nextcldh;
		bool_t ok;

		cldp = Node_map( cldh );
		ino = cldp->n_ino;
		gen = cldp->n_gen;
		nodeparh = cldp->n_parh;
		nrh = cldp->n_nrh;
		grandcldh = cldp->n_cldh;
		nextcldh = cldp->n_sibh;
		Node_unmap( cldh, &cldp );

		if ( parh == persp->p_orphh ) {
			sprintf( tranp->t_namebuf, "%llu.%u", ino, gen );
		} else if ( cldh == persp->p_orphh ) {
			sprintf( tranp->t_namebuf, "%llu.%u", ino, gen );
		} else {
			int namelen;
			namelen = namreg_get( nrh,
					      tranp->t_namebuf,
					      sizeof( tranp->t_namebuf ));
			assert( namelen >= 0 );
		}

		if ( nodeparh == NH_NULL ) {
			mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
			      "node %x %s %llu %u parent NULL\n"),
			      cldh,
			      tranp->t_namebuf,
			      ino,
			      gen );
			return BOOL_FALSE;
		} else if ( nodeparh != parh ) {
			mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_TREE, _(
			      "node %x %s %llu %u parent mismatch: "
			      "nodepar %x par %x\n"),
			      cldh,
			      tranp->t_namebuf,
			      ino,
			      gen,
			      nodeparh,
			      parh );
			return BOOL_FALSE;
		} else {
			mlog( MLOG_DEBUG | MLOG_TREE,
			      "node %x %s %llu %u  parent %x\n",
			      cldh,
			      tranp->t_namebuf,
			      ino,
			      gen,
			      parh );
		}
		ok = tree_chk2_recurse( grandcldh, cldh );
		if ( ! ok ) {
			okaccum = BOOL_FALSE;
		}

		cldh = nextcldh;
	}

	return okaccum;
}

#endif /* TREE_CHK */

static char *whites = " \t\r\n\v\f";

static int is_white( char c );
static void fix_escape( char *string, char *liter );
static void fix_quoted_span( char *string, char *liter );
static void collapse_white( char *string, char *liter );
static size_t distance_to_space( char *s, char *l );

static int
parse( int slotcnt, char **slotbuf, char *string )
{
	char *liter;
	char *s;
	char *l;
	size_t wordcnt;

	/* sanity checkcs
	 */
	assert( slotcnt >= 0 );

	/* allocate a companion to the input string for identifying
	 * characters which are to be interpreted literally.
	 */
	liter = ( char * )calloc( 1, strlen( string ) + 1 );
	if ( ! liter ) {
		return -1;
	}

	/* pass 1: collapse escape sequences, identifying characters which
	 * are to be interpreted literally
	 */
	for ( s = string, l = liter ; *s ; s++, l++ ) {
		if ( *s == '\\' && ! *l ) {
			fix_escape( s, l );
		}
	}

	/* pass 2: collapse quoted spans, identifying characters which
	 * are to be interpreted literally
	 */
	for ( s = string, l = liter ; *s ; s++, l++ ) {
		if ( *s == '\"' && ! *l ) {
			fix_quoted_span( s, l );
		}
	}

	/* pass 3: collapse white space spans into a single space
	 */
	for ( s = string, l = liter ; *s ; s++, l++ ) {
		if ( is_white( *s ) && ! *l ) {
			collapse_white( s, l );
		}
	}

	/* pass 4: identify and null-terminate words
	 */
	wordcnt = 0;
	s = string;
	l = liter;
	while ( *s ) {
		size_t d;
		if ( wordcnt < ( size_t )slotcnt ) {
			slotbuf[ wordcnt ] = s;
		}
		wordcnt++;
		d = distance_to_space( s, l );
		s += d;
		l += d;
		if ( *s ) {
			*s = 0;
			s++;
			l++;
		}
	}

	free( ( void * )liter );
	return ( int )wordcnt;
}

struct escape_table {
	char sequence;
	char substitute;
};
typedef struct escape_table escape_table_t;

static escape_table_t escape_table[ ] = {
 {	'n',	'\n' },
 {	't',	'\t' },
 {	'v',	'\v' },
 {	'b',	'\b' },
 {	'r',	'\r' },
 {	'f',	'\f' },
 {	'f',	'\f' },
 {	'a',	'\a' },
 {	'\\',	'\\' }
};

static void shrink( char *s, size_t cnt );
static int is_hex( char c );
static size_t hex_to_size( char c );
static int is_octal( char c );
static size_t octal_to_size( char c );

static void
fix_escape( char *string, char *liter )
{
	escape_table_t *ep;
	escape_table_t *endep;

	/* first look for special escapes described in table
	 */
	ep = escape_table;
	endep = escape_table + ( sizeof( escape_table )
			         /
			         sizeof( escape_table[ 0 ] ));
	for ( ; ep < endep ; ep++ ) {
		if ( string[ 1 ] == ep->sequence ) {
			string[ 0 ] = ep->substitute;
			liter[ 0 ] = ( char )1;
			shrink( &string[ 1 ], 1 );
			shrink( &liter[ 1 ], 1 );
			return;
		}
	}

	/* detect white space escapes
	 */
	if ( is_white( string[ 1 ] )) {
		liter[ 0 ] = ( char )1;
		shrink( &string[ 0 ], 1 );
		shrink( &liter[ 1 ], 1 );
		return;
	}

	/* detect hex escapes (don't allow null)
	 */
	if ( string[ 1 ] == 'x' ) {
		size_t hexlen;
		size_t accum;
		accum = 0;
		for ( hexlen = 0
		      ;
		      hexlen < 2 && is_hex( string[ 2 + hexlen ] )
		      ;
		      hexlen++ ) {
			accum *= 16;
			accum += hex_to_size( string[ 2 + hexlen ] );
		}
		if ( hexlen && accum ) {
			string[ 0 ] = ( char )accum;
			liter[ 0 ] = ( char )1;
			shrink( &string[ 1 ], 1 + hexlen );
			shrink( &liter[ 1 ], 1 + hexlen );
			return;
		}
	}

	/* detect octal escapes (don't allow null)
	 */
	if ( is_octal( string[ 1 ] )) {
		size_t octallen;
		size_t accum;
		accum = octal_to_size( string[ 1 ] );
		for ( octallen = 1
		      ;
		      octallen < 3 && is_octal( string[ 1 + octallen ] )
		      ;
		      octallen++ ) {
			accum *= 8;
			accum += octal_to_size( string[ 1 + octallen ] );
		}
		if ( accum ) {
			string[ 0 ] = ( char )accum;
			liter[ 0 ] = ( char )1;
			shrink( &string[ 1 ], octallen );
			shrink( &liter[ 1 ], octallen );
			return;
		}
	}

	/* didn't match any escape sequences, so assume literal
	 */
	liter[ 0 ] = ( char )1;
}

static void
fix_quoted_span( char *string, char *liter )
{
	char *s;
	char *l;

	/* first cover the leading quote
	 */
	shrink( string, 1 );
	shrink( liter, 1 );

	/* scan for the next non-literal quote, marking all
	 * characters in between as literal
	 */
	for ( s = string, l = liter ; *s && ( *s != '\"' || *l ) ; s++, l++ ) {
		*l = ( char )1;
	}

	if ( *s ) {
		shrink( s, 1 );
		shrink( l, 1 );
	}
}

static void
collapse_white( char *string, char *liter )
{
	char *s;
	char *l;
	size_t cnt;

	cnt = 0;
	for ( s = string, l = liter ; is_white( *s ) && ! *l ; s++, l++ ) {
		cnt++;
	}
	
	string[ 0 ] = ' ';
	
	if ( cnt > 1 ) {
		shrink( &string[ 1 ], cnt - 1 );
		shrink( &liter[ 1 ], cnt - 1 );
	}
}

static size_t
distance_to_space( char *s, char *l )
{
	size_t cnt;

	for ( cnt = 0 ; *s && ( ! is_white( *s ) || *l ) ; s++, l++ ) {
		cnt++;
	}

	return cnt;
}

static void
shrink( char *s, size_t cnt )
{
	strcpy( s, s + cnt );
}

static int
is_white( char c )
{
	if ( c && strchr( whites, ( int )c )) {
		return 1;
	} else {
		return 0;
	}
}

static int
is_hex( char c )
{
	if ( c >= '0' && c <= '9' ) {
		return 1;
	}

	if ( c >= 'a' && c <= 'f' ) {
		return 1;
	}

	if ( c >= 'A' && c <= 'F' ) {
		return 1;
	}

	return 0;
}

static size_t
hex_to_size( char c )
{
	if ( c >= '0' && c <= '9' ) {
		return ( size_t )( c - '0' );
	}

	if ( c >= 'a' && c <= 'f' ) {
		return ( size_t )( c - 'a' ) + ( size_t )0xa;
	}

	if ( c >= 'A' && c <= 'F' ) {
		return ( size_t )( c - 'A' ) + ( size_t )0xa;
	}

	return 0;
}

static int
is_octal( char c )
{
	if ( c >= '0' && c <= '7' ) {
		return 1;
	}

	return 0;
}

static size_t
octal_to_size( char c )
{
	if ( c >= '0' && c <= '7' ) {
		return ( size_t )( c - '0' );
	}

	return 0;
}

static int
mkdir_r(char *path)
{
	struct stat sbuf;

	if (stat(path, &sbuf) < 0) {
		if (mkdir_r(dirname(strdup(path))) < 0)
			return -1;
		return mkdir(path, 0755);
	}
	else if ((sbuf.st_mode & S_IFDIR) == 0) {
		mlog( MLOG_TRACE | MLOG_ERROR | MLOG_TREE, _(
			"%s is not a directory\n"),
			path);
		mlog_exit(EXIT_ERROR, RV_EXISTS);
		exit(1);
	}
	return 0;
}
