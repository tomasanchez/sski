/**
 * cfg.h
 *
 * @file API de configuraciones
 * @author Tomás Sánchez
 * @since  09.11.2021
 */

#pragma once

#include <string.h>
#include "lib.h"

// ============================================================================================================
//                                   ***** Funciones Públicas  *****
// ============================================================================================================

// -----------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

/**
 * Levanta el archivo configuraciones de la carpeta config/.
 *
 * @param path el Nombre del archivo de configuraciones
 * @return SUCCESS o ERROR
 */
int config_init(char *path);

/**
 * Pregunta si la configuracion fue inicializada.
 *
 * @return SUCCESS o ERROR
 */
int config_initialized(void);

/**
 * Destruye la configuración cargada.
 */
void config_close(void);

/**
 * @brief Obtiene la instancia del singleton
 *
 * @return la referencia a la configuracion
 */
void *config_instance(void);

// -----------------------------------------------------------
//  Puertos
// ------------------------------------------------------------

/**
 * Lee el valor de la key puerto de la config.
 *
 * @return el puerto propiamente dicho
 */
char *puerto(void);

// -----------------------------------------------------------
//  IPs
// ------------------------------------------------------------

/**
 * Lee el valor de la key IP de la config.
 *
 * @return el IP correspondiente
 */
char *ip(void);