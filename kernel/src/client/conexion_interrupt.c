#include "conexion_interrupt.h"
#include "kernel.h"
#include "instruction.h"
#include "conexion.h"
#include "accion.h"
#include "log.h"
#include "cfg.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern context_t g_context;

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

int on_connect_interrupt(void *conexion, bool offline_mode)
{
	if (offline_mode)
	{
		LOG_WARNING("Module working in offline mode.");
		return ERROR;
	}

	while (!conexion_esta_conectada(*(conexion_t *)conexion))
	{
		LOG_TRACE("Connecting...");

		if (conexion_conectar((conexion_t *)conexion) EQ ERROR)
		{
			LOG_ERROR("Could not connect.");
			sleep(TIEMPO_ESPERA);
		}
	}

	return SUCCESS;
}

static int conexion_init(context_t *context)
{
	char *port = puerto_cpu_interrupt();
	char *ip = ip_cpu();

	LOG_DEBUG("Connecting <Cpu> at %s:%s", ip, port);

	// Test connection with cpu
	context->conexion_interrupt = conexion_cliente_create(ip, port);

	if (on_connect_interrupt(&context->conexion_interrupt, false) EQ SUCCESS)
	{

		// Test connection with cpu
		LOG_DEBUG("Connected as CLIENT at %s:%s", ip, port);
	}

	return SUCCESS;
}


// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================



void  *routine_conexion_interrupt(void *data)
{
	context_t *context = data;

	conexion_init(context);

	conexion_enviar_mensaje(context->conexion_interrupt, "Mando un msj");

	for(;;){
		LOG_WARNING("Hola Thread INTERRUPT");
		sleep(TIEMPO_ESPERA);
	}
}
