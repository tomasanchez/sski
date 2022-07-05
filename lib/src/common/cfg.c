/**
 * configuraciones.c
 *
 * @file Facilitaciones del TAD config
 * @author Tomás Sánchez
 * @since  04.23.2021
 */

#include "cfg.h"
#include <commons/config.h>
// getcwd
#include <limits.h>

// ============================================================================================================
//                               ***** Configuracion -  Definiciones *****
// ============================================================================================================

// La configuración en sí misma, ÉSTA CONFIG
static t_config *this;

// La carpeta contenedora de las configs
#define CONFIG_FOLDER_PATH "../config/"

// ============================================================================================================
//                               ***** Funciones Privadas - Declaraciones *****
// ============================================================================================================

/**
 * Obtiene el key-value de la config.
 *
 * @param key el valor de la key a leer
 * @return el valor string obtenido.
 */
char *config_string(char *key);

/**
 * Obtiene el key-value de la config.
 *
 * @param key el valor de la key a leer
 * @return el valor int obtenido
 */
int config_int(char *key);

/**
 * Obtiene el key-value de la config.
 *
 * @param key el valor de la key a leer
 * @return el valor en array, obtenido.
 */
char **config_array(char *key);

/**
 * Obtiene el key-value de la config.
 *
 * @param key el valor de la key a leer
 * @return el valor double obtenido.
 */
double config_double(char *key);

/**
 * Obtiene el key-value de la config.
 *
 * @param key el valor de la key a leer
 * @return el valor loong obtenido.
 */
long config_long(char *key);

// ============================================================================================================
//                               ***** Funciones Privadas - Definiciones *****
// ============================================================================================================

// -----------------------------------------------------------
//  Getters
// ------------------------------------------------------------

inline char *config_string(char *key)
{
	return config_get_string_value(this, key);
}

inline int config_int(char *key)
{
	return config_get_int_value(this, key);
}

inline char **config_array(char *key)
{
	return config_get_array_value(this, key);
}

inline double config_double(char *key)
{
	return config_get_double_value(this, key);
}

inline long config_long(char *key)
{
	return config_get_long_value(this, key);
}

// ============================================================================================================
//                               ***** Funciones Públicas, Definiciones *****
// ============================================================================================================

// -----------------------------------------------------------
//  Constructor y Destructor
// -----------------------------------------------------------

int config_init(char *app_name)
{
	// La ruta del directorio contenedor
	char carpeta_contenedora[MAX_CHARS] = CONFIG_FOLDER_PATH;
	char cwd[MAX_CHARS] = "";

	char *cfg_path = NULL;

	if (getcwd(cwd, sizeof(carpeta_contenedora)) == NULL || strstr(cwd, app_name) != NULL || strstr(cwd, "build") != NULL)
	{
		cfg_path = strcat(carpeta_contenedora, app_name);
	}
	else
	{
		cfg_path = strcat(cwd, "/config/");
		cfg_path = strcat(cfg_path, app_name);
	}
	cfg_path = strcat(cfg_path, ".cfg");

	if (!this)
		this = config_create(cfg_path);

	if (this EQ NULL)
	{
		perror("Error:");
		return ERROR;
	}

	return SUCCESS;
}

int config_initialized()
{
	if (this EQ NULL)
	{
		perror("Error:");
		return ERROR;
	}

	return SUCCESS;
}

void config_close(void)
{
	if (this)
		config_destroy(this);

	this = NULL;
}

void *config_instance(void)
{
	return this;
}
// -----------------------------------------------------------
//  Puertos
// ------------------------------------------------------------

// Config-Key Puerto
#define PUERTO_KEY "PUERTO"

inline char *puerto(void)
{
	return config_string(PUERTO_KEY);
}

// -----------------------------------------------------------
//  IPs
// ------------------------------------------------------------

// Config-Key IP

#define IP_KEY "IP"

inline char *ip(void)
{
	return config_string(IP_KEY);
}

// -----------------------------------------------------------
//  Console
// ------------------------------------------------------------

#define PUERTO_KERNEL_KEY "PUERTO_KERNEL"

inline char *puerto_kernel(void)
{
	return config_string(PUERTO_KERNEL_KEY);
}

#define IP_KERNEL_KEY "IP_KERNEL"

inline char *ip_kernel(void)
{
	return config_string(IP_KERNEL_KEY);
}

// -----------------------------------------------------------
//  Kernel
// ------------------------------------------------------------

