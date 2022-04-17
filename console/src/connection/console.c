/**
 * i-mongo-store.c
 *
 * @file  Módulo iMongo Store
 * @author Tomás Sánchez
 * @since  05.02.2021
 */

#include "console.h"
#include "log.h"
#include "cfg.h"
#include "lib.h"
#include "network.h"
#include <signal.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

// El modulo servidor propiamente dicho
static client_m_t this;

// ============================================================================================================
//                                   ***** Funciones Privadas - Declaraciones  *****
// ============================================================================================================

/**
 * @brief Realiza la liberación de memoria. De detectarse la señal de interrupción.
 *
 * @param signal la signal detectada.
 */
static void cierre_forzoso(int signal);

/**
 * @brief Manda al servidor a escuchar clientes.
 *
 * @return int
 */
static int conexion_init(void);
// ============================================================================================================
//                                   ***** Funciones Privadas - Definiciones  *****
// ============================================================================================================

static void cierre_forzoso(int signal)
{
	if (signal == SIGINT)
	{
		this.signal = signal;
		LOG_WARNING("Se detectó el cierre del proceso vía una señal");
		on_before_exit();
		exit(SIGINT);
	}
}

static int conexion_init(void)
{
	char *port = puerto();
	LOG_DEBUG("Inicializando Cliente en %s:%s", ip(), port);
	this.conexion = conexion_cliente_create(ip(), port);

	if (on_connect(&this.conexion, false) EQ SUCCESS)
	{
		LOG_DEBUG("Modulo conectado en %s:%s", ip(), port);
	}

	return SUCCESS;
}

// ============================================================================================================
//                                   ***** Funciones Públicas  *****
// ============================================================================================================

// ------------------------------------------------------------
//  Life Cycle
// ------------------------------------------------------------

int on_init(void)
{
	if (log_init("console", true) EQ ERROR)
		return ERROR;

	LOG_DEBUG("Logger Inicializado");

	if (config_init(CONF_FILE) EQ ERROR)
	{
		LOG_ERROR("No se abrió la configuración.");
		log_close();
		return ERROR;
	}

	LOG_DEBUG("Configuraciones cargadas");

	// Attach del evento de interrupcion forzada.
	signal(SIGINT, cierre_forzoso);

	LOG_DEBUG("Modulo inicializado!");
	this.status = RUNNING;
	return conexion_init();
}

int on_before_exit(void)
{
	LOG_DEBUG("Cerrando Modulo...");

	config_close();
	LOG_DEBUG("Se liberó la configuración");

	conexion_destroy(&this.conexion);
	LOG_DEBUG("Conexion terminada.");

	LOG_DEBUG("Finalizado");
	log_close();

	if (this.signal == SIGINT)
		exit(SIGINT);

	return EXIT_SUCCESS;
}

// ------------------------------------------------------------
//  Event Handlers
// ------------------------------------------------------------

int on_client_run(void)
{

	while (this.status EQ RUNNING)
	{
		on_client_send(&this.conexion, on_client_read(readline(">> "), &this.status));
	}

	return EXIT_SUCCESS;
}

int on_connect(void *conexion, bool offline_mode)
{
	if (offline_mode)
	{
		LOG_WARNING("Modulo trabajando sin conexion...");
		return ERROR;
	}

	while (!conexion_esta_conectada(*(conexion_t *)conexion))
	{
		LOG_TRACE("Intentando conectar...");

		if (conexion_conectar((conexion_t *)conexion) EQ ERROR)
		{
			LOG_ERROR("No se pudo realizar la conexion...");
			sleep(TIEMPO_ESPERA);
		}
	}

	return SUCCESS;
}

char *on_client_read(char *line, bool *status)
{
	if (line)
	{
		bool is_empty = strcmp(line, EMPTY_STR) EQ 0;

		if (is_empty)
		{
			*status = not RUNNING;
			free(line);
			return NULL;
		}
		else
			return line;
	}
	else
		return NULL;
}

int on_client_send(void *conexion, char *line)
{
	if (line)
	{
		// Si envio algo sera mayor a 0
		int result = conexion_enviar_mensaje(*(conexion_t *)conexion, line) > 0 ? SUCCESS : ERROR;

		free(line);

		return result;
	}

	return ERROR;
}
