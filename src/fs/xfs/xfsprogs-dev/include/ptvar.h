/*
 * Copyright (C) 2018 Oracle.  All Rights Reserved.
 *
 * Author: Darrick J. Wong <darrick.wong@oracle.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef LIBFROG_PERCPU_H_
#define LIBFROG_PERCPU_H_

struct ptvar;

typedef bool (*ptvar_iter_fn)(struct ptvar *ptv, void *data, void *foreach_arg);

struct ptvar *ptvar_init(size_t nr, size_t size);
void ptvar_free(struct ptvar *ptv);
void *ptvar_get(struct ptvar *ptv);
bool ptvar_foreach(struct ptvar *ptv, ptvar_iter_fn fn, void *foreach_arg);

#endif /* LIBFROG_PERCPU_H_ */
