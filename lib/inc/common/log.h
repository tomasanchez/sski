/**
 * log.h
 *
 * @file  <DESCRIPTION>
 * @author Tomás Sánchez
 * @since  09.03.2021
 */

#include "lib.h"
#include <commons/log.h>

// ============================================================================================================
//                               ***** Funciones Públicas, Declaraciones *****
// ============================================================================================================

// -----------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

/**
 * Instancia un logger con el nombre indicado.
 *
 * @param path la ruta en donde leer/escribir los loggins.
 * @param nombre_app el nombre del proceso que loggea.
 * @param consola si mostrar por consola o no
 * @return ERROR o SUCCESS
 */
int log_init(char *path, char *nombre_app, bool consola);

/**
 * Libera la memoria del logger.
 */
void log_close(void);

/**
 * @brief Garantiza mutua exclusión del logger. (WAIT)
 */
void logger_lock();

/**
 * @brief Libera el logger de un lock. (SIGNAL)
 */
void logger_unlock();

// -----------------------------------------------------------
//  Getter
// ------------------------------------------------------------

/**
 * Obtiene el logger, (No qieras meter la pata).
 *
 * @return la instancia del logger
 */
t_log *logger(void);

// -----------------------------------------------------------
//  Loggins
// ------------------------------------------------------------

/**
 * Loggea. si puede, un mensaje con nivel [INFO]
 *
 * @param info el mensaje de información a loggear
 * @param __VA_ARGS__ el formatting del logging.
 */
#define LOG_INFO(...)                    \
	if (logger())                        \
	{                                    \
		logger_lock();                   \
		log_info(logger(), __VA_ARGS__); \
		logger_unlock();                 \
	}

/**
 * Loggea. si puede, un mensaje con nivel [ERROR]
 *
 * @param error el mensaje de error a loggear
 * @param __VA_ARGS__ el formatting del logging.
 */
#define LOG_ERROR(...)                    \
	if (logger())                         \
	{                                     \
		logger_lock();                    \
		log_error(logger(), __VA_ARGS__); \
		logger_unlock();                  \
	}
/**
 * Loggea. si puede, un mensaje con nivel [DEBUG]
 *
 * @param debug el mensaje de debugging a loggear
 * @param __VA_ARGS__ el formatting del logging.
 */
#define LOG_DEBUG(...)                    \
	if (logger())                         \
	{                                     \
		logger_lock();                    \
		log_debug(logger(), __VA_ARGS__); \
		logger_unlock();                  \
	}

/**
 * Loggea. si puede, un mensaje con nivel [WARNING]
 *
 * @param warning el mensaje de advertencia a loggear
 * @param __VA_ARGS__ el formatting del logging.
 */
#define LOG_WARNING(...)                    \
	if (logger())                           \
	{                                       \
		logger_lock();                      \
		log_warning(logger(), __VA_ARGS__); \
		logger_unlock();                    \
	}
/**
 * Loggea. si puede, un mensaje con nivel [TRACE]
 *
 * @param trace el mensaje de tracing a loggear
 * @param __VA_ARGS__ el formatting del logging.
 */
#define LOG_TRACE(...)                    \
	if (logger())                         \
	{                                     \
		logger_lock();                    \
		log_trace(logger(), __VA_ARGS__); \
		logger_unlock();                  \
	}
