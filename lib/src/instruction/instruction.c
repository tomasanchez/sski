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

void instruction_destroy(instruction_t *instruction)
{
	if (instruction)
		free(instruction);
}

void *instruction_to_stream(instruction_t *this)
{
	void *stream = malloc(sizeof(instruction_t));

	size_t offset = 0lu;

	memcpy(stream + offset, &this->icode, sizeof(instcode_t));
	offset += sizeof(instcode_t);
	memcpy(stream + offset, &this->icode, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &this->icode, sizeof(uint32_t));
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
