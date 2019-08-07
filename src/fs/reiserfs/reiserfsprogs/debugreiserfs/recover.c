/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "debugreiserfs.h"

#if 0

/* this reads stdin and recover file of given key:  */
/* the input has to be in the follwong format:
   K dirid objectid
   N name
   B blocknumber
   ..
   then recover_file will read every block, look there specified file and put it into
*/
void do_recover(reiserfs_filsys_t fs)
{
	char name[100];
	char *line = 0;
	int n = 0;
	int fd;
	struct reiserfs_key key = { 0, 0, };
	struct buffer_head *bh;
	struct item_head *ih;
	unsigned long block;
	char code;
	loff_t recovered = 0;
	int i, j;
	reiserfs_bitmap_t bitmap;
	int used, not_used;

	bitmap = reiserfs_create_bitmap(SB_BLOCK_COUNT(fs));
	reiserfs_fetch_disk_bitmap(bitmap, fs);
	/* we check how many blocks recoverd items point to are free or used */
	used = 0;
	not_used = 0;

	fd = 0;
	while (getline(&line, &n, stdin) != -1) {
		if (line[0] == '#' || line[0] == '\n')
			continue;
		switch (line[0]) {
		case 'K':
			/* get a key of file which is to be recovered */
			if (sscanf
			    (line, "%c %u %u\n", &code, &key.k_dir_id,
			     &key.k_objectid) != 3) {
				die("recover_file: wrong input K format");
			}
			printf("Recovering file (%u, %u)\n", key.k_dir_id,
			       key.k_objectid);
			break;

		case 'N':
			/* get a file name */
			recovered = 0;
			if (sscanf(line, "%c %s\n", &code, name) != 2) {
				die("recover_file: wrong input N format");
			}
			fd = open(name, O_RDWR | O_CREAT | O_EXCL, 0644);
			if (fd == -1)
				die("recover_file: could not create file %s: %s", name, strerror(errno));
			printf("Recovering file %s..\n", name);
			break;

		case 'B':
			if (!fd)
				die("recover_file: file name is not specified");
			if (sscanf(line, "%c %lu\n", &code, &block) != 2) {
				die("recover_file: wrong input B format");
			}
			bh = bread(fs->s_dev, block, fs->s_blocksize);
			if (!bh) {
				printf("reading block %lu failed\n", block);
				continue;
			}

			printf("working with block %lu..\n", block);

			ih = item_head(bh, 0);
			for (i = 0; i < node_item_number(bh); i++, ih++) {
				__le32 *indirect;
				struct buffer_head *tmp_bh;

				if (!is_indirect_ih(ih)
				    || key.k_dir_id != ih->ih_key.k_dir_id
				    || key.k_objectid != ih->ih_key.k_objectid)
					continue;

				indirect = (__u32 *) ih_item_body(bh, ih);
				for (j = 0; j < I_UNFM_NUM(ih); j++) {
					block = le32_to_cpu(indirect[j]);
					if (!block)
						continue;
					tmp_bh =
					    bread(fs->s_dev, block,
						  fs->s_blocksize);
					if (!tmp_bh) {
						printf
						    ("reading block %Lu failed\n",
						     (loff_t) block *
						     fs->s_blocksize);
						continue;
					}
					if (lseek64
					    (fd,
					     get_offset(&ih->ih_key) +
					     j * fs->s_blocksize - 1,
					     SEEK_SET) == (loff_t) - 1) {
						printf
						    ("llseek failed to pos %Ld\n",
						     (loff_t) block *
						     fs->s_blocksize);
						brelse(tmp_bh);
						continue;
					}
					if (reiserfs_bitmap_test_bit
					    (bitmap, block))
						used++;
					else
						not_used++;
					/*printf ("block of file %Ld gets block %lu\n",
					   (get_offset (&ih->ih_key) - 1) / fs->s_blocksize + j, block); */
					if (write
					    (fd, tmp_bh->b_data,
					     tmp_bh->b_size) !=
					    tmp_bh->b_size) {
						printf
						    ("write failed to pos %Ld\n",
						     (loff_t) block *
						     fs->s_blocksize);
						brelse(tmp_bh);
						continue;
					}
					recovered += fs->s_blocksize;
					brelse(tmp_bh);
				}
			}
			brelse(bh);
			break;
		}
	}
	printf
	    ("recover_file: %Ld bytes recovered of file %s, key %u %u, %d blocks are free and %d are used\n",
	     recovered, name, key.k_dir_id, key.k_objectid, not_used, used);
}
#endif

