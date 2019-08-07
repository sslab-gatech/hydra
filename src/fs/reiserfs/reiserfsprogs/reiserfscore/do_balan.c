/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

/* Now we have all buffers that must be used in balancing of the tree 	*/
/* Further calculations can not cause schedule(), and thus the buffer 	*/
/* tree will be stable until the balancing will be finished 		*/
/* balance the tree according to the analysis made before,		*/
/* and using buffers obtained after all above.				*/

/**
 ** balance_leaf_when_delete
 ** balance_leaf
 ** do_balance
 **
 **/

#include "includes.h"

/* summary:
 if deleting something ( tb->insert_size[0] < 0 )
   return(balance_leaf_when_delete()); (flag d handled here)
 else
   if lnum is larger than 0 we put items into the left node
   if rnum is larger than 0 we put items into the right node
   if snum1 is larger than 0 we put items into the new node s1
   if snum2 is larger than 0 we put items into the new node s2
Note that all *num* count new items being created.

It would be easier to read balance_leaf() if each of these summary
lines was a separate procedure rather than being inlined.  I think
that there are many passages here and in balance_leaf_when_delete() in
which two calls to one procedure can replace two passages, and it
might save cache space and improve software maintenance costs to do so.

Vladimir made the perceptive comment that we should offload most of
the decision making in this function into fix_nodes/check_balance, and
then create some sort of structure in tb that says what actions should
be performed by do_balance.

-Hans */

/* Balance leaf node in case of delete or cut: insert_size[0] < 0
 *
 * lnum, rnum can have values >= -1
 *	-1 means that the neighbor must be joined with S
 *	 0 means that nothing should be done with the neighbor
 *	>0 means to shift entirely or partly the specified number of items to the neighbor
 */
static int balance_leaf_when_delete(	/*struct reiserfs_transaction_handle *th, */
					   struct tree_balance *tb, int flag)
{
	struct buffer_head *tbS0 = PATH_PLAST_BUFFER(tb->tb_path);
	int item_pos = PATH_LAST_POSITION(tb->tb_path);
	int pos_in_item = tb->tb_path->pos_in_item;
	struct buffer_info bi;
	int n;
	struct item_head *ih;

	ih = item_head(tbS0, item_pos);
	buffer_info_init_tbS0(tb, &bi);

	/* Delete or truncate the item */

	switch (flag) {
	case M_DELETE:		/* delete item in S[0] */

		leaf_delete_items(&bi, 0, item_pos, 1, -1);

		if (!item_pos) {
			// we have removed first item in the node - update left delimiting key
			if (B_NR_ITEMS(tbS0)) {
				replace_key(tb->tb_fs, tb->CFL[0], tb->lkey[0],
					    tbS0, 0);
			} else {
				if (!PATH_H_POSITION(tb->tb_path, 1))
					replace_key(tb->tb_fs, tb->CFL[0],
						    tb->lkey[0],
						    PATH_H_PPARENT(tb->tb_path,
								   0), 0);
			}
		}

		break;

	case M_CUT:{		/* cut item in S[0] */
			if (I_IS_DIRECTORY_ITEM(ih)) {
				/* UFS unlink semantics are such that you can only delete
				   one directory entry at a time. */
				/* when we cut a directory tb->insert_size[0] means number
				   of entries to be cut (always 1) */
				tb->insert_size[0] = -1;
				leaf_cut_from_buffer(&bi, item_pos,
						     pos_in_item,
						     -tb->insert_size[0]);

				if (!item_pos && !pos_in_item) {
					replace_key(tb->tb_fs, tb->CFL[0],
						    tb->lkey[0], tbS0, 0);
				}
			} else {
				leaf_cut_from_buffer(&bi, item_pos,
						     pos_in_item,
						     -tb->insert_size[0]);
			}
			break;
		}

	default:
		print_tb(flag, item_pos, pos_in_item, tb, "when_del");
		reiserfs_panic
		    ("PAP-12040: balance_leaf_when_delete: unexpectable mode: %s(%d)",
		     (flag ==
		      M_PASTE) ? "PASTE" : ((flag ==
					     M_INSERT) ? "INSERT" : "UNKNOWN"),
		     flag);
	}

	/* the rule is that no shifting occurs unless by shifting a node can be freed */
	n = B_NR_ITEMS(tbS0);
	if (tb->lnum[0]) {	/* L[0] takes part in balancing */
		if (tb->lnum[0] == -1) {	/* L[0] must be joined with S[0] */
			if (tb->rnum[0] == -1) {	/* R[0] must be also joined with S[0] */
				if (tb->FR[0] == PATH_H_PPARENT(tb->tb_path, 0)) {
					/* all contents of all the 3 buffers will be in L[0] */
					if (PATH_H_POSITION(tb->tb_path, 1) == 0
					    && 1 < B_NR_ITEMS(tb->FR[0]))
						replace_key(tb->tb_fs,
							    tb->CFL[0],
							    tb->lkey[0],
							    tb->FR[0], 1);

					leaf_move_items(LEAF_FROM_S_TO_L, tb, n,
							-1, 0);
					leaf_move_items(LEAF_FROM_R_TO_L, tb,
							B_NR_ITEMS(tb->R[0]),
							-1, 0);

					reiserfs_invalidate_buffer(tb, tbS0);
					reiserfs_invalidate_buffer(tb, tb->R[0]);

					return 0;
				}
				/* all contents of all the 3 buffers will be in R[0] */
				leaf_move_items(LEAF_FROM_S_TO_R, tb, n, -1, NULL);
				leaf_move_items(LEAF_FROM_L_TO_R, tb,
						B_NR_ITEMS(tb->L[0]), -1, NULL);

				/* right_delimiting_key is correct in R[0] */
				replace_key(tb->tb_fs, tb->CFR[0], tb->rkey[0],
					    tb->R[0], 0);

				reiserfs_invalidate_buffer(tb, tbS0);
				reiserfs_invalidate_buffer(tb, tb->L[0]);

				return -1;
			}

			/* all contents of L[0] and S[0] will be in L[0] */
			leaf_shift_left(tb, n, -1);

			reiserfs_invalidate_buffer(tb, tbS0);

			return 0;
		}
		/* a part of contents of S[0] will be in L[0] and the rest part of S[0] will be in R[0] */

		leaf_shift_left(tb, tb->lnum[0], tb->lbytes);
		leaf_shift_right(tb, tb->rnum[0], tb->rbytes);

		reiserfs_invalidate_buffer(tb, tbS0);

		return 0;
	}

