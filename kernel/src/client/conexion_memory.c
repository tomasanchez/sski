#include "conexion_memory.h"
#include "kernel.h"
#include "instruction.h"
#include "conexion.h"
#include "module.h"
#include "accion.h"
#include "log.h"
#include "cfg.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern kernel_t g_kernel;

/**
 * @brief Connects a Kernel to a Memory
 *
 * @param kernel to connect
 * @return exit status code.
 */
static int on_connect_memory(kernel_t *kernel);

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

static int on_connect_memory(kernel_t *kernel)
{
	char *port = puerto_memoria();
	char *ip = ip_memoria();

	LOG_DEBUG("[Memory Thread] :=> Connecting <Kernel> at %s:%s", ip, port);

	kernel->conexion_memory = conexion_cliente_create(ip, port);

	// if (on_module_connect(&kernel->conexion_memory, false) EQ SUCCESS)
	// {
	// 	LOG_DEBUG("[Memory Thread] :=> Connected as CLIENT at %s:%s", ip, port);
	// }

	return SUCCESS;
}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

// ! Main of [Memory Thread]
void *routine_conexion_memoria(void *data)
{
	kernel_t *kernel = data;

	on_connect_memory(kernel);

	return NULL;
}
