/*
 * semantic.c
 *
 * Copyright 2020 Roberto Nicolás Savinelli <rnsavinelli@est.frba.utn.edu.ar>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <smartlist.h>

#include "semantic.h"

extern t_list *input_file_commands;

static void
_request_instruction(const char *instruccion, int cantidad_de_argumentos, ...)
{
	va_list ap;

	size_t instruction_size = sizeof(char) * (strlen(instruccion) + 1);
	char *request = (char *)malloc(instruction_size);
	strcpy(request, instruccion);

	va_start(ap, cantidad_de_argumentos);
	for (int i = 0; i < cantidad_de_argumentos; i++)
	{
		char *param = va_arg(ap, char *);
		size_t param_size = sizeof(char) * (strlen(param) + 2);
		request = (char *)realloc(request, instruction_size + param_size);
		strcat(request, " ");
		strcat(request, param);
		instruction_size += param_size;
	}

	list_smart_add(input_file_commands, request);

	va_end(ap);
}

void request_exit(void)
{
	_request_instruction("EXIT", 0);
}

void request_no_op(char *constant)
{
	_request_instruction("NO_OP", 1, constant);

	free(constant);
}

void request_io(char *constant)
{
	_request_instruction("I/O", 1, constant);

	free(constant);
}

void request_read(char *constant)
{
	_request_instruction("READ", 1, constant);

	free(constant);
}

void request_write(char *constant_l, char *constant_r)
{
	_request_instruction("WRITE", 2, constant_l, constant_r);

	free(constant_l);

	free(constant_r);
}

void request_copy(char *constant_l, char *constant_r)
{
	_request_instruction("COPY", 2, constant_l, constant_r);

	free(constant_l);

	free(constant_r);
}
