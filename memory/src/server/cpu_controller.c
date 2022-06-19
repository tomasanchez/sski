/**
 * @file cpu_controller.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-19-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "cpu_controller.h"
#include "conexion.h"
#include "server.h"
#include <time.h>

/**
 * @brief Obtains the memory position.
 *
 * @param socket CPU file descriptor
 * @return memory position
 */
uint32_t
obtain_memory_position(int socket);

/**
 * @brief Obtains the value of a position
 *
 * @param position of memory
 * @return stored value
 */
uint32_t
obtain_memory_value(uint32_t position);

void cpu_controller_read(int socket)
{
	uint32_t memory_position = obtain_memory_position(socket);

	uint32_t value = obtain_memory_value(memory_position);

	servidor_enviar_stream(RD, socket, &value, sizeof(value));
}

// ============================================================================================================
//                                   ***** Private Functions  *****
// ============================================================================================================

uint32_t
obtain_memory_position(int socket)
{
	ssize_t bytes_read = -1;
	void *stream = servidor_recibir_stream(socket, &bytes_read);
	uint32_t memory_position = *(uint32_t *)stream;
	free(stream);
	return memory_position;
}

uint32_t
obtain_memory_value(uint32_t position)
{
	return position + rand();
}
