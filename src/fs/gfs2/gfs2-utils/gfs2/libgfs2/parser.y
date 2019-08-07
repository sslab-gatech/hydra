%code requires {
/* Required to break a circular dependency introduced with bison 2.6 */
typedef void* yyscan_t;
}
%code top {
#include <errno.h>
#include "lang.h"
#include "lexer.h"

static int yyerror(struct lgfs2_lang_state *state, yyscan_t lexer, const char *errorstr)
{
	fprintf(stderr, "%d:%d: %s\n", state->ls_linenum, state->ls_colnum, errorstr);
	return 1;
}

}
%defines
%debug
%define api.pure
%parse-param { struct lgfs2_lang_state *state }
%parse-param { yyscan_t lexer }
%lex-param { yyscan_t lexer }
%start script
%token TOK_COLON
%token TOK_COMMA
%token TOK_ID
%token TOK_LBRACE
%token TOK_LBRACKET
%token TOK_NUMBER
%token TOK_OFFSET
%token TOK_RBRACE
%token TOK_RBRACKET
%token TOK_SEMI
%token TOK_SET
%token TOK_GET
%token TOK_STATE
%token TOK_STRING
%token TOK_PATH
%%
script:	statements {
		state->ls_ast_root = $1;
		state->ls_interp_curr = $1;
	}
	| statements TOK_SEMI {
		state->ls_ast_root = $1;
		state->ls_interp_curr = $1;
	}
;
statements: statements TOK_SEMI statement {
		state->ls_ast_tail->ast_left = $3;
		state->ls_ast_tail = $3;
		$$ = $1;
	}
	| statement {
		if (state->ls_ast_tail == NULL)
			state->ls_ast_tail = $1;
		$$ = $1;
	}
;
statement: set_stmt { $$ = $1;}
	| get_stmt { $$ = $1; }
;
set_stmt: TOK_SET blockspec structspec {
		$1->ast_right = $2;
		$2->ast_right = $3;
		$$ = $1;
	}
	| TOK_SET blockspec typespec structspec {
		$1->ast_right = $2;
		$2->ast_right = $3;
		$3->ast_right = $4;
		$$ = $1;
	}
;
get_stmt: TOK_GET blockspec {
		$1->ast_right = $2; $$ = $1;
	}
	| TOK_GET blockspec TOK_STATE {
		$1->ast_right = $2;
		$2->ast_right = $3;
		$$ = $1;
	}
;
blockspec: offset { $$ = $1; }
	| address { $$ = $1; }
	| path { $$ = $1; }
	| block_literal { $$ = $1; }
	| subscript { $$ = $1; }
;
offset:	blockspec TOK_OFFSET {
		$2->ast_left = $1;
		$$ = $2;
	}
;
typespec: identifier {
		$1->ast_type = AST_EX_TYPESPEC;
		$$ = $1;
	}
;
block_literal: identifier { $$ = $1; }
;
subscript: block_literal TOK_LBRACKET index TOK_RBRACKET {
		$4->ast_left = $1;
		$1->ast_left = $3;
		$$ = $4;
	}
;
index: number { $$ = $1; }
	| identifier { $$ = $1; }
;
address: number {
		$1->ast_type = AST_EX_ADDRESS;
		$$ = $1;
	 }
;
structspec: TOK_LBRACE fieldspecs TOK_RBRACE { $$ = $2; }
	| TOK_LBRACE TOK_RBRACE { $$ = NULL; }
;
fieldspecs: fieldspecs TOK_COMMA fieldspec {
		$1->ast_left = $3;
		$$ = $1;
	}
	| fieldspec { $$ = $1; }
;
fieldspec: identifier TOK_COLON fieldvalue {
		$2->ast_right = $1;
		$1->ast_right = $3;
		$$ = $2;
	}
;
fieldvalue: number { $$ = $1; }
	| string { $$ = $1; }
;
number: TOK_NUMBER { $$ = $1; }
string: TOK_STRING { $$ = $1; }
identifier: TOK_ID { $$ = $1; }
path: TOK_PATH { $$ = $1; }
%%

/**
 * Allocate and initialize a new parse state structure. The caller must free the
 * memory returned by this function.
 */
struct lgfs2_lang_state *lgfs2_lang_init(void)
{
	struct lgfs2_lang_state *state;
	state = calloc(1, sizeof(struct lgfs2_lang_state));
	if (state == NULL) {
		return NULL;
	}
	state->ls_linenum = 1;
	return state;
}

void lgfs2_lang_free(struct lgfs2_lang_state **state)
{
	ast_destroy(&(*state)->ls_ast_root);
	free(*state);
	*state = NULL;
}

int lgfs2_lang_parsef(struct lgfs2_lang_state *state, FILE *src)
{
	int ret = 0;
	yyscan_t lexer;

	ret = yylex_init_extra(state, &lexer);
	if (ret != 0) {
		fprintf(stderr, "Failed to initialize lexer.\n");
		return ret;
	}

	yyset_in(src, lexer);
	ret = yyparse(state, lexer);
	yylex_destroy(lexer);
	return ret;
}

int lgfs2_lang_parses(struct lgfs2_lang_state *state, const char *cstr)
{
	int ret;
	FILE *src;
	char *str = strdup(cstr);

	if (str == NULL) {
		perror("Failed to duplicate source string");
		return 1;
	}
	src = fmemopen(str, strlen(str), "r");
	if (src == NULL) {
		perror("Failed to open string as source file");
		free(str);
		return 1;
	}
	ret = lgfs2_lang_parsef(state, src);
	fclose(src);
	free(str);
	if (ret != 0 || state->ls_errnum != 0) {
		return 1;
	}
	return 0;
}
