/**
 * @file kernel_controller.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-19-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include "kernel_controller.h"
#include "server.h"
#include "pcb.h"
#include "cfg.h"
#include "log.h"
#include "os_memory.h"

// ============================================================================================================
//                                   ***** Declarations *****
// ============================================================================================================

extern memory_t g_memory;

/**
 * @brief Swaps a PCB into the memory
 *
 * @param pcb_stream to swap
 * @return memory position
 */
uint32_t
swap_pcb(void *pcb_stream);

pcb_t *
retrieve_swapped_pcb(uint32_t pcb_id);

/**
 * @brief Deletes swapped file
 *
 * @param pcb_id to identfy the swapped_partition
 */
void delete_swapped_pcb(uint32_t pcb_id);

/**
 * @brief Get a free page table index
 *
 * @return the page table index
 */
uint32_t
get_page_table(void);

// ============================================================================================================
//                                   ***** Public Functions *****
// ============================================================================================================

void kernel_controller_swap(int socket)
{
	ssize_t bytes_received = -1;

	void *pcb_stream = servidor_recibir_stream(socket, &bytes_received);
	LOG_TRACE("[Server] :=> A PCB was received to be swapped");

	uint32_t swap_status = swap_pcb(pcb_stream);
	if (swap_status == SUCCESS)
	{
		LOG_TRACE("[Server] :=> PCB was SUCCESSSFULLY swapped");
	}
	else
	{
		LOG_TRACE("[Server] :=> Failed to swap PCB");
	}

	free(pcb_stream);
}

void kernel_controller_read_swap(int socket) 
{
	ssize_t bytes_received = -1;

	uint32_t * pcb_id = (uint32_t *) servidor_recibir_stream(socket, &bytes_received);
	LOG_TRACE("[Server] :=> A PCB ID #%d was received", *pcb_id);

	pcb_t *swapped_pcb = retrieve_swapped_pcb(*pcb_id);

	if(swapped_pcb != NULL) {

		LOG_INFO("[Server] :=> PCB <%d> retrieved", *pcb_id);

		void *swapped_pcb_stream = pcb_to_stream(swapped_pcb);

		ssize_t bytes_sent = servidor_enviar_stream(RETRIEVED_PCB, socket, &swapped_pcb_stream, pcb_bytes_size(swapped_pcb));

		if (bytes_sent <= 0)
		{
			LOG_ERROR("[Server] :=> PCB could not be sent");
		}
		else
		{
			LOG_DEBUG("[Server] :=> PCB <%d> was sent [%ld bytes]", *pcb_id, bytes_sent);
		}

		free(swapped_pcb);

		free(swapped_pcb_stream);
	}

	free(pcb_id);
}

void kernel_controller_delete_swap_file(int socket)
{
	ssize_t bytes_received = -1;

	uint32_t *pcb_id = (uint32_t *)servidor_recibir_stream(socket, &bytes_received);
	LOG_TRACE("[Server] :=> A PCB ID #%d was received", *pcb_id);

	delete_swapped_pcb(*pcb_id);

	free(pcb_id);
}

void kernel_controller_memory_init(int socket)
{
	ssize_t bytes_received = -1;
	void *pid_ref = servidor_recibir_stream(socket, &bytes_received);

	if (bytes_received <= 0)
	{
		LOG_ERROR("[Server] :=> Could not receive PID");
		return;
	}

	uint32_t pid = *(uint32_t *)pid_ref;
	free(pid_ref);

	LOG_DEBUG("[Server] :=> Initializing PCB #%d", pid);

	LOG_TRACE("[Server] :=> Obtaining available page table");

	uint32_t page_table = get_page_table();

	LOG_INFO("[Server] :=> Page Table <%d> was obtained", page_table);

	ssize_t bytes_sent = servidor_enviar_stream(MEMORY_INIT, socket, &page_table, sizeof(page_table));

	if (bytes_sent <= 0)
	{
		LOG_ERROR("[Server] :=> Page table could not be sent");
	}
	else
	{
		LOG_DEBUG("[Server] :=> Page table <%d> was sent [%ld bytes]", page_table, bytes_sent);
	}
}

// ============================================================================================================
//                                   ***** Private Functions *****
// ============================================================================================================

static bool file_exists(char *fname)
{
	return access(fname, F_OK) EQ 0;
}

static void delete_file(char *fname)
{
	if (file_exists(fname))
	{
		remove(fname);
	}
}

pcb_t *
retrieve_swapped_pcb(uint32_t pcb_id) {
	char path[MAX_CHARS] = "";

	pcb_t * pcb = NULL;

	sprintf(path,"%s%s%d%s", path_swap(), "/", pcb_id, ".swap");

	if(file_exists(path)) {

		int fd = open(path, O_RDONLY, 0666);

		if (fd != -1) {
			struct stat sb;

			if (fstat(fd, &sb) != ERROR) {

				void * file_address = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

				void *pcb_stream = malloc(sb.st_size);

				memcpy(pcb_stream, file_address, sb.st_size);

				pcb = pcb_from_stream(pcb_stream);

				munmap(file_address, sb.st_size);

				close(fd);

				free(pcb_stream);
			}
		}
	}

	return pcb;
}

uint32_t
swap_pcb(void *pcb_stream)
{
	uint32_t status = ERROR;

	pcb_t *pcb = pcb_from_stream(pcb_stream);

	char path[MAX_CHARS] = "";

	sprintf(path, "%s%s%d%s", path_swap(), "/", pcb->id, ".swap");

	delete_file(path);

	int fd = open(path, O_WRONLY | O_CREAT, 0666);

	if (fd != -1)
	{
		status = SUCCESS;

		off_t pct_stream_size = (off_t)pcb_bytes_size(pcb);

		ftruncate(fd, pct_stream_size);

		void *file_address = mmap(NULL, pct_stream_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		memcpy(file_address, pcb_stream, pct_stream_size);
		msync(file_address, pct_stream_size, MS_SYNC);
		munmap(file_address, pct_stream_size);
	}

	close(fd);

	free(pcb);

	return status;
}

void delete_swapped_pcb(uint32_t pcb_id)
{
	char path[MAX_CHARS] = "";

	sprintf(path, "%s%s%d%s", path_swap(), "/", pcb_id, ".swap");

	delete_file(path);
}

uint32_t
get_page_table(void)
{
	return create_new_process(&g_memory);
}
