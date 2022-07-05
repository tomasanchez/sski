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

void handshake(cpu_t *cpu)
{

	//int tam_pagina();
	LOG_TRACE("[MMU] :=> Request page table size...");
	accion_t *req_page_size = accion_create(REQ_SIZE, 0); //modificar los parametros
	accion_enviar(req_page_size, cpu->conexion.socket);
	accion_t *recv_page_size = accion_recibir(cpu->conexion.socket);//modificar los parametros
	LOG_DEBUG("[MMU] :=> Page table size is: %d", recv_page_size->param);

	// int entradas_por_tabla(void);
	LOG_TRACE("[MMU] :=> Request amount_entries_per_page...");
	accion_t *req_amount_entries = accion_create(REQ_ENTRY, 0); //modificar los parametros
	accion_enviar(req_amount_entries, cpu->conexion.socket);
	accion_t *recv_amount_entries = accion_recibir(cpu->conexion.socket); //modificar los parametros
	LOG_DEBUG("[MMU] :=> Amount Entries per page: %d", recv_amount_entries->param);

	cpu->page_size = recv_page_size->param;
	cpu->page_amount_entries = recv_amount_entries->param;

	accion_destroy(req_page_size);
	accion_destroy(recv_page_size);
	accion_destroy(req_amount_entries);
	accion_destroy(recv_amount_entries);

}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

void *routine_conexion_memoria(void *data)
{
	cpu_t *cpu = data;

	conexion_init(cpu);

	handshake(cpu);

	return NULL;
}
