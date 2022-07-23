#pragma once

#include "server.h"
#include "thread_manager.h"
#include "cfg.h"
#include "operands.h"
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

	// Max Frames per Process
	uint32_t max_frames;

	// Max entries per table
	uint32_t max_rows;

	// List of frames
	bool *frames;

	// Number of Frames in the memory
	uint32_t no_of_frames;

	// Tables of Level I
	safe_list_t *tables_lvl_1;

	// Tables of Level II
	safe_list_t *tables_lvl_2;

	// Metadata for SWAP files
	safe_list_t *swap_data;

	/**
	 * @brief Selects a frame to be replaced according to the algorithm
	 *
	 * @return the Frame number.
	 */
	uint32_t (*frame_selector)(void *self, uint32_t table_id);
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
