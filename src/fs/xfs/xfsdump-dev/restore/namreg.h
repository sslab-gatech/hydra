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
#ifndef NAMREG_H
#define NAMREG_H

/* namreg.[hc] - directory entry registry
 *
 * provides unique directory entry ID's and storage for the entry
 * name.
 */

/* nrh_t - handle to a registered name
 */
typedef size64_t nrh_t;
#define NRH_NULL	SIZE64MAX


/* namreg_init - creates the name registry. resync is TRUE if the
 * registry should already exist, and we are resynchronizing.
 * if NOT resync, inocnt hints at how many names will be held
 */
extern bool_t namreg_init( char *housekeepingdir,
			   bool_t resync,
			   uint64_t inocnt );


/* namreg_add - registers a name. name does not need to be null-terminated.
 * returns handle for use with namreg_get().
 */
extern nrh_t namreg_add( char *name, size_t namelen );


/* namreg_del - remove a name from the registry
 */
extern void namreg_del( nrh_t nrh );

/* namreg_map - mmap the name registry, allowing for much
 * faster namreg_get() lookups. once called, additional
 * entries cannot be added.
 */
extern rv_t namreg_map( void );

/* namreg_get - retrieves the name identified by the index.
 * fills the buffer with the null-terminated name from the registry.
 * returns the strlen() of the name. returns -1 if the buffer is too
 * small to fit the null-terminated name. return -2 if the name
 * not in the registry. return -3 if a system call fails.
 */
extern int namreg_get( nrh_t nrh, char *bufp, size_t bufsz );

#endif /* NAMREG_H */
