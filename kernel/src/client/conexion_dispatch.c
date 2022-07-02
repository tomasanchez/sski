#include "conexion_dispatch.h"
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

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

static int on_connect_dispatch(kernel_t *kernel)
{
	char *port = puerto_cpu_dispatch();
	char *ip = ip_cpu();

	LOG_DEBUG("[Dispatch Thread] :=> Connecting <Kernel> at %s:%s", ip, port);
	// Test connection with cpu
	kernel->conexion_dispatch = conexion_cliente_create(ip, port);

	if (on_module_connect(&kernel->conexion_dispatch, false) EQ SUCCESS)
	{
		// Test connection with cpu
		LOG_DEBUG("[Dispatch Thread] :=> Connected to <CPU-DISPATCH> as CLIENT at %s:%s", ip, port);
	}

	return SUCCESS;
}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

// ! Main of [DISPATCH-THREAD]
void *routine_conexion_dispatch(void *data)
{
	on_connect_dispatch(data);
	return NULL;
}
