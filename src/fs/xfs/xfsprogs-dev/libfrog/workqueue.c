/*
 * Copyright (C) 2017 Oracle.  All Rights Reserved.
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
 *
 * This code was adapted from repair/threads.c, which (at the time)
 * did not contain a copyright statement.
 */
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include "workqueue.h"

/* Main processing thread */
static void *
workqueue_thread(void *arg)
{
	struct workqueue	*wq = arg;
	struct workqueue_item	*wi;

	/*
	 * Loop pulling work from the passed in work queue.
	 * Check for notification to exit after every chunk of work.
	 */
	while (1) {
		pthread_mutex_lock(&wq->lock);

		/*
		 * Wait for work.
		 */
		while (wq->next_item == NULL && !wq->terminate) {
			assert(wq->item_count == 0);
			pthread_cond_wait(&wq->wakeup, &wq->lock);
		}
		if (wq->next_item == NULL && wq->terminate) {
			pthread_mutex_unlock(&wq->lock);
			break;
		}

		/*
		 *  Dequeue work from the head of the list.
		 */
		assert(wq->item_count > 0);
		wi = wq->next_item;
		wq->next_item = wi->next;
		wq->item_count--;

		pthread_mutex_unlock(&wq->lock);

		(wi->function)(wi->queue, wi->index, wi->arg);
		free(wi);
	}

	return NULL;
}

/* Allocate a work queue and threads. */
int
workqueue_create(
	struct workqueue	*wq,
	void			*wq_ctx,
	unsigned int		nr_workers)
{
	unsigned int		i;
	int			err = 0;

	memset(wq, 0, sizeof(*wq));
	pthread_cond_init(&wq->wakeup, NULL);
	pthread_mutex_init(&wq->lock, NULL);

	wq->wq_ctx = wq_ctx;
	wq->thread_count = nr_workers;
	wq->threads = malloc(nr_workers * sizeof(pthread_t));
	wq->terminate = false;

	for (i = 0; i < nr_workers; i++) {
		err = pthread_create(&wq->threads[i], NULL, workqueue_thread,
				wq);
		if (err)
			break;
	}

	if (err)
		workqueue_destroy(wq);
	return err;
}

/*
 * Create a work item consisting of a function and some arguments and
 * schedule the work item to be run via the thread pool.
 */
int
workqueue_add(
	struct workqueue	*wq,
	workqueue_func_t	func,
	uint32_t		index,
	void			*arg)
{
	struct workqueue_item	*wi;

	if (wq->thread_count == 0) {
		func(wq, index, arg);
		return 0;
	}

	wi = malloc(sizeof(struct workqueue_item));
	if (wi == NULL)
		return ENOMEM;

	wi->function = func;
	wi->index = index;
	wi->arg = arg;
	wi->queue = wq;
	wi->next = NULL;

	/* Now queue the new work structure to the work queue. */
	pthread_mutex_lock(&wq->lock);
	if (wq->next_item == NULL) {
		wq->next_item = wi;
		assert(wq->item_count == 0);
		pthread_cond_signal(&wq->wakeup);
	} else {
		wq->last_item->next = wi;
	}
	wq->last_item = wi;
	wq->item_count++;
	pthread_mutex_unlock(&wq->lock);

	return 0;
}

/*
 * Wait for all pending work items to be processed and tear down the
 * workqueue.
 */
void
workqueue_destroy(
	struct workqueue	*wq)
{
	unsigned int		i;

	pthread_mutex_lock(&wq->lock);
	wq->terminate = 1;
	pthread_mutex_unlock(&wq->lock);

	pthread_cond_broadcast(&wq->wakeup);

	for (i = 0; i < wq->thread_count; i++)
		pthread_join(wq->threads[i], NULL);

	free(wq->threads);
	pthread_mutex_destroy(&wq->lock);
	pthread_cond_destroy(&wq->wakeup);
	memset(wq, 0, sizeof(*wq));
}
