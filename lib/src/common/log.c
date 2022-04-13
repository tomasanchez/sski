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

int log_init(char *path, char *nombre_app, bool consola)
{
    // La ruta del directorio contenedor
    char carpeta_contenedora[MAX_CHARS] = "../log/";

    if (!this)
        this = log_create(strcat(carpeta_contenedora, path), nombre_app, consola && CONSOLA_ACITVA, MIN_LEVEL);

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
