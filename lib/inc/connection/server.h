/**
 * servidor.h
 *
 * @file API de un Servidor, hereda de conexión
 * @author Tomás Sánchez
 * @since  04.17.2021
 */

#pragma once

#include "conexion.h"
#include <stdint.h>
#include <poll.h>
#include "thread_manager.h"

#define N_CLIENTES 1

// ============================================================================================================
//                               ***** Tipos (y estructuras) *****
// ============================================================================================================

/**
 * Estructura de un Servidor.
 *
 * @class
 * @implements Conexion
 * @public
 */
typedef struct Servidor
{
	// La conexión propia del server (socket, etc)
	conexion_t conexion;
	// Si inició o no
	bool iniciado;
	// Thread Tracker
	thread_manager_t tm;
} servidor_t;

// ============================================================================================================
//                                   ***** Funciones Públicas  *****
// ============================================================================================================

// -----------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

/**
 * Crea una conexión para un Server. Debe ser destruida
 *
 * @param ip el número de IP a conectarse
 * @param puerto el número de puerto a conectarse
 * @returns Una nueva estructura de servidor
 */
servidor_t servidor_create(char *ip, char *puerto);

/**
 * Libera la conexión creada para el Servidor.
 *
 * @param servidor la referencia a la estructura de conexion
 */
void servidor_destroy(servidor_t *servidor);

// ------------------------------------------------------------
//  Conexión en sí
// ------------------------------------------------------------

/**
 * Prepara al socket del Server para escuchar los clientes.
 *
 * @param servidor la referencia a la estructura de conexion
 * @returns Si pudo realizarse la escucha 1, sino -1.
 */
int servidor_escuchar(servidor_t *servidor);

/**
 * Manda al servidor a aceptar la conexion de 1 cliente.
 *
 * @param servidor la referencia estructura de conexión.
 * @returns una referencia al socket.
 */
void servidor_run(servidor_t *is_servidor, void *(*rutina)(void *));

/**
 * @brief Cierra la conexión del cliente especificado.
 *
 * @param socket el filedescriptor
 */
void servidor_desconectar_cliente(int socket);

// ------------------------------------------------------------
//  Mensajería
// ------------------------------------------------------------

/**
 * Recibe operaciones del cliente indicado
 *
 * @param socket el filedescriptor del cliente
 * @returns opcode
 */
int servidor_recibir_operacion(int socket);

/**
 * @brief Recibe un stream del cliente indicado
 *
 * @param socket el filedescriptor del cliente
 * @param bytes referencia a la cantidad de bytes recibidos
 * @return El stream recibido.
 */
void *servidor_recibir_stream(int socket, ssize_t *bytes);

/**
 * @brief Recibe un mensaje del cliente indicado
 *
 * @param socket el filedescriptor del cliente
 * @param bytes referencia a la cantidad de bytes recibidos
 * @return El mensaje recibido.
 */
char *servidor_recibir_mensaje(int socket, ssize_t *bytes);

/**
 * @brief Envía un mensaje al cliente seleccionado.
 *
 * @param socket el filedescriptor del cliente
 * @param msg  el mensaje a enviar.
 * @return los bytes envia
 */
ssize_t servidor_enviar_mensaje(int socket, char *msg);

/**
 * @brief Envía un stream al cliente seleccionado.
 *
 * @param opcode codigo de operacion del mensaje.
 * @param socket el filedescriptor del cliente
 * @param str  el stream a enviar.
 * @param str_size  tamaño del stream a enviar.
 * @return los bytes envia
 */
ssize_t servidor_enviar_stream(int opcode, int socket, void *iv_srt, ssize_t iv_str_size);

/**
 * @brief Recibe una accion del cliente indicado
 *
 * @param socket el filedescriptor del cliente
 * @return El parámetro de una acción
 */
uint32_t servidor_recibir_accion(int socket);

/**
 * @brief Envía una acción al cliente seleccionado
 *
 * @param socket el filedescriptor del cliente
 * @param accion la accion a enviar
 * @return los bytes enviados
 */
ssize_t servidor_enviar_accion(int socket, void *accion);