#define IP_MEMORIA "IP_MEMORIA"

inline char *ip_memoria(void)
{
	return config_string(IP_MEMORIA);
}

#define PUERTO_MEMORIA "PUERTO_MEMORIA"

inline char *puerto_memoria(void)
{
	return config_string(PUERTO_MEMORIA);
}

#define IP_CPU "IP_CPU"

inline char *ip_cpu(void)
{
	return config_string(IP_CPU);
}

#define PUERTO_CPU_DISPATCH "PUERTO_CPU_DISPATCH"

inline char *puerto_cpu_dispatch(void)
{
	return config_string(PUERTO_CPU_DISPATCH);
}

#define PUERTO_CPU_INTERRUPT "PUERTO_CPU_INTERRUPT"

inline char *puerto_cpu_interrupt(void)
{
	return config_string(PUERTO_CPU_INTERRUPT);
}

#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"

inline char *puerto_escucha(void)
{
	return config_string(PUERTO_ESCUCHA);
}

#define ALGORITMO_PLANIFICACION "ALGORITMO_PLANIFICACION"

inline char *algoritmo_planificacion(void)
{
	return config_string(ALGORITMO_PLANIFICACION);
}

#define ESTIMACION_INICIAL "ESTIMACION_INICIAL"

inline int estimacion_inicial(void)
{
	return config_int(ESTIMACION_INICIAL);
}

#define ALFA "ALFA"

inline double alfa(void)
{
	return config_double(ALFA);
}

#define GRADO_MULTIPROGRAMACION "GRADO_MULTIPROGRAMACION"

inline int grado_multiprogramacion(void)
{
	return config_int(GRADO_MULTIPROGRAMACION);
}

#define TIEMPO_MAXIMO_BLOQUEADO "TIEMPO_MAXIMO_BLOQUEADO"

inline int tiempo_maximo_bloqueado(void)
{
	return config_int(TIEMPO_MAXIMO_BLOQUEADO);
}

// -----------------------------------------------------------
//  Kernel
// -----------------------------------------------------------

#define PUERTO_ESCUCHA_DISPATCH "PUERTO_ESCUCHA_DISPATCH"

char *
puerto_escucha_dispatch(void)
{
	return config_string(PUERTO_ESCUCHA_DISPATCH);
}

#define PUERTO_ESCUCHA_INTERRUPT "PUERTO_ESCUCHA_INTERRUPT"

char *
puerto_escucha_interrupt(void)
{
	return config_string(PUERTO_ESCUCHA_INTERRUPT);
}

#define RETARDO_NOOP "RETARDO_NOOP"

int retardo_noop(void)
{
	return config_int(RETARDO_NOOP);
}

#define REEMPLAZO_TLB "REEMPLAZO_TLB"

char *
reemplazo_tlb(void)
{
	return config_string(REEMPLAZO_TLB);
}

#define ENTRADAS_TLB "ENTRADAS_TLB"

int entradas_tlb(void)
{
	return config_int(ENTRADAS_TLB);
}

// -----------------------------------------------------------
//  Memoria
// -----------------------------------------------------------

#define TAM_MEMORIA "TAM_MEMORIA"

int tam_memoria(void)
{
	return config_int(TAM_MEMORIA);
}

#define TAM_PAGINA "TAM_PAGINA"

int tam_pagina(void)
{
	return config_int(TAM_PAGINA);
}

#define ENTRADAS_POR_TABLA "ENTRADAS_POR_TABLA"

int entradas_por_tabla(void)
{
	return config_int(ENTRADAS_POR_TABLA);
}

#define RETARDO_MEMORIA "RETARDO_MEMORIA"

int retardo_memoria(void)
{
	return config_int(RETARDO_MEMORIA);
}

#define ALGORITMO_REEMPLAZO "ALGORITMO_REEMPLAZO"

inline char *algoritmo_reemplazo(void)
{
	return config_string(ALGORITMO_REEMPLAZO);
}

#define MARCOS_POR_PROCESO "MARCOS_POR_PROCESO"

int marcos_por_proceso(void)
{
	return config_int(MARCOS_POR_PROCESO);
}

#define RETARDO_SWAP "RETARDO_SWAP"

int retardo_swap(void)
{
	return config_int(RETARDO_SWAP);
}

#define PATH_SWAP "PATH_SWAP"

char *path_swap(void)
{
	return config_string(PATH_SWAP);
}
