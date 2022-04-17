/**
 * conexion.h
 *
 * @file API de conexiones.
 * @author Tomás Sánchez
 * @since  04.16.2021
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "opcode.h"

// ============================================================================================================
//                               ***** Tipos (y estructuras) *****
// ============================================================================================================

/**
 * Estructura de conexión.
 *
 * @class
 * @public
 */
typedef struct Connection
{
	// Información del Servidor
	struct addrinfo *info_server;
	// El número de socket instanciado
	int socket;
	// Si pudo conectarse
	bool conectado;
} conexion_t;

// ============================================================================================================
//                                   ***** Funciones Públicas  *****
// ============================================================================================================

// -----------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

/**
 * Crea una socket para un CLIENTE. Debe ser destruido.
 *
 * @param ip el número de IP a conectarse
 * @param puerto el número de puerto a conectarse
 * @returns Una nueva estructura de conexión
 */
conexion_t conexion_cliente_create(char *ip, char *puerto);

/**
 * Crea una socket para un SERVER. Debe ser destruida
 *
 * @param ip el número de IP a conectarse
 * @param puerto el número de puerto a conectarse
 * @returns Una nueva estructura de conexión
 */
conexion_t conexion_servidor_create(char *ip, char *puerto);

/**
 * Libera la conexión creada sirve para CLIENTE ó SERVIDOR.
 *
 * @param conexion la referencia a la estructura de conexion
 */
void conexion_destroy(conexion_t *);

// -----------------------------------------------------------
//  Conexión en sí
// ------------------------------------------------------------

/**
 * Realiza la conexion propiamente dicha de un CLIENTE. De conectarse, debe ser desconectada.
 *
 * @param conexion la referencia a la estructura de conexion
 * @returns Si pudo realizarse la conexion el file descriptor, sino -1.
 */
int conexion_conectar(conexion_t *);

/**
 * Prepara una conexion de un SERVER para escuchar los distintos clientes.
 *
 * @param conexion la referencia a la estructura de conexion
 * @returns Si pudo realizarse la escucha 1, sino -1.
 */
int conexion_escuchar(conexion_t *);

/**
 * Realiza la desconexion propiamente dicha de un CLIENTE.
 *
 * @param conexion la referencia a la estructura de conexion
 * @returns Si pudo realizarse la desconexion 1, sino -1.
 */
int conexion_desconectar(const conexion_t *);

/**
 * Permite a un SERVER aceptar a un cliente.
 *
 * @param conexion la estructura de conexión.
 * @returns el socket del cliente o -1 si error.
 */
int conexion_esperar_cliente(conexion_t);

/**
 * Consulta si hay una conexión.
 *
 * @param conexion la estructura de conexion a consultar
 * @returns Si hay conexión TRUE, sino FALSE.
 */
bool conexion_esta_conectada(conexion_t);

// -----------------------------------------------------------
//  Envío de Streams
// ------------------------------------------------------------

/**
 * Envía un mensaje a la conexión.
 *
 * @param conexion la estructura de conexion
 * @param mensaje el mensaje a enviar
 * @returns los bytes enviados o, si no pudo enviarse ERROR
 */
ssize_t conexion_enviar_mensaje(conexion_t, char *);

/**
 * @brief Envía un string al socket.
 *
 * @param string    el string propiamente dicho
 * @param socket    el socket a enviarlo
 * @return los bytes enviados o ERROR.
 */
ssize_t enviar_str(char *iv_str, int iv_socket);

/**
 * @brief Envía un stream al socket.
 *
 * @param opcode    el codigo de operacion
 * @param stream    el stream propiamente dicho
 * @param stream_size    tamaño del stream
 * @param socket    el socket a enviarlo
 * @return los bytes enviados o ERROR.
 */
ssize_t enviar_stream(opcode_t opcode, void *stream, size_t stream_size, int socket);

/**
 * @brief Envía un stream al socket.
 *
 * @param conexion la conexión a la cual enviar
 * @param opcode el código de operación
 * @param stream   el stream propiamente dicho
 * @param stream_size el tamaño del stream
 * @return los bytes enviados o ERROR.
 */
ssize_t conexion_enviar_stream(conexion_t is_conexion, opcode_t opcode, void *stream, size_t size);