	if (tb->rnum[0] == -1) {
		/* all contents of R[0] and S[0] will be in R[0] */
		leaf_shift_right(tb, n, -1);
		reiserfs_invalidate_buffer(tb, tbS0);
		return 0;
	}
	return 0;
}

static int balance_leaf(	/*struct reiserfs_transaction_handle *th, */
			       struct tree_balance *tb,	/* see reiserfs_fs.h */
			       struct item_head *ih,	/* item header of inserted item */
			       const char *body,	/* body  of inserted item or bytes to paste */
			       int flag,	/* i - insert, d - delete, c - cut, p - paste
						   (see comment to do_balance) */
			       int zeros_number,	/* it is always 0 */
			       struct item_head *insert_key,	/* in our processing of one level we sometimes determine what
								   must be inserted into the next higher level.  This insertion
								   consists of a key or two keys and their corresponding
								   pointers */
			       struct buffer_head **insert_ptr	/* inserted node-ptrs for the next level */
    )
{
	int pos_in_item = tb->tb_path->pos_in_item;	/* position in item, in bytes for direct and
							   indirect items, in entries for directories (for
							   which it is an index into the array of directory
							   entry headers.) */
	struct buffer_head *tbS0 = PATH_PLAST_BUFFER(tb->tb_path);
/*  struct buffer_head * tbF0 = PATH_H_PPARENT (tb->tb_path, 0);
    int S0_b_item_order = PATH_H_B_ITEM_ORDER (tb->tb_path, 0);*/
	int item_pos = PATH_LAST_POSITION(tb->tb_path);	/*  index into the array of item headers in S[0]
							   of the affected item */
	struct buffer_info bi;
	struct buffer_head *S_new[2];	/* new nodes allocated to hold what could not fit into S */
	int snum[2];		/* number of items that will be placed into S_new (includes partially shifted items) */
	int sbytes[2];		/* if an item is partially shifted into S_new then
				   if it is a directory item
				   it is the number of entries from the item that are shifted into S_new
				   else
				   it is the number of bytes from the item that are shifted into S_new
				 */
	int n, i;
	int ret_val;

	/* Make balance in case insert_size[0] < 0 */
	if (tb->insert_size[0] < 0)
		return balance_leaf_when_delete( /*th, */ tb, flag);

	/* for indirect item pos_in_item is measured in unformatted node
	   pointers. Recalculate to bytes */
	if (flag != M_INSERT
	    && I_IS_INDIRECT_ITEM(item_head(tbS0, item_pos)))
		pos_in_item *= UNFM_P_SIZE;

	if (tb->lnum[0] > 0) {
		/* Shift lnum[0] items from S[0] to the left neighbor L[0] */
		if (item_pos < tb->lnum[0]) {
			/* new item or it part falls to L[0], shift it too */
			n = B_NR_ITEMS(tb->L[0]);

			switch (flag) {
			case M_INSERT:	/* insert item into L[0] */

				if (item_pos == tb->lnum[0] - 1
				    && tb->lbytes != -1) {
					/* part of new item falls into L[0] */
					int new_item_len;

					ret_val =
					    leaf_shift_left( /*th, */ tb,
							    tb->lnum[0] - 1,
							    -1);
					/* Calculate item length to insert to S[0] */
					new_item_len =
					    get_ih_item_len(ih) - tb->lbytes;
					/* Calculate and check item length to insert to L[0] */
					set_ih_item_len(ih,
							get_ih_item_len(ih) -
							new_item_len);
					/* Insert new item into L[0] */
					buffer_info_init_left(tb, &bi, 0);
					leaf_insert_into_buf(&bi,
							     n + item_pos -
							     ret_val, ih, body,
							     zeros_number >
							     get_ih_item_len(ih)
							     ?
							     get_ih_item_len(ih)
							     : zeros_number);

					/* Calculate key component, item length and body to insert into S[0] */
					//ih->ih_key.k_offset += tb->lbytes;
					set_offset(key_format(&ih->ih_key),
						   &ih->ih_key,
						   get_offset(&ih->ih_key) +
						   tb->lbytes *
						   (is_indirect_ih(ih) ? tb->
						    tb_fs->fs_blocksize /
						    UNFM_P_SIZE : 1));
					set_ih_item_len(ih, new_item_len);
					if (tb->lbytes > zeros_number) {
						body +=
						    (tb->lbytes - zeros_number);
						zeros_number = 0;
					} else
						zeros_number -= tb->lbytes;
				} else {
					/* new item in whole falls into L[0] */
					/* Shift lnum[0]-1 items to L[0] */
					ret_val =
					    leaf_shift_left(tb, tb->lnum[0] - 1,
							    tb->lbytes);

					/* Insert new item into L[0] */
					buffer_info_init_left(tb, &bi, 0);
					leaf_insert_into_buf(&bi,
							     n + item_pos -
							     ret_val, ih, body,
							     zeros_number);

					tb->insert_size[0] = 0;
					zeros_number = 0;
				}
				break;

			case M_PASTE:	/* append item in L[0] */

				if (item_pos == tb->lnum[0] - 1
				    && tb->lbytes != -1) {
					/* we must shift the part of the appended item */
					if (I_IS_DIRECTORY_ITEM
					    (item_head(tbS0, item_pos))) {
						/* directory item */
						if (tb->lbytes > pos_in_item) {
							/* new directory entry falls into L[0] */
							struct item_head
							    *pasted;
							int l_pos_in_item =
							    pos_in_item;

							/* Shift lnum[0] - 1 items in whole. Shift lbytes - 1 entries from given directory item */
							ret_val =
							    leaf_shift_left(tb,
									    tb->
									    lnum
									    [0],
									    tb->
									    lbytes
									    -
									    1);
							if (ret_val
							    && !item_pos) {
								pasted =
								    item_head
								    (tb->L[0],
								     B_NR_ITEMS
								     (tb->
								      L[0]) -
								     1);
								l_pos_in_item +=
								    get_ih_entry_count
								    (pasted) -
								    (tb->
								     lbytes -
								     1);
							}

							/* Append given directory entry to directory item */
							buffer_info_init_left(tb, &bi, 0);
							leaf_paste_in_buffer
							    (&bi,
							     n + item_pos -
							     ret_val,
							     l_pos_in_item,
							     tb->insert_size[0],
							     body,
							     zeros_number);

							/* previous string prepared space for pasting new entry, following string pastes this entry */

							/* when we have merge directory item, pos_in_item has been changed too */

							/* paste new directory entry. 1 is entry number */
							leaf_paste_entries(bi.
									   bi_bh,
									   n +
									   item_pos
									   -
									   ret_val,
									   l_pos_in_item,
									   1,
									   (struct
									    reiserfs_de_head
									    *)
									   body,
									   body
									   +
									   DEH_SIZE,
									   tb->
									   insert_size
									   [0]
							    );
							tb->insert_size[0] = 0;
						} else {
							/* new directory item doesn't fall into L[0] */
							/* Shift lnum[0]-1 items in whole. Shift lbytes directory entries from directory item number lnum[0] */
							leaf_shift_left(tb,
									tb->
									lnum[0],
									tb->
									lbytes);
						}
						/* Calculate new position to append in item body */
						pos_in_item -= tb->lbytes;
					} else {
						/* regular object */
						if (tb->lbytes >= pos_in_item) {
							/* appended item will be in L[0] in whole */
							int l_n, temp_n;
							struct reiserfs_key
							    *key;

							/* this bytes number must be appended to the last item of L[h] */
							l_n =
							    tb->lbytes -
							    pos_in_item;

							/* Calculate new insert_size[0] */
							tb->insert_size[0] -=
							    l_n;

							ret_val =
							    leaf_shift_left(tb,
									    tb->
									    lnum
									    [0],
									    get_ih_item_len
									    (item_head
									     (tbS0,
									      item_pos)));
							/* Append to body of item in L[0] */
							buffer_info_init_left(tb, &bi, 0);
							leaf_paste_in_buffer
							    (&bi,
							     n + item_pos -
							     ret_val,
							     get_ih_item_len
							     (item_head
							      (tb->L[0],
							       n + item_pos -
							       ret_val)), l_n,
							     body,
							     zeros_number >
							     l_n ? l_n :
							     zeros_number);

							/* 0-th item in S0 can be only of DIRECT type when l_n != 0 */
							//B_N_PKEY (tbS0, 0)->k_offset += l_n;z
							key = leaf_key(tbS0, 0);
							temp_n =
							    is_indirect_ih
							    (item_head
							     (tb->L[0],
							      n + item_pos -
							      ret_val))
							    ? (int)((l_n /
								     UNFM_P_SIZE)
								    *
								    tb->tb_fs->
								    fs_blocksize)
							    : l_n;

							set_offset(key_format
								   (key), key,
								   get_offset
								   (key) +
								   temp_n);

							//internal_key(tb->CFL[0],tb->lkey[0])->k_offset += l_n;
							key =
							    internal_key(tb->
									   CFL
									   [0],
									   tb->
									   lkey
									   [0]);
							set_offset(key_format
								   (key), key,
								   get_offset
								   (key) +
								   temp_n);

							/* Calculate new body, position in item and insert_size[0] */
							if (l_n > zeros_number) {
								body +=
								    (l_n -
								     zeros_number);
								zeros_number =
								    0;
							} else
								zeros_number -=
								    l_n;
							pos_in_item = 0;
						} else {
							/* only part of the appended item will be in L[0] */

							/* Calculate position in item for append in S[0] */
							pos_in_item -=
							    tb->lbytes;

							/* Shift lnum[0] - 1 items in whole. Shift lbytes - 1 byte from item number lnum[0] */
							leaf_shift_left(tb,
									tb->
									lnum[0],
									tb->
									lbytes);
						}
					}
				} else {
					/* appended item will be in L[0] in whole */
					struct item_head *pasted;

					if (!item_pos && is_left_mergeable(tb->tb_fs, tb->tb_path) == 1) {	/* if we paste into first item of S[0] and it is left mergable */
						/* then increment pos_in_item by the size of the last item in L[0] */
						pasted =
						    item_head(tb->L[0],
								   n - 1);
						if (I_IS_DIRECTORY_ITEM(pasted))
							pos_in_item +=
							    get_ih_entry_count
							    (pasted);
						else
							pos_in_item +=
							    get_ih_item_len
							    (pasted);
					}

					/* Shift lnum[0] - 1 items in whole. Shift lbytes - 1 byte from item number lnum[0] */
					ret_val =
					    leaf_shift_left(tb, tb->lnum[0],
							    tb->lbytes);
					/* Append to body of item in L[0] */
					buffer_info_init_left(tb, &bi, 0);
					leaf_paste_in_buffer(&bi,
							     n + item_pos -
							     ret_val,
							     pos_in_item,
							     tb->insert_size[0],
							     body,
							     zeros_number);

					/* if appended item is directory, paste entry */
					pasted =
					    item_head(tb->L[0],
							   n + item_pos -
							   ret_val);
					if (I_IS_DIRECTORY_ITEM(pasted))
						leaf_paste_entries(bi.bi_bh,
								   n +
								   item_pos -
								   ret_val,
								   pos_in_item,
								   1,
								   (struct
								    reiserfs_de_head
								    *)body,
								   body +
								   DEH_SIZE,
								   tb->
								   insert_size
								   [0]);

					/* if appended item is indirect item, put unformatted node into un list */
					if (I_IS_INDIRECT_ITEM(pasted))
						set_ih_free_space(pasted, 0);

					tb->insert_size[0] = 0;
					zeros_number = 0;
				}
				break;
			default:	/* cases d and t */
				reiserfs_panic
				    ("PAP-12130: balance_leaf: lnum > 0: unexpectable mode: %s(%d)",
				     (flag ==
				      M_DELETE) ? "DELETE" : ((flag ==
							       M_CUT) ? "CUT" :
							      "UNKNOWN"), flag);
			}
		} else {
			/* new item doesn't fall into L[0] */
			leaf_shift_left(tb, tb->lnum[0], tb->lbytes);
		}
	}

	/* tb->lnum[0] > 0 */
	/* Calculate new item position */
	item_pos -= (tb->lnum[0] - ((tb->lbytes != -1) ? 1 : 0));

	if (tb->rnum[0] > 0) {
		/* shift rnum[0] items from S[0] to the right neighbor R[0] */
		n = B_NR_ITEMS(tbS0);
		switch (flag) {

		case M_INSERT:	/* insert item */
			if (n - tb->rnum[0] < item_pos) {
				/* new item or its part falls to R[0] */
				if (item_pos == n - tb->rnum[0] + 1
				    && tb->rbytes != -1) {
					/* part of new item falls into R[0] */
					loff_t old_key_comp, old_len,
					    r_zeros_number;
					const char *r_body;
					loff_t multiplyer;

					leaf_shift_right(tb, tb->rnum[0] - 1,
							 -1);

					/* Remember key component and item length */
					old_key_comp = get_offset(&ih->ih_key);
					old_len = get_ih_item_len(ih);

					multiplyer =
					    is_indirect_ih(ih) ? tb->tb_fs->
					    fs_blocksize / UNFM_P_SIZE : 1;
					/* Calculate key component and item length to insert into R[0] */
					//ih->ih_key.k_offset += (old_len - tb->rbytes);
					set_offset(key_format(&ih->ih_key),
						   &ih->ih_key,
						   old_key_comp + (old_len -
								   tb->rbytes) *
						   multiplyer);

					set_ih_item_len(ih, tb->rbytes);
					/* Insert part of the item into R[0] */
					buffer_info_init_right(tb, &bi, 0);
					if (old_len - tb->rbytes > zeros_number) {
						r_zeros_number = 0;
						r_body =
						    body + old_len -
						    tb->rbytes - zeros_number;
					} else {	/* zeros_number is always 0 */
						r_body = body;
						r_zeros_number =
						    zeros_number - old_len -
						    tb->rbytes;
						zeros_number -= r_zeros_number;
					}

					leaf_insert_into_buf(&bi, 0,
							     ih, r_body,
							     r_zeros_number);

					/* Replace right delimiting key by first key in R[0] */
					replace_key(tb->tb_fs, tb->CFR[0],
						    tb->rkey[0], tb->R[0], 0);

					/* Calculate key component and item length to insert into S[0] */
					//ih->ih_key.k_offset = old_key_comp;
					set_offset(key_format(&ih->ih_key),
						   &ih->ih_key, old_key_comp);

					set_ih_item_len(ih,
							old_len - tb->rbytes);

					tb->insert_size[0] -= tb->rbytes;

				} else {
					/* whole new item falls into R[0] */

					/* Shift rnum[0]-1 items to R[0] */
					ret_val =
					    leaf_shift_right(tb,
							     tb->rnum[0] - 1,
							     tb->rbytes);

					/* Insert new item into R[0] */
					buffer_info_init_right(tb, &bi, 0);
					leaf_insert_into_buf(&bi,
							     item_pos - n +
							     tb->rnum[0] - 1,
							     ih, body,
							     zeros_number);

					/* If we insert new item in the begin of R[0] change the right delimiting key */
					if (item_pos - n + tb->rnum[0] - 1 == 0) {
						replace_key(tb->tb_fs,
							    tb->CFR[0],
							    tb->rkey[0],
							    tb->R[0], 0);
					}
					zeros_number = tb->insert_size[0] = 0;
				}
			} else {
				/* new item or part of it doesn't fall into R[0] */
				leaf_shift_right(tb, tb->rnum[0], tb->rbytes);
			}
			break;

		case M_PASTE:	/* append item */

			if (n - tb->rnum[0] <= item_pos) {	/* pasted item or part of it falls to R[0] */
				if (item_pos == n - tb->rnum[0]
				    && tb->rbytes != -1) {
					/* we must shift the part of the appended item */
					if (I_IS_DIRECTORY_ITEM
					    (item_head(tbS0, item_pos))) {
						/* we append to directory item */
						int entry_count;

						entry_count =
						    get_ih_entry_count
						    (item_head
						     (tbS0, item_pos));
						if (entry_count - tb->rbytes <
						    pos_in_item) {
							/* new directory entry falls into R[0] */
							int paste_entry_position;

							/* Shift rnum[0]-1 items in whole. Shift rbytes-1 directory entries from directory item number rnum[0] */
							leaf_shift_right(tb,
									 tb->
									 rnum
									 [0],
									 tb->
									 rbytes
									 - 1);

							/* Paste given directory entry to directory item */
							paste_entry_position =
							    pos_in_item -
							    entry_count +
							    tb->rbytes - 1;

							buffer_info_init_right(tb, &bi, 0);
							leaf_paste_in_buffer
							    (&bi, 0,
							     paste_entry_position,
							     tb->insert_size[0],
							     body,
							     zeros_number);
							/* paste entry */
							leaf_paste_entries(bi.
									   bi_bh,
									   0,
									   paste_entry_position,
									   1,
									   (struct
									    reiserfs_de_head
									    *)
									   body,
									   body
									   +
									   DEH_SIZE,
									   tb->
									   insert_size
									   [0]
							    );

							if (paste_entry_position
							    == 0) {
								/* change delimiting keys */
								replace_key(tb->
									    tb_fs,
									    tb->
									    CFR
									    [0],
									    tb->
									    rkey
									    [0],
									    tb->
									    R
									    [0],
									    0);
							}

							tb->insert_size[0] = 0;
							pos_in_item++;
						} else {
							/* new directory entry doesn't fall into R[0] */
							leaf_shift_right(tb,
									 tb->
									 rnum
									 [0],
									 tb->
									 rbytes);
						}
					} else {
						/* regular object */

						int n_shift, n_rem,
						    r_zeros_number;
						const char *r_body;
						struct reiserfs_key *key;

						/* Calculate number of bytes which must be shifted from appended item */
						if ((n_shift =
						     tb->rbytes -
						     tb->insert_size[0]) < 0)
							n_shift = 0;

						leaf_shift_right(tb,
								 tb->rnum[0],
								 n_shift);

						/* Calculate number of bytes which must remain in body after appending to R[0] */
						if ((n_rem =
						     tb->insert_size[0] -
						     tb->rbytes) < 0)
							n_rem = 0;

						{
							unsigned long temp_rem =
							    n_rem;

							if (is_indirect_key
							    (leaf_key
							     (tb->R[0], 0)))
								temp_rem =
								    (n_rem /
								     UNFM_P_SIZE)
								    *
								    tb->tb_fs->
								    fs_blocksize;

							//leaf_key(tb->R[0],0)->k_offset += n_rem;
							key =
							    leaf_key(tb->R[0],
								     0);
							set_offset(key_format
								   (key), key,
								   get_offset
								   (key) +
								   temp_rem);

							//internal_key(tb->CFR[0],tb->rkey[0])->k_offset += n_rem;
							key =
							    internal_key(tb->
									   CFR
									   [0],
									   tb->
									   rkey
									   [0]);
							set_offset(key_format
								   (key), key,
								   get_offset
								   (key) +
								   temp_rem);
						}

						mark_buffer_dirty(tb->CFR[0]);

						/* Append part of body into R[0] */
						buffer_info_init_right(tb, &bi, 0);
						if (n_rem > zeros_number) {
							r_zeros_number = 0;
							r_body =
							    body + n_rem -
							    zeros_number;
						} else {
							r_body = body;
							r_zeros_number =
							    zeros_number -
							    n_rem;
							zeros_number -=
							    r_zeros_number;
						}

						leaf_paste_in_buffer(&bi, 0,
								     n_shift,
								     tb->
								     insert_size
								     [0] -
								     n_rem,
								     r_body,
								     r_zeros_number);

						if (I_IS_INDIRECT_ITEM
						    (item_head
						     (tb->R[0], 0))) {
							set_ih_free_space
							    (item_head
							     (tb->R[0], 0), 0);
						}

						tb->insert_size[0] = n_rem;
						if (!n_rem)
							pos_in_item++;
					}
				} else {
					/* pasted item falls into R[0] entirely */

					struct item_head *pasted;

					ret_val =
					    leaf_shift_right(tb, tb->rnum[0],
							     tb->rbytes);

					/* append item in R[0] */
					if (pos_in_item >= 0) {
						buffer_info_init_right(tb, &bi, 0);
						leaf_paste_in_buffer(&bi,
								     item_pos -
								     n +
								     tb->
								     rnum[0],
								     pos_in_item,
								     tb->
								     insert_size
								     [0], body,
								     zeros_number);
					}

					/* paste new entry, if item is directory item */
					pasted =
					    item_head(tb->R[0],
							   item_pos - n +
							   tb->rnum[0]);
					if (I_IS_DIRECTORY_ITEM(pasted)
					    && pos_in_item >= 0) {
						leaf_paste_entries(bi.bi_bh,
								   item_pos -
								   n +
								   tb->rnum[0],
								   pos_in_item,
								   1,
								   (struct
								    reiserfs_de_head
								    *)body,
								   body +
								   DEH_SIZE,
								   tb->
								   insert_size
								   [0]);
						if (!pos_in_item) {
							/* update delimiting keys */
							replace_key(tb->tb_fs,
								    tb->CFR[0],
								    tb->rkey[0],
								    tb->R[0],
								    0);
						}
					}

					if (I_IS_INDIRECT_ITEM(pasted))
						set_ih_free_space(pasted, 0);
					zeros_number = tb->insert_size[0] = 0;
				}
			} else {
				/* new item doesn't fall into R[0] */
				leaf_shift_right(tb, tb->rnum[0], tb->rbytes);
			}
			break;

		default:	/* cases d and t */
			reiserfs_panic
			    ("PAP-12175: balance_leaf: rnum > 0: unexpectable mode: %s(%d)",
			     (flag ==
			      M_DELETE) ? "DELETE" : ((flag ==
						       M_CUT) ? "CUT" :
						      "UNKNOWN"), flag);
		}

	}

	/* tb->rnum[0] > 0 */
	/* if while adding to a node we discover that it is possible to split
	   it in two, and merge the left part into the left neighbor and the
	   right part into the right neighbor, eliminating the node */
	if (tb->blknum[0] == 0) {	/* node S[0] is empty now */
		/* if insertion was done before 0-th position in R[0], right
		   delimiting key of the tb->L[0]'s and left delimiting key are
		   not set correctly */
		if (tb->CFL[0]) {
			if (!tb->CFR[0])
				reiserfs_panic(tb->tb_fs,
					       "vs-12195: balance_leaf: CFR not initialized");
			copy_key(internal_key(tb->CFL[0], tb->lkey[0]),
				 internal_key(tb->CFR[0], tb->rkey[0]));
			mark_buffer_dirty(tb->CFL[0]);
		}

		reiserfs_invalidate_buffer(tb, tbS0);
		return 0;
	}

	/* Fill new nodes that appear in place of S[0] */

	/* I am told that this copying is because we need an array to enable
	   the looping code. -Hans */
	snum[0] = tb->s1num, snum[1] = tb->s2num;
	sbytes[0] = tb->s1bytes;
	sbytes[1] = tb->s2bytes;
	for (i = tb->blknum[0] - 2; i >= 0; i--) {
		/* here we shift from S to S_new nodes */
		S_new[i] = get_FEB(tb);

		/* set block_head's level to leaf level */
		set_blkh_level(B_BLK_HEAD(S_new[i]), DISK_LEAF_NODE_LEVEL);

		n = B_NR_ITEMS(tbS0);

		switch (flag) {
		case M_INSERT:	/* insert item */

			if (n - snum[i] < item_pos) {
				/* new item or it's part falls to first new node S_new[i] */
				if (item_pos == n - snum[i] + 1
				    && sbytes[i] != -1) {
					/* part of new item falls into S_new[i] */
					int old_key_comp, old_len,
					    r_zeros_number;
					const char *r_body;
					loff_t multiplyer;

					/* Move snum[i]-1 items from S[0] to S_new[i] */
					leaf_move_items(LEAF_FROM_S_TO_SNEW, tb,
							snum[i] - 1, -1,
							S_new[i]);

					/* Remember key component and item length */
					old_key_comp = get_offset(&ih->ih_key);
					old_len = get_ih_item_len(ih);
					multiplyer =
					    is_indirect_ih(ih) ? tb->tb_fs->
					    fs_blocksize / UNFM_P_SIZE : 1;

					/* Calculate key component and item length to insert into S_new[i] */
					//ih->ih_key.k_offset += (old_len - sbytes[i]);
					set_offset(key_format(&ih->ih_key),
						   &ih->ih_key,
						   old_key_comp + (old_len -
								   sbytes[i]) *
						   multiplyer);

					set_ih_item_len(ih, sbytes[i]);

					/* Insert part of the item into S_new[i] before 0-th item */
					buffer_info_init_bh(tb, &bi, S_new[i]);

					if (old_len - sbytes[i] > zeros_number) {
						r_zeros_number = 0;
						r_body =
						    body + (old_len -
							    sbytes[i]) -
						    zeros_number;
					} else {
						r_body = body;
						r_zeros_number =
						    zeros_number - (old_len -
								    sbytes[i]);
						zeros_number -= r_zeros_number;
					}

					leaf_insert_into_buf(&bi, 0, ih,
							     r_body,
							     r_zeros_number);

					/* Calculate key component and item length to insert into S[i] */
					//ih->ih_key.k_offset = old_key_comp;
					set_offset(key_format(&ih->ih_key),
						   &ih->ih_key, old_key_comp);
					set_ih_item_len(ih,
							old_len - sbytes[i]);
					tb->insert_size[0] -= sbytes[i];
				} else {	/* whole new item falls into S_new[i] */

					/* Shift snum[0] - 1 items to S_new[i] (sbytes[i] of split item) */
					leaf_move_items(LEAF_FROM_S_TO_SNEW, tb,
							snum[i] - 1, sbytes[i],
							S_new[i]);

					/* Insert new item into S_new[i] */
					buffer_info_init_bh(tb, &bi, S_new[i]);
					leaf_insert_into_buf(&bi,
							     item_pos - n +
							     snum[i] - 1, ih,
							     body,
							     zeros_number);
					zeros_number = tb->insert_size[0] = 0;
				}
			}

			else {	/* new item or it part don't falls into S_new[i] */

				leaf_move_items(LEAF_FROM_S_TO_SNEW, tb,
						snum[i], sbytes[i], S_new[i]);
			}
			break;

		case M_PASTE:	/* append item */

			if (n - snum[i] <= item_pos) {	/* pasted item or part if it falls to S_new[i] */
				if (item_pos == n - snum[i] && sbytes[i] != -1) {	/* we must shift part of the appended item */
					struct item_head *aux_ih;

					if (I_IS_DIRECTORY_ITEM
					    (aux_ih =
					     item_head(tbS0, item_pos))) {
						/* we append to directory item */

						int entry_count;

						entry_count =
						    get_ih_entry_count(aux_ih);

						if (entry_count - sbytes[i] <
						    pos_in_item
						    && pos_in_item <=
						    entry_count) {
							/* new directory entry falls into S_new[i] */

							/* Shift snum[i]-1 items in whole. Shift sbytes[i] directory entries from directory item number snum[i] */
							leaf_move_items
							    (LEAF_FROM_S_TO_SNEW,
							     tb, snum[i],
							     sbytes[i] - 1,
							     S_new[i]);

							/* Paste given directory entry to directory item */
							buffer_info_init_bh(tb, &bi, S_new[i]);
							leaf_paste_in_buffer
							    (&bi, 0,
							     pos_in_item -
							     entry_count +
							     sbytes[i] - 1,
							     tb->insert_size[0],
							     body,
							     zeros_number);
							/* paste new directory entry */
							leaf_paste_entries(bi.
									   bi_bh,
									   0,
									   pos_in_item
									   -
									   entry_count
									   +
									   sbytes
									   [i] -
									   1, 1,
									   (struct
									    reiserfs_de_head
									    *)
									   body,
									   body
									   +
									   DEH_SIZE,
									   tb->
									   insert_size
									   [0]
							    );
							tb->insert_size[0] = 0;
							pos_in_item++;
						} else {	/* new directory entry doesn't fall into S_new[i] */
							leaf_move_items
							    (LEAF_FROM_S_TO_SNEW,
							     tb, snum[i],
							     sbytes[i],
							     S_new[i]);
						}
					} else {	/* regular object */

						int n_shift, n_rem,
						    r_zeros_number;
						const char *r_body;
						struct item_head *tmp;

						/* Calculate number of bytes which must be shifted from appended item */
						n_shift =
						    sbytes[i] -
						    tb->insert_size[0];
						if (n_shift < 0)
							n_shift = 0;
						leaf_move_items
						    (LEAF_FROM_S_TO_SNEW, tb,
						     snum[i], n_shift,
						     S_new[i]);

						/* Calculate number of bytes which must remain in body after append to S_new[i] */
						n_rem =
						    tb->insert_size[0] -
						    sbytes[i];
						if (n_rem < 0)
							n_rem = 0;
						/* Append part of body into S_new[0] */
						buffer_info_init_bh(tb, &bi, S_new[i]);

						if (n_rem > zeros_number) {
							r_zeros_number = 0;
							r_body =
							    body + n_rem -
							    zeros_number;
						} else {
							r_body = body;
							r_zeros_number =
							    zeros_number -
							    n_rem;
							zeros_number -=
							    r_zeros_number;
						}

						leaf_paste_in_buffer(&bi, 0,
								     n_shift,
								     tb->
								     insert_size
								     [0] -
								     n_rem,
								     r_body,
								     r_zeros_number);
						tmp =
						    item_head(S_new[i], 0);
						if (I_IS_INDIRECT_ITEM(tmp)) {
/*			
			    if (n_rem)
				reiserfs_panic ("PAP-12230: balance_leaf: "
						"invalid action with indirect item");
			    set_ih_free_space (tmp, 0);
*/
							set_ih_free_space(tmp,
									  0);
							set_offset(key_format
								   (&tmp->
								    ih_key),
								   &tmp->ih_key,
								   get_offset
								   (&tmp->
								    ih_key) +
								   (n_rem /
								    UNFM_P_SIZE)
								   *
								   tb->tb_fs->
								   fs_blocksize);
						} else
							set_offset(key_format
								   (&tmp->
								    ih_key),
								   &tmp->ih_key,
								   get_offset
								   (&tmp->
								    ih_key) +
								   n_rem);

						//leaf_key(S_new[i],0)->k_offset += n_rem;
//

						tb->insert_size[0] = n_rem;
						if (!n_rem)
							pos_in_item++;
					}
				} else
					/* item falls wholly into S_new[i] */
				{
					struct item_head *pasted;

					leaf_move_items(LEAF_FROM_S_TO_SNEW, tb,
							snum[i], sbytes[i],
							S_new[i]);
					/* paste into item */
					buffer_info_init_bh(tb, &bi, S_new[i]);
					leaf_paste_in_buffer(&bi,
							     item_pos - n +
							     snum[i],
							     pos_in_item,
							     tb->insert_size[0],
							     body,
							     zeros_number);

					pasted =
					    item_head(S_new[i],
							   item_pos - n +
							   snum[i]);
					if (I_IS_DIRECTORY_ITEM(pasted)) {
						leaf_paste_entries(bi.bi_bh,
								   item_pos -
								   n + snum[i],
								   pos_in_item,
								   1,
								   (struct
								    reiserfs_de_head
								    *)body,
								   body +
								   DEH_SIZE,
								   tb->
								   insert_size
								   [0]);
					}

					/* if we paste to indirect item update ih_free_space */
					if (I_IS_INDIRECT_ITEM(pasted))
						set_ih_free_space(pasted, 0);
					zeros_number = tb->insert_size[0] = 0;
				}
			} else {
				/* pasted item doesn't fall into S_new[i] */
				leaf_move_items(LEAF_FROM_S_TO_SNEW, tb,
						snum[i], sbytes[i], S_new[i]);
			}
			break;

		default:	/* cases d and t */
			reiserfs_panic
			    ("PAP-12245: balance_leaf: blknum > 2: unexpectable mode: %s(%d)",
			     (flag ==
			      M_DELETE) ? "DELETE" : ((flag ==
						       M_CUT) ? "CUT" :
						      "UNKNOWN"), flag);
		}

		memcpy(insert_key + i, leaf_key(S_new[i], 0), KEY_SIZE);
		insert_ptr[i] = S_new[i];
	}

	/* if the affected item was not wholly shifted then we perform all
	   necessary operations on that part or whole of the affected item which
	   remains in S */
	if (0 <= item_pos && item_pos < tb->s0num) {
		/* if we must insert or append into buffer S[0] */

		switch (flag) {
		case M_INSERT:	/* insert item into S[0] */
			buffer_info_init_tbS0(tb, &bi);
			leaf_insert_into_buf(&bi, item_pos, ih, body,
					     zeros_number);

			/* If we insert the first key change the delimiting key */
			if (item_pos == 0) {
				if (tb->CFL[0])	/* can be 0 in reiserfsck */
					replace_key(tb->tb_fs, tb->CFL[0],
						    tb->lkey[0], tbS0, 0);
			}
			break;

		case M_PASTE:{	/* append item in S[0] */
				struct item_head *pasted;

				pasted = item_head(tbS0, item_pos);
				/* when directory, may be new entry already pasted */
				if (I_IS_DIRECTORY_ITEM(pasted)) {
					if (pos_in_item >= 0
					    && pos_in_item <=
					    get_ih_entry_count(pasted)) {
						/* prepare space */
						buffer_info_init_tbS0(tb, &bi);
						leaf_paste_in_buffer(&bi,
								     item_pos,
								     pos_in_item,
								     tb->
								     insert_size
								     [0], body,
								     zeros_number);

						/* paste entry */
						leaf_paste_entries(bi.bi_bh,
								   item_pos,
								   pos_in_item,
								   1,
								   (struct
								    reiserfs_de_head
								    *)body,
								   body +
								   DEH_SIZE,
								   tb->
								   insert_size
								   [0]);
						if (!item_pos && !pos_in_item) {
							if (tb->CFL[0])	// can be 0 in reiserfsck
								replace_key(tb->
									    tb_fs,
									    tb->
									    CFL
									    [0],
									    tb->
									    lkey
									    [0],
									    tbS0,
									    0);
						}
						tb->insert_size[0] = 0;
					}
				} else {	/* regular object */
					if (pos_in_item ==
					    get_ih_item_len(pasted)) {
						buffer_info_init_tbS0(tb, &bi);
						leaf_paste_in_buffer(&bi,
								     item_pos,
								     pos_in_item,
								     tb->
								     insert_size
								     [0], body,
								     zeros_number);

						if (I_IS_INDIRECT_ITEM(pasted)) {
							set_ih_free_space
							    (pasted, 0);
						}
						tb->insert_size[0] = 0;
					}
				}
			}	/* case M_PASTE: */
		}
	}

	return 0;
}				/* Leaf level of the tree is balanced (end of balance_leaf) */

