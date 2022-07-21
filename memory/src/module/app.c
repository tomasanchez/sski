/**
 * @file memory.c
 * @author fmaver@frba.utn.edu.ar
 * @brief
 * @version 0.1
 * @date 2022-04-28
 *
 * @copyright Copyright (c) 2022
 *
 */

// ============================================================================================================
//                                   ***** Dependencies  *****
// ============================================================================================================

#define MODULE_NAME "memory"

#include "lib.h"
#include "log.h"
#include "cfg.h"
#include "thread_manager.h"
#include "memory_module.h"
#include "memory_routines.h"
#include "signals.h"
#include "algorithms.h"

// ============================================================================================================
//                                   ***** Private Functions  *****
// ============================================================================================================
/**
 * @brief Initializes the Memory module
 *
 * @param memory the Memory module
 * @return success or failure
 */
static int on_init_memory(memory_t *memory);

/**
 * @brief Destroy the memory elements
 *
 * @param memory the Memory module
 */
static void on_delete_memory(memory_t *memory);

/**
 * @brief Run server
 *
 * @param memory
 * @return SERVER_RUNTIME_ERROR or EXIT_SUCCESS
 */
static int serve(memory_t *memory);

static int on_init_memory(memory_t *memory)
{
	// Init App
	memory->server = servidor_create(IP, puerto_escucha());
	memory->tm = new_thread_manager();

	// Init Memory
	memory->main_memory = malloc(tam_memoria());
	memory->tables_lvl_1 = new_safe_list();
	memory->tables_lvl_2 = new_safe_list();

	// Init Frames
	memory->frame_selector = strcmp(algoritmo_reemplazo(), "CLOCK") == 0 ? clock_selector : improved_clock_selector;
	memory->max_frames = (uint32_t)marcos_por_proceso();
	memory->max_rows = (uint32_t)entradas_por_tabla();
	memory->no_of_frames = (uint32_t)tam_memoria() / (uint32_t)tam_pagina();
	memory->frames = malloc(sizeof(bool) * memory->no_of_frames);
	for (uint32_t i = 0; i < memory->no_of_frames; i++)
	{
		memory->frames[i] = false;
	}

	return EXIT_SUCCESS;
}

static void on_delete_memory(memory_t *memory)
{
	servidor_destroy(&(memory->server));
	thread_manager_destroy(&(memory->tm));
	free(memory->main_memory);
	safe_list_fast_destroy(memory->tables_lvl_1);
	safe_list_fast_destroy(memory->tables_lvl_2);
}

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

int on_init(memory_t *memory)
{
	if (log_init(MODULE_NAME, true) EQ ERROR)
		return LOG_INITIALIZATION_ERROR;

	LOG_DEBUG("Logger started.");

	if (config_init(MODULE_NAME) EQ ERROR)
	{
		LOG_ERROR("Could not open Configuration file.");
		log_close();
		return CONFIGURATION_INITIALIZATION_ERROR;
	}

	LOG_DEBUG("Configurations loaded.");

	if (on_init_memory(memory) EQ EXIT_SUCCESS)
	{
		LOG_DEBUG("Memory initializated");
	}

	signals_init();

	LOG_DEBUG("Server created at %s:%s", IP, puerto_escucha());
	LOG_DEBUG("Memory Module started SUCCESSFULLY");

	return EXIT_SUCCESS;
}

int on_run(memory_t *memory)
{

	serve(memory);

	return EXIT_SUCCESS;
}

void on_before_exit(memory_t *memory, int exit_code)
{
	LOG_WARNING("Closing Memory...");

	on_delete_memory(memory);

	LOG_WARNING("Server has stopped.");

	config_close();

	LOG_WARNING("Configurations unloaded.");
	LOG_TRACE("Memory ended with status <%d>.", exit_code);
	log_close();

	exit(exit_code);
}

int serve(memory_t *memory)
{
	if (servidor_escuchar(&(memory->server)) == -1)
	{
		LOG_ERROR("Server could not listen.");
		return SERVER_RUNTIME_ERROR;
	}

	LOG_DEBUG("Server listenning. Awaiting for connections.");

	for (;;)
		servidor_run(&(memory->server), routine);

	return EXIT_SUCCESS;
}
