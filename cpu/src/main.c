/**
 * @file main.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 03-21-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include "cpu.h"

extern cpu_t g_cpu;

int main(void)
{
	int exit_status = EXIT_SUCCESS;

	if ((exit_status = on_init(&g_cpu)) == EXIT_SUCCESS)
	{
		on_run(&g_cpu);

		return on_before_exit(&g_cpu);
	}

	return exit_status;
}
