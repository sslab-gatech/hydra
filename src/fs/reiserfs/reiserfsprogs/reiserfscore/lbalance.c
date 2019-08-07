/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "includes.h"

/* these are used in do_balance.c */

/* leaf_move_items
   leaf_shift_left
   leaf_shift_right
   leaf_delete_items
   leaf_insert_into_buf
   leaf_paste_in_buffer
   leaf_cut_from_buffer
   leaf_paste_entries
   */

extern struct tree_balance init_tb;
extern int init_item_pos;
extern int init_pos_in_item;
extern int init_mode;

/* copy copy_count entries from source directory item to dest buffer (creating new item if needed) */
static void leaf_copy_dir_entries(struct buffer_info *dest_bi,
				  struct buffer_head *source, int last_first,
				  int item_num, int from, int copy_count)
{
	struct buffer_head *dest = dest_bi->bi_bh;
	int item_num_in_dest;	/* either the number of target item,
				   or if we must create a new item,
				   the number of the item we will
				   create it next to */
	struct item_head *ih;
	struct reiserfs_de_head *deh;
	int copy_records_len;	/* length of all records in item to be copied */
	char *records;

	ih = item_head(source, item_num);

	/* length of all record to be copied and first byte of the last of them */
	deh = B_I_DEH(source, ih);
	if (copy_count) {
		copy_records_len =
		    (from ? get_deh_location(&deh[from - 1]) :
		     get_ih_item_len(ih)) - get_deh_location(&deh[from +
								  copy_count -
								  1]);
		records =
		    source->b_data + get_ih_location(ih) +
		    get_deh_location(&deh[from + copy_count - 1]);
	} else {
		copy_records_len = 0;
		records = NULL;
	}

	/* when copy last to first, dest buffer can contain 0 items */
	item_num_in_dest =
	    (last_first ==
	     LAST_TO_FIRST) ? ((B_NR_ITEMS(dest)) ? 0 : -1) : (B_NR_ITEMS(dest)
							       - 1);

	/* if there are no items in dest or the first/last item in dest is not item of the same directory */
	if ((item_num_in_dest == -1) ||
	    (last_first == FIRST_TO_LAST
	     && are_items_mergeable(item_head(dest, item_num_in_dest), ih,
				    dest->b_size) == 0)
	    || (last_first == LAST_TO_FIRST
		&& are_items_mergeable(ih,
				       item_head(dest, item_num_in_dest),
				       dest->b_size) == 0)) {
		/* create new item in dest */
		struct item_head new_ih;

		/* form item header */
		memcpy(&new_ih.ih_key, &ih->ih_key, KEY_SIZE);

		/* calculate item len */
		set_ih_item_len(&new_ih,
				DEH_SIZE * copy_count + copy_records_len);
		set_ih_entry_count(&new_ih, 0);

		if (last_first == LAST_TO_FIRST) {
			/* form key by the following way */
			if (from < get_ih_entry_count(ih)) {
				set_key_offset_v1(&new_ih.ih_key,
						  get_deh_offset(&deh[from]));
			} else {
				/* no entries will be copied to this item in this function */
				set_key_offset_v1(&new_ih.ih_key,
						  MAX_KEY1_OFFSET);
			}
			set_key_uniqueness(&new_ih.ih_key, DIRENTRY_UNIQUENESS);
		}
		set_ih_key_format(&new_ih, get_ih_key_format(ih));
		set_ih_flags(&new_ih, get_ih_flags(ih));

		/* insert item into dest buffer */
		leaf_insert_into_buf(dest_bi,
				     (last_first ==
				      LAST_TO_FIRST) ? 0 : B_NR_ITEMS(dest),
				     &new_ih, NULL, 0);
	} else {
		/* prepare space for entries */
		leaf_paste_in_buffer(dest_bi,
				     (last_first ==
				      FIRST_TO_LAST) ? (B_NR_ITEMS(dest) -
							1) : 0,
				     0xffff /*MAX_US_INT */ ,
				     DEH_SIZE * copy_count + copy_records_len,
				     records, 0);
	}

	item_num_in_dest =
	    (last_first == FIRST_TO_LAST) ? (B_NR_ITEMS(dest) - 1) : 0;

	leaf_paste_entries(dest_bi->bi_bh, item_num_in_dest,
			   (last_first ==
			    FIRST_TO_LAST) ?
			   get_ih_entry_count(item_head
					      (dest, item_num_in_dest)) : 0,
			   copy_count, deh + from, records,
			   DEH_SIZE * copy_count + copy_records_len);
}

/* Copy the first (if last_first == FIRST_TO_LAST) or last (last_first == LAST_TO_FIRST) item or
   part of it or nothing (see the return 0 below) from SOURCE to the end
   (if last_first) or beginning (!last_first) of the DEST */
