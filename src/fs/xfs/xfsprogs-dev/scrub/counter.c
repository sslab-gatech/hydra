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
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "ptvar.h"
#include "counter.h"

/*
 * Per-Thread Counters
 *
 * This is a global counter object that uses per-thread counters to
 * count things without having to content for a single shared lock.
 * Provided we know the number of threads that will be accessing the
 * counter, each thread gets its own thread-specific counter variable.
 * Changing the value is fast, though retrieving the value is expensive
 * and approximate.
 */
struct ptcounter {
	struct ptvar	*var;
};

/* Initialize per-thread counter. */
struct ptcounter *
ptcounter_init(
	size_t			nr)
{
	struct ptcounter	*p;

	p = malloc(sizeof(struct ptcounter));
	if (!p)
		return NULL;
	p->var = ptvar_init(nr, sizeof(uint64_t));
	if (!p->var) {
		free(p);
		return NULL;
	}
	return p;
}

/* Free per-thread counter. */
void
ptcounter_free(
	struct ptcounter	*ptc)
{
	ptvar_free(ptc->var);
	free(ptc);
}

/* Add a quantity to the counter. */
void
ptcounter_add(
	struct ptcounter	*ptc,
	int64_t			nr)
{
	uint64_t		*p;

	p = ptvar_get(ptc->var);
	*p += nr;
}

static bool
ptcounter_val_helper(
	struct ptvar		*ptv,
	void			*data,
	void			*foreach_arg)
{
	uint64_t		*sum = foreach_arg;
	uint64_t		*count = data;

	*sum += *count;
	return true;
}

/* Return the approximate value of this counter. */
uint64_t
ptcounter_value(
	struct ptcounter	*ptc)
{
	uint64_t		sum = 0;

	ptvar_foreach(ptc->var, ptcounter_val_helper, &sum);
	return sum;
}
