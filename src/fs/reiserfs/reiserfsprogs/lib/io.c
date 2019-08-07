/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "io.h"

#include <string.h>
#include <errno.h>
#include <asm/types.h>

void check_memory_msg(void)
{
	fprintf(stderr,
		"\nThe problem has occurred looks like a hardware problem (perhaps\n"
		"memory). Send us the bug report only if the second run dies at\n"
		"the same place with the same block number.\n");
}

static void check_hd_msg(void)
{
	fprintf(stderr,
		"\nThe problem has occurred looks like a hardware problem. If you have\n"
		"bad blocks, we advise you to get a new hard drive, because once you\n"
		"get one bad block  that the disk  drive internals  cannot hide from\n"
		"your sight,the chances of getting more are generally said to become\n"
		"much higher  (precise statistics are unknown to us), and  this disk\n"
		"drive is probably not expensive enough  for you to you to risk your\n"
		"time and  data on it.  If you don't want to follow that follow that\n"
		"advice then  if you have just a few bad blocks,  try writing to the\n"
		"bad blocks  and see if the drive remaps  the bad blocks (that means\n"
		"it takes a block  it has  in reserve  and allocates  it for use for\n"
		"of that block number).  If it cannot remap the block,  use badblock\n"
		"option (-B) with  reiserfs utils to handle this block correctly.\n");
}

static int is_bad_block(unsigned long block)
{
#ifdef IO_FAILURE_EMULATION

	/* this array similates bad blocks on the device */
	unsigned long bad_blocks[] = {
		8208, 8209, 8210
/*, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19*/
	};
	int i;

	for (i = 0; i < sizeof(bad_blocks) / sizeof(bad_blocks[0]); i++)
		if (bad_blocks[i] == block)
			return 1;

#endif

	return 0;
}

/* All buffers are in double linked cycled list.  If getblk found buffer with
   wanted block number in hash queue it moves buffer to the end of list. */

static int g_nr_buffers;

static unsigned long buffers_memory;

/* create buffers until we spend this fraction of system memory, this
** is a hard limit on the amount of buffer ram used
*/
#define BUFFER_MEMORY_FRACTION 10

/* number of bytes in local buffer cache before we start forcing syncs
** of dirty data and reusing unused buffers instead of allocating new
** ones.  If a flush doesn't find reusable buffers, new ones are
** still allocated up to the BUFFER_MEMORY_FRACTION percentage
**
*/
#define BUFFER_SOFT_LIMIT (500 * 1024)
static unsigned long buffer_soft_limit = BUFFER_SOFT_LIMIT;

#define NR_HASH_QUEUES 4096
static struct buffer_head *g_a_hash_queues[NR_HASH_QUEUES];
static struct buffer_head *Buffer_list_head;
static struct buffer_head *g_free_buffers = NULL;
static struct buffer_head *g_buffer_heads;
static int buffer_hits = 0;
static int buffer_misses = 0;
static int buffer_reads = 0;
static int buffer_writes = 0;

static void _show_buffers(struct buffer_head **list, int dev,
			  unsigned long size)
{
	int all = 0;
	int dirty = 0;
	int in_use = 0;		/* count != 0 */
	int free = 0;
	struct buffer_head *next;

	next = *list;
	if (!next)
		return;

	for (;;) {
		if (next->b_dev == dev && next->b_size == size) {
			all++;
			if (next->b_count != 0) {
				in_use++;
			}
			if (buffer_dirty(next)) {
				dirty++;
			}
			if (buffer_clean(next) && next->b_count == 0) {
				free++;
			}
		}
		next = next->b_next;
		if (next == *list)
			break;
	}

	printf
	    ("show_buffers (dev %d, size %lu): free %d, count != 0 %d, dirty %d, "
	     "all %d\n", dev, size, free, in_use, dirty, all);
}

static void show_buffers(int dev, int size)
{
	_show_buffers(&Buffer_list_head, dev, size);
	_show_buffers(&g_free_buffers, dev, size);
}

static void insert_into_hash_queue(struct buffer_head *bh)
{
	int index = bh->b_blocknr % NR_HASH_QUEUES;

	if (bh->b_hash_prev || bh->b_hash_next)
		die("insert_into_hash_queue: hash queue corrupted");

	if (g_a_hash_queues[index]) {
		g_a_hash_queues[index]->b_hash_prev = bh;
		bh->b_hash_next = g_a_hash_queues[index];
	}
	g_a_hash_queues[index] = bh;
}