/* returns 1 if anything was copied, else 0 */
static int leaf_copy_boundary_item(struct buffer_info *dest_bi,
				   struct buffer_head *src, int last_first,
				   int bytes_or_entries)
{
	struct buffer_head *dest = dest_bi->bi_bh;
	int dest_nr_item, src_nr_item;	/* number of items in the source and destination buffers */
	struct item_head *ih;
	struct item_head *dih;

	dest_nr_item = B_NR_ITEMS(dest);

	if (last_first == FIRST_TO_LAST) {
		/* if ( DEST is empty or first item of SOURCE and last item of DEST are the items of different objects
		   or of different types ) then there is no need to treat this item differently from the other items
		   that we copy, so we return */
		ih = item_head(src, 0);
		dih = item_head(dest, dest_nr_item - 1);
		if (!dest_nr_item
		    || (are_items_mergeable(dih, ih, src->b_size) == 0))
			/* there is nothing to merge */
			return 0;

		if (I_IS_DIRECTORY_ITEM(ih)) {
			if (bytes_or_entries == -1)
				/* copy all entries to dest */
				bytes_or_entries = get_ih_entry_count(ih);
			leaf_copy_dir_entries(dest_bi, src, FIRST_TO_LAST,
					      0, 0, bytes_or_entries);
			return 1;
		}

		/* copy part of the body of the first item of SOURCE to the end of the body of the last item of the DEST
		   part defined by 'bytes_or_entries'; if bytes_or_entries == -1 copy whole body; don't create new item header
		 */
		if (bytes_or_entries == -1)
			bytes_or_entries = get_ih_item_len(ih);

		/* merge first item (or its part) of src buffer with the last
		   item of dest buffer. Both are of the same file */
		leaf_paste_in_buffer(dest_bi, dest_nr_item - 1,
				     get_ih_item_len(dih), bytes_or_entries,
				     ih_item_body(src, ih), 0);

		if (I_IS_INDIRECT_ITEM(dih)) {
			if (bytes_or_entries == get_ih_item_len(ih))
				//dih->u.ih_free_space = ih->u.ih_free_space;
				set_ih_free_space(dih, get_ih_free_space(ih));
		}

		return 1;
	}

	/* copy boundary item to right (last_first == LAST_TO_FIRST) */

	/* ( DEST is empty or last item of SOURCE and first item of DEST
	   are the items of different object or of different types )
	 */
	src_nr_item = B_NR_ITEMS(src);
	ih = item_head(src, src_nr_item - 1);
	dih = item_head(dest, 0);

	if (!dest_nr_item || are_items_mergeable(ih, dih, src->b_size) == 0)
		return 0;

	if (I_IS_DIRECTORY_ITEM(ih)) {
		if (bytes_or_entries == -1)
			/* bytes_or_entries = entries number in last item body of SOURCE */
			bytes_or_entries = get_ih_entry_count(ih);

		leaf_copy_dir_entries(dest_bi, src, LAST_TO_FIRST,
				      src_nr_item - 1,
				      get_ih_entry_count(ih) - bytes_or_entries,
				      bytes_or_entries);
		return 1;
	}

	/* copy part of the body of the last item of SOURCE to the begin of the body of the first item of the DEST;
	   part defined by 'bytes_or_entries'; if byte_or_entriess == -1 copy whole body; change first item key of the DEST;
	   don't create new item header
	 */

	if (bytes_or_entries == -1) {
		/* bytes_or_entries = length of last item body of SOURCE */
		bytes_or_entries = get_ih_item_len(ih);

		/* change first item key of the DEST */
		//dih->ih_key.k_offset = ih->ih_key.k_offset;
		set_offset(key_format(&dih->ih_key), &dih->ih_key,
			   get_offset(&ih->ih_key));

		/* item becomes non-mergeable */
		/* or mergeable if left item was */
		//dih->ih_key.k_uniqueness = ih->ih_key.k_uniqueness;
		set_type(key_format(&dih->ih_key), &dih->ih_key,
			 get_type(&ih->ih_key));
	} else {
		/* merge to right only part of item */
		/* change first item key of the DEST */
		if (I_IS_DIRECT_ITEM(dih)) {
			//dih->ih_key.k_offset -= bytes_or_entries;
			set_offset(key_format(&dih->ih_key), &dih->ih_key,
				   get_offset(&dih->ih_key) - bytes_or_entries);
		} else {
			//dih->ih_key.k_offset -= ((bytes_or_entries/UNFM_P_SIZE)*dest->b_size);
			set_offset(key_format(&dih->ih_key), &dih->ih_key,
				   get_offset(&dih->ih_key) -
				   ((bytes_or_entries / UNFM_P_SIZE) *
				    dest->b_size));
		}
	}

	leaf_paste_in_buffer(dest_bi, 0, 0, bytes_or_entries,
			     ih_item_body(src,
				       ih) + get_ih_item_len(ih) -
			     bytes_or_entries, 0);
	return 1;
}

/* copy cpy_mun items from buffer src to buffer dest
 * last_first == FIRST_TO_LAST means, that we copy cpy_num  items beginning from first-th item in src to tail of dest
 * last_first == LAST_TO_FIRST means, that we copy cpy_num  items beginning from first-th item in src to head of dest
 */
