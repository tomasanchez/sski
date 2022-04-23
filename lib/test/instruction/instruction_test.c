/**
 * @file instruction_test.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-19-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdlib.h>
#include "ctest.h"
#include "instruction.h"
#include "smartlist.h"

CTEST(instruction, when_instructionToStream_then_canBeRecovered)
{
	uint32_t param1 = 10, param2 = 124;

	instruction_t *instruction = instruction_create(C_REQUEST_NO_OP, param1, param2);

	void *stream = instruction_to_stream(instruction);
	instruction_destroy(instruction);

	instruction_t *recovered = instruction_from_stream(stream);

	ASSERT_EQUAL(C_REQUEST_NO_OP, recovered->icode);
	ASSERT_EQUAL(param1, recovered->param0);
	ASSERT_EQUAL(param2, recovered->param1);
	free(stream);

	instruction_destroy(recovered);
}

CTEST(instruction, when_instructionListToStream_then_canBeRecovered)
{

	t_list *instructions = list_create();

	uint32_t param1 = 10, param2 = 124;

	instruction_t *exit_i = instruction_create(C_REQUEST_EXIT, param1, param2);
	instruction_t *read_i = instruction_create(C_REQUEST_READ, param1, param2);
	instruction_t *write_i = instruction_create(C_REQUEST_WRITE, param1, param2);

	list_smart_add(instructions, exit_i);
	list_smart_add(instructions, read_i);
	list_smart_add(instructions, write_i);

	t_list *streams = list_map(instructions, instruction_to_stream);
	list_smart_destroy(instructions, instruction_destroy);
	t_list *recovered = list_map(streams, (void *)(void *)instruction_from_stream);
	list_smart_fast_destroy(streams);

	// Esto asume que EXIT, READ y WRITE son consecutivos
	for (int i = 0; i < list_size(recovered); i++)
	{
		instruction_t *i_recovered = list_get(recovered, i);
		ASSERT_EQUAL(C_REQUEST_EXIT + i, i_recovered->icode);
		ASSERT_EQUAL(param1, i_recovered->param0);
		ASSERT_EQUAL(param2, i_recovered->param1);
	}
	// Esto asume que EXIT, READ y WRITE son consecutivos

	list_smart_destroy(recovered, instruction_destroy);
}
