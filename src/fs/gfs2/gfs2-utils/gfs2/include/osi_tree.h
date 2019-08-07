#ifndef __OSI_RBTREE_DOT_H__
#define __OSI_RBTREE_DOT_H__

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

/* Adapted from the kernel's rbtree.c */
struct osi_node {
	unsigned long  osi_parent_color;
#define	OSI_RED		0
#define	OSI_BLACK	1
	struct osi_node *osi_left;
	struct osi_node *osi_right;
};

#define osi_parent(r)   ((struct osi_node *)((r)->osi_parent_color & ~3))
#define osi_color(r)   ((r)->osi_parent_color & 1)
#define osi_is_red(r)   (!osi_color(r))
#define osi_is_black(r) osi_color(r)
#define osi_set_red(r)  do { (r)->osi_parent_color &= ~1; } while (0)
#define osi_set_black(r)  do { (r)->osi_parent_color |= 1; } while (0)
#define OSI_EMPTY_NODE(node) (osi_parent(node) == node)

struct osi_root
{
	struct osi_node *osi_node;
};

#define OSI_EMPTY_ROOT(root)  ((root)->osi_node == NULL)

static inline void osi_set_parent(struct osi_node *rb, struct osi_node *p)
{
        rb->osi_parent_color = (rb->osi_parent_color & 3) | (unsigned long)p;
}

static inline void osi_set_color(struct osi_node *rb, int color)
{
	rb->osi_parent_color = (rb->osi_parent_color & ~1) | color;
}

static inline void osi_link_node(struct osi_node *node,
				 struct osi_node *parent,
				 struct osi_node **osi_link)
{
	node->osi_parent_color = (unsigned long )parent;
	node->osi_left = node->osi_right = NULL;

	*osi_link = node;
}

static inline void __osi_rotate_left(struct osi_node *node,
				     struct osi_root *root)
{
	struct osi_node *right = node->osi_right;
	struct osi_node *parent = osi_parent(node);

	if ((node->osi_right = right->osi_left))
		osi_set_parent(right->osi_left, node);
	right->osi_left = node;

	osi_set_parent(right, parent);

	if (parent) {
		if (node == parent->osi_left)
			parent->osi_left = right;
		else
			parent->osi_right = right;
	}
	else
		root->osi_node = right;
	osi_set_parent(node, right);
}

static inline void __osi_rotate_right(struct osi_node *node,
				      struct osi_root *root)
{
	struct osi_node *left = node->osi_left;
	struct osi_node *parent = osi_parent(node);

	if ((node->osi_left = left->osi_right))
		osi_set_parent(left->osi_right, node);
	left->osi_right = node;

	osi_set_parent(left, parent);

	if (parent) {
		if (node == parent->osi_right)
			parent->osi_right = left;
		else
			parent->osi_left = left;
	} else
		root->osi_node = left;
	osi_set_parent(node, left);
}

static inline void osi_insert_color(struct osi_node *node,
				    struct osi_root *root)
{
	struct osi_node *parent, *gparent;

