/**
 * main.c
 *
 * @file iMongo Store main
 * @author Tomás Sánchez
 * @since  05.04.2021
 */
#include "console.h"

int main(int argc, char *argv[])
{
	if (on_init(argc) NE ERROR)
	{
		on_client_run(argv[1]);

		return on_before_exit();
	}
	else
		return EXIT_FAILURE;
}
