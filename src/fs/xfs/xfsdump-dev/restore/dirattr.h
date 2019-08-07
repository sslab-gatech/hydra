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
#ifndef DIRATTR_H
#define DIRATTR_H

/* dah_t - handle to registered directory attributes
 * a special handle is reserved for the caller's convenience,
 * to indicate no directory attributes have been registered.
 */
typedef size32_t dah_t;
#define	DAH_NULL	SIZE32MAX


/* dirattr_init - creates the directory attributes registry.
 * resync indicates if an existing context should be re-opened.
 * returns FALSE if an error encountered. if NOT resync,
 * dircnt hints at number of directories to expect.
 */
extern bool_t dirattr_init( char *housekeepingdir,
			    bool_t resync,
			    uint64_t dircnt );


/* dirattr_cleanup - removes all traces
 */
extern void dirattr_cleanup( void );


/* dirattr_add - registers a directory's attributes. knows how to interpret
 * the filehdr. returns handle for use with dirattr_get_...().
 */
extern dah_t dirattr_add( filehdr_t *fhdrp );

/* dirattr_update - modifies existing registered attributes
 */
extern void dirattr_update( dah_t dah, filehdr_t *fhdrp );

/* dirattr_del - frees dirattr no longer needed
 */
extern void dirattr_del( dah_t dah );

/* dirattr_get_... - retrieve various attributes
 */
mode_t dirattr_get_mode( dah_t dah );
uid_t dirattr_get_uid( dah_t dah );
gid_t dirattr_get_gid( dah_t dah );
time32_t dirattr_get_atime( dah_t dah );
time32_t dirattr_get_mtime( dah_t dah );
time32_t dirattr_get_ctime( dah_t dah );
uint32_t dirattr_get_xflags( dah_t dah );
uint32_t dirattr_get_extsize( dah_t dah );
uint32_t dirattr_get_projid( dah_t dah );
uint32_t dirattr_get_dmevmask( dah_t dah );
uint32_t dirattr_get_dmstate( dah_t dah );

/* dirattr_flush - flush dirattr I/O buffer.  Returns 0 if successful.
 */
extern rv_t dirattr_flush( void );

/* dirattr_addextattr - record an extended attribute. second argument is
 * ptr to extattrhdr_t, with extattr name and value appended as
 * described by hdr.
 */
extern void dirattr_addextattr( dah_t dah, extattrhdr_t *ahdrp );

/* dirattr_cb_extattr - calls back for every extended attribute associated with
 * the given dah. stops iteration and returnd FALSE if cbfunc returns FALSE,
 * else returns TRUE.
 */
extern bool_t dirattr_cb_extattr( dah_t dah,
				  bool_t ( * cbfunc )( extattrhdr_t *ahdrp,
						       void *ctxp ),
				  extattrhdr_t *ahdrp,
				  void *ctxp );

#endif /* DIRATTR_H */
