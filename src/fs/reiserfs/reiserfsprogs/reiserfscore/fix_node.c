/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

/**
 ** old_item_num
 ** old_entry_num
 ** set_entry_sizes
 ** create_virtual_node
 ** check_left
 ** check_right
 ** directory_part_size
 ** get_num_ver
 ** item_length
 ** set_parameters
 ** is_leaf_removable
 ** are_leaves_removable
 ** get_empty_nodes
 ** get_lfree
 ** get_rfree
 ** is_left_neighbor_in_cache
 ** decrement_key
 ** get_far_parent
 ** get_parents
 ** can_node_be_removed
 ** ip_check_balance
 ** dc_check_balance_internal
 ** dc_check_balance_leaf
 ** dc_check_balance
 ** check_balance
 ** get_direct_parent
 ** get_neighbors
 ** fix_nodes
 **
 **
 **/

#include "includes.h"

/* To make any changes in the tree we find a node, that contains item to be
   changed/deleted or position in the node we insert a new item to. We call
   this node S. To do balancing we need to decide what we will shift to
   left/right neighbor, or to a new node, where new item will be etc. To make
   this analysis simpler we build virtual node. Virtual node is an array of
   items, that will replace items of node S. (For instance if we are going to
   delete an item, virtual node does not contain it). Virtual node keeps
   information about item sizes and types, mergeability of first and last
   items, sizes of all entries in directory item. We use this array of items
   when calculating what we can shift to neighbors and how many nodes we have
   to have if we do not any shiftings, if we shift to left/right neighbor or
   to both. */

/* taking item number in virtual node, returns number of item, that it has in
   source buffer */
static inline int old_item_num(int new_num, int affected_item_num, int mode)
{
	if (mode == M_PASTE || mode == M_CUT || new_num < affected_item_num)
		return new_num;

	if (mode == M_INSERT)
		return new_num - 1;

	/* delete mode */
	return new_num + 1;
}

/* function returns old entry number in directory item in real node using new
   entry number in virtual item in virtual node */
static inline int old_entry_num(int new_num, int affected_item_num,
				int new_entry_num, int pos_in_item, int mode)
{
	if (mode == M_INSERT || mode == M_DELETE)
		return new_entry_num;

	if (new_num != affected_item_num) {
		/* cut or paste is applied to another item */
		return new_entry_num;
	}

	if (new_entry_num < pos_in_item)
		return new_entry_num;

	if (mode == M_CUT)
		return new_entry_num + 1;

	return new_entry_num - 1;
}

/*
 * Create an array of sizes of directory entries for virtual item
 */
static void set_entry_sizes(struct tree_balance *tb,
			    int old_num, int new_num,
			    struct buffer_head *bh, struct item_head *ih)
{
	struct virtual_node *vn = tb->tb_vn;
	int i;
	struct reiserfs_de_head *deh;
	struct virtual_item *vi;

	deh = B_I_DEH(bh, ih);

	/* seek to given virtual item in array of virtual items */
	vi = vn->vn_vi + new_num;

	/* virtual directory item have this amount of entry after */
	vi->vi_entry_count = get_ih_entry_count(ih) +
	    ((old_num ==
	      vn->vn_affected_item_num) ? ((vn->vn_mode ==
					    M_CUT) ? -1 : (vn->vn_mode ==
							   M_PASTE ? 1 : 0)) :
	     0);
	vi->vi_entry_sizes = (__u16 *) vn->vn_free_ptr;
	vn->vn_free_ptr += vi->vi_entry_count * sizeof(__u16);

	/* set sizes of old entries */
	for (i = 0; i < vi->vi_entry_count; i++) {
		int j;

		j = old_entry_num(old_num, vn->vn_affected_item_num, i,
				  vn->vn_pos_in_item, vn->vn_mode);
		vi->vi_entry_sizes[i] =
		    entry_length(ih, &(deh[j]), j) + DEH_SIZE;
	}

	/* set size of pasted entry */
	if (old_num == vn->vn_affected_item_num && vn->vn_mode == M_PASTE)
		vi->vi_entry_sizes[vn->vn_pos_in_item] = tb->insert_size[0];
}

static void create_virtual_node(struct tree_balance *tb, int h)
{
	struct item_head *ih;
	struct virtual_node *vn = tb->tb_vn;
	int new_num;
	struct buffer_head *Sh;	/* this comes from tb->S[h] */

	Sh = PATH_H_PBUFFER(tb->tb_path, h);

	/* size of changed node */
	vn->vn_size =
	    MAX_CHILD_SIZE(Sh->b_size) - get_blkh_free_space(B_BLK_HEAD(Sh)) +
	    tb->insert_size[h];

	/* for internal nodes array if virtual items is not created */
	if (h) {
		vn->vn_nr_item = (vn->vn_size - DC_SIZE) / (DC_SIZE + KEY_SIZE);
		return;
	}

	/* number of items in virtual node  */
	vn->vn_nr_item =
	    B_NR_ITEMS(Sh) + ((vn->vn_mode == M_INSERT) ? 1 : 0) -
	    ((vn->vn_mode == M_DELETE) ? 1 : 0);

	/* first virtual item */
	vn->vn_vi = (struct virtual_item *)(tb->tb_vn + 1);
	memset(vn->vn_vi, 0, vn->vn_nr_item * sizeof(struct virtual_item));
	vn->vn_free_ptr += vn->vn_nr_item * sizeof(struct virtual_item);

	/* first item in the node */
	ih = item_head(Sh, 0);

	/* define the mergeability for 0-th item (if it is not being deleted) */
	if (is_left_mergeable(tb->tb_fs, tb->tb_path) == 1
	    && (vn->vn_mode != M_DELETE || vn->vn_affected_item_num))
		vn->vn_vi[0].vi_type |= VI_TYPE_LEFT_MERGEABLE;

	/* go through all items those remain in the virtual node (except for the new (inserted) one) */
	for (new_num = 0; new_num < vn->vn_nr_item; new_num++) {
		int j;

		if (vn->vn_affected_item_num == new_num
		    && vn->vn_mode == M_INSERT)
			continue;

		/* get item number in source node */
		j = old_item_num(new_num, vn->vn_affected_item_num,
				 vn->vn_mode);

		vn->vn_vi[new_num].vi_item_len +=
		    get_ih_item_len(&ih[j]) + IH_SIZE;

		if (I_IS_STAT_DATA_ITEM(ih + j)) {
			vn->vn_vi[new_num].vi_type |= VI_TYPE_STAT_DATA;
			continue;
		}

		/* set type of item */
		if (I_IS_DIRECT_ITEM(ih + j))
			vn->vn_vi[new_num].vi_type |= VI_TYPE_DIRECT;

		if (I_IS_INDIRECT_ITEM(ih + j))
			vn->vn_vi[new_num].vi_type |= VI_TYPE_INDIRECT;

		if (I_IS_DIRECTORY_ITEM(ih + j)) {
			set_entry_sizes(tb, j, new_num, Sh, ih + j);
			vn->vn_vi[new_num].vi_type |= VI_TYPE_DIRECTORY;
			if (get_key_offset_v1(&ih[j].ih_key) == DOT_OFFSET)
				vn->vn_vi[new_num].vi_type |=
				    VI_TYPE_FIRST_DIRECTORY_ITEM;
		}

		vn->vn_vi[new_num].vi_item_offset =
		    get_offset(&(ih + j)->ih_key);

		if (new_num != vn->vn_affected_item_num)
			/* this is not being changed */
			continue;

		if (vn->vn_mode == M_PASTE || vn->vn_mode == M_CUT)
			vn->vn_vi[new_num].vi_item_len += tb->insert_size[0];
	}

	/* virtual inserted item is not defined yet */
	if (vn->vn_mode == M_INSERT) {
		vn->vn_vi[vn->vn_affected_item_num].vi_item_len =
		    tb->insert_size[0];
		vn->vn_vi[vn->vn_affected_item_num].vi_item_offset =
		    get_offset(&vn->vn_ins_ih->ih_key);

		switch (get_type(&vn->vn_ins_ih->ih_key)) {
		case TYPE_STAT_DATA:
			vn->vn_vi[vn->vn_affected_item_num].vi_type |=
			    VI_TYPE_STAT_DATA;
			break;
		case TYPE_DIRECT:
			vn->vn_vi[vn->vn_affected_item_num].vi_type |=
			    VI_TYPE_DIRECT;
			break;
		case TYPE_INDIRECT:
			vn->vn_vi[vn->vn_affected_item_num].vi_type |=
			    VI_TYPE_INDIRECT;
			break;
		default:
			/* inseted item is directory (it must be item with "." and "..") */
			vn->vn_vi[vn->vn_affected_item_num].vi_type |=
			    (VI_TYPE_DIRECTORY | VI_TYPE_FIRST_DIRECTORY_ITEM |
			     VI_TYPE_INSERTED_DIRECTORY_ITEM);

			/* this directory item can not be split, so do not set sizes of entries */
			break;
		}
	}

	/* set right merge flag we take right delimiting key and check whether it is a mergeable item */
	if (tb->CFR[0]) {
		ih = (struct item_head *)internal_key(tb->CFR[0],
							tb->rkey[0]);
		if (is_right_mergeable(tb->tb_fs, tb->tb_path) == 1
		    && (vn->vn_mode != M_DELETE
			|| vn->vn_affected_item_num != B_NR_ITEMS(Sh) - 1))
			vn->vn_vi[vn->vn_nr_item - 1].vi_type |=
			    VI_TYPE_RIGHT_MERGEABLE;
	}
}

/* using virtual node check, how many items can be shifted to left
   neighbor */
