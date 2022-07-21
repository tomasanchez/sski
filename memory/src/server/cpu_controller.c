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
#include "cfg.h"
#include "server.h"
#include "log.h"
#include "operands.h"
#include "memory_module.h"
#include <time.h>
#include "page_table.h"

extern memory_t g_memory;

// ============================================================================================================
//                                   ***** Definitions  *****
// ============================================================================================================

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

uint32_t
obtain_second_page(uint32_t id_table_1, uint32_t index);

uint32_t
obtain_frame(uint32_t id_table_2, uint32_t index);
// ============================================================================================================
//                                   ***** Endpoints  *****
// ============================================================================================================

void cpu_controller_read(int socket)
{
	uint32_t memory_position = obtain_memory_position(socket);
	LOG_TRACE("Reading position %d of Memory.", memory_position);

	uint32_t value = obtain_memory_value(memory_position);
	LOG_TRACE("Position read has value= %d.", value);

	ssize_t bytes_sent = servidor_enviar_stream(RD, socket, &value, sizeof(value));

	if (bytes_sent > 0)
	{
		LOG_DEBUG("Value <%d> was sent [%ld bytes]", value, bytes_sent);
	}
	else
	{
		LOG_ERROR("Value could not be sent.");
	}
}

void cpu_controller_send_entries(int fd)
{

	uint32_t entries = entradas_por_tabla();
	LOG_TRACE("[CPU-CONTROLLER] :=> Entries per table: %d", entries);
	ssize_t bytes_sent = fd_send_value(fd, &entries, sizeof(entries));

	if (bytes_sent > 0)
	{
		LOG_DEBUG("[CPU-CONTROLLER] :=> Sent entires per table [%ld bytes]", bytes_sent);
	}
	else
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Sent nothing - THIS SHOULD NEVER HAPPEN");
	}
}

void cpu_controller_send_size(int fd)
{
	// TAM_PAGINA
	uint32_t size = tam_pagina();
	LOG_TRACE("[CPU-CONTROLLER] :=> Page Size is : %dB", size);
	ssize_t bytes_sent = fd_send_value(fd, &size, sizeof(size));

	if (bytes_sent > 0)
	{
		LOG_DEBUG("[CPU-CONTROLLER] :=> Sent Page Size [%ld bytes]", bytes_sent);
	}
	else
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Sent nothing - THIS SHOULD NEVER HAPPEN");
	}
}

void cpu_controller_send_frame(int fd)
{
	ssize_t bytes_read = -1;
	void *stream = servidor_recibir_stream(fd, &bytes_read);
	uint32_t frame = 0;

	if (bytes_read <= 0)
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Could not read stream");
		frame = UINT32_MAX;
	}
	else
	{
		operands_t values = operandos_from_stream(stream);
		frame = obtain_frame(values.op1, values.op2);
		LOG_TRACE("[CPU-CONTROLLER] :=> Frame obtained #%d", frame);
	}

	ssize_t bytes_sent = fd_send_value(fd, &frame, sizeof(frame));

	if (bytes_sent > 0)
	{
		LOG_DEBUG("[CPU-CONTROLLER] :=> Frame sent [%ld bytes]", bytes_sent);
	}
	else
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Sent nothing - THIS SHOULD NEVER HAPPEN");
	}
}

void cpu_controller_send_page_second_level(int fd)
{
	// TODO --> Corregir
	ssize_t bytes_read = -1;
	void *stream = servidor_recibir_stream(fd, &bytes_read);
	uint32_t entry_second_level = 0;

	if (bytes_read <= 0)
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Could not read stream");
		entry_second_level = UINT32_MAX;
	}
	else
	{
		operands_t values = operandos_from_stream(stream);
		entry_second_level = obtain_second_page(values.op1, values.op2);
		LOG_TRACE("[CPU-CONTROLLER] :=> Table LVL 2 ID #%d", entry_second_level);
	}

	ssize_t bytes_sent = fd_send_value(fd, &entry_second_level, sizeof(entry_second_level));
	if (bytes_sent > 0)
	{
		LOG_DEBUG("[CPU-CONTROLLER] :=> Table LVL 2 sent size is [%ld bytes]", bytes_sent);
	}
	else
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Sent nothing - THIS SHOULD NEVER HAPPEN");
	}
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

uint32_t
obtain_second_page(uint32_t id_table_1, uint32_t index)
{
	// TODO: Fix LIST_GET
	page_table_lvl_1_t *table_lvl1 = list_get(g_memory.tables_lvl_1->_list, id_table_1);

	if (index > g_memory.max_rows)
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Index out of bounds for TABLE LVL 2");
		return UINT32_MAX;
	}

	return table_lvl1[index].second_page;
}

uint32_t
obtain_frame(uint32_t id_table_2, uint32_t index)
{
	// TODO: Fix LIST_GET
	page_table_lvl_2_t *table_lvl2 = list_get(g_memory.tables_lvl_2->_list, id_table_2);

	// TODO: VALIDAR QUE ESTO ESTA BIEN (Me huele mal)
	if (index > g_memory.max_rows)
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Index out of bounds for FRAMES");
		return UINT32_MAX;
	}

	return table_lvl2[index].frame;
}
