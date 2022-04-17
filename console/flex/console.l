%{

#include <stdio.h>
#include "parser.h"

#define ERROR_LEXICO(mensaje) yylexerrs++; char buffer[8229]; sprintf(buffer, "Error léxico: %s: %s", mensaje, yytext ); yyerror(buffer);

%}

%option outfile="./src/console/scanner.c"
%option header-file="./include/console/scanner.h"

%option 8bit
%option stack
%option noyywrap
%option nounput
%option noinput
%option yylineno
%option case-insensitive

space [[:space:]]

invalid_character [~`!@#$^&\[}[\]'"?\\>]

alpha [[:alpha:]]

digit [[:digit:]]

constant {digit}+

%%

"NO_OP" { return NO_OP;}
"I/O" { return IO;}
"READ" { return READ;}
"COPY" { return COPY;}
"WRITE" { return WRITE;}
"EXIT" { return EXIT;}

{space}+ {} /* ignore new lines, tabulations and spaces */

{constant} { yylval = strdup(yytext); return CONSTANT; }
{constant}({invalid_character}|{alpha})({invalid_character}|{alpha}|{digit})* { ERROR_LEXICO("constante inválida"); }

. { ERROR_LEXICO("caracter desconocido"); }

%%
