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
#include "opcode.h"
#include "accion.h"

/**
 * @brief Forces a module connection
 *
 * @param connection to connect
 * @param offline_mode wether the module is working offline or not.
 * @return exit status code
 */
int on_module_connect(void *connection, bool offline_mode);

/**
 * @brief module.h - Sends an action with the specified opcode.
 *
 * @param is_conexion
 * @param opcode
 * @param actioncode
 * @param param
 * @return ssize_t
 */
ssize_t on_send_action(conexion_t is_conexion, opcode_t opcode, actioncode_t actioncode, uint32_t param);
