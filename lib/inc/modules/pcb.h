/**
 * @file pcb.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-18-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <inttypes.h>
#include <stdlib.h>
#include "smartlist.h"

typedef enum Status
{
	NONE = -1,
	PCB_NEW,
	PCB_READY,
	PCB_EXECUTING,
	PCB_SUSPENDED_READY,
	PCB_BLOCKED,
	PCB_SUSPENDED_BLOCKED,
	PCB_TERMINATED
} pcb_status_t;

/**
 * @brief Process Control Block.
 *
 */
typedef struct PCB
{
	// The Process Identifier.
	uint32_t id;
	// The PCB status
	pcb_status_t status;
	// Final size in bytes of the process.
	size_t size;
	// List of instructions to be executed.
	t_list *instructions;
	// Reference to the table of pages.
	void *page_table;
	// CPU Burst estimation.
	uint32_t estimation;
	// Program Counter.
	uint32_t pc;
	// IO Burst.
	uint32_t io;
	// Real CPU usage
	uint32_t real;
} pcb_t;

/**
 * @brief Instantiates a new PCB with a list ready to be filled.
 *
 * @param id the process identifier
 * @param size the process final size
 * @param estimation the CPU burst estimation
 * @return a new pcb instance
 */
pcb_t *new_pcb(uint32_t id, size_t size, uint32_t estimation);

/**
 * @brief Deallocates memory used by a PCB.
 *
 * @param pcb direction to the pcb to be deleted
 */
void pcb_destroy(void *pcb);

/**
 * @brief Serializes a PCB.
 *
 * @param pcb the instance.
 * @return a stream containing the pcb data.
 */
void *pcb_to_stream(pcb_t *pcb);

/**
 * @brief Obtains the size in bytes of a PCB instance.
 *
 * @param pcb the instance.
 * @return the number of bytes in use.
 */
size_t pcb_bytes_size(pcb_t *pcb);

/**
 * @brief Recovers a PCB from a stream.
 *
 * @param stream to be deserialized
 * @return a recovered PCB instance
 */
pcb_t *pcb_from_stream(void *stream);

pcb_t *get_pcb_by_pid(t_list *pcbs, uint32_t pid);

/**
 * @brief Tells what PCB should go before the other.
 *
 * @param e1 a PCB instance
 * @param e2 another PCB instance
 * @return true if first pcb has less estimation than second one, false otherwise.
 */
bool pcb_sort_by_estimation(void *e1, void *e2);

/**
 * @brief Obtains the value of a param for a PCB
 *
 * @param pcb the pcb to evaluate
 * @param instruction the index of the instruction
 * @param param the number of the param to be obtained
 * @return uint32_t
 */
uint32_t pcb_get_param_for(pcb_t *pcb, int instruction, int param);
