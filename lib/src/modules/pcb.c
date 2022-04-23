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

pcb_t *new_pcb(uint32_t id, size_t size, uint32_t estimation)
{
	pcb_t *pcb = malloc(sizeof(pcb_t));

	pcb->id = id;
	pcb->size = size;
	pcb->instructions = list_create();
	pcb->page_table = NULL;
	pcb->estimation = estimation;
	pcb->pc = 0;

	return pcb;
}

void pcb_destroy(pcb_t *pcb)
{
	if (pcb)
	{
		list_smart_destroy(pcb->instructions, instruction_destroy);

		if (pcb->page_table)
			free(pcb->page_table);

		free(pcb);
		pcb = NULL;
	}
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
	 * ID | SIZE | ESTIMATION | PC | <List_Count> | Instructions...
	 * --------------------------------------------------------------
	 *
	 */
	memcpy(stream + offset, &pcb->id, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->size, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->estimation, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->pc, sizeof(uint32_t));
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
	memcpy(&pcb->size, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->estimation, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&pcb->pc, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	pcb->instructions = instruction_list_from(stream + offset);
	pcb->page_table = NULL;

	return pcb;
}

size_t pcb_bytes_size(pcb_t *pcb)
{
	// The PCB final size.
	size_t size = 0;
	// The size of [ID, SIZE, ESTIMATION, PC]
	size += sizeof(uint32_t) * 4;
	// Size of the List_SIZE value.
	size += sizeof(uint32_t);
	// The size of the instruction list.
	if (pcb->instructions)
		size += sizeof(instruction_t) * list_size(pcb->instructions);

	return size;
}