void make_empty_leaf(struct buffer_head *bh)
{
	set_blkh_nr_items(B_BLK_HEAD(bh), 0);
	set_blkh_free_space(B_BLK_HEAD(bh), MAX_FREE_SPACE(bh->b_size));
	set_blkh_level(B_BLK_HEAD(bh), DISK_LEAF_NODE_LEVEL);
}

/* Make empty node */
void make_empty_node(struct buffer_info *bi)
{
	make_empty_leaf(bi->bi_bh);

	if (bi->bi_parent)
		set_dc_child_size(B_N_CHILD(bi->bi_parent, bi->bi_position), 0);
}

/* Get first empty buffer */
struct buffer_head *get_FEB(struct tree_balance *tb)
{
	int i;
	struct buffer_head *first_b;
	struct buffer_info bi;

	for (i = 0; i < MAX_FEB_SIZE; i++)
		if (tb->FEB[i] != NULL)
			break;

	if (i == MAX_FEB_SIZE)
		reiserfs_panic("vs-12300: get_FEB: FEB list is empty");

	first_b = tb->FEB[i];
	buffer_info_init_bh(tb, &bi, first_b);
	make_empty_node(&bi);
	misc_set_bit(BH_Uptodate, &first_b->b_state);

	tb->FEB[i] = NULL;
	tb->used[i] = first_b;

