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
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "cfg.h"
#include "fs.h"

// ============================================================================================================
//                               ***** Declarations *****
// ============================================================================================================
static bool
file_exists(char *fname);

static void delete_file(char *fname);
// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

static bool
file_exists(char *fname)
{
	return access(fname, F_OK) EQ 0;
}

static void
delete_file(char *fname)
{
	if (file_exists(fname))
	{
		LOG_WARNING("[FS] :=> Removing File %s");
		remove(fname);
	}
}

int open_create(uint32_t pid)
{
	char path[MAX_CHARS] = "";
	sprintf(path, "%s%s%d%s", path_swap(), "/", pid, ".swap");
	LOG_TRACE("[FS] :=> Creating file %s...", path);
	int fd = open(path, O_RDWR | O_CREAT, 0666);

	if (fd == -1)
	{
		LOG_ERROR("[FS] :=> Could open File %s", path);
		return -1;
	}

	LOG_DEBUG("[FS] :=> File %s was created", path);

	delete_file(path);

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

void create_file(uint32_t pid)
{

	int fd = open_create(pid);

	if (fd != -1)
	{
		close(fd);
	}
}

void delete_swap_file(uint32_t pid)
{
	char path[MAX_CHARS] = "";
	sprintf(path, "%s%s%d%s", path_swap(), "/", pid, ".swap");
	delete_file(path);
}