	while ((parent = osi_parent(node)) && osi_is_red(parent)) {
		gparent = osi_parent(parent);

		if (parent == gparent->osi_left) {
			{
				register struct osi_node *uncle = gparent->osi_right;
				if (uncle && osi_is_red(uncle)) {
					osi_set_black(uncle);
					osi_set_black(parent);
					osi_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			if (parent->osi_right == node) {
				register struct osi_node *tmp;

				__osi_rotate_left(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			osi_set_black(parent);
			osi_set_red(gparent);
			__osi_rotate_right(gparent, root);
		} else {
			{
				register struct osi_node *uncle = gparent->osi_left;
				if (uncle && osi_is_red(uncle)) {
					osi_set_black(uncle);
					osi_set_black(parent);
					osi_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			if (parent->osi_left == node) {
				register struct osi_node *tmp;
				__osi_rotate_right(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			osi_set_black(parent);
			osi_set_red(gparent);
			__osi_rotate_left(gparent, root);
		}
	}

	osi_set_black(root->osi_node);
}

static inline void __osi_erase_color(struct osi_node *node,
				     struct osi_node *parent,
				     struct osi_root *root)
{
	struct osi_node *other;

	while ((!node || osi_is_black(node)) && node != root->osi_node) {
		if (parent->osi_left == node) {
			other = parent->osi_right;
			if (osi_is_red(other)) {
				osi_set_black(other);
				osi_set_red(parent);
				__osi_rotate_left(parent, root);
				other = parent->osi_right;
			}
			if ((!other->osi_left || osi_is_black(other->osi_left)) &&
			    (!other->osi_right || osi_is_black(other->osi_right)))
			{
				osi_set_red(other);
				node = parent;
				parent = osi_parent(node);
			} else {
				if (!other->osi_right || osi_is_black(other->osi_right))
				{
					struct osi_node *o_left;
					if ((o_left = other->osi_left))
						osi_set_black(o_left);
					osi_set_red(other);
					__osi_rotate_right(other, root);
					other = parent->osi_right;
				}
				osi_set_color(other, osi_color(parent));
				osi_set_black(parent);
				if (other->osi_right)
					osi_set_black(other->osi_right);
				__osi_rotate_left(parent, root);
				node = root->osi_node;
				break;
			}
		} else {
			other = parent->osi_left;
			if (osi_is_red(other)) {
				osi_set_black(other);
				osi_set_red(parent);
				__osi_rotate_right(parent, root);
				other = parent->osi_left;
			}
			if ((!other->osi_left || osi_is_black(other->osi_left)) &&
			    (!other->osi_right || osi_is_black(other->osi_right)))
			{
				osi_set_red(other);
				node = parent;
				parent = osi_parent(node);
			} else {
				if (!other->osi_left || osi_is_black(other->osi_left))
				{
					register struct osi_node *o_right;
					if ((o_right = other->osi_right))
						osi_set_black(o_right);
					osi_set_red(other);
					__osi_rotate_left(other, root);
					other = parent->osi_left;
				}
				osi_set_color(other, osi_color(parent));
				osi_set_black(parent);
				if (other->osi_left)
					osi_set_black(other->osi_left);
				__osi_rotate_right(parent, root);
				node = root->osi_node;
				break;
			}
		}
	}
	if (node)
		osi_set_black(node);
}

static inline void osi_erase(struct osi_node *node, struct osi_root *root)
{
	struct osi_node *child, *parent;
	int color;

	if (!node->osi_left)
		child = node->osi_right;
	else if (!node->osi_right)
		child = node->osi_left;
	else {
		struct osi_node *old = node, *left;

		node = node->osi_right;
		while ((left = node->osi_left) != NULL)
			node = left;

		if (osi_parent(old)) {
			if (osi_parent(old)->osi_left == old)
				osi_parent(old)->osi_left = node;
			else
				osi_parent(old)->osi_right = node;
		} else
			root->osi_node = node;

		child = node->osi_right;
		parent = osi_parent(node);
		color = osi_color(node);

		if (parent == old) {
			parent = node;
		} else {
			if (child)
				osi_set_parent(child, parent);
			parent->osi_left = child;

			node->osi_right = old->osi_right;
			osi_set_parent(old->osi_right, node);
		}

		node->osi_parent_color = old->osi_parent_color;
		node->osi_left = old->osi_left;
		osi_set_parent(old->osi_left, node);

		goto color;
	}

	parent = osi_parent(node);
	color = osi_color(node);

	if (child)
		osi_set_parent(child, parent);
	if (parent)
	{
		if (parent->osi_left == node)
			parent->osi_left = child;
		else
			parent->osi_right = child;
	}
	else
		root->osi_node = child;

 color:
	if (color == OSI_BLACK)
		__osi_erase_color(child, parent, root);
}

/*
 * This function returns the first node (in sort order) of the tree.
 */
static inline struct osi_node *osi_first(struct osi_root *root)
{
	struct osi_node	*n;

	n = root->osi_node;
	if (!n)
		return NULL;
	while (n->osi_left)
		n = n->osi_left;
	return n;
}

static inline struct osi_node *osi_last(struct osi_root *root)
{
	struct osi_node	*n;

	n = root->osi_node;
	if (!n)
		return NULL;
	while (n->osi_right)
		n = n->osi_right;
	return n;
}

static inline struct osi_node *osi_next(struct osi_node *node)
{
	struct osi_node *parent;

	if (OSI_EMPTY_NODE(node))
		return NULL;

	/* If we have a right-hand child, go down and then left as far
	   as we can. */
	if (node->osi_right) {
		node = node->osi_right;
		while (node->osi_left)
			node=node->osi_left;
		return node;
	}

	/* No right-hand children.  Everything down and left is
	   smaller than us, so any 'next' node must be in the general
	   direction of our parent. Go up the tree; any time the
	   ancestor is a right-hand child of its parent, keep going
	   up. First time it's a left-hand child of its parent, said
	   parent is our 'next' node. */
	while ((parent = osi_parent(node)) && node == parent->osi_right)
		node = parent;

	return parent;
}

static inline struct osi_node *osi_prev(struct osi_node *node)
{
	struct osi_node *parent;

	if (OSI_EMPTY_NODE(node))
		return NULL;

	/* If we have a left-hand child, go down and then right as far
	   as we can. */
	if (node->osi_left) {
		node = node->osi_left;
		while (node->osi_right)
			node=node->osi_right;
		return node;
	}

	/* No left-hand children. Go up till we find an ancestor which
	   is a right-hand child of its parent */
	while ((parent = osi_parent(node)) && node == parent->osi_left)
		node = parent;

	return parent;
}

static inline void osi_replace_node(struct osi_node *victim,
				    struct osi_node *new,
				    struct osi_root *root)
{
	struct osi_node *parent = osi_parent(victim);

	/* Set the surrounding nodes to point to the replacement */
	if (parent) {
		if (victim == parent->osi_left)
			parent->osi_left = new;
		else
			parent->osi_right = new;
	} else {
		root->osi_node = new;
	}
	if (victim->osi_left)
		osi_set_parent(victim->osi_left, new);
	if (victim->osi_right)
		osi_set_parent(victim->osi_right, new);

	/* Copy the pointers/colour from the victim to the replacement */
	*new = *victim;
}

#endif