static void remove_from_hash_queue(struct buffer_head *bh)
{
	if (bh->b_hash_next == NULL && bh->b_hash_prev == NULL
	    && bh != g_a_hash_queues[bh->b_blocknr % NR_HASH_QUEUES])
		/* (b_dev == -1) ? */
		return;

	if (bh == g_a_hash_queues[bh->b_blocknr % NR_HASH_QUEUES]) {
		if (bh->b_hash_prev)
			die("remove_from_hash_queue: hash queue corrupted");
		g_a_hash_queues[bh->b_blocknr % NR_HASH_QUEUES] =
		    bh->b_hash_next;
	}
	if (bh->b_hash_next)
		bh->b_hash_next->b_hash_prev = bh->b_hash_prev;

	if (bh->b_hash_prev)
		bh->b_hash_prev->b_hash_next = bh->b_hash_next;

	bh->b_hash_prev = bh->b_hash_next = NULL;
}

static void put_buffer_list_end(struct buffer_head **list,
				struct buffer_head *bh)
{
	struct buffer_head *last = NULL;

	if (bh->b_prev || bh->b_next)
		die("put_buffer_list_end: buffer list corrupted");

	if (*list == NULL) {
		bh->b_next = bh;
		bh->b_prev = bh;
		*list = bh;
	} else {
		last = (*list)->b_prev;

		bh->b_next = last->b_next;
		bh->b_prev = last;
		last->b_next->b_prev = bh;
		last->b_next = bh;
	}
}

static void remove_from_buffer_list(struct buffer_head **list,
				    struct buffer_head *bh)
{
	if (bh == bh->b_next) {
		*list = NULL;
	} else {
		bh->b_prev->b_next = bh->b_next;
		bh->b_next->b_prev = bh->b_prev;
		if (bh == *list)
			*list = bh->b_next;
	}

	bh->b_next = bh->b_prev = NULL;
}

static void put_buffer_list_head(struct buffer_head **list,
				 struct buffer_head *bh)
{
	put_buffer_list_end(list, bh);
	*list = bh;
}

/*
#include <sys/mman.h>

static size_t estimate_memory_amount (void)
{
    size_t len = 1;
    size_t max = 0;
    void * addr;

    while (len > 0) {
	addr = mmap (0, len, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (addr == MAP_FAILED) {
	    if (errno != ENOMEM)
		die ("mmap failed: %s\n", strerror(errno));
	    break;
	}
	if (mlock (addr, len) != 0) {
	    if (errno == EPERM)
		die ("No permission to run mlock");
	    break;
	}

	munlock (addr, len);
	munmap (addr, len);
	max = len;
	len *= 2;
    }

    // * If we've looped, we don't want to return 0, we want to return the
    // * last successful len before we looped. In the event that mmap/mlock
    // * failed for len = 1, max will still be 0, so we don't get an invalid
    // * result
    return max;
}
*/

#define GROW_BUFFERS__NEW_BUFERS_PER_CALL 10

/* creates number of new buffers and insert them into head of buffer list */
static int grow_buffers(int size)
{
	int i;
	struct buffer_head *bh, *tmp;

	/* get memory for array of buffer heads */
	bh = (struct buffer_head *)getmem(GROW_BUFFERS__NEW_BUFERS_PER_CALL *
					  sizeof(struct buffer_head) +
					  sizeof(struct buffer_head *));
	if (!g_buffer_heads)
		g_buffer_heads = bh;
	else {
		/* link new array to the end of array list */
		tmp = g_buffer_heads;
		while (*(struct buffer_head **)
		       (tmp + GROW_BUFFERS__NEW_BUFERS_PER_CALL) != NULL)
			tmp =
			    *(struct buffer_head **)(tmp +
						     GROW_BUFFERS__NEW_BUFERS_PER_CALL);
		*(struct buffer_head **)(tmp +
					 GROW_BUFFERS__NEW_BUFERS_PER_CALL) =
		    bh;
	}

	for (i = 0; i < GROW_BUFFERS__NEW_BUFERS_PER_CALL; i++) {

		tmp = bh + i;
		memset(tmp, 0, sizeof(struct buffer_head));
		tmp->b_data = getmem(size);
		if (!tmp->b_data)
			die("grow_buffers: no memory for new buffer data");
		tmp->b_dev = -1;
		tmp->b_size = size;
		put_buffer_list_head(&g_free_buffers, tmp);
	}
	buffers_memory += GROW_BUFFERS__NEW_BUFERS_PER_CALL * size;
	g_nr_buffers += GROW_BUFFERS__NEW_BUFERS_PER_CALL;
	return GROW_BUFFERS__NEW_BUFERS_PER_CALL;
}

