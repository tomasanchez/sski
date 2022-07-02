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
 * @param socket of the client
 */
void request_handler_int(int socket);
// ============================================================================================================
//                                   ***** Funciones Privadas  *****
// ============================================================================================================

void request_handler_int(int fd)
{
	g_cpu.has_interruption = true;
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
				request_handler_int(sender_fd);
				break;

			case PCB:
				receive_pcb(sender_fd);
				SIGNAL(g_cpu.sync.pcb_received);
				break;
			default:
				LOG_ERROR("[Server] :=> Client<%d>: Unrecognized operation code (%d)", sender_fd, opcode);
				break;
			}
		}
	}

	return NULL;
}