	return (first_b);
}

/* Replace n_dest'th key in buffer dest by n_src'th key of buffer src.*/
void replace_key(reiserfs_filsys_t fs,
		 struct buffer_head *dest, int n_dest,
		 struct buffer_head *src, int n_src)
{
	if (dest) {
		if (is_leaf_node(src))
			/* source buffer contains leaf node */
			memcpy(internal_key(dest, n_dest),
			       item_head(src, n_src), KEY_SIZE);
		else
			memcpy(internal_key(dest, n_dest),
			       internal_key(src, n_src), KEY_SIZE);

		mark_buffer_dirty(dest);
	}
}

void reiserfs_invalidate_buffer(struct tree_balance *tb, struct buffer_head *bh)
{
	struct buffer_head *to_be_forgotten;
	set_blkh_level(B_BLK_HEAD(bh), FREE_LEVEL);
	misc_clear_bit(BH_Dirty, &bh->b_state);

	to_be_forgotten = find_buffer(bh->b_dev, bh->b_blocknr, bh->b_size);
	if (to_be_forgotten) {
		to_be_forgotten->b_count++;
		bforget(to_be_forgotten);
	}

	reiserfs_free_block(tb->tb_fs, bh->b_blocknr);
}

int get_left_neighbor_position(const struct tree_balance *tb, int h)
{
	int Sh_position = PATH_H_POSITION(tb->tb_path, h + 1);

	if (Sh_position == 0)
		return B_NR_ITEMS(tb->FL[h]);
	else
		return Sh_position - 1;
}

