/* Auto-Generated Header file name */
%option header-file = "include/console/lexer.h"
/* Auto-generated Source file name */
%option outfile = "src/console/lexer.c"
/* 8-bit Scanner (THREAD-SAFE)*/
%option 8bit noyywrap
/*For "include"*/
%option stack
/* Case insensitve option*/
%option case-insensitive
/* Count line number*/
%option yylineno

/* C/C++ code included inside %top{} are copied to both the `_lexer.c` and  the `_lexer.h` */
%top{
}

/* C/C++ code included inside %{%} is copied to `_lexer.c` but not to the header-file*/
%{
/**
* console_lexer.c
*
* @file  Auto-generated lexer by Flex.
* @author Tomás Sánchez
* @since  04.14.2022
*/

#include <stdlib.h>

// Avoiding warning implicit-function-declaration as fileno is no std in C
extern int fileno(FILE *);
%}


%%
[ \t]
("NO_OP")					{ puts("No operation");}
("I/O")						{ puts("Input/Output");}
("READ")					{ puts("Read");}
("COPY")					{ puts("Copy");}
("WRITE")					{ puts("Write");}
("Exit")					{ puts("Exit");}
[\n]						{ puts("End of line");}
[0-9]+						{ puts("Number");}
.							{ puts("Invalid symbol");}
%%
