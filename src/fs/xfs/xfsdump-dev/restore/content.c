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

#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <attr/attributes.h>
#include <xfs/handle.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <utime.h>
#include <malloc.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>
#include <xfs/xfs.h>
#include <xfs/jdm.h>	/* only for util.h */

#include "config.h"

#include "types.h"
#include "timeutil.h"
#include "util.h"	/* only for r/w routines, ALIGN_PTR */
#include "cldmgr.h"
#include "qlock.h"
#include "lock.h"
#include "path.h"
#include "openutil.h"
#include "exit.h"
#include "getopt.h"
#include "mlog.h"
#include "dlog.h"
#include "bag.h"
#include "node.h"
#include "namreg.h"
#include "stream.h"
#include "global.h"
#include "drive.h"
#include "media.h"
#include "content.h"
#include "content_inode.h"
#include "inomap.h"
#include "dirattr.h"
#include "tree.h"
#include "inventory.h"
#include "mmap.h"
#include "arch_xlate.h"
#include "win.h"
#include "hsmapi.h"

/* content.c - manages restore content
 */

/* structure definitions used locally ****************************************/

#define HOUSEKEEPING_MAGIC	0x686b6d61
	/* "hkma" - see the housekeeping_magic field of pers_t below.
	 */
#define HOUSEKEEPING_VERSION	2
	/* see the housekeeping_version field of pers_t below.
	 * version 2 changed the size of a gen_t, which caused node_t
	 * to change in size. also p_truncategenpr was added to treepers_t.
	 */

#define WRITE_TRIES_MAX	3
	/* retry loop tuning for write(2) workaround
	 */
typedef enum { SYNC_INIT, SYNC_BUSY, SYNC_DONE } sync_t;
	/* for lock-step synchronization
	 */
typedef struct { xfs_ino_t eg_ino; off64_t eg_off; } egrp_t;
	/* extent group descriptor
	 */
typedef char label_t[ GLOBAL_HDR_STRING_SZ ];
	/* dump or mobj label
	 */
typedef enum { PURP_SEARCH, PURP_DIR, PURP_NONDIR } purp_t;
	/* to describe purpose for a media file request. may be for
	 * searching for a dump to restore, for dir restore, or non-dir
	 */
typedef off_t dh_t;
	/* handles for descriptors in persistent state inventory
	 * encoded as byte offset plus one of descriptor into descriptor
	 * portion of persistent state. plus one so DH_NULL can be zero.
	 */
#define DH_NULL		( ( dh_t )0 )
	/* NULL inv. descriptor handles, to terminate linked descriptor lists.
	 * must be zero-valued, so memset of pers.s sets freeh to DH_NULL.
	 */
#define DH2F( h )	( ( pers_file_t * )( ( char * )descp + ( h - 1 )))
#define DH2O( h )	( ( pers_obj_t * )( ( char * )descp + ( h - 1 )))
#define DH2S( h )	( ( pers_strm_t * )( ( char * )descp + ( h - 1 )))
#define DH2D( h )	( ( pers_desc_t * )( ( char * )descp + ( h - 1 )))
	/* convert file, object, and stream inv. descriptor handle into
	 * descriptor pointers
	 */
#define DAU		1
	/* number of descriptor pages to allocate when free list exhausted
	 */
#define IBPGINCR	32
	/* session inv. restore retry buffer increment
	 */

/* Media state abstraction
 */
struct Media {
	drive_t *M_drivep;
	global_hdr_t *M_grhdrp;
	drive_hdr_t *M_drhdrp;
	media_hdr_t *M_mrhdrp;
	content_hdr_t *M_crhdrp;
	content_inode_hdr_t *M_scrhdrp;

	enum { POS_UNKN,	/* whenever media file not open */
	       POS_ATHDR,	/* at beginning of media file */
	       POS_INDIR,	/* at beginning of inomap/dirdump */
	       POS_ATNONDIR,	/* at first non-dir file */
	       POS_END,		/* EOM/EOD */
	       POS_USELESS,	/* current object contains nothing useful */
	       POS_BLANK	/* like useless */
	} M_pos;
		/* media positioning info. initially UNKN, set back to
		 * unkn whenever end_read op called.
		 */
	ix_t M_fmfix;
	ix_t M_lmfix;
	ix_t M_pmfix;
	bool_t M_pmfixvalpr;
	purp_t M_mfixpurp;
	bool_t M_flmfixvalpr;
		/* the indices within the current media object of the first
		 * and last media files seen, as well as previous last.
		 * invalidated whenever purpose changes or media is changed.
		 * previous (pmfix) not valid until second media file seen.
		 */
	ix_t M_fsfix;
	ix_t M_fsoix;
	ix_t M_fssix;
	bool_t M_fsfixvalpr;
		/* index within the current media object of the first
		 * media file that is part of dump being restored,
		 * and indices of the obj and stream containing that mfile.
		 * invalidated on media change.
		 */
};

typedef struct Media Media_t;


/* persistent state - mmapped, has linked lists of dump streams, media
 * objects, and media files. descriptors for each fit into PERS_DESCSZ
 * bytes, and are allocated from a common free pool.
 */

/* persistent media file descriptor
 */
struct pers_file {
	dh_t f_nexth;
		/* singly-linked list of files withing object
		 */
	dh_t f_parh;
		/* parent object
		 */
	bool_t f_szvalpr;
	off64_t f_sz;
		/* if this info came from an inventory (on-line or on-media),
		 * we know the media file size
		 */
	bool_t f_dirtriedpr;
		/* set if attempted to restore dirs from this media file.
		 * says nothing about success or failure. prevents us from
		 * trying to restore dirs from this media file again.
		 */
	bool_t f_valpr;
		/* following three fields are valid
		 */
	egrp_t f_firstegrp;
		/* first extent group in this media file
		 */
	egrp_t f_curegrp;
		/* next extent group to be restored from this media file.
		 * initially equals f_firstegrp.
		 */
	drive_mark_t f_curmark;
		/* drive manager mark for seeking to current extent group
		 */
	bool_t f_nondirdonepr;
		/* TRUE when non-dirs from this media file completely restored,
		 * or as restored as they can be (some or all lost due to
		 * media corruption).
		 */
	bool_t f_nondirskippr;
		/* no non-dirs are needed from this nmedia file (due to
		 * subtree or interactive selections)
		 */
	int f_flags;
		/* mark terminators and inventories
		 */
	bool_t f_underheadpr;
		/* the drive is currently positioned at or in this media file
		 */
};

/* f_flags
 */
#define PF_INV	( 1 << 0 )
#define PF_TERM	( 1 << 1 )

typedef struct pers_file pers_file_t;

/* persistent media object descriptor
 */
struct pers_obj {
	dh_t o_nexth;
		/* singly-linked list of objects in stream
		 */
	dh_t o_parh;
		/* parent dump stream descriptor
		 */
	dh_t o_cldh;
		/* head of list of pertinent media files contained in
		 * this media object
		 */
	bool_t o_idlabvalpr;
		/* id and label fields are valid
		 */
	uuid_t o_id;
		/* uuid of media object
		 */
	label_t o_lab;
		/* label of media object
		 */
	ix_t o_fmfmix;
	bool_t o_fmfmixvalpr;
		/* 0-based index into this mobj's mfiles of first
		 * mfile in the mobj that is part of the dump stream.
		 */
	ix_t o_fmfsix;
	bool_t o_fmfsixvalpr;
		/* 0-based index into this dump stream's mfiles of first
		 * mfile in the mobj that is part of the dump stream.
		 */
	bool_t o_lmfknwnpr;
		/* TRUE if last media file on object is represented in
		 * children list.
		 */
	bool_t o_indrivepr;
	ix_t o_indriveix;
		/* TRUE if this object is in a drive, and which drive it is
		 * in.
		 */
};

typedef struct pers_obj pers_obj_t;

/* media dump stream descriptor
 */
struct pers_strm {
	dh_t s_nexth;
		/* singly-linked list of streams generated by dump
		 */
	dh_t s_cldh;
		/* head of list of mobjs containing this dstrm's mfiles
		 */
	bool_t s_lastobjknwnpr;
		/* TRUE if if last object in the stream is represented in
		 * children list.
		 */
};

typedef struct pers_strm pers_strm_t;

/* media descriptor allocation object (for free list)
 */
union pers_desc {
	dh_t d_nexth;
		/* singly-linked free list of descriptors
		 */
	pers_file_t d_file;
		/* media file descriptor overlay;
		 */
	pers_obj_t d_obj;
		/* media object descriptor overlay;
		 */
	pers_strm_t d_strm;
		/* media stream descriptor overlay;
		 */
};

typedef union pers_desc pers_desc_t;

#define PERS_DESCSZ	512
	/* size of media object, media file, and media stream descriptors.
	 * need to fit integral number into a page, single allocator
	 * used allocate and free all types .
	 */

/* subtree descriptor - the subtree command line arguments are transcribed
 * into variable-length descriptors and placed in an integral number of
 * pages after the persistent header, and before the media descriptor free list.
 */
#define STDESCALIGN	8

struct stdesc {
	bool_t std_sensepr;
		/* TRUE if this is a subtree to INCLUDE, FALSE if EXCLUDE
		 */
	off_t std_nextoff;
		/* offset to next descriptor, in bytes relative to this
		 */
	char std_path[ 1 ];
		/* first character of a NULL-terminated string containing the
		 * the relative subtree pathname
		 */
};

typedef struct stdesc stdesc_t;

/* byte span descriptor - registers a span of a file restored.
 */
struct bytespan {
	off64_t	offset;
	off64_t	endoffset;
} ;

typedef struct bytespan bytespan_t;

/* partial restore descriptor - Keeps track of different byte spans restored
 * for a specific inode.  Used to sync operations between restore streams.
 */
struct partial_rest {
	xfs_ino_t	is_ino;
		/* inode number */
	bytespan_t is_bs[STREAM_SIMMAX];
		/* each stream could conceivably be writing to a single
		 * file simultaneously if one file spans all device streams.
		 * Need a record for each possible place in the file.
		 */
};

typedef struct partial_rest partial_rest_t;

struct stream_context {
	bstat_t    sc_bstat;
	char       sc_path[2 * MAXPATHLEN];
	int   sc_fd;
	int   sc_hsmflags;

	/*
	 * we have to set the owner before we set extended attributes otherwise
	 * capabilities will not be restored correctly as setting the owner with
	 * fchmod will strip the capability attribute from the file. Hence we
	 * need to do this before restoring xattrs and record it so we don't do
	 * it again on completion of file restoration.
	 */
	bool_t	   sc_ownerset;
};

typedef struct stream_context stream_context_t;

/* persistent state file header - on-disk format information plus
 * accumulation state (which spans several sessions) and session state.
 * the latter two have a valid bit, and their fields are not valid until
 * the valid bit is set. all elements defined such that a bzero results
 * in a valid initial state.
 */
struct pers {
	/* on-disk format information used to verify that xfsrestore
	 * can make sense of the data in xfsrestorehousekeepingdir
	 * when running in cumulative mode or when resuming a restore.
	 *
	 * for backwards/forwards compatibility, this struct must be
	 * the first field! also any changes to the struct must address
	 * compatibility with other xfsrestore versions.
	 */
	struct {
		size32_t housekeeping_magic;
			/* used to determine if this struct has been
			 * initialized, and whether the machine's
			 * endianness is the same as the previous
			 * invocation. (data written to xfsrestore's
			 * state directory is not converted to an
			 * endian-neutral format since it only persists
			 * for the life of one or more restore sessions.)
			 */
		size32_t housekeeping_version;
			/* version of the data structures used in the
			 * state files in housekeepingdir. this must be
			 * bumped whenever the on-disk format changes.
			 */
		size64_t pagesize;
			/* headers in the persistent state files
			 * are aligned on page size boundaries, so
			 * this cannot change betweeen invocations.
			 */
	} v;

	/* command line arguments from first session, and session
	 * history.
	 */
	struct {
		bool_t valpr;
			/* not set until a BASE dump has been identified
			 * and validated for restoral, and an attempt has
			 * been made to load the dump inventory into persistent
			 * state, and the namreg and tree abstractions
			 * have been initialized, and the session history
			 * has been initialized and validated.
			 */
		char dstdir[ MAXPATHLEN ];
			/* absolute pathname of the destination directory
			 */
		bool_t dstdirisxfspr;
			/* destination directory is an xfs filesystem; xfs-specific
			 * calls can be made when needed.
			 */
		ix_t dumpcnt;
			/* how many dumps have been applied completedly (A1)
			 */
		uuid_t lastdumpid;
			/* uuid of the last dump completely restored (A1)
			 */
		label_t lastdumplab;
			/* label of the last dump completely restored (A1)
			 */
		bool_t cumpr;
			/* is a cumulative restore (-r)
			 */
		bool_t interpr;
			/* interactive mode specified on command line (-i)
			 */
		bool_t existpr;
			/* existing files may not be overwritten (-e)
			 */
		bool_t changepr;
			/* only missing or old files may be overwritten (-E)
			 */
		bool_t newerpr;
		time32_t newertime;
			/* only files older than example may be overwritten (-n)
			 */
		bool_t ownerpr;
			/* attempt to restore owner/group (-o)
			 */
		ix_t stcnt;
			/* how many subtree args (both inclusive and exclusive)
			 * are recorded in the subtree pages (-s)
			 */
		bool_t firststsensepr;
		bool_t firststsenseprvalpr;
			/* sense of first subtree arg
			 */
		ix_t stpgcnt;
			/* how many pages following the header page are reserved
			 * for the subtree descriptors
			 */
		bool_t restoredmpr;
			/* restore DMAPI event settings
			 */
		bool_t restoreextattrpr;
			/* restore extended attributes
			 */

		ix_t parrestcnt;
			/* Count of partialy restored files.  Used to speed
			 * up searches in parrest.
			 */

		partial_rest_t parrest[ STREAM_SIMMAX * 2 - 2 ];
			/* record of bytes restored to partially restored files.
			 * Max possible is two per stream except the first 
			 * drive will never finish another drives file and the
			 * last drive will never leave a file for another to
			 * complete.
			 */
	} a;

	/* session state.
	 */
	struct {
		bool_t valpr;
			/* until this is true, a resume will ignore (and bzero)
			 * this structure. validate just prior to applying
			 * the directory dump, and after all fields marked (A2)
			 * are correct. invalidate as soon as the session is
			 * complete, and atomically update all fields marked
			 * (A1) at the same time. dirattr abstraction must be
			 * initialized prior to setting this.
			 */
		time32_t accumtime;
			/* for measuring elapsed time of restore
			 */
		uuid_t dumpid;
			/* id of dump currently being applied
			 */
		label_t dumplab;
			/* label of the dump being applied (A2)
			 */
		time32_t begintime;
			/* set when session begun and each time resumed
			 */
		bool_t stat_valpr;
			/* the following stats are not valid until the
			 * first media file header has been read.
			 */
		uint64_t stat_inocnt;
			/* number of non-dir inos to restore during session
			 */
		uint64_t stat_inodone;
			/* number of non-dir inos restored so far
			 */
		off64_t stat_datacnt;
			/* bytes of ordinary files to restore during session
			 */
		off64_t stat_datadone;
			/* bytes of ordinary files restored so far
			 */
		ix_t descpgcnt;
			/* number of pages mapped for pers. media descriptors
			 */
		dh_t descfreeh;
			/* linked list of free media descriptor alloc objs (A2)
			 */
		dh_t strmheadh;
			/* head of singly-linked list of stream descriptors (A2)
			 */
		bool_t fullinvpr;
			/* have discovered and incorporated a full inventory
			 * description into pers. may come from online or a
			 * inventory media file.
			 */
		bool_t marknorefdonepr;
			/* have marked tree nodes as unreferenced by directory
			 * entries, and nulled  dirattr handles.
			 */
		bool_t dirdonepr;
			/* have applied all directories from a dirdump.
			 */
		bool_t adjrefdonepr;
			/* have adjusted marking of nodes no longer referenced
			 * by directory entries.
			 */
		bool_t inomapsanitizedonepr;
			/* the inomap needs to b sanitized prior to subtree
			 * or interactive selections
			 */
		bool_t stdonepr;
			/* have applied subtree selections
			 */
		bool_t interdonepr;
			/* have completed interactive subtree dialog
			 */
		bool_t treepostdonepr;
			/* all of the above treep ost-processing steps have
			 * been completed.
			 */
			/*
			 * nondir restore done here
			 */
		bool_t dirattrdonepr;
			/* directory attributes have been restored and
			 * directory attributes registry has been deleted
			 */
		bool_t orphdeltriedpr;
			/* removed (or tried to remove) orphanage
			 */
		bool_t inomapdelpr;
			/* deleted session ino map
			 */
	} s;
};

typedef struct pers pers_t;

/* transient state. re-generated during each restore session
 */

struct tran {
	time32_t t_starttime;
		/* for measuring elapsed time of restore session
		 */
	size64_t t_dircnt;
	size64_t t_dirdonecnt;
	size64_t t_direntcnt;
		/* for displaying stats on directory reconstruction
		 */
	size64_t t_vmsz;
		/* how much vm may be used. recorded here from main,
		 * passed to tree_init() once we have a valid media
		 * file header
		 */
	bool_t t_toconlypr;
		/* just display table of contents; don't restore files
		 */
	bool_t t_noinvupdatepr;
		/* true if inventory is NOT to be updated when on-media
		 * inventory encountered.
		 */
	bool_t t_dumpidknwnpr;
		/* determined during initialization; if false, set during
		 * per-stream init
		 */
	bool_t t_dirattrinitdonepr;
	bool_t t_namreginitdonepr;
	bool_t t_treeinitdonepr;
		/* determinied during initialization, used during
		 * per-stream restore
		 */
	uuid_t t_reqdumpid;
	bool_t t_reqdumpidvalpr;
		/* uuid of the dump as requested on cmd line
		 */
	char * t_reqdumplab;
	bool_t t_reqdumplabvalpr;
		/* label of the dump as requested on cmd line
		 */
	char *t_hkdir;
		/* absolute pathname of housekeeping directory
		 */
	int t_persfd;
		/* file descriptor of the persistent state file
		 */
	size64_t t_dirdumps;
		/* bitset of streams which contain a directory dump
		 */
	bool_t t_truncategenpr;
		/* force use of truncated generation numbers
		 */
	sync_t t_sync1;
		/* to single-thread attempt to validate command line
		 * selection of dump with online inventory
		 */
	sync_t t_sync2;
		/* to single-thread dump selection by media scan
		 */
	sync_t t_sync3;
		/* to single-thread attempt to apply dirdump to tree
		 */
	sync_t t_sync4;
		/* to single-thread attempt to do tree post-processing
		 * prior to non-directory restore
		 */
	sync_t t_sync5;
		/* to single-thread cleanup after applying non-dir restore
		 */
	qlockh_t t_pilockh;
		/* to establish critical regions while updating pers
		 * inventory
		 */
};

typedef struct tran tran_t;


/* declarations of externally defined global symbols *************************/

extern void usage( void );
extern bool_t preemptchk( void );
extern char *homedir;
extern bool_t pipeline;
extern bool_t stdoutpiped;
extern char *sistr;
extern size_t pgsz;
extern size_t pgmask;


/* forward declarations of locally defined static functions ******************/

static void toconly_cleanup( void );

static Media_t *Media_create( ix_t thrdix );
static void Media_indir( Media_t *Mediap );
static void Media_indir( Media_t *Mediap );
static void Media_atnondir( Media_t *Mediap );
static rv_t Media_mfile_next( Media_t *Mediap,
			      purp_t purp,
			      sync_t *donesyncp,
			      dh_t  *filehp,
			      global_hdr_t **grhdrpp,
			      drive_hdr_t **drhdrpp,
			      media_hdr_t **mrhdrpp,
			      content_hdr_t **crhdrpp,
			      content_inode_hdr_t **scrhdrpp,
			      drive_t **drivepp,
			      filehdr_t *fhdr );
static void Media_end( Media_t *Mediap );
static bool_t Media_prompt_change( drive_t *drivep,
				   purp_t purp,
				   bag_t *bagp,
				   bool_t knownholespr,
				   bool_t maybeholespr );

static bool_t Inv_validate_cmdline( void );
static bool_t dumpcompat( bool_t resumepr,
			  ix_t level,
			  uuid_t baseid,
			  bool_t logpr );
static bool_t promptdumpmatch( ix_t thrdix,
			       global_hdr_t *grhdrp,
			       media_hdr_t *mrhdrp,
			       content_hdr_t *crhdrp,
			       content_inode_hdr_t *scrhdrp );

static void pi_checkpoint( dh_t fileh,
			   drive_mark_t *drivemarkp,
			   xfs_ino_t ino,
			   off64_t off );
static bool_t pi_transcribe( inv_session_t *sessp );
static dh_t pi_addfile( Media_t *Mediap,
			global_hdr_t *grhdrp,
			drive_hdr_t *drhdrp,
			media_hdr_t *mrhdrp,
			content_inode_hdr_t *scrhdrp,
			drive_t * drivep );
static void pi_seestrmend( ix_t strmix );
static void pi_seeobjstrmend( ix_t strmix, ix_t mediaix );
static xfs_ino_t pi_scanfileendino( dh_t fileh );
static bool_t pi_alldone( void );
static bag_t * pi_neededobjs_dir_alloc( bool_t *knownholesprp,
				        bool_t *maybeholesprp );
static bag_t * pi_neededobjs_nondir_alloc( bool_t *knownholesprp,
					   bool_t *maybeholesprp,
					   bool_t showobjindrivepr,
					   bool_t markskippr );
static void pi_neededobjs_free( bag_t *bagp );
static void pi_bracketneededegrps( dh_t thisfileh,
				   egrp_t *first_egrp,
				   egrp_t *next_egrp );
static void pi_update_stats( off64_t sz );
static void pi_hiteod( ix_t strmix, ix_t objix );
static void pi_hiteom( ix_t strmix, ix_t objix );
static void pi_hitnextdump( ix_t strmix, ix_t objix, ix_t lastfileix );
static bool_t pi_know_no_more_on_object( purp_t purp, ix_t strmix, ix_t objix );
static bool_t pi_know_no_more_beyond_on_object( purp_t purp,
					        ix_t strmix,
					        ix_t objix,
					        ix_t fileix );
static void pi_preclean( void );
static void pi_driveempty( ix_t driveix );
static void pi_note_indrive( ix_t driveix, uuid_t mediaid );
static void pi_note_underhead( dh_t thisobjh, dh_t thisfileh );
static void pi_lock( void );
static void pi_unlock( void );

static rv_t applydirdump( drive_t *drivep,
			  dh_t fileh,
			  content_inode_hdr_t *scrhdrp,
			  filehdr_t *fhdrp );
static rv_t treepost( char *path1, char *path2 );
static rv_t applynondirdump( drive_t *drivep,
			     dh_t fileh,
			     content_inode_hdr_t *scrhdrp,
			     char *path1,
			     char *path2,
			     filehdr_t *fhdrp );
static rv_t finalize( char *path1, char *path2 );
static void wipepersstate( void );

static rv_t read_filehdr( drive_t *drivep, filehdr_t *fhdrp, bool_t fhcs );
static rv_t restore_file( drive_t *drivep,
			  filehdr_t *fhdrp,
			  bool_t ehcs,
			  bool_t ahcs,
			  char *path1,
			  char *path2 );
static bool_t restore_reg( drive_t *drivep,
			   filehdr_t *fhdrp,
			   rv_t *rvp,
			   char *path );
static bool_t restore_extent_group( drive_t *drivep,
				    filehdr_t *fhdrp,
				    char *path,
				    int fd,
				    bool_t ehcs,
				    rv_t *rvp);
static bool_t restore_complete_reg( stream_context_t* );
static bool_t restore_spec( filehdr_t *fhdrp, rv_t *rvp, char *path );
static bool_t restore_symlink( drive_t *drivep,
			       filehdr_t *fhdrp,
			       rv_t *rvp,
			       char *path,
			       char *scratchpath,
			       bool_t ehcs );
static rv_t read_extenthdr( drive_t *drivep, extenthdr_t *ehdrp, bool_t ehcs );
static rv_t read_dirent( drive_t *drivep,
			 direnthdr_t *dhdrp,
			 size_t direntbufsz,
			 bool_t dhcs );
static rv_t discard_padding( size_t sz, drive_t *drivep );
static rv_t restore_extent( filehdr_t *fhdrp,
			    extenthdr_t *ehdrp,
			    int fd,
			    char *path,
			    drive_t *drivep,
			    off64_t *bytesreadp );
static bool_t askinvforbaseof( uuid_t baseid, inv_session_t *sessp );
static void addobj( bag_t *bagp,
		    uuid_t *objidp,
		    label_t objlabel,
		    bool_t indrivepr,
		    ix_t indriveix );
static size_t cntobj( bag_t *bagp );
static bool_t gapneeded( egrp_t *firstegrpp, egrp_t *lastegrpp );
static char * ehdr_typestr( int32_t type );
static int egrpcmp( egrp_t *egrpap, egrp_t *egrpbp );
static void display_dump_label( bool_t lockpr,
				int mllevel,
				char *introstr,
				global_hdr_t *grhdrp,
				media_hdr_t *mrhdrp,
				content_hdr_t *crhdrp,
				content_inode_hdr_t *scrhdrp );
static void display_needed_objects( purp_t purp,
				    bag_t *bagp,
				    bool_t knownholespr,
				    bool_t maybeholespr );
static void set_mcflag( ix_t thrdix );
static void clr_mcflag( ix_t thrdix );
static void pi_show( char *introstring );
static void pi_show_nomloglock( void );

static bool_t extattr_init( size_t drivecnt );
static char * get_extattrbuf( ix_t which );
static rv_t restore_extattr( drive_t *drivep,
			     filehdr_t *fhdrp,
			     char *path,
			     bool_t ahcs,
			     bool_t isdirpr,
			     bool_t onlydoreadpr,
			     dah_t dah );
static bool_t restore_dir_extattr_cb( char *path, dah_t dah );
static bool_t restore_dir_extattr_cb_cb( extattrhdr_t *ahdrp, void *ctxp );
static void setextattr( char *path, extattrhdr_t *ahdrp );
static void partial_reg(ix_t d_index, xfs_ino_t ino, off64_t fsize,
                        off64_t offset, off64_t sz);
static bool_t partial_check (xfs_ino_t ino, off64_t fsize);
static bool_t partial_check2 (partial_rest_t *isptr, off64_t fsize);
static int do_fssetdm_by_handle( char *path, fsdmidata_t *fdmp);
static int quotafilecheck(char *type, char *dstdir, char *quotafile);

/* definition of locally defined global variables ****************************/

bool_t content_media_change_needed;
bool_t restore_rootdir_permissions;
char *media_change_alert_program = NULL;
size_t perssz;


/* definition of locally defined static variables *****************************/

static pers_t *persp;		/* mapped at init, ok to use */
static tran_t *tranp;		/* allocated at init, ok to use */
static pers_desc_t *descp = 0;	/* mapped on the fly; don't use! (see macros) */
static char *hkdirname = "xfsrestorehousekeepingdir";
static char *persname = "state";
static char *perspath = 0;
static bool_t mcflag[ STREAM_SIMMAX ]; /* media change flag */


/* definition of locally defined global functions ****************************/