int get_right_neighbor_position(const struct tree_balance *tb, int h)
{
	int Sh_position = PATH_H_POSITION(tb->tb_path, h + 1);

	if (Sh_position == B_NR_ITEMS(PATH_H_PPARENT(tb->tb_path, h)))
		return 0;
	else
		return Sh_position + 1;
}

/* Now we have all of the buffers that must be used in balancing of the tree.
   We rely on the assumption that schedule() will not occur while do_balance
   works. ( Only interrupt handlers are acceptable.)  We balance the tree
   according to the analysis made before this, using buffers already obtained.
   For SMP support it will someday be necessary to add ordered locking of
   tb. */

/* Some interesting rules of balancing:

   we delete a maximum of two nodes per level per balancing: we never delete R, when we delete two
   of three nodes L, S, R then we move them into R.

   we only delete L if we are deleting two nodes, if we delete only one node we delete S

   if we shift leaves then we shift as much as we can: this is a deliberate policy of extremism in
   node packing which results in higher average utilization after repeated random balance
   operations at the cost of more memory copies and more balancing as a result of small insertions
   to full nodes.

   if we shift internal nodes we try to evenly balance the node utilization, with consequent less
   balancing at the cost of lower utilization.

   one could argue that the policy for directories in leaves should be that of internal nodes, but
   we will wait until another day to evaluate this....  It would be nice to someday measure and
   prove these assumptions as to what is optimal....

*/

