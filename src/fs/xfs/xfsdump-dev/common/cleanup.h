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
#ifndef CLEANUP_H
#define CLEANUP_H

/* cleanup.[hc] - allows registration of callbacks to be invoked
 * on exit
 */

typedef void cleanup_t;

extern void cleanup_init( void );

extern cleanup_t *cleanup_register( void ( *funcp )( void *arg1, void *arg2 ),
				    void *arg1,
				    void *arg2 );

extern cleanup_t *cleanup_register_early( 
				    void ( *funcp )( void *arg1, void *arg2 ),
				    void *arg1,
				    void *arg2 );

extern void cleanup_cancel( cleanup_t *cleanupp );

extern void cleanup( void );
extern void cleanup_early( void );

#endif /* CLEANUP_H */
