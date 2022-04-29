/**
 * @file module.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-29-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "module.h"
#include "log.h"
#include "lib.h"

int on_module_connect(void *connection, bool offline_mode)
{
	if (offline_mode)
	{
		LOG_WARNING("Module working in offline mode.");
		return ERROR;
	}

	while (!conexion_esta_conectada(*(conexion_t *)connection))
	{
		LOG_TRACE("Connecting...");

		if (conexion_conectar((conexion_t *)connection) EQ ERROR)
		{
			LOG_ERROR("Could not connect.");
			sleep(TIEMPO_ESPERA);
		}
	}

	return SUCCESS;
}
