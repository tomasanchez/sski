/**
 * @file pcb_test.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-19-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include <time.h>
#include "ctest.h"
#include "pcb.h"
#include "instruction.h"

static void fill_list(t_list *list);

CTEST(pcb, when_pcbToStream_then_canBeRecovered)
{
	uint32_t id = rand(), size = rand(), estimation = rand();
	pcb_t *pcb = new_pcb(id, size, estimation);
	fill_list(pcb->instructions);

	void *stream = pcb_to_stream(pcb);

	pcb_t *recovered = pcb_from_stream(stream);

	ASSERT_EQUAL(pcb->id, recovered->id);
	ASSERT_EQUAL(pcb->size, recovered->size);
	ASSERT_EQUAL(pcb->estimation, recovered->estimation);
	ASSERT_EQUAL(list_size(pcb->instructions), list_size(recovered->instructions));

	for (int i = 0; i < list_size(pcb->instructions); i++)
	{
		instruction_t *original = list_get(pcb->instructions, i);
		instruction_t *i_recovered = list_get(recovered->instructions, i);
		ASSERT_EQUAL(original->icode, i_recovered->icode);
		ASSERT_EQUAL(original->param0, i_recovered->param0);
		ASSERT_EQUAL(original->param1, i_recovered->param1);
	}

	free(stream);
	pcb_destroy(pcb);
	pcb_destroy(recovered);
}

CTEST(pcb, when_pcb_has_less_estimation_goes_first)
{
	uint32_t id = rand(), size = rand(), estimation = rand();

	pcb_t *pcb = new_pcb(id, size, estimation);
	pcb_t *pcb2 = new_pcb(id, size, estimation + 1);

	ASSERT_TRUE(pcb_sort_by_estimation(pcb, pcb2));

	pcb_destroy(pcb);
	pcb_destroy(pcb2);
}

CTEST(pcb, when_pcb_has_less_estimation_goes_second)
{
	uint32_t id = rand(), size = rand(), estimation = rand();

	pcb_t *pcb = new_pcb(id, size, estimation + 1);
	pcb_t *pcb2 = new_pcb(id, size, estimation);

	ASSERT_FALSE(pcb_sort_by_estimation(pcb, pcb2));

	pcb_destroy(pcb);
	pcb_destroy(pcb2);
}

inline void fill_list(t_list *list)
{
	uint32_t g_param0 = rand(), g_param2 = rand();

	instruction_t *exit_i = instruction_create(C_REQUEST_EXIT, g_param0, g_param2);
	instruction_t *read_i = instruction_create(C_REQUEST_READ, g_param0, g_param2);
	instruction_t *write_i = instruction_create(C_REQUEST_WRITE, g_param0, g_param2);

	list_smart_add(list, exit_i);
	list_smart_add(list, read_i);
	list_smart_add(list, write_i);
}
