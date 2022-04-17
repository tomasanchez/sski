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

#define CONF_FILE "console.cfg"

#define RUNNING true

/**
 * @brief
 *
 */
typedef struct ClientModule
{
	// La conexion del cliente
	conexion_t conexion;
	// El estado del módulo
	bool status;
	// Detector de señales
	int signal;

} client_m_t;

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
int on_client_run(char *instructions_file_name, int process_size);

/**
 * @brief Event handler de establecer conexion
 *
 * @param conexion referencia a una conexion
 * @param offline_mode si trabaja sin conexion
 * @return SUCCESS o ERROR
 */
int on_connect(void *conexion, bool offline_mode);

/**
 * @brief Event handler para leer a traves de Readline, al recibir EMPTY_STRING cambia el status
 *
 * @param line la linea leida
 * @param status el estado del proceso
 * @return la linea leida o NULL
 */
char *on_client_read(char *line, bool *status);

/**
 * @brief Event handler para el envio de mensajes
 *
 * @param conexion
 * @param line
 * @return int
 */
int on_client_send(void *conexion, char *line);
