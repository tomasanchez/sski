/**
 * @file conexion_memoria.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar> with Francisco Maver <fmaver@fbra.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-07-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "conexion_memoria.h"
#include "cpu.h"
#include "conexion.h"
#include "log.h"
#include "cfg.h"
#include "module.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern cpu_t g_cpu;

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

static int conexion_init(cpu_t *cpu)
{
	char *port = puerto_memoria();
	char *ip = ip_memoria();

	LOG_DEBUG("[CPU:Client-Memory] :=> Connecting <Cpu> at %s:%s", ip, port);

	cpu->conexion = conexion_cliente_create(ip, port);

	if (on_module_connect(&cpu->conexion, false) EQ SUCCESS)
	{
		LOG_DEBUG("[CPU:Client-Memory] :=> Connected at %s:%s", ip, port);
	}

	return SUCCESS;
}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

void *routine_conexion_memoria(void *data)
{
	cpu_t *cpu = data;

	conexion_init(cpu);

	conexion_enviar_mensaje(cpu->conexion, "Hello from CPU");

}