static void leaf_copy_items_entirely(struct buffer_info *dest_bi,
				     struct buffer_head *src, int last_first,
				     int first, int cpy_num)
{
	struct buffer_head *dest;
	int nr;
	int dest_before;
	int last_loc, last_inserted_loc, location;
	int i, j;
	struct block_head *blkh;
	struct item_head *ih;

	dest = dest_bi->bi_bh;

	if (cpy_num == 0)
		return;

	blkh = B_BLK_HEAD(dest);
	nr = get_blkh_nr_items(blkh);

	/* we will insert items before 0-th or nr-th item in dest buffer. It depends of last_first parameter */
	dest_before = (last_first == LAST_TO_FIRST) ? 0 : nr;

	/* location of head of first new item */
	ih = item_head(dest, dest_before);

	/* prepare space for headers */
	memmove(ih + cpy_num, ih, (nr - dest_before) * IH_SIZE);

	/* copy item headers */
	memcpy(ih, item_head(src, first), cpy_num * IH_SIZE);

	set_blkh_free_space(blkh,
			    get_blkh_free_space(blkh) - IH_SIZE * cpy_num);

	/* location of unmovable item */
	j = location =
	    (dest_before == 0) ? dest->b_size : get_ih_location(ih - 1);
	for (i = dest_before; i < nr + cpy_num; i++) {
		location -= get_ih_item_len(&ih[i - dest_before]);
		set_ih_location(&ih[i - dest_before], location);
	}

	/* prepare space for items */
	last_loc = get_ih_location(&ih[nr + cpy_num - 1 - dest_before]);
	last_inserted_loc = get_ih_location(&ih[cpy_num - 1]);

	/* check free space */
	memmove(dest->b_data + last_loc,
		dest->b_data + last_loc + j - last_inserted_loc,
		last_inserted_loc - last_loc);

	/* copy items */
	memcpy(dest->b_data + last_inserted_loc,
	       item_body(src, (first + cpy_num - 1)), j - last_inserted_loc);

	/* sizes, item number */
	set_blkh_nr_items(blkh, get_blkh_nr_items(blkh) + cpy_num);
	set_blkh_free_space(blkh,
			    get_blkh_free_space(blkh) - (j -
							 last_inserted_loc));

	mark_buffer_dirty(dest);

	if (dest_bi->bi_parent) {
		struct disk_child *dc;
		dc = B_N_CHILD(dest_bi->bi_parent, dest_bi->bi_position);
		set_dc_child_size(dc,
				  get_dc_child_size(dc) + j -
				  last_inserted_loc + IH_SIZE * cpy_num);
		mark_buffer_dirty(dest_bi->bi_parent);
	}
}

/* This function splits the (liquid) item into two items (useful when
   shifting part of an item into another node.) */
static void leaf_item_bottle(struct buffer_info *dest_bi,
			     struct buffer_head *src, int last_first,
			     int item_num, int cpy_bytes)
{
	struct buffer_head *dest = dest_bi->bi_bh;
	struct item_head *ih;

	if (last_first == FIRST_TO_LAST) {
		/* if ( if item in position item_num in buffer SOURCE is directory item ) */
		if (I_IS_DIRECTORY_ITEM(ih = item_head(src, item_num)))
			leaf_copy_dir_entries(dest_bi, src, FIRST_TO_LAST,
					      item_num, 0, cpy_bytes);
		else {
			struct item_head n_ih;

			/* copy part of the body of the item number 'item_num' of SOURCE to the end of the DEST
			   part defined by 'cpy_bytes'; create new item header; change old item_header (????);
			   n_ih = new item_header;
			 */
			memcpy(&n_ih, ih, IH_SIZE);
			set_ih_item_len(&n_ih, cpy_bytes);
			if (I_IS_INDIRECT_ITEM(ih)) {
				//n_ih.u.ih_free_space = 0;
				set_ih_free_space(&n_ih, 0);;
			}
			//n_ih.ih_version = ih->ih_version;
			set_ih_key_format(&n_ih, get_ih_key_format(ih));
			set_ih_flags(&n_ih, get_ih_flags(ih));
			leaf_insert_into_buf(dest_bi, B_NR_ITEMS(dest),
					     &n_ih, item_body(src, item_num),
					     0);
		}
	} else {
		/*  if ( if item in position item_num in buffer SOURCE is directory item ) */
		if (I_IS_DIRECTORY_ITEM(ih = item_head(src, item_num)))
			leaf_copy_dir_entries(dest_bi, src, LAST_TO_FIRST,
					      item_num,
					      get_ih_entry_count(ih) -
					      cpy_bytes, cpy_bytes);
		else {
			struct item_head n_ih;

			/* copy part of the body of the item number 'item_num' of SOURCE to the begin of the DEST
			   part defined by 'cpy_bytes'; create new item header;
			   n_ih = new item_header;
			 */
			memcpy(&n_ih, ih, SHORT_KEY_SIZE);

			if (I_IS_DIRECT_ITEM(ih)) {
				//n_ih.ih_key.k_offset = ih->ih_key.k_offset + ih->ih_item_len - cpy_bytes;
				set_offset(key_format(&ih->ih_key),
					   &n_ih.ih_key,
					   get_offset(&ih->ih_key) +
					   get_ih_item_len(ih) - cpy_bytes);
				//n_ih.ih_key.k_uniqueness = TYPE_DIRECT;
				set_type(key_format(&ih->ih_key), &n_ih.ih_key,
					 TYPE_DIRECT);
				//n_ih.u.ih_free_space = USHRT_MAX;
				set_ih_free_space(&n_ih, USHRT_MAX);
			} else {
				/* indirect item */
				//n_ih.ih_key.k_offset = ih->ih_key.k_offset + (ih->ih_item_len - cpy_bytes) / UNFM_P_SIZE * dest->b_size;
				set_offset(key_format(&ih->ih_key),
					   &n_ih.ih_key,
					   get_offset(&ih->ih_key) +
					   (get_ih_item_len(ih) -
					    cpy_bytes) / UNFM_P_SIZE *
					   dest->b_size);
				//n_ih.ih_key.k_uniqueness = TYPE_INDIRECT;
				set_type(key_format(&ih->ih_key), &n_ih.ih_key,
					 TYPE_INDIRECT);
				//n_ih.u.ih_free_space = ih->u.ih_free_space;
				set_ih_free_space(&n_ih, get_ih_free_space(ih));
			}

			/* set item length */
			set_ih_item_len(&n_ih, cpy_bytes);
			//n_ih.ih_version = ih->ih_version;
			set_ih_key_format(&n_ih, get_ih_key_format(ih));
			set_ih_flags(&n_ih, get_ih_flags(ih));
			leaf_insert_into_buf(dest_bi, 0, &n_ih,
					     item_body(src,
						       item_num) +
					     get_ih_item_len(ih) - cpy_bytes,
					     0);
		}
	}
}