/* read a file containing map of one or more files and either recover
   them or just print info */
/*
static void read_map (FILE * fp)
{
    int i;
    __u32 v32;
    char * buf;
    __u32 ids [4];
    int do_recover = 0;

    buf = 0;
    while (1) {
	if (fread (&v32, sizeof (v32), 1, fp) != 1)
	    break;
	if (v32 != MAP_MAGIC)
	    reiserfs_panic ("read_map: no magic found");

	// device name length and name itself
	fread (&v32, sizeof (v32), 1, fp);
	buf = realloc (buf, v32);
	if (!buf)
	    reiserfs_panic ("realloc failed");
	fread (buf, v32, 1, fp);
	reiserfs_warning (stdout, "\"%s\": ", buf);

	// file name length and name itself
	fread (&v32, sizeof (v32), 1, fp);
	buf = realloc (buf, v32);
	if (!buf)
	    reiserfs_panic ("realloc failed");
	fread (buf, v32, 1, fp);

	// read directory key and poined object key
	fread (ids, sizeof (ids), 1, fp);
	reiserfs_warning (stdout, "[%K]:\"%s\"-->[%K]\n",
			  &ids[0], buf, &ids[2]);

	
	//do_recover = user_confirmed (stdout, "recover? (Y):", "Y\n");
	//if (do_recover)
	//    reiserfs_warning (stderr, "recovering not ready\n");
	

	// how many data blocks are there
	fread (&v32, sizeof (v32), 1, fp);
	if (v32) {
	    buf = realloc (buf, v32 * 4);
	    if (!buf)
		reiserfs_panic ("realloc failed (%u)", v32);
	
	    // read list of data block numbers
	    fread (buf, 4, v32, fp);
	
	    if (!do_recover) {
		for (i = 0; i < v32; i ++)
		    reiserfs_warning (stdout, "%d ", ((__u32 *)buf)[i]);
		reiserfs_warning (stdout, "\n");
	    }
	}
	
	// main tail length
	fread (&v32, sizeof (v32), 1, fp);
	if (v32) {
	    // there is tail
	    buf = realloc (buf, v32);
	    if (!buf)
		reiserfs_panic ("realloc failed");
	    fread (buf, v32, 1, fp);
	    if (!do_recover)
		reiserfs_warning (stdout, "%d bytes long tail\n", v32);
	} else {
	    if (!do_recover)
		reiserfs_warning (stdout, "No tail\n");
	}

	if (fread (&v32, sizeof (v32), 1, fp) != 1)
	    break;
	if (v32 != MAP_END_MAGIC)
	    reiserfs_panic ("read_map: no magic found");
    }

    free (buf);
}

void do_recover (reiserfs_filsys_t fs)
{
    FILE * fp;

    if (map_file (fs)) {
	fp = fopen (map_file (fs), "r");
	if (fp == 0) {
	    reiserfs_warning (stderr, "do_recover: fopen failed: %m");
	    return;
	}
    } else {
	reiserfs_warning (stderr, "Reading file map from stdin..\n");
	fflush (stderr);
	fp = stdin;
    }

    read_map (fp);

    if (fp != stdin)
	fclose (fp);

}
*/

#include <limits.h>

static long int get_answer(long int max)
{
	char *answer, *tmp;
	size_t n = 0;
	long int result = 0;

	do {
		printf("Which should be left?: ");
		getline(&answer, &n, stdin);
		result = strtol(answer, &tmp, 0);
		if ((errno != ERANGE) && (result < max) && (result >= 0) &&
		    (answer != tmp))
			break;
	} while (1);
	return result;
}