bool_t
content_init( int argc, char *argv[ ], size64_t vmsz )
{
	char *dstdir;	/* abs. path to destination dir */
	bool_t cumpr;	/* cmd line cumulative restore specification */
	bool_t resumepr;/* cmd line resumed restore specification */
	bool_t existpr;	/* cmd line overwrite inhibit specification */
	bool_t newerpr;	/* cmd line overwrite inhibit specification */
	time32_t newertime = 0;
	bool_t changepr;/* cmd line overwrite inhibit specification */
	bool_t interpr;	/* cmd line interactive mode requested */
	bool_t ownerpr;	/* cmd line chown/chmod requested */
	bool_t restoredmpr; /* cmd line restore dm api attrs specification */
	bool_t restoreextattrpr; /* cmd line restore extended attr spec */
	bool_t sesscpltpr; /* force completion of prev interrupted session */
	ix_t stcnt;	/* cmd line number of subtrees requested */
	bool_t firststsensepr;
	bool_t firststsenseprvalpr;
	ix_t stsz;	/* bytes required to record subtree selections */
	ix_t stpgcnt;	/* pages required to hold subtree selections */
	ix_t newstpgcnt;/* pages required to hold subtree selections */
	ix_t descpgcnt; /* pages allocated for persistent descriptors */
	struct stat statbuf;
	pid_t pid;
	int c;
	bool_t ok;
	int rval;
	bool_t fullpr;

	/* Calculate the size needed for the persistent inventory 
	 */
	for ( perssz = pgsz; perssz < sizeof(pers_t); perssz += pgsz )
		;

	/* sanity checks
	 */
	assert( sizeof( pers_desc_t ) <= PERS_DESCSZ );
	assert( PERS_DESCSZ <= pgsz );
	assert( ! ( pgsz % PERS_DESCSZ ));
	assert( sizeof( extattrhdr_t ) == EXTATTRHDR_SZ );

	assert( ! ( perssz % pgsz ));

	assert( SYNC_INIT == 0 );

	mlog( MLOG_NITTY,
	      "sizeof( pers_desc_t ) == %d, pgsz == %d, perssz == %d \n",
	      sizeof( pers_desc_t ), pgsz, perssz );

	/* allocate transient state
	 */
	tranp = ( tran_t * )calloc( 1, sizeof( tran_t ));
	assert( tranp );

	/* allocate a qlock for establishing pi critical regions
	 */
	tranp->t_pilockh = qlock_alloc( QLOCK_ORD_PI );

	/* record vmsz; will be used later to init tree abstraction
	 */
	tranp->t_vmsz = vmsz;

	/* record the start time for stats display
	 */
	tranp->t_starttime = time( 0 );

	/* get command line options
	 */
	cumpr = BOOL_FALSE;
	resumepr = BOOL_FALSE;
	existpr = BOOL_FALSE;
	newerpr = BOOL_FALSE;
	changepr = BOOL_FALSE;
	ownerpr = BOOL_FALSE;
	restoredmpr = BOOL_FALSE;
	restoreextattrpr = BOOL_TRUE;
	sesscpltpr = BOOL_FALSE;
	stcnt = 0;
	firststsensepr = firststsenseprvalpr = BOOL_FALSE;
	stsz = 0;
	interpr = BOOL_FALSE;
	restore_rootdir_permissions = BOOL_FALSE;
	optind = 1;
	opterr = 0;
	while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
		switch ( c ) {
		case GETOPT_TOC:
			tranp->t_toconlypr = BOOL_TRUE;
			break;
		case GETOPT_CUMULATIVE:
			cumpr = BOOL_TRUE;
			break;
		case GETOPT_RESUME:
			resumepr = BOOL_TRUE;
			break;
		case GETOPT_EXISTING:
			existpr = BOOL_TRUE;
			break;
		case GETOPT_NEWER:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "-%c argument missing\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			if ( stat( optarg, &statbuf )) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "unable to get status of -%c argument %s:"
				      " %s\n"),
				      c,
				      optarg,
				      strerror( errno ));
				return BOOL_FALSE;
			}
			newerpr = BOOL_TRUE;
			newertime = statbuf.st_mtime;
			break;
		case GETOPT_CHANGED:
			changepr = BOOL_TRUE;
			break;
		case GETOPT_OWNER:
			ownerpr = BOOL_TRUE;
			break;
		case GETOPT_WORKSPACE:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "-%c argument missing\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			if ( optarg[ 0 ] != '/' ) {
				tranp->t_hkdir = path_reltoabs( optarg,
								homedir );
				if ( ! tranp->t_hkdir ) {
					mlog( MLOG_NORMAL | MLOG_ERROR, _(
					      "-%c argument %s is an "
					      "invalid pathname\n"),
					      c,
					      optarg );
					usage( );
					return BOOL_FALSE;
				}
				mlog( MLOG_DEBUG,
				      "alternate workspace path converted "
				      "from %s to %s\n",
				      optarg,
				      tranp->t_hkdir );
			} else {
				tranp->t_hkdir = optarg;
			}
			rval = stat( tranp->t_hkdir, &statbuf );
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "cannot stat -%c argument %s (%s): %s\n"),
				      c,
				      optarg,
				      tranp->t_hkdir,
				      strerror( errno ));
				usage( );
				return BOOL_FALSE;
			}
			if ( ( statbuf.st_mode & S_IFMT ) != S_IFDIR ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "-%c argument %s (%s) "
				      "is not a directory\n"),
				      c,
				      optarg,
				      tranp->t_hkdir );
				usage( );
				return BOOL_FALSE;
			}

			break;
		case GETOPT_DUMPLABEL:
			if ( tranp->t_reqdumplabvalpr ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "too many -%c arguments: "
				      "\"-%c %s\" already given\n"),
				      c,
				      c,
				      tranp->t_reqdumplab );
				usage( );
				return BOOL_FALSE;
			}
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "-%c argument missing\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			if ( strlen( optarg )
			     >
			     sizeofmember( pers_t, s.dumplab )) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "-%c argument %s too long: max is %d\n"),
				      c,
				      optarg,
				      sizeofmember( pers_t, s.dumplab ));
				usage( );
				return BOOL_FALSE;
			}
			tranp->t_reqdumplab = optarg;
			tranp->t_reqdumplabvalpr = BOOL_TRUE;
			break;
		case GETOPT_SESSIONID:
			if ( tranp->t_reqdumpidvalpr ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "too many -%c arguments\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "-%c argument missing\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			if (uuid_parse( optarg, tranp->t_reqdumpid ) < 0 ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "-%c argument not a valid uuid\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			tranp->t_reqdumpidvalpr = BOOL_TRUE;
			break;
		case GETOPT_SUBTREE:
		case GETOPT_NOSUBTREE:
			if ( ! optarg
			     ||
			     optarg[ 0 ] == 0
			     ||
			     optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL, _(
				      "-%c argument missing\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			if ( optarg[ 0 ] == '/' ) {
				mlog( MLOG_NORMAL, _(
				      "-%c argument must be relative\n"),
				      c );
				usage( );
				return BOOL_FALSE;
			}
			stcnt++;
			if ( ! firststsenseprvalpr ) {
				if ( c == GETOPT_SUBTREE ) {
					firststsensepr = BOOL_TRUE;
				} else {
					firststsensepr = BOOL_FALSE;
				}
				firststsenseprvalpr = BOOL_TRUE;
			}
			stsz += sizeof( stdesc_t )
				+
				strlen( optarg )
				+
				( STDESCALIGN - 1 );
			stsz &= ~( STDESCALIGN - 1 );
			break;
		case GETOPT_INTERACTIVE:
			if ( ! dlog_allowed( )) {
				mlog( MLOG_NORMAL, _(
				      "-%c unavailable: no /dev/tty\n"),
				      GETOPT_INTERACTIVE );
				return BOOL_FALSE;
			}
			interpr = BOOL_TRUE;
			break;
		case GETOPT_NOINVUPDATE:
			tranp->t_noinvupdatepr = BOOL_TRUE;
			break;
		case GETOPT_SETDM:
			restoredmpr = BOOL_TRUE;
			break;
		case GETOPT_ALERTPROG:
			if ( ! optarg || optarg[ 0 ] == '-' ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
					"-%c argument missing\n"),
					c );
				usage( );
				return BOOL_FALSE;
			}
			media_change_alert_program = optarg;
			break;
		case GETOPT_NOEXTATTR:
			restoreextattrpr = BOOL_FALSE;
			break;
		case GETOPT_SESSCPLT:
			sesscpltpr = BOOL_TRUE;
			break;
		case GETOPT_SMALLWINDOW:
			/* obsolete */
			break;
		case GETOPT_ROOTPERM:
			restore_rootdir_permissions = BOOL_TRUE;
			break;
		case GETOPT_FMT2COMPAT:
			tranp->t_truncategenpr = BOOL_TRUE;
			break;
		}
	}

	/* command line option error checking
	 */
	if ( cumpr && tranp->t_toconlypr ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "-%c and -%c option cannot be used together\n"),
		      GETOPT_TOC,
		      GETOPT_CUMULATIVE );
		usage( );
		return BOOL_FALSE;
	}
	if ( resumepr && tranp->t_toconlypr ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "-%c and -%c option cannot be used together\n"),
		      GETOPT_TOC,
		      GETOPT_RESUME );
		usage( );
		return BOOL_FALSE;
	}

	/* assume all streams contain a directory dump. streams will remove
	 * themselves from this bitset if they do not contain a directory dump.
	 */
	assert( drivecnt <= sizeof(tranp->t_dirdumps) * NBBY );
	tranp->t_dirdumps = ( 1ULL << drivecnt ) - 1;

	/* the user may specify stdin as the restore source stream,
	 * by a single dash ('-') with no option letter. This must
	 * appear between the last lettered argument and the destination
	 * directory pathname.
	 */
	if ( optind < argc && ! strcmp( argv[ optind ], "-" )) {
		optind++;
	}

	/* the last argument must be the destination directory. not
	 * required if table-of-contents display, or if a resumed restore
	 * or a delta restore.
	 */
	if ( ! tranp->t_toconlypr ) {
		if ( optind >= argc ) {
			dstdir = 0;
		} else {
			if ( argv[ optind ][ 0 ] != '/' ) {
				dstdir = path_reltoabs( argv[ optind ],
							homedir );
				if ( ! dstdir ) {
					mlog( MLOG_NORMAL | MLOG_ERROR, _(
					      "destination directory %s "
					      "invalid pathname\n"),
					      argv[ optind ] );
					usage( );
					return BOOL_FALSE;
				}
				mlog( MLOG_DEBUG,
				      "restore destination path converted "
				      "from %s to %s\n",
				      argv[ optind ],
				      dstdir );
			} else {
				dstdir = argv[ optind ];
			}
			rval = stat( dstdir, &statbuf );
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "cannot stat destination directory %s: "
				      "%s\n"),
				      dstdir,
				      strerror( errno ));
				usage( );
				return BOOL_FALSE;
			}
			if ( ( statbuf.st_mode & S_IFMT ) != S_IFDIR ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "specified destination %s "
				      "is not a directory\n"),
				      dstdir );
				usage( );
				return BOOL_FALSE;
			}
		}
	} else {
		if ( optind < argc ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "do not specify destination directory if "
			      "contents only restore invoked (-%c option)\n"),
			      GETOPT_TOC );
			usage( );
			return BOOL_FALSE;
		}
		dstdir = ".";
	}

	/* generate a full pathname for the housekeeping dir.
	 * the housekeeping dir will by default be placed in the
	 * destination directory, unless this is a toc, in which case
	 * it will be placed in the current directory. in either case, an
	 * alternate directory may be specified on the command line.
	 * if this is toconly, modify the housekeeping dir's name with 
	 * the pid.
	 */
	if ( ! tranp->t_hkdir ) {
		if ( tranp->t_toconlypr ) {
			tranp->t_hkdir = homedir;
		} else {
			if ( ! dstdir ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "destination directory "
				      "not specified\n") );
				usage( );
				return BOOL_FALSE;
			} else {
				tranp->t_hkdir = dstdir;
			}
		}
	}
	if ( tranp->t_toconlypr ) {
		pid = getpid( );
	} else {
		pid = 0;
	}
	tranp->t_hkdir = open_pathalloc( tranp->t_hkdir, hkdirname, pid );

	/* if this is a table-of-contents only restore, register an
	 * exit handler to get rid of the housekeeping directory and
	 * its contents. NOTE: needs several tran fields initialized!
	 */
	if ( tranp->t_toconlypr ) {
		atexit( toconly_cleanup );
	}

	/* create housekeeping dir if not present
	 */
	rval = mkdir( tranp->t_hkdir, S_IRWXU );
	if ( rval && errno != EEXIST ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "unable to create %s: %s\n"),
		      tranp->t_hkdir,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* build a full pathname to pers. state file
	 */
	assert( ! perspath );
	perspath = open_pathalloc( tranp->t_hkdir, persname, 0 );

	/* open, creating if non-existent
	 */
	tranp->t_persfd = open( perspath,
				O_CREAT | O_RDWR,
				S_IRUSR | S_IWUSR );
	if ( tranp->t_persfd < 0 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "could not open/create persistent state file %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* temporarily mmap just the header, and validate the command line
	 * arguments. three cases: no dumps applied so far, or one or more
	 * dumps applied completely, or restore session was interrupted
	 */
	persp = ( pers_t * ) mmap_autogrow(perssz, tranp->t_persfd, 0);

	if ( persp == ( pers_t * )-1 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "could not map persistent state file hdr %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* but first setup or verify the on-disk format information
	 */
	if ( ! persp->a.valpr ) {
		/* this is the first restore session
		 */
		persp->v.housekeeping_magic = HOUSEKEEPING_MAGIC;
		persp->v.housekeeping_version = HOUSEKEEPING_VERSION;
		persp->v.pagesize = pgsz;

	} else {
		/* cumulative or resuming a restore, verify the header
		 */
		if ( persp->v.housekeeping_magic != HOUSEKEEPING_MAGIC ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "%s format corrupt or wrong endianness "
			      "(0x%x, expected 0x%x)\n"),
			      hkdirname,
			      persp->v.housekeeping_magic,
			      HOUSEKEEPING_MAGIC );
			return BOOL_FALSE;
		}
		if ( persp->v.housekeeping_version != HOUSEKEEPING_VERSION ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "%s format version differs from previous "
			      "restore (%u, expected %u)\n"),
			      hkdirname,
			      persp->v.housekeeping_version,
			      HOUSEKEEPING_VERSION );
			return BOOL_FALSE;
		}
		if ( persp->v.pagesize != pgsz ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "%s format differs from previous "
			      "restore due to page size change "
			      "(was %lu, now %lu)\n"),
			      hkdirname,
			      persp->v.pagesize,
			      pgsz );
			return BOOL_FALSE;
		}
	}

	if ( ! persp->a.valpr ) {
		if ( ! dstdir ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "destination directory not specified\n") );
			usage( );
			return BOOL_FALSE;
		}
		if ( strlen( dstdir ) >= sizeofmember( pers_t, a.dstdir )) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "destination directory pathname too long: "
			      "max is %d characters\n"),
			      sizeofmember( pers_t, a.dstdir ) - 1 );
			usage( );
			return BOOL_FALSE;
		}
		if ( resumepr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c option invalid: there is no "
			      "interrupted restore to resume\n"),
			      GETOPT_RESUME );
			usage( );
			return BOOL_FALSE;
		}
		if ( sesscpltpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c option invalid: there is no "
			      "interrupted restore to force completion of\n"),
			      GETOPT_SESSCPLT );
			usage( );
			return BOOL_FALSE;
		}
	} else if ( ! persp->s.valpr ) {
		if ( ! cumpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "must rm -rf %s prior to noncumulative restore\n"),
			      tranp->t_hkdir );
			return BOOL_FALSE;
		}
		if ( resumepr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c option invalid: there is no "
			      "interrupted restore to resume\n"),
			      GETOPT_RESUME );
			usage( );
			return BOOL_FALSE;
		}
		if ( sesscpltpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c option invalid: there is no "
			      "interrupted restore to force completion of\n"),
			      GETOPT_SESSCPLT );
			usage( );
			return BOOL_FALSE;
		}
		if ( existpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating "
			      "cumulative restore\n"),
			      GETOPT_EXISTING );
			return BOOL_FALSE;
		}
		if ( newerpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating "
			      "cumulative restore\n"),
			      GETOPT_NEWER );
			return BOOL_FALSE;
		}
		if ( changepr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating "
			      "cumulative restore\n"),
			      GETOPT_CHANGED );
			return BOOL_FALSE;
		}
		if ( ownerpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating "
			      "cumulative restore\n"),
			      GETOPT_OWNER );
			return BOOL_FALSE;
		}
		if ( stcnt ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c and -%c valid only when initiating "
			      "cumulative restore\n"),
			      GETOPT_SUBTREE,
			      GETOPT_NOSUBTREE );
			return BOOL_FALSE;
		}
		if ( tranp->t_truncategenpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating "
			      "cumulative restore\n"),
			      GETOPT_FMT2COMPAT );
			return BOOL_FALSE;
		}
	} else {
		if ( ! resumepr && ! sesscpltpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c option required to resume "
			      "or "
			      "-%c option required to force completion of "
			      "previously "
			      "interrupted restore session\n"),
			      GETOPT_RESUME,
			      GETOPT_SESSCPLT );
			return BOOL_FALSE;
		}
		if ( tranp->t_reqdumplabvalpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating restore\n"),
			      GETOPT_DUMPLABEL );
			return BOOL_FALSE;
		}
		if ( tranp->t_reqdumpidvalpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating restore\n"),
			      GETOPT_SESSIONID );
			return BOOL_FALSE;
		}
		if ( existpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating restore\n"),
			      GETOPT_EXISTING );
			return BOOL_FALSE;
		}
		if ( newerpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating restore\n"),
			      GETOPT_NEWER );
			return BOOL_FALSE;
		}
		if ( changepr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating restore\n"),
			      GETOPT_CHANGED );
			return BOOL_FALSE;
		}
		if ( ownerpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating restore\n"),
			      GETOPT_OWNER );
			return BOOL_FALSE;
		}
		if ( interpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating restore\n"),
			      GETOPT_INTERACTIVE );
			return BOOL_FALSE;
		}
		if ( stcnt ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			     "-%c and -%c valid only when initiating restore\n"),
			      GETOPT_SUBTREE,
			      GETOPT_NOSUBTREE );
			return BOOL_FALSE;
		}
		if ( tranp->t_truncategenpr ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "-%c valid only when initiating restore\n"),
			      GETOPT_FMT2COMPAT );
			return BOOL_FALSE;
		}
	}

	if ( persp->a.valpr ) {
		if ( restoredmpr && persp->a.restoredmpr != restoredmpr) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			     "-%c cannot reset flag from previous restore\n"),
			      GETOPT_SETDM );
			return BOOL_FALSE;
		}
		if ( ! restoreextattrpr && 
		       persp->a.restoreextattrpr != restoreextattrpr) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			     "-%c cannot reset flag from previous restore\n"),
			      GETOPT_NOEXTATTR );
			return BOOL_FALSE;
		}
	}

	/* force owner option if root
	 */
	ownerpr = ( geteuid( ) == 0 ) ? BOOL_TRUE : ownerpr;

	/* force completion of interrupted restore if asked to do so
	 */
	if ( sesscpltpr ) {
		char *path1;
		char *path2;
		rv_t rv;
		int rval;

		path1 = ( char * )calloc( 1, 2 * MAXPATHLEN );
		assert( path1 );
		path2 = ( char * )calloc( 1, 2 * MAXPATHLEN );
		assert( path2 );
		assert( persp->a.valpr );
		assert( persp->s.valpr );
		rval = chdir( persp->a.dstdir );
		if ( rval ) {
			mlog( MLOG_NORMAL, _(
			      "chdir %s failed: %s\n"),
			      persp->a.dstdir,
			      strerror( errno ));
			return BOOL_FALSE;
		}
		ok = dirattr_init( tranp->t_hkdir, BOOL_TRUE, ( uint64_t )0 );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
		ok = namreg_init( tranp->t_hkdir, BOOL_TRUE, ( uint64_t )0 );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
		ok = inomap_sync_pers( tranp->t_hkdir );
		if ( ! ok ) {
			return BOOL_FALSE;
		}

		/* This is only a full restore if we're doing a level
		 * 0 restore.
		 */
		if (persp->a.dumpcnt == 0) {
			fullpr = BOOL_TRUE;
		} else {
			fullpr = BOOL_FALSE;
		}

		ok = tree_sync( tranp->t_hkdir,
				persp->a.dstdir,
				tranp->t_toconlypr,
				fullpr,
				persp->a.dstdirisxfspr );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
		rv = finalize( path1, path2 );
		free( ( void * )path1 );
		free( ( void * )path2 );
		switch ( rv ) {
		case RV_OK:
			break;
		case RV_ERROR:
			return EXIT_ERROR;
		case RV_INTR:
			return EXIT_NORMAL;
		case RV_CORE:
		default:
			return EXIT_FAULT;
		}
	}

	/* for the three cases, calculate old and new mapping params
	 * and wipe partial state
	 */
	if ( ! persp->a.valpr ) {
		stpgcnt = 0;
		newstpgcnt = ( stsz + pgmask ) / pgsz;
		descpgcnt = 0;
		memset( ( void * )&persp->a, 0,
			sizeof( pers_t ) - offsetofmember( pers_t, a ));
	} else if ( ! persp->s.valpr ) {
		stpgcnt = persp->a.stpgcnt;
		newstpgcnt = stpgcnt;
		descpgcnt = 0;
		memset( ( void * )&persp->s, 0, sizeofmember( pers_t, s ));
	} else {
		stpgcnt = persp->a.stpgcnt;
		newstpgcnt = stpgcnt;
		descpgcnt = persp->s.descpgcnt;
		assert( resumepr );
		mlog( MLOG_VERBOSE, _(
		      "resuming restore previously begun %s\n"),
		      ctimennl( &persp->s.begintime ));
		persp->s.begintime = time( 0 );
	}

	/* unmap temp mapping of hdr, truncate, and remap hdr/subtrees
	 */
	rval = munmap( ( void * )persp, perssz );
	assert( ! rval );
	rval = ftruncate( tranp->t_persfd, ( off_t )perssz
					   +
					   ( off_t )( stpgcnt + descpgcnt )
					   *
					   ( off_t )pgsz );
	assert( ! rval );
	stpgcnt = newstpgcnt;
	persp = ( pers_t * ) mmap_autogrow( perssz + stpgcnt * pgsz,
				   tranp->t_persfd, 0);
	if ( persp == ( pers_t * )-1 ) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "could not map persistent state file %s: %s\n"),
		      perspath,
		      strerror( errno ));
		return BOOL_FALSE;
	}

	/* if first restore session, record cmd line args and subtrees
	 * and start time.
	 */
	if ( ! persp->a.valpr ) {
		stdesc_t *stdescp;

		strcpy( persp->a.dstdir, dstdir );
		persp->a.dstdirisxfspr = platform_test_xfs_path( dstdir );
		if ( cumpr ) {
			persp->a.cumpr = cumpr;
		}
		if ( interpr ) {
			persp->a.interpr = interpr;
		}
		if ( existpr ) {
			persp->a.existpr = existpr;
		}
		if ( changepr ) {
			persp->a.changepr = changepr;
		}
		if ( ownerpr ) {
			persp->a.ownerpr = ownerpr;
		}
		if ( newerpr ) {
			persp->a.newerpr = newerpr;
			persp->a.newertime = newertime;
		}
		persp->a.restoredmpr = restoredmpr;
		if ( ! persp->a.dstdirisxfspr ) {
			restoreextattrpr = BOOL_FALSE;
		}
		persp->a.restoreextattrpr = restoreextattrpr;
		persp->a.stcnt = stcnt;
		persp->a.firststsensepr = firststsensepr;
		persp->a.firststsenseprvalpr = firststsenseprvalpr;
		persp->a.stpgcnt = stpgcnt;
		optind = 1;
		opterr = 0;
		stdescp = ( stdesc_t * )( ( char * )persp + perssz );
		while ( ( c = getopt( argc, argv, GETOPT_CMDSTRING )) != EOF ) {
			size_t stdsz;
			switch ( c ) {
			case GETOPT_SUBTREE:
			case GETOPT_NOSUBTREE:
				stdescp->std_sensepr = ( c == GETOPT_SUBTREE )
						       ?
						       BOOL_TRUE
						       :
						       BOOL_FALSE;
				stdsz = sizeof( stdesc_t )
					+
					strlen( optarg )
					+
					( STDESCALIGN - 1 );
				stdsz &= ~( STDESCALIGN - 1 );
				assert( stdsz <= ( size_t )OFFMAX );
				stdescp->std_nextoff = ( off_t )stdsz;
				strcpy( stdescp->std_path, optarg );
				stdescp = ( stdesc_t * )
					  ( ( char * )stdescp + stdsz );
				stcnt--;
				break;
			}
		}
		assert( stcnt == 0 );
	}

	/* initialize the local extattr abstraction. must be done even if
	 * we don't intend to restore extended attributes
	 */
	ok = extattr_init( drivecnt );
	if ( ! ok ) {
		return BOOL_FALSE;
	}

	/* effectively initialize libhandle on this filesystem by
	 * allocating a file system handle. this needs to be done
	 * before any open_by_handle() calls (and possibly other
	 * libhandle calls).
	 */
	if ( persp->a.dstdirisxfspr ) {
		void	*fshanp;
		size_t	fshlen=0;

		if(path_to_fshandle(persp->a.dstdir, &fshanp, &fshlen)) {
			mlog( MLOG_NORMAL,
				_("unable to construct a file "
				  "system handle for %s: %s\n"),
				persp->a.dstdir,
				strerror( errno ));
			return BOOL_FALSE;
		}
		/* libhandle has it cached, release this copy */
		free_handle(fshanp, fshlen);
	}

	/* map in pers. inv. descriptors, if any. NOTE: this ptr is to be
	 * referenced ONLY via the macros provided; the descriptors will be
	 * occasionally remapped, causing the ptr to change.
	 */
	assert( ! descp );
	if ( descpgcnt ) {
		descp = ( pers_desc_t * ) mmap_autogrow( descpgcnt * pgsz,
						tranp->t_persfd,
						( off_t )perssz
						+
						( off_t )( stpgcnt * pgsz ));
		if ( descp == ( pers_desc_t * )-1 ) {
			mlog( MLOG_NORMAL | MLOG_ERROR, _(
			      "could not map persistent state file inv %s: "
			      "%s (%d)\n"),
			      perspath,
			      strerror( errno ),
			      errno );
			descp = 0;
			return BOOL_FALSE;
		}
		pi_preclean( );
	}

	/* if resuming an interrupted restore, indicate we know the id
	 * of the dump session being restored. otherwise, it will be determined
	 * during coordination of per-drive threads.
	 */
	if ( persp->a.valpr && persp->s.valpr ) {
		persp->s.begintime = time( 0 );
		tranp->t_dumpidknwnpr = BOOL_TRUE;
	}

	/* sync up with the directory attributes registry.
	 * starts fresh with each dump session restored.
	 * determine if full init needed instead.
	 */
	if ( persp->a.valpr && persp->s.valpr ) {
		ok = dirattr_init( tranp->t_hkdir, BOOL_TRUE, ( uint64_t )0 );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
		tranp->t_dirattrinitdonepr = BOOL_TRUE;
	}

	/* sync up with the name registry. created by the
	 * first session, retained by subsequent sessions.
	 * determine if full init needed instead.
	 */
	if ( persp->a.valpr ) {
		ok = namreg_init( tranp->t_hkdir, BOOL_TRUE, ( uint64_t )0 );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
		tranp->t_namreginitdonepr = BOOL_TRUE;
	}

	/* sync up with the inomap abstraction. created anew with each fresh
	 * restore session, but persistent after tree updated with dirdump.
	 * determine if full init needed instead.
	 */
	ok = inomap_sync_pers( tranp->t_hkdir );
	if ( ! ok ) {
		return BOOL_FALSE;
	}

	/* sync up with the tree abstraction. created by the
	 * first session, retained by subsequent sessions.
	 * don't call tree_init( ) from here; can only be called
	 * when a valid media file header is at hand.
	 */
	if ( persp->a.valpr ) {
		/* This is only a full restore if we're doing a level
		 * 0 restore.
		 */
		if (persp->a.dumpcnt == 0) {
			fullpr = BOOL_TRUE;
		} else {
			fullpr = BOOL_FALSE;
		}

		ok = tree_sync( tranp->t_hkdir,
				persp->a.dstdir,
				tranp->t_toconlypr,
				fullpr,
				persp->a.dstdirisxfspr );
		if ( ! ok ) {
			return BOOL_FALSE;
		}
		tranp->t_treeinitdonepr = BOOL_TRUE;
	}

	/* set media change flags to FALSE;
	 */
	{
		ix_t ix;
		ix_t endix = sizeof( mcflag )
			     /
			     sizeof( mcflag[ 0 ] );
		for ( ix = 0 ; ix < endix ; ix++ ) {
			mcflag[ ix ] = BOOL_FALSE;
		}
	}
	content_media_change_needed = BOOL_FALSE;

	pi_show( " at initialization" );
	return BOOL_TRUE;
}

/* stream thread entry point - returns exit code
 */
int
content_stream_restore( ix_t thrdix )
{
	dh_t fileh;
	Media_t *Mediap; /* local media abstraction */
	char *path1;
	char *path2;
	drive_t *drivep;
	int dcaps;
	global_hdr_t *grhdrp;
	drive_hdr_t *drhdrp;
	media_hdr_t *mrhdrp;
	content_hdr_t *crhdrp;
	content_inode_hdr_t *scrhdrp;
	stream_context_t *strctxp;
	filehdr_t fhdr; /* save hdr terminating dir restore */
	uuid_t lastdumprejectedid;
	rv_t rv;
	bool_t ok;
	int rval;

	/* allocate two path buffers
	 */
	path1 = ( char * )calloc( 1, 2 * MAXPATHLEN );
	assert( path1 );
	path2 = ( char * )calloc( 1, 2 * MAXPATHLEN );
	assert( path2 );

	/* set the current directory to dstdir. the tree abstraction
	 * depends on the current directory being the root of the
	 * destination file system.
	 */
	rval = chdir( persp->a.dstdir );
	if ( rval ) {
		mlog( MLOG_NORMAL, _(
		      "chdir %s failed: %s\n"),
		      persp->a.dstdir,
		      strerror( errno ));
		return mlog_exit(EXIT_ERROR, RV_ERROR);
	}

	/* set my file creation mask to zero, to avoid modifying the
	 * dumped mode bits
	 */
	( void )umask( 0 );

	/* initialize the Media abstraction
	 */
	Mediap = Media_create( thrdix );

	/*
	 * initialize the stream context
	 */
	strctxp = (stream_context_t *)calloc(1, sizeof(stream_context_t));
	if (!strctxp) {
		mlog( MLOG_NORMAL | MLOG_ERROR,
		      _("malloc of stream context failed (%d bytes): %s\n"),
		      sizeof(stream_context_t),
		      strerror( errno ));
		return mlog_exit(EXIT_ERROR, RV_ERROR);
	}
	strctxp->sc_fd = -1;
	Mediap->M_drivep->d_strmcontextp = (void *)strctxp;

	/* if we don't know the dump session id to restore,
	 * first see if command line options can be validated
	 * against the online inventory to identify it. only
	 * one stream needs to do this; the others just wait.
	 * side-effect of validation is to incorporate the online
	 * inventory into the persistent state.
	 */
	if ( tranp->t_dumpidknwnpr ) {
		tranp->t_sync1 = SYNC_DONE;
	}
	while ( tranp->t_sync1 != SYNC_DONE ) {
		lock( );
		if ( tranp->t_sync1 == SYNC_BUSY ) {
			unlock( );
			sleep( 1 );
			if ( cldmgr_stop_requested( )) {
				return mlog_exit(EXIT_NORMAL, RV_INTR);
			}
			continue;
		}
		if ( tranp->t_sync1 == SYNC_DONE ) {
			unlock( );
			continue;
		}
		tranp->t_sync1 = SYNC_BUSY;
		unlock( );
		mlog( MLOG_DEBUG,
		      "checking and validating command line dump id/label\n" );
		ok = Inv_validate_cmdline( );
		    /* side-effect - searches for and incorporates online inv
		     * into pi, and makes persp->s.dumpid valid.
		     */
		if ( ok == BOOL_ERROR ) {
			return mlog_exit(EXIT_ERROR, RV_OPT);
		}
		tranp->t_dumpidknwnpr = ok;
		tranp->t_sync1 = SYNC_DONE;
	}

	/* if we still don't know the session to restore, search the
	 * media for a match either to the command line arguments or
	 * until the operator selects a media file from the desired
	 * dump.
	 */
	if ( tranp->t_dumpidknwnpr ) {
		tranp->t_sync2 = SYNC_DONE;
	}
	uuid_clear(lastdumprejectedid);
	if ( tranp->t_sync2 != SYNC_DONE ) {
		mlog( MLOG_VERBOSE, _(
		      "searching media for dump\n") );
	}
	while ( tranp->t_sync2 != SYNC_DONE ) {
		bool_t matchpr;
		inv_session_t *sessp;
		bool_t resumepr;
		ix_t level;
		uuid_t *baseidp;

		rv = Media_mfile_next( Mediap,
				       PURP_SEARCH,
				       &tranp->t_sync2,
				       0,
				       &grhdrp,
				       &drhdrp,
				       &mrhdrp,
				       &crhdrp,
				       &scrhdrp,
				       &drivep,
				       &fhdr );
		switch ( rv ) {
		case RV_OK:
			break;
		case RV_DONE:
		case RV_NOMORE:
			continue;
		case RV_INTR:
		case RV_QUIT:
		case RV_DRIVE:
			Media_end( Mediap );
			return mlog_exit(EXIT_NORMAL, rv);
		case RV_CORE:
		default:
			Media_end( Mediap );
			return mlog_exit(EXIT_FAULT, rv);
		}
		dcaps = drivep->d_capabilities;

		lock( );
		while ( tranp->t_sync2 == SYNC_BUSY ) {
			unlock( );
			sleep( 1 );
			if ( cldmgr_stop_requested( )) {
				Media_end( Mediap );
				return mlog_exit(EXIT_NORMAL, RV_INTR);
			}
			lock( );
		}
		if ( tranp->t_sync2 == SYNC_DONE ) {
			unlock( );
			continue;
		}
		tranp->t_sync2 = SYNC_BUSY;

		unlock( );
		mlog( MLOG_DEBUG,
		      "dump found: checking\n" );
		matchpr = BOOL_FALSE;
		resumepr = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_RESUME );
		assert( scrhdrp->cih_level >= 0 );
		level = ( ix_t )scrhdrp->cih_level;
		baseidp = resumepr
			  ?
			  &scrhdrp->cih_resume_id
			  :
			  &scrhdrp->cih_last_id;
		if ( tranp->t_reqdumpidvalpr ) {
			if ( uuid_compare( tranp->t_reqdumpid,
					 grhdrp->gh_dumpid) == 0) {
				matchpr = BOOL_TRUE;
				display_dump_label( BOOL_TRUE, /* lock */
						    MLOG_VERBOSE, _(
						    "found dump matching "
						    "specified id:\n"),
						    grhdrp,
						    mrhdrp,
						    crhdrp,
						    scrhdrp );
			}
		} else if ( tranp->t_reqdumplabvalpr ) {
			if ( ! strncmp( tranp->t_reqdumplab,
					grhdrp->gh_dumplabel,
					sizeof( grhdrp->gh_dumplabel ))) {
				matchpr = BOOL_TRUE;
				display_dump_label( BOOL_TRUE, /* lock */
						    MLOG_VERBOSE,  _(
						    "found dump matching "
						    "specified label:\n"),
						    grhdrp,
						    mrhdrp,
						    crhdrp,
						    scrhdrp );
			}
		} else if ( dumpcompat( resumepr,
					level,
					*baseidp,
					BOOL_FALSE )) {
			if ( uuid_compare( lastdumprejectedid,
					 grhdrp->gh_dumpid) == 0) {
				matchpr = BOOL_FALSE;
			} else {
				if ( dlog_allowed( )
				     &&
				     ( ( dcaps & DRIVE_CAP_FILES )
				       ||
				       ( dcaps & DRIVE_CAP_REMOVABLE )
				       ||
				       drivecnt > 1 )) {
					matchpr = promptdumpmatch( thrdix,
								   grhdrp,
								   mrhdrp,
								   crhdrp,
								   scrhdrp );
				} else {
					matchpr = BOOL_TRUE;
					display_dump_label( BOOL_TRUE,/* lock */
							    MLOG_VERBOSE, _(
							    "dump "
							    "description: \n"),
							    grhdrp,
							    mrhdrp,
							    crhdrp,
							    scrhdrp );
				}
			}
		}
		if ( cldmgr_stop_requested( )) {
			Media_end( Mediap );
			return mlog_exit(EXIT_NORMAL, RV_INTR);
		}
		if ( ! matchpr ) {
			Media_end( Mediap );
			uuid_copy(lastdumprejectedid, grhdrp->gh_dumpid);
			tranp->t_sync2 = SYNC_INIT;
			if ( ! dlog_allowed( )
			     ||
			     ( ! ( dcaps & DRIVE_CAP_FILES )
			       &&
			       ! ( dcaps & DRIVE_CAP_REMOVABLE ))) {
				return mlog_exit(EXIT_NORMAL, RV_QUIT);
			}
			continue;
		}
		if ( ! dumpcompat( resumepr, level, *baseidp, BOOL_TRUE )) {
			Media_end( Mediap );
			return mlog_exit(EXIT_ERROR, RV_COMPAT);
		}
		strncpyterm( persp->s.dumplab,
			     grhdrp->gh_dumplabel,
			     sizeof( persp->s.dumplab ));
		sessp = 0;

		/* don't look at the online inventory if the input is piped
		 */
		if ( ! drivep->d_isnamedpipepr
		     &&
		     ! drivep->d_isunnamedpipepr ) {
			ok = inv_get_session_byuuid(NULL,
						    &grhdrp->gh_dumpid,
						    &sessp);
			if ( ok && sessp ) {
				mlog( MLOG_VERBOSE, _(
				      "using online session inventory\n") );
				persp->s.fullinvpr = pi_transcribe( sessp );
				inv_free_session( &sessp );
			}
		}
		fileh = pi_addfile( Mediap,
				    grhdrp,
				    drhdrp,
				    mrhdrp,
				    scrhdrp,
				    drivep );
			/* done here because Media_mfile_next doesn't know
			 * if this is a match
			 */
		if ( fileh == DH_NULL ) {
			return mlog_exit(EXIT_FAULT, RV_ERROR);
		}
		uuid_copy(persp->s.dumpid,grhdrp->gh_dumpid);
		persp->s.begintime = time( 0 );
		tranp->t_dumpidknwnpr = BOOL_TRUE;
		tranp->t_sync2 = SYNC_DONE;
	}

	/* all drives coordinate in attempt to apply session dir dump.
	 * only one actually completes.
	 */
	if ( persp->s.dirdonepr ) {
		tranp->t_sync3 = SYNC_DONE;
	}
	if ( tranp->t_sync3 != SYNC_DONE ) {
		mlog( MLOG_VERBOSE, _(
		      "searching media for directory dump\n") );
	}
	while ( tranp->t_sync3 != SYNC_DONE ) {
		rv = Media_mfile_next( Mediap,
				       PURP_DIR,
				       &tranp->t_sync3,
				       &fileh,
				       &grhdrp,
				       &drhdrp,
				       &mrhdrp,
				       &crhdrp,
				       &scrhdrp,
				       &drivep,
				       &fhdr );
		switch ( rv ) {
		case RV_OK:
			break;
		case RV_DONE:
		case RV_NOMORE:
			continue;
		case RV_INTR:
		case RV_QUIT:
		case RV_DRIVE:
			Media_end( Mediap );
			return mlog_exit(EXIT_NORMAL, rv);
		case RV_CORE:
		default:
			Media_end( Mediap );
			return mlog_exit(EXIT_FAULT, rv);
		}
		dcaps = drivep->d_capabilities;
		assert( fileh != DH_NULL );
		lock( );
		if ( tranp->t_sync3 == SYNC_BUSY ) {
			unlock( );
			mlog( MLOG_TRACE,
			      "waiting for directories to be restored\n" );
			lock( );
		}
		while ( tranp->t_sync3 == SYNC_BUSY ) {
			unlock( );
#if DEBUG_DUMPSTREAMS
			{
			    static int count[STREAM_MAX] = {0};
			    int streamix = stream_getix( pthread_self() );
			    if (++(count[streamix]) == 30) {
				mlog( MLOG_TRACE,
					"still waiting for dirs to be restored\n");
				count[streamix] = 0;
			    }
			}
#endif
			sleep( 1 );
			if ( cldmgr_stop_requested( )) {
				Media_end( Mediap );
				return mlog_exit(EXIT_NORMAL, RV_INTR);
			}
			lock( );
		}
		if ( tranp->t_sync3 == SYNC_DONE ) {
			unlock( );
			continue;
		}
		if ( !(scrhdrp->cih_dumpattr & CIH_DUMPATTR_DIRDUMP) ) {
			/* if no streams have a directory dump, issue a
			 * message and exit. first set SYNC_BUSY to prevent
			 * other threads from coming through here and issuing
			 * the same message.
			 */
			tranp->t_dirdumps &= ~(1ULL << thrdix);
			if ( !tranp->t_dirdumps ) {
				tranp->t_sync3 = SYNC_BUSY;
			}
			unlock( );
			if ( !tranp->t_dirdumps ) {
				mlog( MLOG_VERBOSE | MLOG_ERROR, _(
					"no directory dump found\n") );
				Media_end( Mediap );
				return mlog_exit(EXIT_NORMAL, RV_ERROR);
			}
			sleep( 1 );
			if ( cldmgr_stop_requested( )) {
				Media_end( Mediap );
				return mlog_exit(EXIT_NORMAL, RV_INTR);
			}
			continue;
		}
		tranp->t_sync3 = SYNC_BUSY;
		unlock( );
		if ( ! tranp->t_dirattrinitdonepr ) {
			mlog( MLOG_TRACE,
			      "initializing directory attributes registry\n" );
			mlog( MLOG_NITTY,
			      "content_stream_restore: dircnt %llu\n",
			      scrhdrp->cih_inomap_dircnt );
			ok = dirattr_init( tranp->t_hkdir,
					   BOOL_FALSE,
					   scrhdrp->cih_inomap_dircnt );
			if ( ! ok ) {
				Media_end( Mediap );
				return mlog_exit(EXIT_ERROR, RV_ERROR);
			}
			tranp->t_dirattrinitdonepr = BOOL_TRUE;
		}

		if ( ! tranp->t_namreginitdonepr ) {
			mlog( MLOG_TRACE,
			      "initializing directory entry name registry\n" );
			ok = namreg_init( tranp->t_hkdir,
					  BOOL_FALSE,
					  scrhdrp->cih_inomap_dircnt
					  +
					  scrhdrp->cih_inomap_nondircnt );
			if ( ! ok ) {
				Media_end( Mediap );
				return mlog_exit(EXIT_ERROR, RV_ERROR);
			}
			tranp->t_namreginitdonepr = BOOL_TRUE;
		}

		if ( ! tranp->t_treeinitdonepr ) {
			bool_t fullpr;

			fullpr = ( scrhdrp->cih_level
				   ==
				   0 )
				 &&
				 ! ( scrhdrp->cih_dumpattr
				    &
				    CIH_DUMPATTR_RESUME );

			mlog( MLOG_TRACE,
			      "initializing directory hierarchy image\n" );
			ok = tree_init( tranp->t_hkdir,
					persp->a.dstdir,
					tranp->t_toconlypr,
					persp->a.ownerpr,
					scrhdrp->cih_rootino,
					scrhdrp->cih_inomap_firstino,
					scrhdrp->cih_inomap_lastino,
					scrhdrp->cih_inomap_dircnt,
					scrhdrp->cih_inomap_nondircnt,
					tranp->t_vmsz,
					fullpr,
					persp->a.restoredmpr,
					persp->a.dstdirisxfspr,
					grhdrp->gh_version,
					tranp->t_truncategenpr );
			if ( ! ok ) {
				Media_end( Mediap );
				return mlog_exit(EXIT_ERROR, RV_ERROR);
			}
			tranp->t_treeinitdonepr = BOOL_TRUE;

		} else {
			ok = tree_check_dump_format( grhdrp->gh_version );
			if ( ! ok ) {
				Media_end( Mediap );
				return mlog_exit(EXIT_ERROR, RV_ERROR);
			}
		}

		/* commit the session and accumulative state
		 */
		persp->s.valpr = BOOL_TRUE;
		persp->a.valpr = BOOL_TRUE;

		mlog( MLOG_VERBOSE, _(
		      "reading directories\n") );
		win_locks_off(); /* we are single threaded here */
		rv = applydirdump( drivep, fileh, scrhdrp, &fhdr );
		win_locks_on();
		mlog( MLOG_TRACE,
		      "number of mmap calls for windows = %lu\n", win_getnum_mmaps());
		switch ( rv ) {
		case RV_OK:
			DH2F( fileh )->f_dirtriedpr = BOOL_TRUE;
			Media_atnondir( Mediap );
			tranp->t_sync3 = SYNC_DONE;
			break;
		case RV_CORRUPT:
			Media_indir( Mediap );
			DH2F( fileh )->f_dirtriedpr = BOOL_TRUE;
			tranp->t_sync3 = SYNC_INIT;
			break;
		case RV_INTR:
		case RV_DRIVE:
			Media_end( Mediap );
			return mlog_exit(EXIT_NORMAL, rv);
		case RV_CORE:
		default:
			Media_end( Mediap );
			return mlog_exit(EXIT_FAULT, rv);
		}
	}

	/* now let one thread do all tree post-processing prior to
	 * non-dir restore
	 */
	if ( persp->s.treepostdonepr ) {
		tranp->t_sync4 = SYNC_DONE;
	}
	while ( tranp->t_sync4 != SYNC_DONE ) {
		lock( );
		if ( tranp->t_sync4 == SYNC_BUSY ) {
			unlock( );
			mlog( MLOG_TRACE,
			      "waiting for directory post-processing "
			      "to complete\n" );
			lock( );
		}
		while ( tranp->t_sync4 == SYNC_BUSY ) {
			unlock( );
#if DEBUG_DUMPSTREAMS
			{
			static int count[STREAM_MAX] = {0};
			int streamix = stream_getix( pthread_self() );
			    if (++(count[streamix]) == 30) {
				mlog( MLOG_NORMAL,
				      "still waiting for dirs post-processing\n");
				count[streamix] = 0;
			    }
			}
#endif
			sleep( 1 );
			if ( cldmgr_stop_requested( )) {
				Media_end( Mediap );
				return mlog_exit(EXIT_NORMAL, RV_INTR);
			}
			lock( );
		}
		if ( tranp->t_sync4 == SYNC_DONE ) {
			unlock( );
			continue;
		}
		tranp->t_sync4 = SYNC_BUSY;
		unlock( );
		mlog( MLOG_VERBOSE, _(
		      "directory post-processing\n") );
		win_locks_off(); /* we are single threaded here */
		rv = treepost( path1, path2 );
		win_locks_on();
		switch ( rv ) {
		case RV_OK:
			break;
		case RV_ERROR:
			Media_end( Mediap );
			return mlog_exit(EXIT_ERROR, RV_ERROR);
		case RV_INTR:
			Media_end( Mediap );
			return mlog_exit(EXIT_INTERRUPT, RV_INTR);
		case RV_CORE:
		default:
			Media_end( Mediap );
			return mlog_exit(EXIT_FAULT, rv);
		}

		/* now that we have a tree and inomap, scan the
		 * pi to see what media files can be skipped.
		 * this func has cancer: too many flags and
		 * side-effects!
		 */
		{
		bool_t dummyknownholespr;
		bool_t dummymaybeholespr;
		bag_t *bagp = pi_neededobjs_nondir_alloc( &dummyknownholespr,
							  &dummymaybeholespr,
							  BOOL_FALSE,
							  BOOL_TRUE );
		if ( bagp ) {
			pi_neededobjs_free( bagp );
			bagp = 0;
		}
		}

		/* release exclusion
		 */
		tranp->t_sync4 = SYNC_DONE;
	}

	/* now all are free to do concurrent non-dir restore!
	 * apply media files until there are no more, or we are interrupted
	 */
	for (;;) {
		mlog( MLOG_DEBUG,
		      "getting next media file for non-dir restore\n" );
		rv = Media_mfile_next( Mediap,
				       PURP_NONDIR,
				       0,
				       &fileh,
				       &grhdrp,
				       &drhdrp,
				       &mrhdrp,
				       &crhdrp,
				       &scrhdrp,
				       &drivep,
				       &fhdr );
		if ( rv == RV_NOMORE ) {
			break;
		}
		switch ( rv ) {
		case RV_OK:
			break;
		case RV_INTR:
		case RV_QUIT:
		case RV_DRIVE:
			Media_end( Mediap );
			return mlog_exit(EXIT_NORMAL, rv);
		case RV_CORE:
		default:
			Media_end( Mediap );
			return mlog_exit(EXIT_FAULT, rv);
		}
		dcaps = drivep->d_capabilities;
		assert( fileh > DH_NULL );
		if ( tranp->t_toconlypr ) {
			mlog( MLOG_VERBOSE, _(
			      "reading non-directory files\n") );
		} else {
			mlog( MLOG_VERBOSE, _(
			      "restoring non-directory files\n") );
		}
		mlog( MLOG_TRACE,
		      "media file %u in "
		      "object %u "
		      "of stream %u\n",
		      mrhdrp->mh_mediafileix,
		      mrhdrp->mh_mediaix,
		      drhdrp->dh_driveix );
		mlog( MLOG_DEBUG,
		      "file %u in stream, "
		      "file %u in dump %u on object\n",
		      mrhdrp->mh_dumpfileix,
		      mrhdrp->mh_dumpmediafileix,
		      mrhdrp->mh_dumpmediaix );
		rv = applynondirdump( drivep,
				      fileh,
				      scrhdrp,
				      path1,
				      path2,
				      &fhdr );
		switch ( rv ) {
		case RV_OK:
			DH2F( fileh )->f_nondirdonepr = BOOL_TRUE;
			Media_end( Mediap );
			break;
		case RV_INTR:
		case RV_DRIVE:
		case RV_INCOMPLETE:
			Media_end( Mediap );
			return mlog_exit(EXIT_NORMAL, rv);
		case RV_CORE:
		default:
			Media_end( Mediap );
			return mlog_exit(EXIT_FAULT, rv);
		}
	}

	/* finally, choose one thread to do final processing
	 * and cleanup. the winner waits, the losers all exit.
	 * once the losers exit, the winner can perform cleanup.
	 */
	lock( );
	if ( tranp->t_sync5 == SYNC_BUSY ) {
		unlock( );
		return mlog_exit(EXIT_NORMAL, RV_DONE);
	}
	tranp->t_sync5 = SYNC_BUSY;
	unlock( );
	if ( drivecnt > 1 ) {
		mlog( MLOG_TRACE,
		      "waiting for other streams to exit\n" );
	}
	while ( cldmgr_otherstreamsremain( thrdix )) {
		sleep( 1 );
	}

	mlog( MLOG_DEBUG,
	      "tree finalize\n" );
	rv = finalize( path1, path2 );
	if (rv == RV_OK || rv == RV_INTR) {
		rval = EXIT_NORMAL;
	} else if (rv == RV_ERROR) {
		rval = EXIT_ERROR;
	} else {
		rval = EXIT_FAULT;
	}

	return mlog_exit(rval, rv);
}

