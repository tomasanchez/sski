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

static int recibir_operacion(int iv_socket)
{
    // Variable a Exportar Opcode - el número de operación.
    int ev_opcode;
    // Valor de Retorno Bytes - Los bytes recibidos o -1 si error.
    ssize_t rv_bytes;

    rv_bytes = recv(iv_socket, &ev_opcode, sizeof(int), MSG_WAITALL);

    if (rv_bytes <= 0)
        return rv_bytes;

    else
        return ev_opcode;
}

static void *recibir_buffer(int iv_socket, ssize_t *iv_size)
{
    /**
     * Referencia a Exportar buffer - el buffer recibido;
     */
    void *er_buffer;

    // Variable local tamaño
    size_t lv_size = 0;

    // Valor de Retorno bytes - Los bytes recibidos o ERROR
    ssize_t rv_bytes = recv(iv_socket, &lv_size, sizeof(int), MSG_WAITALL);

    if (rv_bytes EQ ERROR)
        return NULL;

    er_buffer = malloc(lv_size);

    rv_bytes = recv(iv_socket, er_buffer, lv_size, MSG_WAITALL);

    if (rv_bytes EQ ERROR)
    {
        free(er_buffer);
        return NULL;
    }

    *iv_size = lv_size;

    return er_buffer;
}

// ============================================================================================================
//                               ***** Funciones Públicas, Definiciones *****
// ============================================================================================================

// -----------------------------------------------------------
//  Constructor / Destructor
// ------------------------------------------------------------

servidor_t servidor_create(char *iv_ip, char *iv_puerto)
{
    // Estructura a Exportar servidor - el nuevo servidor
    servidor_t es_servidor;

    es_servidor.conexion = conexion_servidor_create(iv_ip, iv_puerto);

    es_servidor.iniciado = false;

    return es_servidor;
}

void servidor_destroy(servidor_t *is_servidor)
{
    if (is_servidor)
        conexion_destroy(&is_servidor->conexion);
}

// -----------------------------------------------------------
//  Conectividad
// ------------------------------------------------------------

inline int servidor_escuchar(servidor_t *is_servidor)
{
    return conexion_escuchar(&is_servidor->conexion);
}

void servidor_run(servidor_t *is_servidor, void *(*rutina)(void *))
{
    errno = 0;
    // La remote address
    struct sockaddr_storage remoteaddr;
    // La longigutd de la address
    socklen_t addrlen = sizeof remoteaddr;

    // Acepto un nuevo cliente
    int fd = ERROR;

    fd = accept(is_servidor->conexion.socket, (struct sockaddr *)&remoteaddr, &addrlen);

    if (fd > 0)
    {
        char remoteIP[INET6_ADDRSTRLEN];
        int *newfd = malloc(sizeof(int));

        LOG_DEBUG("Nueva conexión de %s en el socket %d.",
                  inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr),
                            remoteIP, INET6_ADDRSTRLEN),
                  fd);

        *newfd = fd;

        thread_manager_lanzar(rutina, (void *)newfd);
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

inline ssize_t servidor_enviar_mensaje(int socket, char *iv_msg)
{
    return enviar_str(iv_msg, socket);
}

inline void *servidor_recibir_stream(int socket, ssize_t *bytes)
{
    return recibir_buffer(socket, bytes);
}

inline ssize_t servidor_enviar_stream(int opcode, int socket, void *iv_str, ssize_t iv_str_size)
{
    return enviar_stream(opcode, iv_str, iv_str_size, socket);
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