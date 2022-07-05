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
 * @brief Action ID Code
 *
 */
typedef enum ActionCode
{
	NEW_PROCESS,
	REQ_SIZE,
	REQ_ENTRY,
} actioncode_t;

#define NO_ACTION_PARAMETER 0

/**
 * @brief Acciones que se pueden tomar sobre un Tripulante
 *
 */
typedef struct Accion
{
	// El código de operacion
	actioncode_t actioncode;
	// El parámetro de la operación
	uint32_t param;
} accion_t;

// ============================================================================================================
//                               ***** Funciones Públicas *****
// ============================================================================================================

// ------------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

void *accion_serializar(const accion_t *accion);

/**
 * @brief Instancia una nueva accion
 *
 * @param actioncode el codigo de accion
 * @param param el parametro de la accion
 * @return Una referencia a una accion
 */
accion_t *accion_create(actioncode_t actioncode, uint32_t param);

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
 * @brief Espera una accion.
 *
 * @param socket el file descriptor del cual recibir.
 * @return la accion recibida
 */
accion_t *accion_recibir(int socket);

accion_t *accion_from_stream(void *stream);