static int check_left(struct tree_balance *tb, int h, int cur_free)
{
	int i;
	struct virtual_node *vn = tb->tb_vn;
	int d_size, ih_size, bytes = -1;

	/* internal level */
	if (h > 0) {
		if (!cur_free) {
			tb->lnum[h] = 0;
			return 0;
		}
		tb->lnum[h] = cur_free / (DC_SIZE + KEY_SIZE);
		return -1;
	}

	/* leaf level */

	if (!cur_free || !vn->vn_nr_item) {
		/* no free space */
		tb->lnum[h] = 0;
		tb->lbytes = -1;
		return 0;
	}

	if ((unsigned int)cur_free >=
	    (vn->vn_size -
	     ((vn->vn_vi[0].vi_type & VI_TYPE_LEFT_MERGEABLE) ? IH_SIZE : 0))) {
		/* all contents of S[0] fits into L[0] */
		tb->lnum[0] = vn->vn_nr_item;
		tb->lbytes = -1;
		return -1;
	}

	d_size = 0, ih_size = IH_SIZE;

	/* first item may be merge with last item in left neighbor */
	if (vn->vn_vi[0].vi_type & VI_TYPE_LEFT_MERGEABLE)
		d_size = -((int)IH_SIZE), ih_size = 0;

	tb->lnum[0] = 0;
	for (i = 0; i < vn->vn_nr_item; i++, ih_size = IH_SIZE, d_size = 0) {
		d_size += vn->vn_vi[i].vi_item_len;
		if (cur_free >= d_size) {
			/* the item can be shifted entirely */
			cur_free -= d_size;
			tb->lnum[0]++;
			continue;
		}

		/* the item cannot be shifted entirely, try to split it */
		/* check whether L[0] can hold ih and at least one byte of the item body */
		if (cur_free <= ih_size) {
			/* cannot shift even a part of the current item */
			tb->lbytes = -1;
			return -1;
		}
		cur_free -= ih_size;

		if (vn->vn_vi[i].vi_type & VI_TYPE_STAT_DATA ||
		    vn->vn_vi[i].vi_type & VI_TYPE_INSERTED_DIRECTORY_ITEM) {
			/* virtual item is a stat_data or empty directory body ("." and ".."), that is not split able */
			tb->lbytes = -1;
			return -1;
		}

		if (vn->vn_vi[i].vi_type & VI_TYPE_DIRECT) {
			/* body of a direct item can be split by 8 bytes */
			int align = 8 - (vn->vn_vi[i].vi_item_offset - 1) % 8;
//          reiserfs_warning(stderr,"\nbalancing: cur_free (%d) ", cur_free);
			tb->lbytes = bytes =
			    (cur_free >=
			     align) ? (align +
				       ((cur_free - align) / 8 * 8)) : 0;
//          reiserfs_warning(stderr,"offset (0x%Lx), move_left (%d), get offset (0x%Lx)",
//              vn->vn_vi[i].vi_item_offset, bytes, vn->vn_vi[i].vi_item_offset + bytes);
		}

		if (vn->vn_vi[i].vi_type & VI_TYPE_INDIRECT)
			/* body of a indirect item can be split at unformatted pointer bound */
			tb->lbytes = bytes = cur_free - cur_free % UNFM_P_SIZE;

		/* item is of directory type */
		if (vn->vn_vi[i].vi_type & VI_TYPE_DIRECTORY) {
			/* directory entries are the solid granules of the directory
			   item, they cannot be split in the middle */

			/* calculate number of dir entries that can be shifted, and
			   their total size */
			int j;
			struct virtual_item *vi;

			tb->lbytes = 0;
			bytes = 0;
			vi = &vn->vn_vi[i];

			for (j = 0; j < vi->vi_entry_count; j++) {
				if (vi->vi_entry_sizes[j] > cur_free)
					/* j-th entry doesn't fit into L[0] */
					break;

				bytes += vi->vi_entry_sizes[j];
				cur_free -= vi->vi_entry_sizes[j];
				tb->lbytes++;
			}
			/* "." can not be cut from first directory item */
			if ((vn->vn_vi[i].
			     vi_type & VI_TYPE_FIRST_DIRECTORY_ITEM)
			    && tb->lbytes < 2)
				tb->lbytes = 0;
		}

		if (tb->lbytes <= 0) {
			/* nothing can flow from the item */
			tb->lbytes = -1;
			return -1;
		}

		/* something can flow from the item */
		tb->lnum[0]++;
		return bytes;	/* part of split item in bytes */
	}

	reiserfs_panic(0,
		       "vs-8065: check_left: all items fit in the left neighbor");
	return 0;
}

/* using virtual node check, how many items can be shifted to right
   neighbor */
static int check_right(struct tree_balance *tb, int h, int cur_free)
{
	int i;
	struct virtual_node *vn = tb->tb_vn;
	int d_size, ih_size, bytes = -1;

	/* internal level */
	if (h > 0) {
		if (!cur_free) {
			tb->rnum[h] = 0;
			return 0;
		}
		tb->rnum[h] = cur_free / (DC_SIZE + KEY_SIZE);
		return -1;
	}

	/* leaf level */

	if (!cur_free || !vn->vn_nr_item) {
		/* no free space  */
		tb->rnum[h] = 0;
		tb->rbytes = -1;
		return 0;
	}

	if ((unsigned int)cur_free >=
	    (vn->vn_size -
	     ((vn->vn_vi[vn->vn_nr_item - 1].
	       vi_type & VI_TYPE_RIGHT_MERGEABLE) ? IH_SIZE : 0))) {
		/* all contents of S[0] fits into R[0] */
		tb->rnum[h] = vn->vn_nr_item;
		tb->rbytes = -1;
		return -1;
	}

	d_size = 0, ih_size = IH_SIZE;

	/* last item may be merge with first item in right neighbor */
	if (vn->vn_vi[vn->vn_nr_item - 1].vi_type & VI_TYPE_RIGHT_MERGEABLE)
		d_size = -(int)IH_SIZE, ih_size = 0;

	tb->rnum[0] = 0;
	for (i = vn->vn_nr_item - 1; i >= 0; i--, d_size = 0, ih_size = IH_SIZE) {
		d_size += vn->vn_vi[i].vi_item_len;
		if (cur_free >= d_size) {
			/* the item can be shifted entirely */
			cur_free -= d_size;
			tb->rnum[0]++;
			continue;
		}

		/* the item cannot be shifted entirely, try to split it */
		if (vn->vn_vi[i].vi_type & VI_TYPE_STAT_DATA
		    || vn->vn_vi[i].vi_type & VI_TYPE_INSERTED_DIRECTORY_ITEM) {
			/* virtual item is a stat_data or empty directory body ("." and
			   "..), that is not split able */
			tb->rbytes = -1;
			return -1;
		}

		/* check whether R[0] can hold ih and at least one byte of the item
		   body */
		if (cur_free <= ih_size) {
			/* cannot shift even a part of the current item */
			tb->rbytes = -1;
			return -1;
		}

		/* R[0] can hold the header of the item and at least one byte of its
		   body */
		cur_free -= ih_size;	/* cur_free is still > 0 */

		/* item is of direct type */
		if (vn->vn_vi[i].vi_type & VI_TYPE_DIRECT) {
			/* body of a direct item can be split by 8 bytes */
			int align = vn->vn_vi[i].vi_item_len % 8;
//          reiserfs_warning(stderr,"\nbalancing: cur_free (%d) ", cur_free);
			tb->rbytes = bytes =
			    (cur_free >=
			     align) ? (align +
				       ((cur_free - align) / 8 * 8)) : 0;
//          reiserfs_warning(stderr, "offset (0x%Lx) len (%d), move right (%d), get offset (0x%Lx)",
//              vn->vn_vi[i].vi_item_offset, vn->vn_vi[i].vi_item_len, bytes,
//              vn->vn_vi[i].vi_item_offset + vn->vn_vi[i].vi_item_len - bytes);
		}

		/* item is of indirect type */
		if (vn->vn_vi[i].vi_type & VI_TYPE_INDIRECT)
			/* an unformatted node pointer (having size long) is a solid
			   granule of the item */
			tb->rbytes = bytes = cur_free - cur_free % UNFM_P_SIZE;

		/* item is of directory type */
		if (vn->vn_vi[i].vi_type & VI_TYPE_DIRECTORY) {
			int j;
			struct virtual_item *vi;

			tb->rbytes = 0;
			bytes = 0;
			vi = &vn->vn_vi[i];

			for (j = vi->vi_entry_count - 1; j >= 0; j--) {
				if (vi->vi_entry_sizes[j] > cur_free)
					/* j-th entry doesn't fit into L[0] */
					break;

				bytes += vi->vi_entry_sizes[j];
				cur_free -= vi->vi_entry_sizes[j];
				tb->rbytes++;
			}

			/* ".." can not be cut from first directory item */
			if ((vn->vn_vi[i].
			     vi_type & VI_TYPE_FIRST_DIRECTORY_ITEM)
			    && tb->rbytes > vi->vi_entry_count - 2)
				tb->rbytes = vi->vi_entry_count - 2;
		}

		if (tb->rbytes <= 0) {
			/* nothing can flow from the item */
			tb->rbytes = -1;
			return -1;
		}

		/* something can flow from the item */
		tb->rnum[0]++;
		return bytes;	/* part of split item in bytes */
	}

	reiserfs_panic
	    ("vs-8095: check_right: all items fit in the left neighbor");
	return 0;
}

/* sum of entry sizes between from-th and to-th entries including both edges */
static int directory_part_size(struct virtual_item *vi, int from, int to)
{
	int i, retval;

	retval = 0;
	for (i = from; i <= to; i++)
		retval += vi->vi_entry_sizes[i];

	return retval;
}

/*
 * from - number of items, which are shifted to left neighbor entirely
 * to - number of item, which are shifted to right neighbor entirely
 * from_bytes - number of bytes of boundary item (or directory entries) which
 * are shifted to left neighbor
 * to_bytes - number of bytes of boundary item (or directory entries) which
 * are shifted to right neighbor */