struct buffer_head *find_buffer(int dev, unsigned long block,
				unsigned long size)
{
	struct buffer_head *next;

	next = g_a_hash_queues[block % NR_HASH_QUEUES];
	for (;;) {
		struct buffer_head *tmp = next;
		if (!next)
			break;
		next = tmp->b_hash_next;
		if (tmp->b_blocknr != block || tmp->b_size != size
		    || tmp->b_dev != dev)
			continue;
		next = tmp;
		break;
	}
	return next;
}

static struct buffer_head *get_free_buffer(struct buffer_head **list,
					   unsigned long size)
{
	struct buffer_head *next;

	next = *list;
	if (!next)
		return NULL;

	for (;;) {
		if (!next)
			die("get_free_buffer: buffer list is corrupted");
		if (next->b_count == 0 && buffer_clean(next)
		    && next->b_size == size) {
			remove_from_hash_queue(next);
			remove_from_buffer_list(list, next);
			return next;
		}
		next = next->b_next;
		if (next == *list)
			break;
	}
	return NULL;
}

/* to_write == 0 when all blocks have to be flushed. Otherwise - write only
   buffers with b_count == 0 */
static int sync_buffers(struct buffer_head **list, int dev, int to_write)
{
	struct buffer_head *next;
	int written = 0;

restart:
	next = *list;
	if (!next)
		return 0;
	for (;;) {
		if (!next)
			die("sync_buffers: buffer list is corrupted");

		if (next->b_dev == dev && buffer_dirty(next)
		    && buffer_uptodate(next)) {
			if ((to_write == 0 || next->b_count == 0)
			    && !buffer_do_not_flush(next)) {
				bwrite(next);
			}
		}

		/* if this buffer is reusable, put it onto the end of the free list */
		if (next->b_count == 0 && buffer_clean(next)) {
			remove_from_hash_queue(next);
			remove_from_buffer_list(list, next);
			put_buffer_list_end(&g_free_buffers, next);
			written++;
			if (written == to_write)
				return written;
			goto restart;
		}
		if (to_write && written >= to_write)
			return written;

		next = next->b_next;
		if (next == *list)
			break;
	}

	return written;
}

void flush_buffers(int dev)
{
	if (dev == -1)
		die("flush_buffers: device is not specified");

	sync_buffers(&Buffer_list_head, dev, 0 /*all */ );
	buffer_soft_limit = BUFFER_SOFT_LIMIT;
}

struct buffer_head *getblk(int dev, unsigned long block, int size)
{
	struct buffer_head *bh;

	bh = find_buffer(dev, block, size);
	if (bh) {
		/* move the buffer to the end of list */

		/*checkmem (bh->b_data, bh->b_size); */

		remove_from_buffer_list(&Buffer_list_head, bh);
		put_buffer_list_end(&Buffer_list_head, bh);
		bh->b_count++;
		buffer_hits++;
		return bh;
	}
	buffer_misses++;

	bh = get_free_buffer(&g_free_buffers, size);
	if (bh == NULL) {
		if (buffers_memory >= buffer_soft_limit) {
			if (sync_buffers(&Buffer_list_head, dev, 32) == 0) {
				grow_buffers(size);
				buffer_soft_limit = buffers_memory +
				    GROW_BUFFERS__NEW_BUFERS_PER_CALL * size;
			}
		} else {
			if (grow_buffers(size) == 0)
				sync_buffers(&Buffer_list_head, dev, 32);
		}

		bh = get_free_buffer(&g_free_buffers, size);
		if (bh == NULL) {
			show_buffers(dev, size);
			die("getblk: no free buffers after grow_buffers "
			    "and refill (%d)", g_nr_buffers);
		}
	}

	bh->b_count = 1;
	bh->b_dev = dev;
	bh->b_size = size;
	bh->b_blocknr = block;
	bh->b_end_io = NULL;
	memset(bh->b_data, 0, size);
	misc_clear_bit(BH_Dirty, &bh->b_state);
	misc_clear_bit(BH_Uptodate, &bh->b_state);

