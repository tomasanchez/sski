/**
 * @file console.h
 * @author Franco Parente (fparente14@frba.utn.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-04-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "lib.h"
#include "conexion.h"
#include "instruction.h"
#include "accion.h"

#define RUNNING true

/**
 * @brief
 *
 */
typedef struct ConsoleModule
{
	// La conexion del cliente
	conexion_t conexion;
	// El estado del módulo
	bool status;
	// Detector de señales
	int signal;

} console_t;

// ============================================================================================================
//                                   ***** Funciones Públicas  *****
// ============================================================================================================

// ------------------------------------------------------------
//  Lifecycle methods
// ------------------------------------------------------------

/**
 * @brief Se llama al Inicializar un modulo
 *
 * @return SUCCESS o bien ERROR
 */
int on_init(void);

/**
 * @brief Realiza el cierre liberando los recursos.
 *
 * @return EXIT_SUCCESS o la SIGNAL correspondiente
 */
int on_before_exit(void);

/**
 * @brief Event hanlder, cuando el servidor se encuentra atendiendo clientes
 *
 * @return ERROR o SUCCESS
 */
int on_run(char *instructions_file_name, uint32_t process_size);

/**
 * @brief Event handler para el envio de mensajes
 *
 * @param conexion
 * @param line
 * @return int
 */
ssize_t on_send_instruction(void *conexion, instruction_t *inst);
