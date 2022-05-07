#include "cpu.h"
#include "server.h"
#include "dispatcher.h"
#include "routines.h"
#include "instruction.h"
#include "conexion.h"
#include "accion.h"
#include "log.h"
#include "cfg.h"
#include "cpu.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern cpu_t g_cpu;

// ============================================================================================================
//                                   ***** Funciones Privadas  *****
// ============================================================================================================

/**
 * @brief Realiza los tratamientos del mensaje recibido.
 *
 * @param cliente la posicion del cliente en la lista.
 * @return El mensaje leído.
 */
static char *
recibir_mensaje(int cliente)
{
	// Bytes recibidos
	ssize_t size = ERROR;
	// El mensaje (MSG) recibido
	return servidor_recibir_mensaje(cliente, &size);
}

// ============================================================================================================
//                                   ***** Funciones Publicas  *****
// ============================================================================================================
// TAKES SENDER FD AS INPUT
void *request_handler(void *fd)
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
				LOG_WARNING("Client <%d> has ended connection", sender_fd);
			}
			else
			{
				LOG_ERROR("Error while recieving a message from Client <%d>", sender_fd);
				LOG_DEBUG("Lost connection with Client <%d>", sender_fd);
			}

			servidor_desconectar_cliente(sender_fd);

			thread_manager_end_thread(&g_cpu.server_dispatch.tm);
			thread_manager_end_thread(&g_cpu.server_interrupt.tm);

			return NULL;
		}
		else
		{
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