	put_buffer_list_end(&Buffer_list_head, bh);
	insert_into_hash_queue(bh);
	/*checkmem (bh->b_data, bh->b_size); */

	return bh;
}

void brelse(struct buffer_head *bh)
{
	if (!bh)
		return;

	if (bh->b_count == 0)
		die("brelse: can not free a free buffer %lu", bh->b_blocknr);

	/*checkmem (bh->b_data, get_mem_size (bh->b_data)); */

	bh->b_count--;
}

void bforget(struct buffer_head *bh)
{
	if (bh) {
		bh->b_state = 0;
		brelse(bh);
		remove_from_hash_queue(bh);
		remove_from_buffer_list(&Buffer_list_head, bh);
		put_buffer_list_head(&Buffer_list_head, bh);
	}
}

/* Returns 0 on success; 1 - end of file; 0 - OK. */
static int f_read(struct buffer_head *bh)
{
	unsigned long long offset;
	ssize_t bytes;

	buffer_reads++;

	offset = (unsigned long long)bh->b_size * bh->b_blocknr;
	if (lseek(bh->b_dev, offset, SEEK_SET) < 0)
		return -1;

	bytes = read(bh->b_dev, bh->b_data, bh->b_size);

	return bytes < 0 ? -1 : (bytes != (ssize_t) bh->b_size ? 1 : 0);
}

struct buffer_head *bread(int dev, unsigned long block, size_t size)
{
	struct buffer_head *bh;
	int ret;

	if (is_bad_block(block))
		return NULL;

	bh = getblk(dev, block, size);

	/*checkmem (bh->b_data, get_mem_size(bh->b_data)); */

	if (buffer_uptodate(bh))
		return bh;

	ret = f_read(bh);

	if (ret > 0) {
		die("%s: End of file, cannot read the block (%lu).\n",
		    __FUNCTION__, block);
	} else if (ret < 0) {
		/* BAD BLOCK LIST SUPPORT
		 * die ("%s: Cannot read a block # %lu. Specify list of badblocks\n",*/

		if (errno == EIO) {
			check_hd_msg();
			die("%s: Cannot read the block (%lu): (%s).\n",
			    __FUNCTION__, block, strerror(errno));
		} else {
			fprintf(stderr,
				"%s: Cannot read the block (%lu): (%s).\n",
				__FUNCTION__, block, strerror(errno));
			return NULL;
		}
	}

	mark_buffer_uptodate(bh, 0);
	return bh;
}

#define ROLLBACK_FILE_START_MAGIC       "_RollBackFileForReiserfsFSCK"

static struct block_handler *rollback_blocks_array;
static __u32 rollback_blocks_number = 0;
static FILE *s_rollback_file = NULL;
static FILE *log_file;
static int do_rollback = 0;

static char *rollback_data;
static int rollback_blocksize;

void init_rollback_file(char *rollback_file, unsigned int *blocksize,
			FILE * log)
{
	char *string;
	struct stat buf;

	if (rollback_file == NULL)
		return;

	stat(rollback_file, &buf);

	s_rollback_file = fopen(rollback_file, "w+");
	if (s_rollback_file == NULL) {
		fprintf(stderr,
			"Cannot create file %s, work without a rollback file\n",
			rollback_file);
		return;
	}

	rollback_blocksize = *blocksize;

	string = ROLLBACK_FILE_START_MAGIC;
	fwrite(string, 28, 1, s_rollback_file);
	fwrite(&rollback_blocksize, sizeof(rollback_blocksize), 1,
	       s_rollback_file);
	fwrite(&rollback_blocks_number, sizeof(rollback_blocks_number), 1,
	       s_rollback_file);
	fflush(s_rollback_file);

	rollback_data = getmem(rollback_blocksize);

//    printf("\ncheckmem1");
//    fflush (stdout);
//    checkmem (rollback_data, get_mem_size((char *)rollback_data));
//    printf(" OK");

	log_file = log;
	if (log_file)
		fprintf(log_file, "rollback: file (%s) initialize\n",
			rollback_file);

	do_rollback = 0;
}

#if 0
static void erase_rollback_file(char *rollback_file)
{
	close_rollback_file();
	unlink(rollback_file);
}
#endif

