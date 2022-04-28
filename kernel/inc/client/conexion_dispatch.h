#pragma once

/**
 * @brief Se llama a la rutina para el hilo-cliente que conecta con el cpu(dispatch)
 *
 * @param fd
 * @return void*
 */

void  *routine_conexion_dispatch(void *fd);
