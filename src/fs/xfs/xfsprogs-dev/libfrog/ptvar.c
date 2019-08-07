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
#include <unistd.h>
#include "platform_defs.h"
#include "ptvar.h"

/*
 * Per-thread Variables
 *
 * This data structure manages a lockless per-thread variable.  We
 * implement this by allocating an array of memory regions, and as each
 * thread tries to acquire its own region, we hand out the array
 * elements to each thread.  This way, each thread gets its own
 * cacheline and (after the first access) doesn't have to contend for a
 * lock for each access.
 */
struct ptvar {
	pthread_key_t	key;
	pthread_mutex_t	lock;
	size_t		nr_used;
	size_t		nr_counters;
	size_t		data_size;
	unsigned char	data[0];
};
#define PTVAR_SIZE(nr, sz) (sizeof(struct ptvar) + ((nr) * (size)))

/* Initialize per-thread counter. */
struct ptvar *
ptvar_init(
	size_t		nr,
	size_t		size)
{
	struct ptvar	*ptv;
	int		ret;

#ifdef _SC_LEVEL1_DCACHE_LINESIZE
	/* Try to prevent cache pingpong by aligning to cacheline size. */
	size = max(size, sysconf(_SC_LEVEL1_DCACHE_LINESIZE));
#endif

	ptv = malloc(PTVAR_SIZE(nr, size));
	if (!ptv)
		return NULL;
	ptv->data_size = size;
	ptv->nr_counters = nr;
	ptv->nr_used = 0;
	memset(ptv->data, 0, nr * size);
	ret = pthread_mutex_init(&ptv->lock, NULL);
	if (ret)
		goto out;
	ret = pthread_key_create(&ptv->key, NULL);
	if (ret)
		goto out_mutex;
	return ptv;

out_mutex:
	pthread_mutex_destroy(&ptv->lock);
out:
	free(ptv);
	return NULL;
}

/* Free per-thread counter. */
void
ptvar_free(
	struct ptvar	*ptv)
{
	pthread_key_delete(ptv->key);
	pthread_mutex_destroy(&ptv->lock);
	free(ptv);
}

/* Get a reference to this thread's variable. */
void *
ptvar_get(
	struct ptvar	*ptv)
{
	void		*p;

	p = pthread_getspecific(ptv->key);
	if (!p) {
		pthread_mutex_lock(&ptv->lock);
		assert(ptv->nr_used < ptv->nr_counters);
		p = &ptv->data[(ptv->nr_used++) * ptv->data_size];
		pthread_setspecific(ptv->key, p);
		pthread_mutex_unlock(&ptv->lock);
	}
	return p;
}

/* Iterate all of the per-thread variables. */
bool
ptvar_foreach(
	struct ptvar	*ptv,
	ptvar_iter_fn	fn,
	void		*foreach_arg)
{
	size_t		i;
	bool		ret = true;

	pthread_mutex_lock(&ptv->lock);
	for (i = 0; i < ptv->nr_used; i++) {
		ret = fn(ptv, &ptv->data[i * ptv->data_size], foreach_arg);
		if (!ret)
			break;
	}
	pthread_mutex_unlock(&ptv->lock);

	return ret;
}