int open_rollback_file(char *rollback_file, FILE * log)
{
	char string[28];
	struct stat buf;

	if (rollback_file == NULL)
		return -1;

	if (stat(rollback_file, &buf)) {
		fprintf(stderr, "Cannot stat rollback file (%s)\n",
			rollback_file);
		return -1;
	}

	s_rollback_file = fopen(rollback_file, "r+");
	if (s_rollback_file == NULL) {
		fprintf(stderr, "Cannot open file (%s)\n", rollback_file);
		return -1;
	}

	fread(string, 28, 1, s_rollback_file);
	if (!strcmp(string, ROLLBACK_FILE_START_MAGIC)) {
		fprintf(stderr,
			"Specified file (%s) does not look like a rollback file\n",
			rollback_file);
		fclose(s_rollback_file);
		s_rollback_file = NULL;
		return -1;
	}

	fread(&rollback_blocksize, sizeof(rollback_blocksize), 1,
	      s_rollback_file);

	if (rollback_blocksize <= 0) {
		fprintf(stderr, "rollback: wrong rollback blocksize, exit\n");
		return -1;
	}

	log_file = log;
	if (log_file)
		fprintf(log_file, "rollback: file (%s) opened\n",
			rollback_file);

	do_rollback = 1;
	return 0;
}

void close_rollback_file(void)
{
	if (!s_rollback_file)
		return;

	if (!do_rollback) {
		if (fseek(s_rollback_file, 28 + sizeof(int), SEEK_SET) ==
		    (loff_t) - 1)
			return;
		fwrite(&rollback_blocks_number, sizeof(rollback_blocksize), 1,
		       s_rollback_file);
		if (log_file)
			fprintf(log_file, "rollback: %u blocks backed up\n",
				rollback_blocks_number);
	}

	fclose(s_rollback_file);

	freemem(rollback_data);
	freemem(rollback_blocks_array);

//    fprintf (stdout, "rollback: (%u) blocks saved, \n", rollback_blocks_number);

/*    for (i = 0; i < rollback_blocks_number; i++)
        fprintf(stdout, "device (%Lu), block number (%u)\n",
                rollback_blocks_array [i].device,
                rollback_blocks_array [i].blocknr);
    fprintf(stdout, "\n");
    */
}

void do_fsck_rollback(int fd_device, int fd_journal_device, FILE * progress)
{
	long long int offset;

	struct stat buf;
	int descriptor;
	ssize_t retval;
	int count_failed = 0;
	int count_rollbacked = 0;

	int b_dev;
	int n_dev = 0;
	int n_journal_dev = 0;
	unsigned long done = 0;

	if (fd_device == 0) {
		fprintf(stderr, "rollback: unspecified device, exit\n");
		return;
	}

	if (fd_journal_device) {
		if (!fstat(fd_journal_device, &buf)) {
			n_journal_dev = buf.st_rdev;
		} else {
			fprintf(stderr,
				"rollback: specified journal device cannot be stated\n");
		}
	}

	if (!fstat(fd_device, &buf)) {
		n_dev = buf.st_rdev;
	} else {
		fprintf(stderr,
			"rollback: specified device cannot be stated, exit\n");
		return;
	}

	rollback_data = getmem(rollback_blocksize);
//    printf("\ncheckmem2");
//    fflush (stdout);
//    checkmem (rollback_data, get_mem_size((char *)rollback_data));
//   printf(" OK");

	fread(&rollback_blocks_number, sizeof(rollback_blocks_number), 1,
	      s_rollback_file);

	while (1) {
		print_how_far(progress, &done, rollback_blocks_number, 1,
			      0 /*not quiet */ );

		descriptor = 0;
		if ((retval =
		     fread(&b_dev, sizeof(b_dev), 1, s_rollback_file)) <= 0) {
			if (retval)
				fprintf(stderr, "rollback: fread: %s\n",
					strerror(errno));
			break;
		}
		if ((retval =
		     fread(&offset, sizeof(offset), 1, s_rollback_file)) <= 0) {
			if (retval)
				fprintf(stderr, "rollback: fread: %s\n",
					strerror(errno));
			break;
		}

		if ((retval =
		     fread(rollback_data, rollback_blocksize, 1,
			   s_rollback_file)) <= 0) {
			if (retval)
				fprintf(stderr, "rollback: fread: %s\n",
					strerror(errno));
			break;
		}

		if (n_dev == b_dev)
			descriptor = fd_device;
		if ((n_journal_dev) && (n_journal_dev == b_dev))
			descriptor = fd_journal_device;

		if (descriptor == 0) {
			fprintf(stderr,
				"rollback: block from unknown device, skip block\n");
			count_failed++;
			continue;
		}

		if (lseek(descriptor, offset, SEEK_SET) == (loff_t) - 1) {
			fprintf(stderr,
				"device cannot be lseeked, skip block\n");
			count_failed++;
			continue;
		}

		if (write(descriptor, rollback_data, rollback_blocksize) == -1) {
			fprintf(stderr,
				"rollback: write %d bytes returned error "
				"(block=%lld, dev=%d): %s\n",
				rollback_blocksize, offset / rollback_blocksize,
				b_dev, strerror(errno));
			count_failed++;
		} else {
			count_rollbacked++;
			/*if you want to know what gets rollbacked, uncomment it */
/*            if (log_file != 0 && log_file != stdout)
                fprintf (log_file, "rollback: block %Lu of device %Lu was restored\n",
                        (loff_t)offset/rollback_blocksize, b_dev);
*/
//            fprintf (stdout, "rollback: block (%Ld) written\n", (loff_t)offset/rollback_blocksize);
		}
	}

	printf("\n");
	if (log_file)
		fprintf(log_file, "rollback: (%u) blocks restored\n",
			count_rollbacked);
}

