/**
 * servidor.c
 *
 * @file Definiciones relacionadas al encapsulamiento de un Servidor
 * @author Tomás Sánchez
 * @since  04.17.2021
 */

#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "server.h"
#include "network.h"
#include "accion.h"
#include "log.h"
#include "thread_manager.h"

// ============================================================================================================
//                               ***** Conexion -  Definiciones *****
// ============================================================================================================

// ============================================================================================================
//                               ***** Funciones Privadas - Declaraciones *****
// ============================================================================================================

/**
 * Recibe un opcode.
 *
 * @param socket el socket del cliente
 * @return el opcode o -1 si error
 */
static int recibir_operacion(int socket);

/**
 * Recibe un buffer.
 *
 * @param socket el socket del cliente
 * @param size el tamaño en bytes del buffer recibido
 * @return referencia a un buffer allocado
 */
static void *recibir_buffer(int socket, ssize_t *bytes);

// ============================================================================================================
//                               ***** Funciones Privadas - Definiciones *****
// ============================================================================================================

// -----------------------------------------------------------
//  Streams
// ------------------------------------------------------------

static int recibir_operacion(int socket)
{
	// Variable a Exportar Opcode - el número de operación.
	int opcode;
	// Valor de Retorno Bytes - Los bytes recibidos o -1 si error.
	ssize_t recv_ret;

	recv_ret = recv(socket, &opcode, sizeof(int), MSG_WAITALL);

	if (recv_ret <= 0)
		return recv_ret;

	else
		return opcode;
}

static void *recibir_buffer(int socket, ssize_t *bytes_size)
{
	/**
	 * Referencia a Exportar buffer - el buffer recibido;
	 */
	void *buffer_stream;

	// Variable local tamaño
	size_t size = 0;

	// Valor de Retorno bytes - Los bytes recibidos o ERROR
	ssize_t recv_ret = recv(socket, &size, sizeof(int), MSG_WAITALL);

	if (recv_ret EQ ERROR)
		return NULL;

	buffer_stream = malloc(size);

	recv_ret = recv(socket, buffer_stream, size, MSG_WAITALL);

	if (recv_ret EQ ERROR)
	{
		free(buffer_stream);
		return NULL;
	}

	*bytes_size = size;

	return buffer_stream;
}

// ============================================================================================================
//                               ***** Funciones Públicas, Definiciones *****
// ============================================================================================================

// -----------------------------------------------------------
//  Constructor / Destructor
// ------------------------------------------------------------

servidor_t servidor_create(char *ip, char *port)
{
	// Estructura a Exportar servidor - el nuevo servidor
	servidor_t server;

	server.conexion = conexion_servidor_create(ip, port);

	server.iniciado = false;

	server.tm = new_thread_manager();

	return server;
}

void servidor_destroy(servidor_t *server)
{
	if (server)
		conexion_destroy(&server->conexion);

	thread_manager_destroy(&server->tm);
}

// -----------------------------------------------------------
//  Conectividad
// ------------------------------------------------------------

inline int servidor_escuchar(servidor_t *server)
{
	return conexion_escuchar(&server->conexion);
}

void servidor_run(servidor_t *server, void *(*interceptor)(void *))
{
	errno = 0;
	// La remote address
	struct sockaddr_storage remoteaddr;
	// La longigutd de la address
	socklen_t addrlen = sizeof remoteaddr;

	// Acepto un nuevo cliente
	int fd = ERROR;

	fd = accept(server->conexion.socket, (struct sockaddr *)&remoteaddr, &addrlen);

	if (fd > 0)
	{
		char remoteIP[INET6_ADDRSTRLEN];
		int *newfd = malloc(sizeof(int));

		LOG_DEBUG("Nueva conexión de %s en el socket %d.",
				  inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr),
							remoteIP, INET6_ADDRSTRLEN),
				  fd);

		*newfd = fd;

		thread_manager_launch(&server->tm, interceptor, (void *)newfd);
	}
	else
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			LOG_WARNING("Un error ocurrió durante accept()");
	}

	return;
}

void servidor_desconectar_cliente(int socket)
{
	close(socket);
}

// ------------------------------------------------------------
//  Comunicación
// ------------------------------------------------------------

inline int servidor_recibir_operacion(int socket)
{
	return recibir_operacion(socket);
}

// ----------------------
//  Mensajes
// ----------------------

inline char *servidor_recibir_mensaje(int socket, ssize_t *bytes)
{
	return (char *)recibir_buffer(socket, bytes);
}

inline ssize_t servidor_enviar_mensaje(int socket, char *msg)
{
	return enviar_str(msg, socket);
}

inline void *servidor_recibir_stream(int socket, ssize_t *bytes)
{
	return recibir_buffer(socket, bytes);
}

inline ssize_t servidor_enviar_stream(int opcode, int socket, void *stream, ssize_t size)
{
	return enviar_stream(opcode, stream, size, socket);
}

// ----------------------
//  Acciones
// ----------------------

uint32_t servidor_recibir_accion(int socket)
{
	return accion_recibir(socket);
}

ssize_t servidor_enviar_accion(int socket, void *accion)
{
	return accion_enviar((accion_t *)accion, socket);
}
