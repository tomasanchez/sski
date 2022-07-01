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
#include "instruction.h"

extern t_list *input_file_commands;

static void
_request_instruction(instcode_t instcode, int param0, int param1)
{
	instruction_t *instruction = instruction_create(instcode, param0, param1);

	list_smart_add(input_file_commands, instruction);
}

void request_exit(void)
{
	_request_instruction(C_REQUEST_EXIT, NO_INSTRUCTION_PARAMETER, NO_INSTRUCTION_PARAMETER);
}

void request_no_op(char *constant)
{
	int intConstant = atoi(constant);

	for (int i = 0; i < intConstant; i++)
		_request_instruction(C_REQUEST_NO_OP, NO_INSTRUCTION_PARAMETER, NO_INSTRUCTION_PARAMETER);

	free(constant);
}

void request_io(char *constant)
{
	_request_instruction(C_REQUEST_IO, atoi(constant), NO_INSTRUCTION_PARAMETER);

	free(constant);
}

void request_read(char *constant)
{
	_request_instruction(C_REQUEST_READ, atoi(constant), NO_INSTRUCTION_PARAMETER);

	free(constant);
}

void request_write(char *constant_l, char *constant_r)
{
	_request_instruction(C_REQUEST_WRITE, atoi(constant_l), atoi(constant_r));

	free(constant_l);

	free(constant_r);
}

void request_copy(char *constant_l, char *constant_r)
{
	_request_instruction(C_REQUEST_COPY, atoi(constant_l), atoi(constant_r));

	free(constant_l);

	free(constant_r);
}
