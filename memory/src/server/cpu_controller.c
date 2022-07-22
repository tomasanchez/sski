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
#include <math.h>
#include "cfg.h"
#include "os_memory.h"
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
receive_physical_address(int socket);

/**
 * @brief Obtains operands (physical address & value).
 *
 * @param socket CPU file descriptor
 * @return memory position
 */
operands_t
receive_operands(int socket, uint32_t *pid);

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

uint32_t
frame_exist(memory_t *memory, uint32_t frame);

uint32_t
get_frame(uint32_t physical_address);

bool frame_is_present(memory_t *memory, uint32_t table_number, uint32_t frame);

bool should_replace_frame(memory_t *memory, uint32_t table_number_2);

uint32_t
get_table_lvl1_number(memory_t *memory, uint32_t table_number_2);

void replaze_frame(uint32_t frame_to_replace);

// ============================================================================================================
//                                   ***** Endpoints  *****
// ============================================================================================================

void cpu_controller_read(int socket)
{
	uint32_t pid = UINT32_MAX;
	operands_t operands = receive_operands(socket, &pid);
	uint32_t physical_address = operands.op1;
	LOG_TRACE("[CPU-CONTROLLER] :=> Reading Physical Address <%d>.", physical_address);

	uint32_t frame = get_frame(physical_address);
	uint32_t table_number_2 = get_table_lvl2_number(&g_memory, frame);
	uint32_t value = UINT32_MAX;

	// Frame DOES NOT EXIST
	if (table_number_2 == UINT32_MAX)
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Invalid Frame <%d> not found in any table", frame);
	}
	else
	{
		page_table_lvl_2_t *frame_ref = get_frame_ref(&g_memory, frame);

		if (frame_ref)
			frame_ref->modified = false;

		value = read_from_memory(&g_memory, physical_address);
	}

	LOG_INFO("[Memory] :=> Read Value <%d> from <%d>", value, physical_address);

	ssize_t bytes_sent = servidor_enviar_stream(RD, socket, &value, sizeof(value));

	if (bytes_sent > 0)
	{
		LOG_DEBUG("[CPU-CONTROLLER] :=> Value <%d> was sent [%ld bytes]", value, bytes_sent);
	}
	else
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Value could not be sent.");
	}
}

void cpu_controller_write(int socket)
{

	uint32_t pid = UINT32_MAX;
	operands_t operands = receive_operands(socket, &pid);
	uint32_t physical_address = operands.op1;
	uint32_t value = operands.op2;
	LOG_TRACE("[CPU-CONTROLLER] :=> Writting into the Physical Address <%d>, Value <%d>", physical_address, value);

	uint32_t frame = get_frame(physical_address);
	uint32_t table_number_2 = get_table_lvl2_number(&g_memory, frame);

	// Frame DOES NOT EXIST
	if (table_number_2 == UINT32_MAX)
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Invalid Frame <%d> not found in any table", frame);
		return;
	}
	page_table_lvl_2_t *frame_ref = get_frame_ref(&g_memory, frame);

	if (frame_ref)
		frame_ref->modified = true;

	write_in_memory(&g_memory, physical_address, value);
	LOG_INFO("[Memory] :=> Value <%d> written into Physical Address <%d> (Frame #%d)", value, physical_address, frame);

	/*
	Frame_size = 256
	Physical Addres= 0
	Value = 1000

	0|1|2|3|.... 											|255

	#0
	0_
	1_
	2_
	...
	63_

	memcpy(g_memory + physical_address, &value, sizeof(value));

	0->0+sizeof(value)
	0-4
	frame_id = 0
	floor(0+4 / frame_size)= 0

	#0
	0_ 1000
	1_
	2_
	...
	63_
	----
	WRITE 4 1001
	memcpy(g_memory + physical_address, &value, sizeof(value));

	#0
	0_ 1000
	1_ 1001
	2_
	...
	63_

	WRITE 260 2000

	floor(260 / 256) = 1

	MAIN_MEMORY = 0
	#1
	[256]_0 2000
	[260]_1 2000
	264_2
	_3

	COPY 256 260

	WRITE 2048 4000

	FRAME #8
	[2048]_0 4000
	[2052]_1 0
	[2056]_2 0

	YA NO SE PUEDE USARE MAS MARCOS DE UN FRAME
	------------

	WRITE 2052 4001
	FRAME #8
	[2048]_0 4000
	[2052]_1 4001
	[2056]_2 0

	-------------
	WRITE 1024 3000
	FRAME #4

	! existe_frame(g_memory, dir_fisica)?
		-> !esta_presente()?
		if hay_lugar() ?
			-> create_frame();
		else
			->replace_frame()
			-> frame_a_reemplazar = g_memory.frame_selector();
			-> swap_frame();
				-> frame_a_reemplazar.presente = false
				-> swap(direc_fisica);
			-> create_frame();

	-> write(direc_fisica, valor);

	*/
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
		LOG_TRACE("[CPU-CONTROLLER] :=> Requested Table#%d[%d]...", values.op1, values.op2);
		entry_second_level = obtain_second_page(values.op1, values.op2);
		LOG_INFO("[CPU-CONTROLLER] :=> Table#%d[%d]= #%d", values.op1, values.op2, entry_second_level);
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
receive_physical_address(int socket)
{
	ssize_t bytes_read = -1;
	void *stream = servidor_recibir_stream(socket, &bytes_read);
	uint32_t memory_position = *(uint32_t *)stream;
	free(stream);
	return memory_position;
}

