/**
 * @file pcb.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-18-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "pcb.h"
#include "instruction.h"
#include <string.h>
#include "safe_queue.h"
#include "smartlist.h"

pcb_t *new_pcb(uint32_t id, size_t size, uint32_t estimation)
{
	pcb_t *pcb = malloc(sizeof(pcb_t));
	pcb->id = id;					   // Identificador del proceso
	pcb->status = NONE;				   // Estado del PCB.
	pcb->size = size;				   // Tamaño en bytes del proceso, el mismo no cambiará a lo largo de la ejecución
	pcb->instructions = list_create(); // Lista de instrucciones a ejecutar
	pcb->page_table = UINT32_MAX;	   // Tabla de páginas del proceso en memoria, esta información la tendremos recién cuando el proceso pase a estado READY
	pcb->estimation = estimation;	   // Estimación utilizada para planificar los procesos en el algoritmo SRT, la misma tendrá un valor inicial definido por archivo de configuración y será recalculada bajo la fórmula de promedio ponderado
	pcb->pc = 0;					   // Número de la próxima instrucción a ejecutar
	pcb->io = 0;					   // Tiempo de espera en el sistema de IO
	pcb->real = 0;					   // Tiempo real de ejecución
	return pcb;
}

static inline bool _get_by_pid(uint32_t pid, pcb_t *pcb)
{
	return pcb->id == pid;
}

pcb_t *get_pcb_by_pid(t_list *pcbs, uint32_t pid)
{
	bool clojure(void *pcb) { return _get_by_pid(pid, (pcb_t *)pcb); }

	return list_find(pcbs, clojure);
}

pcb_t *pcb_by_id(safe_queue_t *queue, uint32_t id)
{
	bool id_criteria(void *pcb) { return _get_by_pid(id, (pcb_t *)pcb); }

	return safe_queue_find_by(queue, id_criteria);
}

bool pcb_exists(safe_queue_t *queue, uint32_t id)
{
	bool id_criteria(void *pcb) { return _get_by_pid(id, (pcb_t *)pcb); }

	return safe_queue_any(queue, id_criteria);
}

pcb_t *pcb_remove_by_id(safe_queue_t *queue, uint32_t id)
{
	bool id_criteria(void *pcb) { return _get_by_pid(id, (pcb_t *)pcb); }

	return safe_queue_remove_by(queue, id_criteria);
}

bool pcb_sort_by_estimation(void *e1, void *e2)
{
	return ((pcb_t *)e1)->estimation <= ((pcb_t *)e2)->estimation;
}

uint32_t pcb_get_param_for(pcb_t *pcb, int instruction, int param)
{
	instruction_t *i = list_get(pcb->instructions, instruction);
	return param == 0 ? i->param0 : i->param1;
}

void pcb_destroy(void *pcb)
{
	if ((pcb_t *)pcb)
	{
		list_smart_destroy(((pcb_t *)pcb)->instructions, instruction_destroy);

		free(pcb);
	}

	pcb = NULL;
}

void *pcb_to_stream(pcb_t *pcb)
{
	// The size of the pcb to be sent
	size_t pcb_size = pcb_bytes_size(pcb);

	// The stream to serialize.
	void *stream = malloc(pcb_size);
	// The pointer offset.
	size_t offset = 0;

	/**
	 * @brief The serialized stream will be:
	 *
	 * --------------------------------------------------------------
	 * ID | STATUS | SIZE | ESTIMATION | PC | LVL1 - Table | <List_Count> | Instructions...
	 * --------------------------------------------------------------
	 *
	 */
	memcpy(stream + offset, &pcb->id, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->status, sizeof(pcb->status));
	offset += sizeof(pcb->status);
	memcpy(stream + offset, &pcb->size, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->estimation, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->pc, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->page_table, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	int list_count = list_size(pcb->instructions);
	memcpy(stream + offset, &list_count, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	// When list is empty, just return the stream.
	if (list_count == 0)
		return stream;

	// Otherwise serialize the instructions: Convert the instructions to stream.
	t_list *streams_list = list_map(pcb->instructions, instruction_to_stream);

	// Copy them to the stream_buffer.
	for (int i = 0; i < list_count; i++)
	{
		instruction_t *instruction = list_get(streams_list, i);

		memcpy(stream + offset, instruction, sizeof(instruction_t));
		offset += sizeof(instruction_t);

		instruction = NULL;
	}

	// Destroy the stream_list.
	list_smart_fast_destroy(streams_list);

	return stream;
}

pcb_t *pcb_from_stream(void *stream)
{
	pcb_t *pcb = malloc(sizeof(pcb_t));

	size_t offset = 0;
	memcpy(&pcb->id, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->status, stream + offset, sizeof(pcb->status));
	offset += sizeof(pcb->status);
	memcpy(&pcb->size, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->estimation, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->pc, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->page_table, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	pcb->instructions = instruction_list_from(stream + offset);

	return pcb;
}

size_t pcb_bytes_size(pcb_t *pcb)
{
	// The PCB final size.
	size_t size = 0;
	// The size of [ID, SIZE, ESTIMATION, PC, LVL1 PT ID]
	size += sizeof(uint32_t) * 5;
	size += sizeof(pcb->status);
	// Size of the List_SIZE value.
	size += sizeof(uint32_t);
	// The size of the instruction list.
	if (pcb->instructions)
		size += sizeof(instruction_t) * list_size(pcb->instructions);

	return size;
}
