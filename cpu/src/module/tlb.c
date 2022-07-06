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

cpu_t g_cpu;
TLB_t* TLB;


void init_TLB()
{
	char* algorithm = reemplazo_tlb();

	if (strcmp(algorithm, "LRU") == 0)
	{
		TLB_builder = &update_TLB_LRU;
	}

	if (strcmp(algorithm, "FIFO") == 0)
	{
		TLB_builder = &update_TLB_FIFO;
		TLB->ultima_pos_liberada = 0;
	}

	uint32_t tamanio_tlb = entradas_tlb();
	TLB = malloc(sizeof(TLB_t) * tamanio_tlb);
	TLB->tamanio = tamanio_tlb;

	// limpio TLB para iniciarla vacia
	for (uint32_t i=0; i < tamanio_tlb; i++)
	{
		TLB[i].bit_presencia = 0;
		TLB[i].tiempo_ult_acceso = 0;
	}

	free(algorithm);

}


void reset_TLB()
{
	for(uint32_t i = 0; i < TLB->tamanio; i++)
		TLB[i].bit_presencia = 0;
}


void update_TLB(uint32_t pagina, uint32_t marco)
{
	(*TLB_builder)(pagina,marco);
}


void update_TLB_FIFO( uint32_t nueva_pagina,uint32_t nuevo_frame)
{

 for (uint32_t i = 0; i < TLB->tamanio; i++){
	 if (TLB[i].bit_presencia == 0){
		 TLB[i].pagina = nueva_pagina;
		 TLB[i].frame = nuevo_frame;
		 TLB[i].bit_presencia = 1;
		 return;
	 }
 }

 TLB[TLB->ultima_pos_liberada].pagina = nueva_pagina;
 TLB[TLB->ultima_pos_liberada].frame = nuevo_frame;
 TLB->ultima_pos_liberada = (TLB->ultima_pos_liberada + 1) % TLB->tamanio;
}

void update_TLB_LRU(uint32_t nueva_pagina, uint32_t nuevo_frame){

	for(uint32_t i = 0; i < TLB->tamanio; i++){
		TLB[i].tiempo_ult_acceso++;
	}

	for (uint32_t i = 0; i < TLB->tamanio; i++){
		 if (TLB[i].bit_presencia == 0){
			 TLB[i].pagina = nueva_pagina;
			 TLB[i].frame = nuevo_frame;
			 TLB[i].bit_presencia = 1;
			 TLB[i].tiempo_ult_acceso = 0;
			 return;
		 }
	 }

	uint32_t max_index;
	uint32_t mejor_tiempo = 0;

	for (uint32_t i = 0; i < TLB->tamanio; i++){
		if (TLB[i].tiempo_ult_acceso > mejor_tiempo){
			max_index = 1;
			mejor_tiempo = TLB[i].tiempo_ult_acceso;
		}
	}

	TLB[max_index].frame = nuevo_frame;
	TLB[max_index].pagina = nueva_pagina;
	TLB[max_index].bit_presencia = 1;
	TLB[max_index].tiempo_ult_acceso = 0;
}

bool hit_TLB(uint32_t numero_pagina, uint32_t* marco)
{
	for (uint32_t i = 0; i < TLB->tamanio; i++){
		if (TLB[i].bit_presencia == true && TLB[i].pagina == numero_pagina){
			*marco = TLB[i].frame;
			return true;
		}
	}
	return false;
}