void do_balance(struct tree_balance *tb,	/* tree_balance structure               */
		struct item_head *ih,	/* item header of inserted item */
		const char *body,	/* body  of inserted item or bytes to paste */
		int flag,	/* i - insert, d - delete
				   c - cut, p - paste

				   Cut means delete part of an item (includes
				   removing an entry from a directory).

				   Delete means delete whole item.

				   Insert means add a new item into the tree.

				   Paste means to append to the end of an existing
				   file or to insert a directory entry.  */
		int zeros_num)
{
	//int pos_in_item = tb->tb_path->pos_in_item;
	int child_pos,		/* position of a child node in its parent */
	 h;			/* level of the tree being processed */
	struct item_head insert_key[2];	/* in our processing of one level we
					   sometimes determine what must be
					   inserted into the next higher level.
					   This insertion consists of a key or two
					   keys and their corresponding pointers */
	struct buffer_head *insert_ptr[2];	/* inserted node-ptrs for the next
						   level */

	/* if we have no real work to do  */
	if (!tb->insert_size[0]) {
		unfix_nodes( /*th, */ tb);
		return;
	}

	if (flag == M_INTERNAL) {
		insert_ptr[0] = (struct buffer_head *)body;
		/* we must prepare insert_key */

		if (PATH_H_B_ITEM_ORDER(tb->tb_path, 0)	/*LAST_POSITION (tb->tb_path) */
		    /*item_pos */  == -1) {
			/* get delimiting key from buffer in tree */
			copy_key(&insert_key[0].ih_key,
				 leaf_key(PATH_PLAST_BUFFER(tb->tb_path), 0));
			/*insert_ptr[0]->b_item_order = 0; */
		} else {
			/* get delimiting key from new buffer */
			copy_key(&insert_key[0].ih_key,
				 leaf_key((struct buffer_head *)body, 0));
			/*insert_ptr[0]->b_item_order = item_pos; */
		}

		/* and insert_ptr instead of balance_leaf */
		child_pos = PATH_H_B_ITEM_ORDER(tb->tb_path, 0) /*item_pos */ ;
	} else
		/* balance leaf returns 0 except if combining L R and S into one node.
		   see balance_internal() for explanation of this line of code. */
		child_pos = PATH_H_B_ITEM_ORDER(tb->tb_path, 0) +
		    balance_leaf( /*th, */ tb /*, pos_in_item */ , ih, body,
				 flag, zeros_num, insert_key, insert_ptr);

	/* Balance internal level of the tree. */
	for (h = 1; h < MAX_HEIGHT && tb->insert_size[h]; h++)
		child_pos =
		    balance_internal( /*th, */ tb, h, child_pos, insert_key,
				     insert_ptr);

	/* Release all (except for S[0]) non NULL buffers fixed by fix_nodes() */
	unfix_nodes( /*th, */ tb);
}