/* If cpy_bytes equals minus one than copy cpy_num whole items from SOURCE to DEST.
   If cpy_bytes not equal to minus one than copy cpy_num-1 whole items from SOURCE to DEST.
   From last item copy cpy_num bytes for regular item and cpy_num directory entries for
   directory item. */
static int leaf_copy_items(struct buffer_info *dest_bi, struct buffer_head *src,
			   int last_first, int cpy_num, int cpy_bytes)
{
	int pos, i, src_nr_item, bytes;

	if (cpy_num == 0)
		return 0;

	if (last_first == FIRST_TO_LAST) {
		/* copy items to left */
		pos = 0;
		if (cpy_num == 1)
			bytes = cpy_bytes;
		else
			bytes = -1;

		/* copy the first item or it part or nothing to the end of the DEST (i = leaf_copy_boundary_item(DEST,SOURCE,0,bytes)) */
		i = leaf_copy_boundary_item(dest_bi, src, FIRST_TO_LAST,
					    bytes);
		cpy_num -= i;
		if (cpy_num == 0)
			return i;
		pos += i;
		if (cpy_bytes == -1)
			/* copy first cpy_num items starting from position 'pos' of SOURCE to end of DEST */
			leaf_copy_items_entirely(dest_bi, src,
						 FIRST_TO_LAST, pos, cpy_num);
		else {
			/* copy first cpy_num-1 items starting from position 'pos-1' of the SOURCE to the end of the DEST */
			leaf_copy_items_entirely(dest_bi, src,
						 FIRST_TO_LAST, pos,
						 cpy_num - 1);

			/* copy part of the item which number is cpy_num+pos-1 to the end of the DEST */
			leaf_item_bottle(dest_bi, src, FIRST_TO_LAST,
					 cpy_num + pos - 1, cpy_bytes);
		}
	} else {
		/* copy items to right */
		src_nr_item = B_NR_ITEMS(src);
		if (cpy_num == 1)
			bytes = cpy_bytes;
		else
			bytes = -1;

		/* copy the last item or it part or nothing to the begin of the DEST (i = leaf_copy_boundary_item(DEST,SOURCE,1,bytes)); */
		i = leaf_copy_boundary_item(dest_bi, src, LAST_TO_FIRST,
					    bytes);

		cpy_num -= i;
		if (cpy_num == 0)
			return i;

		pos = src_nr_item - cpy_num - i;
		if (cpy_bytes == -1) {
			/* starting from position 'pos' copy last cpy_num items of SOURCE to begin of DEST */
			leaf_copy_items_entirely(dest_bi, src,
						 LAST_TO_FIRST, pos, cpy_num);
		} else {
			/* copy last cpy_num-1 items starting from position 'pos+1' of the SOURCE to the begin of the DEST; */
			leaf_copy_items_entirely(dest_bi, src,
						 LAST_TO_FIRST, pos + 1,
						 cpy_num - 1);

			/* copy part of the item which number is pos to the begin of the DEST */
			leaf_item_bottle(dest_bi, src, LAST_TO_FIRST, pos,
					 cpy_bytes);
		}
	}
	return i;
}

/* there are types of coping: from S[0] to L[0], from S[0] to R[0],
   from R[0] to L[0]. for each of these we have to define parent and
   positions of destination and source buffers */
static void leaf_define_dest_src_infos(int shift_mode, struct tree_balance *tb,
				       struct buffer_info *dest_bi,
				       struct buffer_info *src_bi,
				       int *first_last,
				       struct buffer_head *Snew)
{
	/* define dest, src, dest parent, dest position */
	switch (shift_mode) {
	case LEAF_FROM_S_TO_L:	/* it is used in leaf_shift_left */
		buffer_info_init_last(tb, src_bi);
		buffer_info_init_left(tb, dest_bi, 0);
		*first_last = FIRST_TO_LAST;
		break;

	case LEAF_FROM_S_TO_R:	/* it is used in leaf_shift_right */
		buffer_info_init_last(tb, src_bi);
		buffer_info_init_right(tb, dest_bi, 0);
		*first_last = LAST_TO_FIRST;
		break;

	case LEAF_FROM_R_TO_L:	/* it is used in balance_leaf_when_delete */
		buffer_info_init_right(tb, src_bi, 0);
		buffer_info_init_left(tb, dest_bi, 0);
		*first_last = FIRST_TO_LAST;
		break;

	case LEAF_FROM_L_TO_R:	/* it is used in balance_leaf_when_delete */
		buffer_info_init_left(tb, src_bi, 0);
		buffer_info_init_right(tb, dest_bi, 0);
		*first_last = LAST_TO_FIRST;
		break;

	case LEAF_FROM_S_TO_SNEW:
		buffer_info_init_last(tb, src_bi);
		buffer_info_init_bh(tb, dest_bi, Snew);
		*first_last = LAST_TO_FIRST;
		break;

	default:
		reiserfs_panic(0,
			       "vs-10250: leaf_define_dest_src_infos: shift type is unknown (%d)",
			       shift_mode);
	}
}

/* copy mov_num items and mov_bytes of the (mov_num-1)th item to
   neighbor. Delete them from source */
int leaf_move_items(int shift_mode, struct tree_balance *tb,
		    int mov_num, int mov_bytes, struct buffer_head *Snew)
{
	int ret_value;
	struct buffer_info dest_bi, src_bi;
	int first_last;

	leaf_define_dest_src_infos(shift_mode, tb, &dest_bi, &src_bi,
				   &first_last, Snew);

	ret_value = leaf_copy_items(&dest_bi, src_bi.bi_bh, first_last,
				    mov_num, mov_bytes);

	leaf_delete_items(&src_bi, first_last,
			  (first_last ==
			   FIRST_TO_LAST) ? 0 : (B_NR_ITEMS(src_bi.bi_bh) -
						 mov_num), mov_num, mov_bytes);

	return ret_value;
}

