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

	LOG_WARNING("[Server] :=> Recovering a PCB with PID: <%d>", *pid);
	pcb_t *pcb = get_pcb_by_pid(g_kernel.pcbs->_list, *pid);

	if (pcb == NULL)
	{
		LOG_ERROR("[Server] :=> PCB with PID: <%d> not found", *pid);
		return NULL;
	}
	else
	{
		LOG_DEBUG("[Server] :=> PCB was found");
	}

	pcb->instructions = instructions;

	int sem_value = -99;

	sem_getvalue(g_kernel.scheduler.req_admit, &sem_value);
	LOG_WARNING("[Server] :=> Semaphore has value <%d> Before Signal", sem_value);
	SIGNAL(g_kernel.scheduler.req_admit);
	sem_getvalue(g_kernel.scheduler.req_admit, &sem_value);
	LOG_WARNING("[Server] :=> Semaphore has value <%d> After Signal", sem_value);

	return NULL;
}

void *dispatch_handle_syscall(void *args, uint32_t *pid)
{
	accion_t *accion = ((accion_t *)args);

	LOG_DEBUG("[Server] :=> Received Action: %d %d", accion->actioncode, accion->param);

	switch (accion->actioncode)
	{
	case NEW_PROCESS:

		LOG_WARNING("[Server] :=> Obtaining an available process ID");

		*pid = get_pid_libre(&(g_kernel.pids));

		if (*pid != UNDEFINED)
		{
			LOG_DEBUG("[Server] :=> PID <%d> was assigned", *pid);
			ssize_t process_size = accion->param;
			pcb_t *new_process = new_pcb(*pid, process_size, estimacion_inicial());
			safe_list_add(g_kernel.pcbs, new_process);
		}
		else
		{
			LOG_ERROR("[Server] :=> Couldn't assign a PID");
		}

		break;

	default:
		LOG_ERROR("[Server] :=> Undefined Action Code");
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
