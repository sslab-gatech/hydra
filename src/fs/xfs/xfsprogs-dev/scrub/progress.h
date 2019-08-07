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
#ifndef XFS_SCRUB_PROGRESS_H_
#define XFS_SCRUB_PROGRESS_H_

#define CLEAR_EOL	"\033[K"
#define START_IGNORE	'\001'
#define END_IGNORE	'\002'

bool progress_init_phase(struct scrub_ctx *ctx, FILE *progress_fp,
			 unsigned int phase, uint64_t max, int rshift,
			 unsigned int nr_threads);
void progress_end_phase(void);
void progress_add(uint64_t x);

#endif /* XFS_SCRUB_PROGRESS_H_ */
