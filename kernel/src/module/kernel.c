#include "server.h"
#include "lib.h"
#include "log.h"
#include "cfg.h"
#include "routines.h"
#include "thread_manager.h"
#include "signals.h"
#include "kernel.h"
#include "main.h"

int on_init(context_t *context)
{
	if (log_init(MODULE_NAME, true) == ERROR)
		return LOG_INITIALIZATION_ERROR;

	if (config_init("kernel") == ERROR)
	{
		LOG_ERROR("No se pudo levantar la configuracion.");
		return CONFIGURATION_INITIALIZATION_ERROR;
	}

	thread_manager_init();

	/* BO initialization routines */

	/* EO initialization routines */

	signals_init();

	context->server = servidor_create(ip(), puerto_escucha());
	LOG_DEBUG("Escuchando en %s:%s", ip(), puerto_escucha());

	return EXIT_SUCCESS;
}

int on_run(context_t *context)
{
	if (servidor_escuchar(&(context->server)) == -1)
	{
		LOG_ERROR("No se pudo levantar el servidor.");
		return SERVER_RUNTIME_ERROR;
	}

	LOG_DEBUG("Servidor a la espera de clientes...");

	for (;;)
		servidor_run(&(context->server), routine);

	return EXIT_SUCCESS;
}

void on_before_exit(context_t *context, int exit_code)
{
	LOG_DEBUG("Finalizando ejecucion.\n");

	thread_manager_end();

	servidor_destroy(&(context->server));

	/* BO finalization routines */

	/* EO finalization routines */

	config_close();

	log_close();

	exit(exit_code);
}
