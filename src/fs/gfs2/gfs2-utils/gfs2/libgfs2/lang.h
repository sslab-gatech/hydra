#ifndef LANG_H
#define LANG_H
#include <stdint.h>
#include "libgfs2.h"

struct lgfs2_lang_state {
	int ls_colnum;
	int ls_linenum;
	int ls_errnum;
	struct ast_node *ls_ast_root;
	struct ast_node *ls_ast_tail;
	struct ast_node *ls_interp_curr;
};

typedef enum {
	AST_NONE,
	// Statements
	AST_ST_SET,
	AST_ST_GET,

	_AST_EX_START,
	// Expressions
	AST_EX_ID,
	AST_EX_NUMBER,
	AST_EX_STRING,
	AST_EX_ADDRESS,
	AST_EX_PATH,
	AST_EX_SUBSCRIPT,
	AST_EX_OFFSET,
	AST_EX_BLOCKSPEC,
	AST_EX_STRUCTSPEC,
	AST_EX_FIELDSPEC,
	AST_EX_TYPESPEC,

	// Keywords
	AST_KW_STATE,
} ast_node_t;

enum {
	AST_INTERP_SUCCESS	= 0, // Success
	AST_INTERP_FAIL		= 1, // Failure
	AST_INTERP_INVAL	= 2, // Invalid field/type mismatch
	AST_INTERP_ERR		= 3, // Something went wrong, see errno
};

extern const char* ast_type_string[];

struct ast_node {
	ast_node_t ast_type;
	struct ast_node *ast_left;
	struct ast_node *ast_right;
	char *ast_text;
	char *ast_str;
	uint64_t ast_num;
};

extern struct ast_node *ast_new(ast_node_t type, const char *text);
extern void ast_destroy(struct ast_node **val);

#define YYSTYPE struct ast_node *

#endif /* LANG_H */
