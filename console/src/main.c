/**
 * main.c
 *
 * @file iMongo Store main
 * @author Tomás Sánchez
 * @since  05.04.2021
 */
#include "console.h"

extern int yylex();

int main(void)
{

	yylex();

	if (on_init() NE ERROR)
	{
		on_client_run();

		return on_before_exit();
	}
	else
		return EXIT_FAILURE;
}
