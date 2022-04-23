/**
 * accion.c
 *
 * @file API de Comandos de Consola que se envían a los módulos
 * @author Tomás Sánchez
 * @since  05.04.2021
 */

#include "accion.h"
#include "conexion.h"
#include "server.h"
#include <stdlib.h>

// ============================================================================================================
//                                   ***** Funciones Privadas - Declaraciones  *****
// ============================================================================================================

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

accion_t *accion_from_stream(void *stream)
{
	actioncode_t _code = NO_ACTION_PARAMETER;
	uint32_t _param = 0u;

	size_t offset = 0lu;
	memcpy(&_code, stream, sizeof(actioncode_t));
	offset += sizeof(actioncode_t);
	memcpy(&_param, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return accion_create(_code, _param);
}

void *accion_serializar(const accion_t *accion)
{
	// El stream a enviarse
	void *stream = malloc(accion_size());

	// Variable Local offset - el offset del stream
	int lv_offset = 0;

	// Copio de a bytes, incrementando el offset en todos los casos
	/**
	 *          [ actioncode ][ PARAM ]
	 */
	memcpy(stream + lv_offset, &(accion->actioncode), sizeof(int));
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

accion_t *accion_create(actioncode_t actioncode, uint32_t param)
{
	accion_t *accion = malloc(sizeof(accion_t));

	accion->actioncode = actioncode;
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

accion_t *accion_recibir(int socket)
{
	ssize_t size = ERROR;
	void *stream = servidor_recibir_stream(socket, &size);
	accion_t *accion = accion_from_stream(stream);
	free(stream);

	return accion;
}