/*
static void rollback__mark_block_saved (struct block_handler * rb_e) {
    if (rollback_blocks_array == NULL)
        rollback_blocks_array = getmem (ROLLBACK__INCREASE_BLOCK_NUMBER * sizeof (*rb_e));

    if (rollback_blocks_number == get_mem_size ((void *)rollback_blocks_array) / sizeof (*rb_e))
        rollback_blocks_array = expandmem (rollback_blocks_array, get_mem_size((void *)rollback_blocks_array),
                        ROLLBACK__INCREASE_BLOCK_NUMBER * sizeof (*rb_e));

//    checkmem ((char *)rollback_blocks_array, get_mem_size((char *)rollback_blocks_array));

    rollback_blocks_array[rollback_blocks_number] = *rb_e;
    rollback_blocks_number ++;
    qsort (rollback_blocks_array, rollback_blocks_number, sizeof (*rb_e), rollback_compare);

//    printf("\ncheckmem3");
//    fflush (stdout);
//   checkmem ((char *)rollback_blocks_array, get_mem_size((char *)rollback_blocks_array));
//    printf(" OK");
}
*/
/* for now - just make sure that bad blocks did not get here */
int bwrite(struct buffer_head *bh)
{
	unsigned long long offset;
	long long bytes, size;

	if (is_bad_block(bh->b_blocknr)) {
		fprintf(stderr,
			"bwrite: bad block is going to be written: %lu\n",
			bh->b_blocknr);
		exit(8);
	}

	if (!buffer_dirty(bh) || !buffer_uptodate(bh))
		return 0;

	buffer_writes++;
	if (bh->b_start_io)
		/* this is used by undo feature of reiserfsck */
		bh->b_start_io(bh->b_blocknr);

	size = bh->b_size;
	offset = (loff_t) size *(loff_t) bh->b_blocknr;

	if (lseek(bh->b_dev, offset, SEEK_SET) == (loff_t) - 1) {
		fprintf(stderr,
			"bwrite: lseek to position %llu (block=%lu, dev=%d): %s\n",
			offset, bh->b_blocknr, bh->b_dev, strerror(errno));
		exit(8);	/* File system errors left uncorrected */
	}

	if (s_rollback_file != NULL
	    && bh->b_size == (unsigned long)rollback_blocksize) {
		struct stat buf;
		__u32 position;
		struct block_handler block_h;

		/*log previous content into the log */
		if (!fstat(bh->b_dev, &buf)) {
			block_h.blocknr = bh->b_blocknr;
			block_h.device = buf.st_rdev;
			if (reiserfs_bin_search(&block_h, rollback_blocks_array,
						rollback_blocks_number,
						sizeof(block_h), &position,
						blockdev_list_compare)
			    != POSITION_FOUND) {
				/*read initial data from the disk */
				if (read(bh->b_dev, rollback_data, bh->b_size)
				    == (long long)bh->b_size) {
					fwrite(&buf.st_rdev,
					       sizeof(buf.st_rdev), 1,
					       s_rollback_file);
					fwrite(&offset, sizeof(offset), 1,
					       s_rollback_file);
					fwrite(rollback_data,
					       rollback_blocksize, 1,
					       s_rollback_file);
					fflush(s_rollback_file);
					blocklist__insert_in_position(&block_h,
								      (void
								       *)
								      (&rollback_blocks_array),
								      &rollback_blocks_number,
								      sizeof
								      (block_h),
								      &position);

					/*if you want to know what gets saved, uncomment it */
/*                    if (log_file != 0 && log_file != stdout) {
                        fprintf (log_file, "rollback: block %lu of device %Lu was "
			    "backed up\n", bh->b_blocknr, buf.st_rdev);
                    }
*/

				} else {
					fprintf(stderr,
						"bwrite: read (block=%lu, dev=%d): %s\n",
						bh->b_blocknr, bh->b_dev,
						strerror(errno));
					exit(8);
				}

				if (lseek(bh->b_dev, offset, SEEK_SET) ==
				    (loff_t) - 1) {
					fprintf(stderr,
						"bwrite: lseek to position %llu (block=%lu, "
						"dev=%d): %s\n", offset,
						bh->b_blocknr, bh->b_dev,
						strerror(errno));
					exit(8);
				}
			}
		} else {
			fprintf(stderr,
				"bwrite: fstat of (%d) returned -1: %s\n",
				bh->b_dev, strerror(errno));
		}
	} else if (s_rollback_file != NULL) {
		fprintf(stderr,
			"rollback: block (%lu) has the size different from "
			"the fs uses, block skipped\n", bh->b_blocknr);
	}

	bytes = write(bh->b_dev, bh->b_data, size);
	if (bytes != size) {
		fprintf(stderr,
			"bwrite: write %lld bytes returned %lld (block=%ld, "
			"dev=%d): %s\n", size, bytes, bh->b_blocknr, bh->b_dev,
			strerror(errno));
		exit(8);
	}

	mark_buffer_clean(bh);

	if (bh->b_end_io) {
		bh->b_end_io(bh, 1);
	}

	return 0;
}

