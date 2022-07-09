/**
 * @file tlb.h
 * @author Francisco Maver <fmaver@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 2022-07-06
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "pcb.h"
#include "thread_manager.h"
#include "conexion.h"
#include "server.h"
#include "instruction.h"
#include "sync.h"

typedef struct g_tlb
{
	uint32_t pagina;
	uint32_t frame;

	uint32_t bit_presencia;
	uint32_t tiempo_ult_acceso;

	// TODO --> validar mirando la consigna si se pueden tener estos valores en la estructura del TLB
	uint32_t size;
	uint32_t ultima_pos_liberada;

	void (*replace)(void *self, uint32_t param1, uint32_t param2);

} tlb_t;

/**
 * @brief Iniciar el TLB
 *
 */
void tlb_init();

/**
 * @brief Revisa si el numero de pagina se encuentra en la TLB
 *
 * @param numero_pagina
 * @param marco
 * @return true
 * @return false
 */
bool page_in_TLB(uint32_t numero_pagina, uint32_t *marco);

void replace_fifo(tlb_t *self, uint32_t nueva_pagina, uint32_t nuevo_frame);
void replace_lru(tlb_t *self, uint32_t nueva_pagina, uint32_t nuevo_frame);

void tlb_reset(tlb_t *tlb);
