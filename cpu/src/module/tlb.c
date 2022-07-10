/**
 * @file tlb.c
 * @author Francisco Maver <fmaver@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 2022-07-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "conexion_memoria.h"
#include "pcb_controller.h"
#include "request_handler.h"
#include "lib.h"
#include "cpu.h"
#include "tlb.h"
#include "log.h"
#include "cfg.h"
#include "conexion.h"
#include "accion.h"
#include "instruction.h"
#include "operands.h"
#include <signal.h>

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

extern cpu_t g_cpu;
//tlb_t *g_tlb;


tlb_t *tlb_create(uint32_t cant_entradas_TLB)
{
	tlb_t *tlb = malloc(sizeof(tlb_t) * cant_entradas_TLB);

	return tlb;
}

tlb_t *tlb_init()
{
	uint32_t cant_entradas_TLB = entradas_tlb();

	tlb_t *tlb = tlb_create(cant_entradas_TLB);

	char *algorithm = reemplazo_tlb();

	tlb->replace = strcmp(algorithm, "LRU") == 0 ? replace_lru : replace_fifo;

	tlb->size = cant_entradas_TLB;

	// limpio TLB para iniciarla vacia
	for (uint32_t i = 0; i < cant_entradas_TLB; i++)
	{
		tlb[i].tiempo_ult_acceso = 0;
		tlb[i].pagina = PAGINA_VACIA;
	}

	return tlb;
}

void tlb_reset(tlb_t **tlb)
{
	free(*tlb);

	*tlb = tlb_init();
}

void replace_fifo(void *tlb, uint32_t nueva_pagina, uint32_t nuevo_frame)
{
	tlb_t *self = tlb;

	for (uint32_t i = 0; i < self->size; i++)
	{
		if (self[i].bit_presencia == 0)
		{
			self[i].pagina = nueva_pagina;
			self[i].frame = nuevo_frame;
			self[i].bit_presencia = 1;
			return;
		}
	}
	self[self->ultima_pos_liberada].pagina = nueva_pagina;
	self[self->ultima_pos_liberada].frame = nuevo_frame;
	self->ultima_pos_liberada = (self->ultima_pos_liberada + 1) % self->size;
}

void replace_lru(void *tlb, uint32_t nueva_pagina, uint32_t nuevo_frame)
{
	tlb_t *self = tlb;

	for (uint32_t i = 0; i < self->size; i++)
	{
		// Aumento el tiempo de acceso de cada entrada. Si llega a existir el caso en que
		// no haya paginas vacias, aumentan todos y no va a generar ningun problema
		self[i].tiempo_ult_acceso++;

		if(self[i].pagina == PAGINA_VACIA)
		{
			self[i].pagina = nueva_pagina;
			self[i].frame = nuevo_frame;
			self[i].tiempo_ult_acceso = 0;
			return;
		}
	}

	uint32_t pag_index_max;
	uint32_t acceso_mas_antiguo = 0;

	for (uint32_t i = 0; i < self->size; i++)
	{
		if (self[i].tiempo_ult_acceso > acceso_mas_antiguo)
		{
			pag_index_max = i;
			acceso_mas_antiguo = self[i].tiempo_ult_acceso;
		}
	}
	self[pag_index_max].frame = nuevo_frame;
	self[pag_index_max].pagina = nueva_pagina;
	self[pag_index_max].tiempo_ult_acceso = 0;
}

bool page_in_TLB(tlb_t *self, uint32_t numero_pagina, uint32_t *marco)
{
	for (uint32_t i = 0; i < self->size; i++)
	{
		if (self[i].pagina == numero_pagina)
	 	{
			// Si la pagina esta en la TLB, devuelvo True y el Marco correspondiente
	 		*marco = self[i].frame;
	 		return true;
	 	}
	}

	return false;
}
