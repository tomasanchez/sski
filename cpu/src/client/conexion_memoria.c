#include "conexion_memoria.h"
#include "cpu.h"
#include "instruction.h"
#include "conexion.h"
#include "accion.h"
#include "log.h"
#include "cfg.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern cpu_t g_cpu;

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

static int conexion_init(cpu_t* cpu)
{
	char *port = puerto_memoria();
	char *ip = ip_memoria();

	LOG_DEBUG("Connecting <Cpu> at %s:%s", ip, port);
	cpu->conexion = conexion_cliente_create(ip, port);

	// Test connection with Kernel
	//cpu->conexion = conexion_cliente_create("127.0.0.1", "8000");

	if (on_connect(&cpu->conexion, false) EQ SUCCESS)
	{
		LOG_DEBUG("Connected as CLIENT at %s:%s", ip, port);
		// Test connection with Kernel
		//LOG_DEBUG("Connected as CLIENT at %s:%s", "127.0.0.1", "8000");
	}

	return SUCCESS;
}


// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================


void  *routine_conexion_memoria(void *data)
{
	cpu_t *cpu = data;

	conexion_init(cpu);

	conexion_enviar_mensaje(cpu->conexion, "Mando un msj");

	for(;;){
		LOG_WARNING("Hola Thread 2");
		sleep(TIEMPO_ESPERA);
	}
}