/* called after all threads have exited. scans state to decide
 * if interrupted or not.
 */
bool_t
content_complete( void )
{
	bool_t completepr;
	time_t elapsed;

	if ( ! persp ) {
		completepr = BOOL_TRUE;
	} else if ( ! persp->a.valpr ) {
		completepr =  BOOL_TRUE;
	} else if ( ! persp->s.valpr ) {
		completepr =  BOOL_TRUE;
	} else {
		completepr = BOOL_FALSE;
	}

	elapsed = time( 0 ) - tranp->t_starttime;
	if ( persp ) {
		elapsed += persp->s.accumtime;
	}

	if ( completepr ) {
		if ( tranp->t_toconlypr ) {
			mlog( MLOG_VERBOSE, _(
			      "table of contents display complete"
			      ": %ld seconds elapsed"
			      "\n"),
			      elapsed );
		} else {
			int found;

			found = quotafilecheck("user",
					       persp->a.dstdir,
					       CONTENT_QUOTAFILE);
			found += quotafilecheck("project",
						persp->a.dstdir,
						CONTENT_PQUOTAFILE);
			found += quotafilecheck("group",
						persp->a.dstdir,
						CONTENT_GQUOTAFILE);

			if (found)
				mlog( MLOG_NORMAL,
				      _("use \'xfs_quota\' to restore quotas\n") );

			mlog( MLOG_VERBOSE, _(
			      "restore complete"
			      ": %ld seconds elapsed"
			      "\n"),
			      elapsed );
		}
	} else if ( tranp->t_toconlypr ) {
		mlog( MLOG_VERBOSE | MLOG_NOTE, _(
		      "table of contents display interrupted"
		      ": %ld seconds elapsed"
		      "\n"),
		      elapsed );
	} else {
		mlog( MLOG_VERBOSE | MLOG_NOTE, _(
		      "restore interrupted"
		      ": %ld seconds elapsed"
		      ": may resume later using -%c option"
		      "\n"),
		      elapsed,
		      GETOPT_RESUME );
	}

	/* accumulate total elapsed time
	 */
	if ( persp ) {
		persp->s.accumtime = elapsed;
	}

	if ( ! persp->a.valpr ) {
		wipepersstate( );
		persp = 0;
	}

	return completepr;
}

#define STATLINESZ	160

size_t
content_statline( char **linespp[ ] )
{
	static char statlinebuf[ 1 ][ STATLINESZ ];
	static char *statline[ 1 ];
	size64_t inodone;
	off64_t datadone;
	size64_t inocnt;
	off64_t datacnt;
	double percent;
	time_t elapsed;
	time_t now;
	struct tm *tmp;
	ix_t i;

	/* build and supply the line array
	 */
	for ( i = 0 ; i < 1 ; i++ ) {
		statline[ i ] = &statlinebuf[ i ][ 0 ];
	}
	*linespp = statline;

	if ( ! persp->s.stat_valpr ) {
		return 0;
	}

	/* calculate the elapsed time
	 */
	elapsed = persp->s.accumtime
		  +
		  ( time( 0 ) - tranp->t_starttime );

	/* get local time
	 */
	now = time( 0 );
	tmp = localtime( &now );

	if ( ! persp->s.dirdonepr ) {
		if ( ! tranp->t_dircnt ) {
			return 0;
		}

		percent = ( double )tranp->t_dirdonecnt
			  /
			  ( double )tranp->t_dircnt;
		percent *= 100.0;
		sprintf( statline[ 0 ], _(
			 "status at %02d:%02d:%02d: "
			 "%llu/%llu directories reconstructed, "
			 "%.1f%%%% complete, "
			 "%llu directory entries processed, "
			 "%ld seconds elapsed\n"),
			 tmp->tm_hour,
			 tmp->tm_min,
			 tmp->tm_sec,
			 (unsigned long long)tranp->t_dirdonecnt,
			 (unsigned long long)tranp->t_dircnt,
			 percent,
			 (unsigned long long)tranp->t_direntcnt,
			 elapsed );
		assert( strlen( statline[ 0 ] ) < STATLINESZ );
		
		return 1;
	}

	/* get the accumulated totals for non-dir inos and data bytes dumped.
	 * not under lock!
	 */
	inodone = persp->s.stat_inodone;
	datadone = persp->s.stat_datadone;
	inocnt = persp->s.stat_inocnt;
	datacnt = persp->s.stat_datacnt;

	/* calculate percentage of data dumped
	 */
	if ( datacnt ) {
		percent = ( double )datadone
			  /
			  ( double )datacnt;
		percent *= 100.0;
	} else {
		percent = 100.0;
	}
	if ( percent > 100.0 ) {
		percent = 100.0;
	}

	/* format the status line in a local static buffer (non-re-entrant!)
	 */
	sprintf( statline[ 0 ], _(
		 "status at %02d:%02d:%02d: %llu/%llu files restored, "
		 "%.1f%%%% complete, "
		 "%ld seconds elapsed\n"),
		 tmp->tm_hour,
		 tmp->tm_min,
		 tmp->tm_sec,
		 (unsigned long long)inodone,
		 (unsigned long long)inocnt,
		 percent,
		 elapsed );
	assert( strlen( statline[ 0 ] ) < STATLINESZ );
	
	/* return buffer to caller
	 */
	return 1;
}

void
content_showinv( void )
{
	pi_show_nomloglock( );
}

void
content_showremainingobjects( void )
{
	bool_t knownholespr = BOOL_FALSE;
	bool_t maybeholespr = BOOL_FALSE;
	bag_t *bagp;

	bagp = pi_neededobjs_nondir_alloc( &knownholespr,
					   &maybeholespr,
					   BOOL_TRUE,
					   BOOL_FALSE );
	display_needed_objects( PURP_NONDIR,
				bagp,
				knownholespr,
				maybeholespr );
	if ( bagp ) {
		pi_neededobjs_free( bagp );
		bagp = 0;
	}
}

/* dlog_begin already called; this is a second-level dialog.
 * prompt for each thread currently waiting for confirmation,
 * as well as an info prompt.
 */
#define PREAMBLEMAX	3
#define QUERYMAX	36
#define CHOICEMAX	30
#define ACKMAX		3
#define POSTAMBLEMAX	3
#define DLOG_TIMEOUT	300
#define DLOG_TIMEOUT_MEDIA	3600

#define CHOICESTRSZ	10
typedef struct { ix_t thrdix; char choicestr[ CHOICESTRSZ ]; } cttm_t;

char *
content_mediachange_query( void )
{
	cttm_t choicetothrdmap[ STREAM_SIMMAX ];
	char *querystr[ QUERYMAX ];
	size_t querycnt;
	char *choicestr[ CHOICEMAX ];
	size_t choicecnt;
	char *ackstr[ ACKMAX ];
	size_t ackcnt;
	size_t maxdrvchoiceix;
	size_t infoix;
	size_t nochangeix;
	size_t responseix;
	ix_t thrdix;

	infoix = querycnt = 0;
	querystr[ querycnt++ ] =
		_("select a drive to acknowledge media change\n");
	choicecnt = 0;
	maxdrvchoiceix = 0;
	for ( thrdix = 0 ; thrdix < STREAM_SIMMAX ; thrdix++ ) {
		if ( mcflag[ thrdix ] ) {
			choicetothrdmap[ choicecnt ].thrdix = thrdix;
			sprintf( choicetothrdmap[ choicecnt ].choicestr,
				 _("drive %u"),
				 (unsigned int)thrdix );
			choicestr[ choicecnt ] =
					choicetothrdmap[ choicecnt ].choicestr;
			maxdrvchoiceix = choicecnt;
			choicecnt++;
		}
	}
	if ( persp->s.valpr ) {
		infoix = choicecnt;
		choicestr[ choicecnt++ ] = _("display needed media objects");
	}
	nochangeix = choicecnt;
	choicestr[ choicecnt++ ] = _("continue");
	assert( choicecnt <= CHOICEMAX );
	responseix = dlog_multi_query( querystr,
				       querycnt,
				       choicestr,
				       choicecnt,
				       0,           /* hilitestr */
				       IXMAX,       /* hiliteix */
				       0,           /* defaultstr */
				       nochangeix,  /* defaultix */
				       DLOG_TIMEOUT,
				       nochangeix, /* timeout ix */
				       nochangeix, /* sigint ix */
				       nochangeix, /* sighup ix */
				       nochangeix);/* sigquit ix */
	if ( responseix <= maxdrvchoiceix ) {
		clr_mcflag( choicetothrdmap[ responseix ].thrdix );
		return _("media change acknowledged\n");
	}
	if ( responseix == infoix ) {
		bool_t knownholespr = BOOL_FALSE;
		bool_t maybeholespr = BOOL_FALSE;
		bag_t *bagp = pi_neededobjs_nondir_alloc( &knownholespr,
							  &maybeholespr,
							  BOOL_FALSE,
							  BOOL_FALSE );
		display_needed_objects( PURP_NONDIR,
					bagp,
					knownholespr,
					maybeholespr );
		if ( bagp ) {
			pi_neededobjs_free( bagp );
			bagp = 0;
		}
		ackcnt = 0;
		dlog_multi_ack( ackstr,
				ackcnt );
		querycnt = 0;
		choicecnt = 0;
		nochangeix = choicecnt;
		choicestr[ choicecnt++ ] = _("continue");
		assert( choicecnt <= CHOICEMAX );
		responseix = dlog_multi_query( querystr,
					       querycnt,
					       choicestr,
					       choicecnt,
					       0,           /* hilitestr */
					       IXMAX,       /* hiliteix */
					       0,           /* defaultstr */
					       nochangeix,  /* defaultix */
					       DLOG_TIMEOUT,
					       nochangeix, /* timeout ix */
					       nochangeix, /* sigint ix */
					       nochangeix, /* sighup ix */
					       nochangeix);/* sigquit ix */
		return _("continuing\n");
	}
	assert( responseix == nochangeix );
	return _("continuing\n");
}

/* definition of locally defined static functions ****************************/

/* does all pre-processing leading up to applying the dirdump,
 * then applies the dirdump. updates pers progress flags along the way.
 * does NOT do any post-processing!
 */
/* ARGSUSED */
static rv_t
applydirdump( drive_t *drivep,
	      dh_t fileh,
	      content_inode_hdr_t *scrhdrp,
	      filehdr_t *fhdrp )
{
	bool_t fhcs;
	bool_t dhcs;
	bool_t ahcs;

	fhcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_FILEHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;
	dhcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_DIRENTHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;
	ahcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_EXTATTRHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;

	if ( ! persp->s.marknorefdonepr ) {
		tree_marknoref( );
		persp->s.marknorefdonepr = BOOL_TRUE;
	}

	if ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_NOTSELFCONTAINED ) {
		mlog( MLOG_NORMAL | MLOG_NOTE, _(
		      "dump is not self-contained, "
		      "orphaned files expected if base dump(s) "
		      "was not applied\n") );
	}

	if ( ! persp->s.dirdonepr ) {
		rv_t rv;
		dah_t dah;

		char _direntbuf[ sizeof( direnthdr_t )
				+
				NAME_MAX + 1
				+
				DIRENTHDR_ALIGN ];
		char *direntbuf = ALIGN_PTR(_direntbuf, DIRENTHDR_ALIGN);
		size_t direntbufsz =
			sizeof(_direntbuf) - (direntbuf - _direntbuf);

		mlog( MLOG_TRACE,
		      "reading the ino map\n" );
		rv = inomap_restore_pers( drivep, scrhdrp, tranp->t_hkdir );
		if ( rv != RV_OK ) {
			return rv;
		}

		tranp->t_dircnt = scrhdrp->cih_inomap_dircnt;
		tranp->t_dirdonecnt = 0;
		tranp->t_direntcnt = 0;

		mlog( MLOG_TRACE,
		      "reading the directories \n" );

		dah = DAH_NULL;
		for (;;) {
			nh_t dirh;

			/* read the file header
			 */
			rv = read_filehdr( drivep, fhdrp, fhcs );
			if ( rv ) {
				return rv;
			}

			/* if this is a null file hdr, we're done
			 * reading dirs, and there are no nondirs.
			 * done.
			 */
			if ( fhdrp->fh_flags & FILEHDR_FLAGS_NULL ) {
				break;
			}

			/* if its not a directory, must be the
			 * first non-dir file. done.
			 */
			if ( ( fhdrp->fh_stat.bs_mode & S_IFMT ) != S_IFDIR ) {
				break;
			}

			/* if stop requested bail out gracefully
			 */
			if ( cldmgr_stop_requested( )) {
				return RV_INTR;
			}

			/* if in a pipeline , call preemptchk( ) to
			 * print status reports
			 */
			if ( pipeline )
			{
				mlog( MLOG_DEBUG ,
					"preemptchk( )\n");
				preemptchk( );
			}

			/* may be an extended attributes file hdr
			 */
			if ( fhdrp->fh_flags & FILEHDR_FLAGS_EXTATTR ) {
				rv = restore_extattr( drivep,
						      fhdrp,
						      0,
						      ahcs,
						      BOOL_TRUE, /* isdirpr */
						      BOOL_FALSE, /* onlydoreadpr */
						      dah );
				if ( rv != RV_OK ) {
					return rv;
				}
				continue;
			}

			/* add the directory to the tree. save the
			 * returned tree node id, to associate with
			 * the directory entries. get the dirattr handle,
			 * so that any extattr following will be associated
			 * with that dir.
			 */
			dah = DAH_NULL;
			dirh = tree_begindir( fhdrp, &dah );
			if (dirh == NH_NULL)
			    return RV_ERROR;

			/* read the directory entries, and populate the
			 * tree with them. we can tell when we are done
			 * by looking for a null dirent.
			 */
			for ( ; ; ) {
				register direnthdr_t *dhdrp =
						    ( direnthdr_t * )direntbuf;
				register size_t namelen;

				rv = read_dirent( drivep,
						  dhdrp,
						  direntbufsz,
						  dhcs );
				if ( rv != RV_OK ) {
					return rv;
				}

				/* if null, we're done with this dir.
				 * break out of this inner loop and
				 * move on th the next dir.
				 */
				if ( dhdrp->dh_ino == 0 ) {
					break;
				}
				namelen = strlen( dhdrp->dh_name );
				assert( namelen <= NAME_MAX );

				/* add this dirent to the tree.
				 */
				rv = tree_addent( dirh,
					     dhdrp->dh_ino,
					     dhdrp->dh_gen,
					     dhdrp->dh_name,
					     namelen );
				if ( rv != RV_OK ) {
					return rv;
				}
				tranp->t_direntcnt++;
			}

			tree_enddir( dirh );

			tranp->t_dirdonecnt++;
		}

		if ((rv = dirattr_flush()) != RV_OK) {
			return rv;
		}

		if ((rv = namreg_map()) != RV_OK) {
			return rv;
		}

		persp->s.dirdonepr = BOOL_TRUE;
	}

	mlog( MLOG_VERBOSE, _("%llu directories and %llu entries processed\n"),
		tranp->t_dirdonecnt, tranp->t_direntcnt);

	return RV_OK;
}

/* like applydirdump, but just eats up the inomap/dirdump portion of the
 * dump, doesn't use it. the first non-dir filehdr_t is copied into supplied
 * buffer. returns integer error code from drive ops used.
 */
/* ARGSUSED */
rv_t
eatdirdump( drive_t *drivep,
	    dh_t fileh,
	    content_inode_hdr_t *scrhdrp,
	    filehdr_t *fhdrp )
{
	bool_t fhcs;
	bool_t dhcs;
	bool_t ahcs;

	char _direntbuf[ sizeof( direnthdr_t )
			+
			NAME_MAX + 1
			+
			DIRENTHDR_ALIGN ];
	char *direntbuf = ALIGN_PTR(_direntbuf, DIRENTHDR_ALIGN);
	size_t direntbufsz = sizeof(_direntbuf) - (direntbuf - _direntbuf);
	rv_t rv;

	fhcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_FILEHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;
	dhcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_DIRENTHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;
	ahcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_EXTATTRHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;

	mlog( MLOG_DEBUG,
	      "discarding ino map\n" );
	rv = inomap_discard( drivep, scrhdrp );
	if ( rv != RV_OK ) {
		return rv;
	}

	mlog( MLOG_DEBUG,
	      "discarding directories \n" );
	for (;;) {
		/* read the file header
		 */
		rv = read_filehdr( drivep, fhdrp, fhcs );
		if ( rv ) {
			return rv;
		}

		/* if this is a null file hdr, we're done
		 * reading dirs, and there are no nondirs.
		 * done.
		 */
		if ( fhdrp->fh_flags & FILEHDR_FLAGS_NULL ) {
			break;
		}

		/* if its not a directory, must be the
		 * first non-dir file. done.
		 */
		if ( ( fhdrp->fh_stat.bs_mode & S_IFMT ) != S_IFDIR ) {
			break;
		}

		/* if stop requested bail out gracefully
		 */
		if ( cldmgr_stop_requested( )) {
			return RV_INTR;
		}

		/* may be an extended attributes file hdr
		 */
		if ( fhdrp->fh_flags & FILEHDR_FLAGS_EXTATTR ) {
			rv = restore_extattr( drivep,
					      fhdrp,
					      0,
					      ahcs,
					      BOOL_TRUE, /* isdirpr */
					      BOOL_TRUE, /* onlydoreadpr */
					      DAH_NULL );
			if ( rv != RV_OK ) {
				return rv;
			}
			continue;
		}

		/* read the directory entries.
		 * we can tell when we are done
		 * by looking for a null dirent.
		 */
		for ( ; ; ) {
			register direnthdr_t *dhdrp =
					    ( direnthdr_t * )direntbuf;
			/* REFERENCED */
			register size_t namelen;

			rv = read_dirent( drivep,
					  dhdrp,
					  direntbufsz,
					  dhcs );
			if ( rv ) {
				return rv;
			}

			/* if null, we're done with this dir.
			 * break out of this inner loop and
			 * move on th the next dir.
			 */
			if ( dhdrp->dh_ino == 0 ) {
				break;
			}
			namelen = strlen( dhdrp->dh_name );
			assert( namelen <= NAME_MAX );
		}
	}

	return RV_OK;
}

/* does all post-processing of the tree required prior to restoral of
 * the non-directory portion of the dump. only one thread at a time,
 * so no locking needed. since the steps are interdependent, loops
 * until no point in doing so.
 */
static rv_t
treepost( char *path1, char *path2 )
{
	bool_t ok;

#ifdef TREE_CHK
	/* first scan the tree for corruption
	 */
	mlog( MLOG_DEBUG | MLOG_TREE,
	      "checking tree for consistency\n" );
	if ( ! tree_chk( )) {
		return RV_CORE;
	}
#endif /* TREE_CHK */

	/* adjust ref flags based on what dirs were dumped
	 */
	if ( ! persp->s.adjrefdonepr ) {
		mlog( MLOG_DEBUG | MLOG_TREE,
		      "adjusting dirent ref flags\n" );
		ok = tree_adjref( );
		if ( ! ok ) {
			return RV_INTR;
		}
		persp->s.adjrefdonepr = BOOL_TRUE;
	}

	/* if a subtree or interactive restore, sanitize the inomap
	 * so only inos selected by subtree or interactive cmds will
	 * be present in inomap.
	 */
	if ( ! persp->s.inomapsanitizedonepr ) {
		if ( persp->a.interpr
		     ||
		     ( persp->a.firststsenseprvalpr
		       &&
		       persp->a.firststsensepr )) {
			inomap_sanitize( );
		}
		persp->s.inomapsanitizedonepr = BOOL_TRUE;
	}

	/* apply subtree selections
	 */
	if ( ! persp->s.stdonepr ) {
		ix_t stix;
		stdesc_t *stdescp;

		mlog( MLOG_DEBUG | MLOG_TREE,
		      "applying subtree selections\n" );

		/* if first subtree selection is inclusive in sense,
		 * first mark the entire tree as unselected. otherwise,
		 * select all (no subtree selections or first was excluding).
		 */
		if ( ( persp->a.interpr
		       &&
		       ( ! persp->a.firststsenseprvalpr
		         ||
		         persp->a.firststsensepr ))
		     ||
		     ( persp->a.firststsenseprvalpr
		       &&
		       persp->a.firststsensepr )) {
			tree_markallsubtree( BOOL_FALSE );
		} else {
			tree_markallsubtree( BOOL_TRUE );
		}

		/* now apply all subtree commands from command line
		 */
		for ( stix = 0,
		      stdescp = ( stdesc_t * )( ( char * )persp + perssz )
		      ;
		      stix < persp->a.stcnt
		      ;
		      stix++,
		      stdescp = ( stdesc_t * )( ( char * )stdescp
						+
						stdescp->std_nextoff )) {
			ok = tree_subtree_parse( stdescp->std_sensepr,
						 stdescp->std_path );
			if ( ! ok ) {
				mlog( MLOG_NORMAL | MLOG_ERROR, _(
				      "subtree argument %s invalid\n"),
				      stdescp->std_path );
				return RV_ERROR;
			}
		}
		persp->s.stdonepr = BOOL_TRUE;
	}

	/* next engage interactive subtree selection
	 */
	if ( ! persp->s.interdonepr ) {
		if ( persp->a.interpr ) {
			ok = tree_subtree_inter( );
			if ( ! ok ) {
				return RV_INTR;
			}
		}
		persp->s.interdonepr = BOOL_TRUE;
	}

	ok = tree_post( path1, path2 );

	if ( ! ok ) {
		return RV_INTR;
	}

	ok = tree_extattr( restore_dir_extattr_cb, path1 );
	if ( ! ok ) {
		return RV_INTR;
	}

	persp->s.treepostdonepr = BOOL_TRUE;

	return RV_OK;
}

static rv_t
applynondirdump( drive_t *drivep,
		 dh_t fileh,
		 content_inode_hdr_t *scrhdrp,
		 char *path1,
		 char *path2,
		 filehdr_t *fhdrp )
{
	rv_t rv = RV_UNKNOWN;
	bool_t fhcs;
	bool_t ehcs;
	bool_t ahcs;
	egrp_t first_egrp;
	egrp_t next_egrp;
	stream_context_t *strctxp = (stream_context_t *)drivep->d_strmcontextp;

	/* determine if file header and/or extent heade checksums present
	 */
	fhcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_FILEHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;
	ehcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_EXTENTHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;
	ahcs = ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_EXTATTRHDR_CHECKSUM )
	       ?
	       BOOL_TRUE
	       :
	       BOOL_FALSE;

	/* determine the first and next egrps needed from this media file.
	 * used to decide if stats should be updated
	 */
	pi_bracketneededegrps( fileh, &first_egrp, &next_egrp );

	/* initialize the stream context
	 */
	memset(&strctxp->sc_bstat, 0, sizeof(bstat_t));
	strctxp->sc_path[0] = '\0';
	strctxp->sc_fd = -1;
	strctxp->sc_ownerset = BOOL_FALSE;


	for ( ; ; ) {
		drive_ops_t *dop = drivep->d_opsp;
		drive_mark_t drivemark;
		bstat_t *bstatp = &fhdrp->fh_stat;
		bool_t resyncpr = BOOL_FALSE;
		int rval;

		/* if a null file header, break
		 */
		if ( fhdrp->fh_flags & FILEHDR_FLAGS_NULL ) {
			rv = RV_OK;
			goto applynondirdump_out;
		}

		/* if working on a different file than we were previously,
		 * complete the old one and begin the new one.
		 */
		if ( bstatp->bs_ino != strctxp->sc_bstat.bs_ino ) {

			restore_complete_reg(strctxp);

			/* start new ino */
			memcpy(&strctxp->sc_bstat, bstatp, sizeof(bstat_t));
			strctxp->sc_path[0] = '\0';
			strctxp->sc_fd = -1;
			strctxp->sc_ownerset = BOOL_FALSE;

			rv = restore_file( drivep, fhdrp, ehcs, ahcs, path1, path2 );

		} else if ( fhdrp->fh_flags & FILEHDR_FLAGS_EXTATTR ) {
			rv = restore_extattr( drivep,
					      fhdrp,
					      strctxp->sc_path,
					      ahcs,
					      BOOL_FALSE, /* isdirpr */
					      BOOL_FALSE, /* onlydoreadpr */
					      DAH_NULL );
		} else {
			/* Must be another extent group for the current file */
			restore_extent_group( drivep,
					      fhdrp,
					      strctxp->sc_path,
					      strctxp->sc_fd,
					      ehcs,
					      &rv );
		}

		switch( rv ) {
		case RV_OK:
			break;
		case RV_EOD:
			rv = RV_OK;
			goto applynondirdump_out;
		case RV_CORRUPT:
			rval = ( * dop->do_next_mark )( drivep );
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_WARNING, _(
				      "unable to resync media file: "
				      "some portion of dump will NOT "
				      "be restored\n") );
				rv = RV_OK;  /* treat as EOD */
				goto applynondirdump_out;
			}
			resyncpr = BOOL_TRUE;
			break;
		default:
			goto applynondirdump_out;
		}

		/* update stats if appropriate
		 */
		if ( ( ( bstatp->bs_mode & S_IFMT ) == S_IFREG )
		     &&
		     ! ( fhdrp->fh_flags & FILEHDR_FLAGS_EXTATTR )
		     &&
		     fhdrp->fh_offset == 0 ) {
			egrp_t cur_egrp;
			cur_egrp.eg_ino = fhdrp->fh_stat.bs_ino;
			cur_egrp.eg_off = fhdrp->fh_offset;
			if (cur_egrp.eg_ino > first_egrp.eg_ino ) {
				if ( cur_egrp.eg_ino < next_egrp.eg_ino
				     ||
				     next_egrp.eg_off > 0 ) {
					assert( cur_egrp.eg_ino
						<=
						next_egrp.eg_ino );
					pi_update_stats( bstatp->bs_blocks
							 *
							 ( off64_t )
							 bstatp->bs_blksize );
				}
			}
		}

		do {
			/* get a mark for the next read, in case we restart here
			 */
			( * dop->do_get_mark )( drivep, &drivemark );

			/* read the file header. 
			 */
			rv = read_filehdr( drivep, fhdrp, fhcs );
			switch( rv ) {
			case RV_OK:
				break;
			case RV_EOD:
				rv = RV_OK;
				goto applynondirdump_out;
			case RV_CORRUPT:
				rval = ( * dop->do_next_mark )( drivep );
				if ( rval ) {
					mlog( MLOG_NORMAL | MLOG_WARNING, _(
					      "unable to resync media file: "
					      "some portion of dump will NOT "
					      "be restored\n") );
					rv = RV_OK;  /* treat as EOD */
					goto applynondirdump_out;
				}
				resyncpr = BOOL_TRUE;
			default:
				goto applynondirdump_out;
			}

			if ( resyncpr && rv == RV_OK ) {
				mlog( MLOG_NORMAL | MLOG_WARNING, _(
				      "resynchronization achieved at "
				      "ino %llu offset %lld\n"),
				      fhdrp->fh_stat.bs_ino,
				      fhdrp->fh_offset );
				resyncpr = BOOL_FALSE;
			}
		} while ( resyncpr );

		/* checkpoint into persistent state if not a null file hdr
		 */
		if ( ! ( fhdrp->fh_flags & FILEHDR_FLAGS_NULL )) {
			pi_checkpoint( fileh,
				       &drivemark,
				       fhdrp->fh_stat.bs_ino,
				       fhdrp->fh_offset );
		}

		/* if in a pipeline , call preemptchk( ) to
		 * print status reports
		 */
		if ( pipeline )
		{
			mlog( MLOG_DEBUG ,
				"preemptchk( )\n");
			preemptchk( );
		}
	}

applynondirdump_out:

	/* We've hit the end of this media file or encountered corruption.
	 * In either case, we may not be back to complete the metadata for
	 * this file, so attempt to complete it now.
	 */
	restore_complete_reg(strctxp);

	return rv;
}

/* ARGSUSED */
static rv_t
finalize( char *path1, char *path2 )
{
	bool_t ok;

	if ( ! tranp->t_toconlypr ) {

		/* restore directory attributes
		 */
		if ( ! persp->s.dirattrdonepr ) {;
			ok = tree_setattr( path1 );
			if ( ! ok ) {
				return RV_INTR;
			}
			persp->s.dirattrdonepr = BOOL_TRUE;
		}

		/* remove orphanage if empty
		 */
		if ( ! persp->s.orphdeltriedpr ) {;
			ok = tree_delorph( );
			if ( ! ok ) {
				return RV_INTR;
			}
			persp->s.orphdeltriedpr = BOOL_TRUE;
		}

		/* delete the persistent ino map
		 */
		if ( ! persp->s.inomapdelpr ) {
			inomap_del_pers( tranp->t_hkdir );
			persp->s.inomapdelpr = BOOL_TRUE;
		}
	}

	/* at this point, all session-only persistent state has been deleted.
	 * if this is a cumulative restore, just update the pers cum state and
	 * invalidate the pers session state. otherwise, invalidate the
	 * persistent state. content_complete will remove housekeeping dir.
	 */
	if ( persp->a.cumpr ) {
		/* following must be atomic!
		 */
		persp->a.dumpcnt++;
		uuid_copy(persp->a.lastdumpid, persp->s.dumpid);
		strcpy( persp->a.lastdumplab, persp->s.dumplab );
		persp->s.valpr = BOOL_FALSE;
	} else {
		persp->a.valpr = BOOL_FALSE;
	}
	return RV_OK;
}

static void
toconly_cleanup( void )
{
	if ( ! tranp ) {
		return;
	}

	if ( ! tranp->t_toconlypr ) {
		return;
	}

	if ( ! tranp->t_hkdir ) {
		return;
	}

	if ( ! strlen( tranp->t_hkdir )) {
		return;
	}

	wipepersstate( );
}

static void
wipepersstate( void )
{
	DIR *dirp;
	struct dirent64 *direntp;
	char pathname[ MAXPATHLEN ];
	dirp = opendir( tranp->t_hkdir );
	if ( ! dirp ) {
		return;
	}

	while ( ( direntp = readdir64( dirp )) != 0 ) {
		/* REFERENCED */
		int len;
		if ( ! strcmp( direntp->d_name, "." )) {
			continue;
		}
		if ( ! strcmp( direntp->d_name, ".." )) {
			continue;
		}
		len = sprintf( pathname,
			       "%s/%s",
			       tranp->t_hkdir,
			       direntp->d_name );
		assert( len > 0 );
		assert( len < MAXPATHLEN );
		( void )unlink( pathname );
		closedir( dirp );
		dirp = opendir( tranp->t_hkdir );
		if ( ! dirp ) {
			return;
		}
	}
	closedir( dirp );

	rmdir( tranp->t_hkdir );
}

/* Inv abstraction ***********************************************************/

/* attempt to validate id or label against online inventory.
 * sets pers id/label and pers idvalpr etc as side-effect (does NOT set valpr!)
 */
