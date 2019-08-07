/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

/* when --check fsck builds a map of objectids of files it finds in the tree
   when --rebuild-tree - fsck builds map of objectids it inserts into tree */

#define ALLOC_SIZE			1024
#define MAX_ID				(~(__u32)0)

/* 2 bytes for the counter */
#define BM_SIZE				(ALLOC_SIZE - sizeof(__u16))
#define BM_INTERVAL			(BM_SIZE * 8)
#define INDEX_COUNT			((MAX_ID / BM_INTERVAL) + 1)

#define id_map_interval(map, id)	(map->index + (id / BM_INTERVAL))

#define id_map_local_count(interval)	(interval + BM_SIZE)

/*
typedef struct sb_id_map {
    __u32 * m_begin;
    __u32 m_size, m_used_slot_count;
} sb_id_map_t;
*/

id_map_t *id_map_init()
{
	id_map_t *map;
	__u32 i;

	map = getmem(sizeof(id_map_t));
	map->index = mem_alloc(INDEX_COUNT * sizeof(void *));

	for (i = 0; i < INDEX_COUNT; i++) {
		if (map->index[i] != (void *)0)
			map->index[i] = (void *)0;
	}

	id_map_mark(map, 0);
	id_map_mark(map, 1);

	/* id == 0 should not be there, just for convinient usage */
	map->count--;

	return map;
}

void id_map_free(id_map_t *map)
{
	__u32 i;

	for (i = 0; i < INDEX_COUNT; i++) {
		if (map->index[i] != (void *)0 && map->index[i] != (void *)1)
			freemem(map->index[i]);
	}

	freemem(map->index);
	freemem(map);
}

int id_map_test(id_map_t *map, __u32 id)
{
	void **interval = id_map_interval(map, id);

	if (*interval == (void *)0)
		return 0;

	if (*interval == (void *)1)
		return 1;

	return misc_test_bit(id % BM_INTERVAL, *interval);
}

int id_map_mark(id_map_t *map, __u32 id)
{
	void **interval = id_map_interval(map, id);

	if (*interval == (void *)0)
		*interval = getmem(ALLOC_SIZE);

	if (*interval == (void *)1)
		return 1;

	if (misc_test_bit(id % BM_INTERVAL, *interval))
		return 1;

	misc_set_bit(id % BM_INTERVAL, *interval);

	(*(__u16 *) id_map_local_count(*interval))++;
	map->count++;

	if ((*(__u16 *) id_map_local_count(*interval)) == BM_INTERVAL) {
		/* Dealloc fully used bitmap */
		freemem(*interval);
		*interval = (void *)1;
	}

	if (map->last_used < (id / BM_INTERVAL))
		map->last_used = id / BM_INTERVAL;

	return 0;
}

/* call this for proper_id_map only!! */
__u32 id_map_alloc(id_map_t *map)
{
	__u32 i, zero_count;
	__u32 id = 0, first = ~(__u32) 0;

	for (i = 0, zero_count = 0; zero_count < 10 && i < INDEX_COUNT - 1; i++) {
		if (map->index[i] == (void *)0) {
			if (zero_count == 0)
				first = i;

			zero_count++;
		} else if (map->index[i] != (void *)1)
			break;
	}

	if (map->index[i] != (void *)1 && map->index[i] != (void *)0) {
		id = misc_find_first_zero_bit(map->index[i], BM_INTERVAL);
		if (id >= BM_INTERVAL)
			die("Id is out of interval size, interval looks corrupted.");

		id += i * BM_INTERVAL;
	} else if (first != ~(__u32) 0) {
		id = first * BM_INTERVAL;
		if (id == 0)
			id = 2;
	} else
		die("%s: No more free objectid is available.", __FUNCTION__);

	id_map_mark(map, id);

	return id;
}

/* this could be used if some more sofisticated flushing will be needed. */
/*
static void sb_id_map_pack(sb_id_map_t *map) {
    map->m_begin[1] = map->m_begin[map->m_used_slot_count - 1];
    memset(map->m_begin + 2, 0, map->m_used_slot_count - 2);
    map->m_used_slot_count = 2;
}*/

static __u32 id_map_next_bound(id_map_t *map, __u32 start)
{
	__u32 index = start / BM_INTERVAL;
	__u32 offset = start % BM_INTERVAL;
	int look_for;

	if (map->index[index] == (void *)0)
		look_for = 1;
	else if (map->index[index] == (void *)1)
		look_for = 0;
	else
		look_for = !misc_test_bit(offset, map->index[index]);

	offset++;

start_again:

	if (look_for) {
		while (index < INDEX_COUNT && map->index[index] == (void *)0)
			index++;

		if (index == INDEX_COUNT)
			return 0;

		if (map->index[index] == (void *)1)
			return index * BM_INTERVAL;

		offset =
		    misc_find_next_set_bit(map->index[index], BM_INTERVAL,
					   offset);

		if (offset >= BM_INTERVAL) {
			offset = 0;
			index++;
			goto start_again;
		}

		return index * BM_INTERVAL + offset;
	} else {
		while (index < INDEX_COUNT && map->index[index] == (void *)1)
			index++;

		if (index == INDEX_COUNT)
			return 0;

		if (map->index[index] == (void *)0)
			return index * BM_INTERVAL;

		offset =
		    misc_find_next_zero_bit(map->index[index], BM_INTERVAL,
					    offset);

		if (offset >= BM_INTERVAL) {
			offset = 0;
			index++;
			goto start_again;
		}

		return index * BM_INTERVAL + offset;
	}
}

