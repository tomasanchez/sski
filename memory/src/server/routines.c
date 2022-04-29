#include "routines.h"
#include "memoryModule.h"


// ============================================================================================================
//                                   ***** Definitions  *****
// ============================================================================================================

extern context_t g_context;


// ============================================================================================================
//                                   ***** Private Functions *****
// ============================================================================================================




// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

// TAKES SENDER FD AS INPUT
void *routine(void *fd)
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
				// Connection closed
				LOG_WARNING("Client <%d> has ended connection", sender_fd);
			}
			else
			{
				LOG_ERROR("Error while recieving a message from Client <%d>", sender_fd);
				LOG_DEBUG("Lost connection with Client <%d>", sender_fd);
			}

			servidor_desconectar_cliente(sender_fd); // Bye!

			thread_manager_end_thread(&g_context.server.tm);

			return NULL;
		}
		else
		{
			// We got some good server from a client
			LOG_TRACE("Client<%d>: Requests <%s> operation.", sender_fd, opcode_to_string(opcode));

			switch (opcode)
			{
			case MSG:
				dispatch_imprimir_mensaje((void *)recibir_mensaje(sender_fd));
				break;

			case SYS:
				dispatch_handle_action((void *)accion_recibir(sender_fd));
				break;

			case CMD:
				dispatch_handle_instruction((void *)recibir_instruction(sender_fd));
				break;

			default:
				LOG_ERROR("Client<%d>: Unrecognized operation code (%d)", sender_fd, opcode);
				break;
			}
		}
	}

	return NULL;
}
