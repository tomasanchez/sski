#pragma once

#include "server.h"
#include "thread_manager.h"
#include "cfg.h"
#include "safe_list.h"
#include "log.h"

typedef struct Memory
{
	// The Memory Multi-thread Server dependency.
	servidor_t server;

	// The Memory thread manager
	thread_manager_t tm;

	// The module main memory.
	void *main_memory;

	// Tables of Level I
	safe_list_t *tables_lvl_1;

	// Tables of Level II
	safe_list_t *tables_lvl_2;

} memory_t;

/**
 * @brief Initializes the Memory Module with the required structures.
 *
 * @param memory the server memory structure.
 * @return an status code.
 */
int on_init(memory_t *memory);

/**
 * @brief Responsible of Memory main processing action.
 *
 * @param memory the server memory structure.
 * @return an status code.
 */
int on_run(memory_t *memory);

/**
 * @brief Method called when Memory does close. Destroys created structures.
 *
 * @param memory the current processing memory
 * @param exit_code an status code
 */
void on_before_exit(memory_t *memory, int exit_code);