static int get_num_ver(int mode, struct tree_balance *tb, int h,
		       int from, int from_bytes,
		       int to, int to_bytes, short *snum012, int flow)
{
	int i;
	int bytes;
	struct virtual_node *vn = tb->tb_vn;
	struct virtual_item *vi;

	int total_node_size, max_node_size, current_item_size;
	int needed_nodes;
	int start_item,		/* position of item we start filling node from */
	 end_item,		/* position of item we finish filling node by */
	 start_bytes,		/* number of first bytes (entries for directory) of start_item-th item
				   we do not include into node that is being filled */
	 end_bytes;		/* number of last bytes (entries for directory) of end_item-th item
				   we do node include into node that is being filled */
	int splitted_item_positions[2];	/* these are positions in virtual item of items,
					   that are splitted between S[0] and S1new and S1new and S2new */

	max_node_size = MAX_CHILD_SIZE(tb->tb_fs->fs_blocksize);

	/* snum012 [0-2] - number of items, that lay to S[0], first new node and
	   second new node */
	snum012[3] = -1;	/* s1bytes */
	snum012[4] = -1;	/* s2bytes */

	/* internal level */
	if (h > 0) {
		i = ((to - from) * (KEY_SIZE + DC_SIZE) + DC_SIZE);
		if (i == max_node_size)
			return 1;
		return (i / max_node_size + 1);
	}

	/* leaf level */
	needed_nodes = 1;
	total_node_size = 0;

	start_item = from;
	start_bytes = from_bytes;
	end_item = vn->vn_nr_item - to - 1;
	end_bytes = to_bytes;

	/* go through all items begining from the start_item-th item and ending by
	   the end_item-th item. If start_bytes != -1 we skip first start_bytes
	   item units (entries in case of directory). If end_bytes != -1 we skip
	   end_bytes units of the end_item-th item. */
	for (i = start_item; i <= end_item; i++) {
		/* get size of current item */
		current_item_size = (vi = &vn->vn_vi[i])->vi_item_len;

		/* do not take in calculation head part (from_bytes) of from-th item */
		if (i == start_item && start_bytes != -1) {
			if (vi->vi_type & VI_TYPE_DIRECTORY)
				current_item_size -=
				    directory_part_size(vi, 0, start_bytes - 1);
			else
				current_item_size -= start_bytes;
		}

		/* do not take in calculation tail part of (to-1)-th item */
		if (i == end_item && end_bytes != -1) {
			if (vi->vi_type & VI_TYPE_DIRECTORY)
				/* first entry, that is not included */
				current_item_size -=
				    directory_part_size(vi,
							vi->vi_entry_count -
							end_bytes,
							vi->vi_entry_count - 1);
			else
				current_item_size -= end_bytes;
		}

		/* if item fits into current node entirely */
		if (total_node_size + current_item_size <= max_node_size) {
			snum012[needed_nodes - 1]++;
			total_node_size += current_item_size;
			continue;
		}

		if (current_item_size > max_node_size)
			/* virtual item length is longer, than max size of item in a
			   node. It is impossible for direct item */
			/* we will try to split it */
			flow = 1;

		if (!flow) {
			/* as we do not split items, take new node and continue */
			needed_nodes++;
			i--;
			total_node_size = 0;
			continue;
		}

		if (total_node_size + (int)IH_SIZE >= max_node_size) {
			/* even minimal item does not fit into current node, take new node
			   and continue */
			needed_nodes++, i--, total_node_size = 0;
			continue;
		}
		if (vi->vi_type & VI_TYPE_STAT_DATA) {
			/* stat data can not be split */
			needed_nodes++, i--, total_node_size = 0;
			continue;
		}

		/*bytes is free space in filled node */
		bytes = max_node_size - total_node_size - IH_SIZE;

		if (vi->vi_type & VI_TYPE_DIRECT) {
			/* body of a direct item can be split by 8 bytes. */
			int align = 8 - (vn->vn_vi[i].vi_item_offset - 1) % 8;
//          reiserfs_warning(stderr,"\nbalancing: cur_free (%d) ", bytes);
//          reiserfs_warning(stderr,"offset (0x%Lx), move (%d), get offset (0x%Lx)",
//              vn->vn_vi[i].vi_item_offset, (bytes - align) / 8 * 8,
//              vn->vn_vi[i].vi_item_offset + ((bytes - align) / 8 * 8));
			bytes =
			    (bytes >=
			     align) ? (align + ((bytes - align) / 8 * 8)) : 0;
		}

		/* item is of indirect type */
		if (vi->vi_type & VI_TYPE_INDIRECT)
			/* an unformatted node pointer (having size long) is a solid
			   granule of the item. bytes of unformatted node pointers fits
			   into free space of filled node */
			bytes -= (bytes) % UNFM_P_SIZE;

		/* S1bytes or S2bytes. It depends from needed_nodes */
		snum012[needed_nodes - 1 + 3] = bytes;

		/* item is of directory type */
		if (vi->vi_type & VI_TYPE_DIRECTORY) {
			/* calculate, how many entries can be put into current node */
			int j;
			int end_entry;

			snum012[needed_nodes - 1 + 3] = 0;

			total_node_size += IH_SIZE;
			if (start_bytes == -1 || i != start_item)
				start_bytes = 0;

			end_entry =
			    vi->vi_entry_count -
			    ((i == end_item
			      && end_bytes != -1) ? end_bytes : 0);
			for (j = start_bytes; j < end_entry; j++) {
				/* j-th entry doesn't fit into current node */
				if (total_node_size + vi->vi_entry_sizes[j] >
				    max_node_size)
					break;
				snum012[needed_nodes - 1 + 3]++;
				bytes += vi->vi_entry_sizes[j];
				total_node_size += vi->vi_entry_sizes[j];
			}
			/* "." can not be cut from first directory item */
			if (start_bytes == 0
			    && (vn->vn_vi[i].
				vi_type & VI_TYPE_FIRST_DIRECTORY_ITEM)
			    && snum012[needed_nodes - 1 + 3] < 2)
				snum012[needed_nodes - 1 + 3] = 0;
		}

		if (snum012[needed_nodes - 1 + 3] <= 0) {
			/* nothing fits into current node, take new node and continue */
			needed_nodes++, i--, total_node_size = 0;
			continue;
		}

		/* something fits into the current node */
		if (vi->vi_type & VI_TYPE_DIRECTORY)
			start_bytes += snum012[needed_nodes - 1 + 3];
		else
			start_bytes = bytes;

		snum012[needed_nodes - 1]++;
		splitted_item_positions[needed_nodes - 1] = i;

		needed_nodes++;
		/* continue from the same item with start_bytes != -1 */
		start_item = i;
		i--;
		total_node_size = 0;
	}

	/* snum012[3] and snum012[4] contain how many bytes (entries) of split
	   item can be in S[0] and S1new. s1bytes and s2bytes are how many bytes
	   (entries) can be in S1new and S2new. Recalculate it */

	if (snum012[4] > 0) {	/* s2bytes */
		/* get number of item that is split between S1new and S2new */
		int split_item_num;
		int bytes_to_r, bytes_to_l;

		split_item_num = splitted_item_positions[1];
		bytes_to_l =
		    ((from == split_item_num
		      && from_bytes != -1) ? from_bytes : 0);
		bytes_to_r =
		    ((end_item == split_item_num
		      && end_bytes != -1) ? end_bytes : 0);
		if (vn->vn_vi[split_item_num].vi_type & VI_TYPE_DIRECTORY) {
			int entries_to_S2new;

			/* calculate number of entries fit into S2new */
			entries_to_S2new =
			    vn->vn_vi[split_item_num].vi_entry_count -
			    snum012[4] - bytes_to_r - bytes_to_l;
			if (snum012[3] != -1 && snum012[1] == 1) {
				/* directory split into 3 nodes */
				int entries_to_S1new;

				entries_to_S2new -= snum012[3];
				entries_to_S1new = snum012[4];
				snum012[3] = entries_to_S1new;
				snum012[4] = entries_to_S2new;
				return needed_nodes;
			}
			snum012[4] = entries_to_S2new;
		} else {
			/* item is not of directory type */
			int bytes_to_S2new;

			bytes_to_S2new =
			    vn->vn_vi[split_item_num].vi_item_len - IH_SIZE -
			    snum012[4] - bytes_to_r - bytes_to_l;
			snum012[4] = bytes_to_S2new;
		}
	}

	/* now we know S2bytes, calculate S1bytes */
	if (snum012[3] > 0) {	/* s1bytes */
		/* get number of item that is split between S0 and S1new */
		int split_item_num;
		int bytes_to_r, bytes_to_l;

		split_item_num = splitted_item_positions[0];
		bytes_to_l =
		    ((from == split_item_num
		      && from_bytes != -1) ? from_bytes : 0);
		bytes_to_r =
		    ((end_item == split_item_num
		      && end_bytes != -1) ? end_bytes : 0);
		if (vn->vn_vi[split_item_num].vi_type & VI_TYPE_DIRECTORY) {
			/* entries, who go to S1new node */
			snum012[3] =
			    vn->vn_vi[split_item_num].vi_entry_count -
			    snum012[3] - bytes_to_r - bytes_to_l;
		} else
			/* bytes, who go to S1new node (not including HI_SIZE) */
			snum012[3] =
			    vn->vn_vi[split_item_num].vi_item_len - IH_SIZE -
			    snum012[3] - bytes_to_r - bytes_to_l;
	}

	return needed_nodes;
}

/* size of item_num-th item in bytes when regular and in entries when item is
   directory */
static int item_length(struct tree_balance *tb, int item_num)
{
	struct virtual_node *vn = tb->tb_vn;

	if (vn->vn_vi[item_num].vi_type & VI_TYPE_DIRECTORY)
		return vn->vn_vi[item_num].vi_entry_count;

	return vn->vn_vi[item_num].vi_item_len - IH_SIZE;
}

/* Set parameters for balancing.
 * Performs write of results of analysis of balancing into structure tb,
 * where it will later be used by the functions that actually do the balancing.
 * Parameters:
 *	tb	tree_balance structure;
 *	h	current level of the node;
 *	lnum	number of items from S[h] that must be shifted to L[h];
 *	rnum	number of items from S[h] that must be shifted to R[h];
 *	blk_num	number of blocks that S[h] will be splitted into;
 *	s012	number of items that fall into splitted nodes.
 *	lbytes	number of bytes which flow to the left neighbor from the item that is not
 *		not shifted entirely
 *	rbytes	number of bytes which flow to the right neighbor from the item that is not
 *		not shifted entirely
 *	s1bytes	number of bytes which flow to the first  new node when S[0] splits (this number is contained in s012 array)
 */

static void set_parameters(struct tree_balance *tb, int h, int lnum,
			   int rnum, int blk_num, short *s012, int lb, int rb)
{

	tb->lnum[h] = lnum;
	tb->rnum[h] = rnum;
	tb->blknum[h] = blk_num;

	if (h == 0) {		/* only for leaf level */
		if (s012 != NULL) {
			tb->s0num = *s012++,
			    tb->s1num = *s012++, tb->s2num = *s012++;
			tb->s1bytes = *s012++;
			tb->s2bytes = *s012;
		}
		tb->lbytes = lb;
		tb->rbytes = rb;
	}
}

static void decrement_key(struct reiserfs_key *p_s_key)
{
	int type;

	type = get_type(p_s_key);
	switch (type) {
	case TYPE_STAT_DATA:
		set_key_objectid(p_s_key, get_key_objectid(p_s_key) - 1);
		set_type_and_offset(key_format(p_s_key), p_s_key,
				    (loff_t) MAX_FILE_SIZE_V2, TYPE_INDIRECT);
		return;

	case TYPE_INDIRECT:
	case TYPE_DIRECT:
	case TYPE_DIRENTRY:
		set_offset(key_format(p_s_key), p_s_key,
			   get_offset(p_s_key) - 1);
		if (get_offset(p_s_key) == 0)
			set_type(key_format(p_s_key), p_s_key, TYPE_STAT_DATA);
		return;
	}
	reiserfs_warning(stderr,
			 "vs-8125: decrement_key: item of wrong type found %k",
			 p_s_key);
}

int are_items_mergeable(struct item_head *left, struct item_head *right,
			int bsize)
{
	if (comp_keys(&left->ih_key, &right->ih_key) != -1) {
		reiserfs_panic
		    ("vs-16070: are_items_mergeable: left %k, right %k",
		     &(left->ih_key), &(right->ih_key));
	}

	if (not_of_one_file(&left->ih_key, &right->ih_key))
		return 0;

	if (I_IS_DIRECTORY_ITEM(left)) {
		return 1;
	}

	if ((I_IS_DIRECT_ITEM(left) && I_IS_DIRECT_ITEM(right)) ||
	    (I_IS_INDIRECT_ITEM(left) && I_IS_INDIRECT_ITEM(right)))
		return (get_offset(&left->ih_key) +
			get_bytes_number(left,
					 bsize) ==
			get_offset(&right->ih_key)) ? 1 : 0;

	return 0;
}

/* get left neighbor of the leaf node */
static struct buffer_head *get_left_neighbor(reiserfs_filsys_t s,
					     struct reiserfs_path *path)
{
	struct reiserfs_key key;
	struct reiserfs_path path_to_left_neighbor;
	struct buffer_head *bh;

	copy_key(&key, leaf_key(PATH_PLAST_BUFFER(path), 0));
	decrement_key(&key);

	init_path(&path_to_left_neighbor);
	search_by_key(s, &key, &path_to_left_neighbor, DISK_LEAF_NODE_LEVEL);
	if (PATH_LAST_POSITION(&path_to_left_neighbor) == 0) {
		pathrelse(&path_to_left_neighbor);
		return NULL;
	}
	bh = PATH_PLAST_BUFFER(&path_to_left_neighbor);
	bh->b_count++;
	pathrelse(&path_to_left_neighbor);
	return bh;
}

static struct buffer_head *get_right_neighbor(reiserfs_filsys_t s,
					      struct reiserfs_path *path)
{
	struct reiserfs_key key;
	const struct reiserfs_key *rkey;
	struct reiserfs_path path_to_right_neighbor;
	struct buffer_head *bh;

	rkey = get_rkey(path, s);
	if (comp_keys(rkey, &MIN_KEY) == 0)
		reiserfs_panic
		    ("vs-16080: get_right_neighbor: get_rkey returned min key (path has changed)");
	copy_key(&key, rkey);

	init_path(&path_to_right_neighbor);
	search_by_key(s, &key, &path_to_right_neighbor, DISK_LEAF_NODE_LEVEL);
	if (PATH_PLAST_BUFFER(&path_to_right_neighbor) ==
	    PATH_PLAST_BUFFER(path)) {
		pathrelse(&path_to_right_neighbor);
		return NULL;
	}
	bh = PATH_PLAST_BUFFER(&path_to_right_neighbor);
	bh->b_count++;
	pathrelse(&path_to_right_neighbor);
	return bh;
}

int is_left_mergeable(reiserfs_filsys_t s, struct reiserfs_path *path)
{
	struct item_head *right;
	struct buffer_head *bh;
	int retval;

	right = item_head(PATH_PLAST_BUFFER(path), 0);

	bh = get_left_neighbor(s, path);
	if (bh == NULL) {
		return 0;
	}
	retval =
	    are_items_mergeable(item_head(bh, B_NR_ITEMS(bh) - 1), right,
				bh->b_size);
	brelse(bh);
	return retval;
}

int is_right_mergeable(reiserfs_filsys_t s, struct reiserfs_path *path)
{
	struct item_head *left;
	struct buffer_head *bh;
	int retval;

	left =
	    item_head(PATH_PLAST_BUFFER(path),
			   B_NR_ITEMS(PATH_PLAST_BUFFER(path)) - 1);

	bh = get_right_neighbor(s, path);
	if (bh == NULL) {
		return 0;
	}
	retval = are_items_mergeable(left, item_head(bh, 0), bh->b_size);
	brelse(bh);
	return retval;
}

