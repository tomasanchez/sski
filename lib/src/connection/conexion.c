/**
 * conexion.c
 *
 * @file Definiciones relacionadas al encapsulamiento de las conexiones
 * @author Tomás Sánchez
 * @since  04.16.2021
 */
#include <signal.h>
#include <netdb.h>
#include <fcntl.h>

#include "lib.h"
#include "conexion.h"
#include "buffer.h"
#include "package.h"

// ============================================================================================================
//                               ***** Conexion -  Definiciones *****
// ============================================================================================================

// ============================================================================================================
//                               ***** Funciones Privadas - Declaraciones *****
// ============================================================================================================

/**
 * Encapsula los funcionamientos de getaddrinfo
 *
 * @param ip el ip a conectarse
 * @param puerto el puerto de conexión
 * @param conexion la referencia de la conexion a direccionar
 */
static int _address(char *, char *, conexion_t *);

// ============================================================================================================
//                               ***** Funciones Privadas - Definiciones *****
// ============================================================================================================

// -----------------------------------------------------------
//  Misc
// ------------------------------------------------------------

int _address(char *ip, char *port, conexion_t *this)
{
	// Estructura local hints - Las hints para la creacion del socket.
	struct addrinfo hints;
	int rv = -1;

	// Seteos de memoria a las hints -- No importa como funciona
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(ip, port, &hints, &this->info_server)) != 0)
	{
		fprintf(stderr, "%s\n", gai_strerror(rv));
		return ERROR;
	}

	return SUCCESS;
}

// -----------------------------------------------------------
//  Streams
// ------------------------------------------------------------

bool tiene_mensaje(char *msg)
{
	return msg != NULL && strlen(msg) > 0;
}

ssize_t enviar_str(char *str, int socket)
{
	// Estructura Local paquete - el paquete a enviar
	package_t *package = package_create(MSG, str);

	// Local stream - el paquete serializado, requiere free(1)
	void *stream = package_serialize(package);

	// Variable a Exportar bytes - Los bytes enviados o ERROR (-1)
	ssize_t bytes_sent = send(socket, stream, package_get_real_size(package), 0);

	free(stream);

	package_destroy(package);

	return bytes_sent;
}

ssize_t enviar_stream(opcode_t opcode, void *str, size_t size, int socket)
{
	// Estructura Local paquete - el paquete a enviar
	package_t *package = new_package_for(opcode, size, str);

	// Local stream - el paquete serializado, requiere free(1)
	void *stream = package_serialize(package);

	// Variable a Exportar bytes - Los bytes enviados o ERROR (-1)
	ssize_t bytes_sent = send(socket, stream, package_get_real_size(package), 0);

	free(stream);

	package_destroy(package);

	return bytes_sent;
}

// ============================================================================================================
//                               ***** Funciones Públicas, Definiciones *****
// ============================================================================================================

// ------------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

conexion_t conexion_cliente_create(char *ip, char *port)
{
	// Estructura a Exportar conexion - La nueva conexión
	conexion_t that;

	_address(ip, port, &that);

	// Creo el socket y la conexion
	that.socket = socket(that.info_server->ai_family, that.info_server->ai_socktype, that.info_server->ai_protocol);
	that.conectado = false;

	return that;
}

conexion_t conexion_servidor_create(char *ip, char *port)
{

	// Estructura a Exportar conexion - La nueva conexión
	conexion_t that = {NULL, 0, false};
	int yes = 1;

	if (_address(ip, port, &that) == ERROR)
	{
		return that;
	}

	// Estructura local info - iterador de la lista de ardrinfo.
	struct addrinfo *_info;

	for (_info = that.info_server; _info != NULL; _info = _info->ai_next)
	{
		// Busco el socket que puede abirse
		that.socket = socket(_info->ai_family, _info->ai_socktype, _info->ai_protocol);
		if (that.socket < 0)
		{
			continue;
		}

		// Lose the pesky "address already in use" error message
		setsockopt(that.socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		// Avoids a blocking accept state (ADDS BUSY WAITING)
		// fcntl(es_conexion.socket, F_SETFL, O_NONBLOCK);

		// Corroboro que además de abrirse, el socket puede bindears
		if (bind(that.socket, _info->ai_addr, _info->ai_addrlen) EQ ERROR)
		{
			// Si no bindea, cierro el socket
			close(that.socket);
			continue;
		}

		// Si todo salio OK simplemente rompo el LOOP.
		break;
	}

	// Para el SERVER, considero conectado cuando se encuentra escuchando (listen)
	that.conectado = false;

	return that;
}

inline void conexion_destroy(conexion_t *this)
{
	conexion_desconectar(this);
	// Libero la información del server
	freeaddrinfo(this->info_server);
}

// ------------------------------------------------------------
//  Conexión Intrínseca
// ------------------------------------------------------------

inline int conexion_conectar(conexion_t *this)
{
	// Conecto, y guardo si se pudo conectar
	this->conectado =
		connect(this->socket, this->info_server->ai_addr, this->info_server->ai_addrlen) != ERROR;

	// Devuelvo si tuvo exito o error
	return this->conectado ? this->socket : ERROR;
}

inline int conexion_escuchar(conexion_t *this)
{
	// Escucho, y guardo el resultado de si se pudo abrir la escucha

	this->conectado = listen(this->socket, SOMAXCONN) != ERROR;

	return this->conectado ? SUCCESS : ERROR;
}

inline int conexion_desconectar(const conexion_t *this)
{
	return close(this->socket);
}

int conexion_esperar_cliente(conexion_t this)
{
	// Estructura Local direccion de cliente - La informacion de direccion del cliente.
	struct sockaddr_in client_addr;
	// Variable local tamaño de dirección
	unsigned int addr_size = sizeof(struct sockaddr_in);

	return accept(this.socket, (void *)&client_addr, &addr_size);
}

inline bool conexion_esta_conectada(conexion_t this)
{
	return this.conectado;
}

// ------------------------------------------------------------
//  Comunicación
// ------------------------------------------------------------

// --------------
//  Mensajes
// --------------

inline ssize_t conexion_enviar_mensaje(conexion_t this, char *msg)
{
	return conexion_esta_conectada(this) ? enviar_str(msg, this.socket) : ERROR;
}

// --------------
//  Mensajes
// --------------
inline ssize_t conexion_enviar_stream(conexion_t this, opcode_t opcode, void *strean, size_t size)
{
	return conexion_esta_conectada(this) ? enviar_stream(opcode, strean, size, this.socket) : ERROR;
}

void *conexion_recibir_stream(int socket, ssize_t *bytes_size)
{
	/**
	 * Referencia a Exportar buffer - el buffer recibido;
	 */
	void *buffer_stream;

	// Variable local tamaño
	size_t opcode = 0;

	// Valor de Retorno bytes - Los bytes recibidos o ERROR
	ssize_t recv_ret = recv(socket, &opcode, sizeof(int), MSG_WAITALL);

	if (recv_ret EQ ERROR)
		return NULL;

	size_t size = 0;

	recv_ret = recv(socket, &size, sizeof(int), MSG_WAITALL);

	if (size == 0 || recv_ret <= 0)
		return NULL;

	buffer_stream = malloc(size);

	recv_ret = recv(socket, buffer_stream, size, MSG_WAITALL);

	if (recv_ret EQ ERROR)
	{
		free(buffer_stream);
		return NULL;
	}

	*bytes_size = size + 2 * sizeof(int);

	return buffer_stream;
}
