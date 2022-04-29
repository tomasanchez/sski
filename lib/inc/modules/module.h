/**
 * @file module.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-29-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "conexion.h"

/**
 * @brief Forces a module connection
 *
 * @param connection to connect
 * @param offline_mode wether the module is working offline or not.
 * @return exit status code
 */
int on_module_connect(void *connection, bool offline_mode);
