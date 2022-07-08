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

typedef struct TLB
{
	uint32_t pagina;
	uint32_t frame;

	uint32_t bit_presencia;
	uint32_t tiempo_ult_acceso;

	// TODO --> validar mirando la consigna si se pueden tener estos valores en la estructura del TLB
	uint32_t tamanio;
	uint32_t ultima_pos_liberada;

} TLB_t;


/**
 * @brief Iniciar el TLB
 *
 */
void init_TLB();

/**
 * @brief Reseteo la TLB: Todos los Bits de presencia a 0
 *
 */
void reset_TLB();

/**
 * @brief Funcion de puntero para poder tener la TLB según el algoritmo
 *
 * @param param1
 * @param param2
 */
void (*TLB_builder)(uint32_t param1, uint32_t param2);

/**
 * @brief Actualizo la TLB llamando al TLBbuilder que se ajustará al algoritmo correspondiente
 *
 * @param pagina
 * @param marco
 */
void update_TLB(uint32_t pagina, uint32_t marco);

/**
 * @brief Se actualiza la TLB para el algoritmo FIFO
 *
 */
void update_TLB_FIFO( uint32_t pagina,uint32_t marco);


/**
 * @brief Se actualiza la TLB para el algoritmo LRU
 *
 * @param pagina
 * @param marco
 */
void update_TLB_LRU(uint32_t pagina, uint32_t marco);

/**
 * @brief Revisa si el numero de pagina se encuentra en la TLB
 *
 * @param numero_pagina
 * @param marco
 * @return true
 * @return false
 */
bool page_in_TLB(uint32_t numero_pagina, uint32_t* marco);