operands_t
receive_operands(int socket, uint32_t *pid)
{
	ssize_t bytes_read = -1;
	void *stream = servidor_recibir_stream(socket, &bytes_read);
	LOG_TRACE("[CPU-CONTROLLER] :=> Received Package [%ld bytes]", bytes_read);
	memcpy(pid, stream, sizeof(uint32_t));
	operands_t operands;
	memcpy(&operands, stream + sizeof(uint32_t), sizeof(operands_t));
	free(stream);
	return operands;
}

uint32_t
obtain_memory_value(uint32_t position)
{
	return position + rand();
}

uint32_t
obtain_second_page(uint32_t id_table_1, uint32_t index)
{
	page_table_lvl_1_t *table_lvl1 = safe_list_get(g_memory.tables_lvl_1, id_table_1);

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
	page_table_lvl_2_t *table_lvl2 = safe_list_get(g_memory.tables_lvl_2, id_table_2);

	// Fail Fast
	if (index > g_memory.max_rows)
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Index out of bounds for FRAMES");
		return UINT32_MAX;
	}

	if (!table_lvl2[index].present)
	{
		LOG_ERROR("[CPU-CONTROLLER] :=> Page Fault: Frame  not allocated");
		uint32_t new_frame = find_free_frame(&g_memory);

		if (should_replace_frame(&g_memory, index))
		{
			LOG_WARNING("[MEMORY] :=> Page replacement is required");
			uint32_t id_table_1 = get_table_lvl1_number(&g_memory, id_table_2);
			uint32_t frame_to_replace = g_memory.frame_selector(&g_memory, id_table_1);
			replaze_frame(frame_to_replace);
			LOG_INFO("[ALGORITHM] :=> Replaced Frame: #%d -> #%d", frame_to_replace, new_frame);
		}

		if (table_lvl2[index].frame == INVALID_FRAME)
		{
			table_lvl2[index].frame = new_frame;
			table_lvl2[index].present = true;
			table_lvl2[index].use = true;
			LOG_INFO("[Memory] :=> Table#%d[%d] = { Frame: %d ...}", id_table_2, index, new_frame);
		}
		else
		{
			uint32_t created_at = create_frame_for_table(&g_memory, index, id_table_2);
			LOG_INFO("[Memory] :=> Table#%d[%d] = { Frame: %d ...}", id_table_2, created_at, new_frame);
		}

		return new_frame;
	}
	else
	{
		LOG_WARNING("[Memory] :=> Frame <%d> modified", table_lvl2[index].frame);
	}

	return table_lvl2[index].frame;
}

uint32_t frame_exist(memory_t *memory, uint32_t frame)
{
	return get_table_lvl2_number(memory, frame);
}

uint32_t
get_frame(uint32_t physical_address)
{
	return floor(physical_address / tam_pagina());
}

bool frame_is_present(memory_t *memory, uint32_t table_number, uint32_t frame)
{
	page_table_lvl_2_t *table = safe_list_get(memory->tables_lvl_2, table_number);

	for (uint32_t i = 0; i < memory->max_rows; i++)
	{
		if (table[i].frame == frame)
		{
			return table[i].present;
		}
	}

	return false;
}

bool should_replace_frame(memory_t *memory, uint32_t table_number_2)
{
	uint32_t table_number = get_table_lvl1_number(memory, table_number_2);
	page_table_lvl_1_t *table = safe_list_get(memory->tables_lvl_1, table_number);
	uint32_t count = 0;

	for (uint32_t i = 0; i < memory->max_rows; i++)
	{
		page_table_lvl_2_t *table_2 = safe_list_get(memory->tables_lvl_2, table[i].second_page);
		for (uint32_t j = 0; j < memory->max_rows; j++)
		{
			if (table_2[j].present == true)
			{
				count++;
			}
		}
	}

	return count >= (uint32_t)marcos_por_proceso();
}

uint32_t
get_table_lvl1_number(memory_t *memory, uint32_t table_number_2)
{
	uint32_t size = (uint32_t)list_size(memory->tables_lvl_2->_list);

	for (uint32_t i = 0; i < size; i++)
	{
		page_table_lvl_1_t *table = safe_list_get(memory->tables_lvl_1, i);

		for (uint32_t j = 0; j < memory->max_rows; j++)
		{
			if (table[j].second_page == table_number_2)
			{
				return i;
			}
		}
	}

	return UINT32_MAX;
}

void replaze_frame(uint32_t frame_to_replace)
{
	uint32_t table_number_2 = get_table_lvl2_number(&g_memory, frame_to_replace);
	page_table_lvl_2_t *table = safe_list_get(g_memory.tables_lvl_2, table_number_2);

	for (uint32_t i = 0; i < g_memory.max_rows; i++)
	{
		if (table[i].frame == frame_to_replace)
		{
			table[i].present = false;
			table[i].use = false;
		}
	}
}