static bool_t
Inv_validate_cmdline( void )
{
	inv_session_t *sessp;
	bool_t ok;
	bool_t rok;

	assert( ! persp->s.valpr );

	ok = BOOL_FALSE;
	sessp = 0;
	if ( tranp->t_reqdumpidvalpr ) {
		ok = inv_get_session_byuuid(NULL, &tranp->t_reqdumpid, &sessp);
	} else if ( tranp->t_reqdumplabvalpr ) {
		ok = inv_get_session_bylabel(NULL, tranp->t_reqdumplab, &sessp);
	}
	rok = BOOL_FALSE;
	if ( ok && sessp ) {
		uuid_t baseid;

		uuid_clear(baseid);	
                askinvforbaseof( baseid, sessp );
		if ( ! dumpcompat( sessp->s_isresumed,
				   ( ix_t )( sessp->s_level ),
				   baseid,
				   BOOL_TRUE )) {
			inv_free_session( &sessp );
			return BOOL_ERROR;
		}

		mlog( MLOG_VERBOSE, _(
		      "using online session inventory\n") );
		persp->s.fullinvpr = pi_transcribe( sessp );
		if ( persp->s.fullinvpr ) {
			strncpyterm( persp->s.dumplab,
				     sessp->s_label,
				     sizeof( persp->s.dumplab ));
			uuid_copy(persp->s.dumpid, sessp->s_sesid);
			rok = BOOL_TRUE;
		}
		inv_free_session( &sessp );
	}
	return rok;
}


/* Media abstraction *********************************************************/


static Media_t *
Media_create( ix_t thrdix )
{
	drive_t *drivep = drivepp[ thrdix ];
	global_hdr_t *grhdrp = drivep->d_greadhdrp;
	drive_hdr_t *drhdrp = drivep->d_readhdrp;
	media_hdr_t *mrhdrp = ( media_hdr_t * )drhdrp->dh_upper;
	content_hdr_t *crhdrp = ( content_hdr_t * )mrhdrp->mh_upper;
	content_inode_hdr_t *scrhdrp =
				( content_inode_hdr_t * )crhdrp->ch_specific;
	Media_t *Mediap;


	mlog( MLOG_DEBUG,
	      "Media_create\n" );

	Mediap = ( Media_t * )calloc( 1, sizeof( Media_t ));
	Mediap->M_drivep = drivep;
	Mediap->M_grhdrp = grhdrp;
	Mediap->M_drhdrp = drhdrp;
	Mediap->M_mrhdrp = mrhdrp;
	Mediap->M_crhdrp = crhdrp;
	Mediap->M_scrhdrp = scrhdrp;
	assert( POS_UNKN == 0 );

	return Mediap;
}

/* these calls allow the Media users to clue Media in to fine position changes
 * within the current media file
 */
static void
Media_indir( Media_t *Mediap )
{
	mlog( MLOG_DEBUG,
	      "Media_indir\n" );

	Mediap->M_pos = POS_INDIR;
}

static void
Media_atnondir( Media_t *Mediap )
{
	mlog( MLOG_DEBUG,
	      "Media_atnondir\n" );

	Mediap->M_pos = POS_ATNONDIR;
}

/* supplies pertinent media files to the caller. if purpose is search,
 * returns all media files. otherwise, returns only media files with the
 * dump ID. smart enough to know that if purpose was search but is now dir,
 * current media file can be returned again. same for other transitions.
 * always traverses the media object in a forward direction, beginning with
 * current media file, wrapping around to beginning of media if necessary. 
 * also supplies fresh hdr pointers and drive manager. in current
 * implementation these do not change, but will when we use new TLM. does
 * fine positioning within media file according to purpose of request.
 *
 * Note: 
 * The difference between rval and rv. 
 * rval is used for the drive_* functions (e.g. do_begin_read)
 * and will take on values such as DRIVE_ERROR_*.
 * However, it also set to 0 for no error and 1 for error.
 * It is defaulted to 0 for no error.
 * rv, however, is of type rv_t and is used for functions returning
 * rv_t and for the result of this function.
 * It takes on values like RV_OK, RV_EOF.
 */
static rv_t
Media_mfile_next( Media_t *Mediap,
		  purp_t purp,
		  sync_t *donesyncp,
		  dh_t  *filehp,
		  global_hdr_t **grhdrpp,
		  drive_hdr_t **drhdrpp,
		  media_hdr_t **mrhdrpp,
		  content_hdr_t **crhdrpp,
		  content_inode_hdr_t **scrhdrpp,
		  drive_t **drivepp,
		  filehdr_t *fhdrp ) /*caller-supplied buf if NONDIR purp*/
{
	drive_t *drivep = Mediap->M_drivep;
	drive_ops_t *dop = drivep->d_opsp;
	global_hdr_t *grhdrp = Mediap->M_grhdrp;
	drive_hdr_t *drhdrp = Mediap->M_drhdrp;
	media_hdr_t *mrhdrp = Mediap->M_mrhdrp;
	content_hdr_t *crhdrp = Mediap->M_crhdrp;
	content_inode_hdr_t *scrhdrp = Mediap->M_scrhdrp;
	dh_t fileh;
	int rval = 0; /* no error by default */
	rv_t rv;
	bool_t ok;
	uuid_t prevmfiledumpid;

	mlog( MLOG_DEBUG | MLOG_MEDIA,
	      "Media_mfile_next: purp==%d pos==%d\n",
	      purp,
	      Mediap->M_pos );

	/* pass back hdr and drive ptrs
	 */
	*grhdrpp =  grhdrp;
	*drhdrpp =  drhdrp;
	*mrhdrpp =  mrhdrp;
	*crhdrpp =  crhdrp;
	*scrhdrpp =  scrhdrp;
	*drivepp = drivep;

	/* if ref return for pers mfile desc supplied, pre-zero
	 */
	if ( filehp ) {
		*filehp = DH_NULL;
	}

	/* keep a close eye on the validity of fileh
	 */
	fileh = DH_NULL;

	/* if purpose has changed, invalidate first, last, and previous indices
	 */
	if ( Mediap->M_flmfixvalpr ) {
		if ( purp != Mediap->M_mfixpurp ) {
			Mediap->M_flmfixvalpr = BOOL_FALSE;
			Mediap->M_pmfixvalpr = BOOL_FALSE;
		}
	}

	/* use a local variable to keep track of dump sessions seen on
	 * media. if not in search mode, each time we see a different
	 * dump session, log a message to keep the user informed.
	 * invalidated each time we change media or rewind.
	 */
	uuid_clear(prevmfiledumpid);

	/* if restore is complete, return indication. be sure to end read
	 * if active.
	 */
	if ( purp == PURP_NONDIR
	     &&
	     pi_alldone( )) {
		if ( Mediap->M_pos == POS_ATHDR
		     ||
		     Mediap->M_pos == POS_INDIR
		     ||
		     Mediap->M_pos == POS_ATNONDIR ) {
			( * dop->do_end_read )( drivep );
			Mediap->M_pos = POS_UNKN;
			fileh = DH_NULL;
		}
		return RV_NOMORE;
	}

	/* loop searching for an acceptable media file.
	 * change media as necessary.
	 */
	for ( ; ; ) {
		bool_t emptypr; /* begin_read says drive empty */
		bool_t partofdumppr;
		bool_t hassomepr;
		bool_t resumepr;
		bool_t canseeknextpr;
		drive_mark_t chkpnt;
		bool_t fhcs;
		bag_t *bagp = NULL;
		bool_t knownholespr;
		bool_t maybeholespr;
		xfs_ino_t begino;
		xfs_ino_t endino;
		int dcaps = drivep->d_capabilities;
		dh_t objh = DH_NULL;

		emptypr = BOOL_FALSE;

		/* check if no point in going on
		 */
		if ( cldmgr_stop_requested( )) {
			return RV_INTR;
		}
		if ( donesyncp && *donesyncp == SYNC_DONE ) {
			return RV_DONE;
		}
		if ( purp == PURP_NONDIR
		     &&
		     pi_alldone( )) {
			return RV_NOMORE;
		}

		/* if we have a useless media object, get another one
		 */
		if ( Mediap->M_pos == POS_USELESS
		     ||
		     Mediap->M_pos == POS_BLANK ) {
			goto newmedia;
		}

		/* if the purpose if to search, and we already have
		 * a media file, that media file has already been
		 * searched, so set pos to cause another begin read
		 */
		if ( purp == PURP_SEARCH ) {
			if ( Mediap->M_pos == POS_ATHDR
			     ||
			     Mediap->M_pos == POS_INDIR
			     ||
			     Mediap->M_pos == POS_ATNONDIR ) {
				Mediap->M_pos = POS_UNKN;
			}
		}

		/* if already have a media file, skip the begin_read
		 */
		if ( Mediap->M_pos == POS_ATHDR
		     ||
		     Mediap->M_pos == POS_INDIR
		     ||
		     Mediap->M_pos == POS_ATNONDIR ) {
			goto validate;
		}

		/* see if the indices say we've seen all there is to see
		 */
		if ( Mediap->M_flmfixvalpr ) {
			if ( Mediap->M_pos == POS_UNKN ) {
				if ( Mediap->M_lmfix + 1 == Mediap->M_fmfix ) {
					goto newmedia;
				}
			}
			if ( Mediap->M_pos == POS_END ) {
				if ( Mediap->M_fmfix == 0 ) {
					goto newmedia;
				}
				if ( Mediap->M_fsfixvalpr
				     &&
				     Mediap->M_fmfix <= Mediap->M_fsfix ) {
					goto newmedia;
				}
			}
		}

		/* if we are at the end, do a rewind, or get new media
		 * if rewinds not possible. this may take a while, so
		 * afterwards check for interrupt or if someone else
		 * has finished the job.
		 */
		if ( Mediap->M_pos == POS_END ) {
			if ( ! ( dcaps & DRIVE_CAP_REWIND )) {
				goto newmedia;
			}
			mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
			      "rewinding\n") );
			( * drivep->d_opsp->do_rewind )( drivep );
			Mediap->M_pos = POS_UNKN;
			if ( cldmgr_stop_requested( )) {
				return RV_INTR;
			}
			if ( donesyncp && *donesyncp == SYNC_DONE ) {
				return RV_DONE;
			}
			if ( purp == PURP_NONDIR
			     &&
			     pi_alldone( )) {
				return RV_NOMORE;
			}
		}

		/* begin a new media file, and determine new position.
		 * bail if catastrophic. also, tell pi about EOD/EOM
		 * if appropriate.
		 */
		rval = ( * drivep->d_opsp->do_begin_read )( drivep );
		switch ( rval ) {
		case 0:
			mlog_lock( );
			mlog( MLOG_VERBOSE | MLOG_NOLOCK | MLOG_MEDIA, _(
			      "examining media file %u\n"),
			      mrhdrp->mh_mediafileix );
			mlog( MLOG_TRACE | MLOG_NOLOCK | MLOG_MEDIA,
			      "file %u in "
			      "object %u "
			      "of stream %u\n",
			      mrhdrp->mh_mediafileix,
			      mrhdrp->mh_mediaix,
			      drhdrp->dh_driveix );
			mlog( MLOG_TRACE | MLOG_NOLOCK | MLOG_MEDIA,
			      "file %u in stream, "
			      "file %u of dump %u on object\n",
			      mrhdrp->mh_dumpfileix,
			      mrhdrp->mh_dumpmediafileix,
			      mrhdrp->mh_dumpmediaix );
			mlog_unlock( );

			Mediap->M_pos = POS_ATHDR;
			if ( Mediap->M_flmfixvalpr ) {
				Mediap->M_pmfix = Mediap->M_lmfix;
				Mediap->M_pmfixvalpr = BOOL_TRUE;
			}

			pi_note_indrive( drhdrp->dh_driveix,
					 mrhdrp->mh_mediaid );

			break;
		case DRIVE_ERROR_EOD:
			Mediap->M_pos = POS_END;
			if ( Mediap->M_fsfixvalpr ) {
				assert( purp != PURP_SEARCH );
				pi_hiteod( Mediap->M_fssix,
					   Mediap->M_fsoix );
			}
			break;
		case DRIVE_ERROR_EOM:
			Mediap->M_pos = POS_END;
			if ( Mediap->M_fsfixvalpr ) {
				assert( purp != PURP_SEARCH );
				pi_hiteom( Mediap->M_fssix,
					   Mediap->M_fsoix );
			}
			break;
		case DRIVE_ERROR_MEDIA:
/*
 * pv: 766024; tes@engr
 * The setting of emptypr, in my opinion, should only happen
 * in the case that the drive does not have a tape online.
 * This corresponds to a couple of cases in prepare_drive(). 
 * Otherwise, when we go to a newmedia we won't be able to eject
 * the tape when we want/need to.
 * This may need to be reviewed in the future.
 */
			emptypr = BOOL_TRUE; /* so don't try to eject */
			/* fall through */
		case DRIVE_ERROR_FOREIGN:
		case DRIVE_ERROR_FORMAT:
		case DRIVE_ERROR_VERSION:
		case DRIVE_ERROR_EOF:
			Mediap->M_pos = POS_USELESS;
			break;
		case DRIVE_ERROR_BLANK:
			Mediap->M_pos = POS_BLANK;
			break;
		case DRIVE_ERROR_CORRUPTION:
			Mediap->M_pos = POS_UNKN;
			continue;
		case DRIVE_ERROR_STOP:
			return RV_INTR;
		case DRIVE_ERROR_DEVICE:
			return RV_DRIVE;
		default:
			return RV_CORE;
		}

validate:
		/* update the positional indices
		 */
		if ( Mediap->M_pos == POS_ATHDR
		     ||
		     Mediap->M_pos == POS_INDIR
		     ||
		     Mediap->M_pos == POS_ATNONDIR ) {
			if ( ! Mediap->M_flmfixvalpr ) {
				Mediap->M_fmfix = mrhdrp->mh_mediafileix;
				Mediap->M_mfixpurp = purp;
				Mediap->M_flmfixvalpr = BOOL_TRUE;
			}
			Mediap->M_lmfix = mrhdrp->mh_mediafileix;
		}

		/* check for interrupt. be sure to end_read if necessary
		 */
		if ( cldmgr_stop_requested( )) {
			if ( Mediap->M_pos == POS_ATHDR
			     ||
			     Mediap->M_pos == POS_INDIR
			     ||
			     Mediap->M_pos == POS_ATNONDIR ) {
				( * dop->do_end_read )( drivep );
				Mediap->M_pos = POS_UNKN;
				fileh = DH_NULL;
			}
			return RV_INTR;
		}

		/* check if another thread has finished job (for this purpose).
		 * don't end_read, we will be back.
		 */
		if ( donesyncp && *donesyncp == SYNC_DONE ) {
			return RV_DONE;
		}

		/* we may be done due to the actions of other threads.
		 * if so, return indicating so
		 */
		if ( purp == PURP_NONDIR
		     &&
		     pi_alldone( )) {
			return RV_NOMORE;
		}

		/* if the media object is useless, go get more
		 */
		if ( Mediap->M_pos == POS_USELESS
		     ||
		     Mediap->M_pos == POS_BLANK ) {
			goto newmedia;
		}

		/* if we hit the end, this is not a search, and we've
		 * seen at least one media file pertaining to the dump,
		 * ask the inventory if there is any point in examining
		 * the beginning of the object.
		 */
		if ( Mediap->M_pos == POS_END
		     &&
		     purp != PURP_SEARCH
		     &&
		     Mediap->M_fsfixvalpr
		     &&
		     pi_know_no_more_on_object( purp,
						Mediap->M_fssix,
						Mediap->M_fsoix )) {
			goto newmedia;
		}

		/* if we hit the end, go back to the top, where
		 * we will decide if we should rewind or get new media.
		 */
		if ( Mediap->M_pos == POS_END ) {
			continue;
		}

		/* if the purpose is to search, return this media file
		 */
		if ( purp == PURP_SEARCH ) {
			assert( Mediap->M_pos == POS_ATHDR );
			return RV_OK;
		}

		/* see if this media file is part of the desired dump session
		 */
		partofdumppr = ( bool_t )(uuid_compare( persp->s.dumpid,
						     grhdrp->gh_dumpid) == 0);
		if (!partofdumppr) {
		    char gh_string_uuid[UUID_STR_LEN + 1];
		    char inv_string_uuid[UUID_STR_LEN + 1];

		    uuid_unparse( grhdrp->gh_dumpid, gh_string_uuid);
		    uuid_unparse( persp->s.dumpid, inv_string_uuid);
		    mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
		          "inventory session uuid (%s) does not match "
		          "the media header's session uuid (%s)\n"),
			  inv_string_uuid, gh_string_uuid );
		}

		/* if media file dump id is different from the preceeding
		 * media file, print something useful at TRACE verbosity.
		 */
		if ( uuid_compare( prevmfiledumpid,
				   grhdrp->gh_dumpid) != 0) {

			char string_uuid[UUID_STR_LEN + 1];

			mlog_lock( );
			mlog( MLOG_TRACE | MLOG_NOLOCK | MLOG_MEDIA,
			      "dump session label: \"%s\"\n",
			      grhdrp->gh_dumplabel );

			uuid_unparse( grhdrp->gh_dumpid, string_uuid);
			mlog( MLOG_TRACE | MLOG_NOLOCK | MLOG_MEDIA,
			      "dump session id: %s\n",
			      string_uuid );
			mlog( MLOG_TRACE | MLOG_NOLOCK | MLOG_MEDIA,
			      "stream %u, object %u, file %u\n",
			      drhdrp->dh_driveix,
			      mrhdrp->mh_mediaix,
			      mrhdrp->mh_dumpmediafileix );
			mlog_unlock( );
			uuid_copy(prevmfiledumpid, grhdrp->gh_dumpid);
		}

		/* if this media file is not part of the desired dump session,
		 * and a preceeding media file on this object was part of the
		 * dump, we know we have hit the end of the stream. tell the
		 * persistent inventory.
		 */
		 if ( ! partofdumppr
		      &&
		      Mediap->M_fsfixvalpr
		      &&
		      Mediap->M_lmfix > Mediap->M_fsfix ) {
			pi_hitnextdump( Mediap->M_fssix,
					Mediap->M_fsoix,
					Mediap->M_lmfix );
		}

		/* if this media file is not part of the desired dump session,
		 * we are doing non-dir, and the preceeding media file on this
		 * object was part of the dump, we know we have hit the end of
		 * the stream. check if we are done.
		 */
		 if ( ! partofdumppr
		      &&
		      purp == PURP_NONDIR
		      &&
		      Mediap->M_fsfixvalpr
		      &&
		      Mediap->M_lmfix > Mediap->M_fsfix ) {
			if ( pi_alldone( )) {
				( * dop->do_end_read )( drivep );
				Mediap->M_pos = POS_UNKN;
				fileh = DH_NULL;
				return RV_NOMORE;
			}
		}
			
		/* if this media file is not part of the desired dump session,
		 * and preceeding media files on this object were, decide if
		 * we need to rewind and look at the beginning of the object.
		 */
		if ( ! partofdumppr
		     &&
		     Mediap->M_fsfixvalpr
		     &&
		     Mediap->M_fmfix <= Mediap->M_fsfix ) {
			( * dop->do_end_read )( drivep );
			Mediap->M_pos = POS_UNKN;
			fileh = DH_NULL;
			if ( dcaps & DRIVE_CAP_REWIND ) {
				mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
				  "rewinding\n") );
				( * drivep->d_opsp->do_rewind )( drivep );
				continue;
			} else {
				goto newmedia;
			}
		}

		/* if this media file is not part of the desired dump session,
		 * and the above conditions were not met, then keep looking
		 */
		if ( ! partofdumppr ) {
			( * dop->do_end_read )( drivep );
			Mediap->M_pos = POS_UNKN;
			fileh = DH_NULL;
			continue;
		}

		/* record the index within this media object of the first
		 * media file in the dump stream
		 */
		if ( ! Mediap->M_fsfixvalpr ) {
			Mediap->M_fsfix =
				     mrhdrp->mh_mediafileix
				     -
				     mrhdrp->mh_dumpmediafileix;
			Mediap->M_fsoix = mrhdrp->mh_mediaix;
			Mediap->M_fssix = drhdrp->dh_driveix;
			Mediap->M_fsfixvalpr = BOOL_TRUE;
		}

		/* this media file is part of the dump. add it to the
		 * persistent inventory and get a file handle.
		 */
		fileh = pi_addfile( Mediap,
				    grhdrp,
				    drhdrp,
				    mrhdrp,
				    scrhdrp,
				    drivep );

		if ( fileh == DH_NULL ) {
			return RV_CORE;
		}

		pi_lock( );
		objh = DH2F( fileh )->f_parh;
		DH2O( objh )->o_indriveix = drivep->d_index;
		DH2O( objh )->o_indrivepr = BOOL_TRUE;
		pi_unlock( );

		pi_note_underhead( objh, fileh );

		/* if purp is nondir, we may be done.
		 */
		if ( purp == PURP_NONDIR && pi_alldone( )) {
			( * dop->do_end_read )( drivep );
			Mediap->M_pos = POS_UNKN;
			return RV_NOMORE;
		}

		/* check for a wraparound
		 */
		if ( Mediap->M_flmfixvalpr ) {
			if ( Mediap->M_fsfixvalpr
			     &&
			     Mediap->M_fmfix <= Mediap->M_fsfix
			     &&
			     Mediap->M_lmfix < Mediap->M_fmfix ) {
				( * dop->do_end_read )( drivep );
				Mediap->M_pos = POS_UNKN;
				fileh = DH_NULL;
				goto newmedia;
			}
			if ( Mediap->M_pmfixvalpr
			     &&
			     Mediap->M_pmfix < Mediap->M_fmfix
			     &&
			     Mediap->M_lmfix > Mediap->M_fmfix ) {
				( * dop->do_end_read )( drivep );
				Mediap->M_pos = POS_UNKN;
				fileh = DH_NULL;
				goto newmedia;
			}
		}

		/* if this media file is an inventory or a terminator,
		 * we have hit the end of the stream. don't tell the persistent
		 * inventory; it already knows because of a pi_addfile.
		 * decide if any preceeding media files are useful and if so
		 * go get them. otherwise get another media object.
		 */
		if ( MEDIA_TERMINATOR_CHK( mrhdrp )
		     ||
		     scrhdrp->cih_mediafiletype
		     ==
		     CIH_MEDIAFILETYPE_INVENTORY ) {
			( * dop->do_end_read )( drivep );
			Mediap->M_pos = POS_UNKN;
			fileh = DH_NULL;
			if ( pi_know_no_more_on_object( purp,
							Mediap->M_fssix,
							Mediap->M_fsoix )) {
				goto newmedia;
			}
			if ( Mediap->M_fmfix > Mediap->M_fsfix
			     &&
			     ( dcaps & DRIVE_CAP_REWIND )) {
				pi_note_underhead( objh, DH_NULL );
				mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
					"rewinding\n") );
				( * drivep->d_opsp->do_rewind )( drivep );
				continue;
			}
			goto newmedia;
		}

		/* if the purpose is dir, but this media file is not positioned
		 * at the hdr or has already been tried, get another one.
		 * use the persistent inventory to do this intelligently.
		 */
		if ( purp == PURP_DIR
		     &&
		     ( Mediap->M_pos != POS_ATHDR
		       ||
		       DH2F( fileh )->f_dirtriedpr )) {
			( * dop->do_end_read )( drivep );
			Mediap->M_pos = POS_UNKN;
			fileh = DH_NULL;
			if ( pi_know_no_more_beyond_on_object( purp,
							       Mediap->M_fssix,
							       Mediap->M_fsoix,
						mrhdrp->mh_dumpmediafileix )) {
				if ( pi_know_no_more_on_object( purp,
								Mediap->M_fssix,
							    Mediap->M_fsoix )) {
					goto newmedia;
				}
				if ( Mediap->M_fmfix > Mediap->M_fsfix
				     &&
				     ( dcaps & DRIVE_CAP_REWIND )) {
					pi_note_underhead( objh, DH_NULL );
					mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
						"rewinding\n") );
					( * drivep->d_opsp->do_rewind )(drivep);
					continue;
				}
				goto newmedia;
			} else {
				continue;
			}
		}

		/* if the purpose is dir, give it to the caller
		 */
		if ( purp == PURP_DIR ) {
			assert( Mediap->M_pos == POS_ATHDR );
			if ( filehp ) {
				assert( fileh != DH_NULL );
				*filehp = fileh;
			}
			return RV_OK;
		}

		/* if we made it this far, the purpose is NONDIR and this
		 * is a valid media file from the desired dump session.
		 */

		/* see if this media file contains any inodes not yet restored
		 */
		assert( fileh != DH_NULL );
		pi_lock( );
		assert( DH2F( fileh )->f_valpr );
		begino = DH2F( fileh )->f_curegrp.eg_ino;
		endino = pi_scanfileendino( fileh );
		hassomepr = inomap_rst_needed( begino, endino );

		/* if we have already given up on this media file or
		 * it doesn't contains anything not yet restored,
		 * or it can be skipped, move on. force the done flag on,
		 * so we don't check it again.
		 */
		if ( DH2F( fileh )->f_nondirdonepr
		     ||
		     DH2F( fileh )->f_nondirskippr
		     ||
		     ! hassomepr ) {
			if ( ! DH2F( fileh )->f_nondirskippr ) {
				DH2F( fileh )->f_nondirdonepr = BOOL_TRUE;
			}
			pi_unlock( );
			( * dop->do_end_read )( drivep );
			Mediap->M_pos = POS_UNKN;
			fileh = DH_NULL;
			if ( pi_know_no_more_beyond_on_object( purp,
							       Mediap->M_fssix,
							       Mediap->M_fsoix,
						mrhdrp->mh_dumpmediafileix )) {
				if ( pi_know_no_more_on_object( purp,
								Mediap->M_fssix,
							    Mediap->M_fsoix )) {
					goto newmedia;
				}
				if ( Mediap->M_fmfix > Mediap->M_fsfix
				     &&
				     ( dcaps & DRIVE_CAP_REWIND )) {
					pi_note_underhead( objh, DH_NULL );
					mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
						"rewinding\n") );
					( * drivep->d_opsp->do_rewind )(drivep);
					continue;
				}
				goto newmedia;
			} else {
				continue;
			}
		}

		/* so the purpose is NONDIR and we like this media file.
		 * be sure we are positioned at the beginning of the
		 * non-dir filehdr not yet restored, and supply to caller.
		 */

		/* need to position just after the first
		 * non-dir filehdr_t not yet restored.
		 * may be a problem if we are currently positioned
		 * in the middle of the dir dump and have no
		 * checkpoint to seek to. if at beginning
		 * and no check point, can still get there
		 * by doing dummy read of dirdump.
		 */
		assert( fileh != DH_NULL );
		assert( DH2F( fileh )->f_valpr );
		resumepr = ( ( DH2F( fileh )->f_firstegrp.eg_ino
			       !=
			       DH2F( fileh )->f_curegrp.eg_ino )
			     ||
			     ( DH2F( fileh )->f_firstegrp.eg_off
			       !=
			       DH2F( fileh )->f_curegrp.eg_off ));
		chkpnt = DH2F( fileh )->f_curmark;
		pi_unlock( );
		fhcs = ( scrhdrp->cih_dumpattr
			 &
			 CIH_DUMPATTR_FILEHDR_CHECKSUM )
		       ?
		       BOOL_TRUE
		       :
		       BOOL_FALSE;
		canseeknextpr = dcaps & DRIVE_CAP_NEXTMARK;
		switch( Mediap->M_pos ) {
		case POS_ATHDR:
		case POS_INDIR:
			if ( resumepr ) {
			    mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
				  "seeking past portion of media file "
				  "already restored\n") );
			    rval = ( * dop->do_seek_mark )( drivep,
							    &chkpnt );
			    if ( ! rval ) {
				    rv_t rv;
				    rv = read_filehdr( drivep,
						       fhdrp,
						       fhcs );
				    if ( rv != RV_OK ) {
					    rval = 1;
				    } else {
					    mlog( MLOG_TRACE | MLOG_MEDIA,
						  "positioned at unrestored "
						  "portion of media file\n" );
				    }
			    }
			} else if ( canseeknextpr ) {
			    mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
				  "seeking past media file "
				  "directory dump\n") );
			    rval = ( * dop->do_next_mark )( drivep);
			    if ( ! rval ) {
				    rv_t rv;
				    rv = read_filehdr( drivep,
						       fhdrp,
						       fhcs );
				    if ( rv != RV_OK ) {
					    rval = 1;
				    } else {
					    mlog( MLOG_TRACE | MLOG_MEDIA,
						  "positioned at "
						  "media file "
						  "non-directory dump\n" );
				    }
			    }
			} else if ( Mediap->M_pos == POS_ATHDR ) {
			    mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
				  "seeking past media file "
				  "directory dump\n") );
			    rv = eatdirdump( drivep,
					     fileh,
					     scrhdrp,
					     fhdrp );
			    if ( rv != RV_OK ) {
				    rval = 1;
			    } else {
				    mlog( MLOG_TRACE | MLOG_MEDIA,
					  "positioned at "
					  "media file "
					  "non-directory dump\n" );
			    }
			} else {
			    rval = 1;
			}
			break;
		case POS_ATNONDIR:
			rval = 0;
			break;
		default:
		    assert( 0 );
		    rval = 1;
		    break;
		}

		/* if error encountered during fine positioning,
		 * mark file so we won't try it again
		 */
		if ( rval ) {
			DH2F( fileh )->f_nondirdonepr = BOOL_TRUE;
		} else {
			Mediap->M_pos = POS_ATNONDIR;
		}

		/* if no error during fine positioning, return.
		 */
		if ( ! rval ) {
			if ( filehp ) {
				assert( fileh != DH_NULL );
				*filehp = fileh;
			}
			return RV_OK;
		}

		/* an error occurred during fine positioning. any other useful
		 * media files on this object? if so, continue; if not, get
		 * more media.
		 */
		( * dop->do_end_read )( drivep );
		Mediap->M_pos = POS_UNKN;
		fileh = DH_NULL;
		assert( purp == PURP_NONDIR );
		if ( pi_know_no_more_beyond_on_object( purp,
						       Mediap->M_fssix,
						       Mediap->M_fsoix,
					mrhdrp->mh_dumpmediafileix )) {
			if ( pi_know_no_more_on_object( purp,
							Mediap->M_fssix,
							Mediap->M_fsoix )) {
				goto newmedia;
			}
			if ( Mediap->M_fmfix > Mediap->M_fsfix
			     &&
			     ( dcaps & DRIVE_CAP_REWIND )) {
				pi_note_underhead( objh, DH_NULL );
				mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
					"rewinding\n") );
				( * drivep->d_opsp->do_rewind )(drivep);
				continue;
			}
			goto newmedia;
		} else {
			continue;
		}
		/* fall through */

newmedia:
		/* invalidate prev id, so we log a TRACE msg for first
		 * media file seen on new media
		 */
		uuid_clear(prevmfiledumpid);

		/* if we are searching and some other thread completed
		 * the search, don't pop the media unless it is useless
		 */
		if ( purp == PURP_SEARCH
		     &&
		     Mediap->M_pos != POS_USELESS
		     &&
		     Mediap->M_pos != POS_BLANK
		     &&
		     donesyncp
		     &&
		     *donesyncp == SYNC_DONE ) {
			return RV_DONE;
		}

		/* if media not removable, just return 
		 */
		if ( ( * dop->do_get_device_class )( drivep )
		     ==
		     DEVICE_NONREMOVABLE )
		{
		    /* if no error has already been detected then don't log
		       a failure */
		    if (mlog_get_hint() == RV_NONE)
			mlog_exit_hint(RV_OK);
		    return RV_QUIT;
		}

		/* check for an interrupt
		 */
		if ( cldmgr_stop_requested( )) {
			return RV_INTR;
		}

		/* check if we are done.
		 */
		switch( purp ) {
		case PURP_SEARCH:
			knownholespr = BOOL_TRUE;
			maybeholespr = BOOL_FALSE;
			bagp = 0;
			break;
		case PURP_DIR:
			knownholespr = BOOL_FALSE;
			maybeholespr = BOOL_FALSE;
			bagp = pi_neededobjs_dir_alloc( &knownholespr,
							&maybeholespr );
			break;
		case PURP_NONDIR:
			knownholespr = BOOL_FALSE;
			maybeholespr = BOOL_FALSE;
			bagp = pi_neededobjs_nondir_alloc( &knownholespr,
							   &maybeholespr,
							   BOOL_FALSE,
							   BOOL_FALSE );
			break;
		default:
			assert( 0 );
		}

		if ( ! bagp && ! knownholespr && ! maybeholespr ) {
			/* if PURP_DIR, this may be a problem
			 */
			if ( purp == PURP_NONDIR ) {
				return RV_NOMORE;
			}
		}

		/* eject media if drive not already empty
		 */
		if ( ! emptypr ) {
			int dcaps = drivep->d_capabilities;
			if ( purp == PURP_SEARCH ) {
				if ( Mediap->M_pos == POS_USELESS ) {
					mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
					      "media object not useful\n") );
				} else if ( Mediap->M_pos == POS_BLANK ) {
					mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
					      "media object empty\n") );
				} else {
					mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
					      "all media files examined, "
					      "none selected for restoral\n") );
				}
			}
			if ( dcaps & DRIVE_CAP_EJECT ) {
				( * dop->do_eject_media )( drivep );
			}
		}

		/* tell the persistent inventory this drive is now empty
		 */
		pi_driveempty( drivep->d_index );

		/* invalidate all positional descriptors
		 */
		Mediap->M_pos = POS_UNKN;
		Mediap->M_flmfixvalpr = BOOL_FALSE;
		Mediap->M_pmfixvalpr = BOOL_FALSE;
		Mediap->M_fsfixvalpr = BOOL_FALSE;
		fileh = DH_NULL;


		/* ask for a media change: supply a list of media objects
		 * which may contain useful media files
		 */
		if ( dlog_allowed( )) {
			/* If an alert program has been specified , run it.
			 */
			if (media_change_alert_program != NULL)
				system(media_change_alert_program);

			if ( drivecnt > 1 && ! stdoutpiped ) {
				ix_t thrdix = drivep->d_index;
				if ( bagp ) {
					pi_neededobjs_free( bagp );
					bagp = 0;
				}
				assert( sistr );
				mlog( MLOG_NORMAL | MLOG_NOTE | MLOG_MEDIA, _(
				      "please change media: "
				      "type %s to confirm media change\n"),
				      sistr );
				set_mcflag( thrdix );
				while ( mcflag[ thrdix ] ) {
					sleep( 2 );
					if ( cldmgr_stop_requested( )) {
						clr_mcflag( thrdix );
						return RV_INTR;
					}
					if ( purp == PURP_NONDIR
					     &&
					     pi_alldone( )) {
						clr_mcflag( thrdix );
						return RV_NOMORE;
					}
				}
				ok = BOOL_TRUE;
			} else {
				ok = Media_prompt_change( drivep,
							  purp,
							  bagp,
							  knownholespr,
							  maybeholespr );
			}
		} else {
			ok = BOOL_FALSE;
		}
		if ( bagp ) {
			pi_neededobjs_free( bagp );
			bagp = 0;
		}
		if ( cldmgr_stop_requested( )) {
			return RV_INTR;
		}
		if ( ! ok ) {
			return RV_QUIT;
		}
	}
	/* NOTREACHED */
}

/* figures out and calls if needed do_end_read( ).
 */
static void
Media_end( Media_t *Mediap )
{
	drive_t *drivep = Mediap->M_drivep;
	drive_ops_t *dop = drivep->d_opsp;

	mlog( MLOG_DEBUG | MLOG_MEDIA,
	      "Media_end: pos==%d\n",
	      Mediap->M_pos );

	if ( Mediap->M_pos == POS_ATHDR
	     ||
	     Mediap->M_pos == POS_INDIR
	     ||
	     Mediap->M_pos == POS_ATNONDIR ) {
		( * dop->do_end_read )( drivep );
		Mediap->M_pos = POS_UNKN;
	}
}

/* Persistent inventory operators *******************************************/

/* the persistent inventory is an mmap()ed file containing a hierarchical
 * representation of all the media files generated by a dump session. it
 * is useful for asking questions about how much of the dump remains to
 * be restored.
 *
 * the top of the hierarchy is a linked list of streams. each of these contains
 * a linked list of objects, which in turn each contain a linked list of files.
 * each stream descriptor also has a flag indicating the last object in the
 * stream is represented. each file descriptor contains a flag indicating the
 * last file in that object is represented. the object descriptor also contains
 * two indices; one indicating where in the dump stream the first media file
 * in that object lies, and where in the object the first media file pertaining
 * to the dump stream lies. each file descriptor describes the first extent
 * group in that media file, and the next extent group in the media file to be
 * restored. the file descriptor also contains flags indicating if an attempt
 * (successful or unsuccessful) to use the file for a directory dump, and
 * a similar flag for non-dir.
 *
 * all media files generated during the dump are represented, including
 * terminators and inventories.
 */