/* Shift shift_num items (and shift_bytes of last shifted item if shift_bytes != -1)
   from S[0] to L[0] and replace the delimiting key */
int leaf_shift_left(struct tree_balance *tb, int shift_num, int shift_bytes)
{
	struct buffer_head *S0 = PATH_PLAST_BUFFER(tb->tb_path);
	int i;

	/* move shift_num (and shift_bytes bytes) items from S[0] to left neighbor L[0] */
	i = leaf_move_items(LEAF_FROM_S_TO_L, tb, shift_num, shift_bytes, NULL);

	if (shift_num) {
		if (B_NR_ITEMS(S0) == 0) {
			/* everything is moved from S[0] */
			if (PATH_H_POSITION(tb->tb_path, 1) == 0)
				replace_key(tb->tb_fs, tb->CFL[0], tb->lkey[0],
					    PATH_H_PPARENT(tb->tb_path, 0), 0);
		} else {
			/* replace lkey in CFL[0] by 0-th key from S[0]; */
			replace_key(tb->tb_fs, tb->CFL[0], tb->lkey[0], S0, 0);

		}
	}

	return i;
}

/* CLEANING STOPPED HERE */

/* Shift shift_num (shift_bytes) items from S[0] to the right neighbor, and replace the delimiting key */
int leaf_shift_right(struct tree_balance *tb, int shift_num, int shift_bytes)
{
	int ret_value;

	/* move shift_num (and shift_bytes) items from S[0] to right neighbor R[0] */
	ret_value =
	    leaf_move_items(LEAF_FROM_S_TO_R, tb, shift_num, shift_bytes, NULL);

	/* replace rkey in CFR[0] by the 0-th key from R[0] */
	if (shift_num) {
		replace_key(tb->tb_fs, tb->CFR[0], tb->rkey[0], tb->R[0], 0);
	}

	return ret_value;
}

static void leaf_delete_items_entirely(struct buffer_info *bi,
				       int first, int del_num);
/*  If del_bytes == -1, starting from position 'first' delete del_num items in whole in buffer CUR.
    If not.
    If last_first == 0. Starting from position 'first' delete del_num-1 items in whole. Delete part of body of
    the first item. Part defined by del_bytes. Don't delete first item header
    If last_first == 1. Starting from position 'first+1' delete del_num-1 items in whole. Delete part of body of
    the last item . Part defined by del_bytes. Don't delete last item header.
*/
void leaf_delete_items(struct buffer_info *cur_bi, int last_first, int first,
		       int del_num, int del_bytes)
{
	struct buffer_head *bh = cur_bi->bi_bh;;
	int item_amount = B_NR_ITEMS(bh);

	if (del_num == 0)
		return;

	if (first == 0 && del_num == item_amount && del_bytes == -1) {
		make_empty_node(cur_bi);
		mark_buffer_dirty(bh);
		return;
	}

	if (del_bytes == -1)
		/* delete del_num items beginning from item in position first */
		leaf_delete_items_entirely(cur_bi, first, del_num);
	else {
		if (last_first == FIRST_TO_LAST) {
			/* delete del_num-1 items beginning from item in position first  */
			leaf_delete_items_entirely(cur_bi, first,
						   del_num - 1);

			/* delete the part of the first item of the bh do not
			   delete item header */
			leaf_cut_from_buffer(cur_bi, 0, 0, del_bytes);
		} else {
			struct item_head *ih;
			int len;

			/* delete del_num-1 items beginning from item in position first+1  */
			leaf_delete_items_entirely(cur_bi, first + 1,
						   del_num - 1);

			if (I_IS_DIRECTORY_ITEM
			    (ih = item_head(bh, B_NR_ITEMS(bh) - 1)))
				/* the last item is directory  */
				/* len = numbers of directory entries in this item */
				len = get_ih_entry_count(ih);
			else
				/* len = body len of item */
				len = get_ih_item_len(ih);

			/* delete the part of the last item of the bh
			   do not delete item header
			 */
			leaf_cut_from_buffer(cur_bi, B_NR_ITEMS(bh) - 1,
					     len - del_bytes, del_bytes);
		}
	}
}

