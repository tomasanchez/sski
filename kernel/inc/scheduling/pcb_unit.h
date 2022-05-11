/**
 * @file pcb_unit.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "pcb.h"
#include "pcb_state.h"

/**
 * @brief A PCB Scheduler's Unit.
 *
 */
typedef struct PcbUnit
{
	// The PCB Itself
	pcb_t *_pcb;
	// The Scheduler data
	pcb_state_t state;
} pcb_unit_t;

/**
 * @brief Creates a new PCB Unit
 *
 * @param pcb reference which contains the data
 * @return a unit to be scheduled
 */
pcb_unit_t *
new_pcb_unit(pcb_t *pcb);

/**
 * @brief Deallocates Memory used by a PCB_UNIT
 *
 * @param pcb_unit to be deleted
 */
void pcb_unit_destroy(void *pcb_unit);
