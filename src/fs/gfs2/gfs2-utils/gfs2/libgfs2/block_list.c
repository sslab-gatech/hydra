#include "clusterautoconfig.h"

#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "libgfs2.h"

void gfs2_special_free(struct special_blocks *blist)
{
	struct special_blocks *f;

	while(!osi_list_empty(&blist->list)) {
		f = osi_list_entry(blist->list.next, struct special_blocks,
				   list);
		osi_list_del(&f->list);
		free(f);
	}
}

struct special_blocks *blockfind(struct special_blocks *blist, uint64_t num)
{
	osi_list_t *head = &blist->list;
	osi_list_t *tmp;
	struct special_blocks *b;

	for (tmp = head->next; tmp != head; tmp = tmp->next) {
		b = osi_list_entry(tmp, struct special_blocks, list);
		if (b->block == num)
			return b;
	}
	return NULL;
}

void gfs2_special_add(struct special_blocks *blocklist, uint64_t block)
{
	struct special_blocks *b;

	b = malloc(sizeof(struct special_blocks));
	if (b) {
		memset(b, 0, sizeof(*b));
		b->block = block;
		osi_list_add_prev(&b->list, &blocklist->list);
	}
}

void gfs2_special_set(struct special_blocks *blocklist, uint64_t block)
{
	if (blockfind(blocklist, block))
		return;
	gfs2_special_add(blocklist, block);
}

void gfs2_special_clear(struct special_blocks *blocklist, uint64_t block)
{
	struct special_blocks *b;

	b = blockfind(blocklist, block);
	if (b) {
		osi_list_del(&b->list);
		free(b);
	}
}
