/**
* accion.h
*
* @file API de Acciones
* @author Tomás Sánchez
* @since  05.04.2021
*/

#pragma once

#include "lib.h"
#include "conexion.h"
#include <inttypes.h>

// ============================================================================================================
//                               ***** Tipos (y estructuras) *****
// ============================================================================================================

/**
 * @brief Acciones que se pueden tomar sobre un Tripulante
 * 
 */
typedef struct Accion
{
	// El código de operacion
	opcode_t opcode;
	// El parámetro de la operación
	uint32_t param;
} accion_t;

// ============================================================================================================
//                               ***** Funciones Públicas *****
// ============================================================================================================

// ------------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

/**
 * @brief Instancia una nueva accion
 * 
 * @param opcode el codigo de accion
 * @param param el parametro de la accion
 * @return Una referencia a una accion
 */
accion_t *accion_create(opcode_t opcode, uint32_t param);

/**
 * @brief Destruye la intancia
 * 
 * @param accion la accion a destruirse.
 */
void accion_destroy(accion_t *accion);

// ------------------------------------------------------------
//  Envíos
// ------------------------------------------------------------

/**
 * @brief Envía una accion lista para recibirse.
 * 
 * @param accion la accion a enviar
 * @param socket el file descriptor al cual enviar
 * @return Los bytes enviados o ERROR. 
 */
ssize_t accion_enviar(accion_t *accion, int socket);

/**
 * @brief Recibe una accion
 * 
 * @param socket el file descriptor del cual recibir.
 * @return el parametro de la accion.
 */
uint32_t accion_recibir(int socket);

/**
 * @brief Espera una accion.
 * 
 * @param socket el file descriptor del cual recibir.
 * @return la accion recibida
 */
accion_t *accion_recibir_full(int socket);
