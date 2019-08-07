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
#ifndef DRIVE_H
#define DRIVE_H

/* drive.[hc] - drive abstraction
 *
 * drive_t provides a framework for device-independent drive abstraction,
 * as well as initialization of appropriate drive handlers.
 *
 * A drive_t is a drive manager providing capabilities for reading, writing,
 * and manipulating media objects (e.g., tape cartridges).
 *
 * All writing and reading is done within the scope of a media file. Writes
 * may be cut short due to media events (e.g., EOM). The mark
 * mechanism allows the caller to determine what portion of the write data
 * stream is guaranteed to have been committed to media.
 *
 * Marks may be placed in the stream during writes. During subsequent
 * reads of the data stream, the media object may be positioned at a
 * specific mark, or to the next mark in the stream.
 *
 * To accomodate end-of-media uncertainty, the marks are set in two steps.
 * First the caller asks to set a mark at a position within the write stream,
 * providing a callback function. The callback will be invoked when the data
 * prior to that mark has been committed to media.
 *
 * The caller must re-position the media (e.g., change media objects or
 * begin a new media file) and re-write any portion of the data written
 * after the last committed mark. It is entirely possible for part of that
 * data to have made it onto the previous media file, due to the uncertainty
 * of end-of-media handling of some drives. Thus the caller must be prepared
 * for portions of data read from the end of one media file to be duplicated
 * at the beginning of the next media file.
 *
 * To allow repositioning the stream in the above scenarios,
 * drive_t supplies forward-space-file, backward-space-file, rewind,
 * seek_mark, and get_mark operators.
 *
 * A call to the end_read or end_write operator virtually positions the
 * media at the beginning of the next media file. Thus if a caller calls
 * begin read, looks at the header and decides to overwrite the media
 * file, a call to the bsf operator must be made prior to the call to
 * begin_write. However, if the media is positioned at EOD and the caller calls
 * begin_read, EOD will be returned and no repositioning is needed. A call
 * to begin_write will append a new media file after the last media file.
 *
 * Drive_init1 selects and instantiates a drive manager for each drive
 * identified on the command line. This is done by showing the drive
 * successively to each drive strategy module. The drive strategies
 * supply a match score. The strategy with the highest score gets to
 * instantiate a drive manager for that drive.
 *
 * Drive_init2 allocates and initializes read and write header buffers for
 * each drive, and calls each drive manager's init operator, which begins async
 * initialization of the drive. This includes determining if a media object
 * is currently mounted in that drive.
 *
 * Drive_init3 synchronizes with the async initializations kicked off by
 * drive_init2.
 *
 * Each drive strategy is defined by a drive_strategy_t. This supplies
 * a match function, and an instantiate function.
 *
 * The match function lets the given strategy determine if it is the correct
 * strategy to choose. The instantiate function creates a driver manager.
 *
 * The I/O write model presents the user with a function to get a write buffer,
 * or get a page-aligned write buffer, and a function to take back the buffer
 * and commit part or all of that buffer to media. The read model is similar;
 * the caller may ask for a buffer filled with read data, and gives the buffer
 * back after the data is used. These models facilitate high-performance
 * operation, since the buffers can be used in direct I/O.
 */

/* drive_hdr_t - drive media file header
 *
 * A drive media file header is imbedded in the global media file header
 * structure, which is in turn placed at the beginning of each media file. The
 * drive hdr has three parts: generally useful drive-related info, info
 * specific to the selected drive strategy, and upper layer info. Each drive
 * handler contains two drive header images; a read hdr and a write hdr.
 *
 * CAUTION! the various components of the media file header are carefully
 * crafted to fit in DRIVE_HDR_SZ bytes.
 */
#define DRIVE_HDR_SZ		sizeofmember( global_hdr_t, gh_upper )

struct drive_hdr {
	uint32_t dh_drivecnt;				/*   4    4 */
		/* number of drives used to dump the fs */
	uint32_t dh_driveix;				/*   4    8 */
		/* 0-based index of the drive used to dump this stream */
	int32_t dh_strategyid;				/*   4    c */
		/* ID of the drive strategy used to produce this dump */
	char dh_pad1[ 0x1f4 ];				/* 1f4  200 */
		/* padding */
	char dh_specific[ 0x200 ];			/* 200  400 */
		/* drive strategy-specific info */
	char dh_upper[ DRIVE_HDR_SZ - 0x400 ];		/* 800  c00 */
		/* header info private to upper software layers */
};

