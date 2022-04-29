/**
 * main.c
 *
 * @file  Módulo
 * @author Tomás A. Sánchez and R. Nicolás Savinelli
 * @since  03.17.2022
 */

#include "kernel.h"

kernel_t g_kernel;

int main(void)
{
	int status_code = on_init(&g_kernel);

	if (status_code == EXIT_SUCCESS)
		status_code = on_run(&g_kernel);

	on_before_exit(&g_kernel, status_code);
}