static void
pi_lock( void )
{
	qlock_lock( tranp->t_pilockh );
}

static void
pi_unlock( void )
{
	qlock_unlock( tranp->t_pilockh );
}

/* sets check point in media file descriptor
 */
static void
pi_checkpoint( dh_t fileh, drive_mark_t *drivemarkp, xfs_ino_t ino, off64_t off )
{
	pi_lock( );
	DH2F( fileh )->f_curmark = *drivemarkp;
	DH2F( fileh )->f_curegrp.eg_ino = ino;
	DH2F( fileh )->f_curegrp.eg_off = off;
	pi_unlock( );
}

/* lock must be held by caller
 */
static bool_t
pi_allocdesc( dh_t *deschp )
{
	dh_t desch;

	if ( persp->s.descfreeh == DH_NULL ) {
		ix_t stpgcnt = persp->a.stpgcnt;
		ix_t olddescpgcnt = persp->s.descpgcnt;
		ix_t newdescpgcnt = olddescpgcnt + DAU;
		ix_t descppg = pgsz / PERS_DESCSZ;
		ix_t descix;
		/* REFERENCED */
		int rval;

		/* first unmap if any existing descriptors
		 */
		if ( descp ) {
			assert( olddescpgcnt > 0 );
			rval = munmap( ( void * )descp,
				       olddescpgcnt * pgsz );
			assert( ! rval );
			descp = 0;
		} else {
			assert( olddescpgcnt == 0 );
		}

		/* remap with DAU more pages of descriptors
		 */
		assert( stpgcnt <= ( ix_t )INTGENMAX );
		assert( newdescpgcnt > 0 );
		descp = ( pers_desc_t * ) mmap_autogrow( newdescpgcnt * pgsz,
						tranp->t_persfd,
						( off_t )perssz
						+
						( off_t )( stpgcnt * pgsz ));
		if ( descp == ( pers_desc_t * )( -1 )) {
			pi_unlock( );
			mlog( MLOG_NORMAL | MLOG_ERROR | MLOG_MEDIA, _(
			      "could not remap persistent state file "
			      "inv %s: %d (%s)\n"),
			      perspath,
			      errno,
			      strerror( errno ));
			pi_lock( );
			descp = 0;
			return BOOL_FALSE;
		}
		persp->s.descfreeh = ( dh_t )( olddescpgcnt * pgsz + 1 );
		for ( descix = 0, desch = persp->s.descfreeh
		      ;
		      descix < ( DAU * descppg ) - 1
		      ;
		      descix++, desch += PERS_DESCSZ ) {
			DH2D( desch )->d_nexth = desch + PERS_DESCSZ;
		}
		DH2D( desch )->d_nexth = DH_NULL;
		persp->s.descpgcnt = newdescpgcnt;
	}

	desch = persp->s.descfreeh;
	persp->s.descfreeh = DH2D( desch )->d_nexth;
	memset( ( void * )DH2D( desch ), 0, sizeof( pers_desc_t ));
	assert( desch != DH_NULL );
	*deschp = desch;
	return BOOL_TRUE;
}

/* inserts the indexed file into the given stream. ensures that all
 * previous files are represented as well. if dmfix is not valid, only
 * adds objects.
 */
static dh_t
pi_insertfile( ix_t drivecnt,
	       ix_t driveix,
	       ix_t mediaix,
	       bool_t idlabvalpr,
	       uuid_t *mediaidp,
	       label_t medialabel,
	       bool_t previdlabvalpr,
	       uuid_t *prevmediaidp,
	       label_t prevmedialabel,
	       bool_t mfixvalpr,
	       ix_t mediafileix,
	       bool_t dmfixvalpr,
	       ix_t dumpmediafileix,
	       bool_t dfixvalpr,
	       ix_t dumpfileix,
	       bool_t egrpvalpr,
	       xfs_ino_t startino,
	       off64_t startoffset,
	       int flags,
	       bool_t fileszvalpr,
	       off64_t filesz )
{
	ix_t strmix;
	dh_t strmh;
	ix_t objix;
	dh_t objh;
	dh_t prevobjh;
	ix_t fileix;
	dh_t fileh;
	dh_t prevfileh;
	bool_t ok;

	pi_lock( );

	/* first alloc stream descriptors if needed
	 */
	if ( persp->s.strmheadh == DH_NULL ) {
		for ( strmix = 0 ; strmix < drivecnt ; strmix++ ) {
			ok = pi_allocdesc( &strmh );
			if ( ! ok ) {
				pi_unlock( );
				return DH_NULL;
			}
			DH2S( strmh )->s_nexth = persp->s.strmheadh;
			persp->s.strmheadh = strmh;
		}
	}

	/* get handle to this stream
	 */
	for ( strmix = 0,
	      strmh = persp->s.strmheadh
	      ;
	      strmix < driveix
	      ;
	      strmix++,
	      strmh = DH2S( strmh )->s_nexth )
		;
	assert( strmh != DH_NULL );

	/* get handle to this object by walking/constructing this stream's
	 * object list, up to the desired object
	 */
	objh = prevobjh = DH_NULL;
	for ( objix = 0 ; objix <= mediaix ; objix++ ) {
		prevobjh = objh;
		if ( objix == 0 ) {
			objh = DH2S( strmh )->s_cldh;
		} else {
			objh = DH2O( prevobjh )->o_nexth;
		}
		if ( objh == DH_NULL ) {
			ok = pi_allocdesc( &objh );
			if ( ! ok ) {
				pi_unlock( );
				return DH_NULL;
			}
			DH2O( objh )->o_parh = strmh;
			if ( objix == 0 ) {
				DH2S( strmh )->s_cldh = objh;
			} else {
				DH2O( prevobjh )->o_nexth = objh;
			}
		}
	}

	/* update the object fields if not yet valid
	 */
	if ( idlabvalpr
	     &&
	     ! DH2O( objh )->o_idlabvalpr ) {
		uuid_copy(DH2O( objh )->o_id, *mediaidp);
		strncpy( DH2O( objh )->o_lab,
			 medialabel,
			 sizeof( DH2O( objh )->o_lab ));
		DH2O( objh )->o_idlabvalpr = BOOL_TRUE;
	}
	if ( mfixvalpr
	     &&
	     dmfixvalpr
	     &&
	     ! DH2O( objh )->o_fmfmixvalpr ) {
		DH2O( objh )->o_fmfmix = mediafileix - dumpmediafileix;
		DH2O( objh )->o_fmfmixvalpr = BOOL_TRUE;
	}
	if ( dfixvalpr
	     &&
	     dmfixvalpr
	     &&
	     ! DH2O( objh )->o_fmfsixvalpr ) {
		DH2O( objh )->o_fmfsix = dumpfileix - dumpmediafileix;
		DH2O( objh )->o_fmfsixvalpr = BOOL_TRUE;
	}

	/* record the previous object's id and label if not yet valid
	 */
	if ( prevobjh != DH_NULL
	     &&
	     previdlabvalpr
	     &&
	     ! DH2O( prevobjh )->o_idlabvalpr ) {
		uuid_copy(DH2O( prevobjh )->o_id, *prevmediaidp);
		strncpy( DH2O( prevobjh )->o_lab,
			       prevmedialabel,
			       sizeof( DH2O( prevobjh )->o_lab ));
		DH2O( prevobjh )->o_idlabvalpr = BOOL_TRUE;
	}

	/* if the dump file and dump media file indices are valid,
	 * and the previous object has at least one media file with its
	 * dump file index valid, can infer the index of the last media
	 * file on the previous dump object.
	 */
	if ( DH2O( objh )->o_fmfsixvalpr
	     &&
	     prevobjh != DH_NULL
	     &&
	     DH2O( prevobjh )->o_fmfsixvalpr
	     &&
	     ! DH2O( prevobjh )->o_lmfknwnpr ) {
		size_t prevmfcnt;
		assert( DH2O( objh )->o_fmfsix > DH2O( prevobjh )->o_fmfsix );
		prevmfcnt = DH2O( objh )->o_fmfsix - DH2O( prevobjh )->o_fmfsix;
		pi_unlock( );
		assert( mediaix > 0 );
		( void )pi_insertfile( drivecnt,
				       driveix,
				       mediaix - 1,
				       BOOL_FALSE,
				       0,
				       0,
				       BOOL_FALSE,
				       0,
				       0,
				       BOOL_FALSE,
				       0,
				       BOOL_TRUE,
				       prevmfcnt - 1,
				       BOOL_TRUE,
				       DH2O( objh )->o_fmfsix - 1,
				       0,
				       0,
				       0,
				       0,
				       BOOL_FALSE,
				       ( off64_t )0 );
		pi_seeobjstrmend( strmix, mediaix - 1 );
		pi_lock( );
	}

	/* if don't know dump stream media file index, can't add any media files
	 */
	if ( ! dmfixvalpr ) {
		pi_unlock( );
		pi_show( " after pi_insertfile no media file ix" );
		return DH_NULL;
	}

	/* get handle to this file by walking/constructing this object's
	 * file list, up to the desired file
	 */
	fileh = DH_NULL;
	for ( fileix = 0 ; fileix <= dumpmediafileix ; fileix++ ) {
		prevfileh = fileh;
		if ( fileix == 0 ) {
			fileh = DH2O( objh )->o_cldh;
		} else {
			fileh = DH2F( prevfileh )->f_nexth;
		}
		if ( fileh == DH_NULL ) {
			ok = pi_allocdesc( &fileh );
			if ( ! ok ) {
				pi_unlock( );
				return DH_NULL;
			}
			DH2F( fileh )->f_parh = objh;
			if ( fileix == 0 ) {
				DH2O( objh )->o_cldh = fileh;
			} else {
				DH2F( prevfileh )->f_nexth = fileh;
			}
		}
	}

	/* update the media file fields not yet valid
	 */
	if ( egrpvalpr && ! DH2F( fileh )->f_valpr ) {
		assert( ! ( DH2F( fileh )->f_flags & PF_INV ));
		assert( ! ( DH2F( fileh )->f_flags & PF_TERM ));
		DH2F( fileh )->f_firstegrp.eg_ino = startino;
		DH2F( fileh )->f_firstegrp.eg_off = startoffset;
		DH2F( fileh )->f_curegrp = DH2F( fileh )->f_firstegrp;
		DH2F( fileh )->f_valpr = BOOL_TRUE;
	}

	/* set flags
	 */
	DH2F( fileh )->f_flags = flags;

	/* if we know the file size,
	 * update it
	 */
	if ( fileszvalpr ) {
		DH2F( fileh )->f_sz = filesz;
		DH2F( fileh )->f_szvalpr = BOOL_TRUE;
	}

	pi_unlock( );
	pi_show( " after pi_insertfile" );
	return fileh;
}

/* add pers file desc if not already present. will automatically
 * update/alloc pers obj and strm descriptors. If given a session inventory,
 * attempt to incorporate into pi. also, initializes completion stats.
 */
/* ARGSUSED */
static dh_t
pi_addfile( Media_t *Mediap,
	    global_hdr_t *grhdrp,
	    drive_hdr_t *drhdrp,
	    media_hdr_t *mrhdrp,
	    content_inode_hdr_t *scrhdrp,
	    drive_t * drivep )
{
	dh_t fileh;

	if ( ! persp->s.stat_valpr ) {
		persp->s.stat_inocnt = scrhdrp->cih_inomap_nondircnt;
		persp->s.stat_inodone = 0;
		assert( scrhdrp->cih_inomap_datasz <= OFF64MAX );
		persp->s.stat_datacnt = ( off64_t )scrhdrp->cih_inomap_datasz;
		persp->s.stat_datadone = 0;
		persp->s.stat_valpr = BOOL_TRUE;
	}

	/* if we see a terminator, we know we have seen the end of
	 * a stream.
	 */
	if ( MEDIA_TERMINATOR_CHK( mrhdrp )) {
		fileh = pi_insertfile( drhdrp->dh_drivecnt,
				       drhdrp->dh_driveix,
				       mrhdrp->mh_mediaix,
				       BOOL_TRUE,
				       &mrhdrp->mh_mediaid,
				       mrhdrp->mh_medialabel,
				       BOOL_TRUE,
				       &mrhdrp->mh_prevmediaid,
				       mrhdrp->mh_prevmedialabel,
				       BOOL_TRUE,
				       mrhdrp->mh_mediafileix,
				       BOOL_TRUE,
				       mrhdrp->mh_dumpmediafileix,
				       BOOL_TRUE,
				       mrhdrp->mh_dumpfileix,
				       BOOL_FALSE,
				       ( xfs_ino_t )0,
				       ( off64_t )0,
				       PF_TERM,
				       BOOL_FALSE,
				       ( off64_t )0 );
		if ( fileh == DH_NULL ) {
			return DH_NULL;
		}
		pi_seestrmend( drhdrp->dh_driveix );
		pi_seeobjstrmend( drhdrp->dh_driveix, mrhdrp->mh_mediaix );
		return fileh;
	}

	/* data file
	 */
	if ( scrhdrp->cih_mediafiletype == CIH_MEDIAFILETYPE_DATA ) {
		/* tell the inventory about this media file
		 */
		fileh = pi_insertfile( drhdrp->dh_drivecnt,
				       drhdrp->dh_driveix,
				       mrhdrp->mh_mediaix,
				       BOOL_TRUE,
				       &mrhdrp->mh_mediaid,
				       mrhdrp->mh_medialabel,
				       BOOL_TRUE,
				       &mrhdrp->mh_prevmediaid,
				       mrhdrp->mh_prevmedialabel,
				       BOOL_TRUE,
				       mrhdrp->mh_mediafileix,
				       BOOL_TRUE,
				       mrhdrp->mh_dumpmediafileix,
				       BOOL_TRUE,
				       mrhdrp->mh_dumpfileix,
				       BOOL_TRUE,
				       scrhdrp->cih_startpt.sp_ino,
				       scrhdrp->cih_startpt.sp_offset,
				       0,
				       BOOL_FALSE,
				       ( off64_t )0 );
		if ( fileh == DH_NULL ) {
			return DH_NULL;
		}
		assert( drhdrp->dh_drivecnt > 0 );
		if ( drhdrp->dh_driveix < drhdrp->dh_drivecnt - 1 ) {
			/* if this is not in the last stream, we know
			 * there is at least one other media file in
			 * the following stream, and we know its start pt
			 */
			( void )pi_insertfile( drhdrp->dh_drivecnt,
					       drhdrp->dh_driveix + 1,
					       0,
					       BOOL_FALSE,
					       0,
					       0,
					       BOOL_FALSE,
					       0,
					       0,
					       BOOL_FALSE,
					       0,
					       BOOL_TRUE,
					       0,
					       BOOL_FALSE,
					       0,
					       BOOL_TRUE,
					       scrhdrp->cih_endpt.sp_ino,
					       scrhdrp->cih_endpt.sp_offset,
					       0,
					       BOOL_FALSE,
					       ( off64_t )0 );
		}
		if ( ! ( drivep->d_capabilities & DRIVE_CAP_FILES )) {
			/* if drive does not support multiple files,
			 * we know this is end of object and stream
			 */
			pi_seestrmend( drhdrp->dh_driveix );
			pi_seeobjstrmend( drhdrp->dh_driveix, mrhdrp->mh_mediaix );
		}

		return fileh;
	}

	/* inventory file
	 */
	if ( scrhdrp->cih_mediafiletype == CIH_MEDIAFILETYPE_INVENTORY ) {
		fileh = pi_insertfile( drhdrp->dh_drivecnt,
				       drhdrp->dh_driveix,
				       mrhdrp->mh_mediaix,
				       BOOL_TRUE,
				       &mrhdrp->mh_mediaid,
				       mrhdrp->mh_medialabel,
				       BOOL_TRUE,
				       &mrhdrp->mh_prevmediaid,
				       mrhdrp->mh_prevmedialabel,
				       BOOL_TRUE,
				       mrhdrp->mh_mediafileix,
				       BOOL_TRUE,
				       mrhdrp->mh_dumpmediafileix,
				       BOOL_TRUE,
				       mrhdrp->mh_dumpfileix,
				       BOOL_FALSE,
				       ( xfs_ino_t )0,
				       ( off64_t )0,
					PF_INV,
					BOOL_FALSE,
					( off64_t )0 );
		if ( fileh == DH_NULL ) {
			return DH_NULL;
		}
		pi_seestrmend( drhdrp->dh_driveix );
		pi_seeobjstrmend( drhdrp->dh_driveix,
				  mrhdrp->mh_mediaix );
		if ( drhdrp->dh_driveix < drhdrp->dh_drivecnt - 1 ) {
			( void )pi_insertfile( drhdrp->dh_drivecnt,
					       drhdrp->dh_driveix + 1,
					       0,
					       BOOL_FALSE,
					       0,
					       0,
					       BOOL_FALSE,
					       0,
					       0,
					       BOOL_FALSE,
					       0,
					       BOOL_TRUE,
					       0,
					       BOOL_FALSE,
					       0,
					       BOOL_TRUE,
					       scrhdrp->cih_endpt.sp_ino,
					       scrhdrp->cih_endpt.sp_offset,
					       0,
					       BOOL_FALSE,
					       ( off64_t )0 );
		}
		if ( ! persp->s.fullinvpr
		     &&
		     Mediap->M_pos == POS_ATHDR ) {
			size_t bufszincr;
			size_t bufsz;
			size_t buflen;
			char *bufp;
			inv_session_t *sessp;
			invt_sessinfo_t   sessinfo;
			bool_t ok;
			bool_t donepr;

			/* read inventory into buffer
			 */
			bufszincr = IBPGINCR * PGSZ;
				/* use 4096, no need to be correlated
				 * with system page size
				 */
			bufsz = bufszincr;
			buflen = 0;
			bufp = ( char * )malloc( bufsz );

			/* need to read until we hit EOF/EOD. that's the only
			 * way to know how big the inventory is. mark the Media
			 * current media file as no longer at hdr.
			 */
			Mediap->M_pos = POS_ATNONDIR;
			donepr = BOOL_FALSE;
			while ( ! donepr ) {
				int nread;
				drive_ops_t *dop = drivep->d_opsp;
				int rval = 0;
				nread = read_buf( bufp + buflen,
						  bufszincr,
						  ( void * )drivep,
						  ( rfp_t )dop->do_read,
					    ( rrbfp_t )dop->do_return_read_buf,
						  &rval );
				switch( rval ) {
				case 0:
					assert( nread == ( int )bufszincr );
					buflen += ( size_t )nread;
					bufsz += bufszincr;
					bufp = ( char * )realloc(( void * )bufp,
								 bufsz );
					assert( bufp );
					continue;
				case DRIVE_ERROR_EOD:
				case DRIVE_ERROR_EOF:
					buflen += ( size_t )nread;
					donepr = BOOL_TRUE;
					break;
				default:
					free( ( void * )bufp );
					return fileh;
				}
			}

			/* ask inventory to convert buffer into session
			 * desc.
			 */
			sessp = 0;
			if ( ! buflen ) {
				ok = BOOL_FALSE;
			} else {
			    /* extract the session information from the buffer */
			    if ( stobj_unpack_sessinfo( bufp, buflen, &sessinfo )<0 ) {
				ok = BOOL_FALSE;
			    } else {
				stobj_convert_sessinfo(&sessp, &sessinfo);
				ok = BOOL_TRUE;
			    }
			}
			if ( ! ok || ! sessp ) {
				mlog( MLOG_DEBUG | MLOG_WARNING | MLOG_MEDIA, _(
				      "on-media session "
				      "inventory corrupt\n") );
			} else {
				/* if root, update online inventory.
				 */
				if ( ! geteuid( )
				     &&
				     ! tranp->t_noinvupdatepr ) {
					mlog( MLOG_VERBOSE | MLOG_MEDIA, _(
					      "incorporating on-media session "
					      "inventory into online "
					      "inventory\n") );
					inv_put_sessioninfo( &sessinfo ); 
				}

				/* convert into pi format
				 */
				mlog( MLOG_VERBOSE | MLOG_MEDIA,
				      "using on-media session inventory\n" );
				persp->s.fullinvpr = pi_transcribe( sessp );
				inv_free_session( &sessp );
			}
			free( ( void * )bufp );
		}
		return fileh;
	}

	assert( 0 );
	return DH_NULL;
}

/* translate a session inventory into a pi
 */
static bool_t
pi_transcribe( inv_session_t *sessp )
{
	ix_t strmcnt;
	ix_t strmix;

	/* traverse inventory, transcribing into pers inv.
	 */
	strmcnt =  ( size_t )sessp->s_nstreams;
	for ( strmix = 0 ; strmix < strmcnt ; strmix++ ) {
		inv_stream_t *strmp;
		size_t fileix;
		size_t filecnt;
		uuid_t lastobjid;
		label_t lastobjlabel;
		ix_t mediaix;
		size_t dumpmediafileix;

		strmp = &sessp->s_streams[ strmix ];
		filecnt = strmp->st_nmediafiles;
                uuid_clear(lastobjid);
		lastobjlabel[ 0 ] = 0;
		mediaix = 0;
		dumpmediafileix = 0;

		/* insert all media files from this stream. note that
		 * the media object representation is inverted
		 */
		for ( fileix = 0 ; fileix < filecnt ; fileix++ ) {
			inv_mediafile_t *filep;
			bool_t fileszvalpr;

			filep = &strmp->st_mediafiles[ fileix ];
			if ( uuid_compare( filep->m_moid,
					   lastobjid ) != 0) {
				dumpmediafileix = 0;
				if ( fileix != 0 ) {
					pi_seeobjstrmend( strmix, mediaix );
					mediaix++;
				}
			}

			fileszvalpr = BOOL_TRUE;

			( void )pi_insertfile( strmcnt,
					       strmix,
					       mediaix,
					       BOOL_TRUE,
					       &filep->m_moid,
					       filep->m_label,
					       BOOL_TRUE,
					       &lastobjid,
					       lastobjlabel,
					       BOOL_TRUE,
					       filep->m_mfile_index,
					       BOOL_TRUE,
					       dumpmediafileix,
					       BOOL_TRUE,
					       fileix,
					       filep->m_isinvdump
					       ?
					       BOOL_FALSE
					       :
					       BOOL_TRUE,
					       filep->m_startino,
					       filep->m_startino_off,
					       filep->m_isinvdump
					       ?
					       PF_INV
					       :
					       0,
					       fileszvalpr,
					       filep->m_size );
			uuid_copy(lastobjid, filep->m_moid);
			strncpy( lastobjlabel,
				 filep->m_label,
				 sizeof( lastobjlabel ));
			dumpmediafileix++;
		}
		pi_seestrmend( strmix );
		pi_seeobjstrmend( strmix, mediaix );
	}

	return BOOL_TRUE;
}

/* clean up pers. inv: initially no media objects in drives. flags may
 * be set from previously interrupted invocation.
 */
static void
pi_preclean( void )
{
	dh_t strmh;
	dh_t objh;
	dh_t fileh;

	for ( strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL
	      ;
	      strmh = DH2S( strmh )->s_nexth ) {
		for ( objh = DH2S( strmh )->s_cldh
		      ;
		      objh != DH_NULL
		      ;
		      objh = DH2O( objh )->o_nexth ) {
			DH2O( objh )->o_indrivepr = BOOL_FALSE;
			for ( fileh = DH2O( objh )->o_cldh
			      ;
			      fileh != DH_NULL
			      ;
			      fileh = DH2F( fileh )->f_nexth ) {
				DH2F( fileh )->f_underheadpr = BOOL_FALSE;
			}
		}
	}
}

/* tell pi no media objects are in this drive
 */
static void
pi_driveempty( ix_t driveix )
{
	dh_t strmh;
	dh_t objh;
	dh_t fileh;

	pi_lock( );

	for ( strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL
	      ;
	      strmh = DH2S( strmh )->s_nexth ) {
		for ( objh = DH2S( strmh )->s_cldh
		      ;
		      objh != DH_NULL
		      ;
		      objh = DH2O( objh )->o_nexth ) {
			if ( DH2O( objh )->o_indrivepr
			     &&
			     DH2O( objh )->o_indriveix == driveix ) {
				DH2O( objh )->o_indrivepr = BOOL_FALSE;
				for ( fileh = DH2O( objh )->o_cldh
				      ;
				      fileh != DH_NULL
				      ;
				      fileh = DH2F( fileh )->f_nexth ) {
					DH2F( fileh )->f_underheadpr =
								    BOOL_FALSE;
				}
			}
		}
	}

	pi_unlock( );
}

/* tell pi this media object is in the drive
 */
static void
pi_note_indrive( ix_t driveix, uuid_t media_id )
{
	dh_t strmh;
	dh_t objh;

	pi_lock( );

	for ( strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL
	      ;
	      strmh = DH2S( strmh )->s_nexth ) {
		for ( objh = DH2S( strmh )->s_cldh
		      ;
		      objh != DH_NULL
		      ;
		      objh = DH2O( objh )->o_nexth ) {
			if ( DH2O( objh )->o_idlabvalpr
			     &&
			     uuid_compare( DH2O( objh )->o_id, media_id) == 0) {
				DH2O( objh )->o_indrivepr = BOOL_TRUE;
				DH2O( objh )->o_indriveix = driveix;
				goto done;
			}
		}
	}

done:
	pi_unlock( );
}

/* tell pi this media file is under the head of the drive containing the object
 */
static void
pi_note_underhead( dh_t thisobjh, dh_t thisfileh )
{
	dh_t fileh;

	if ( thisobjh == DH_NULL ) {
		return;
	}

	pi_lock( );

	if ( thisfileh != DH_NULL ) {
		DH2F( thisfileh )->f_underheadpr = BOOL_TRUE;
	}

	for ( fileh = DH2O( thisobjh )->o_cldh
	      ;
	      fileh != DH_NULL
	      ;
	      fileh = DH2F( fileh )->f_nexth ) {
		if ( fileh != thisfileh ) {
			DH2F( fileh )->f_underheadpr = BOOL_FALSE;
		}
	}

	pi_unlock( );
}

/* mark the pi stream indicating all objects in that stream are known.
 */
static void
pi_seestrmend( ix_t strmix )
{
	ix_t ix;
	dh_t strmh;

	pi_lock( );

	/* get handle to the indexed stream
	 */
	for ( ix = 0,
	      strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL && ix < strmix
	      ;
	      ix++,
	      strmh = DH2S( strmh )->s_nexth )
		;

	/* if an empty stream (can happen when dump interrupted),
	 * nothing need be done, so return
	 */
	if ( strmh == DH_NULL ) {
		pi_unlock( );
		return;
	}

	/* set stream flag and object and file counts
	 */
	DH2S( strmh )->s_lastobjknwnpr = BOOL_TRUE;

	pi_unlock( );
	pi_show( " after pi_seestrmend" );
}

/* mark pi indicating all media files in object are known
 */
static void
pi_seeobjstrmend( ix_t strmix, ix_t mediaix )
{
	ix_t ix;
	dh_t strmh;
	dh_t objh;

	pi_lock( );

	/* get handle to the indexed stream
	 */
	for ( ix = 0,
	      strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL && ix < strmix
	      ;
	      ix++,
	      strmh = DH2S( strmh )->s_nexth )
		;

	/* if an empty stream (can happen when dump interrupted),
	 * nothing need be done, so return
	 */
	if ( strmh == DH_NULL ) {
		pi_unlock( );
		return;
	}


	/* get handle to indexed object in stream
	 */
	for ( ix = 0,
	      objh = DH2S( strmh )->s_cldh
	      ;
	      objh != DH_NULL && ix < mediaix
	      ;
	      ix++,
	      objh = DH2O( objh )->o_nexth )
		;
	
	/* if an empty object (can happen when dump interrupted),
	 * nothing need be done, so return
	 */
	if ( objh == DH_NULL ) {
		pi_unlock( );
		return;
	}

	
	/* set object flag
	 */
	DH2O( objh )->o_lmfknwnpr = BOOL_TRUE;

	pi_unlock( );
	pi_show( " after pi_seeobjstrmend" );
}

/* scans pi to determine ino of last file wholly or partially contained on
 * this mfile. must err on the high side if partial info.
 * NOTE: assumes caller locks pi!
 */
static xfs_ino_t
pi_scanfileendino( dh_t fileh )
{
	dh_t strmh;
	ix_t mode = 0;

	assert( fileh != DH_NULL );

	/* traverse the pi tree, looking for the next media file after
	 */
	for ( strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL
	      ;
	      strmh = DH2S( strmh )->s_nexth ) {
	    dh_t objh;

	    for ( objh = DH2S( strmh )->s_cldh
		  ;
		  objh != DH_NULL
		  ;
		  objh = DH2O( objh )->o_nexth ) {
		dh_t nexth;

		for ( nexth = DH2O( objh )->o_cldh
		      ;
		      nexth != DH_NULL
		      ;
		      nexth = DH2F( nexth )->f_nexth ) {

		    switch( mode ) {
		    case 0:
			if ( nexth == fileh ) {
				mode = 1;
			}
			break;
		    default:
			if ( DH2F( nexth )->f_valpr ) {
			    xfs_ino_t ino;

			    assert( ! ( DH2F( nexth )->f_flags & PF_INV ));
			    assert( ! ( DH2F( nexth )->f_flags & PF_TERM ));
			    if ( DH2F( nexth )->f_firstegrp.eg_off ) {
				ino =  DH2F( nexth )->f_firstegrp.eg_ino;
				return ino;
			    } else {
				assert( DH2F( nexth )->f_firstegrp.eg_ino > 0 );
				ino =  DH2F( nexth )->f_firstegrp.eg_ino - 1;
				return ino;
			    }
			}
			break;
		    }
		}
	    }
	}
	return INO64MAX;
}

/* used to detemine range of extent groups still to be restored
 * from media file. *--o
 */
static void
pi_bracketneededegrps( dh_t thisfileh, egrp_t *first_egrp, egrp_t *next_egrp )
{
	dh_t strmh;
	bool_t thisfoundpr = BOOL_FALSE;
	dh_t prech = DH_NULL;
	dh_t follh = DH_NULL;


	assert( thisfileh != DH_NULL );

	/* traverse the pi tree, looking for fileh
	 */
	pi_lock( );
	assert( DH2F( thisfileh )->f_valpr );

	for ( strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL
	      ;
	      strmh = DH2S( strmh )->s_nexth ) {
	    dh_t objh;

	    for ( objh = DH2S( strmh )->s_cldh
		  ;
		  objh != DH_NULL
		  ;
		  objh = DH2O( objh )->o_nexth ) {
		dh_t fileh;

		for ( fileh = DH2O( objh )->o_cldh
		      ;
		      fileh != DH_NULL
		      ;
		      fileh = DH2F( fileh )->f_nexth ) {
		    if ( ! thisfoundpr ) {
			if ( fileh == thisfileh ) {
			    thisfoundpr = BOOL_TRUE;
			} else if ( DH2F( fileh )->f_valpr ) {
			    assert( ! ( DH2F( fileh )->f_flags & PF_INV ));
			    assert( ! ( DH2F( fileh )->f_flags & PF_TERM ));
			    prech = fileh;
			}
		    } else if ( DH2F( fileh )->f_valpr ) {
			assert( ! ( DH2F( fileh )->f_flags & PF_INV ));
			assert( ! ( DH2F( fileh )->f_flags & PF_TERM ));
			assert( follh == DH_NULL );
			follh = fileh;
			goto done;
		    }
		}
	    }
	}
done:

	assert( thisfoundpr );

	/* initially the lower bracket is this file descriptor's
	 * current egrp. this catches the case where a previous restore
	 * session was interrupted while restoring this media file.
	 */
	*first_egrp = DH2F( thisfileh )->f_curegrp;

	/* if the closest valid preceeding media file's current egrp is
	 * greater, use it as the lower bracket
	 */
	if ( prech != DH_NULL
	     &&
	     egrpcmp( &DH2F( prech )->f_curegrp, first_egrp ) > 0 ) {
		*first_egrp = DH2F( prech )->f_curegrp;
	}

	/* the upper bracket is initially the end of the world.
	 * if we found a valid following file descriptor describing a
	 * media file which has already been at least restored, use
	 * its first egrp as an upper bracket.
	 */
	next_egrp->eg_ino = INO64MAX;
	next_egrp->eg_off = OFF64MAX;
	if ( follh != DH_NULL
	     &&
	     egrpcmp( &DH2F( follh )->f_curegrp, &DH2F( follh )->f_firstegrp )
	     >
	     0 ) {
		*next_egrp = DH2F( follh )->f_firstegrp;
	}

	pi_unlock( );
}

static void
pi_update_stats( off64_t sz )
{
	pi_lock( );
	assert( persp->s.stat_valpr );
	persp->s.stat_inodone++;
	persp->s.stat_datadone += sz;
	pi_unlock( );
}

/* pi_iterator - each invocation of the iterator advances to the next media file
 * in the dump session, walking the media file hierarchy depth-wise. if
 * an object's file list is exhausted and the first media file in the next
 * object is returned and the exhausted object's last media file has not yet
 * been identified, the return-by-ref flag filemissingpr is set. similarly for
 * streams, objmissingpr.
 * NOTE: does not preset missingpr's to FALSE.
 * NOTE: caller must lock pi.
 */

struct pi_iter {
	bool_t initializedpr;
	dh_t strmh;
	dh_t objh;
	dh_t fileh;
	bool_t donepr;
};

typedef struct pi_iter pi_iter_t;

static pi_iter_t *
pi_iter_alloc( void )
{
	pi_iter_t *iterp;

	iterp = ( pi_iter_t * )calloc( 1, sizeof( pi_iter_t ));
	assert( iterp );
	return iterp;
}

static void
pi_iter_free( pi_iter_t *iterp )
{
	free( ( void * )iterp );
}

static dh_t
pi_iter_nextfileh( pi_iter_t *iterp,
		   bool_t *objmissingprp,
		   bool_t *filemissingprp )
{
	assert( ! iterp->donepr );

	if ( persp->s.strmheadh == DH_NULL ) {
		iterp->donepr = BOOL_TRUE;
		return DH_NULL;
	}

	if ( ! iterp->initializedpr ) {
		assert( persp->s.strmheadh != DH_NULL );
		iterp->strmh = persp->s.strmheadh;
		iterp->objh = DH2S( iterp->strmh )->s_cldh;
		if ( iterp->objh == DH_NULL ) {
			if ( ! DH2S( iterp->strmh )->s_lastobjknwnpr ) {
				*objmissingprp = BOOL_TRUE;
			}
		} else {
			iterp->fileh = DH2O( iterp->objh )->o_cldh;
			if ( iterp->fileh == DH_NULL ) {
				if ( ! DH2O( iterp->objh )->o_lmfknwnpr ) {
					*filemissingprp = BOOL_TRUE;
				}
			}
		}

		while ( iterp->fileh == DH_NULL ) {
			while ( iterp->objh == DH_NULL ) {
				if ( ! DH2S( iterp->strmh )->s_lastobjknwnpr ) {
					*objmissingprp = BOOL_TRUE;
				}
				iterp->strmh = DH2S( iterp->strmh )->s_nexth;
				if ( iterp->strmh == DH_NULL ) {
					iterp->donepr = BOOL_TRUE;
					return DH_NULL;
				}
				iterp->objh = DH2S( iterp->strmh )->s_cldh;
			}
			iterp->fileh = DH2O( iterp->objh )->o_cldh;
			if ( iterp->fileh == DH_NULL ) {
				if ( ! DH2O( iterp->objh )->o_lmfknwnpr ) {
					*filemissingprp = BOOL_TRUE;
				}
				iterp->objh = DH2O( iterp->objh )->o_nexth;
			}
		}
		iterp->initializedpr = BOOL_TRUE;
		return iterp->fileh;
	}

	iterp->fileh = DH2F( iterp->fileh )->f_nexth;
	while ( iterp->fileh == DH_NULL ) {
		if ( ! DH2O( iterp->objh )->o_lmfknwnpr ) {
			*filemissingprp = BOOL_TRUE;
		}
		iterp->objh = DH2O( iterp->objh )->o_nexth;
		while ( iterp->objh == DH_NULL ) {
			if ( ! DH2S( iterp->strmh )->s_lastobjknwnpr ) {
				*objmissingprp = BOOL_TRUE;
			}
			iterp->strmh = DH2S( iterp->strmh )->s_nexth;
			if ( iterp->strmh == DH_NULL ) {
				iterp->donepr = BOOL_TRUE;
				return DH_NULL;
			}
			iterp->objh = DH2S( iterp->strmh )->s_cldh;
		}
		iterp->fileh = DH2O( iterp->objh )->o_cldh;
	}

	return iterp->fileh;
}