/* check, does node disappear if we shift tb->lnum[0] items to left neighbor
   and tb->rnum[0] to the right one. */
static int is_leaf_removable(struct tree_balance *tb)
{
	struct virtual_node *vn = tb->tb_vn;
	int to_left, to_right;
	int size;
	int remain_items;

	/* number of items, that will be shifted to left (right) neighbor entirely */
	to_left = tb->lnum[0] - ((tb->lbytes != -1) ? 1 : 0);
	to_right = tb->rnum[0] - ((tb->rbytes != -1) ? 1 : 0);
	remain_items = vn->vn_nr_item;

	/* how many items remain in S[0] after shiftings to neighbors */
	remain_items -= (to_left + to_right);

	if (remain_items < 1) {
		/* all content of node can be shifted to neighbors */
		set_parameters(tb, 0, to_left, vn->vn_nr_item - to_left, 0,
			       NULL, -1, -1);
		return 1;
	}

	if (remain_items > 1 || tb->lbytes == -1 || tb->rbytes == -1)
		/* S[0] is not removable */
		return 0;

	/* check, whether we can divide 1 remaining item between neighbors */

	/* get size of remaining item (in directory entry count if directory) */
	size = item_length(tb, to_left);

	if (tb->lbytes + tb->rbytes >= size) {
		set_parameters(tb, 0, to_left + 1, to_right + 1, 0, NULL,
			       tb->lbytes, -1);
		return 1;
	}

	return 0;
}

/* check whether L, S, R can be joined in one node */
static int are_leaves_removable(struct tree_balance *tb, int lfree, int rfree)
{
	struct virtual_node *vn = tb->tb_vn;
	int ih_size;
	struct buffer_head *S0;

	S0 = PATH_H_PBUFFER(tb->tb_path, 0);

	ih_size = 0;
	if (vn->vn_nr_item) {
		if (vn->vn_vi[0].vi_type & VI_TYPE_LEFT_MERGEABLE)
			ih_size += IH_SIZE;

		if (vn->vn_vi[vn->vn_nr_item - 1].
		    vi_type & VI_TYPE_RIGHT_MERGEABLE)
			ih_size += IH_SIZE;
	} else {
		/* there was only one item and it will be deleted */
		struct item_head *ih;

		ih = item_head(S0, 0);
		if (tb->CFR[0]
		    && !not_of_one_file(&(ih->ih_key),
					internal_key(tb->CFR[0],
						       tb->rkey[0])))
			if (I_IS_DIRECTORY_ITEM(ih)) {
				/* we can delete any directory item in fsck (if it is unreachable) */
				if (get_offset(&ih->ih_key) != DOT_OFFSET) {
					/* must get left neighbor here to make sure, that
					   left neighbor is of the same directory */
					struct buffer_head *left;

					left =
					    get_left_neighbor(tb->tb_fs,
							      tb->tb_path);
					if (left) {
						struct item_head *last;

						if (B_NR_ITEMS(left) == 0)
							reiserfs_panic
							    ("vs-8135: are_leaves_removable: "
							     "empty node in the tree");
						last =
						    item_head(left,
								   B_NR_ITEMS
								   (left) - 1);
						if (!comp_short_keys
						    (&last->ih_key,
						     &ih->ih_key))
							ih_size = IH_SIZE;
						brelse(left);
					}
				}
			}

	}

	if ((int)MAX_CHILD_SIZE(S0->b_size) + vn->vn_size <=
	    rfree + lfree + ih_size) {
		set_parameters(tb, 0, -1, -1, -1, NULL, -1, -1);
		return 1;
	}
	return 0;

}

/* when we do not split item, lnum and rnum are numbers of entire items */
#define SET_PAR_SHIFT_LEFT \
if (h)\
{\
   int to_l;\
   \
   to_l = (MAX_NR_KEY(Sh)+1 - lpar + vn->vn_nr_item + 1) / 2 -\
	      (MAX_NR_KEY(Sh) + 1 - lpar);\
	      \
	      set_parameters (tb, h, to_l, 0, lnver, NULL, -1, -1);\
}\
else \
{\
   if (lset==LEFT_SHIFT_FLOW)\
     set_parameters (tb, h, lpar, 0, lnver, snum012+lset,\
		     tb->lbytes, -1);\
   else\
     set_parameters (tb, h, lpar - (tb->lbytes!=-1), 0, lnver, snum012+lset,\
		     -1, -1);\
}

#define SET_PAR_SHIFT_RIGHT \
if (h)\
{\
   int to_r;\
   \
   to_r = (MAX_NR_KEY(Sh)+1 - rpar + vn->vn_nr_item + 1) / 2 - (MAX_NR_KEY(Sh) + 1 - rpar);\
   \
   set_parameters (tb, h, 0, to_r, rnver, NULL, -1, -1);\
}\
else \
{\
   if (rset==RIGHT_SHIFT_FLOW)\
     set_parameters (tb, h, 0, rpar, rnver, snum012+rset,\
		  -1, tb->rbytes);\
   else\
     set_parameters (tb, h, 0, rpar - (tb->rbytes!=-1), rnver, snum012+rset,\
		  -1, -1);\
}

/* Get new buffers for storing new nodes that are created while balancing.
 * Returns:	SCHEDULE_OCCURED - schedule occured while the function worked;
 *	        CARRY_ON - schedule didn't occur while the function worked;
 *	        NO_DISK_SPACE - no disk space.
 */
static int get_empty_nodes(struct tree_balance *p_s_tb, int n_h)
{
	struct buffer_head *p_s_new_bh,
	    *p_s_Sh = PATH_H_PBUFFER(p_s_tb->tb_path, n_h);
	unsigned long *p_n_blocknr, a_n_blocknrs[MAX_AMOUNT_NEEDED] = { 0, };
	int n_counter, n_number_of_freeblk, n_amount_needed,	/* number of needed empty blocks */
	 n_repeat;
	reiserfs_filsys_t fs = p_s_tb->tb_fs;

	if (n_h == 0 && p_s_tb->insert_size[n_h] == 0x7fff)
		return CARRY_ON;

	/* number_of_freeblk is the number of empty blocks which have been
	   acquired for use by the balancing algorithm minus the number of
	   empty blocks used in the previous levels of the analysis,
	   number_of_freeblk = tb->cur_blknum can be non-zero if a
	   schedule occurs after empty blocks are acquired, and the
	   balancing analysis is then restarted, amount_needed is the
	   number needed by this level (n_h) of the balancing analysis.

	   Note that for systems with many processes writing, it would be
	   more layout optimal to calculate the total number needed by all
	   levels and then to run reiserfs_new_blocks to get all of them
	   at once.  */

	/* Initiate number_of_freeblk to the amount acquired prior to the restart of
	   the analysis or 0 if not restarted, then subtract the amount needed
	   by all of the levels of the tree below n_h. */
	/* blknum includes S[n_h], so we subtract 1 in this calculation */
	for (n_counter = 0, n_number_of_freeblk = p_s_tb->cur_blknum;
	     n_counter < n_h; n_counter++)
		n_number_of_freeblk -=
		    (p_s_tb->blknum[n_counter]) ? (p_s_tb->blknum[n_counter] -
						   1) : 0;

	/* Allocate missing empty blocks. */
	/* if p_s_Sh == 0  then we are getting a new root */
	n_amount_needed = (p_s_Sh) ? (p_s_tb->blknum[n_h] - 1) : 1;
	/*  Amount_needed = the amount that we need more than the amount that we have. */
	if (n_amount_needed > n_number_of_freeblk)
		n_amount_needed -= n_number_of_freeblk;
	else			/* If we have enough already then there is nothing to do. */
		return CARRY_ON;

	if ((n_repeat = reiserfs_new_blocknrs(p_s_tb->tb_fs, a_n_blocknrs,
					      PATH_PLAST_BUFFER(p_s_tb->
								tb_path)->
					      b_blocknr,
					      n_amount_needed)) != CARRY_ON) {
		return n_repeat;	/* Out of disk space. */
	}

	/* for each blocknumber we just got, get a buffer and stick it on FEB */
	for (p_n_blocknr = a_n_blocknrs, n_counter = 0;
	     n_counter < n_amount_needed; p_n_blocknr++, n_counter++) {
		p_s_new_bh = getblk(fs->fs_dev, *p_n_blocknr, fs->fs_blocksize);
		if (p_s_new_bh->b_count > 1) {
			die("get_empty_nodes: not free empty buffer");
		}

		/* Put empty buffers into the array. */
		p_s_tb->FEB[p_s_tb->cur_blknum++] = p_s_new_bh;
	}

	return CARRY_ON;
}

/* Get free space of the left neighbor,
 * which is stored in the parent node of the left neighbor.
 */
static int get_lfree(struct tree_balance *tb, int h)
{
	struct buffer_head *l, *f;
	int order;

	if ((f = PATH_H_PPARENT(tb->tb_path, h)) == NULL ||
	    (l = tb->FL[h]) == NULL)
		return 0;

	if (f == l)
		order = PATH_H_B_ITEM_ORDER(tb->tb_path, h) - 1;
	else {
		order = get_blkh_nr_items(B_BLK_HEAD(l));
		f = l;
	}

	if (get_dc_child_size(B_N_CHILD(f, order)) == 0) {
		reiserfs_warning(stderr,
				 "get_lfree: block %u block_head %z has bad child pointer %y, order %d\n",
				 l->b_blocknr, l, B_N_CHILD(f, order), order);
	}
	return (MAX_CHILD_SIZE(f->b_size) -
		get_dc_child_size(B_N_CHILD(f, order)));
}

/* Get free space of the right neighbor, which is stored in the parent node of
 * the right neighbor.  */
static int get_rfree(struct tree_balance *tb, int h)
{
	struct buffer_head *r, *f;
	int order;

	if ((f = PATH_H_PPARENT(tb->tb_path, h)) == NULL ||
	    (r = tb->FR[h]) == NULL)
		return 0;

	if (f == r)
		order = PATH_H_B_ITEM_ORDER(tb->tb_path, h) + 1;
	else {
		order = 0;
		f = r;
	}

	return (MAX_CHILD_SIZE(f->b_size) -
		get_dc_child_size(B_N_CHILD(f, order)));

}

/* Check whether left neighbor is in memory. */
static int is_left_neighbor_in_cache(struct tree_balance *p_s_tb, int n_h)
{
	struct buffer_head *p_s_father;
	reiserfs_filsys_t fs = p_s_tb->tb_fs;
	unsigned long n_left_neighbor_blocknr;
	int n_left_neighbor_position;

	if (!p_s_tb->FL[n_h])	/* Father of the left neighbor does not exist. */
		return 0;

	/* Calculate father of the node to be balanced. */
	p_s_father = PATH_H_PBUFFER(p_s_tb->tb_path, n_h + 1);
	/* Get position of the pointer to the left neighbor into the left father. */
	n_left_neighbor_position = (p_s_father == p_s_tb->FL[n_h]) ?
	    p_s_tb->lkey[n_h] : get_blkh_nr_items(B_BLK_HEAD(p_s_tb->FL[n_h]));
	/* Get left neighbor block number. */
	n_left_neighbor_blocknr =
	    get_dc_child_blocknr(B_N_CHILD
				 (p_s_tb->FL[n_h], n_left_neighbor_position));
	/* Look for the left neighbor in the cache. */
	if ((p_s_father =
	     find_buffer(fs->fs_dev, n_left_neighbor_blocknr,
			 fs->fs_blocksize)))
		return 1;

	return 0;
}

#define LEFT_PARENTS  'l'
#define RIGHT_PARENTS 'r'

