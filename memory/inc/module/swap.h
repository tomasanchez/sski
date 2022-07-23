/**
 * @file swap.h
 * @author Francisco Maver (fmaver@frba.utn.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "swap.h"
#include <inttypes.h>

typedef struct SwapData
{
	uint32_t pid;
	uint32_t offset;
	uint32_t size;
} swap_data_t;

swap_data_t *new_swap_data(uint32_t pid, uint32_t size);

void unswap_frame_for_pcb(uint32_t pid, uint32_t frame);

void unswap_pcb(uint32_t pid);

void swap_frame_for_pcb(uint32_t pid, uint32_t frame);

void swap_pcb(void *pcb_ref);