/* produces a list of media objects needed. also indicates if we know
 * some unidentified media objects are needed, and if it is possible
 * that we need some unidentifed objects, but don't know for sure.
 * if markskippr is set, set the f_nondirskipr flag if the media file
 * does not contain any nondirs of interest.
 */

struct bagobj {
	bagelem_t bagelem;
	uuid_t id;
	label_t label;
	bool_t indrivepr;
	ix_t indriveix;
};

typedef struct bagobj bagobj_t;

static bag_t *
pi_neededobjs_nondir_alloc( bool_t *knownholesprp,
			    bool_t *maybeholesprp,
			    bool_t showobjindrivepr,
			    bool_t markskippr )
{
	bag_t *bagp;
	pi_iter_t *headiterp;
	pi_iter_t *tailiterp;
	dh_t headh;
	dh_t tailh;
	egrp_t tailegrp;
	bool_t knownobjmissingpr;
	bool_t maybeobjmissingpr;
	bool_t maybefilemissingpr;
	dh_t lastobjaddedh;
	int objlistlen;

	/* no point in proceeding if pi not begun
	 */
	if ( persp->s.strmheadh == DH_NULL ) {
		*knownholesprp = BOOL_TRUE;
		*maybeholesprp = BOOL_FALSE;
		return 0;
	}

	/* to hold a list of media object handles: caller must free
	 * using pi_neededobjs_free( ).
	 */
	bagp = bag_alloc( );

	/* allocate two iterators to scan pi
	 */
	tailiterp = pi_iter_alloc( );
	headiterp = pi_iter_alloc( );

	/* set the handle to the last file added to the list to NULL.
	 * this will be updated each time we add an object to the list,
	 * preventing the same object from being added more than once.
	 * this works because the media files for a given object will
	 * always appear contiguous and just once in a pi iteration.
	 */
	lastobjaddedh = DH_NULL;
	objlistlen = 0;

	/* these will be set TRUE if the tail iterator ever indicates
	 * we crossed an object or stream boundary and did not see a
	 * valid last file  or last object respectively. can accumulate
	 * the booleans, since iterator never sets FALSE, just TRUE.
	 */
	maybeobjmissingpr = BOOL_FALSE;
	maybefilemissingpr = BOOL_FALSE;

	/* this will be set TRUE if we see a needed media file but the
	 * object containing the media file has not been IDed.
	 */
	knownobjmissingpr = BOOL_FALSE;
	
	tailegrp.eg_ino = 0;
	tailegrp.eg_off = 0;

	tailh = DH_NULL;

	/* lock up the inventory during the scan
	 */
	pi_lock( );

	do {
		egrp_t headegrp;
		bool_t foundgappr;

		/* advance the head until we see the next media file which has
		 * a valid egrp, or until we run out of media files.
		 */
		do {
			bool_t dummyobjmissingpr;
			bool_t dummyfilemissingpr;
			headh = pi_iter_nextfileh( headiterp,
						   &dummyobjmissingpr,
						   &dummyfilemissingpr );
		} while ( headh != DH_NULL && ! DH2F( headh )->f_valpr );
		if ( headh == DH_NULL ) {
			headegrp.eg_ino = INO64MAX;
			headegrp.eg_off = OFF64MAX;
		} else {
			assert( ! ( DH2F( headh )->f_flags & PF_INV ));
			assert( ! ( DH2F( headh )->f_flags & PF_TERM ));
			headegrp = DH2F( headh )->f_firstegrp;
		}

		/* see if the range of egrps from head up to but not including
		 * tail needed according to ino map
		 */
		if ( gapneeded( &tailegrp, &headegrp )) {
			foundgappr = BOOL_TRUE;
		} else {
			foundgappr = BOOL_FALSE;
		}

		/* now bring tail up to head, adding objects and setting flags
		 * along the way. note special handling of NULL tailh. possible
		 * only first time through: ignore. also, ignore inv and term.
		 */
		do {
		    /* if requested, mark media files not needed
		     */
		    if ( markskippr
			 &&
			 ! foundgappr
			 &&
			 tailh != DH_NULL
			 &&
			 ! ( DH2F( tailh )->f_flags & PF_INV )
			 &&
			 ! ( DH2F( tailh )->f_flags & PF_TERM )
			 &&
			 ! DH2F( tailh )->f_nondirskippr ) {
			    DH2F( tailh )->f_nondirskippr = BOOL_TRUE;
		    }

		    /* build up list of needed objects
		     */
		    if ( foundgappr
			 &&
			 tailh != DH_NULL
			 &&
			 ! ( DH2F( tailh )->f_flags & PF_INV )
			 &&
			 ! ( DH2F( tailh )->f_flags & PF_TERM )
			 &&
			 ! DH2F( tailh )->f_nondirdonepr
			 &&
			 ! DH2F( tailh )->f_nondirskippr ) {

			    dh_t objh = DH2F( tailh )->f_parh;

			    if ( ! DH2O( objh )->o_indrivepr
				 ||
				 showobjindrivepr ) {
				if ( DH2O( objh )->o_idlabvalpr ) {
					if ( objh != lastobjaddedh ) {
					    addobj( bagp,
						    &DH2O( objh )->o_id,
						    DH2O( objh )->o_lab,
						    DH2O( objh )->o_indrivepr,
						    DH2O( objh )->o_indriveix );
					    lastobjaddedh = objh;
					    objlistlen++;
					}
				} else {
					knownobjmissingpr = BOOL_TRUE;
				}
			    }
		    }

		    /* pull the tail up to the next media file
		     */
		    tailh = pi_iter_nextfileh( tailiterp,
					       &maybeobjmissingpr,
					       &maybefilemissingpr );
		} while ( tailh != headh );

		tailegrp = headegrp;

	} while ( headh != DH_NULL );

	pi_unlock( );

	/* free the iterators
	 */
	pi_iter_free( tailiterp );
	pi_iter_free( headiterp );

	/* free the bag and return NULL if object list empty
	 */
	if ( objlistlen == 0 ) {
		bag_free( bagp );
		bagp = 0;
	}

	*maybeholesprp = ( maybeobjmissingpr || maybefilemissingpr );
	*knownholesprp = knownobjmissingpr;
	return bagp;
}

static bag_t *
pi_neededobjs_dir_alloc( bool_t *knownholesprp, bool_t *maybeholesprp )
{
	bag_t *bagp;
	dh_t fileh;
	pi_iter_t *iterp;
	bool_t knownobjmissingpr;
	bool_t maybeobjmissingpr;
	bool_t maybefilemissingpr;
	dh_t lastobjaddedh;
	int objlistlen;

	bagp = bag_alloc( );
	iterp = pi_iter_alloc( );

	knownobjmissingpr = BOOL_FALSE;
	maybeobjmissingpr = BOOL_FALSE;
	maybefilemissingpr = BOOL_FALSE;
	lastobjaddedh = DH_NULL;
	objlistlen = 0;

	pi_lock( );

	while ( ( fileh = pi_iter_nextfileh( iterp,
					     &maybeobjmissingpr,
					     &maybefilemissingpr ))
		!= DH_NULL ) {
		if ( ! DH2F( fileh )->f_dirtriedpr ) {
			dh_t objh = DH2F( fileh )->f_parh;
			if ( ! DH2O( objh )->o_indrivepr ) {
				if ( DH2O( objh )->o_idlabvalpr ) {
					if ( objh != lastobjaddedh ) {
						addobj( bagp,
							&DH2O( objh )->o_id,
							DH2O( objh )->o_lab,
						    DH2O( objh )->o_indrivepr,
						    DH2O( objh )->o_indriveix );
						lastobjaddedh = objh;
						objlistlen++;
					}
				} else {
					knownobjmissingpr = BOOL_TRUE;
				}
			}
		}
	}

	pi_unlock( );

	pi_iter_free( iterp );

	if ( objlistlen == 0 ) {
		bag_free( bagp );
		bagp = 0;
	}

	*maybeholesprp = ( maybeobjmissingpr || maybefilemissingpr );
	*knownholesprp = knownobjmissingpr;
	return bagp;
}

static void
pi_neededobjs_free( bag_t *bagp )
{
	bagiter_t bagiter;
	bagobj_t *bagobjp;
	bagelem_t *bagelemp;
	size64_t dummykey;
	void *dummypayloadp;

	assert( bagp );

	bagiter_init( bagp, &bagiter );

	bagobjp = 0;
	while (( bagelemp = bagiter_next( &bagiter, ( void ** )&bagobjp ) )) {
		bag_remove( bagp, bagelemp, &dummykey, &dummypayloadp );
		assert( bagobjp );
		assert( bagobjp == ( bagobj_t * )dummypayloadp );
		free( ( void * )bagobjp );
		bagobjp = 0;
	}

	bag_free( bagp );
}

/* a macro predicate to indicate if we know we are done. if we are not
 * done or don't know, returns FALSE.
 */
static bool_t
pi_alldone( void )
{
	bag_t *bagp;
	bool_t knownholespr;
	bool_t maybeholespr;
	size_t cnt;

	knownholespr = BOOL_FALSE;
	maybeholespr = BOOL_FALSE;
	bagp = pi_neededobjs_nondir_alloc( &knownholespr,
					   &maybeholespr,
					   BOOL_TRUE,
					   BOOL_FALSE );
	if ( bagp ) {
		cnt = cntobj( bagp );
		pi_neededobjs_free( bagp );
	} else {
		cnt = 0;
	}

	if ( cnt || knownholespr || maybeholespr ) {
		return BOOL_FALSE;
	} else {
		return BOOL_TRUE;
	}
}

/* tells the persistent inventory we hit end-of-data while examining the
 * object specified by the index param. this tells us we've seen the end
 * of the stream as well as the end of the object.
 */
static void
pi_hiteod( ix_t strmix, ix_t objix )
{
	ix_t ix;
	dh_t strmh;
	dh_t objh;
	size_t objcnt;
	ix_t lastobjix;

	pi_lock( );

	/* get handle to the indexed stream
	 */
	for ( ix = 0,
	      strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL && ix < strmix
	      ;
	      ix++,
	      strmh = DH2S( strmh )->s_nexth )
		;
	assert( strmh != DH_NULL );

	/* get index to last object in stream
	 */
	for ( objcnt = 0, objh = DH2S( strmh )->s_cldh
	      ;
	      objh != DH_NULL
	      ;
	      objh = DH2O( objh )->o_nexth, objcnt++ )
		;
	assert( objcnt != 0 );
	lastobjix = objcnt - 1;
	
	pi_unlock( );

	/* can't possibly happen, but check for case where pi indicates
	 * other media objects beyond this one.
	 */
	if ( objix != lastobjix ) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_MEDIA, _(
		      "hit EOD at stream %u object %u, "
		      "yet inventory indicates last object index is %u\n"),
		      strmix,
		      objix,
		      lastobjix );
	} else {
		pi_seestrmend( strmix );
	}

	pi_seeobjstrmend( strmix, lastobjix );
}

/* tells the persistent inventory we hit end-of-media while examining the
 * object specified by the index param. this tells us we've seen the end
 * of the object.
 */
static void
pi_hiteom( ix_t strmix, ix_t objix )
{
	pi_seeobjstrmend( strmix, objix );
}

static void
pi_hitnextdump( ix_t strmix, ix_t objix, ix_t lastfileix )
{
	ix_t ix;
	dh_t strmh;
	dh_t objh;
	size_t objcnt;
	ix_t lastobjix;

	pi_lock( );

	/* get handle to the indexed stream
	 */
	for ( ix = 0,
	      strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL && ix < strmix
	      ;
	      ix++,
	      strmh = DH2S( strmh )->s_nexth )
		;
	assert( strmh != DH_NULL );

	/* get index to last object in stream
	 */
	for ( objcnt = 0, objh = DH2S( strmh )->s_cldh
	      ;
	      objh != DH_NULL
	      ;
	      objh = DH2O( objh )->o_nexth, objcnt++ )
		;
	assert( objcnt != 0 );
	lastobjix = objcnt - 1;
	
	pi_unlock( );

	/* can't possibly happen, but check for case where pi indicates
	 * other media objects beyond this one.
	 */
	if ( objix != lastobjix ) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_MEDIA, _(
		      "hit next dump at stream %u object %u file %u, "
		      "yet inventory indicates last object index is %u\n"),
		      strmix,
		      objix,
		      lastfileix,
		      lastobjix );
	} else {
		pi_seestrmend( strmix );
	}

	pi_seeobjstrmend( strmix, lastobjix );
}

/* returns TRUE if pi is certain no more useful media files remaining
 * on object. if any doubt, such as not knowing the last media file on
 * the object, returns FALSE.
 */
static bool_t
pi_know_no_more_on_object( purp_t purp, ix_t strmix, ix_t objix )
{
	ix_t ix;
	dh_t strmh;
	dh_t objh;
	dh_t fileh;

	assert( purp == PURP_DIR || purp == PURP_NONDIR );

	pi_lock( );

	/* get handle to the indexed stream
	 */
	for ( ix = 0,
	      strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL && ix < strmix
	      ;
	      ix++,
	      strmh = DH2S( strmh )->s_nexth )
		;
	assert( strmh != DH_NULL );

	/* get handle to indexed object
	 */
	for ( ix = 0, objh = DH2S( strmh )->s_cldh
	      ;
	      objh != DH_NULL && ix < objix
	      ;
	      ix++,
	      objh = DH2O( objh )->o_nexth )
		;
	assert( objh != DH_NULL );
	
	/* if don't know last media file on object, return FALSE
	 */
	if ( ! DH2O( objh )->o_lmfknwnpr ) {
		pi_unlock( );
		return BOOL_FALSE;
	}

	/* check all media files on object. if any are not marked done,
	 * return FALSE.
	 */
	for ( fileh = DH2O( objh )->o_cldh
	      ;
	      fileh != DH_NULL
	      ;
	      fileh = DH2F( fileh )->f_nexth ) {
		if ( DH2F( fileh )->f_flags & PF_INV ) {
			continue;
		}
		if ( DH2F( fileh )->f_flags & PF_TERM ) {
			continue;
		}
		if ( purp == PURP_DIR ) {
			if ( ! DH2F( fileh )->f_dirtriedpr ) {
				pi_unlock( );
				return BOOL_FALSE;
			}
		} else {
			if ( ! DH2F( fileh )->f_nondirskippr
			     &&
			     ! DH2F( fileh )->f_nondirdonepr ) {
				pi_unlock( );
				return BOOL_FALSE;
			}
		}
	}
	
	pi_unlock( );
	return BOOL_TRUE;
}

static bool_t
pi_know_no_more_beyond_on_object( purp_t purp,
				  ix_t strmix,
				  ix_t objix,
				  ix_t fileix )
{
	ix_t ix;
	dh_t strmh;
	dh_t objh;
	dh_t fileh;

	assert( purp == PURP_DIR || purp == PURP_NONDIR );

	pi_lock( );

	/* get handle to the indexed stream
	 */
	for ( ix = 0,
	      strmh = persp->s.strmheadh
	      ;
	      strmh != DH_NULL && ix < strmix
	      ;
	      ix++,
	      strmh = DH2S( strmh )->s_nexth )
		;
	assert( strmh != DH_NULL );

	/* get handle to indexed object
	 */
	for ( ix = 0,
	      objh = DH2S( strmh )->s_cldh
	      ;
	      objh != DH_NULL && ix < objix
	      ;
	      ix++,
	      objh = DH2O( objh )->o_nexth )
		;
	assert( objh != DH_NULL );
	
	/* if don't know last media file on object, return FALSE
	 */
	if ( ! DH2O( objh )->o_lmfknwnpr ) {
		pi_unlock( );
		return BOOL_FALSE;
	}

	/* check all files on object after indexed file. if any are not marked
	 * done, return FALSE. skip inventory and terminator files.
	 */
	for ( ix = 0,
	      fileh = DH2O( objh )->o_cldh
	      ;
	      fileh != DH_NULL
	      ;
	      ix++,
	      fileh = DH2F( fileh )->f_nexth ) {
		if ( ix <= fileix ) {
			continue;
		}
		if ( DH2F( fileh )->f_flags & PF_INV ) {
			continue;
		}
		if ( DH2F( fileh )->f_flags & PF_TERM ) {
			continue;
		}
		if ( purp == PURP_DIR ) {
			if ( ! DH2F( fileh )->f_dirtriedpr ) {
				pi_unlock( );
				return BOOL_FALSE;
			}
		} else {
			if ( ! DH2F( fileh )->f_nondirdonepr
			     &&
			     ! DH2F( fileh )->f_nondirskippr ) {
				pi_unlock( );
				return BOOL_FALSE;
			}
		}
	}
	
	pi_unlock( );
	return BOOL_TRUE;
}

/* indicates if the given extent group range is called for by the
 * ino map. *---o (endpoint not inclusive)
 */
static bool_t
gapneeded( egrp_t *firstegrpp, egrp_t *lastegrpp )
{
	xfs_ino_t endino;

	if ( firstegrpp->eg_ino > lastegrpp->eg_ino ) {
		return BOOL_FALSE;
	}

	if ( firstegrpp->eg_ino == lastegrpp->eg_ino
	     &&
	     firstegrpp->eg_off > lastegrpp->eg_off ) {
		return BOOL_FALSE;
	}

	if ( lastegrpp->eg_off > 0 || lastegrpp->eg_ino == 0 ) {
		endino = lastegrpp->eg_ino;
	} else {
		endino = lastegrpp->eg_ino - 1;
	}

	if ( ! inomap_rst_needed( firstegrpp->eg_ino, endino )) {
		return BOOL_FALSE;
	}

	return BOOL_TRUE;
}

static void
addobj( bag_t *bagp,
	uuid_t *idp,
	label_t label,
	bool_t indrivepr,
	ix_t indriveix )
{
	bagobj_t *bagobjp;

	bagobjp = ( bagobj_t * )calloc( 1, sizeof( bagobj_t ));
	assert( bagobjp );
	uuid_copy(bagobjp->id, *idp);
	strncpy( bagobjp->label,
		 label,
		 sizeof( bagobjp->label ));
	bagobjp->indrivepr = indrivepr;
	bagobjp->indriveix = indriveix;
	bag_insert( bagp,
		    &bagobjp->bagelem,
		    ( size64_t )0,
		    ( void * )bagobjp );
}

static size_t
cntobj( bag_t *bagp )
{
	bagiter_t bagiter;
	bagobj_t *bagobjp;
	size_t cnt;

	assert( bagp );

	bagiter_init( bagp, &bagiter );
	cnt = 0;
	bagobjp = 0; /* keep lint happy */
	while ( bagiter_next( &bagiter, ( void ** )&bagobjp )) {
		cnt++;
		bagobjp = 0; /* keep lint happy */
	}

	return cnt;
}


/* misc. static functions ***************************************************/

/* queries inventory for the base of the given session. if the given session
 * was a resumed dump, then must be last dump of same level. otherwise,
 * must be last dump of a lesser level
 */
static bool_t
askinvforbaseof( uuid_t baseid, inv_session_t *sessp )
{
	ix_t level;
	bool_t resumedpr;
	inv_idbtoken_t invtok;
	inv_session_t *basesessp;
	bool_t ok;

	level = ( ix_t )sessp->s_level;
	resumedpr = sessp->s_isresumed;

	/* don't look for base if level 0 and not resumed
	 */
	if ( level == 0 && ! resumedpr ) {
		return BOOL_TRUE;
	}

	/* open the inventory for this file system
	 */
	invtok = inv_open( INV_BY_UUID,
			   INV_SEARCH_ONLY,
			   ( void * )&sessp->s_fsid );
	if ( invtok == INV_TOKEN_NULL ) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_MEDIA, _(
		      "unable to open inventory to validate dump\n") );
		return BOOL_FALSE;
	}

	/* get the base session
	 */
	if ( resumedpr ) {
		ok = inv_lastsession_level_equalto(&sessp->s_fsid,
						    invtok,
						    (u_char_t)level,
						    &basesessp);
	} else {
		ok = inv_lastsession_level_lessthan(&sessp->s_fsid,
						     invtok,
						     (u_char_t)level,
						     &basesessp);
	}
	if ( ! ok ) {
		mlog( MLOG_NORMAL | MLOG_WARNING | MLOG_MEDIA, _(
		      "unable to find base dump in inventory "
		      "to validate dump\n") );
		return BOOL_FALSE;
	}

	/* close the inventory
	 */
	ok = inv_close( invtok );
	assert( ok );

	/* return id of base session
	 */
        uuid_copy(baseid, basesessp->s_sesid);

	/* free the base session descriptor
	 */
	inv_free_session( &basesessp );

	return BOOL_TRUE;
}

static bool_t
dumpcompat( bool_t resumepr, ix_t level, uuid_t baseid, bool_t logpr )
{
	if ( persp->a.cumpr ) {
		if ( persp->a.dumpcnt == 0 ) {
			if ( resumepr ) {
				if ( logpr ) {
				    mlog( MLOG_NORMAL | MLOG_ERROR, _(
					  "cumulative restores must begin with "
					  "an initial (not resumed) "
					  "level 0 dump\n") );
				}
				return BOOL_FALSE;
			}
			if ( level > 0 ) {
				if ( logpr ) {
				    mlog( MLOG_NORMAL | MLOG_ERROR, _(
					  "cumulative restores must begin with "
					  "a level 0 dump\n") );
				}
				return BOOL_FALSE;
			}
		} else {
			if ( resumepr ) {
				if ( uuid_compare( persp->a.lastdumpid,
						   baseid) != 0) {
					if ( logpr ) {
					    mlog( MLOG_NORMAL | MLOG_ERROR, _(
						  "selected resumed dump "
						  "not a resumption of "
						  "previously applied dump\n"));
					}
					return BOOL_FALSE;
				}
			} else {
				if ( uuid_compare( persp->a.lastdumpid,
						   baseid) != 0) {
					if ( logpr ) {
					    mlog( MLOG_NORMAL | MLOG_ERROR, _(
						  "selected dump not based on "
						  "previously applied dump\n"));
					}
					return BOOL_FALSE;
				}
			}
		}
	}

	return BOOL_TRUE;
}

/* prompts for a new media object. supplies list of media objects still
 * needed, and indicates if there are or may be unidentified media objects
 * still needed/available
 */
static bool_t
Media_prompt_change( drive_t *drivep,
		     purp_t purp,
		     bag_t *bagp,
		     bool_t knownholespr,
		     bool_t maybeholespr )
{
	fold_t fold;
	char question[ 100 ];
	char *preamblestr[ PREAMBLEMAX ];
	size_t preamblecnt;
	char *querystr[ QUERYMAX ];
	size_t querycnt;
	char *choicestr[ CHOICEMAX ];
	size_t choicecnt;
	char *ackstr[ ACKMAX ];
	size_t ackcnt;
	char *postamblestr[ POSTAMBLEMAX ];
	size_t postamblecnt;
	ix_t doix;
	ix_t dontix;
	ix_t invstatix;
	ix_t neededix;
	ix_t responseix;
	ix_t sigintix;

retry:
	fold_init( fold, _("change media dialog"), '=' );
	preamblecnt = 0;
	preamblestr[ preamblecnt++ ] = "\n";
	preamblestr[ preamblecnt++ ] = fold;
	preamblestr[ preamblecnt++ ] = "\n\n";
	assert( preamblecnt <= PREAMBLEMAX );
	dlog_begin( preamblestr, preamblecnt );

	/* query: ask if media changed or declined
	 */
	if ( drivecnt > 1 ) {
		sprintf( question, _(
			 "please change media in "
			 "drive %u\n"),
			 (unsigned int)drivep->d_index );
	} else {
		sprintf( question, _(
			 "please change media in "
			 "drive\n") );
	}
	querycnt = 0;
	querystr[ querycnt++ ] = question;
	assert( querycnt <= QUERYMAX );
	choicecnt = 0;
	dontix = choicecnt;
	choicestr[ choicecnt++ ] = _("media change declined");
	if ( purp != PURP_SEARCH ) {
		invstatix = choicecnt;
		choicestr[ choicecnt++ ] = _("display media inventory status");
		neededix = choicecnt;
		choicestr[ choicecnt++ ] = _("list needed media objects");
	} else {
		invstatix = IXMAX;
		neededix = IXMAX;
	}
	doix = choicecnt;
	choicestr[ choicecnt++ ] = _("media changed");
	assert( choicecnt <= CHOICEMAX );
	sigintix = IXMAX - 1;

	responseix = dlog_multi_query( querystr,
				       querycnt,
				       choicestr,
				       choicecnt,
				       0,           /* hilitestr */
				       IXMAX,       /* hiliteix */
				       0,           /* defaultstr */
				       doix,        /* defaultix */
				       DLOG_TIMEOUT_MEDIA,
				       dontix,		/* timeout ix */
				       sigintix,	/* sigint ix */
				       dontix,		/* sighup ix */
				       dontix );	/* sigquit ix */
	ackcnt = 0;
	if ( responseix == doix ) {
		ackstr[ ackcnt++ ] = _("examining new media\n");
	} else if ( responseix == dontix ) {
		ackstr[ ackcnt++ ] = _("media change aborted\n");
	} else if ( responseix == invstatix ) {
		ackstr[ ackcnt++ ] = "\n";
		assert( ackcnt <= ACKMAX );
		dlog_multi_ack( ackstr,
				ackcnt );
		pi_show_nomloglock( );
		postamblecnt = 0;
		fold_init( fold, _("end dialog"), '-' );
		postamblestr[ postamblecnt++ ] = "\n";
		postamblestr[ postamblecnt++ ] = fold;
		postamblestr[ postamblecnt++ ] = "\n\n";
		assert( postamblecnt <= POSTAMBLEMAX );
		dlog_end( postamblestr,
			  postamblecnt );
		goto retry;
	} else if ( responseix == neededix ) {
		ackstr[ ackcnt++ ] = "\n";
		assert( ackcnt <= ACKMAX );
		dlog_multi_ack( ackstr,
				ackcnt );
		display_needed_objects( purp,
					bagp,
					knownholespr,
					maybeholespr );
		postamblecnt = 0;
		fold_init( fold, "end dialog", '-' );
		postamblestr[ postamblecnt++ ] = "\n";
		postamblestr[ postamblecnt++ ] = fold;
		postamblestr[ postamblecnt++ ] = "\n\n";
		assert( postamblecnt <= POSTAMBLEMAX );
		dlog_end( postamblestr,
			  postamblecnt );
		goto retry;
	} else {
		assert( responseix == sigintix );
		ackstr[ ackcnt++ ] = _("keyboard interrupt\n");
	}

	assert( ackcnt <= ACKMAX );
	dlog_multi_ack( ackstr,
			ackcnt );

	postamblecnt = 0;
	fold_init( fold, _("end dialog"), '-' );
	postamblestr[ postamblecnt++ ] = "\n";
	postamblestr[ postamblecnt++ ] = fold;
	postamblestr[ postamblecnt++ ] = "\n\n";
	assert( postamblecnt <= POSTAMBLEMAX );
	dlog_end( postamblestr,
		  postamblecnt );

	if ( responseix == sigintix ) {
		if ( cldmgr_stop_requested( )) {
			return BOOL_FALSE;
		}
		sleep( 1 ); /* to allow main thread to begin dialog */
		mlog( MLOG_NORMAL | MLOG_BARE,
		      "" ); /* to block until main thread dialog complete */
		sleep( 1 ); /* to allow main thread to request children die */
		if ( cldmgr_stop_requested( )) {
			return BOOL_FALSE;
		}
		mlog( MLOG_DEBUG,
		      "retrying media change dialog\n" );
		goto retry;
	}

	return responseix == doix;
}

/* prompts the operator, asking if the current media file header describes
 * the dump to be restored
 */
static bool_t
promptdumpmatch( ix_t thrdix,
		 global_hdr_t *grhdrp,
		 media_hdr_t *mrhdrp,
		 content_hdr_t *crhdrp,
		 content_inode_hdr_t *scrhdrp )
{
	fold_t fold;
	char introstring[ 80 ];
	char *preamblestr[ PREAMBLEMAX ];
	size_t preamblecnt;
	char *querystr[ QUERYMAX ];
	size_t querycnt;
	char *choicestr[ CHOICEMAX ];
	size_t choicecnt;
	char *ackstr[ ACKMAX ];
	size_t ackcnt;
	char *postamblestr[ POSTAMBLEMAX ];
	size_t postamblecnt;
	ix_t doix;
	ix_t dontix;
	ix_t responseix;
	ix_t sigintix;

retry:
	preamblecnt = 0;
	fold_init( fold, _("dump selection dialog"), '=' );
	preamblestr[ preamblecnt++ ] = "\n";
	preamblestr[ preamblecnt++ ] = fold;
	preamblestr[ preamblecnt++ ] = "\n\n";
	assert( preamblecnt <= PREAMBLEMAX );
	dlog_begin( preamblestr, preamblecnt );

	/* display vital stats and ask if this one should be restored
	 */
	if ( drivecnt > 0 ) {
		sprintf( introstring, _(
			 "the following dump has been found"
			 " on drive %u"
			 "\n\n"),
			 (unsigned int)thrdix );
	} else {
		sprintf( introstring, _(
			 "the following dump has been found"
			 "\n\n") );
	}
	assert( strlen( introstring ) < sizeof( introstring ));
	display_dump_label( BOOL_FALSE,
			    MLOG_NORMAL | MLOG_BARE,
			    introstring,
			    grhdrp,
			    mrhdrp,
			    crhdrp,
			    scrhdrp );

	querycnt = 0;
	if ( tranp->t_toconlypr ) {
		querystr[ querycnt++ ] = _("\nexamine this dump?\n");
	} else {
		querystr[ querycnt++ ] = (persp->a.interpr) ?
			_("\ninteractively restore from this dump?\n")
				: _("\nrestore this dump?\n");
	}
	assert( querycnt <= QUERYMAX );
	choicecnt = 0;
	dontix = choicecnt;
	choicestr[ choicecnt++ ] = _("skip");
	doix = choicecnt;
	choicestr[ choicecnt++ ] = (persp->a.interpr) ?
				_("interactively restore\n") : _("restore\n");
	assert( choicecnt <= CHOICEMAX );
	sigintix = IXMAX - 1;

	responseix = dlog_multi_query( querystr,
				       querycnt,
				       choicestr,
				       choicecnt,
				       0,           /* hilitestr */
				       IXMAX,       /* hiliteix */
				       0,           /* defaultstr */
				       doix,        /* defaultix */
				       0,
				       IXMAX,		/* timeout ix */
				       sigintix,	/* sigint ix */
				       dontix,		/* sighup ix */
				       dontix );	/* sigquit ix */
	ackcnt = 0;
	if ( responseix == doix ) {
		ackstr[ ackcnt++ ] = (persp->a.interpr) ?
			_("this dump selected for interactive restoral\n")
			      : _("this dump selected for restoral\n");
	} else if ( responseix == dontix ) {
		ackstr[ ackcnt++ ] = _("dump skipped\n");
	} else {
		assert( responseix == sigintix );
		ackstr[ ackcnt++ ] = _("keyboard interrupt\n");
	}

	assert( ackcnt <= ACKMAX );
	dlog_multi_ack( ackstr,
			ackcnt );

	postamblecnt = 0;
	fold_init( fold, "end dialog", '-' );
	postamblestr[ postamblecnt++ ] = "\n";
	postamblestr[ postamblecnt++ ] = fold;
	postamblestr[ postamblecnt++ ] = "\n\n";
	assert( postamblecnt <= POSTAMBLEMAX );
	dlog_end( postamblestr,
		  postamblecnt );

	if ( responseix == sigintix ) {
		if ( cldmgr_stop_requested( )) {
			return BOOL_FALSE;
		}
		sleep( 1 ); /* to allow main thread to begin dialog */
		mlog( MLOG_NORMAL | MLOG_BARE,
		      "" ); /* to block until main thread dialog complete */
		sleep( 1 ); /* to allow main thread to request children die */
		if ( cldmgr_stop_requested( )) {
			return BOOL_FALSE;
		}
		mlog( MLOG_DEBUG,
		      "retrying dump selection dialog\n" );
		goto retry;
	}

	return responseix == doix;
}

/* restore_file - knows how to restore non-directory files
 *
 * uses the tree's callback iterator, which will call me for each
 * link to the specified inode.
 */
struct cb_context {
	drive_t *cb_drivep;
	filehdr_t *cb_fhdrp;
	rv_t cb_rv;
	bool_t cb_ehcs;
	bool_t cb_ahcs;
	char *cb_path1;
	char *cb_path2;
};

typedef struct cb_context cb_context_t;

static bool_t restore_file_cb( void *, bool_t, char *, char * );

static rv_t
restore_file( drive_t *drivep,
	      filehdr_t *fhdrp,
	      bool_t ehcs,
	      bool_t ahcs,
	      char *path1,
	      char *path2 )
{
	rv_t rv;
	bstat_t *bstatp = &fhdrp->fh_stat;
	cb_context_t context;

	/* ask the tree to call me back for each link to this inode.
	 * my callback will restore the file the first time it is
	 * invoked, and create a hard link in subsequent calls.
	 */
	context.cb_drivep = drivep;
	context.cb_fhdrp = fhdrp;
	context.cb_rv = RV_OK;
	context.cb_ehcs = ehcs;
	context.cb_ahcs = ahcs;
	context.cb_path1 = path1;
	context.cb_path2 = path2;
	rv = tree_cb_links( bstatp->bs_ino,
		       bstatp->bs_gen,
		       bstatp->bs_ctime.tv_sec,
		       bstatp->bs_mtime.tv_sec,
		       restore_file_cb,
		       &context,
		       path1,
		       path2 );
	if (context.cb_rv) /* context error result has precedence */
	    return context.cb_rv; /* this would be set by callback */
	else
	    return rv;
}

/* called for each link to the file described by fhdr. the first
 * call is detected by noting linkpr is FALSE, and is used to create/
 * update the first link to the file, using path1. subsequent calls have
 * linkpr set false, and should link path1 to path2. if path1 is ever null,
 * just pull from media: don't create.
 * if this func returns FALSE, will cause tree_cb_links to abort
 */
static bool_t
restore_file_cb( void *cp, bool_t linkpr, char *path1, char *path2 )
{
	cb_context_t *contextp = ( cb_context_t * )cp;
	drive_t *drivep = contextp->cb_drivep;
	filehdr_t *fhdrp = contextp->cb_fhdrp;
	bstat_t *bstatp = &fhdrp->fh_stat;
	rv_t *rvp = &contextp->cb_rv;
	bool_t ehcs = contextp->cb_ehcs;
	bool_t ahcs = contextp->cb_ahcs;
	stream_context_t *strctxp = (stream_context_t *)drivep->d_strmcontextp;

	int rval;
	bool_t ok;

	if ( cldmgr_stop_requested( )) {
		*rvp = RV_INTR;
		return BOOL_FALSE;
	}

	if ( ! linkpr ) {
		if (path1) {
			/* cache the path for use in restoring attributes
			 * and extended attributes
			 */
			strcpy(strctxp->sc_path, path1);
		}

		/* call type-specific function to create the file
		 */
		switch( bstatp->bs_mode & S_IFMT ) {
		case S_IFREG:
			ok = restore_reg( drivep, fhdrp, rvp, path1 );
			if (!ok)
				return ok;
			if ( fhdrp->fh_flags & FILEHDR_FLAGS_EXTATTR ) {
				*rvp = restore_extattr( drivep,
							fhdrp,
							path1,
							ahcs,
							BOOL_FALSE, /* isdirpr */
							BOOL_FALSE, /* onlydoreadpr */
							DAH_NULL );
			} else {
				ok = restore_extent_group( drivep,
							   fhdrp,
							   path1,
							   strctxp->sc_fd,
							   ehcs,
							   rvp );
			}
			return ok;
		case S_IFBLK:
		case S_IFCHR:
		case S_IFIFO:
#ifdef S_IFNAM
		case S_IFNAM:
#endif
		case S_IFSOCK:
			ok = restore_spec( fhdrp, rvp, path1 );
			return ok;
		case S_IFLNK:
			ok = restore_symlink( drivep,
					      fhdrp,
					      rvp,
					      path1,
					      path2,
					      ehcs );
			return ok;
		default:
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "ino %llu: unknown file type: %08x\n"),
			      bstatp->bs_ino,
			      bstatp->bs_mode );
			return BOOL_FALSE;
		}
	} else if ( ! tranp->t_toconlypr ) {
		assert( path1 );
		assert( path2 );
		mlog( MLOG_TRACE,
		      "linking %s to %s\n",
		      path1,
		      path2 );
		rval = unlink( path2 );
		if ( rval && errno != ENOENT ) {
			mlog( MLOG_VERBOSE | MLOG_WARNING, _(
			      "unable to unlink "
			      "current file prior to linking "
			      "%s to %s:"
			      " %s\n"),
			      path1,
			      path2,
			      strerror( errno ));
		} else {
			rval = link( path1, path2 );
			if ( rval ) {
				mlog( MLOG_NORMAL | MLOG_WARNING, _(
				      "attempt to "
				      "link %s to %s failed:"
				      " %s\n"),
				      path1,
				      path2,
				      strerror( errno ));
			}
		}
		return BOOL_TRUE;
	} else {
		mlog( MLOG_NORMAL | MLOG_BARE,
		      "%s\n",
		      path2 );
		return BOOL_TRUE;
	}
}

