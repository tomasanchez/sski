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

/**
 * @brief Process Control Block.
 *
 */
typedef struct PCB
{
	// The Process Identifier.
	uint32_t id;
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