typedef struct drive_hdr drive_hdr_t;

/* drive_strategy_t - drive strategy
 *
 * The drive strategy has two operators, ds_match() and ds_instantiate().
 * ds_match() returns an integer indicating the degree of match of the
 * strategy to the drives indicated on the command line. ds_init()
 * initializes the drive strategy, creates and initializes a drive manager
 * for each drive, and if possible positions each drive at the beginning of
 * the current file and reads the file header into d_readhdr. ds_drivep
 * points to an array of drive managers, and ds_drivecnt indicates
 * the length of that array.
 */
struct drive;			/* forward declaration */

struct drive_strategy {
	int ds_id;
		    /* strategy ID
		     */
	char *ds_description;
		    /* a short char string describing strategy
		     */
	int ( * ds_match )( int argc,
				 char *argv[ ],
				 struct drive *drivep );
		    /* returns degree of match. drivep has been pre-allocated
		     * and initialized with generic info.
		     */
	bool_t ( * ds_instantiate )( int argc,
				     char *argv[ ],
				     struct drive *drivep );
		    /* creates a drive manager instance, by filling in the
		     * blanks of the pre-allocated drive descriptor
		     * returns FALSE on failure.
		     */
	off64_t ds_recmarksep;
		    /* recommended maximum separation (in bytes)
		     * between marks. can be observed when sending
		     * long data streams to the drive. this field is
		     * transfered to d_recmarksep during instantiation.
		     */
	off64_t ds_recmfilesz;
		    /* recommended maximum length of media files.
		     * transfered to d_recmarksep during instantiation.
		     */
};

typedef struct drive_strategy drive_strategy_t;

/* definitions of strategy ids
 */
#define DRIVE_STRATEGY_SCSITAPE	0
#define DRIVE_STRATEGY_SIMPLE	1
#define DRIVE_STRATEGY_RMT	0  /* same as SCSITAPE for now */


/* drive_mark_t - token identifying a mark within a media object file
 *
 * 64 bit cookie, the structure of which is known only to the drive manager
 * which generated it. generated by do_get_mark, given to do_seek_mark.
 */
typedef off64_t drive_mark_t;

/* drive mark callback function
 * invoked for each mark registered with do_set_mark, in the same order.
 * committed is TRUE if the data up to the mark was committed to media.
 * committed is FALSE if it is POSSIBLE that some data prior to the mark
 * was NOT committed.
 */
struct drive_markrec; /* forward decl */
typedef void ( * drive_mcbfp_t )( void *context_t,
				  struct drive_markrec *markrecp,
				  bool_t committed );

/* drive_markrec_t - context for set mark callback function
 *
 * caller allocates and passes ptr to do_set_mark, along with a callback func.
 * callback returns the record, which must be freed by the original caller.
 * 
 */
struct drive_markrec {
	drive_mark_t dm_log;		/* identifies position within mfile */
	drive_mcbfp_t dm_cbfuncp;	/* caller's callback function */
	void *dm_cbcontextp;		/* caller's context */
	struct drive_markrec *dm_nextp;	/* for linked list */
};

typedef struct drive_markrec drive_markrec_t;

/* drive_t - drive manager
 *
 * A drive manager is described by a set of generic parameters
 * and operators.
 */
struct drive_ops;	/* forward declaration */

struct drive {
	drive_strategy_t *d_strategyp;
				/* back ptr to strategy
				 */
	struct drive_ops *d_opsp;
				/* pointer to drive operations
				 */
	void *d_contextp;
				/* manager-specific data
				 */
	void *d_strmcontextp;
				/* stream-specific data
				 */
	global_hdr_t *d_greadhdrp;
				/* global portion of media file header of
				 * media file drive currently positioned.
				 */
	drive_hdr_t *d_readhdrp;
				/* drive portion of media file header of
				 * media file drive currently positioned.
				 */
	global_hdr_t *d_gwritehdrp;
				/* global portion of media file header to
				 * be written by the next begin_write op.
				 */
	drive_hdr_t *d_writehdrp;
				/* drive portion of media file header to
				 * be written by the next begin_write op.
				 */
	char *d_pathname;	/* e.g., /dev/tape */
	ix_t d_index;		/* e.g., 0, 1, 2, ... */
	bool_t d_isnamedpipepr;	/* is a named pipe */
	bool_t d_isunnamedpipepr;/* is an unnamed pipe */
	int d_capabilities;/* see DRIVE_CAP_xxx below */
	off64_t d_cap_est;	/* capacity estimate in bytes; -1 if unknown */
	int d_rate_est;	/* bytes per second; -1 if unknown */
	drive_markrec_t *d_markrecheadp; /* linked list of mark records */
	drive_markrec_t *d_markrectailp; /* yet to be committed */
	off64_t d_recmarksep;	/* transfered from strategy on instantiation */
	off64_t d_recmfilesz;	/* transfered from strategy on instantiation */
};

