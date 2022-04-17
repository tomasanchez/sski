/**
 * log.c
 *
 * @file Approach de un Loggger como Singleton
 * @author Tomás Sánchez
 * @since  09.03.2021
 */

// Log header
#include "log.h"
// strcat
#include <string.h>
// mutex
#include <pthread.h>
// getcwd
#include <limits.h>

// ============================================================================================================
//                               ***** Log -  Definiciones *****
// ============================================================================================================

// Único y estático Logger.
static t_log *this;
// Mutex del logger
static pthread_mutex_t mutex;
// El mínimoo nivel de loggeo.
#define MIN_LEVEL LOG_LEVEL_TRACE
// Si la consola va a estar activa
#define CONSOLA_ACITVA true

// ============================================================================================================
//                               ***** Funciones Públicas, Definiciones *****
// ============================================================================================================

// -----------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

int log_init(char *app_name, bool consola)
{

	// La ruta del directorio contenedor
	char carpeta_contenedora[MAX_CHARS] = "../log/";
	char cwd[MAX_CHARS] = "";

	char *log_path = NULL;

	if (getcwd(cwd, sizeof(carpeta_contenedora)) == NULL || strstr(cwd, app_name) != NULL || strstr(cwd, "build") != NULL)
	{
		log_path = strcat(carpeta_contenedora, app_name);
	}
	else
	{
		log_path = strcat(cwd, "/log/");
		log_path = strcat(log_path, app_name);
	}
	log_path = strcat(log_path, ".log");

	if (!log_path)
		return ERROR;

	if (!this)
		this = log_create(log_path, app_name, consola && CONSOLA_ACITVA, MIN_LEVEL);

	if (this)
	{
		pthread_mutex_init(&mutex, NULL);
		return SUCCESS;
	}
	else
	{
		perror(">> Logger Error");
		return ERROR;
	}
}

inline void log_close(void)
{
	if (this)
	{
		pthread_mutex_destroy(&mutex);
		log_destroy(this);
		this = NULL;
	}
}

// -----------------------------------------------------------
//  Getter
// ------------------------------------------------------------

t_log *logger(void)
{
	return this;
}

void logger_lock(void)
{
	pthread_mutex_lock(&mutex);
}
void logger_unlock(void)
{
	pthread_mutex_unlock(&mutex);
}
