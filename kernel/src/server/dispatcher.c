#include <commons/string.h>
#include <commons/collections/list.h>

#include "dispatcher.h"
#include "accion.h"
#include "instruction.h"
#include "thread_manager.h"
#include "server.h"
#include "smartlist.h"
#include "accion.h"
#include "log.h"
#include "lib.h"
#include "cfg.h"
#include "pcb.h"
#include "kernel.h"

// ============================================================================================================
//                               ***** Dispatcher -  Definiciones *****
// ============================================================================================================

extern kernel_t g_kernel;

/**
 * @brief Logs and instruction.
 *
 * @param i an instruction reference
 */
void log_instruction(void *i);
// ============================================================================================================
//                               ***** Funciones Privadas - Definiciones *****
// ============================================================================================================

void *dispatch_imprimir_mensaje(void *args)
{
	char *msg = ((char *)args);

	THREAD_SAFE(LOG_INFO("Mensaje: %s", msg));

	free(msg);

	return NULL;
}

void *dispatch_handle_instruction(void *args, uint32_t *pid)
{
	void *instructions = (t_list *)args;

	list_iterate(instructions, log_instruction);

	pcb_t *pcb = get_pcb_by_pid(g_kernel.pcbs->_list, *pid);

	pcb->instructions = instructions;

	SIGNAL(g_kernel.scheduler.req_admit);

	return NULL;
}

void *dispatch_handle_syscall(void *args, uint32_t *pid)
{
	accion_t *accion = ((accion_t *)args);

	LOG_INFO("Received Action: %d %d", accion->actioncode, accion->param);

	switch (accion->actioncode)
	{
	case NEW_PROCESS:
		*pid = get_pid_libre(&(g_kernel.pids));
		if (*pid != UNDEFINED)
		{
			ssize_t process_size = accion->param;
			pcb_t *new_process = new_pcb(*pid, process_size, estimacion_inicial());
			safe_list_add(g_kernel.pcbs, new_process);
		}

		break;

	default:
		break;
	}

	accion_destroy(accion);

	return NULL;
}

void log_instruction(void *i)
{
	instruction_t *instruction = (instruction_t *)i;
	LOG_INFO("Received Instruction: %d %d %d", instruction->icode, instruction->param0, instruction->param1);
}
