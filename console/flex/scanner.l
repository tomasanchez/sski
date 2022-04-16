/*
 * scanner.l
 *
 * Copyright 2021 Roberto Nicolás Savinelli <rnsavinelli@frba.utn.edu.ar>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

%{

#include <stdio.h>
#include "parser.h"

#define ERROR_LEXICO(mensaje) yylexerrs++; char buffer[8229]; sprintf(buffer, "Error léxico: %s: %s", mensaje, yytext ); yyerror(buffer);

%}

%option outfile="./src/scanner.c"
%option header-file="./include/scanner.h"

%option noyywrap
%option nounput
%option noinput
%option yylineno

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
