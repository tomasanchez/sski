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
	int status_code = on_init(&g_context);

	if (status_code == EXIT_SUCCESS)
		status_code = on_run(&g_context);

	on_before_exit(&g_context, status_code);
}
