#include "kernel.h"
#include "server.h"
#include "dispatcher.h"
#include "log.h"
#include "cfg.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

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
				LOG_WARNING("El cliente en el socket %d terminó la conexión.", sender_fd);
			}
			else
			{
				LOG_ERROR("Error detectado al utilizar recv() con el cliente %d.", sender_fd);
				LOG_DEBUG("Terminando conexión con el cliente %d.", sender_fd);
			}

			servidor_desconectar_cliente(sender_fd); // Bye!

			thread_manager_terminar_thread();

			return NULL;
		}
		else
		{
			// We got some good server from a client
			LOG_TRACE("Recibí el código de operación <%s> del cliente %d.", opcode_to_string(opcode), sender_fd);

			switch (opcode)
			{
			case MSG:
				dispatch_imprimir_mensaje((void *)recibir_mensaje(sender_fd));
				break;

			default:
				LOG_ERROR("Código de operación recibido del cliente %d es inválido: %d.", sender_fd, opcode);
				break;
			}
		}
	}

	return NULL;
}
