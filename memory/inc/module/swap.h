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

#include "cfg.h"
#include "operands.h"
#include "safe_list.h"
#include "log.h"

typedef struct RegistroPrimerNivel{
	uint32_t nro_registro;
	uint32_t pagina_madre;
	uint32_t valor;
	bool contiene_valor;
} registro_t;

typedef struct SwapItem{
	registro_t* registro;
	uint32_t indice;
}item_t;

typedef struct Swap{
	FILE* archivo_swap;
	char* swap_name;
	t_list* items;
}swap_t;

void swapInit();

/**
 * @brief no se que hace
 *
 * @param item
 */
bool esRegistro(void* item);

/**
 * @brief Indica si el registro pasado esta en el swap
 *
 * @param swap
 * @param registro
 *
 */
bool swap_estaEnSwap(swap_t* swap,registro_t* registro);

/**
 * @brief Escribe el registro en el swap
 *
 * @param swap
 * @param registro
 *
 */
void swap_escribirEnSwap(swap_t* swap,registro_t* registro);

/**
 * @brief Escribe en memoria los datos leidos del registro pasado
 *
 * @param swap
 * @param registro
 *
 */
void swap_escribirEnMemoria(swap_t* swap ,registro_t* registro);

/**
 * @brief Devuelve un swap a partir del pid pasado
 *
 * @param pid
 * @return swap
 *
 */
swap_t
*swap_crearSwap(uint32_t pid);

/**
 * @brief Destruye el swap pasado por parametro
 *
 * @param swap
 *
 */
void
swap_destruirSwap(swap_t* swap);
