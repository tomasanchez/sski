%code top{
    #include <stdio.h>

    #include "scanner.h"
    #include "semantic.h"
}

%code provides{
    void yyerror(const char *);
    extern int yylexerrs;
}

%defines "./include/parser.h"
%output "./src/parser.c"

%define api.value.type {char *}
%define parse.error verbose

%token CONSTANT EXIT NO_OP READ IO WRITE COPY

%start instructions

%%

instructions
    : instruction
	| instructions instruction
	;

instruction
	: EXIT { request_exit(); }
	| NO_OP CONSTANT { request_no_op($2); }
	| IO CONSTANT { request_io($2); }
	| READ CONSTANT { request_read($2); }
	| WRITE CONSTANT CONSTANT { request_write($2, $3); }
	| COPY CONSTANT CONSTANT { request_copy($2, $3); }
	;

%%

/* Informa la ocurrencia de un error. */
void yyerror(const char *s){
	printf("línea #%d: %s\n", yylineno, s);
	return;
}
