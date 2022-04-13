/**
 * main.c
 *
 * @file  Módulo
 * @author Tomás A. Sánchez and R. Nicolás Savinelli
 * @since  03.17.2022
 */

#include "runtime.h"

context_t g_context;

int main(void)
{
	int exit_code = init(&g_context);

	if (exit_code == EXIT_SUCCESS)
		exit_code = run(&g_context);

	stop(&g_context, exit_code);
}
