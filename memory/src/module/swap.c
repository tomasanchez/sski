/**
 * @file swap.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-22-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "swap.h"
#include "pcb.h"
#include "os_memory.h"
#include "fs.h"
#include "cfg.h"
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

extern memory_t g_memory;

// ============================================================================================================
//                               ***** Declarations *****
// ============================================================================================================

void swap_frame(uint32_t frame, void *file_address, uint32_t *offset);

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

void swap_frame_for_pcb(uint32_t pid, uint32_t frame)
{
	swap_data_t *swap_data = get_swap_data_for_pcb(&g_memory, pid);

	if (swap_data == NULL)
	{
		LOG_ERROR("[SWAP] No swap data for pid %d", pid);
		return;
	}

	LOG_DEBUG("[SWAP] Obtained SwapData{PID:%d, OFFSET: %d, SIZE: %d}", swap_data->pid, swap_data->offset, swap_data->size);

	int fd = open_file(pid);

	if (fd == -1)
	{
		LOG_ERROR("[SWAP] Couldn't open file for pid %d", pid);
		return;
	}

	off_t swap_size = (off_t)swap_data->size;

	ftruncate(fd, swap_size);

	void *file_address = mmap(NULL, swap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (file_address == MAP_FAILED)
	{
		LOG_ERROR("[SWAP] :=> Couldn't MMAP file: %s", strerror(errno));
		return;
	}

	swap_frame(frame, file_address, &swap_data->offset);
	msync(file_address, swap_size, MS_SYNC);
	munmap(file_address, swap_size);
	close(fd);
	page_table_lvl_2_t *frame_ref = get_frame_ref(&g_memory, frame);
	frame_ref->present = false;
	usleep(retardo_swap() * 1000);
}

void swap_pcb(void *pcb_ref)
{

	pcb_t *pcb = pcb_from_stream(pcb_ref);
	free(pcb_ref);

	// Fail fast
	if (pcb == NULL)
	{
		LOG_ERROR("[SWAP] :=> Could not retrieve PCB from stream");
		return;
	}

	int fd = open_file(pcb->id);

	if (fd == -1)
	{
		LOG_ERROR("[SWAP] :=> Couldn't open SWAP file for PCB#%d", pcb->id);
		return;
	}

	off_t swap_size = (off_t)pcb->size > get_frames_used_size(&g_memory, pcb->page_table) ? pcb->size : get_frames_used_size(&g_memory, pcb->page_table);

	ftruncate(fd, swap_size);

	void *file_address = mmap(NULL, swap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (file_address == MAP_FAILED)
	{
		LOG_ERROR("[SWAP] :=> Couldn't MMAP file: %s", strerror(errno));
		return;
	}

	LOG_INFO("[SWAP] :=> Mapping into <%p> [%ld bytes]", file_address, swap_size);
	LOG_WARNING("[SWAP] :=> Memcpying data related to PCB #%d", pcb->id);

	uint32_t offset = get_offset_for_pcb(&g_memory, pcb->id);

	page_table_lvl_2_t **big_table = create_big_table(&g_memory, pcb->page_table);

	for (uint32_t i = 0; i < g_memory.max_rows * g_memory.max_rows; i++)
	{
		if ((big_table[i])->frame != INVALID_FRAME && (big_table[i])->present)
			swap_frame((big_table[i])->frame, file_address, &offset);
	}

	msync(file_address, swap_size, MS_SYNC);
	munmap(file_address, swap_size);
	swap_data_t *data = get_swap_data_for_pcb(&g_memory, pcb->id);
	data->size = swap_size;
	LOG_INFO("[SWAP] :=> PCB #%d was swapped [%ld bytes]", pcb->id, swap_size);
	free(big_table);
	close(fd);
	pcb_destroy(pcb);
	usleep(retardo_swap() * 1000);
}

swap_data_t *new_swap_data(uint32_t pid, uint32_t size)
{
	swap_data_t *swap_data = malloc(sizeof(swap_data_t));
	swap_data->pid = pid;
	swap_data->size = size;
	swap_data->offset = 0;
	return swap_data;
}

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

void swap_frame(uint32_t frame, void *file_address, uint32_t *offset)
{

	size_t frame_size = tam_pagina();
	size_t swapped_frame_size = sizeof(uint32_t) + frame_size;

	void *frame_stream = malloc(swapped_frame_size);
	memcpy(frame_stream, &frame, sizeof(uint32_t));
	memcpy(frame_stream + sizeof(uint32_t), get_frame_address(&g_memory, frame), frame_size);
	memcpy(file_address + *offset, frame_stream, swapped_frame_size);
	*offset += swapped_frame_size;
	free(frame_stream);
}

void unswap_frame_for_pcb(uint32_t pid, uint32_t frame)
{

	int fd = open_file(pid);

	if (fd == -1)
	{
		LOG_ERROR("[SWAP] :=> Couldn't open file for pid %d", pid);
		return;
	}

	swap_data_t *swap_data = get_swap_data_for_pcb(&g_memory, pid);

	void *file_address = mmap(NULL, swap_data->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (file_address == MAP_FAILED)
	{
		LOG_ERROR("[SWAP] :=> Couldn't MMAP file: %s", strerror(errno));
		return;
	}

	size_t page_size = tam_pagina();
	size_t swap_entry = page_size + sizeof(uint32_t);

	size_t replace_index = 0;
	for (size_t i = 0; i < swap_data->offset; i += swap_entry)
	{
		uint32_t frame_swapped = UINT32_MAX;
		memcpy(&frame_swapped, file_address + i, sizeof(uint32_t));

		if (frame_swapped == frame)
		{
			void *frame_data = malloc(page_size);
			memcpy(frame_data, file_address + i + sizeof(uint32_t), sizeof(page_size));
			memcpy(g_memory.main_memory + (page_size * frame), frame_data, page_size);
			free(frame_data);
			replace_index = i;
			break;
		}
	}

	if (swap_data->offset > page_size)
	{
		memcpy(file_address + replace_index, file_address + swap_data->offset - page_size, page_size);
		swap_data->offset -= swap_entry;
	}
	else
	{
		swap_data->offset = 0;
	}

	msync(file_address, swap_data->size, MS_SYNC);
	munmap(file_address, swap_data->size);
	close(fd);
	page_table_lvl_2_t *frame_ref = get_frame_ref(&g_memory, frame);
	frame_ref->present = true;
	usleep(retardo_swap() * 1000);
}

void unswap_pcb(uint32_t pid)
{
	swap_data_t *swap_data = get_swap_data_for_pcb(&g_memory, pid);

	int fd = open_file(pid);

	if (fd == -1)
	{
		LOG_ERROR("[SWAP] :=> Couldn't open file for pid %d", pid);
		return;
	}

	void *file_address = mmap(NULL, swap_data->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (file_address == MAP_FAILED)
	{
		LOG_ERROR("[SWAP] :=> Couldn't MMAP file: %s", strerror(errno));
		return;
	}

	size_t page_size = tam_pagina();
	size_t swap_entry = page_size + sizeof(uint32_t);

	for (size_t i = 0; i < swap_data->offset; i += swap_entry)
	{

		uint32_t frame_swapped = UINT32_MAX;
		memcpy(&frame_swapped, file_address + i, sizeof(uint32_t));

		page_table_lvl_2_t *frame_ref = get_frame_ref(&g_memory, frame_swapped);

		if (frame_ref->present)
		{
			void *frame_data = malloc(page_size);
			memcpy(frame_data, file_address + i + sizeof(uint32_t), sizeof(page_size));
			memcpy(g_memory.main_memory + (page_size * frame_ref->frame), frame_data, page_size);
			free(frame_data);
		}
		else
		{
			swap_data->offset = i + swap_entry;
		}
	}

	msync(file_address, swap_data->size, MS_SYNC);
	munmap(file_address, swap_data->size);
	close(fd);
	usleep(retardo_swap() * 1000);
}
