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
#include "conexion.h"
#include "smartlist.h"

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
	instcode_t _icode = NO_INSTRUCTION;
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

ssize_t instruction_send(conexion_t is_conexion, instruction_t *is_instruction)
{
	// Local stream - el paquete serializado, requiere free(1)
	void *stream = instruction_to_stream(is_instruction);

	ssize_t bytes_sent = conexion_enviar_stream(is_conexion, CMD, stream, sizeof(instruction_t));

	free(stream);

	return bytes_sent;
}
