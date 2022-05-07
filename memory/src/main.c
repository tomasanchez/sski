/**
 * @file main.c
 * @author fmaver@frba.utn.edu.ar
 * @brief
 * @version 0.1
 * @date 2022-04-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "memory_module.h"

memory_t g_memory;

int main(void)
{
	int status_code = on_init(&g_memory);

	if (status_code == EXIT_SUCCESS)
		status_code = on_run(&g_memory);

	on_before_exit(&g_memory, status_code);
}
