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
	if (log_init(MODULE_NAME, true) EQ ERROR)
		return LOG_INITIALIZATION_ERROR;

	LOG_DEBUG("Logger started.");

	if (config_init(MODULE_NAME) EQ ERROR)
	{
		LOG_ERROR("Could not open Configuration file.");
		log_close();
		return CONFIGURATION_INITIALIZATION_ERROR;
	}

	LOG_DEBUG("Configurations loaded.");

	thread_manager_init();

	/* BO initialization routines */

	/* EO initialization routines */

	signals_init();

	context->server = servidor_create(ip(), puerto_escucha());

	LOG_DEBUG("Server created at %s:%s", ip(), puerto_escucha());

	LOG_DEBUG("Kernel Module started SUCCESSFULLY");

	return EXIT_SUCCESS;
}

int on_run(context_t *context)
{
	if (servidor_escuchar(&(context->server)) == -1)
	{
		LOG_ERROR("Server could not listen.");
		return SERVER_RUNTIME_ERROR;
	}

	LOG_DEBUG("Server listenning. Awaiting for connections.");

	for (;;)
		servidor_run(&(context->server), routine);

	return EXIT_SUCCESS;
}

void on_before_exit(context_t *context, int exit_code)
{
	LOG_WARNING("Closing Kernel...");

	thread_manager_end();

	servidor_destroy(&(context->server));
	LOG_WARNING("Server has stopped.");

	/* BO finalization routines */

	/* EO finalization routines */

	config_close();

	LOG_WARNING("Configurations unloaded.");
	LOG_TRACE("Kernel ended with status <%d>.", exit_code);
	log_close();

	exit(exit_code);
}
