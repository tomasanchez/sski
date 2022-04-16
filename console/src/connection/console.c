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
#include "smartlist.h"
#include "parser.h"
#include "scanner.h"
#include <signal.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>

#define YYACCEPT 0
#define YYABORT 1
#define YYNOMEM 2

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

// El modulo servidor propiamente dicho
static client_m_t this;

t_list *commands = NULL;

int yylexerrs = 0;

extern int yynerrs;

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
	char *port = puerto_kernel();
	LOG_DEBUG("Inicializando Cliente en %s:%s", ip_kernel(), port);
	this.conexion = conexion_cliente_create(ip_kernel(), port);

	if (on_connect(&this.conexion, false) EQ SUCCESS)
	{
		LOG_DEBUG("Modulo conectado en %s:%s", ip_kernel(), port);
	}

	return SUCCESS;
}

static unsigned int analizar_instrucciones(char *instructions_file)
{
	unsigned int yyresult = 0;

	yyin = fopen(instructions_file, "r");
	switch (yyparse())
	{
	case YYACCEPT:
		LOG_DEBUG("No se encontraron errores");
		yyresult = YYACCEPT;
		break;
	case YYABORT:
		LOG_ERROR("Se encontraron errores en la lista de instrucciones\n");
		yyresult = YYABORT;
		break;
	case YYNOMEM:
		LOG_ERROR("Memoria insuficiente\n");
		yyresult = YYNOMEM;
		break;
	}

	fclose(yyin);

	yylex_destroy();

	return yyresult;
}
// ============================================================================================================
//                                   ***** Funciones Públicas  *****
// ============================================================================================================

// ------------------------------------------------------------
//  Life Cycle
// ------------------------------------------------------------

int on_init(int argc)
{
	switch (argc)
	{
	case 1:
		printf("console: fatal error: no input files");
		return ERROR;
		break;

	case 2:
		printf("console: fatal error: no process size");
		return ERROR;
		break;

	default:
		break;
	}

	commands = list_smart_create(commands);

	if (log_init("client.log", "Client", true) EQ ERROR)
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

	list_smart_fast_destroy(commands);

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

int on_client_run(char *instructions_file)
{
	LOG_DEBUG("Analizando sintáctica y semánticamente la lista de instrucciones");
	unsigned int analysis_result = analizar_instrucciones(instructions_file);

	if (analysis_result != YYACCEPT)
		this.status = not RUNNING;

	if (yynerrs == 0 && yylexerrs == 0)
	{
		while (this.status EQ RUNNING)
		{
			while (commands->elements_count > 0)
			{
				on_client_send(&this.conexion, (char *)list_remove(commands, 0));
			}

			this.status = not RUNNING;
		}
	}

	return analysis_result;
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