/*
 * Set the file owner and strip suid/sgid if necessary. On failure, it will
 * close the file descriptor, unlink the file and return -1. On success,
 * it will mark the stream contexts as having set the owner and return 0.
 */
static int
set_file_owner(
	char		 *path,
	int	 *fdp,
	stream_context_t *strcxtp)
{
	bstat_t		*bstatp = &strcxtp->sc_bstat;
	mode_t		mode = (mode_t)bstatp->bs_mode;
	int		rval;

	rval = fchown(*fdp, (uid_t)bstatp->bs_uid, (gid_t)bstatp->bs_gid );
	if (!rval)
		goto done;

	mlog(MLOG_VERBOSE | MLOG_WARNING,
	     _("chown (uid=%u, gid=%u) %s failed: %s\n"),
	     bstatp->bs_uid, bstatp->bs_gid, path, strerror(errno));

	if (mode & S_ISUID) {
		mlog(MLOG_VERBOSE | MLOG_WARNING,
		     _("stripping setuid bit on %s since chown failed\n"),
		     path);
		mode &= ~S_ISUID;
	}

	if ((mode & (S_ISGID|S_IXGRP)) == (S_ISGID|S_IXGRP)) {
		mlog(MLOG_VERBOSE | MLOG_WARNING,
		     _("stripping setgid bit on %s since chown failed\n"),
		     path);
		mode &= ~S_ISGID;
	}

	if (mode == (mode_t)bstatp->bs_mode)
		goto done;

	rval = fchmod(*fdp, mode);
	if (rval) {
		mlog(MLOG_VERBOSE | MLOG_ERROR,
		     _("unable to strip setuid/setgid on %s, unlinking file.\n"),
		     path);
		unlink(path);
		close(*fdp);
		*fdp = -1;
		return -1;
	}
done:
	strcxtp->sc_ownerset = BOOL_TRUE;
	return 0;
}

/* called to begin a regular file. if no path given, or if just toc,
 * don't actually write, just read. also get into that situation if
 * cannot prepare destination. fd == -1 signifies no write. *statp
 * is set to indicate drive errors. returns FALSE if should abort
 * this iteration.
 */
static bool_t
restore_reg( drive_t *drivep,
	     filehdr_t *fhdrp,
	     rv_t *rvp,
	     char *path )
{
	bstat_t *bstatp = &fhdrp->fh_stat;
	stream_context_t *strctxp = (stream_context_t *)drivep->d_strmcontextp;
	int *fdp = &strctxp->sc_fd;
	int rval;
	struct fsxattr fsxattr;
	struct stat64 stat;
	int oflags;

	if ( !path )
		return BOOL_TRUE;

	if ( fhdrp->fh_offset ) {
		if ( ! tranp->t_toconlypr ) {
			mlog( MLOG_TRACE,
			      "restoring regular file ino %llu %s"
			      " (offset %lld)\n",
			      bstatp->bs_ino,
			      path,
			      fhdrp->fh_offset );
		} else {
			mlog( MLOG_NORMAL | MLOG_BARE,
			      _("%s (offset %lld)\n"),
			      path,
			      fhdrp->fh_offset );
		}
	} else {
		if ( ! tranp->t_toconlypr ) {
			mlog( MLOG_TRACE,
			      "restoring regular file ino %llu %s\n",
			      bstatp->bs_ino,
			      path );
		} else {
			mlog( MLOG_NORMAL | MLOG_BARE,
			      "%s\n",
			      path );
		}
	}

	if ( tranp->t_toconlypr )
		return BOOL_TRUE;

	oflags = O_CREAT | O_RDWR;
	if (persp->a.dstdirisxfspr && bstatp->bs_xflags & XFS_XFLAG_REALTIME)
		oflags |= O_DIRECT;
			
	*fdp = open( path, oflags, S_IRUSR | S_IWUSR );
	if ( *fdp < 0 ) {
		mlog( MLOG_NORMAL | MLOG_WARNING,
		      _("open of %s failed: %s: discarding ino %llu\n"),
		      path,
		      strerror( errno ),
		      bstatp->bs_ino );
		return BOOL_TRUE;
	}

	rval = fstat64( *fdp, &stat );
	if ( rval != 0 ) {
		mlog( MLOG_VERBOSE | MLOG_WARNING,
		      _("attempt to stat %s failed: %s\n"),
		      path,
		      strerror( errno ));
	} else {
		if ( stat.st_size != bstatp->bs_size ) {
			mlog( MLOG_TRACE,
			      "truncating %s from %lld to %lld\n",
			      path,
			      stat.st_size,
			      bstatp->bs_size );

			rval = ftruncate64( *fdp, bstatp->bs_size );
			if ( rval != 0 ) {
				mlog( MLOG_VERBOSE | MLOG_WARNING,
				      _("attempt to truncate %s failed: %s\n"),
				      path,
				      strerror( errno ));
			}
		}
	}

	if (strctxp->sc_ownerset == BOOL_FALSE && persp->a.ownerpr) {
		rval = set_file_owner(path, fdp, strctxp);
		if (rval)
			return BOOL_TRUE;
	}

	if ( persp->a.dstdirisxfspr ) {

		/* set the extended inode flags, except those which must
		 * be set only after all data has been restored.
		 */
		assert( bstatp->bs_extsize >= 0 );
		memset((void *)&fsxattr, 0, sizeof( fsxattr ));
		fsxattr.fsx_xflags = bstatp->bs_xflags & ~POST_DATA_XFLAGS;
		fsxattr.fsx_extsize = (uint32_t) bstatp->bs_extsize;
		fsxattr.fsx_projid = bstat_projid(bstatp);

		rval = ioctl( *fdp, XFS_IOC_FSSETXATTR, (void *)&fsxattr);
		if ( rval < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING,
			      _("attempt to set extended attributes "
				"(xflags 0x%x, extsize = 0x%x, projid = 0x%x) "
				"of %s failed: %s\n"),
			      fsxattr.fsx_xflags,
			      fsxattr.fsx_extsize,
			      fsxattr.fsx_projid,
			      path,
			      strerror(errno));
		}
	}

	if ( persp->a.dstdirisxfspr && persp->a.restoredmpr ) {
		HsmBeginRestoreFile( bstatp,
				     *fdp,
				     &strctxp->sc_hsmflags );
	}

	return BOOL_TRUE;
}

/* called to peel a regular file's extent groups from the media.
 * if no path given, or if just toc, don't actually write, just
 * read. fd == -1 signifies no write. *rvp is set to indicate
 * drive errors. returns FALSE if should abort this iteration.
 */
static bool_t
restore_extent_group( drive_t *drivep,
		      filehdr_t *fhdrp,
		      char *path,
		      int fd,
		      bool_t ehcs,
		      rv_t *rvp )
{
	bstat_t *bstatp = &fhdrp->fh_stat;
	off64_t restoredsz = 0;
	extenthdr_t ehdr;
	off64_t bytesread;
	rv_t rv;

	/* copy data extents from media to the file
	 */
	for ( ; ; ) {
		/* read the extent header
		 */
		rv = read_extenthdr( drivep, &ehdr, ehcs );
		if ( rv != RV_OK ) {
			*rvp = rv;
			return BOOL_FALSE;
		}
		mlog( MLOG_NITTY,
		      "read extent hdr type %s offset %lld sz %lld flags %x\n",
		      ehdr_typestr( ehdr.eh_type ),
		      ehdr.eh_offset,
		      ehdr.eh_sz,
		      ehdr.eh_flags );

		/* if we see the specially marked last extent hdr,
		 * we are done.
		 */
		if ( ehdr.eh_type == EXTENTHDR_TYPE_LAST ) {
			/* For a wholly sparse file, there is no HOLE
			 * record; advance restoredsz to EOF.
			 */
			if (!restoredsz)
				restoredsz = bstatp->bs_size;
			break;
		}

		/* if its an ALIGNment extent, discard the extent.
		 */
		if ( ehdr.eh_type == EXTENTHDR_TYPE_ALIGN ) {
			size_t sz;
			assert( ehdr.eh_sz <= INTGENMAX );
			sz = ( size_t )ehdr.eh_sz;
			rv = discard_padding( sz, drivep );
			if ( rv != RV_OK ) {
				*rvp = rv;
				return BOOL_FALSE;
			}
			continue;
		}

		/* Add up extents restored to later check if the file
		 * is done.
		 */
		restoredsz += ehdr.eh_sz;  /* Increments of block size (usually 512) */

		/* Holes do not need to be restored since we now
		 * unlink the file at the start of the restore.
		 */
		if ( ehdr.eh_type == EXTENTHDR_TYPE_HOLE ) {
			continue;
		}

		/* real data
		 */
		assert( ehdr.eh_type == EXTENTHDR_TYPE_DATA );
		bytesread = 0;
		rv = restore_extent( fhdrp,
				     &ehdr,
				     fd,
				     path,
				     drivep,
				     &bytesread );
		if ( rv != RV_OK ) {
			*rvp = rv;
			return BOOL_FALSE;
		}

		if ( cldmgr_stop_requested( )) {
			*rvp = RV_INTR;
			return BOOL_FALSE;
		}
	}

	/* The extent group has been restored.  If the file is not
	 * complete, we may need to co-ordinate with other restore 
	 * streams to time the restoration of extended attributes
	 * and certain extended inode flags. Register the portion
	 * of the file completed here in the persistent state.
	 */
	if (bstatp->bs_size > restoredsz) {
		partial_reg(drivep->d_index,
			    bstatp->bs_ino,
			    bstatp->bs_size,
			    fhdrp->fh_offset,
			    restoredsz);
	}

	return BOOL_TRUE;
}

/* apply the attributes that can only go on now that all data
 * and extended attributes have been applied. fd == -1 signifies
 * no write, due to unknown path or toc only.
 */
static bool_t
restore_complete_reg(stream_context_t *strcxtp)
{
	bstat_t *bstatp = &strcxtp->sc_bstat;
	char *path = strcxtp->sc_path;
	int fd = strcxtp->sc_fd;
	struct utimbuf utimbuf;
	int rval;

	// only applies to regular files
	if (!S_ISREG((strcxtp->sc_bstat.bs_mode)))
		return BOOL_TRUE;

	if (fd < 0)
		return BOOL_TRUE;

	if (!partial_check(bstatp->bs_ino, bstatp->bs_size)) {
		close(fd);
		return BOOL_TRUE;
	}

	/* set the access and modification times
	 */
	utimbuf.actime = ( time32_t )bstatp->bs_atime.tv_sec;
	utimbuf.modtime = ( time32_t )bstatp->bs_mtime.tv_sec;
	rval = utime( path, &utimbuf );
	if ( rval ) {
		mlog( MLOG_VERBOSE | MLOG_WARNING, _(
		      "unable to set access and modification "
		      "times of %s: %s\n"),
		      path,
		      strerror( errno ));
	}

	/* set the owner and group (if enabled)
	 */
	if (strcxtp->sc_ownerset == BOOL_FALSE && persp->a.ownerpr) {
		rval = set_file_owner(path, &fd, strcxtp);
		if (rval)
			return BOOL_TRUE;
	}

	/* set the permissions/mode
	 */
	rval = fchmod( fd, ( mode_t )bstatp->bs_mode );
	if ( rval ) {
		mlog( MLOG_VERBOSE | MLOG_WARNING, _(
		      "unable to set mode of %s: %s\n"),
		      path,
		      strerror( errno ));
	}

	if ( persp->a.dstdirisxfspr && persp->a.restoredmpr ) {
		fsdmidata_t fssetdm;

		/* Set the DMAPI Fields. */
		fssetdm.fsd_dmevmask = bstatp->bs_dmevmask;
		fssetdm.fsd_padding = 0;
		fssetdm.fsd_dmstate = bstatp->bs_dmstate;

		rval = ioctl( fd, XFS_IOC_FSSETDM, ( void * )&fssetdm );
		if ( rval ) {
			mlog( MLOG_NORMAL | MLOG_WARNING,
			      _("attempt to set DMI attributes of %s "
			      "failed: %s\n"),
			      path,
			      strerror( errno ));
		}

		HsmEndRestoreFile( path, fd, &strcxtp->sc_hsmflags );
	}

	/* set any extended inode flags that couldn't be set
	 * prior to restoring the data.
	 */
	if ( persp->a.dstdirisxfspr && bstatp->bs_xflags & POST_DATA_XFLAGS ) {
		struct fsxattr fsxattr;
		memset((void *)&fsxattr, 0, sizeof( fsxattr ));
		fsxattr.fsx_xflags = bstatp->bs_xflags;
		fsxattr.fsx_extsize = (uint32_t)bstatp->bs_extsize;
		fsxattr.fsx_projid = bstat_projid(bstatp);

		rval = ioctl( fd, XFS_IOC_FSSETXATTR, (void *)&fsxattr );
		if ( rval < 0 ) {
			mlog(MLOG_NORMAL | MLOG_WARNING,
			     _("attempt to set extended attributes "
			     "(xflags 0x%x, extsize = 0x%x, projid = 0x%x) "
			     "of %s failed: %s\n"),
			     fsxattr.fsx_xflags,
			     fsxattr.fsx_extsize,
			     fsxattr.fsx_projid,
			     path,
			     strerror(errno));
		}
	}

	close(fd);
	return BOOL_TRUE;
}

/* ARGSUSED */
static bool_t
restore_spec( filehdr_t *fhdrp, rv_t *rvp, char *path )
{
	bstat_t *bstatp = &fhdrp->fh_stat;
	struct utimbuf utimbuf;
	char *printstr;
	int rval;

	if ( ! path ) {
		return BOOL_TRUE;
	}

	switch ( bstatp->bs_mode & S_IFMT ) {
	case S_IFBLK:
		printstr = _("block special file");
		break;
	case S_IFCHR:
		printstr = _("char special file");
		break;
	case S_IFIFO:
		printstr = _("named pipe");
		break;
#ifdef S_IFNAM
	case S_IFNAM:
		printstr = _("XENIX named pipe");
		break;
#endif
	case S_IFSOCK:
		printstr = _("UNIX domain socket");
		break;
	default:
		mlog( MLOG_NORMAL | MLOG_WARNING, _(
		      "%s: unknown file type: mode 0x%x ino %llu\n"),
		      path,
		      bstatp->bs_mode,
		      fhdrp->fh_stat.bs_ino );
		return BOOL_TRUE;
	}

	if ( ! tranp->t_toconlypr ) {
		mlog( MLOG_TRACE,
		      "restoring %s ino %llu %s\n",
		      printstr,
		      fhdrp->fh_stat.bs_ino,
		      path );
	} else {
		mlog( MLOG_NORMAL | MLOG_BARE,
		      "%s\n",
		      path );
	}

	if ( ! tranp->t_toconlypr ) {
		if ( ( bstatp->bs_mode & S_IFMT ) == S_IFSOCK ) {
			int sockfd;
			struct sockaddr_un addr;
			size_t addrlen;

			sockfd = socket( AF_UNIX, SOCK_STREAM, 0 );
			if ( sockfd < 0 ) {
				mlog( MLOG_VERBOSE | MLOG_WARNING, _(
				      "unable to create "
				      "%s ino %llu %s: %s: discarding\n"),
				      printstr,
				      fhdrp->fh_stat.bs_ino,
				      path,
				      strerror( errno ));
				return BOOL_TRUE;
			}
			memset( ( void * )&addr, 0, sizeof( addr ));
			addr.sun_family = AF_UNIX;
			if ( strlen( path ) >= sizeof( addr.sun_path )) {
				mlog( MLOG_VERBOSE | MLOG_WARNING, _(
				      "pathname too long for bind of "
				      "%s ino %llu %s: discarding\n"),
				      printstr,
				      fhdrp->fh_stat.bs_ino,
				      path );
				( void )close( sockfd );
				return BOOL_TRUE;
			}
			strcpy( addr.sun_path, path );
			addrlen = strlen( addr.sun_path )
				  +
				  sizeof( addr.sun_family );
			rval = bind( sockfd,
				     ( struct sockaddr * )&addr,
				     ( int )addrlen );
			if ( rval < 0 ) {
				mlog( MLOG_VERBOSE | MLOG_WARNING, _(
				      "unable to bind "
				      "%s ino %llu %s: %s: discarding\n"),
				      printstr,
				      fhdrp->fh_stat.bs_ino,
				      path,
				      strerror( errno ));
				( void )close( sockfd );
				return BOOL_TRUE;
			}
			( void )close( sockfd );

		} else {
			/* create the node
			*/
			rval = mknod( path,
				      ( mode_t )bstatp->bs_mode,
				      ( dev_t )IRIX_DEV_TO_KDEVT(bstatp->bs_rdev));
			if ( rval && rval != EEXIST ) {
				mlog( MLOG_VERBOSE | MLOG_WARNING, _(
				      "unable to create %s "
				      "ino %llu %s: %s: discarding\n"),
				      printstr,
				      fhdrp->fh_stat.bs_ino,
				      path,
				      strerror( errno ));
				return BOOL_TRUE;
			}
		}

		/* set the owner and group (if enabled)
		 */
		if ( persp->a.ownerpr ) {
			rval = chown( path,
				      ( uid_t )bstatp->bs_uid,
				      ( gid_t )bstatp->bs_gid );
			if ( rval ) {
				mlog( MLOG_VERBOSE | MLOG_WARNING,
				      _("chown (uid=%u, gid=%u) %s "
				      "failed: %s\n"),
				      bstatp->bs_uid,
				      bstatp->bs_gid,
				      path,
				      strerror( errno ));
			}
		}

		/* set the permissions/mode
		 */
		rval = chmod( path, ( mode_t )fhdrp->fh_stat.bs_mode );
		if ( rval ) {
			mlog( MLOG_VERBOSE | MLOG_WARNING, _(
			      "unable to set mode of %s: %s\n"),
			      path,
			      strerror( errno ));
		}

		/* set the access and modification times
		 */
		utimbuf.actime = ( time32_t )bstatp->bs_atime.tv_sec;
		utimbuf.modtime = ( time32_t )bstatp->bs_mtime.tv_sec;
		rval = utime( path, &utimbuf );
		if ( rval ) {
			mlog( MLOG_VERBOSE | MLOG_WARNING, _(
			      "unable to set access and modification "
			      "times of %s: %s\n"),
			      path,
			      strerror( errno ));
		}
	}

	return BOOL_TRUE;
}

static bool_t
restore_symlink( drive_t *drivep,
		 filehdr_t *fhdrp,
		 rv_t *rvp,
		 char *path,
		 char *scratchpath,
		 bool_t ehcs )
{
	bstat_t *bstatp = &fhdrp->fh_stat;
	drive_ops_t *dop = drivep->d_opsp;
	extenthdr_t ehdr;
	char *scratch;
	int nread;
	int rval;
	rv_t rv;
	mode_t oldumask;

	if ( path ) {
		if ( ! tranp->t_toconlypr ) {
			mlog( MLOG_TRACE,
			      "restoring symbolic link ino %llu %s\n",
			      bstatp->bs_ino,
			      path );
		} else {
			mlog( MLOG_NORMAL | MLOG_BARE,
			      "%s\n",
			      path );
		}
	}

	/* read the extent header
	 */
	rv = read_extenthdr( drivep, &ehdr, ehcs );
	if ( rv != RV_OK ) {
		*rvp = rv;
		return BOOL_FALSE;
	}

	/* symlinks always have one extent
	 */
	assert( ehdr.eh_type == EXTENTHDR_TYPE_DATA );

	/* read the link path extent
	 */
	if ( ehdr.eh_sz < ( off64_t )( 2 * MAXPATHLEN )) {
		scratch = scratchpath;
	} else {
		scratch = 0;
	}
	nread = read_buf( scratch,
			  ( size_t )ehdr.eh_sz,
			  ( void * )drivep,
			  ( rfp_t )dop->do_read,
			  ( rrbfp_t )dop->do_return_read_buf,
			  &rval );
	if ( rval ) {
		switch( rval ) {
		case DRIVE_ERROR_EOF:
		case DRIVE_ERROR_EOD:
		case DRIVE_ERROR_EOM:
		case DRIVE_ERROR_MEDIA:
			*rvp = RV_EOD;
			break;
		case DRIVE_ERROR_CORRUPTION:
			*rvp = RV_CORRUPT;
			break;
		case DRIVE_ERROR_DEVICE:
			*rvp = RV_DRIVE;
			break;
		case DRIVE_ERROR_CORE:
		default:
			*rvp = RV_CORE;
		}
		return BOOL_FALSE;
	}
	assert( ( off64_t )nread == ehdr.eh_sz );
	if ( ! scratch ) {
		if ( path ) {
			mlog( MLOG_VERBOSE | MLOG_WARNING, _(
			      "unable to create symlink ino %llu "
			      "%s: src too long: discarding\n"),
			      bstatp->bs_ino,
			      path );
		}
		return BOOL_TRUE;
	}
	scratchpath[ nread ] = 0;
	if ( ! tranp->t_toconlypr && path ) {
		/* create the symbolic link
		 */
		/* NOTE: There is no direct way to set mode for 
		 * sym links. Do it using umask.
		 * No way of setting times for sym links.
		 */

		oldumask = umask( (( mode_t )(~bstatp->bs_mode)) & 0777 );

		rval = symlink( scratchpath, path );

		umask( oldumask );

		if ( rval ) {
			mlog( MLOG_VERBOSE | MLOG_WARNING, _(
			      "unable to create "
			      "symlink ino %llu %s: %s: discarding\n"),
			      bstatp->bs_ino,
			      path,
			      strerror( errno ));
			return BOOL_TRUE;
		}

		/* set the owner and group (if enabled)
		 */
		if ( persp->a.ownerpr ) {
			rval = lchown( path,
				       ( uid_t )bstatp->bs_uid,
				       ( gid_t )bstatp->bs_gid );
			if ( rval ) {
				mlog( MLOG_VERBOSE | MLOG_WARNING,
				      _("chown (uid=%u, gid=%u) %s "
				      "failed: %s\n"),
				      bstatp->bs_uid,
				      bstatp->bs_gid,
				      path,
				      strerror( errno ));
			}
		}

		if ( persp->a.restoredmpr) {
		fsdmidata_t fssetdm;
		
		/*	Restore DMAPI fields. */

		fssetdm.fsd_dmevmask = bstatp->bs_dmevmask;
		fssetdm.fsd_padding = 0;
		fssetdm.fsd_dmstate = bstatp->bs_dmstate;
		rval = do_fssetdm_by_handle(path, &fssetdm);
		}
	}

	return BOOL_TRUE;
}

/* ARGSUSED */
static rv_t
read_filehdr( drive_t *drivep, filehdr_t *fhdrp, bool_t fhcs )
{
	bstat_t *bstatp = &fhdrp->fh_stat;
	drive_ops_t *dop = drivep->d_opsp;
	/* REFERENCED */
	int nread;
	int rval;
	filehdr_t tmpfh;

	nread = read_buf( ( char * )&tmpfh,
			  sizeof( *fhdrp ),
			  ( void * )drivep,
			  ( rfp_t )dop->do_read,
			  ( rrbfp_t )dop->do_return_read_buf,
			  &rval );
	xlate_filehdr(&tmpfh, fhdrp, 1);

	switch( rval ) {
	case 0:
		break;
	case DRIVE_ERROR_EOD:
	case DRIVE_ERROR_EOF:
	case DRIVE_ERROR_EOM:
	case DRIVE_ERROR_MEDIA:
		return RV_EOD;
	case DRIVE_ERROR_CORRUPTION:
		return RV_CORRUPT;
	case DRIVE_ERROR_DEVICE:
		return RV_DRIVE;
	case DRIVE_ERROR_CORE:
	default:
		return RV_CORE;
	}
	assert( ( size_t )nread == sizeof( *fhdrp ));

	mlog( MLOG_NITTY,
	      "read file hdr off %lld flags 0x%x ino %llu mode 0x%08x\n",
	      fhdrp->fh_offset,
	      fhdrp->fh_flags,
	      bstatp->bs_ino,
	      bstatp->bs_mode );

	if ( fhcs ) {
		if ( ! ( fhdrp->fh_flags & FILEHDR_FLAGS_CHECKSUM )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "corrupt file header\n") );
			return RV_CORRUPT;
		}
		if ( !is_checksum_valid( fhdrp, FILEHDR_SZ )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "bad file header checksum\n") );
			return RV_CORRUPT;
		}
	}

	return RV_OK;
}

/* ARGSUSED */
static rv_t
read_extenthdr( drive_t *drivep, extenthdr_t *ehdrp, bool_t ehcs )
{
	drive_ops_t *dop = drivep->d_opsp;
	/* REFERENCED */
	int nread;
	int rval;
	extenthdr_t tmpeh;

	nread = read_buf( ( char * )&tmpeh,
			  sizeof( *ehdrp ),
			  ( void * )drivep,
			  ( rfp_t )dop->do_read,
			  ( rrbfp_t )dop->do_return_read_buf,
			  &rval );
	xlate_extenthdr(&tmpeh, ehdrp, 1);

	switch( rval ) {
	case 0:
		break;
	case DRIVE_ERROR_EOD:
	case DRIVE_ERROR_EOF:
	case DRIVE_ERROR_EOM:
	case DRIVE_ERROR_MEDIA:
		return RV_EOD;
	case DRIVE_ERROR_CORRUPTION:
		return RV_CORRUPT;
	case DRIVE_ERROR_DEVICE:
		return RV_DRIVE;
	case DRIVE_ERROR_CORE:
	default:
		return RV_CORE;
	}
	assert( ( size_t )nread == sizeof( *ehdrp ));

	mlog( MLOG_NITTY,
	      "read extent hdr size %lld offset %lld type %d flags %08x\n",
	      ehdrp->eh_sz,
	      ehdrp->eh_offset,
	      ehdrp->eh_type,
	      ehdrp->eh_flags );

	if ( ehcs ) {
		if ( ! ( ehdrp->eh_flags & EXTENTHDR_FLAGS_CHECKSUM )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "corrupt extent header\n") );
			return RV_CORRUPT;
		}
		if ( !is_checksum_valid( ehdrp, EXTENTHDR_SZ )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "bad extent header checksum\n") );
			return RV_CORRUPT;
		}
	}

	return RV_OK;
}

/* ARGSUSED */
static rv_t
read_dirent( drive_t *drivep,
	     direnthdr_t *dhdrp,
	     size_t direntbufsz,
	     bool_t dhcs )
{
	global_hdr_t *grhdrp = drivep->d_greadhdrp;
	drive_ops_t *dop = drivep->d_opsp;
	/* REFERENCED */
	int nread;
	int rval;
	direnthdr_t tmpdh;
	char *namep;    // beginning of name following the direnthdr_t

	assert( sizeof( direnthdr_t ) == DIRENTHDR_SZ );
	assert( sizeof( direnthdr_v1_t ) == DIRENTHDR_SZ );

	/* read the head of the dirent
	 */
	nread = read_buf( ( char * )&tmpdh,
			  DIRENTHDR_SZ,
			  ( void * )drivep,
			  ( rfp_t )dop->do_read,
			  ( rrbfp_t )
			  dop->do_return_read_buf,
			  &rval );
	switch( rval ) {
	case 0:
		break;
	case DRIVE_ERROR_EOD:
	case DRIVE_ERROR_EOF:
	case DRIVE_ERROR_EOM:
	case DRIVE_ERROR_MEDIA:
		return RV_EOD;
	case DRIVE_ERROR_CORRUPTION:
		return RV_CORRUPT;
	case DRIVE_ERROR_DEVICE:
		return RV_DRIVE;
	case DRIVE_ERROR_CORE:
	default:
		return RV_CORE;
	}
	assert( ( size_t )nread == DIRENTHDR_SZ );

	if ( grhdrp->gh_version >= GLOBAL_HDR_VERSION_3 ) {
		xlate_direnthdr(&tmpdh, dhdrp, 1);
		namep = dhdrp->dh_name + sizeof(dhdrp->dh_name);

		if ( dhcs && !is_checksum_valid( dhdrp, DIRENTHDR_SZ )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
				"bad directory entry header checksum\n") );
			return RV_CORRUPT;
		}
	} else {
		direnthdr_v1_t dhdr_v1;
		xlate_direnthdr_v1((direnthdr_v1_t *)&tmpdh, &dhdr_v1, 1);
		dhdrp->dh_ino = dhdr_v1.dh_ino;
		dhdrp->dh_gen = BIGGEN2GEN(dhdr_v1.dh_gen);
		dhdrp->dh_checksum = dhdr_v1.dh_checksum;
		dhdrp->dh_sz = dhdr_v1.dh_sz;
		memcpy(dhdrp->dh_name, dhdr_v1.dh_name, sizeof(dhdr_v1.dh_name));
		namep = dhdrp->dh_name + sizeof(dhdr_v1.dh_name);

		if ( dhcs && !is_checksum_valid( &dhdr_v1, DIRENTHDR_SZ )) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
				"bad directory entry header checksum\n") );
			return RV_CORRUPT;
		}
	}

	mlog( MLOG_NITTY,
	      "read dirent hdr ino %llu gen %u size %u\n",
	      dhdrp->dh_ino,
	      ( size_t )dhdrp->dh_gen,
	      ( size_t )dhdrp->dh_sz );

	if ( dhdrp->dh_sz == 0 ) {
		mlog( MLOG_NORMAL | MLOG_WARNING, _(
			"corrupt directory entry header\n") );
		return RV_CORRUPT;
	}

	/* if null, return
	 */
	if ( dhdrp->dh_ino == 0 ) {
		assert( ( size_t )dhdrp->dh_sz == sizeof( direnthdr_t ));
		return RV_OK;
	}

	/* read the remainder of the dirent.
	 */
	assert( ( size_t )dhdrp->dh_sz <= direntbufsz );
	assert( ( size_t )dhdrp->dh_sz >= sizeof( direnthdr_t ));
	assert( ! ( ( size_t )dhdrp->dh_sz & ( DIRENTHDR_ALIGN - 1 )));
	if ( ( size_t )dhdrp->dh_sz > sizeof( direnthdr_t )) {
		size_t remsz = ( size_t )dhdrp->dh_sz - sizeof( direnthdr_t );
		nread = read_buf( namep,
				  remsz,
				  ( void * )drivep,
				  ( rfp_t )dop->do_read,
				  ( rrbfp_t )
				  dop->do_return_read_buf,
				  &rval );
		switch( rval ) {
		case 0:
			break;
		case DRIVE_ERROR_EOD:
		case DRIVE_ERROR_EOF:
		case DRIVE_ERROR_EOM:
		case DRIVE_ERROR_MEDIA:
			return RV_EOD;
		case DRIVE_ERROR_CORRUPTION:
			return RV_CORRUPT;
		case DRIVE_ERROR_DEVICE:
			return RV_DRIVE;
		case DRIVE_ERROR_CORE:
		default:
			return RV_CORE;
		}
		assert( ( size_t ) nread == remsz );
	}

	return RV_OK;
}

/* ARGSUSED */
static rv_t
read_extattrhdr( drive_t *drivep, extattrhdr_t *ahdrp, bool_t ahcs )
{
	drive_ops_t *dop = drivep->d_opsp;
	/* REFERENCED */
	int nread;
	int rval;
	extattrhdr_t tmpah;

	nread = read_buf( ( char * )&tmpah,
			  sizeof( *ahdrp ),
			  ( void * )drivep,
			  ( rfp_t )dop->do_read,
			  ( rrbfp_t )dop->do_return_read_buf,
			  &rval );
	xlate_extattrhdr(&tmpah, ahdrp, 1);

	switch( rval ) {
	case 0:
		break;
	case DRIVE_ERROR_EOD:
	case DRIVE_ERROR_EOF:
	case DRIVE_ERROR_EOM:
	case DRIVE_ERROR_MEDIA:
		return RV_EOD;
	case DRIVE_ERROR_CORRUPTION:
		return RV_CORRUPT;
	case DRIVE_ERROR_DEVICE:
		return RV_DRIVE;
	case DRIVE_ERROR_CORE:
	default:
		return RV_CORE;
	}
	assert( ( size_t )nread == sizeof( *ahdrp ));

	mlog( MLOG_NITTY,
	      "read extattr hdr sz %u valoff %u flags 0x%x valsz %u cs 0x%x\n",
	      ahdrp->ah_sz,
	      ( uint )ahdrp->ah_valoff,
	      ( uint )ahdrp->ah_flags,
	      ahdrp->ah_valsz,
	      ahdrp->ah_checksum );

	if ( ahcs ) {
		if ( ahdrp->ah_flags & EXTATTRHDR_FLAGS_CHECKSUM ) {
			if ( !is_checksum_valid( ahdrp, EXTATTRHDR_SZ )) {
				mlog( MLOG_NORMAL | MLOG_WARNING, _(
					"bad extattr header checksum\n") );
				return RV_CORRUPT;
			}
		} else if ( ahdrp->ah_flags & EXTATTRHDR_FLAGS_OLD_CHECKSUM ) {
			/* possibly a corrupt header, but most likely an old
			 * header, which cannot be verified due to a bug in how
			 * its checksum was calculated.
			 */
			static bool_t warned = BOOL_FALSE;
			if ( !warned ) {
				mlog( MLOG_NORMAL | MLOG_WARNING, _(
					"ignoring old-style extattr "
					"header checksums\n") );
				warned = BOOL_TRUE;
			}
		} else {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "corrupt extattr header\n") );
			return RV_CORRUPT;
		}
	}

	return RV_OK;
}

static rv_t
discard_padding( size_t sz, drive_t *drivep )
{
	drive_ops_t *dop = drivep->d_opsp;
	/* REFERENCED */
	int nread;
	int rval;

	nread = read_buf( 0,
			  sz,
			  ( void * )drivep,
			  ( rfp_t )dop->do_read,
			  ( rrbfp_t )dop->do_return_read_buf,
			  &rval );
	switch( rval ) {
	case 0:
		assert( ( size_t )nread == sz );
		return RV_OK;
	case DRIVE_ERROR_EOF:
	case DRIVE_ERROR_EOD:
	case DRIVE_ERROR_EOM:
	case DRIVE_ERROR_MEDIA:
		return RV_EOD;
	case DRIVE_ERROR_CORRUPTION:
		return RV_CORRUPT;
	case DRIVE_ERROR_DEVICE:
		return RV_DRIVE;
	case DRIVE_ERROR_CORE:
	default:
		return RV_CORE;
	}
}

