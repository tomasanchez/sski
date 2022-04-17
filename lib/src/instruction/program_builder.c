/**
 * @file instruction_builder.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-15-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "program_builder.h"
#include <stdlib.h>

program_builder_t program_builder_create_struct()
{
	program_builder_t builder = {list_create(),
								 NULL};
	return builder;
}

void program_builder_destroy_struct(program_builder_t builder)
{
	list_smart_destroy(builder.instructions, (void (*)(void *))instruction_destroy);
}
