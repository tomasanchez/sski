/**
 * runtime.h
 *
 * @file runtime interface for the server module
 * @author Tomás A. Sánchez and R. Nicolás Savinelli
 * @since  03.17.2022
 */

#pragma once

#include "server.h"
#include "thread_manager.h"
#include "cfg.h"
#include "log.h"

typedef struct context
{
	servidor_t server;
} context_t;

int init(context_t *context);

int run(context_t *context);

void stop(context_t *context, int exit_code);
