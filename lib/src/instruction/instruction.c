/**
 * @file instruction.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-15-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include <string.h>

#include "instruction.h"
#include "smartlist.h"

// Global Variable for list operations
size_t _instruction_offset = 0lu;

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

instruction_t *new_instruction(instcode_t icode)
{
	instruction_t *instance = NULL;
	instance = malloc(sizeof(instruction_t));
	instance->icode = icode;
	return instance;
}

instruction_t *instruction_new(instcode_t icode, uint32_t param0)
{
	instruction_t *instance = new_instruction(icode);
	instance->param0 = param0;
	return instance;
}

instruction_t *instruction_create(instcode_t icode, uint32_t param0, uint32_t param1)
{
	instruction_t *instance = instruction_new(icode, param0);
	instance->param1 = param1;
	return instance;
}

void instruction_destroy(void *instruction)
{
	if (instruction)
		free(instruction);
}

void *instruction_to_stream(void *instruction)
{
	instruction_t *this = (instruction_t *)instruction;
	void *stream = malloc(sizeof(instruction_t));

	size_t offset = 0lu;

	memcpy(stream + offset, &this->icode, sizeof(instcode_t));
	offset += sizeof(instcode_t);
	memcpy(stream + offset, &this->param0, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &this->param1, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}

instruction_t *instruction_from_stream(void *stream)
{
	instcode_t _icode = NO_OP;
	uint32_t _param0 = 0u, _param1 = 0u;

	size_t offset = 0lu;
	memcpy(&_icode, stream, sizeof(instcode_t));
	offset += sizeof(instcode_t);
	memcpy(&_param0, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&_param1, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return instruction_create(_icode, _param0, _param1);
}

void *instruction_reduce(void *buffer, void *next)
{
	size_t offset = sizeof(instruction_t);
	memcpy(&buffer + _instruction_offset, next, offset);
	_instruction_offset += offset;
	return buffer;
}

void *instruction_list_from(void *stream)
{
	t_list *list = list_create();
	uint32_t size = 0;
	size_t offset = 0;

	memcpy(&size, stream, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (uint32_t i = 0; i < size; i++)
	{
		list_smart_add(list, instruction_from_stream(stream + offset));
		offset += sizeof(instruction_t);
	}

	return list;
}
