#include "kernel.h"
#include "server.h"
#include "dispatcher.h"
#include "instruction.h"
#include "conexion.h"
#include "accion.h"
#include "log.h"
#include "cfg.h"
#include "pcb.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern kernel_t g_kernel;

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

/**
 * @brief Recieves a list of instructions
 *
 * @param fd the socket of the client
 * @return an instruction list.
 */
static t_list *recibir_instructions(int fd)
{
	ssize_t size = ERROR;
	void *stream = servidor_recibir_stream(fd, &size);
	return instruction_list_from(stream);
}

// ============================================================================================================
//                                   ***** Funciones Publicas  *****
// ============================================================================================================
// TAKES SENDER FD AS INPUT
void *routine(void *fd)
{
	int sender_fd = 0;
	uint32_t sender_pid = 0;

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
				LOG_WARNING("[Server] :=> Client <%d> has ended connection", sender_fd);
			}
			else
			{
				LOG_ERROR("[Server] :=> Error while recieving a message from Client <%d>", sender_fd);
				LOG_DEBUG("[Server] :=> Lost connection with Client <%d>", sender_fd);
			}

			servidor_desconectar_cliente(sender_fd); // Bye!

			thread_manager_end_thread(&g_kernel.server.tm);

			return NULL;
		}
		else
		{
			// We got some good server from a client
			LOG_TRACE("[Server] :=> Client<%d> Requests <%s> operation.", sender_fd, opcode_to_string(opcode));

			switch (opcode)
			{
			case MSG:
				dispatch_imprimir_mensaje((void *)recibir_mensaje(sender_fd));
				break;

			case SYS:
				dispatch_handle_syscall((void *)accion_recibir(sender_fd), &sender_pid);
				break;

			case CMD:
				dispatch_handle_instruction((void *)recibir_instructions(sender_fd), &sender_pid);
				break;

			default:
				LOG_ERROR("[Server] :=> Client<%d> sent an unrecognized operation code (%d)", sender_fd, opcode);
				break;
			}
		}
	}

	return NULL;
}