void init_path(struct reiserfs_path *path)
{
	path->path_length = ILLEGAL_PATH_ELEMENT_OFFSET;
}

/* Calculate far left/right parent of the left/right neighbor of the current node, that
 * is calculate the left/right (FL[h]/FR[h]) neighbor of the parent F[h].
 * Calculate left/right common parent of the current node and L[h]/R[h].
 * Calculate left/right delimiting key position.
 * Returns:	PATH_INCORRECT   - path in the tree is not correct;
 		SCHEDULE_OCCURRED - schedule occured while the function worked;
 *	        CARRY_ON         - schedule didn't occur while the function worked;
 */
static int get_far_parent(struct tree_balance *p_s_tb,
			  int n_h,
			  struct buffer_head **pp_s_father,
			  struct buffer_head **pp_s_com_father, char c_lr_par)
{
	struct buffer_head *p_s_parent;
	struct reiserfs_path s_path_to_neighbor_father,
	    *p_s_path = p_s_tb->tb_path;
	struct reiserfs_key s_lr_father_key;
	int n_counter,
	    n_position = -1,
	    n_first_last_position = 0,
	    n_path_offset = PATH_H_PATH_OFFSET(p_s_path, n_h);

	/* Starting from F[n_h] go upwards in the tree, and look for the common
	   ancestor of F[n_h], and its neighbor l/r, that should be obtained. */

	n_counter = n_path_offset;

	for (; n_counter > FIRST_PATH_ELEMENT_OFFSET; n_counter--) {
		/* Check whether parent of the current buffer in the path is really parent in the tree. */
		if (!B_IS_IN_TREE
		    (p_s_parent = PATH_OFFSET_PBUFFER(p_s_path, n_counter - 1)))
			reiserfs_panic
			    ("get_far_parent: buffer of path is notin the tree");

		/* Check whether position in the parent is correct. */
		if ((n_position =
		     PATH_OFFSET_POSITION(p_s_path,
					  n_counter - 1)) >
		    B_NR_ITEMS(p_s_parent))
			reiserfs_panic
			    ("get_far_parent: incorrect position in the parent");

		/* Check whether parent at the path really points to the child. */
		if (get_dc_child_blocknr(B_N_CHILD(p_s_parent, n_position)) !=
		    PATH_OFFSET_PBUFFER(p_s_path, n_counter)->b_blocknr)
			reiserfs_panic
			    ("get_far_parent: incorrect disk child in the parent");

		/* Return delimiting key if position in the parent is not equal to first/last one. */
		if (c_lr_par == RIGHT_PARENTS)
			n_first_last_position =
			    get_blkh_nr_items(B_BLK_HEAD(p_s_parent));
		if (n_position != n_first_last_position) {
			(*pp_s_com_father = p_s_parent)->b_count++;
			break;
		}
	}

	/* we are in the root of the tree. */
	if (n_counter == FIRST_PATH_ELEMENT_OFFSET) {
		struct reiserfs_super_block *sb;

		sb = p_s_tb->tb_fs->fs_ondisk_sb;

		/* Check whether first buffer in the path is the root of the tree. */
		if (PATH_OFFSET_PBUFFER
		    (p_s_tb->tb_path,
		     FIRST_PATH_ELEMENT_OFFSET)->b_blocknr ==
		    get_sb_root_block(sb)) {
			*pp_s_father = *pp_s_com_father = NULL;
			return CARRY_ON;
		}
		reiserfs_panic("get_far_parent: root not found in the path");
	}

	if (n_position == -1)
		reiserfs_panic("get_far_parent: position is not defined");

	/* So, we got common parent of the current node and its left/right
	   neighbor.  Now we are geting the parent of the left/right neighbor. */

	/* Form key to get parent of the left/right neighbor. */
	copy_key(&s_lr_father_key,
		 internal_key(*pp_s_com_father,
				(c_lr_par ==
				 LEFT_PARENTS) ? (p_s_tb->lkey[n_h - 1] =
						  n_position -
						  1) : (p_s_tb->rkey[n_h - 1] =
							n_position)));

	if (c_lr_par == LEFT_PARENTS) {
		//reiserfs_warning ("decrememnting key %k\n", &s_lr_father_key);
		decrement_key(&s_lr_father_key);
		//reiserfs_warning ("done: %k\n", &s_lr_father_key);
	}

	init_path(&s_path_to_neighbor_father);

	if (search_by_key
	    (p_s_tb->tb_fs, &s_lr_father_key, &s_path_to_neighbor_father,
	     n_h + 1) == IO_ERROR)
		return IO_ERROR;

	*pp_s_father = PATH_PLAST_BUFFER(&s_path_to_neighbor_father);
	s_path_to_neighbor_father.path_length--;
	pathrelse(&s_path_to_neighbor_father);
	//decrement_counters_in_path(&s_path_to_neighbor_father);
	return CARRY_ON;
}

/* Get parents of neighbors of node in the path(S[n_path_offset]) and common parents of
 * S[n_path_offset] and L[n_path_offset]/R[n_path_offset]: F[n_path_offset], FL[n_path_offset],
 * FR[n_path_offset], CFL[n_path_offset], CFR[n_path_offset].
 * Calculate numbers of left and right delimiting keys position: lkey[n_path_offset], rkey[n_path_offset].
 * Returns:	SCHEDULE_OCCURRED - schedule occured while the function worked;
 *	        CARRY_ON - schedule didn't occur while the function worked;
 */
static int get_parents(struct tree_balance *p_s_tb, int n_h)
{
	struct reiserfs_path *p_s_path = p_s_tb->tb_path;
	int n_position,
	    n_ret_value,
	    n_path_offset = PATH_H_PATH_OFFSET(p_s_tb->tb_path, n_h);
	struct buffer_head *p_s_curf, *p_s_curcf;

	/* Current node is the root of the tree or will be root of the tree */
	if (n_path_offset <= FIRST_PATH_ELEMENT_OFFSET) {
		/* The root can not have parents.
		   Release nodes which previously were obtained as parents of the current node neighbors. */
		brelse(p_s_tb->FL[n_h]);
		brelse(p_s_tb->CFL[n_h]);
		brelse(p_s_tb->FR[n_h]);
		brelse(p_s_tb->CFR[n_h]);
		//decrement_bcount(p_s_tb->FL[n_h]);
		//decrement_bcount(p_s_tb->CFL[n_h]);
		//decrement_bcount(p_s_tb->FR[n_h]);
		//decrement_bcount(p_s_tb->CFR[n_h]);
		p_s_tb->FL[n_h] = p_s_tb->CFL[n_h] = p_s_tb->FR[n_h] =
		    p_s_tb->CFR[n_h] = NULL;
		return CARRY_ON;
	}

	/* Get parent FL[n_path_offset] of L[n_path_offset]. */
	if ((n_position = PATH_OFFSET_POSITION(p_s_path, n_path_offset - 1))) {
		/* Current node is not the first child of its parent. */
		(p_s_curf = p_s_curcf =
		 PATH_OFFSET_PBUFFER(p_s_path, n_path_offset - 1))->b_count +=
2;
		p_s_tb->lkey[n_h] = n_position - 1;
	} else {
		/* Calculate current parent of L[n_path_offset], which is the left neighbor of the current node.
		   Calculate current common parent of L[n_path_offset] and the current node. Note that
		   CFL[n_path_offset] not equal FL[n_path_offset] and CFL[n_path_offset] not equal F[n_path_offset].
		   Calculate lkey[n_path_offset]. */
		if ((n_ret_value = get_far_parent(p_s_tb, n_h + 1, &p_s_curf,
						  &p_s_curcf,
						  LEFT_PARENTS)) != CARRY_ON)
			return n_ret_value;	/*schedule() occured or path is not correct */
	}

	brelse(p_s_tb->FL[n_h]);
	p_s_tb->FL[n_h] = p_s_curf;	/* New initialization of FL[n_h]. */

	brelse(p_s_tb->CFL[n_h]);
	p_s_tb->CFL[n_h] = p_s_curcf;	/* New initialization of CFL[n_h]. */

/* Get parent FR[n_h] of R[n_h]. */

/* Current node is the last child of F[n_h]. FR[n_h] != F[n_h]. */
	if (n_position ==
	    get_blkh_nr_items(B_BLK_HEAD(PATH_H_PBUFFER(p_s_path, n_h + 1)))) {
/* Calculate current parent of R[n_h], which is the right neighbor of F[n_h].
   Calculate current common parent of R[n_h] and current node. Note that CFR[n_h]
   not equal FR[n_path_offset] and CFR[n_h] not equal F[n_h]. */
		if ((n_ret_value =
		     get_far_parent(p_s_tb, n_h + 1, &p_s_curf, &p_s_curcf,
				    RIGHT_PARENTS)) != CARRY_ON)
			return n_ret_value;	/*schedule() occured while get_far_parent() worked. */
	} else {
/* Current node is not the last child of its parent F[n_h]. */
		(p_s_curf = p_s_curcf =
		 PATH_OFFSET_PBUFFER(p_s_path, n_path_offset - 1))->b_count +=
2;
		p_s_tb->rkey[n_h] = n_position;
	}

	brelse /*decrement_bcount */ (p_s_tb->FR[n_h]);
	p_s_tb->FR[n_h] = p_s_curf;	/* New initialization of FR[n_path_offset]. */

	brelse /*decrement_bcount */ (p_s_tb->CFR[n_h]);
	p_s_tb->CFR[n_h] = p_s_curcf;	/* New initialization of CFR[n_path_offset]. */

	return CARRY_ON;	/* schedule not occured while get_parents() worked. */
}

/* it is possible to remove node as result of shiftings to
   neighbors even when we insert or paste item. */
static inline int can_node_be_removed(int mode, int lfree, int sfree, int rfree,
				      struct tree_balance *tb, int h)
{
	struct buffer_head *Sh = PATH_H_PBUFFER(tb->tb_path, h);
	int levbytes = tb->insert_size[h];
	struct item_head *r_ih = NULL;

	if (tb->CFR[h])
		r_ih =
		    (struct item_head *)internal_key(tb->CFR[h], tb->rkey[h]);

	if (lfree + rfree + sfree < (int)(MAX_CHILD_SIZE(Sh->b_size) + levbytes
					  /* shifting may merge items which might save space */
					  -
					  ((!h
					    && is_left_mergeable(tb->tb_fs,
								 tb->tb_path) ==
					    1) ? IH_SIZE : 0)
					  -
					  ((!h && r_ih
					    && is_right_mergeable(tb->tb_fs,
								  tb->
								  tb_path) ==
					    1) ? IH_SIZE : 0)
					  + ((h) ? KEY_SIZE : 0))) {
		/* node can not be removed */
		if (sfree >= levbytes) {	/* new item fits into node S[h] without any shifting */
			if (!h)
				tb->s0num =
				    B_NR_ITEMS(Sh) +
				    ((mode == M_INSERT) ? 1 : 0);
			set_parameters(tb, h, 0, 0, 1, NULL, -1, -1);
			return NO_BALANCING_NEEDED;
		}
	}
	return !NO_BALANCING_NEEDED;
}

/* Check whether current node S[h] is balanced when increasing its size by
 * Inserting or Pasting.
 * Calculate parameters for balancing for current level h.
 * Parameters:
 *	tb	tree_balance structure;
 *	h	current level of the node;
 *	inum	item number in S[h];
 *	mode	i - insert, p - paste;
 * Returns:	1 - schedule occured;
 *	        0 - balancing for higher levels needed;
 *	       -1 - no balancing for higher levels needed;
 *	       -2 - no disk space.
 */
