/**
 * @file pcb.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-18-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "pcb.h"
#include "instruction.h"

pcb_t *new_pcb(uint32_t id, size_t size, uint32_t estimation)
{
	pcb_t *pcb = malloc(sizeof(pcb_t));

	pcb->id = id;
	pcb->size = size;
	pcb->instructions = list_create();
	pcb->page_table = NULL;
	pcb->estimation = estimation;

	return pcb;
}

void pcb_destroy(pcb_t *pcb)
{
	if (pcb)
	{
		list_smart_destroy(pcb->instructions, instruction_destroy);

		if (pcb->page_table)
			free(pcb->page_table);

		free(pcb);
		pcb = NULL;
	}
}