typedef struct drive drive_t;

struct drive_ops {
	bool_t ( * do_init )( drive_t *drivep );
				/* initializes drive, and begins async
				 * determination of media object presence
				 * returns FALSE if session should be aborted.
				 */
	bool_t ( * do_sync )( drive_t *drivep );
				/* synchronizes with the activity kicked off
				 * by do_init. returns FALSE if session should
				 * be aborted.
				 */
	int ( * do_begin_read )( drive_t *drivep );
				/* prepares the drive manager for reading.
				 * if the media is positioned at BOM or just
				 * after a file mark, current media file is
				 * begun. Otherwise, the drive will advance
				 * media to just after the next file mark first.
				 * loads the read header buffer with the
				 * header at the beginning of the media file.
				 * return value will be set non zero:
				 *	BLANK - blank tape, positioned
				 *		at BOT and ready for a
				 *		do_begin_write;
				 *	FOREIGN - unrecognized data at
				 *		BOT, positioned
				 *		at BOT and ready for a
				 *		do_begin_write;
				 *	EOD - tried to read past end of
				 *		recorded data;
				 *	EOM - tried to read past end of media
				 *		object;
				 *	MEDIA - no media object in drive;
				 *	FORMAT - unrecognized media file header;
				 *	VERSION - unsupported media file header;
				 *	CORRUPTION - encountered corrupt data
				 *		at beginning of media file;
				 *	DEVICE - drive hardware error, other
				 *		than missing media;
				 *	INVAL - operator error;
				 *	CORE  - driver wants core dump;
				 *	STOP - upper level asked lengthy
				 *		drive op to give up.
				 *	OVERWRITE - overwrite option to dump
				 *	specified by user. Assume tape
				 *	positioned and ready for write.
				 * if not successful (non-zero return), caller
				 * must NOT call end_read prior to next
				 * begin_read. if successful, caller MUST call
				 * end_read prior to next begin_read.
				 */
	char * ( * do_read )( drive_t *drivep,
			      size_t wanted_bufsz,
			      size_t *actual_bufszp,
			      int *statp );
				/* asks the drive manager for a buffered filled
				 * with the next read stream data.
				 * the actual buffer size supplied may
				 * be shorter than requested; it will never
				 * be longer than requested. the buffer
				 * should be returned to the drive manager as
				 * soon as possible, to facilitate drive
				 * streaming. the actual buffer size may
				 * be short due to the internal buffering
				 * mechanism, or due to media events; in
				 * the latter case, *statp will explain why:
				 *	EOD - tried to read past end of
				 *		recorded data; this is semant-
				 *		ically equiv. to EOF, but
				 *		occurs on last media file only.
				 *	EOF - hit end of media file prior
				 *		to completing read; probably
				 *		due to a write fail during dump,
				 *		or were near end of tape (e.g.,
				 *		early warning).
				 *	EOM - tried to read past end of media
				 *		object;
				 *	MEDIA - no media object in drive;
				 *	CORRUPTION - encountered corrupt data;
				 *	DEVICE - drive hardware error, other
				 *		than missing media;
				 *	CORE  - driver wants core dump;
				 * in all cases, the buffer returned contains
				 * valid data (although the buffer size may
				 * be zero!).
				 */
	void ( * do_return_read_buf )( drive_t *drivep,
				       char *bufp,
				       size_t bufsz );
				/* returns the buffer obtained
				 * from the previous do_read() call.
				 * the entire buffer must be returned
				 * in one shot.
				 */
	void ( * do_get_mark )( drive_t *drivep,
				drive_mark_t *drivemarkp );
				/* returns (by reference) a mark corresponding
				 * to the next byte which will be read by a
				 * call to do_read(). will be used in a later
				 * session to seek to that position.
				 */
	int ( * do_seek_mark )( drive_t *drivep,
				     drive_mark_t *drivemarkp );
				/* searches for the specified mark within the
				 * current file. returns zero if the mark
				 * was found, or an error explaining why not:
				 *	EOF - end of file was encountered;
				 *	EOD - end of recorded date encountered;
				 *	EOM - end of media encountered;
				 *	CORRUPTION - encountered corrupt data;
				 *	DEVICE - device error;
				 */
	int ( * do_next_mark )( drive_t *drivep );
				/* if d_capabilities has DRIVE_CAP_NEXTMARK set,
				 * drive has the capability to
				 * seek forward to the next mark. returns
				 * zero if a mark was found, or an error code:
				 *	EOF - end of file was encountered;
				 *	EOD - end of recorded date encountered;
				 *	EOM - end of media encountered;
				 *	DEVICE - device error;
				 * if currently at a mark, will go to the next.
				 */
	void ( *do_end_read )( drive_t *drivep );
				/* ends the file read. must be called prior
				 * to beginning another read or write session.
				 * ensures that the next call to begin_read
				 * will position the media at the next media
				 * file.
				 */
	int ( * do_begin_write )( drive_t *drivep );
				/* begins a write media file for writing.
				 * asserts the media is positioned at BOM or
				 * just after a file mark. write header will
				 * be placed at the head of the file.
				 * same return codes as do_write():
				 *	EOM - end of media object encountered;
				 *	MEDIA - media object missing or broken;
				 *	DEVICE - device error;
				 *	CORE  - driver error
				 */
	void ( * do_set_mark )( drive_t *drivep,
				drive_mcbfp_t cbfuncp,
				void *cbcontextp,
				drive_markrec_t *markrecp );
				/* marks the position in the write stream
				 * where the next write will occur.
				 * At the time the data written
				 * prior to the mark is committed to media,
				 * the callback will be invoked, with
				 * the context and markrec pointers. The caller
				 * append his mark context to the drive markrec
				 * buffer pointed to by markrecp. the drive
				 * manager will use the drive_markrec_t
				 * to track the progress of the mark.
				 * If EOM is encountered before the
				 * mark is committed to media, the mark
				 * callback for all uncommitted marks will
				 * be called, but with the committed flag
				 * set to FALSE. it is up to the
				 * caller to free the mark buffers he previously
				 * allocated. note the drive strategy member
				 * d_recmarksep, the recommended maximum
				 * separation between marks. This is a hint
				 * based on underlying drive characteristics.
				 * the caller should record the committed
				 * marks; when do_write() fails due to
				 * EOM, the caller can resume writing after
				 * last committed marked point in the write
				 * stream.
				 */
	char * ( * do_get_write_buf )( drive_t *drivep,
				       size_t wanted_bufsz,
				       size_t *actual_bufszp );
				/* asks the drive manager for a buffer.
				 * returns a pointer to a buffer, and its
				 * size. must call do_write() before
				 * calling do_get_write_buf() again. By asking
				 * the drive manager for a buffer, buffer
				 * copying can be avoided. This facilitates
				 * achieving streaming performance on tape
				 * media. Also, the drive manager can implement
				 * a double-buffering scheme, causing source
				 * reads and drive writes to occur
				 * simultaneously. note that the buffer can
				 * be larger or smaller than the wanted bufsz,
				 * but will be at least 1 byte in length.
				 */
	int ( * do_write )( drive_t *drivep,
				 char *bufp,
				 size_t bufsz );
				/* asks the drive manager to write bufsz
				 * bytes from the buffer, which was acquired
				 * from a previous call to do_get_write_buf().
				 * returns the following status values:
				 *	EOM - end of media object encountered;
				 *	MEDIA - media object missing or broken;
				 *	DEVICE - device error;
				 *	CORE  - driver error;
				 * in the case of a non-zero return status,
				 * the caller must refer to the last mark
				 * callback received, to know the last data
				 * guaranteed to have been committed to media.
				 * any writes subsequent to the placement of
				 * that mark may or may not be on media.
				 * the caller should therefore be prepared
				 * to see the same data twice when crossing
				 * a media file boundary while reading.
				 * note that no indication of the actual
				 * number of bytes written is returned.
				 * even if only part of the buffer
				 * obtained from do_get_write_buf()
				 * is used, the do_write() call returns the
				 * entire buffer to the drive manager; it
				 * is not available for subsequent writes.
				 * instead, the caller must get another buffer
				 * using do_get_write_buf().
				 */
	size_t ( * do_get_align_cnt )( drive_t *drivep );
				/* used during writing. returns the number
				 * of bytes which should be written to
				 * page-align the next do_get_write_buf()
				 * call. NOTE: the caller can assume that
				 * alignment will be maintained after the
				 * initial alignment done using this info.
				 */
	int ( * do_end_write )( drive_t *drivep, off64_t *ncommittedp );
				/* terminates a media file write sequence.
				 * flushes any buffered data not yet committed
				 * to media, and calls callbacks for all marks
				 * not yet returned. just like do_write(), can
				 * encounter EOM, in which case the
				 * last affirmative mark callback received will
				 * indicate what portion of the write data
				 * stream may have not been committed to media.
				 *	EOM - end of media object encountered;
				 *	MEDIA - media object missing or broken;
				 *	DEVICE - device error;
				 * returns by reference number of bytes
				 * committed to media for the media file.
				 * this is the number of bytes written to
				 * media, which may be greater than what the
				 * client wrote due to record padding.
				 * NOTE: if last call to do_write returned
				 * an error, do_end_write will not do any
				 * I/O, and will return 0.
				 */
	int ( * do_fsf )( drive_t *drivep,
			      int count,
			      int *statp );
				/* if d_capabilities has DRIVE_CAP_FSF set,
				 * drive has the capability to
				 * forward space count files. returns the
				 * number of actual files forwarded to. for
				 * example, if count is one, and there is a
				 * file after the current file, positions to
				 * the beginning of that file and returns 1.
				 * if the current file is the last one on the
				 * medium, returns 0, and leaves position at
				 * the end of recorded data, after the file
				 * mark. if there is a following file,
				 * positions such that the next read will
				 * retrieve the first data in that file.
				 * furthermore, *statp will be set in the
				 * following cases:
				 *	EOD - encountered end of recorded data
				 *	EOM - encountered end of media
				 *	DEVICE - device error;
				 * NOTE: if media is positioned anywhere other
				 * than at the beginning of a media file,
				 * behaves as if position is at most recent
				 * file mark or BOT.
				 */
	int ( * do_bsf )( drive_t *drivep,
			       int count,
			       int *statp );
				/* if d_capabilities has DRIVE_CAP_BSF set,
				 * drive has the capability to backward space
				 * count files. returns the number of actual
				 * files backed to. 
				 * can only be invoked when not reading or
				 * writing (i.e., only after calling end_read
				 * or end_write, or prior to calling begin_read
				 * or begin_write). if count is zero, backs
				 * up to the beginning of the last media file
				 * read or written. if count is 1, backs up to
				 * the media file preceeding that one. returns
				 * the number of media files skipped. if
				 * the current file is the first one on the
				 * medium, returns 0, and leaves position
				 * at the beginning of recorded data. if count
				 * is one and there is a preceeding file,
				 * positions such that the next read will
				 * retrieve the first data in that file.
				 * furthermore, *statp will be set in the
				 * following cases:
				 *	BOM - hit beginning of recorded data;
				 *	DEVICE - device error;
				 */
	int ( * do_rewind )( drive_t *drivep );
				/* if d_capabilities has DRIVE_CAP_REWIND set,
				 * drive has the capability to
				 * position at beginning of recorded data
				 *	DEVICE - device error;
				 */
	int ( * do_erase )( drive_t *drivep );
				/* if d_capabilities has DRIVE_CAP_ERASE set,
				 * drive has the capability to
				 * erase: all content of media object is
				 * eradicated.
				 *	DEVICE - device error;
				 */
	int ( * do_eject_media )( drive_t *drivep );
				/* if d_capabilities has DRIVE_CAP_EJECT set,
				 * drive has capability
				 * to eject media, and will do so when called.
				 *	DEVICE - device error;
				 */
	int ( * do_get_device_class )( drive_t *drivep );
				/* returns the media class of the device
				 * (see below).
				 */
	void ( * do_display_metrics )( drive_t *drivep );
				/* use BARE mlog to print useful throughput
				 * and performance info. set to NULL if
				 * nothing to say.
				 */
	void ( * do_quit )( drive_t * drivep );
				/* tells the drive manager to de-allocate
				 * resources, INCLUDING the slave process.
				 */
};