/* ip means Inserting or Pasting */
static int ip_check_balance( /*struct reiserfs_transaction_handle *th, */ struct
			    tree_balance *tb, int h)
{
	struct virtual_node *vn = tb->tb_vn;
	int levbytes,		/* Number of bytes that must be inserted into (value is
				   negative if bytes are deleted) buffer which contains
				   node being balanced.  The mnemonic is that the attempted
				   change in node space used level is levbytes bytes. */
	 n_ret_value;

	int lfree, sfree, rfree /* free space in L, S and R */ ;

	/* nver is short for number of vertices, and lnver is the number if we
	   shift to the left, rnver is the number if we shift to the right, and
	   lrnver is the number if we shift in both directions.  The goal is to
	   minimize first the number of vertices, and second, the number of
	   vertices whose contents are changed by shifting, and third the number
	   of uncached vertices whose contents are changed by shifting and must be
	   read from disk.  */
	int nver, lnver, rnver, lrnver;

	/* used at leaf level only, S0 = S[0] is the node being balanced, sInum [
	   I = 0,1,2 ] is the number of items that will remain in node SI after
	   balancing.  S1 and S2 are new nodes that might be created. */

	/* we perform 8 calls to get_num_ver().  For each call we calculate five
	   parameters.  where 4th parameter is s1bytes and 5th - s2bytes */
	short snum012[40] = { 0, };	/* s0num, s1num, s2num for 8 cases
					   0,1 - do not shift and do not shift but bottle
					   2 - shift only whole item to left
					   3 - shift to left and bottle as much as possible
					   4,5 - shift to right (whole items and as much as possible
					   6,7 - shift to both directions (whole items and as much as possible)
					 */

	/* Sh is the node whose balance is currently being checked */
	struct buffer_head *Sh;

	/* special mode for insert pointer to the most low internal node */
	if (h == 0 && vn->vn_mode == M_INTERNAL) {
		/* blk_num == 2 is to get pointer inserted to the next level */
		set_parameters(tb, h, 0, 0, 2, NULL, -1, -1);
		return 0;
	}

	Sh = PATH_H_PBUFFER(tb->tb_path, h);
	levbytes = tb->insert_size[h];

	/* Calculate balance parameters for creating new root. */
	if (!Sh) {
		if (!h)
			reiserfs_panic
			    ("vs-8210: ip_check_balance: S[0] can not be 0");
		switch (n_ret_value = get_empty_nodes(tb, h)) {
		case CARRY_ON:
			set_parameters(tb, h, 0, 0, 1, NULL, -1, -1);
			return NO_BALANCING_NEEDED;	/* no balancing for higher levels needed */

		case NO_DISK_SPACE:
			return n_ret_value;

		default:
			reiserfs_panic
			    ("vs-8215: ip_check_balance: incorrect return value of get_empty_nodes");
		}
	}

	if ((n_ret_value = get_parents(tb, h)) != CARRY_ON)	/* get parents of S[h] neighbors. */
		return n_ret_value;

	sfree = get_blkh_free_space(B_BLK_HEAD(Sh));

	/* get free space of neighbors */
	rfree = get_rfree(tb, h);
	lfree = get_lfree(tb, h);

	if (can_node_be_removed(vn->vn_mode, lfree, sfree, rfree, tb, h) ==
	    NO_BALANCING_NEEDED)
		/* and new item fits into node S[h] without any shifting */
		return NO_BALANCING_NEEDED;

	create_virtual_node(tb, h);

	/* determine maximal number of items we can shift to the left neighbor
	   (in tb structure) and the maximal number of bytes that can flow to the
	   left neighbor from the left most liquid item that cannot be shifted
	   from S[0] entirely (returned value) */
	check_left(tb, h, lfree);

	/* determine maximal number of items we can shift to the right neighbor
	   (in tb structure) and the maximal number of bytes that can flow to the
	   right neighbor from the right most liquid item that cannot be shifted
	   from S[0] entirely (returned value) */
	check_right(tb, h, rfree);

	/* all contents of internal node S[h] can be moved into its neighbors,
	   S[h] will be removed after balancing */
	if (h && (tb->rnum[h] + tb->lnum[h] >= vn->vn_nr_item + 1)) {
		int to_r;

		/* Since we are working on internal nodes, and our internal nodes have
		   fixed size entries, then we can balance by the number of items
		   rather than the space they consume.  In this routine we set the left
		   node equal to the right node, allowing a difference of less than or
		   equal to 1 child pointer. */
		to_r =
		    ((MAX_NR_KEY(Sh) << 1) + 2 - tb->lnum[h] - tb->rnum[h] +
		     vn->vn_nr_item + 1) / 2 - (MAX_NR_KEY(Sh) + 1 -
						tb->rnum[h]);
		set_parameters(tb, h, vn->vn_nr_item + 1 - to_r, to_r, 0, NULL,
			       -1, -1);
		return CARRY_ON;
	}

	/* all contents of S[0] can be moved into its neighbors S[0] will be
	   removed after balancing. */
	if (!h && is_leaf_removable(tb))
		return CARRY_ON;

	/* why do we perform this check here rather than earlier??
	   Answer: we can win 1 node in some cases above. Moreover we
	   checked it above, when we checked, that S[0] is not removable
	   in principle */
	if (sfree >= levbytes) {	/* new item fits into node S[h] without any shifting */
		if (!h)
			tb->s0num = vn->vn_nr_item;
		set_parameters(tb, h, 0, 0, 1, NULL, -1, -1);
		return NO_BALANCING_NEEDED;
	}

	{
		int lpar, rpar, nset, lset, rset, lrset;
		/*
		 * regular overflowing of the node
		 */

		/* get_num_ver works in 2 modes (FLOW & NO_FLOW) lpar, rpar - number
		   of items we can shift to left/right neighbor (including splitting
		   item) nset, lset, rset, lrset - shows, whether flowing items give
		   better packing */
#define FLOW 1
#define NO_FLOW 0		/* do not any splitting */

		/* we choose one the following */
#define NOTHING_SHIFT_NO_FLOW	0
#define NOTHING_SHIFT_FLOW	5
#define LEFT_SHIFT_NO_FLOW	10
#define LEFT_SHIFT_FLOW		15
#define RIGHT_SHIFT_NO_FLOW	20
#define RIGHT_SHIFT_FLOW	25
#define LR_SHIFT_NO_FLOW	30
#define LR_SHIFT_FLOW		35

		lpar = tb->lnum[h];
		rpar = tb->rnum[h];

		/* calculate number of blocks S[h] must be split into when nothing is
		   shifted to the neighbors, as well as number of items in each part
		   of the split node (s012 numbers), and number of bytes (s1bytes) of
		   the shared drop which flow to S1 if any */
		nset = NOTHING_SHIFT_NO_FLOW;
		nver = get_num_ver(vn->vn_mode, tb, h,
				   0, -1, h ? vn->vn_nr_item : 0, -1,
				   snum012, NO_FLOW);

		if (!h) {
			int nver1;

			/* note, that in this case we try to bottle between S[0] and S1
			   (S1 - the first new node) */
			nver1 = get_num_ver(vn->vn_mode, tb, h,
					    0, -1, 0, -1,
					    snum012 + NOTHING_SHIFT_FLOW, FLOW);
			if (nver > nver1)
				nset = NOTHING_SHIFT_FLOW, nver = nver1;
		}

		/* calculate number of blocks S[h] must be split into when l_shift_num
		   first items and l_shift_bytes of the right most liquid item to be
		   shifted are shifted to the left neighbor, as well as number of
		   items in each part of the split node (s012 numbers), and number
		   of bytes (s1bytes) of the shared drop which flow to S1 if any */
		lset = LEFT_SHIFT_NO_FLOW;
		lnver = get_num_ver(vn->vn_mode, tb, h,
				    lpar - ((h || tb->lbytes == -1) ? 0 : 1),
				    -1, h ? vn->vn_nr_item : 0, -1,
				    snum012 + LEFT_SHIFT_NO_FLOW, NO_FLOW);
		if (!h) {
			int lnver1;

			lnver1 = get_num_ver(vn->vn_mode, tb, h,
					     lpar -
					     ((tb->lbytes != -1) ? 1 : 0),
					     tb->lbytes, 0, -1,
					     snum012 + LEFT_SHIFT_FLOW, FLOW);
			if (lnver > lnver1)
				lset = LEFT_SHIFT_FLOW, lnver = lnver1;
		}

		/* calculate number of blocks S[h] must be split into when r_shift_num
		   first items and r_shift_bytes of the left most liquid item to be
		   shifted are shifted to the right neighbor, as well as number of
		   items in each part of the splitted node (s012 numbers), and number
		   of bytes (s1bytes) of the shared drop which flow to S1 if any */
		rset = RIGHT_SHIFT_NO_FLOW;
		rnver = get_num_ver(vn->vn_mode, tb, h,
				    0, -1,
				    h ? (vn->vn_nr_item - rpar) : (rpar -
								   ((tb->
								     rbytes !=
								     -1) ? 1 :
								    0)), -1,
				    snum012 + RIGHT_SHIFT_NO_FLOW, NO_FLOW);
		if (!h) {
			int rnver1;

			rnver1 = get_num_ver(vn->vn_mode, tb, h,
					     0, -1,
					     (rpar -
					      ((tb->rbytes != -1) ? 1 : 0)),
					     tb->rbytes,
					     snum012 + RIGHT_SHIFT_FLOW, FLOW);

			if (rnver > rnver1)
				rset = RIGHT_SHIFT_FLOW, rnver = rnver1;
		}

		/* calculate number of blocks S[h] must be split into when items are
		   shifted in both directions, as well as number of items in each part
		   of the splitted node (s012 numbers), and number of bytes (s1bytes)
		   of the shared drop which flow to S1 if any */
		lrset = LR_SHIFT_NO_FLOW;
		lrnver = get_num_ver(vn->vn_mode, tb, h,
				     lpar - ((h || tb->lbytes == -1) ? 0 : 1),
				     -1,
				     h ? (vn->vn_nr_item - rpar) : (rpar -
								    ((tb->
								      rbytes !=
								      -1) ? 1 :
								     0)), -1,
				     snum012 + LR_SHIFT_NO_FLOW, NO_FLOW);
		if (!h) {
			int lrnver1;

			lrnver1 = get_num_ver(vn->vn_mode, tb, h,
					      lpar -
					      ((tb->lbytes != -1) ? 1 : 0),
					      tb->lbytes,
					      (rpar -
					       ((tb->rbytes != -1) ? 1 : 0)),
					      tb->rbytes,
					      snum012 + LR_SHIFT_FLOW, FLOW);
			if (lrnver > lrnver1)
				lrset = LR_SHIFT_FLOW, lrnver = lrnver1;
		}

		/* Our general shifting strategy is:
		   1) to minimized number of new nodes;
		   2) to minimized number of neighbors involved in shifting;
		   3) to minimized number of disk reads; */

		/* we can win TWO or ONE nodes by shifting in both directions */
		if (lrnver < lnver && lrnver < rnver) {
			if (lrset == LR_SHIFT_FLOW)
				set_parameters(tb, h, tb->lnum[h], tb->rnum[h],
					       lrnver, snum012 + lrset,
					       tb->lbytes, tb->rbytes);
			else
				set_parameters(tb, h,
					       tb->lnum[h] -
					       ((tb->lbytes == -1) ? 0 : 1),
					       tb->rnum[h] -
					       ((tb->rbytes == -1) ? 0 : 1),
					       lrnver, snum012 + lrset, -1, -1);

			return CARRY_ON;
		}

		/* if shifting doesn't lead to better packing then don't shift */
		if (nver == lrnver) {
			set_parameters(tb, h, 0, 0, nver, snum012 + nset, -1,
				       -1);
			return CARRY_ON;
		}

		/* now we know that for better packing shifting in only one direction
		   either to the left or to the right is required */

		/*  if shifting to the left is better than shifting to the right */
		if (lnver < rnver) {
			SET_PAR_SHIFT_LEFT;
			return CARRY_ON;
		}

		/* if shifting to the right is better than shifting to the left */
		if (lnver > rnver) {
			SET_PAR_SHIFT_RIGHT;
			return CARRY_ON;
		}

		/* now shifting in either direction gives the same number of nodes and
		   we can make use of the cached neighbors */
		if (is_left_neighbor_in_cache(tb, h)) {
			SET_PAR_SHIFT_LEFT;
			return CARRY_ON;
		}

		/* shift to the right independently on whether the right neighbor in
		   cache or not */
		SET_PAR_SHIFT_RIGHT;
		return CARRY_ON;
	}
}

