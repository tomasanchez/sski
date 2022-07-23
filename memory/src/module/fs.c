/**
 * @file fs.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief File System implementation
 * @version 0.1
 * @date 07-22-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "cfg.h"
#include "fs.h"

// ============================================================================================================
//                               ***** Declarations *****
// ============================================================================================================

static void delete_file(char *fname);
// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

static void
delete_file(char *fname)
{
	if (file_exists(fname))
	{
		LOG_WARNING("[FS] :=> Removing File %s", fname);
		remove(fname);
	}
}

int open_create(uint32_t pid)
{
	char path[MAX_CHARS] = "";
	sprintf(path, "%s%s%d%s", path_swap(), "/", pid, ".swap");
	delete_file(path);
	LOG_TRACE("[FS] :=> Creating file %s...", path);
	int fd = open(path, O_RDWR | O_CREAT, 0666);

	if (fd == -1)
	{
		LOG_ERROR("[FS] :=> Could open File %s", path);
		return -1;
	}

	LOG_DEBUG("[FS] :=> File %s opened", path);

	return fd;
}
// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

int open_file(uint32_t pid)
{
	char path[MAX_CHARS] = "";

	sprintf(path, "%s%s%d%s", path_swap(), "/", pid, ".swap");

	return open(path, O_RDWR | O_CREAT, 0666);
}

void create_file(uint32_t pid, uint32_t size)
{

	int fd = open_create(pid);

	if (fd != -1)
	{

			uint32_t length = size;

			ftruncate(fd, length);

			void *file_address = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

			if (file_address == MAP_FAILED)
			{
				LOG_ERROR("[FS] :=> %s", strerror(errno));
				return;
			}

			memcpy(file_address, &pid, sizeof(pid));
			msync(file_address, length, MS_SYNC);
			munmap(file_address, length);
			LOG_INFO("[FS] :=> Created file for PCB #%d [%dbytes]", pid, size);
	

		close(fd);
	}
}

void delete_swap_file(uint32_t pid)
{
	char path[MAX_CHARS] = "";
	sprintf(path, "%s%s%d%s", path_swap(), "/", pid, ".swap");
	delete_file(path);
}

bool file_exists(char *fname)
{
	return access(fname, F_OK) EQ 0;
}