/* insert item into the leaf node in position before */
void leaf_insert_into_buf(struct buffer_info *bi,
			  int before,
			  struct item_head *inserted_item_ih,
			  const char *inserted_item_body, int zeros_number)
{
	struct buffer_head *bh = bi->bi_bh;
	int nr;
	struct block_head *blkh;
	struct item_head *ih;
	int i;
	int last_loc, unmoved_loc;
	char *to;

	blkh = B_BLK_HEAD(bh);
	nr = get_blkh_nr_items(blkh);

	/* get item new item must be inserted before */
	ih = item_head(bh, before);

	/* prepare space for the body of new item */
	last_loc = nr ? get_ih_location(&ih[nr - before - 1]) : bh->b_size;
	unmoved_loc = before ? get_ih_location(ih - 1) : bh->b_size;

	memmove(bh->b_data + last_loc - get_ih_item_len(inserted_item_ih),
		bh->b_data + last_loc, unmoved_loc - last_loc);

	to = bh->b_data + unmoved_loc - get_ih_item_len(inserted_item_ih);
	memset(to, 0, zeros_number);
	to += zeros_number;

	/* copy body to prepared space */
	if (inserted_item_body)
		//if (mem_mode == REISERFS_USER_MEM)
		//  copy_from_user (to, inserted_item_body, inserted_item_ih->ih_item_len - zeros_number);
		//else {
		memmove(to, inserted_item_body,
			get_ih_item_len(inserted_item_ih) - zeros_number);
	//}
	else
		memset(to, '\0',
		       get_ih_item_len(inserted_item_ih) - zeros_number);

	/* insert item header */
	memmove(ih + 1, ih, IH_SIZE * (nr - before));
	memmove(ih, inserted_item_ih, IH_SIZE);

	/* change locations */
	for (i = before; i < nr + 1; i++) {
		unmoved_loc -= get_ih_item_len(&ih[i - before]);
		set_ih_location(&ih[i - before], unmoved_loc);
	}

	/* sizes, free space, item number */
	set_blkh_nr_items(blkh, get_blkh_nr_items(blkh) + 1);
	set_blkh_free_space(blkh, get_blkh_free_space(blkh) -
			    (IH_SIZE + get_ih_item_len(inserted_item_ih)));

	mark_buffer_dirty(bh);

	if (bi->bi_parent) {
		struct disk_child *dc;

		dc = B_N_CHILD(bi->bi_parent, bi->bi_position);
		set_dc_child_size(dc,
				  get_dc_child_size(dc) + IH_SIZE +
				  get_ih_item_len(inserted_item_ih));
		mark_buffer_dirty(bi->bi_parent);
	}

	if (is_a_leaf(bh->b_data, bh->b_size) != THE_LEAF)
		reiserfs_panic("leaf_insert_into_buf: bad leaf %lu: %b",
			       bh->b_blocknr, bh);
}

/* paste paste_size bytes to affected_item_num-th item.
   When item is a directory, this only prepare space for new entries */
void leaf_paste_in_buffer(struct buffer_info *bi,
			  int affected_item_num,
			  int pos_in_item,
			  int paste_size, const char *body, int zeros_number)
{
	struct buffer_head *bh = bi->bi_bh;
	int nr;
	struct block_head *blkh;
	struct item_head *ih;
	int i;
	int last_loc, unmoved_loc;

	blkh = B_BLK_HEAD(bh);
	nr = get_blkh_nr_items(blkh);

	/* item to be appended */
	ih = item_head(bh, affected_item_num);

	last_loc = get_ih_location(&ih[nr - affected_item_num - 1]);
	unmoved_loc = affected_item_num ? get_ih_location(ih - 1) : bh->b_size;

	/* prepare space */
	memmove(bh->b_data + last_loc - paste_size, bh->b_data + last_loc,
		unmoved_loc - last_loc);

	/* change locations */
	for (i = affected_item_num; i < nr; i++)
		set_ih_location(&ih[i - affected_item_num],
				get_ih_location(&ih[i - affected_item_num]) -
				paste_size);

	if (body) {
		if (!I_IS_DIRECTORY_ITEM(ih)) {
			//if (mem_mode == REISERFS_USER_MEM) {
			//memset (bh->b_data + unmoved_loc - paste_size, 0, zeros_number);
			//copy_from_user (bh->b_data + unmoved_loc - paste_size + zeros_number, body, paste_size - zeros_number);
			//} else
			{
				if (!pos_in_item) {
					/* shift data to right */
					memmove(bh->b_data +
						get_ih_location(ih) +
						paste_size,
						bh->b_data +
						get_ih_location(ih),
						get_ih_item_len(ih));
					/* paste data in the head of item */
					memset(bh->b_data + get_ih_location(ih),
					       0, zeros_number);
					memcpy(bh->b_data +
					       get_ih_location(ih) +
					       zeros_number, body,
					       paste_size - zeros_number);
				} else {
					memset(bh->b_data + unmoved_loc -
					       paste_size, 0, zeros_number);
					memcpy(bh->b_data + unmoved_loc -
					       paste_size + zeros_number, body,
					       paste_size - zeros_number);
				}
			}
		}
	} else
		memset(bh->b_data + unmoved_loc - paste_size, '\0', paste_size);

	set_ih_item_len(ih, get_ih_item_len(ih) + paste_size);

	/* change free space */
	set_blkh_free_space(blkh, get_blkh_free_space(blkh) - paste_size);

	mark_buffer_dirty(bh);

	if (bi->bi_parent) {
		struct disk_child *dc;

		dc = B_N_CHILD(bi->bi_parent, bi->bi_position);
		set_dc_child_size(dc, get_dc_child_size(dc) + paste_size);
		mark_buffer_dirty(bi->bi_parent);
	}
	if (is_a_leaf(bh->b_data, bh->b_size) != THE_LEAF)
		reiserfs_panic("leaf_paste_in_buffer: bad leaf %lu: %b",
			       bh->b_blocknr, bh);
}

/* cuts DEL_COUNT entries beginning from FROM-th entry. Directory item
   does not have free space, so it moves DEHs and remaining records as
   necessary. Return value is size of removed part of directory item
   in bytes. */
static int leaf_cut_entries(struct buffer_head *bh,
			    struct item_head *ih, int from, int del_count)
{
	char *item;
	struct reiserfs_de_head *deh;
	int prev_record_offset;	/* offset of record, that is (from-1)th */
	char *prev_record;	/* */
	int cut_records_len;	/* length of all removed records */
	int i;
	int entry_count;

	/* first byte of item */
	item = ih_item_body(bh, ih);

	/* entry head array */
	deh = B_I_DEH(bh, ih);
	entry_count = get_ih_entry_count(ih);