typedef struct drive_ops drive_ops_t;

/* the drive managers are visible globally, but should be accessed sparingly.
 * valid after drive_init1() returns successfully
 */
extern drive_t **drivepp;
extern size_t drivecnt; 
extern size_t partialmax; 

/* drive capabilities - bit positions in the capabilities mask
 * DO NOT CHANGE: used in dh_capabilities field of scsi drive hdr.
 */
#define DRIVE_CAP_BSF		( 1 << 0 ) /* can backspace files */
#define DRIVE_CAP_FSF		( 1 << 1 ) /* can forwardspace files */
#define DRIVE_CAP_REWIND	( 1 << 2 ) /* can rewind */
#define DRIVE_CAP_FILES		( 1 << 4 ) /* supports multiple files */
#define DRIVE_CAP_APPEND	( 1 << 5 ) /* can append to end of rec. data */
#define DRIVE_CAP_OVERWRITE	( 1 << 6 ) /* can overwrite recorded data */
#define DRIVE_CAP_ERASE		( 1 << 6 ) /* can erase media */
#define DRIVE_CAP_NEXTMARK	( 1 << 8 ) /* can seek to a next good mark */
#define DRIVE_CAP_EJECT		( 1 << 12 ) /* can eject media */
#define DRIVE_CAP_AUTOREWIND	( 1 << 13 ) /* rewinds on media insertion */
#define DRIVE_CAP_READ		( 1 << 14 ) /* can read media */
#define DRIVE_CAP_REMOVABLE	( 1 << 15 ) /* can change media */