static int _check_and_free_buffer_list(struct buffer_head *list)
{
	struct buffer_head *next = list;
	int count = 0;
	if (!list)
		return 0;

	for (;;) {
		if (next->b_count != 0)
			fprintf(stderr,
				"check_and_free_buffer_mem: not free buffer "
				"(%d, %ld, %ld, %d)\n", next->b_dev,
				next->b_blocknr, next->b_size, next->b_count);

		if (buffer_dirty(next) && buffer_uptodate(next))
			fprintf(stderr,
				"check_and_free_buffer_mem: dirty buffer "
				"(%d %lu) found\n", next->b_dev,
				next->b_blocknr);

		freemem(next->b_data);
		count++;
		next = next->b_next;
		if (next == list)
			break;
	}
	return count;
}

static void check_and_free_buffer_mem(void)
{
	int count = 0;
	struct buffer_head *next;

//    printf("check and free buffer mem, hits %d misses %d reads %d writes %d\n",
//          buffer_hits, buffer_misses, buffer_reads, buffer_writes) ;
	/*sync_buffers (0, 0); */

	count = _check_and_free_buffer_list(Buffer_list_head);
	count += _check_and_free_buffer_list(g_free_buffers);

	if (count != g_nr_buffers)
		die("check_and_free_buffer_mem: found %d buffers, must be %d",
		    count, g_nr_buffers);

	/* free buffer heads */
	while ((next = g_buffer_heads)) {
		g_buffer_heads = *(struct buffer_head **)
		    (next + GROW_BUFFERS__NEW_BUFERS_PER_CALL);

		freemem(next);
	}

	return;
}

/* */
void free_buffers(void)
{
	check_and_free_buffer_mem();
}

static void _invalidate_buffer_list(struct buffer_head *list, int dev)
{
	struct buffer_head *next;

	if (!list)
		return;

	next = list;

	for (;;) {
		if (next->b_dev == dev) {
			if (buffer_dirty(next) || next->b_count)
				fprintf(stderr,
					"invalidate_buffers: dirty buffer or used buffer (%d %lu) found\n",
					next->b_count, next->b_blocknr);
			next->b_state = 0;
			remove_from_hash_queue(next);
		}
		next = next->b_next;
		if (next == list)
			break;
	}
}

/* forget all buffers of the given device */
void invalidate_buffers(int dev)
{
	_invalidate_buffer_list(Buffer_list_head, dev);
	_invalidate_buffer_list(g_free_buffers, dev);
}