void id_map_flush(struct id_map *map, reiserfs_filsys_t fs)
{
	int size, max, i;
	__u32 id, prev_id;
	__le32 *sb_objectid_map;

	size = reiserfs_super_block_size(fs->fs_ondisk_sb);
	sb_objectid_map = (__le32 *) ((char *)(fs->fs_ondisk_sb) + size);

	max = ((fs->fs_blocksize - size) >> 3 << 1);
	set_sb_oid_maxsize(fs->fs_ondisk_sb, max);

	id = 1;
	sb_objectid_map[0] = cpu_to_le32(1);

	for (i = 1; i < max - 1; i++) {
		id = id_map_next_bound(map, id);
		sb_objectid_map[i] = cpu_to_le32(id);
		if (id == 0) {
			if (i % 2)
				die("%s: Used interval is not closed on flushing.", __FUNCTION__);
			break;
		}
	}

	if (map->index[map->last_used] == (void *)0)
		die("Object id map looks corrupted - last used interval cannot be zeroed.");

	i++;

	if (i == max) {
		if (id == 0)
			die("Objectid interval does not contain any set bit what is expected.");

		if (map->index[map->last_used] == (void *)1) {
			prev_id = BM_INTERVAL - 1;
		} else {
			prev_id = ~(__u32) 0;

			if (id < map->last_used * BM_INTERVAL)
				id = 0;
			else
				id %= BM_INTERVAL;

			if (misc_test_bit(id, map->index[map->last_used]))
				prev_id = id;

			while ((id =
				misc_find_next_set_bit(map->
						       index[map->last_used],
						       BM_INTERVAL,
						       (id + 1))) !=
			       BM_INTERVAL) {
				prev_id = id;
			}

			if (prev_id == ~(__u32) 0)
				die("Objectid interval does not contain any set bit what is expected.");

			prev_id++;
		}

		sb_objectid_map[max - 1] =
		    cpu_to_le32(prev_id + map->last_used * BM_INTERVAL);
	} else {
		i--;
		memset(sb_objectid_map + i, 0, (max - i) * sizeof(__u32));
	}

	set_sb_oid_cursize(fs->fs_ondisk_sb, i);
}

/* FIXME: these 3 methods must be implemented also.

void fetch_objectid_map (struct id_map * map, reiserfs_filsys_t fs)
{
    int sb_size;
    __le32 * sb_objectid_map;

    sb_size = reiserfs_super_block_size (fs->fs_ondisk_sb);
    sb_objectid_map = (__le32 *)((char *)(fs->fs_ondisk_sb) + sb_size);

    if (map->m_page_count != 1)
	die ("fetch_objectid_map: can not fetch long map");

    make_id_space(map, 0);
    memcpy (map->m_begin, sb_objectid_map, get_sb_oid_cursize (fs->fs_ondisk_sb) * sizeof (__u32));
    map->m_used_slot_count = get_sb_oid_cursize (fs->fs_ondisk_sb);
}

#define OBJMAP_START_MAGIC 375331
#define OBJMAP_END_MAGIC 7700472

void reiserfs_objectid_map_save (FILE * fp, struct id_map * id_map)
{
    __u32 v;
    int i;

    v = OBJMAP_START_MAGIC;
    fwrite (&v, 4, 1, fp);

    v = id_map->m_used_slot_count;
    fwrite (&v, 4, 1, fp);

    for (i = 0; i < id_map->m_page_count - 1; i++) {
	fwrite ((char *)id_map->m_begin + MAP_SIZE * i, 4, 1, fp);
    }

    for (i = 0; i < id_map->m_used_slot_count * sizeof(__u32) - (id_map->m_page_count - 1) * MAP_SIZE; i++) {
	fwrite ((char *)id_map->m_begin + MAP_SIZE * (id_map->m_page_count - 1) + i, 4, 1, fp);
    }

    v = OBJMAP_END_MAGIC;
    fwrite (&v, 4, 1, fp);
}

struct id_map * reiserfs_objectid_map_load (FILE * fp)
{
    __u32 v;
    int i;
    struct id_map * id_map;

    fread (&v, 4, 1, fp);
    if (v != OBJMAP_START_MAGIC) {
	reiserfs_warning (stderr, "reiserfs_objectid_map_load: no objectid map begin magic found");
	return 0;
    }
	
    // read bit size of objectid map
    fread (&v, 4, 1, fp);

    id_map = init_id_map (MAP_NOT_PACKED);

    id_map->m_used_slot_count = v;
    id_map->m_page_count = v / MAP_SIZE + 1;

    id_map->m_begin = expandmem (id_map->m_begin, 0, id_map->m_page_count * MAP_SIZE);

    for (i = 0; i < id_map->m_page_count - 1; i++) {
	fread ((char *)id_map->m_begin + MAP_SIZE * i, 4, 1, fp);
    }

    for (i = 0; i < id_map->m_used_slot_count * sizeof(__u32) - (id_map->m_page_count - 1) * MAP_SIZE; i++) {
	fread ((char *)id_map->m_begin + MAP_SIZE * (id_map->m_page_count - 1) + i, 4, 1, fp);
    }

    fread (&v, 4, 1, fp);

    if (v != OBJMAP_END_MAGIC) {
	reiserfs_warning (stderr, "reiserfs_objectid_map_load: no objectid map end magic found");
	return 0;
    }

    fflush (stderr);
    return id_map;
}
*/
