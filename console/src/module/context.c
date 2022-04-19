/**
 * @file context.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-18-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>

#include "context.h"
#include "lib.h"

context_t context_evaluate(int argc, char *argv[])
{
	context_t context = {NULL, 0, CONTEXT_UNAVAILABLE};

	switch (argc)
	{
	case 1:
		printf("console: fatal error: no input files");
		context.status = NO_INPUT_FILE_NAME_ERROR;
		break;

	case 2:
		printf("console: fatal error: no process size");
		context.status = NO_PROCESS_SIZE_ERROR;
		break;

	default:
		context.input_file_name = argv[1];
		context.process_size = atoi(argv[2]);
		context.status = CONTEXT_AVAILABLE;
		break;
	}

	return context;
}
