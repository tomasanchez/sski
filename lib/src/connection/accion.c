/**
 * accion.c
 *
 * @file API de Comandos de Consola que se envían a los módulos
 * @author Tomás Sánchez
 * @since  05.04.2021
 */

#include "accion.h"
#include "conexion.h"
#include <stdlib.h>

// ============================================================================================================
//                                   ***** Funciones Privadas - Declaraciones  *****
// ============================================================================================================

/**
 * @brief Serializa una Accion
 *
 * @param accion la accion a serializar
 * @return Un stream serializado
 */
void *accion_serializar(const accion_t *accion);

/**
 * @brief Atajo de cáculo del tamaño fijo de una accion
 *
 * @return el verdadero sizeOf accion_t
 */
static ssize_t accion_size();
// ============================================================================================================
//                                   ***** Funciones Privadas - Definiciones  *****
// ============================================================================================================

static inline ssize_t accion_size()
{
	return sizeof(int) + sizeof(uint32_t);
}

void *accion_serializar(const accion_t *accion)
{
	// El stream a enviarse
	void *stream = malloc(accion_size());

	// Variable Local offset - el offset del stream
	int lv_offset = 0;

	// Copio de a bytes, incrementando el offset en todos los casos
	/**
	 *          [ OPCODE ][ PARAM ]
	 */
	memcpy(stream + lv_offset, &(accion->opcode), sizeof(int));
	lv_offset += sizeof(int);
	memcpy(stream + lv_offset, &(accion->param), sizeof(uint32_t));
	lv_offset += sizeof(uint32_t);

	return stream;
}

// ============================================================================================================
//                                   ***** Funciones Públicas  *****
// ============================================================================================================

// ------------------------------------------------------------
//  Constructor y Destructor
// ------------------------------------------------------------

accion_t *accion_create(opcode_t opcode, uint32_t param)
{
	accion_t *accion = malloc(sizeof(accion_t));

	accion->opcode = opcode;
	accion->param = param;

	return accion;
}

inline void accion_destroy(accion_t *accion)
{
	if (accion)
		free(accion);
}

// ------------------------------------------------------------
//  Envíos
// ------------------------------------------------------------

ssize_t accion_enviar(accion_t *accion, int socket)
{
	// Los bytes enviados
	ssize_t bytes;
	// La accion serializada
	void *stream = accion_serializar(accion);

	if (stream)
	{
		bytes = send(socket, stream, accion_size(), 0);
		free(stream);
	}

	return bytes;
}

uint32_t accion_recibir(int socket)
{
	// Variable a Exportar param - el valor del parámetreo de una accion.
	uint32_t param = 0;

	// Valor de Retorno Bytes - Los bytes recibidos o -1 si error.
	ssize_t rv_bytes;

	rv_bytes = recv(socket, &param, sizeof(uint32_t), MSG_WAITALL);

	return rv_bytes <= 0 ? 0 : param;
}

accion_t *accion_recibir_full(int socket)
{
	// Variable a Exportar accion - la accion recibida.
	accion_t *accion = malloc(sizeof(accion_t));

	recv(socket, accion, accion_size(), MSG_WAITALL);

	return accion;
}
