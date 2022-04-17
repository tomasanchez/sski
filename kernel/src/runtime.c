#include "server.h"
#include "lib.h"
#include "log.h"
#include "cfg.h"
#include "routines.h"
#include "thread_manager.h"
#include "signals.h"
#include "runtime.h"
#include "main.h"

int init(context_t *context)
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

	context->server = servidor_create(ip(), puerto());

	return EXIT_SUCCESS;
}

int run(context_t *context)
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

void stop(context_t *context, int exit_code)
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