	if (del_count == 0) {
		int shift;
		int last_location;

		last_location = get_deh_location(deh + entry_count - 1);
		shift = last_location - DEH_SIZE * entry_count;

		memmove(deh + entry_count, item + last_location,
			get_ih_item_len(ih) - last_location);
		for (i = 0; i < entry_count; i++)
			set_deh_location(&deh[i],
					 get_deh_location(&deh[i]) - shift);

		return shift;
	}

	/* first byte of remaining entries, those are BEFORE cut entries
	   (prev_record) and length of all removed records (cut_records_len) */
	prev_record_offset =
	    (from ? get_deh_location(&deh[from - 1]) : get_ih_item_len(ih));
	cut_records_len =
	    prev_record_offset /*from_record */  -
	    get_deh_location(&deh[from + del_count - 1]);
	prev_record = item + prev_record_offset;

	/* adjust locations of remaining entries */
	for (i = get_ih_entry_count(ih) - 1; i > from + del_count - 1; i--) {
		set_deh_location(deh + i,
				 get_deh_location(deh + i) -
				 (DEH_SIZE * del_count));
	}

	for (i = 0; i < from; i++) {
		set_deh_location(deh + i,
				 get_deh_location(deh + i) -
				 (DEH_SIZE * del_count + cut_records_len));
	}

	set_ih_entry_count(ih, get_ih_entry_count(ih) - del_count);

	/* shift entry head array and entries those are AFTER removed entries */
	memmove((char *)(deh + from),
		deh + from + del_count,
		prev_record - cut_records_len - (char *)(deh + from +
							 del_count));

	/* shift records, those are BEFORE removed entries */
	memmove(prev_record - cut_records_len - DEH_SIZE * del_count,
		prev_record, item + get_ih_item_len(ih) - prev_record);

	return DEH_SIZE * del_count + cut_records_len;
}

/*  when cut item is part of regular file
        pos_in_item - first byte that must be cut
        cut_size - number of bytes to be cut beginning from pos_in_item

   when cut item is part of directory
        pos_in_item - number of first deleted entry
        cut_size - count of deleted entries
    */
void leaf_cut_from_buffer(struct buffer_info *bi, int cut_item_num,
			  int pos_in_item, int cut_size)
{
	int nr;
	struct buffer_head *bh = bi->bi_bh;
	struct block_head *blkh;
	struct item_head *ih;
	int last_loc, unmoved_loc;
	int i;

	blkh = B_BLK_HEAD(bh);
	nr = get_blkh_nr_items(blkh);

	/* item head of truncated item */
	ih = item_head(bh, cut_item_num);

	if (I_IS_DIRECTORY_ITEM(ih)) {
		/* first cut entry () */
		cut_size = leaf_cut_entries(bh, ih, pos_in_item, cut_size);
		if (pos_in_item == 0) {
			/* change item key by key of first entry in the item */
			set_key_offset_v1(&ih->ih_key,
					  get_deh_offset(B_I_DEH(bh, ih)));
			/*memcpy (&ih->ih_key.k_offset, &(B_I_DEH (bh, ih)->deh_offset), SHORT_KEY_SIZE); */
		}
	} else {
		/* item is direct or indirect */
		/* shift item body to left if cut is from the head of item */
		if (pos_in_item == 0) {
			memmove(bh->b_data + get_ih_location(ih),
				bh->b_data + get_ih_location(ih) + cut_size,
				get_ih_item_len(ih) - cut_size);

			/* change key of item */
			if (I_IS_DIRECT_ITEM(ih)) {
				//ih->ih_key.k_offset += cut_size;
				set_offset(key_format(&ih->ih_key), &ih->ih_key,
					   get_offset(&ih->ih_key) + cut_size);
			} else {
				//ih->ih_key.k_offset += (cut_size / UNFM_P_SIZE) * bh->b_size;
				set_offset(key_format(&ih->ih_key), &ih->ih_key,
					   get_offset(&ih->ih_key) +
					   (cut_size / UNFM_P_SIZE) *
					   bh->b_size);
			}
		}
	}

	/* location of the last item */
	last_loc = get_ih_location(&ih[nr - cut_item_num - 1]);

	/* location of the item, which is remaining at the same place */
	unmoved_loc = cut_item_num ? get_ih_location(ih - 1) : bh->b_size;

	/* shift */
	memmove(bh->b_data + last_loc + cut_size, bh->b_data + last_loc,
		unmoved_loc - last_loc - cut_size);

	/* change item length */
	set_ih_item_len(ih, get_ih_item_len(ih) - cut_size);

	if (I_IS_INDIRECT_ITEM(ih)) {
		if (pos_in_item)
			//ih->u.ih_free_space = 0;
			set_ih_free_space(ih, 0);
	}

	/* change locations */
	for (i = cut_item_num; i < nr; i++) {
		set_ih_location(&ih[i - cut_item_num],
				get_ih_location(&ih[i - cut_item_num]) +
				cut_size);
	}

	/* size, free space */
	set_blkh_free_space(blkh, get_blkh_free_space(blkh) + cut_size);

	mark_buffer_dirty(bh);

	if (bi->bi_parent) {
		struct disk_child *dc;

		dc = B_N_CHILD(bi->bi_parent, bi->bi_position);
		set_dc_child_size(dc, get_dc_child_size(dc) - cut_size);
		mark_buffer_dirty(bi->bi_parent);
	}
	if (is_a_leaf(bh->b_data, bh->b_size) != THE_LEAF &&
	    is_a_leaf(bh->b_data, bh->b_size) != HAS_IH_ARRAY)
		reiserfs_panic("leaf_cut_from_buffer: bad leaf %lu: %b",
			       bh->b_blocknr, bh);
}