/* drive manager error codes - interpretation specific to and described
 * in context of use.
 */
#define DRIVE_ERROR_CORRUPTION	1
#define DRIVE_ERROR_EOF		2
#define DRIVE_ERROR_EOD		3
#define DRIVE_ERROR_EOM		4
#define DRIVE_ERROR_BOM		5
#define DRIVE_ERROR_DEVICE	6
#define DRIVE_ERROR_FORMAT	7
#define DRIVE_ERROR_MEDIA	8
#define DRIVE_ERROR_VERSION	9
#define DRIVE_ERROR_CORE	10
#define DRIVE_ERROR_TIMEOUT	11
#define DRIVE_ERROR_STOP	12
#define DRIVE_ERROR_INVAL	13
#define DRIVE_ERROR_BLANK	14
#define DRIVE_ERROR_FOREIGN	15
#define DRIVE_ERROR_OVERWRITE	16


/* drive_init1 - select and instantiate a drive manager for each drive
 * specified on the command line.
 *
 * Highly heuristic, using all avalable sources of information, including
 * the command line (command name and parameters), experimentation, and
 * drive-specific media functions, but excluding time-consuming operations
 * such as tape motion.
 *
 * sets globals drivepp and drivecnt.
 *
 * Returns FALSE if utility should be aborted.
 */
