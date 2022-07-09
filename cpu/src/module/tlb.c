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
tlb_t g_tlb;

void tlb_init()
{
	char *algorithm = reemplazo_tlb();

	g_tlb.replace = strcmp(algorithm, "LRU") == 0 ? replace_lru : replace_fifo;

	uint32_t size = entradas_tlb();
	g_tlb.size = size;

	// limpio TLB para iniciarla vacia
	for (uint32_t i = 0; i < size; i++)
	{
		g_tlb.bit_presencia = 0;
		g_tlb.tiempo_ult_acceso = 0;
	}
}

void tlb_reset(tlb_t *tlb)
{
}

void replace_fifo(tlb_t *self, uint32_t nueva_pagina, uint32_t nuevo_frame)
{

	// for (uint32_t i = 0; i < g_tlb->tamanio; i++)
	// {
	// 	if (g_tlb[i].bit_presencia == 0)
	// 	{
	// 		g_tlb[i].pagina = nueva_pagina;
	// 		g_tlb[i].frame = nuevo_frame;
	// 		g_tlb[i].bit_presencia = 1;
	// 		return;
	// 	}
	// }

	// g_tlb[g_tlb->ultima_pos_liberada].pagina = nueva_pagina;
	// g_tlb[g_tlb->ultima_pos_liberada].frame = nuevo_frame;
	// g_tlb->ultima_pos_liberada = (g_tlb->ultima_pos_liberada + 1) % g_tlb->tamanio;
}

void replace_lru(tlb_t *self, uint32_t nueva_pagina, uint32_t nuevo_frame)
{

	// for (uint32_t i = 0; i < g_tlb->tamanio; i++)
	// {
	// 	g_tlb[i].tiempo_ult_acceso++;
	// }

	// for (uint32_t i = 0; i < g_tlb->tamanio; i++)
	// {
	// 	if (g_tlb[i].bit_presencia == 0)
	// 	{
	// 		g_tlb[i].pagina = nueva_pagina;
	// 		g_tlb[i].frame = nuevo_frame;
	// 		g_tlb[i].bit_presencia = 1;
	// 		g_tlb[i].tiempo_ult_acceso = 0;
	// 		return;
	// 	}
	// }

	// uint32_t max_index;
	// uint32_t mejor_tiempo = 0;

	// for (uint32_t i = 0; i < g_tlb->tamanio; i++)
	// {
	// 	if (g_tlb[i].tiempo_ult_acceso > mejor_tiempo)
	// 	{
	// 		max_index = 1;
	// 		mejor_tiempo = g_tlb[i].tiempo_ult_acceso;
	// 	}
	// }

	// g_tlb[max_index].frame = nuevo_frame;
	// g_tlb[max_index].pagina = nueva_pagina;
	// g_tlb[max_index].bit_presencia = 1;
	// g_tlb[max_index].tiempo_ult_acceso = 0;
}

bool page_in_TLB(uint32_t numero_pagina, uint32_t *marco)
{
	// for (uint32_t i = 0; i < g_tlb->tamanio; i++)
	// {
	// 	if (g_tlb[i].bit_presencia == true && g_tlb[i].pagina == numero_pagina)
	// 	{
	// 		*marco = g_tlb[i].frame;
	// 		return true;
	// 	}
	// }

	return false;
}