/* Check whether current node S[h] is balanced when Decreasing its size by
 * Deleting or Cutting for INTERNAL node of internal tree.
 * Calculate parameters for balancing for current level h.
 * Parameters:
 *	tb	tree_balance structure;
 *	h	current level of the node;
 *	inum	item number in S[h];
 *	mode	i - insert, p - paste;
 * Returns:	1 - schedule occured;
 *	        0 - balancing for higher levels needed;
 *	       -1 - no balancing for higher levels needed;
 *	       -2 - no disk space.
 *
 * Note: Items of internal nodes have fixed size, so the balance condition for
 * the internal part of internal tree is as for the B-trees.
 */
static int dc_check_balance_internal(struct tree_balance *tb, int h)
{
	struct virtual_node *vn = tb->tb_vn;

	/* Sh is the node whose balance is currently being checked,
	   and Fh is its father.  */
	struct buffer_head *Sh, *Fh;
	int n_ret_value;
	int lfree, rfree /* free space in L and R */ ;

	Sh = PATH_H_PBUFFER(tb->tb_path, h);
	Fh = PATH_H_PPARENT(tb->tb_path, h);

	/* using tb->insert_size[h], which is negative in this case,
	   create_virtual_node calculates: new_nr_item = number of items node
	   would have if operation is performed without balancing (new_nr_item); */
	create_virtual_node(tb, h);

	if (!Fh) {
		/* S[h] is the root. */
		if (vn->vn_nr_item > 0) {
			set_parameters(tb, h, 0, 0, 1, NULL, -1, -1);
			return NO_BALANCING_NEEDED;	/* no balancing for higher levels needed */
		}
		/* new_nr_item == 0.
		 * Current root will be deleted resulting in
		 * decrementing the tree height. */
		set_parameters(tb, h, 0, 0, 0, NULL, -1, -1);
		return CARRY_ON;
	}

	if ((n_ret_value = get_parents(tb, h)) != CARRY_ON)
		return n_ret_value;

	/* get free space of neighbors */
	rfree = get_rfree(tb, h);
	lfree = get_lfree(tb, h);

	/* determine maximal number of items we can fit into neighbors */

	check_left(tb, h, lfree);
	check_right(tb, h, rfree);

	if (vn->vn_nr_item >= MIN_NR_KEY(Sh)) {
		/* Balance condition for the internal node is valid. In this case we
		 * balance only if it leads to better packing. */
		if (vn->vn_nr_item == MIN_NR_KEY(Sh)) {
			/* Here we join S[h] with one of its neighbors, which is
			 * impossible with greater values of new_nr_item. */
			if (tb->lnum[h] >= vn->vn_nr_item + 1) {
				/* All contents of S[h] can be moved to L[h]. */
				int n;
				int order_L;

				order_L =
				    ((n =
				      PATH_H_B_ITEM_ORDER(tb->tb_path,
							  h)) ==
				     0) ? B_NR_ITEMS(tb->FL[h]) : n - 1;
				n = get_dc_child_size(B_N_CHILD
						      (tb->FL[h],
						       order_L)) / (DC_SIZE +
								    KEY_SIZE);
				set_parameters(tb, h, -n - 1, 0, 0, NULL, -1,
					       -1);
				return CARRY_ON;
			}

			if (tb->rnum[h] >= vn->vn_nr_item + 1) {
				/* All contents of S[h] can be moved to R[h]. */
				int n;
				int order_R;

				order_R =
				    ((n =
				      PATH_H_B_ITEM_ORDER(tb->tb_path,
							  h)) ==
				     B_NR_ITEMS(Fh)) ? 0 : n + 1;
				n = get_dc_child_size(B_N_CHILD
						      (tb->FR[h],
						       order_R)) / (DC_SIZE +
								    KEY_SIZE);
				set_parameters(tb, h, 0, -n - 1, 0, NULL, -1,
					       -1);
				return CARRY_ON;
			}
		}

		if (tb->rnum[h] + tb->lnum[h] >= vn->vn_nr_item + 1) {
			/* All contents of S[h] can be moved to the neighbors (L[h] &
			   R[h]). */
			int to_r;

			to_r =
			    ((MAX_NR_KEY(Sh) << 1) + 2 - tb->lnum[h] -
			     tb->rnum[h] + vn->vn_nr_item + 1) / 2 -
			    (MAX_NR_KEY(Sh) + 1 - tb->rnum[h]);
			set_parameters(tb, h, vn->vn_nr_item + 1 - to_r, to_r,
				       0, NULL, -1, -1);
			return CARRY_ON;
		}

		/* Balancing does not lead to better packing. */
		set_parameters(tb, h, 0, 0, 1, NULL, -1, -1);
		return NO_BALANCING_NEEDED;
	}

	/* Current node contain insufficient number of items. Balancing is
	   required.  Check whether we can merge S[h] with left neighbor. */
	if (tb->lnum[h] >= vn->vn_nr_item + 1)
		if (is_left_neighbor_in_cache(tb, h)
		    || tb->rnum[h] < vn->vn_nr_item + 1 || !tb->FR[h]) {
			int n;
			int order_L;

			order_L =
			    ((n =
			      PATH_H_B_ITEM_ORDER(tb->tb_path,
						  h)) ==
			     0) ? B_NR_ITEMS(tb->FL[h]) : n - 1;
			n = get_dc_child_size(B_N_CHILD(tb->FL[h], order_L)) /
			    (DC_SIZE + KEY_SIZE);
			set_parameters(tb, h, -n - 1, 0, 0, NULL, -1, -1);
			return CARRY_ON;
		}

	/* Check whether we can merge S[h] with right neighbor. */
	if (tb->rnum[h] >= vn->vn_nr_item + 1) {
		int n;
		int order_R;

		order_R =
		    ((n =
		      PATH_H_B_ITEM_ORDER(tb->tb_path,
					  h)) == B_NR_ITEMS(Fh)) ? 0 : (n + 1);
		n = get_dc_child_size(B_N_CHILD(tb->FR[h], order_R)) /
		    (DC_SIZE + KEY_SIZE);
		set_parameters(tb, h, 0, -n - 1, 0, NULL, -1, -1);
		return CARRY_ON;
	}

	/* All contents of S[h] can be moved to the neighbors (L[h] & R[h]). */
	if (tb->rnum[h] + tb->lnum[h] >= vn->vn_nr_item + 1) {
		int to_r;

		to_r =
		    ((MAX_NR_KEY(Sh) << 1) + 2 - tb->lnum[h] - tb->rnum[h] +
		     vn->vn_nr_item + 1) / 2 - (MAX_NR_KEY(Sh) + 1 -
						tb->rnum[h]);
		set_parameters(tb, h, vn->vn_nr_item + 1 - to_r, to_r, 0, NULL,
			       -1, -1);
		return CARRY_ON;
	}

	/* For internal nodes try to borrow item from a neighbor */
	/* Borrow one or two items from caching neighbor */
	if (is_left_neighbor_in_cache(tb, h) || !tb->FR[h]) {
		int from_l;

		from_l =
		    (MAX_NR_KEY(Sh) + 1 - tb->lnum[h] + vn->vn_nr_item +
		     1) / 2 - (vn->vn_nr_item + 1);
		set_parameters(tb, h, -from_l, 0, 1, NULL, -1, -1);
		return CARRY_ON;
	}

	set_parameters(tb, h, 0,
		       -((MAX_NR_KEY(Sh) + 1 - tb->rnum[h] + vn->vn_nr_item +
			  1) / 2 - (vn->vn_nr_item + 1)), 1, NULL, -1, -1);
	return CARRY_ON;
}

/* Check whether current node S[h] is balanced when Decreasing its size by
 * Deleting or Truncating for LEAF node of internal tree.
 * Calculate parameters for balancing for current level h.
 * Parameters:
 *	tb	tree_balance structure;
 *	h	current level of the node;
 *	inum	item number in S[h];
 *	mode	i - insert, p - paste;
 * Returns:	1 - schedule occured;
 *	        0 - balancing for higher levels needed;
 *	       -1 - no balancing for higher levels needed;
 *	       -2 - no disk space.
 */
static int dc_check_balance_leaf(struct tree_balance *tb, int h)
{
	struct virtual_node *vn = tb->tb_vn;

	/* the maximal item size */
	int n_ret_value;
	/* F0 is the parent of the node whose balance is currently being checked */
	struct buffer_head *F0;
	int lfree, rfree /* free space in L and R */ ;

	F0 = PATH_H_PPARENT(tb->tb_path, 0);

	if (!F0) {
		/* S[0] is the root now. */
		set_parameters(tb, h, 0, 0, 1, NULL, -1, -1);
		return NO_BALANCING_NEEDED;
	}

	if ((n_ret_value = get_parents(tb, h)) != CARRY_ON)
		return n_ret_value;

	/* get free space of neighbors */
	rfree = get_rfree(tb, h);
	lfree = get_lfree(tb, h);

	create_virtual_node(tb, h);

	/* if 3 leaves can be merge to one, set parameters and return */
	if (are_leaves_removable(tb, lfree, rfree))
		return CARRY_ON;

	/* determine maximal number of items we can shift to the left/right
	   neighbor and the maximal number of bytes that can flow to the
	   left/right neighbor from the left/right most liquid item that cannot be
	   shifted from S[0] entirely */
	check_left(tb, h, lfree);
	check_right(tb, h, rfree);

	/* check whether we can merge S with left neighbor. */
	if (tb->lnum[0] >= vn->vn_nr_item && tb->lbytes == -1)
		if (is_left_neighbor_in_cache(tb, h) || ((tb->rnum[0] - ((tb->rbytes == -1) ? 0 : 1)) < vn->vn_nr_item) ||	/* S can not be merged with R */
		    !tb->FR[h]) {
			/* set parameter to merge S[0] with its left neighbor */
			set_parameters(tb, h, -1, 0, 0, NULL, -1, -1);
			return CARRY_ON;
		}

	/* check whether we can merge S[0] with right neighbor. */
	if (tb->rnum[0] >= vn->vn_nr_item && tb->rbytes == -1) {
		set_parameters(tb, h, 0, -1, 0, NULL, -1, -1);
		return CARRY_ON;
	}

	/* All contents of S[0] can be moved to the neighbors (L[0] & R[0]). Set
	   parameters and return */
	if (is_leaf_removable(tb))
		return CARRY_ON;

	/* Balancing is not required. */
	tb->s0num = vn->vn_nr_item;
	set_parameters(tb, h, 0, 0, 1, NULL, -1, -1);
	return NO_BALANCING_NEEDED;
}

/* Check whether current node S[h] is balanced when Decreasing its size by
 * Deleting or Cutting.
 * Calculate parameters for balancing for current level h.
 * Parameters:
 *	tb	tree_balance structure;
 *	h	current level of the node;
 *	inum	item number in S[h];
 *	mode	d - delete, c - cut.
 * Returns:	1 - schedule occured;
 *	        0 - balancing for higher levels needed;
 *	       -1 - no balancing for higher levels needed;
 *	       -2 - no disk space.
 */
static int dc_check_balance(struct tree_balance *tb, int h)
{
	if (h)
		return dc_check_balance_internal(tb, h);
	else
		return dc_check_balance_leaf(tb, h);
}