extern bool_t drive_init1( int argc, char *argv[] );


/* drive_init2 - allocate and initialize read and write hdr buffers,
 * and cause each drive manager to asynchronously determine if
 * drive has media object mounted.
 *
 * Returns FALSE if the session should be aborted.
 */
extern bool_t drive_init2( int argc,
			   char *argv[],
			   global_hdr_t *gwhdrtemplatep );


/* drive_init3 - synchronize with async activity kicked off by drive_init3.
 *
 * Returns FALSE if the session should be aborted.
 */
extern bool_t drive_init3( void );

/* drive_mark_commit - invokes callback for all drive marks committed
 * to media. ncommitted is the number of bytes actually committed to
 * media so far. mark records with a mark offset less than or equal to
 * ncommitted will have their callbacks invoked.
 */
extern void drive_mark_commit( drive_t *drivep, off64_t ncommitted );


/* drive_mark_discard - invokes callback of all uncommitted marks,
 * indicating the commit did not occur.
 */
extern void drive_mark_discard( drive_t *drivep );


/* drive_display_metrics - display drive throughput and streaming metrics
 * for all drives
 */
extern void drive_display_metrics( void );


/* device classes
 * used for determining which media driver to employ
 */
#define DEVICE_NONREMOVABLE      1
#define DEVICE_TAPE_REMOVABLE    2
#define DEVICE_DISK_REMOVABLE    3

#endif /* DRIVE_H */
