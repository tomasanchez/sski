#include "cpu.h"
#include "server.h"
#include "dispatcher.h"
#include "request_handler.h"
#include "instruction.h"
#include "conexion.h"
#include "accion.h"
#include "log.h"
#include "cfg.h"
#include "cpu.h"
#include "pcb_controller.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern cpu_t g_cpu;

/**
 * @brief Handle Interrupt request
 *
 */
void request_handler_int();
// ============================================================================================================
//                                   ***** Funciones Privadas  *****
// ============================================================================================================

void request_handler_int()
{
	LOG_ERROR("[INT] :=> Interruption received");

	if (g_cpu.pcb != NULL)
	{
		g_cpu.has_interruption = true;
		LOG_DEBUG("[INT] :=> Execution interrupted");
	}
	else
	{
		LOG_ERROR("[INT] :=> Ignoring interrupt request (No PCB)");
	}
}

// ============================================================================================================
//                                   ***** Funciones Publicas  *****
// ============================================================================================================
// TAKES SENDER FD AS INPUT
void *
request_handler(void *fd)
{

	int sender_fd = 0;
	memcpy((void *)&sender_fd, fd, sizeof(int));
	free(fd);

	for (;;)
	{
		int opcode = servidor_recibir_operacion(sender_fd);

		if (opcode <= 0)
		{
			if (opcode == DC)
			{
				LOG_WARNING("[Server] :=> Client <%d> has ended connection", sender_fd);
			}
			else
			{
				LOG_ERROR("[Server] :=> Error while recieving a message from Client <%d>", sender_fd);
				LOG_DEBUG("[Server] :=> Lost connection with Client <%d>", sender_fd);
			}

			servidor_desconectar_cliente(sender_fd);

			int result = 0;
			result += thread_manager_end_thread(&g_cpu.server_dispatch.tm);
			result += thread_manager_end_thread(&g_cpu.server_interrupt.tm);

			if (result == 2)
			{
				LOG_ERROR("[Server] :=> NO THREAD WAS INTERRUPTED; THIS SHOULD NEVER HAPPEN");
			}

			return NULL;
		}
		else
		{
			LOG_TRACE("[Server] :=> Client<%d>: Requests <%s> operation.", sender_fd, opcode_to_string(opcode));

			switch (opcode)
			{
			case INT:
				request_handler_int();
				break;

			case PCB:
				receive_pcb(sender_fd);
				break;

			default:
				LOG_ERROR("[Server] :=> Client<%d>: Unrecognized operation code (%d)", sender_fd, opcode);
				break;
			}
		}
	}

	return NULL;
}
