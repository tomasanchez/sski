#include "memory_routines.h"
#include "memory_module.h"
#include "memory_dispatcher.h"

// ============================================================================================================
//                                   ***** Definitions  *****
// ============================================================================================================

extern memory_t g_memory;

// ============================================================================================================
//                                   ***** Private Functions *****
// ============================================================================================================

static char *recibir_mensaje(int cliente)
{
	// Bytes recibidos
	ssize_t size = ERROR;
	// El mensaje (MSG) recibido
	return servidor_recibir_mensaje(cliente, &size);
}

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

			thread_manager_end_thread(&g_memory.server.tm);

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

			default:
				LOG_ERROR("Client<%d>: Unrecognized operation code (%d)", sender_fd, opcode);
				break;
			}
		}
	}

	return NULL;
}