/* Check whether current node S[h] is balanced.
 * Calculate parameters for balancing for current level h.
 * Parameters:
 *
 *	tb	tree_balance structure:
 *
 *              tb is a large structure that must be read about in the header file
 *              at the same time as this procedure if the reader is to successfully
 *              understand this procedure
 *
 *	h	current level of the node;
 *	inum	item number in S[h];
 *	mode	i - insert, p - paste, d - delete, c - cut.
 * Returns:	1 - schedule occured;
 *	        0 - balancing for higher levels needed;
 *	       -1 - no balancing for higher levels needed;
 *	       -2 - no disk space.
 */
static int check_balance(int mode, struct tree_balance *tb,
			 int h, int inum, int pos_in_item,
			 struct item_head *ins_ih)
{
	struct virtual_node *vn;

	vn = tb->tb_vn = (struct virtual_node *)(tb->vn_buf);	// + ROUND_UP(SB_BMAP_NR (tb->tb_fs) * 2 / 8 + 1, 4));
	vn->vn_free_ptr = (char *)(tb->tb_vn + 1);
	vn->vn_mode = mode;
	vn->vn_affected_item_num = inum;
	vn->vn_pos_in_item = pos_in_item;
	vn->vn_ins_ih = ins_ih;

	if (tb->insert_size[h] > 0)
		/* Calculate balance parameters when size of node is increasing. */
		return ip_check_balance(tb, h);

	/* Calculate balance parameters when  size of node is decreasing. */
	return dc_check_balance(tb, h);
}

/* Check whether parent at the path is the really parent of the current node.*/
static void get_direct_parent(struct tree_balance *p_s_tb, int n_h)
{
	struct buffer_head *p_s_bh;
	struct reiserfs_path *p_s_path = p_s_tb->tb_path;
	int n_position,
	    n_path_offset = PATH_H_PATH_OFFSET(p_s_tb->tb_path, n_h);

	/* We are in the root or in the new root. */
	if (n_path_offset <= FIRST_PATH_ELEMENT_OFFSET) {
		struct reiserfs_super_block *sb;

		if (n_path_offset < FIRST_PATH_ELEMENT_OFFSET - 1)
			reiserfs_panic
			    ("PAP-8260: get_direct_parent: illegal offset in the path");

		sb = p_s_tb->tb_fs->fs_ondisk_sb;
		if (PATH_OFFSET_PBUFFER(p_s_path, FIRST_PATH_ELEMENT_OFFSET)->
		    b_blocknr == get_sb_root_block(sb)) {
			/* Root is not changed. */
			PATH_OFFSET_PBUFFER(p_s_path, n_path_offset - 1) = NULL;
			PATH_OFFSET_POSITION(p_s_path, n_path_offset - 1) = 0;
			return;
		}
		reiserfs_panic("get_direct_parent: root changed");
	}

	if (!B_IS_IN_TREE
	    (p_s_bh = PATH_OFFSET_PBUFFER(p_s_path, n_path_offset - 1)))
		reiserfs_panic
		    ("get_direct_parent: parent in the path is not in the tree");

	if ((n_position =
	     PATH_OFFSET_POSITION(p_s_path,
				  n_path_offset - 1)) > B_NR_ITEMS(p_s_bh))
		reiserfs_panic("get_direct_parent: wrong position in the path");

	if (get_dc_child_blocknr(B_N_CHILD(p_s_bh, n_position)) !=
	    PATH_OFFSET_PBUFFER(p_s_path, n_path_offset)->b_blocknr)
		reiserfs_panic
		    ("get_direct_parent: parent in the path is not parent "
		     "of the current node in the tree");

	return;			/* Parent in the path is unlocked and really parent of the current node.  */
}

/* Using lnum[n_h] and rnum[n_h] we should determine what neighbors
 * of S[n_h] we
 * need in order to balance S[n_h], and get them if necessary.
 * Returns:	SCHEDULE_OCCURRED - schedule occured while the function worked;
 *	        CARRY_ON - schedule didn't occur while the function worked;
 */
static int get_neighbors(struct tree_balance *p_s_tb, int n_h)
{
	int n_child_position,
	    n_path_offset = PATH_H_PATH_OFFSET(p_s_tb->tb_path, n_h + 1);
	unsigned long n_son_number;
	reiserfs_filsys_t fs = p_s_tb->tb_fs;
	struct buffer_head *p_s_bh;
	/*struct virtual_node * vn = p_s_tb->tb_vn; */

	if (p_s_tb->lnum[n_h]) {
		/* We need left neighbor to balance S[n_h]. */
		p_s_bh = PATH_OFFSET_PBUFFER(p_s_tb->tb_path, n_path_offset);

		n_child_position =
		    (p_s_bh ==
		     p_s_tb->FL[n_h]) ? p_s_tb->
		    lkey[n_h] : get_blkh_nr_items(B_BLK_HEAD(p_s_tb->FL[n_h]));
		n_son_number =
		    get_dc_child_blocknr(B_N_CHILD
					 (p_s_tb->FL[n_h], n_child_position));
		p_s_bh = bread(fs->fs_dev, n_son_number, fs->fs_blocksize);
		if (!p_s_bh)
			return IO_ERROR;
		brelse(p_s_tb->L[n_h]);
		p_s_tb->L[n_h] = p_s_bh;
	}

	if (p_s_tb->rnum[n_h]) {	/* We need right neighbor to balance S[n_path_offset]. */
		p_s_bh = PATH_OFFSET_PBUFFER(p_s_tb->tb_path, n_path_offset);
		n_child_position =
		    (p_s_bh == p_s_tb->FR[n_h]) ? p_s_tb->rkey[n_h] + 1 : 0;
		n_son_number =
		    get_dc_child_blocknr(B_N_CHILD
					 (p_s_tb->FR[n_h], n_child_position));
		p_s_bh = bread(fs->fs_dev, n_son_number, fs->fs_blocksize);
		if (!p_s_bh)
			return IO_ERROR;

		brelse(p_s_tb->R[n_h]);
		p_s_tb->R[n_h] = p_s_bh;
	}
	return CARRY_ON;
}

static int get_mem_for_virtual_node(struct tree_balance *tb)
{
	tb->vn_buf = getmem(tb->tb_fs->fs_blocksize);
	return CARRY_ON;
}

static void free_virtual_node_mem(struct tree_balance *tb)
{
	freemem(tb->vn_buf);
}

/* Prepare for balancing, that is
 *	get all necessary parents, and neighbors;
 *	analyze what and where should be moved;
 *	get sufficient number of new nodes;
 * Balancing will start only after all resources will be collected at a time.
 *
 * When ported to SMP kernels, only at the last moment after all needed nodes
 * are collected in cache, will the resources be locked using the usual
 * textbook ordered lock acquisition algorithms.  Note that ensuring that
 * this code neither write locks what it does not need to write lock nor locks out of order
 * will be a pain in the butt that could have been avoided.  Grumble grumble. -Hans
 *
 * fix is meant in the sense of render unchanging
 *
 * Latency might be improved by first gathering a list of what buffers are needed
 * and then getting as many of them in parallel as possible? -Hans
 *
 * Parameters:
 *	op_mode	i - insert, d - delete, c - cut (truncate), p - paste (append)
 *	tb	tree_balance structure;
 *	inum	item number in S[h];
 *      pos_in_item - comment this if you can
 *      ins_ih & ins_sd are used when inserting
 * Returns:	1 - schedule occurred while the function worked;
 *	        0 - schedule didn't occur while the function worked;
 *             -1 - if no_disk_space
 */

int fix_nodes(int n_op_mode, struct tree_balance *p_s_tb,
	      struct item_head *p_s_ins_ih)
{
	int n_pos_in_item = p_s_tb->tb_path->pos_in_item;
	int n_ret_value, n_h, n_item_num = get_item_pos(p_s_tb->tb_path);
	/*    struct buffer_head  * p_s_tbS0 = get_bh (p_s_tb->tb_path); */
	/*    struct item_head * ih = get_ih (p_s_tb->tb_path); */

	if (get_mem_for_virtual_node(p_s_tb) != CARRY_ON)
		reiserfs_panic("fix_nodes: no memory for virtual node");

	/* Starting from the leaf level; for all levels n_h of the tree. */
	for (n_h = 0; n_h < MAX_HEIGHT && p_s_tb->insert_size[n_h]; n_h++) {
		get_direct_parent(p_s_tb, n_h);

		if ((n_ret_value =
		     check_balance( /*th, */ n_op_mode, p_s_tb, n_h, n_item_num,
				   n_pos_in_item, p_s_ins_ih)) != CARRY_ON) {
			if (n_ret_value == NO_BALANCING_NEEDED) {
				/* No balancing for higher levels needed. */
				if ((n_ret_value =
				     get_neighbors(p_s_tb, n_h)) != CARRY_ON) {
					return n_ret_value;
				}
				if (n_h != MAX_HEIGHT - 1)
					p_s_tb->insert_size[n_h + 1] = 0;
				/* ok, analysis and resource gathering are complete */
				break;
			}

			return n_ret_value;
		}

		if ((n_ret_value = get_neighbors(p_s_tb, n_h)) != CARRY_ON) {
			return n_ret_value;
		}

		if ((n_ret_value =
		     get_empty_nodes( /*th, */ p_s_tb, n_h)) != CARRY_ON) {
			return n_ret_value;	/* No disk space */
		}

		if (!PATH_H_PBUFFER(p_s_tb->tb_path, n_h)) {
			/* We have a positive insert size but no nodes exist on this
			   level, this means that we are creating a new root. */
			if (n_h < MAX_HEIGHT - 1)
				p_s_tb->insert_size[n_h + 1] = 0;
		} else if (!PATH_H_PBUFFER(p_s_tb->tb_path, n_h + 1)) {
			if (p_s_tb->blknum[n_h] > 1) {
				/* The tree needs to be grown, so this node S[n_h] which
				   is the root node is split into two nodes, and a new
				   node (S[n_h+1]) will be created to become the root
				   node.  */
				p_s_tb->insert_size[n_h + 1] =
				    (DC_SIZE +
				     KEY_SIZE) * (p_s_tb->blknum[n_h] - 1) +
				    DC_SIZE;
			} else if (n_h < MAX_HEIGHT - 1)
				p_s_tb->insert_size[n_h + 1] = 0;
		} else
			p_s_tb->insert_size[n_h + 1] =
			    (DC_SIZE + KEY_SIZE) * (p_s_tb->blknum[n_h] - 1);
	}

	return CARRY_ON;	/* go ahead and balance */
}

void unfix_nodes(struct tree_balance *p_s_tb)
{
	struct reiserfs_path *p_s_path = p_s_tb->tb_path;
	int n_counter;
	//  int i, j;
	//struct buffer_head * bh;

	/* Release path buffers. */
	pathrelse(p_s_path);

	for (n_counter = 0; n_counter < MAX_HEIGHT; n_counter++) {
		/* Release fathers and neighbors. */
		brelse(p_s_tb->L[n_counter]);
		brelse(p_s_tb->R[n_counter]);
		brelse(p_s_tb->FL[n_counter]);
		brelse(p_s_tb->FR[n_counter]);
		brelse(p_s_tb->CFL[n_counter]);
		brelse(p_s_tb->CFR[n_counter]);
	}

	/* Could be optimized. Will be done by PAP someday */
	for (n_counter = 0; n_counter < MAX_FEB_SIZE; n_counter++) {
		if (p_s_tb->FEB[n_counter]) {
			/* release what was not used */
			reiserfs_free_block(p_s_tb->tb_fs,
					    p_s_tb->FEB[n_counter]->b_blocknr);

			bforget(p_s_tb->FEB[n_counter]);
			/* tree balance bitmap of bitmaps has bit set already */
		}
		/* release used as new nodes including a new root */
		brelse(p_s_tb->used[n_counter]);
	}

	free_virtual_node_mem(p_s_tb);
}
