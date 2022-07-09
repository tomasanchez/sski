/**
 * @file cpu.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-23-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "pcb.h"
#include "thread_manager.h"
#include "conexion.h"
#include "server.h"
#include "instruction.h"
#include "sync.h"
#include "tlb.h"

#define MODULE_NAME "cpu"
#define VALOR_INVALIDO UINT32_MAX

/**
 * @brief CPU Module.
 *
 */
typedef struct CPU
{
	// Server for Kernel - For PCB (Dispatch)
	servidor_t server_dispatch;
	// Server for Kernel - For Interruptions
	servidor_t server_interrupt;
	// Memory Connection
	conexion_t conexion;
	// CPU's Thread Launcher;
	thread_manager_t tm;
	// Current PCB in execution.
	pcb_t *pcb;
	// Thread Synchronizer
	cpu_sync_t sync;
	// Wether the CPU received an interrupt signal
	bool has_interruption;
	// Page Size
	uint32_t page_size;
	// Amount of entries per page
	uint32_t page_amount_entries;
	// Whether the CPU is executin a PCB or not
	bool is_executing;
	//TLB
	tlb_t *tlb;
} cpu_t;

int on_connect(void *conexion, bool offline_mode);
/**
 * @brief Inits a CPU module.
 *
 * @param cpu to be initialized
 */
int on_init(cpu_t *cpu);

/**
 * @brief Handles the main execution procedure.
 *
 * @param cpu to run
 * @return the exit status code
 */
int on_run(cpu_t *cpu);

/**
 * @brief Frees memory usage of a CPU module.
 *
 * @param cpu the module reference
 * @return an exit staus code
 */
int on_before_exit(cpu_t *cpu);

/**
 * @brief determina si la próxima etapa (Fetch Operands) es necesaria.
 *
 * @param instruction
 * @return bool
 */
bool decode(instruction_t *instruction);

/**
 * @brief Ejecuta la instrucción
 * @param param1 first parameter
 * @param param2 second parameter
 * @param data module connection
 */
uint32_t instruction_execute(instruction_t *instruction, void *data);

/**
 * @brief ejecuta instrucción NO_OP
 *
 */
void execute_NO_OP(uint time);

/**
 * @brief ejecuta instruccion IO
 *
 */
void execute_IO(instruction_t *instruction, cpu_t *cpu);

/**
 * @brief ejecuta instruccion EXIT
 *
 */
void execute_EXIT(instruction_t *instruction, cpu_t *cpu);

/*
 * @brief ejecuta instrucción READ
 *
 * @return uint32_t
 */
uint32_t execute_READ(uint32_t param1);

/**
 * @brief ejecuta instruccion WRITE
 *
 */
void execute_WRITE(uint32_t position, uint32_t value);

/**
 * @brief ejecuto la instruccion COPY
 * @return uint32_t
 */
uint32_t
execute_COPY(uint32_t param1, uint32_t param2);

/**
 * @brief (MMU) -> se obtiene la direccion fisica de un proceso, a través de la direccion lógica.
 *
 * @param pcb
 * @param logical_address
 * @return uint32_t
 */
uint32_t req_physical_address(cpu_t* cpu, uint32_t logical_address);

/**
 * @brief N°Pag = DL / Tam_de_Pagina
 *
 * @param direccion_logica
 * @return uint32_t
 */
uint32_t obtener_numero_pagina(uint32_t direccion_logica, uint32_t tamanio_pagina);

/**
 * @brief off = (DL - tam_pag) * N°Pag
 *
 * @param direccion_logica
 * @return uint32_t
 */
uint32_t obtener_offset(uint32_t direccion_logica, uint32_t tamanio_pagina);

/**
 * @brief entrada P1 = N°Pag / cant_entradas_por_pagina
 *
 * @param direccion_logica
 * @return uint32_t
 */
uint32_t obtener_entrada_primer_nivel(uint32_t direccion_logica, uint32_t tamanio_pagina, uint32_t cant_en_por_pag);

/**
 * @brief entrada P2 = N°Pag % cant_entradas_por_pagina
 *
 * @param direccion_logica
 * @return uint32_t
 */
uint32_t obtener_entrada_segundo_nivel(uint32_t direccion_logica, uint32_t tamanio_pagina, uint32_t cant_en_por_pag);

uint32_t obtener_tabla_segundo_nivel(uint32_t tabla_primer_nivel, uint32_t desplazamiento);

uint32_t obtener_frame(uint32_t tabla_segundo_nivel,uint32_t desplazamiento);
