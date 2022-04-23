#include <commons/string.h>
#include <commons/collections/list.h>

#include "dispatcher.h"
#include "accion.h"
#include "instruction.h"
#include "thread_manager.h"
#include "server.h"
#include "smartlist.h"
#include "accion.h"
#include "log.h"
#include "lib.h"

// ============================================================================================================
//                               ***** Dispatcher -  Definiciones *****
// ============================================================================================================

// Los PIDs
bool pids[PIDS];

// ============================================================================================================
//                               ***** Funciones Privadas - Declaraciones *****
// ============================================================================================================

// ------------------------------------------------------------
//  Getters
// ------------------------------------------------------------

/**
 * @brief Get the primer pid libre
 *
 * @return el ID libre o UNDEFINED
 */
static uint32_t get_pid_libre(void);

/**
 * @brief Lee un un entero de 32 bits de una porción de memoria
 *
 * @param stream la porción de memoria de la cual leer
 * @param offset el desplazamiento dentro de esa memoria
 * @return el valor leido
 */
static uint32_t _get_uint32(void *stream, size_t *offset);

/**
 * @brief Lee un entero de tipo INT de una porción de memoria
 *
 * @param args la porción de memoria de la cual leer
 * @param offset el desplazamiento dentro de esa memoria
 * @return el valor leido
 */
static int _get_cliente(void *args, size_t *offset);

// ============================================================================================================
//                               ***** Funciones Privadas - Definiciones *****
// ============================================================================================================

void *dispatch_imprimir_mensaje(void *args)
{
	char *msg = ((char *)args);

	THREAD_SAFE(LOG_INFO("Mensaje: %s", msg));

	free(msg);

	return NULL;
}

void *dispatch_handle_instruction(void *args)
{
	instruction_t *instruction = ((instruction_t *)args);

	THREAD_SAFE(LOG_INFO("Received Instruction: %d %d %d", instruction->icode, instruction->param0, instruction->param1));

	instruction_destroy(instruction);

	return NULL;
}

void *dispatch_handle_action(void *args)
{
	accion_t *accion = ((accion_t *)args);

	THREAD_SAFE(LOG_INFO("Received Action: %d %d", accion->actioncode, accion->param););

	accion_destroy(accion);

	return NULL;
}

// ------------------------------------------------------------
//  Getters
// ------------------------------------------------------------

static uint32_t get_pid_libre(void)
{
	for (uint32_t i = 0; i < PIDS; i++)
	{
		if (!pids[i])
		{
			pids[i] = true;
			return i;
		}
	}

	return UNDEFINED;
}

static int _get_cliente(void *args, size_t *offset)
{
	int cliente = 0;

	memcpy((void *)&cliente, args + *offset, sizeof(int));
	*offset += sizeof(int);

	return cliente;
}

static uint32_t _get_uint32(void *stream, size_t *offset)
{
	uint32_t value = 0;

	memcpy((void *)&value, stream + *offset, sizeof(uint32_t));
	*offset += sizeof(uint32_t);

	return value;
}

// ----------------------
//  Wrappers
// ----------------------

// Obtiene la PATOTA ID
static uint32_t (*_get_pid)(void *, size_t *) = _get_uint32;
