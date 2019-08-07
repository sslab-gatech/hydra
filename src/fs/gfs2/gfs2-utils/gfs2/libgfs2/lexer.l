%{
#include "lang.h"
#include "parser.h"

#define EXTRA ((struct lgfs2_lang_state *)yyextra)

#define P(token, type, text) do {\
	*(yylval) = ast_new(type, text);\
	if (*(yylval) == NULL) {\
		EXTRA->ls_errnum = errno;\
		return 1;\
	}\
	return (TOK_##token);\
} while(0)

#define COLNUM EXTRA->ls_colnum
#define YY_USER_ACTION COLNUM += yyleng;

%}
%option bison-bridge reentrant
%option warn debug
%option nounput noinput
%option noyywrap
%option extra-type="struct lgfs2_lang_state *"

letter			[a-zA-Z_]
decdigit		[0-9]
decnumber		-?{decdigit}+
hexdigit		[0-9a-fA-F]
hexnumber		-?0x{hexdigit}+
number			({decnumber}|{hexnumber})
offset			\+{number}
id			{letter}({letter}|{decdigit}|\.)*
string			\'([^\']|\\\')*\'
path			\'\/([^\']|\\\')*\'
ccomment		\/\/.*\n
shcomment		\#.*\n
comment			({ccomment}|{shcomment})
whitespace		[ \t\r]+

%%

\{			{
			return TOK_LBRACE;
			}
\}			{
			return TOK_RBRACE;
			}
\[			{
			return TOK_LBRACKET;
			}
\]			{
			P(RBRACKET, AST_EX_SUBSCRIPT, "[ ]");
			}
\,			{
			return TOK_COMMA;
			}
\:			{
			P(COLON, AST_EX_FIELDSPEC, yytext);
			}
\;			{
			return TOK_SEMI;
			}
set			{
			P(SET, AST_ST_SET, yytext);
			}
get			{
			P(GET, AST_ST_GET, yytext);
			}
state			{
			P(STATE, AST_KW_STATE, yytext);
			}
{path}			{
			yytext[yyleng-1] = '\0';
			P(PATH, AST_EX_PATH, yytext + 1);
			}
{string}		{
			yytext[yyleng-1] = '\0';
			P(STRING, AST_EX_STRING, yytext + 1);
			}
{offset}		{
			P(OFFSET, AST_EX_OFFSET, yytext);
			}
{number}		{
			P(NUMBER, AST_EX_NUMBER, yytext);
			}
{id}			{
			P(ID, AST_EX_ID, yytext);
			}
{comment}		{
			COLNUM = 0;
			EXTRA->ls_linenum++;
			}
<<EOF>>			{
			return 0;
			}
\n			{
			COLNUM = 0;
			EXTRA->ls_linenum++;
			}
{whitespace}		;
.			{
			printf("Unexpected character '%s' on line %d column %d\n",
			       yytext, yylineno, COLNUM);
			return 1;
			}

%%
