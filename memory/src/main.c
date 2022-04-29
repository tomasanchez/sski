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
#include "memoryModule.h"

context_t g_context;

int main(void)
{
	int status_code = on_init(&g_context);

	if (status_code == EXIT_SUCCESS)
		status_code = on_run(&g_context);

	on_before_exit(&g_context, status_code);
}