static void recover_items(FILE * fp, reiserfs_filsys_t fs, FILE * target_file)
{
	struct buffer_head *bh, *bh_pointed;
	struct item_head *ih;
	struct saved_item item, *cur;
	int size = sizeof(struct saved_item) - sizeof(struct saved_item *);
	struct saved_item *map = NULL;
	__u32 map_size = 0;
	int start = -1;
	unsigned int i, j;
	__u64 offset = 0, length;
	long int result = 0;
	unsigned long unfm_ptr;

	while (fread(&item, size, 1, fp) == 1) {
		map_size += sizeof(struct saved_item);
		map = realloc(map, map_size);
		memcpy((void *)map + map_size - sizeof(struct saved_item),
		       &item, size);
	}

	for (i = 1, cur = map + 1; i <= map_size / sizeof(struct saved_item);
	     i++, cur++) {
		bh = bread(fs->fs_dev, (cur - 1)->si_block, fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(fp, "bread failed\n");
			continue;
		}

		if (i == map_size / sizeof(struct saved_item)) {
			if (start != -1) {
				reiserfs_print_item(stdout, bh,
						    item_head(bh,
								   (cur -
								    1)->
								   si_item_num));
				result = get_answer(i - start) + start;
			} else {
				result = i - 1;
			}

			start = -1;
		} else if (is_direntry_ih(&(cur - 1)->si_ih)
			   || is_stat_data_ih(&(cur - 1)->si_ih)) {
			brelse(bh);
			continue;
		} else {
			length =
			    get_bytes_number(&(cur - 1)->si_ih,
					     fs->fs_blocksize);
			if (offset <
			    get_offset(&(cur - 1)->si_ih.ih_key) +
			    get_bytes_number(&(cur - 1)->si_ih,
					     fs->fs_blocksize))
				offset =
				    get_offset(&(cur - 1)->si_ih.ih_key) +
				    (length ? length - 1 : 0);

			if (offset >= get_offset(&cur->si_ih.ih_key)) {
				/* Problem interval */
				if (start == -1)
					start = i - 1;

				printf("Problem item %d:\n", i - start - 1);
				reiserfs_print_item(stdout, bh,
						    item_head(bh,
								   (cur -
								    1)->
								   si_item_num));
			} else if (start != -1) {
				/* problem interval finished */
				printf("Problem item %d:\n", i - start - 1);
				reiserfs_print_item(stdout, bh,
						    item_head(bh,
								   (cur -
								    1)->
								   si_item_num));

				result =
				    get_answer((long int)i - start) + start;
				start = -1;
			} else {
				result = i - 1;
			}
		}
		brelse(bh);

		if (start != -1)
			continue;

		printf("write %ld\n", result);

		bh = bread(fs->fs_dev, (map + result)->si_block,
			   fs->fs_blocksize);
		if (!bh) {
			reiserfs_warning(fp, "bread failed\n");
			continue;
		}

		fseek(target_file,
		      get_offset(&(map + result)->si_ih.ih_key) - 1, SEEK_SET);
		ih = item_head(bh, (map + result)->si_item_num);
		if (is_direct_ih(ih)) {
			fwrite(ih_item_body(bh, ih),
			       (map + result)->si_ih.ih2_item_len, 1,
			       target_file);
		} else if (is_indirect_ih(ih)) {
			for (j = 0; j < I_UNFM_NUM(ih); j++) {
				unfm_ptr =
				    d32_get((__le32 *) ih_item_body(bh, ih), j);
				if (!unfm_ptr) {
					fseek(target_file, fs->fs_blocksize,
					      SEEK_CUR);
					continue;
				}
				bh_pointed =
				    bread(fs->fs_dev, unfm_ptr,
					  fs->fs_blocksize);
				if (!bh_pointed) {
					reiserfs_warning(fp, "bread failed\n");
					continue;
				}
				fwrite(bh_pointed->b_data, fs->fs_blocksize, 1,
				       target_file);
				brelse(bh_pointed);
			}
		}
		brelse(bh);
	}

	free(map);
}

void do_recover(reiserfs_filsys_t fs)
{
	FILE *fp, *recovery;

	if (map_file(fs)) {
		fp = fopen(map_file(fs), "r");
		if (fp == 0) {
			reiserfs_warning(stderr, "fopen failed: %m\n");
			return;
		}
	} else {
		reiserfs_warning(stderr, "Reading file map from stdin..\n");
		fflush(stderr);
		fp = stdin;
	}

	if (!(recovery = fopen(recovery_file(fs), "w+"))) {
		reiserfs_warning(stderr, "fopen failed: %m\n");
		return;
	}

	recover_items(fp, fs, recovery);

	if (fp != stdin)
		fclose(fp);

	fclose(recovery);
}
