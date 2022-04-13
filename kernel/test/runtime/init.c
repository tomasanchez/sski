#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "runtime.h"
#include "ctest.h"

CTEST(kernel_init, calling_log_init_does_not_fail)
{
	if (log_init(LOG_FILE, MODULE_NAME, true) == ERROR)
		ASSERT_FAIL();

	log_close();
}

CTEST(kernel_init, calling_config_init_does_not_fail)
{
	if (config_init(CONF_FILE) == ERROR)
		ASSERT_FAIL();

	config_close();
}

CTEST(kernel_init, calling_thread_manager_init_init_does_not_fail)
{
	if (thread_manager_init() == ERROR)
		ASSERT_FAIL();

	thread_manager_end();
}

CTEST(kernel_init, calling_servidor_create_does_create_a_valid_server)
{
	context_t context;

	config_init(CONF_FILE);

	context.server = servidor_create(ip(), puerto_escucha());

	ASSERT_FALSE(context.server.iniciado);
	ASSERT_NOT_NULL(context.server.conexion.info_server);
	ASSERT_FALSE(context.server.conexion.conectado);
	ASSERT_TRUE(context.server.conexion.socket >= 0);

	servidor_destroy(&(context.server));

	config_close();
}