%{
/*
 * mltlparser.y file
 * To generate the parser run: "bison mltlparser.y"
 */

#include "mltl_formula.h"
#include "mltlparser.h"
#include "mltllexer.h"

int yyerror(mtl_formula **formula, yyscan_t scanner, const char *msg) {
	fprintf (stderr, "\033[31mERROR\033[0m: %s\n", msg);
	exit(1);
	return 0;
}

%}

%code requires {
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}

%output  "mltlparser.c"
%defines "mltlparser.h"
 
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { mtl_formula **formula }
%parse-param { yyscan_t scanner }

%union {
	char* var_name;
	int num;
	interval *inter;
	mtl_formula *formula;
}

%left TOKEN_EQUIV
%left TOKEN_IMPLIES
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_RELEASE
%left TOKEN_WEAK_RELEASE
%left TOKEN_UNTIL
%left TOKEN_WEAK_UNTIL
%right TOKEN_FUTURE
%right TOKEN_GLOBALLY
%right TOKEN_NEXT
%right TOKEN_WEAK_NEXT
%right TOKEN_NOT

%token TOKEN_TRUE
%token TOKEN_FALSE
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LBRAKET
%token TOKEN_RBRAKET
%token TOKEN_COMMA
%token <var_name> TOKEN_VARIABLE
%token <num> TOKEN_INT

%type <inter> interval 
%type <formula> expr

%%

input
	: expr { *formula = $1; }
	;

expr
	: TOKEN_VARIABLE	{ $$ = create_var($1);	}
	| TOKEN_TRUE		{ $$ = create_operation( eTRUE, NULL, NULL, NULL );	}
	| TOKEN_FALSE		{ $$ = create_operation( eFALSE, NULL, NULL, NULL );	}
	| TOKEN_LPAREN expr TOKEN_RPAREN { $$ = $2;	}
	| expr TOKEN_EQUIV expr		{ $$ = create_operation( eEQUIV, $1, $3, NULL );		}
	| expr TOKEN_IMPLIES expr	{ $$ = create_operation( eIMPLIES, $1, $3, NULL );	}
	| expr TOKEN_OR expr		{ $$ = create_operation( eOR, $1, $3, NULL );			}
	| expr TOKEN_AND expr		{ $$ = create_operation( eAND, $1, $3, NULL );		}
	| expr TOKEN_RELEASE interval expr	{ $$ = create_operation( eRELEASE, $1, $4, $3 );	}
	| expr TOKEN_WEAK_RELEASE interval expr	{ $$ = create_operation( eWRELEASE, $1, $4, $3 );	}
	| expr TOKEN_UNTIL interval expr		{ $$ = create_operation( eUNTIL, $1, $4, $3 );		}
	| expr TOKEN_WEAK_UNTIL interval expr		{ $$ = create_operation( eWUNTIL, $1, $4, $3 );		}
	| TOKEN_FUTURE interval expr			{ $$ = create_operation( eFUTURE, NULL, $3, $2 );	}
	| TOKEN_GLOBALLY interval expr		{ $$ = create_operation( eGLOBALLY, NULL, $3, $2 );	}
	| TOKEN_NOT expr			{ $$ = create_operation( eNOT, NULL, $2, NULL );		}
	;
	
interval
        : TOKEN_LBRAKET TOKEN_INT TOKEN_COMMA TOKEN_INT TOKEN_RBRAKET       { $$ = create_interval ($2, $4);    }
        | TOKEN_LBRAKET TOKEN_INT TOKEN_RBRAKET     { $$ = create_interval ($2, INF);     }
        ;
	


%%
