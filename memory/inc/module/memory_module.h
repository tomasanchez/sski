#pragma once

#include "server.h"
#include "thread_manager.h"
#include "cfg.h"
#include "log.h"


typedef struct Context
{
	// The MEmory Multi-thread Server dependency.
	servidor_t server;

	thread_manager_t tm;

} context_t;

/**
 * @brief Initializes the Memory Module with the required structures.
 *
 * @param context the server context structure.
 * @return an status code.
 */
int on_init(context_t *context);

/**
 * @brief Responsible of Memory main processing action.
 *
 * @param context the server context structure.
 * @return an status code.
 */
int on_run(context_t *context);

/**
 * @brief Method called when Memory does close. Destroys created structures.
 *
 * @param context the current processing context
 * @param exit_code an status code
 */
void on_before_exit(context_t *context, int exit_code);
