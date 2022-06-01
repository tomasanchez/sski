/**
 * @file pcb_unit.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include "pcb_unit.h"
#include "pcb_state.h"

pcb_unit_t *
new_pcb_unit(pcb_t *pcb)
{
	pcb_unit_t *process = malloc(sizeof(pcb_unit_t));
	process->_pcb = pcb;
	process->state = NEW;
}

void pcb_unit_destroy(void *pcb_unit)
{
	if (pcb_unit)
	{
		pcb_unit_t *process = pcb_unit;
		pcb_destroy(process->_pcb);
		free(process);
	}
}
