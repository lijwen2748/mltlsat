%{
/**
 * mltllexer.l file
 * To generate the lexical analyzer run: "flex mtllexer.l"
 */

#include "mltl_formula.h"
#include "mltlparser.h"

#include <stdio.h>
#include <stdlib.h>

%}

%option outfile="mltllexer.c" header-file="mltllexer.h"
%option warn nodefault
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

LPAREN		"("
RPAREN		")"
LBRAKET     \[
RBRAKET     \]
COMMA       \,

EQUIV		"<->"
IMPLIES		"->"
OR			\|\|?
AND			&&?
RELEASE     R
WEAK_RELEASE    V
UNTIL		U
WEAK_UNTIL	W
FUTURE		F|\<\>
GLOBALLY	G|\[\]
NEXT		X
WEAK_NEXT	N
NOT			!|~

INT         [1-9][0-9]*|[0-9]


TRUE		[tT]rue|TRUE
FALSE		[fF]alse|FALSE
ID			[a-zA-Z_][a-zA-Z0-9_]*
WS			[ \r\n\t]*

%%

{LPAREN}        { return TOKEN_LPAREN;		}
{RPAREN}        { return TOKEN_RPAREN;		}
{LBRAKET}       { return TOKEN_LBRAKET;     }
{RBRAKET}       { return TOKEN_RBRAKET;     }
{COMMA}         { return TOKEN_COMMA;       }
{EQUIV}			{ return TOKEN_EQUIV;		}
{IMPLIES}		{ return TOKEN_IMPLIES;		}
{OR}			{ return TOKEN_OR;			}
{AND}			{ return TOKEN_AND;			}
{RELEASE}		{ return TOKEN_RELEASE;		}
{WEAK_RELEASE}		{ return TOKEN_WEAK_RELEASE;		}
{UNTIL}			{ return TOKEN_UNTIL;		}
{WEAK_UNTIL}	{ return TOKEN_WEAK_UNTIL;	}
{FUTURE}		{ return TOKEN_FUTURE;		}
{GLOBALLY}		{ return TOKEN_GLOBALLY;	}
{NEXT}			{ return TOKEN_NEXT;		}
{WEAK_NEXT}		{ return TOKEN_WEAK_NEXT;	}
{NOT}			{ return TOKEN_NOT;			}

{WS}            { /* Skip blanks. */		}
{TRUE}			{ return TOKEN_TRUE;		}
{FALSE}			{ return TOKEN_FALSE;		}
{ID}			{ yylval->var_name = yytext; return TOKEN_VARIABLE; }
{INT}           { yylval->num = atoi (yytext); return TOKEN_INT; }


.				{ fprintf(stderr, "\033[31mERROR\033[0m: Unrecognized symbol: \033[34m%s\033[0m\n", yytext); exit(1); }
%%
/*
int yyerror(const char *msg) {
    fprintf(stderr,"Error:%s\n",msg);
	exit(1);
	return 0;
}*/