/* delete del_num items from buffer starting from the first'th item */
static void leaf_delete_items_entirely(struct buffer_info *bi,
				       int first, int del_num)
{
	struct buffer_head *bh = bi->bi_bh;
	int nr;
	int i, j;
	int last_loc, last_removed_loc;
	struct block_head *blkh;
	struct item_head *ih;

	if (del_num == 0)
		return;

	blkh = B_BLK_HEAD(bh);
	nr = get_blkh_nr_items(blkh);

	if (first == 0 && del_num == nr) {
		/* this does not work */
		make_empty_node(bi);

		mark_buffer_dirty(bh);
		return;
	}

	ih = item_head(bh, first);

	/* location of unmovable item */
	j = (first == 0) ? bh->b_size : get_ih_location(ih - 1);

	/* delete items */
	last_loc = get_ih_location(&ih[nr - 1 - first]);
	last_removed_loc = get_ih_location(&ih[del_num - 1]);

	memmove(bh->b_data + last_loc + j - last_removed_loc,
		bh->b_data + last_loc, last_removed_loc - last_loc);

	/* delete item headers */
	memmove(ih, ih + del_num, (nr - first - del_num) * IH_SIZE);

	/* change item location */
	for (i = first; i < nr - del_num; i++) {
		set_ih_location(&ih[i - first],
				get_ih_location(&ih[i - first]) + j -
				last_removed_loc);
	}

	/* sizes, item number */
	set_blkh_nr_items(blkh, get_blkh_nr_items(blkh) /*nr */ -del_num);
	set_blkh_free_space(blkh,
			    get_blkh_free_space(blkh) + j - last_removed_loc +
			    IH_SIZE * del_num);

	mark_buffer_dirty(bh);

	if (bi->bi_parent) {
		struct disk_child *dc;

		dc = B_N_CHILD(bi->bi_parent, bi->bi_position);
		set_dc_child_size(dc, get_dc_child_size(dc) -
				  (j - last_removed_loc + IH_SIZE * del_num));
		mark_buffer_dirty(bi->bi_parent);
	}
	if (is_a_leaf(bh->b_data, bh->b_size) != THE_LEAF &&
	    is_a_leaf(bh->b_data, bh->b_size) != HAS_IH_ARRAY)
		reiserfs_panic("leaf_delete_items_entirely: bad leaf %lu: %b",
			       bh->b_blocknr, bh);
}

/* paste new_entry_count entries (new_dehs, records) into position before to item_num-th item */
void leaf_paste_entries(struct buffer_head *bh,
			int item_num, int before, int new_entry_count,
			struct reiserfs_de_head *new_dehs,
			const char *records, int paste_size)
{
	struct item_head *ih;
	char *item;
	struct reiserfs_de_head *deh;
	char *insert_point;
	int i, old_entry_num;

	if (new_entry_count == 0)
		return;

	ih = item_head(bh, item_num);

	/* first byte of dest item */
	item = ih_item_body(bh, ih);

	/* entry head array */
	deh = B_I_DEH(bh, ih);

	/* new records will be pasted at this point */
	insert_point =
	    item +
	    (before ? get_deh_location(&deh[before - 1])
	     : (get_ih_item_len(ih) - paste_size));

	/* adjust locations of records that will be AFTER new records */
	for (i = get_ih_entry_count(ih) - 1; i >= before; i--)
		set_deh_location(deh + i,
				 get_deh_location(deh + i) +
				 DEH_SIZE * new_entry_count);

	/* adjust locations of records that will be BEFORE new records */
	for (i = 0; i < before; i++)
		set_deh_location(deh + i,
				 get_deh_location(deh + i) + paste_size);

	old_entry_num = get_ih_entry_count(ih);
	//ih_entry_count(ih) += new_entry_count;
	set_ih_entry_count(ih, old_entry_num + new_entry_count);

	/* prepare space for pasted records */
	memmove(insert_point + paste_size, insert_point,
		item + (get_ih_item_len(ih) - paste_size) - insert_point);

	/* copy new records */
	memcpy(insert_point + DEH_SIZE * new_entry_count, records,
	       paste_size - DEH_SIZE * new_entry_count);

	/* prepare space for new entry heads */
	deh += before;
	memmove((char *)(deh + new_entry_count), deh,
		insert_point - (char *)deh);

	/* copy new entry heads */
	memcpy(deh, new_dehs, DEH_SIZE * new_entry_count);

	/* set locations of new records */
	for (i = 0; i < new_entry_count; i++)
		set_deh_location(deh + i,
				 get_deh_location(deh + i) +
				 (-get_deh_location
				  (&new_dehs[new_entry_count - 1]) +
				  insert_point + DEH_SIZE * new_entry_count -
				  item));

	/* change item key if neccessary (when we paste before 0-th entry */
	if (!before)
		set_key_offset_v1(&ih->ih_key, get_deh_offset(new_dehs));
}

/* wrappers for operations on one separated node */

void delete_item(reiserfs_filsys_t fs, struct buffer_head *bh, int item_num)
{
	struct buffer_info bi;

	buffer_info_init_bh(NULL, &bi, bh);
	bi.bi_fs = fs;

	leaf_delete_items_entirely(&bi, item_num, 1);
}

void cut_entry(reiserfs_filsys_t fs, struct buffer_head *bh,
	       int item_num, int entry_num, int del_count)
{
	struct buffer_info bi;

	buffer_info_init_bh(NULL, &bi, bh);
	bi.bi_fs = fs;

	leaf_cut_from_buffer(&bi, item_num, entry_num, del_count);
}
