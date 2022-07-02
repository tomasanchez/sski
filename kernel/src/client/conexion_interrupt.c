#include "conexion_interrupt.h"
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

static int on_connect_interrupt(kernel_t *kernel)
{
	char *port = puerto_cpu_interrupt();
	char *ip = ip_cpu();

	LOG_DEBUG("[Interrupt Thread] :=> Connection of <Kernel> created at %s:%s", ip, port);

	// Test connection with cpu
	kernel->conexion_interrupt = conexion_cliente_create(ip, port);

	if (on_module_connect(&kernel->conexion_interrupt, false) EQ SUCCESS)
	{

		// Test connection with cpu
		LOG_DEBUG("[Interrupt Thread] :=> Connected to <CPU> as CLIENT at %s:%s", ip, port);
	}

	return SUCCESS;
}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================
// ! Main of [Interrupt Thread]
void *routine_conexion_interrupt(void *data)
{
	on_connect_interrupt(data);
	return NULL;
}
