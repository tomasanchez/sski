/**
 * main.c
 *
 * @file iMongo Store main
 * @author Tomás Sánchez
 * @since  05.04.2021
 */
#include "console.h"
#include "context.h"

int main(int argc, char *argv[])
{
	int exit_code = EXIT_FAILURE;

	context_t context = context_evaluate(argc, argv);

	if (context.status == CONTEXT_AVAILABLE)
	{
		if (on_init() NE ERROR)
		{
			on_run(context.input_file_name, context.process_size);

			exit_code = on_before_exit();
		}
	}
	else
	{
		exit_code = context.status;
	}

	return exit_code;
}