static rv_t
restore_extent( filehdr_t *fhdrp,
		extenthdr_t *ehdrp,
		int fd,
		char *path,
		drive_t *drivep,
		off64_t *bytesreadp )
{
	bstat_t *bstatp = &fhdrp->fh_stat;
	drive_ops_t *dop = drivep->d_opsp;
	off64_t off = ehdrp->eh_offset;
	off64_t sz = ehdrp->eh_sz;
	off64_t new_off;
	struct dioattr da;
	bool_t isrealtime = BOOL_FALSE;

	*bytesreadp = 0;

	if ( fd != -1 ) {
		assert( path );
		/* seek to the beginning of the extent.
		 * must be on a basic fs blksz boundary.
		 */
		assert( ( off & ( off64_t )( BBSIZE - 1 )) == 0 );
		new_off = lseek64(  fd, off, SEEK_SET );
		if ( new_off < 0 ) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "attempt to seek %s to %lld failed: %s: "
			      "not restoring extent off %lld sz %lld\n"),
			      path,
			      off,
			      strerror( errno ),
			      off,
			      sz );
			fd = -1;
			new_off = off;
		}
		assert( new_off == off );
	}
	if ( (fd != -1) && (bstatp->bs_xflags & XFS_XFLAG_REALTIME) ) {
		if ( (ioctl(fd, XFS_IOC_DIOINFO, &da) < 0) ) {
			mlog( MLOG_NORMAL | MLOG_WARNING, _(
			      "dioinfo %s failed: "
			      "%s: discarding ino %llu\n"),
			      path,
			      strerror( errno ),
			      fhdrp->fh_stat.bs_ino );
			fd = -1;
		} else
			isrealtime = BOOL_TRUE;
	}

	/* move from media to fs.
	 */
	while ( sz ) {
		char *bufp;
		size_t req_bufsz;	/* requested bufsz */
		size_t sup_bufsz;	/* supplied bufsz */
		int nwritten;
		int rval;
		size_t ntowrite;

		req_bufsz = ( size_t )min( ( off64_t )INTGENMAX, sz );
		bufp = ( * dop->do_read )(drivep, req_bufsz, &sup_bufsz, &rval);
		if ( rval ) {
			rv_t rv;
			char *reasonstr;
			switch( rval ) {
			case DRIVE_ERROR_EOF:
				rv = RV_EOD;
				reasonstr = _("end of media file");
				break;
			case DRIVE_ERROR_EOD:
				rv = RV_EOD;
				reasonstr = _("end of recorded data");
				break;
			case DRIVE_ERROR_EOM:
				rv = RV_EOD;
				reasonstr = _("end of media");
				break;
			case DRIVE_ERROR_MEDIA:
				rv = RV_EOD;
				reasonstr = _("media error or no media");
				break;
			case DRIVE_ERROR_CORRUPTION:
				rv = RV_CORRUPT;
				reasonstr = _("end of media file");
				break;
			case DRIVE_ERROR_DEVICE:
				rv = RV_DRIVE;
				reasonstr = _("end of media file");
				break;
			case DRIVE_ERROR_CORE:
			default:
				rv = RV_CORE;
				reasonstr = _("dumping core");
				break;
			}
			mlog( MLOG_NORMAL, _(
			      "attempt to read %u bytes failed: %s\n"),
			      req_bufsz,
			      reasonstr );
			return rv;
		}
		if ( off >= bstatp->bs_size ) {
			assert( off == bstatp->bs_size );
			ntowrite = 0;
		} else if ((off64_t)sup_bufsz > bstatp->bs_size - off ) {
			ntowrite = ( size_t )( bstatp->bs_size - off );
		} else {
			ntowrite = sup_bufsz;
		}
		assert( ntowrite <= ( size_t )INTGENMAX );
		if ( ntowrite > 0 ) {
			*bytesreadp += ( off64_t )ntowrite;
			if ( fd != -1 ) {
				size_t tries;
				size_t remaining;
				int rval;
				off64_t tmp_off;

				rval = 0; /* for lint */
				for ( nwritten = 0,
				      tries = 0,
				      remaining = ntowrite,
				      tmp_off = off
				      ;
				      nwritten < ( int )ntowrite
				      &&
				      tries < WRITE_TRIES_MAX
				      ;
				      nwritten += rval,
				      tries++,
				      remaining -= ( size_t )rval,
				      tmp_off += ( off64_t )rval ) {
					int rttrunc = 0;
					int trycnt = 0;
					assert( remaining
						<=
						( size_t )INTGENMAX );
					/*
					 * Realtime files must be written
					 * to the end of the block even if
					 * it has been truncated back.
					 */
					if ( isrealtime &&
					    (remaining % da.d_miniosz != 0 ||
					     remaining < da.d_miniosz) ) {
						/*
						 * Since the ring and static 
						 * buffers from the different 
						 * drives are always large, we
						 * just need to write to the 
						 * end of the next block 
						 * boundry and truncate.
						 */
						rttrunc = remaining;
						remaining += da.d_miniosz - 
						   (remaining % da.d_miniosz);
					}
					/*
					 * Do the write. Due to delayed allocation
					 * it's possible to receive false ENOSPC
					 * errors when the filesystem is nearly
					 * full. XFS kernel code tries to avoid
					 * this, but cannot always do so. Catch
					 * ENOSPC and mimic the kernel behavior
					 * by trying to flush the current file
					 * first, then trying a system wide sync
					 * if ENOSPC still occurs.
					 */
					for (trycnt = 0; trycnt < 3; trycnt++) {
						rval = write( fd, bufp, remaining );
						if (rval >= 0 || errno != ENOSPC)
							break;

						( trycnt == 0 ) ?
							fdatasync(fd) : sync();
					}
					if ( rval < 0 ) {
						nwritten = rval;
						break;
					}
					assert( ( size_t )rval <= remaining );
					if ( rval < remaining ) {
						mlog( MLOG_NORMAL | MLOG_WARNING,
						      _("attempt to "
						      "write %u bytes to %s at "
						      "offset %lld failed: "
						      "only %d bytes written\n"),
						      remaining,
						      path,
						      tmp_off,
						      rval );
					}
					if (rttrunc) {
						/* truncate and re-set rval */
						if (rval == remaining)
							rval = rttrunc;
						ftruncate(fd, bstatp->bs_size);
					}
				}
			} else {
				nwritten = ( int )ntowrite;
			}
		} else {
			nwritten = 0;
		}
		( * dop->do_return_read_buf )( drivep, bufp, sup_bufsz );
		if ( ( size_t )nwritten != ntowrite ) {
			if ( nwritten < 0 ) {
				mlog( MLOG_NORMAL, _(
				      "attempt to write %u bytes to %s "
				      "at offset %lld failed: %s\n"),
				      ntowrite,
				      path,
				      off,
				      strerror( errno ));
			} else {
				assert( ( size_t )nwritten < ntowrite );
				mlog( MLOG_NORMAL, _(
				      "attempt to write %u bytes to %s at "
				      "offset %lld failed: only %d bytes "
				      "written\n"),
				      ntowrite,
				      path,
				      off,
				      nwritten );
			}
			/* stop attempting to write, but complete reads
			 */
			fd = -1;
			assert( ntowrite <= ( size_t )INTGENMAX );
			nwritten = ( int )ntowrite;
		}
		sz -= ( off64_t )sup_bufsz;
		off += ( off64_t )nwritten;
	}

	return RV_OK;
}

static char *extattrbufp = 0; /* ptr to start of all the extattr buffers */
static size_t extattrbufsz = 0; /* size of each extattr buffer */

static bool_t
extattr_init( size_t drivecnt )
{
	assert( ! extattrbufp );
	extattrbufsz = EXTATTRHDR_SZ		/* dump hdr */
		       +
		       NAME_MAX			/* attribute name */
		       +
		       1			/* NULL term. of name */
		       +
		       ATTR_MAX_VALUELEN;	/* attribute value */
	extattrbufsz = roundup(extattrbufsz, EXTATTRHDR_ALIGN);

	extattrbufp = memalign( EXTATTRHDR_ALIGN, extattrbufsz * drivecnt );
	if (extattrbufp == NULL) {
		mlog( MLOG_NORMAL | MLOG_ERROR, _(
		      "Failed to allocate extended attribute buffer\n") );
		return BOOL_FALSE;
	}

	return BOOL_TRUE;
}

static char *
get_extattrbuf( ix_t which )
{
        return extattrbufp + (extattrbufsz * which);
}


struct extattr_cb_ctx {
	extattrhdr_t *ecb_ahdrp;
};

typedef struct extattr_cb_ctx extattr_cb_ctx_t;

static rv_t
restore_extattr( drive_t *drivep,
		 filehdr_t *fhdrp,
		 char *path,
		 bool_t ahcs,
		 bool_t isdirpr,
		 bool_t onlydoreadpr,
		 dah_t dah )
{
	drive_ops_t *dop = drivep->d_opsp;
	extattrhdr_t *ahdrp = ( extattrhdr_t * )get_extattrbuf( drivep->d_index );
	stream_context_t *strctxp = (stream_context_t *)drivep->d_strmcontextp;
	bstat_t *bstatp = &fhdrp->fh_stat;
	bool_t isfilerestored = BOOL_FALSE;

	assert( extattrbufp );

	if ( ! isdirpr )
		isfilerestored = partial_check(bstatp->bs_ino,  bstatp->bs_size);

	/* peel off extattrs until null hdr hit
	 */
	for ( ; ; ) {
		size_t recsz;
		/* REFERENCED */
		int nread;
		int rval;
		rv_t rv;

		rv = read_extattrhdr( drivep, ahdrp, ahcs );
		if ( rv != RV_OK ) {
			return rv;
		}

		if ( ahdrp->ah_flags & EXTATTRHDR_FLAGS_NULL ) {
			return RV_OK;
		}

		recsz = ( size_t )ahdrp->ah_sz;
		assert( recsz <= extattrbufsz );
		assert( recsz >= EXTATTRHDR_SZ );
		nread = read_buf( ( char * )&ahdrp[ 1 ],
				  recsz - EXTATTRHDR_SZ,
				  ( void * )drivep,
				  ( rfp_t )dop->do_read,
				  ( rrbfp_t )dop->do_return_read_buf,
				  &rval );
		switch( rval ) {
		case 0:
			break;
		case DRIVE_ERROR_EOD:
		case DRIVE_ERROR_EOF:
		case DRIVE_ERROR_EOM:
		case DRIVE_ERROR_MEDIA:
			return RV_EOD;
		case DRIVE_ERROR_CORRUPTION:
			return RV_CORRUPT;
		case DRIVE_ERROR_DEVICE:
			return RV_DRIVE;
		case DRIVE_ERROR_CORE:
		default:
			return RV_CORE;
		}
		assert( nread == ( int )( recsz - EXTATTRHDR_SZ ));

		if ( ! persp->a.restoreextattrpr && ! persp->a.restoredmpr ) {
			continue;
		}

		if ( onlydoreadpr || tranp->t_toconlypr )
			continue;

		/* NOTE: In the cases below, if we get errors then we issue warnings 
		 * but we do not stop the restoration.
		 * We can still restore the file possibly without the 
		 * extended attributes.
		 */
		if ( isdirpr ) {
			assert( ! path );
			if ( dah != DAH_NULL ) {
				dirattr_addextattr( dah, ahdrp );
			}
		} else if ( isfilerestored && path[0] != '\0' ) {
			setextattr( path, ahdrp );

			if ( persp->a.dstdirisxfspr && persp->a.restoredmpr ) {
				int flag = 0;
				char *attrname = (char *)&ahdrp[1];
				if (ahdrp->ah_flags & EXTATTRHDR_FLAGS_ROOT)
					flag = ATTR_ROOT;
				else if (ahdrp->ah_flags & EXTATTRHDR_FLAGS_SECURE)
					flag = ATTR_SECURE;

				HsmRestoreAttribute( flag,
						     attrname,
						     &strctxp->sc_hsmflags );
			}
		}
	}
	/* NOTREACHED */
}

static bool_t
restore_dir_extattr_cb( char *path, dah_t dah )
{
        /* 
         * directory extattr's are built during the directory phase
         * by 1 thread so we only need one extattr buffer
         * -> we pick the 0th one
         */
	extattrhdr_t *ahdrp = ( extattrhdr_t * )get_extattrbuf( 0 );
	bool_t ok;

	/* ask the dirattr abstraction to call me back for each
	 * extended dirattr associated with this dah.
	 */
	ok = dirattr_cb_extattr( dah,
				 restore_dir_extattr_cb_cb,
				 ahdrp,
				 ( void * )path );
	return ok;
}

static bool_t
restore_dir_extattr_cb_cb( extattrhdr_t *ahdrp, void *ctxp )
{
	char *path = ( char * )ctxp;

	setextattr( path, ahdrp );

	return BOOL_TRUE;
}

static void
setextattr( char *path, extattrhdr_t *ahdrp )
{
	static	char dmiattr[] = "SGI_DMI_";
	bool_t isrootpr = ahdrp->ah_flags & EXTATTRHDR_FLAGS_ROOT;
	bool_t issecurepr = ahdrp->ah_flags & EXTATTRHDR_FLAGS_SECURE;
	bool_t isdmpr;
	int attr_namespace;
	int rval;

	isdmpr = ( isrootpr &&
		   !strncmp((char *)(&ahdrp[1]), dmiattr, sizeof(dmiattr)-1) );

	/* If restoreextattrpr not set, then we are here because -D was
	 * specified. So return unless it looks like a root DMAPI attribute.
	 */
	if ( !persp->a.restoreextattrpr && !isdmpr )
		return;

	if ( isrootpr ) {
		attr_namespace = ATTR_ROOT;
	} else if ( issecurepr ) {
		attr_namespace = ATTR_SECURE;
	} else {
		attr_namespace = 0;
	}

	rval = attr_set( path,
			 ( char * )( &ahdrp[ 1 ] ),
			 ( ( char * )ahdrp ) + ( u_long_t )ahdrp->ah_valoff,
			 ( int )ahdrp->ah_valsz,
			 attr_namespace | ATTR_DONTFOLLOW );
	if ( rval ) {
		char *namespace;
		if ( isrootpr ) {
			namespace = _("root");
		} else if ( issecurepr ) {
			namespace = _("secure");
		} else {
			namespace = _("non-root");
		}

		mlog( MLOG_VERBOSE | MLOG_WARNING, _(
		      "unable to set %s extended attribute for %s: "
		      "%s (%d)\n"),
		      namespace,
		      path,
		      strerror( errno ),
		      errno );
	}
}

#ifdef DEBUGPARTIALS
/*
 * Debug code to view the partials in the partial register
 */
void
dump_partials(void)
{
	partial_rest_t *isptr = NULL;
	bytespan_t *bsptr = NULL;
	int i;

	pi_lock();
	printf("\npartial_reg: count=%d\n", (int)persp->a.parrestcnt);
	if (persp->a.parrestcnt > 0) {
		for (i=0; i < partialmax; i++ ) {
			if (persp->a.parrest[i].is_ino > 0) {
				int j;

				isptr = &persp->a.parrest[i];
				printf("\tino=%llu ",
				       (unsigned long long)isptr->is_ino);
				for (j=0, bsptr=isptr->is_bs;
				     j < drivecnt; 
				     j++, bsptr++)
				{
					if (bsptr->endoffset > 0) {
						printf("%d:%lld-%lld ",
						   j, (long long)bsptr->offset,
						   (long long)bsptr->endoffset);
					} 
				}
				printf( "\n");
			}
		}
	}
	printf("\n");
	pi_unlock();
}


/* There can only be at most 2 partials for a given stream.
 * An unfinished one from a split and the current one from
 * a multiple group extent or another split.
 * If there are more than 2, then there is an internal error.
 */
void
check_valid_partials(void)
{
	int *num_partials; /* array for sum of partials for a given drive */
	partial_rest_t *isptr = NULL;
	bytespan_t *bsptr = NULL;
	int i;

	/* zero the sums for each stream */
	num_partials = calloc(drivecnt, sizeof(int));
	if (!num_partials) {
		perror("num_partials array allocation");
		return;
	}

	pi_lock();
	if (persp->a.parrestcnt > 0) {
		for (i=0; i < partialmax; i++ ) {
			if (persp->a.parrest[i].is_ino > 0) {
				int j;

				isptr = &persp->a.parrest[i];
				for (j=0, bsptr=isptr->is_bs;
				     j < drivecnt; 
				     j++, bsptr++)
				{
					if (bsptr->endoffset > 0) {
					    num_partials[j]++;
					    if (num_partials[j] > 2) {
						pi_unlock();
						mlog( MLOG_NORMAL | MLOG_WARNING, 
		  "partial_reg: Too many partials (>2) for drive: %d\n", j);
						dump_partials();
						exit(EXIT_ERROR);
					    }
					} 
				}
			}
		}
	}
	pi_unlock();
	free(num_partials);
}
#endif

/* partial_reg - Registers files that are only partially restored by
 * a dump stream into the persistent state.
 *
 * This is done because DMAPI extended attributes must not be set until
 * the entire file has been restored in order to co-ordinate with the 
 * Data Migration Facility (DMF) daemons.  Since extended attributes are
 * recorded with each extent group in the dump, this registry is used to
 * make sure only the final dump stream applies the extended attributes.
 *
 * Likewise, certain extended inode flags (e.g. XFS_XFLAG_IMMUTABLE)
 * should only be set after all data for a file has been restored.
 */
static void
partial_reg( ix_t d_index, 
	     xfs_ino_t ino, 
	     off64_t fsize, 
	     off64_t offset, 
	     off64_t sz)
{
	off64_t	endoffset;
	partial_rest_t *isptr = NULL;
	bytespan_t *bsptr = NULL;
	int i;

	mlog(MLOG_NITTY, "partial_reg: d_index = %d, ino = %llu, "
                           "fsize = %lld, offset = %lld, sz = %lld\n", 
                           d_index, ino, fsize, offset, sz);

	endoffset = offset + sz;

	if ( partialmax == 0 )
		return;

	pi_lock();

	/* Search for a matching inode.  Gaps can exist so we must search
	 * all entries. 
	 */
	for (i=0; i < partialmax; i++ ) {
		if (persp->a.parrest[i].is_ino == ino) {
			isptr = &persp->a.parrest[i];
			break;
		}
	}

	/* If not found, find a free one, fill it in and return */
	if ( ! isptr ) {
		mlog(MLOG_NITTY | MLOG_NOLOCK,
			"partial_reg: no entry found for %llu\n", ino);
		/* find a free one */
		for (i=0; i < partialmax; i++ ) {
			if (persp->a.parrest[i].is_ino == 0) {
				int j;

				isptr = &persp->a.parrest[i];
				isptr->is_ino = ino;
				persp->a.parrestcnt++;

				/* Clear all endoffsets (this value is
				 * used to decide if an entry is used or
				 * not
				 */
				for (j=0, bsptr=isptr->is_bs;
				     j < drivecnt; j++, bsptr++) {
				     bsptr->endoffset = 0;
				}

				goto found;
			}
		}

		/* Should never get here. */
		pi_unlock();
		mlog( MLOG_NORMAL | MLOG_WARNING, _(
		  "partial_reg: Out of records. Extend attrs applied early.\n"));
#ifdef DEBUGPARTIALS
		dump_partials();
#endif
		return;
	}

found:
	/* Update this drive's entry */
	bsptr = &isptr->is_bs[d_index];
	if (bsptr->endoffset == 0) {
		/* no existing entry for this drive, fill in the values */
		bsptr->offset = offset;
		bsptr->endoffset = endoffset;
		mlog(MLOG_NITTY | MLOG_NOLOCK,
			"partial_reg: update entry [%d]: "
			"<off = %lld, end = %lld>\n",
			d_index, offset, endoffset);
	} else {
		bool_t ret;

		/* entry exists for this drive, just extend the endoffset, the
		 * records will be sequential for any given drive.
		 */
		bsptr->endoffset = endoffset;
		ret = partial_check2(isptr, fsize);
		mlog(MLOG_NITTY | MLOG_NOLOCK,
			"partial_reg: extend entry [%d]: <end = %lld>\n",
			d_index, endoffset);
		mlog(MLOG_NITTY | MLOG_NOLOCK,
			"partial_reg: partial_check returns: %d\n", ret);
	}

	pi_unlock();

#ifdef DEBUGPARTIALS
	check_valid_partials();
	dump_partials();
#endif
}


/* Checks the registry of files that are only partially restored by
 * any given dump stream to see if the remainder of the file has
 * been restored by another dump stream.
 */
static bool_t
partial_check (xfs_ino_t ino, off64_t fsize)
{
	partial_rest_t *isptr = NULL;
	bool_t ret;
	int i;

	if ( partialmax == 0 )
		return BOOL_TRUE;

	pi_lock();

	/* Check if no files are listed in the sync area */
	if (persp->a.parrestcnt == 0) {
		pi_unlock();
		return BOOL_TRUE;
	}

	/* Search for the inode.  Gaps can exist so we must search
	 * all entries. 
	 */
	for (i=0; i < partialmax; i++ ) {
		if (persp->a.parrest[i].is_ino == ino) {
			isptr = &persp->a.parrest[i];
			break;
		}
	}

	/* If not found, return okay */
	if ( ! isptr ) {
		pi_unlock();
		return BOOL_TRUE;
	}

	ret = partial_check2(isptr, fsize);

	pi_unlock();

#ifdef DEBUGPARTIALS
	check_valid_partials();
	dump_partials();
#endif

	return ret;
}

/*
 * Checks the given parrest entry to see if the file has
 * been completely restored.
 * Always invoked with the persistent inventory locked (pi_lock)
 */
static bool_t
partial_check2(partial_rest_t *isptr, off64_t fsize)
{
	bytespan_t *bsptr = NULL;
	off64_t curoffset = 0;
	int i;

gapsearch:
	/* Search the entire set of bytespan records to see if the next
	 * span has been restored.  Bytespans are not necessarily in order
	 * so the search is repeated from the start each time.
	 */
	for (i=0, bsptr=isptr->is_bs; i < drivecnt; i++, bsptr++) {
		if (bsptr->endoffset > 0 && 
		    bsptr->offset <= curoffset && 
		    bsptr->endoffset > curoffset) 
		{
			curoffset = bsptr->endoffset;
			goto gapsearch;
		}
	}

	/* Check if all bytes are accounted for.  */
	if (curoffset >= fsize) {
		isptr->is_ino = 0;  /* clear the entry */
		persp->a.parrestcnt--;
		return BOOL_TRUE;
	} else {
		return BOOL_FALSE;
	}
}

static char *
ehdr_typestr( int32_t type )
{
	switch ( type ) {
	case EXTENTHDR_TYPE_LAST:
		return "LAST";
	case EXTENTHDR_TYPE_ALIGN:
		return "ALIGN";
	case EXTENTHDR_TYPE_DATA:
		return "DATA";
	case EXTENTHDR_TYPE_HOLE:
		return "HOLE";
	default:
		return "?";
	}
}

/* ARGSUSED */
bool_t
content_overwrite_ok( char *path,
		      int32_t ctime,
		      int32_t mtime,
		      char **reasonstrp,
		      bool_t *exists )
{
	struct stat statbuf;

	*exists = BOOL_TRUE;

	/* if file doesn't exist, allow
	 */

	if ( lstat( path, &statbuf )) {
		*reasonstrp = 0;
		if ( errno == ENOENT ) {
			*exists = BOOL_FALSE;
		}
		return BOOL_TRUE;
	}

	/* if overwrites absolutely inhibited, disallow
	 */
	if ( persp->a.existpr ) {
		*reasonstrp = _("overwrites inhibited");
		return BOOL_FALSE;
	}

	/* if newer time specified, compare 
	 */
	if ( persp->a.newerpr ) {
		if ( ( time32_t )ctime < persp->a.newertime ) {
			*reasonstrp = _("too old");
			return BOOL_FALSE;
		}
	}

	/* don't overwrite changed files
	 */
	if ( persp->a.changepr ) {
		if ( statbuf.st_ctime >= ( time32_t )ctime ) {
			*reasonstrp = _("existing version is newer");
			return BOOL_FALSE;
		}
	}

	*reasonstrp = 0;
	return BOOL_TRUE;
}


static void
set_mcflag( ix_t thrdix )
{
	lock( );
	mcflag[ thrdix ] = BOOL_TRUE;
	content_media_change_needed = BOOL_TRUE;
	unlock( );
}

static void
clr_mcflag( ix_t thrdix )
{
	lock( );
	mcflag[ thrdix ] = BOOL_FALSE;
	for ( thrdix = 0 ; thrdix < drivecnt ; thrdix++ ) {
		if ( mcflag[ thrdix ] ) {
			unlock( );
			return;
		}
	}
	content_media_change_needed = BOOL_FALSE;
	unlock( );
}

/* debug functions ***********************************************************/

static void
pi_show( char *introstring )
{
	char strbuf[ 100 ];
	/* REFERENCED */
	int strbuflen;
	fold_t fold;

	if ( mlog_level_ss[ MLOG_SS_MEDIA ] < MLOG_NITTY + 1 ) {
		return;
	}

	mlog_lock( );

	strbuflen = sprintf( strbuf,
			     "persistent inventory media file tree%s",
			     introstring );
	assert( ( size_t )strbuflen < sizeof( strbuf ));
	fold_init( fold, strbuf, ':' );
	mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
	      "\n%s\n\n",
	      fold );

	pi_show_nomloglock( );

	fold_init( fold, "end persistent inventory display", '.'  );
	mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
	      "\n%s\n\n",
	      fold );

	mlog_unlock( );
}

static void
pi_show_nomloglock( void )
{
	dh_t strmh;
	int strmix;


	/* no point in proceeding if pi not begun
	 */
	if ( persp->s.strmheadh == DH_NULL ) {
		mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA, _(
		      "session inventory unknown\n") );
		return;
	}

	mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA, _(
	      "session inventory display\n") );

	/* iterate over all streams
	 */
	for ( strmh = persp->s.strmheadh, strmix = 0
	      ;
	      strmh != DH_NULL
	      ;
	      strmh = DH2S( strmh )->s_nexth, strmix++ ) {
		dh_t objh;
		int objix;

		mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
		      _("\nmedia stream %u:\n"),
		      strmix );
		if ( DH2S( strmh )->s_cldh == DH_NULL ) {
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
			      _("\n    media objects not yet identified\n") );
			continue;
		}
		/* iterate over all objects
		 */
		for ( objh = DH2S( strmh )->s_cldh, objix = 0
		      ;
		      objh != DH_NULL
		      ;
		      objh = DH2O( objh )->o_nexth, objix++ ) {
			dh_t fileh;
			ix_t fileix;

			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
			      _("\n    media object %u:\n\n"),
			      objix );
			if ( DH2O( objh )->o_idlabvalpr ) {
				if ( strlen( DH2O( objh )->o_lab )) {
				mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
				      _("    label: ") );
				mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					"\"%s\"\n",
				      DH2O( objh )->o_lab );
				} else {
				mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
				      _("    label is blank\n") );
				}
				if ( ! uuid_is_null( DH2O( objh )->o_id)) {
				    char media_string_uuid[UUID_STR_LEN + 1];
				    uuid_unparse( DH2O( objh )->o_id,
						    media_string_uuid);
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  _("    id: %s\n"),
					  media_string_uuid );
				}
			} else {
				mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
				      _("    label not identified\n") );
			}
			if ( DH2O( objh )->o_fmfmixvalpr ) {
				mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
				      _("    index within object "
				      "of first media file: %u\n"),
				      DH2O( objh )->o_fmfmix );
			}
			if ( DH2O( objh )->o_fmfsixvalpr ) {
				mlog( MLOG_DEBUG | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
				      _("    index within stream "
				      "of first media file: %u\n"),
				      DH2O( objh )->o_fmfsix );
			}

			if ( DH2O( objh )->o_indrivepr ) {
				if ( drivecnt > 1 ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  _("    now in drive %u\n"),
					  DH2O( objh )->o_indriveix );
				} else {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  _("    now in drive\n") );
				}
			}

			if ( DH2O( objh )->o_cldh == DH_NULL ) {
				mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
				      _("        media files not yet "
				      "identified\n") );
				continue;
			}

			/* iterate over all files
			 */
			for ( fileh = DH2O( objh )->o_cldh, fileix = 0
			      ;
			      fileh != DH_NULL
			      ;
			      fileh = DH2F( fileh )->f_nexth, fileix++ ) {
			    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
				  _("\n        media file %u"),
				  fileix );
			    if ( DH2O( objh )->o_fmfmixvalpr ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  " (%u):\n",
					  DH2O( objh )->o_fmfmix + fileix );
			    } else {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  ":\n" );
			    }
			    if ( DH2F( fileh )->f_szvalpr ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					 _("        size: "
					 "%lld bytes\n"),
					 DH2F( fileh )->f_sz );
			    }
			    if ( DH2F( fileh )->f_dirtriedpr ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					 _("        used for directory "
					 "restoral\n") );
			    }
			    if ( DH2F( fileh )->f_valpr ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					 _("        first extent contained: "
					 "ino %llu off %lld\n"),
					 DH2F( fileh )->f_firstegrp.eg_ino,
					 DH2F( fileh )->f_firstegrp.eg_off );
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					 _("        next extent to restore: "
					 "ino %llu off %lld\n"),
					 DH2F( fileh )->f_curegrp.eg_ino,
					 DH2F( fileh )->f_curegrp.eg_off );
				    mlog( MLOG_DEBUG | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					 _("        rollback mark %lld\n"),
					 DH2F( fileh )->f_curmark );
			    }
			    if ( DH2F( fileh )->f_nondirskippr ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  _("        contains no selected "
					  "non-directories\n") );
			    }
			    if ( DH2F( fileh )->f_nondirdonepr ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  _("        non-directories done\n") );
			    }
			    if ( DH2F( fileh )->f_flags & PF_INV ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  _("        contains session "
					  "inventory\n") );
			    }
			    if ( DH2F( fileh )->f_flags & PF_TERM ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  _("        is stream terminator\n") );
			    }
			    if ( DH2F( fileh )->f_underheadpr ) {
				    mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
					  _("        now reading\n") );
			    }
			}
			if ( ! DH2O( objh )->o_lmfknwnpr ) {
				mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
				      _("\n        may be additional "
				      "unidentified media files\n") );
			}
		}
		if ( ! DH2S( strmh )->s_lastobjknwnpr ) {
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK | MLOG_MEDIA,
			      _("\n    may be "
			      "additional unidentified media objects\n\n") );
		}
	}
}

static int
egrpcmp( egrp_t *egrpap, egrp_t *egrpbp )
{
	if ( egrpap->eg_ino < egrpbp->eg_ino ) {
		return -1;
	} else if ( egrpap->eg_ino > egrpbp->eg_ino ) {
		return 1;
	} else if ( egrpap->eg_off < egrpbp->eg_off ) {
		return -1;
	} else if ( egrpap->eg_off > egrpbp->eg_off ) {
		return 1;
	} else {
		return 0;
	}
}

static void
display_dump_label( bool_t lockpr,
		    int mllevel,
		    char *introstr,
		    global_hdr_t *grhdrp,
		    media_hdr_t *mrhdrp,
		    content_hdr_t *crhdrp,
		    content_inode_hdr_t *scrhdrp )
{
	char dateline[ 28 ];
	char level_string[ 2 ];
	char dump_string_uuid[UUID_STR_LEN + 1];
	char media_string_uuid[UUID_STR_LEN + 1];
	char fs_string_uuid[UUID_STR_LEN + 1];

	assert( scrhdrp->cih_level >= 0 );
	assert( scrhdrp->cih_level < 10 );
	level_string[ 0 ] = ( char )( '0' + ( u_char_t )scrhdrp->cih_level );
	level_string[ 1 ] = 0;

	uuid_unparse(grhdrp->gh_dumpid, dump_string_uuid);
	uuid_unparse(mrhdrp->mh_mediaid, media_string_uuid);
	uuid_unparse(crhdrp->ch_fsid, fs_string_uuid);

	if ( lockpr ) {
		mlog_lock( );
	}

	mlog( mllevel | MLOG_NOLOCK,
	      "%s",
	      introstr );
	mlog( mllevel | MLOG_NOLOCK,
	      _("hostname: %s\n"),
	      grhdrp->gh_hostname );
	mlog( mllevel | MLOG_NOLOCK,
	      _("mount point: %s\n"),
	      crhdrp->ch_mntpnt );
	mlog( mllevel | MLOG_NOLOCK,
	      _("volume: %s\n"),
	      crhdrp->ch_fsdevice );
	mlog( mllevel | MLOG_NOLOCK,
	      _("session time: %s"),
	      ctime32_r( &grhdrp->gh_timestamp, dateline ));
	mlog( mllevel | MLOG_NOLOCK,
	      _("level: %s%s\n"),
	      level_string,
	      ( scrhdrp->cih_dumpattr & CIH_DUMPATTR_RESUME )
	      ?
	      _(" resumed")
	      :
	      "" );
	mlog( mllevel | MLOG_NOLOCK,
	      _("session label: ") );
	mlog( mllevel | MLOG_NOLOCK | MLOG_BARE,
	      "\"%s\"\n",
	      grhdrp->gh_dumplabel );
	mlog( mllevel | MLOG_NOLOCK,
	      _("media label: ") );
	mlog( mllevel | MLOG_NOLOCK | MLOG_BARE,
	      "\"%s\"\n",
	      mrhdrp->mh_medialabel );
	mlog( mllevel | MLOG_NOLOCK,
	      _("file system id: %s\n"),
	      fs_string_uuid );
	mlog( mllevel | MLOG_NOLOCK,
	      _("session id: %s\n"),
	      dump_string_uuid );
	mlog( mllevel | MLOG_NOLOCK,
	      _("media id: %s\n"),
	      media_string_uuid );

	if ( lockpr ) {
		mlog_unlock( );
	}
}

static void
display_needed_objects( purp_t purp,
			bag_t *bagp,
			bool_t knownholespr,
			bool_t maybeholespr )
{
	if ( bagp ) {
		ix_t ix;
		bagiter_t iter;
		bagobj_t *bagobjp;
		if ( purp == PURP_DIR ) {
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK, _(
			      "the following media objects "
			      "contain media files not yet tried "
			      "for directory hierarchy restoral:\n") );
		}
		if ( purp == PURP_NONDIR ) {
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK, _(
			      "the following media objects "
			      "are needed:\n") );
		}
		bagiter_init( bagp, &iter );
		bagobjp = 0; /* keep lint happy */
		ix = 0;
		while ( bagiter_next( &iter,
				      ( void ** )&bagobjp )) {
			char uuidstr[UUID_STR_LEN + 1];
			uuid_unparse( bagobjp->id, uuidstr);
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      "\n" );
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      _("%2u. label: "),
			      ix );
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      "\"%s\"\n",
			      bagobjp->label );
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      _("    id: ") );
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      "\"%s\"\n",
			      uuidstr );
			if ( bagobjp->indrivepr ) {
				if ( drivecnt > 1 ) {
					mlog( MLOG_NORMAL
					      |
					      MLOG_BARE
					      |
					      MLOG_NOLOCK,
					      _("    now in drive %u\n"),
					      bagobjp->indriveix );
				} else {
					mlog( MLOG_NORMAL
					      |
					      MLOG_BARE
					      |
					      MLOG_NOLOCK,
					      _("    now in drive\n") );
				}
			}
			ix++;
			bagobjp = 0; /* keep lint happy */
		}
	}
	if ( knownholespr ) {
		if ( purp == PURP_DIR ) {
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      bagp ? 
			      _("\nthere are additional unidentified media "
			      "objects containing media files not yet tried "
			      "for directory hierarchy restoral:\n")
				:
			      _("\nthere are unidentified media "
			      "objects containing media files not yet tried "
			      "for directory hierarchy restoral:\n") );
		}
		if ( purp == PURP_NONDIR ) {
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      bagp ?
			      _("\nthere are additional unidentified media "
			      "objects not yet fully restored\n")
				:
			      _("\nthere are unidentified media objects "
			      "not yet fully restored\n") );
		}
	} else if ( maybeholespr ) {
		if ( purp == PURP_DIR ) {
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      bagp ?
			      _("\nthere may be additional unidentified media "
			      "objects containing media files not yet tried "
			      "for directory hierarchy restoral:\n")
				:
			      _("\nthere may be unidentified media "
			      "objects containing media files not yet tried "
			      "for directory hierarchy restoral:\n") );
			      
		}
		if ( purp == PURP_NONDIR ) {
			mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
			      bagp ?
			      _("\nthere may be additional unidentified media "
			      "objects not yet fully restored\n")
				:
			      _("\there may be unidentified media "
			      "objects not yet fully restored\n") );
		}
	}

	if ( ! bagp && ! knownholespr && ! maybeholespr ) {
		mlog( MLOG_NORMAL | MLOG_BARE | MLOG_NOLOCK,
		      _("no additional media objects needed\n") );
	}
}

static int
do_fssetdm_by_handle(
	char		*path,
	fsdmidata_t	*fdmp)
{
	void		*hanp;
	size_t		hlen=0;
	int		rc;

	if (path_to_handle(path, &hanp, &hlen)) {
		mlog( MLOG_NORMAL | MLOG_WARNING, _(
			"path_to_handle of %s failed:%s\n"),
			path, strerror( errno ));
		return -1;
	}

	rc = fssetdm_by_handle(hanp, hlen, fdmp);
	free_handle(hanp, hlen);
	if (rc) {
		mlog( MLOG_NORMAL | MLOG_WARNING, _(
			"fssetdm_by_handle of %s failed %s\n"),
			path, strerror( errno ));
	}
	return rc;
}

static int
quotafilecheck(char *type, char *dstdir, char *quotafile)
{
	struct stat s;
	char buf[MAXPATHLEN];

	sprintf( buf,
		 "%s/%s",
		 dstdir,
		 quotafile );

	if ( stat (buf, &s ) >= 0 && S_ISREG(s.st_mode)) {
		mlog( MLOG_NORMAL, _(
		      "%s quota information written to '%s'\n"),
		      type,
		      buf );
		return 1;
	}

	return 0;
}
