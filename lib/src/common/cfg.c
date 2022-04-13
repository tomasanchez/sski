/**
 * configuraciones.c
 *
 * @file Facilitaciones del TAD config
 * @author Tomás Sánchez
 * @since  04.23.2021
 */

#include "cfg.h"
#include <commons/config.h>

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

int config_init(char *path)
{
	// La ruta del directorio contenedor
	char carpeta_contenedora[MAX_CHARS] = CONFIG_FOLDER_PATH;

	if (!this)
		this = config_create(strcat(carpeta_contenedora, path));

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
//  Kernel
// ------------------------------------------------------------

#define IP_MEMORIA "IP_MEMORIA"

inline char *ip_memoria(void)
{
	return config_string(IP_MEMORIA);
}

#define PUERTO_MEMORIA "PUERTO_MEMORIA"

inline int puerto_memoria(void)
{
	return config_int(PUERTO_MEMORIA);
}

#define IP_CPU "IP_CPU"

inline char *ip_cpu(void)
{
	return config_string(IP_CPU);
}

#define PUERTO_CPU_DISPATCH "PUERTO_CPU_DISPATCH"

inline int puerto_cpu_dispatch(void)
{
	return config_int(PUERTO_CPU_DISPATCH);
}

#define PUERTO_CPU_INTERRUPT "PUERTO_CPU_INTERRUPT"

inline int puerto_cpu_interrupt(void)
{
	return config_int(PUERTO_CPU_INTERRUPT);
}

#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"

inline int puerto_escucha(void)
{
	return config_int(PUERTO_ESCUCHA);
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
