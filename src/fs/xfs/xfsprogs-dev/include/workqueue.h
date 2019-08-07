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
 * This code was adapted from repair/threads.h.
 */
#ifndef	_WORKQUEUE_H_
#define	_WORKQUEUE_H_

struct workqueue;

typedef void workqueue_func_t(struct workqueue *wq, uint32_t index, void *arg);

struct workqueue_item {
	struct workqueue	*queue;
	struct workqueue_item	*next;
	workqueue_func_t	*function;
	void			*arg;
	uint32_t		index;
};

struct workqueue {
	void			*wq_ctx;
	pthread_t		*threads;
	struct workqueue_item	*next_item;
	struct workqueue_item	*last_item;
	pthread_mutex_t		lock;
	pthread_cond_t		wakeup;
	unsigned int		item_count;
	unsigned int		thread_count;
	bool			terminate;
};

int workqueue_create(struct workqueue *wq, void *wq_ctx,
		unsigned int nr_workers);
int workqueue_add(struct workqueue *wq, workqueue_func_t fn,
		uint32_t index, void *arg);
void workqueue_destroy(struct workqueue *wq);

#endif	/* _WORKQUEUE_H_ */
