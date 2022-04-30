#pragma once

#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "accion.h"
#include "instruction.h"

#include "thread_manager.h"

void *dispatch_imprimir_mensaje(void *args);

void *dispatch_handle_instruction(void *args, uint32_t *pid);

void *dispatch_handle_syscall(void *args, uint32_t *pid);
