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
 * Singleton of Config: Reads from config/module_name.cfg.
 *
 * @param module_name el del modulo.
 * @return SUCCESS o ERROR
 */
int config_init(char *module_name);

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

// -----------------------------------------------------------
//  Console
// ------------------------------------------------------------

char *puerto_kernel(void);

char *ip_kernel(void);

// -----------------------------------------------------------
//  Kernel
// ------------------------------------------------------------

char *ip_memoria(void);

char *puerto_memoria(void);

char *ip_cpu(void);

char *puerto_cpu_dispatch(void);

char *puerto_cpu_interrupt(void);

char *puerto_escucha(void);

char *algoritmo_planificacion(void);

int estimacion_inicial(void);

double alfa(void);

int grado_multiprogramacion(void);

int tiempo_maximo_bloqueado(void);

// -----------------------------------------------------------
//  CPU
// -----------------------------------------------------------

char *
puerto_escucha_dispatch(void);

char *
puerto_escucha_interrupt(void);

int retardo_noop(void);

char *
reemplazo_tlb(void);

int entradas_tlb(void);

// -----------------------------------------------------------
//  Memoria
// -----------------------------------------------------------

int tam_memoria(void);

int tam_pagina(void);

int entradas_por_tabla(void);

int retardo_memoria(void);

char *algoritmo_reemplazo(void);

int marcos_por_proceso(void);

int retardo_swap(void);

char *path_swap(void);
