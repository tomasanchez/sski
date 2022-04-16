/**
 * @file program_builder.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-15-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "smartlist.h"
#include "instruction.h"

typedef struct ProgramBuilder
{
	// List of instructions
	t_list *instructions;

	// Buffered element
	instruction_t *last_instruction;

} program_builder_t;

/**
 * @brief Creates a program builder struct ready to be used.
 *
 * @return an struct for a program builder
 */
program_builder_t program_builder_create_struct();

void instruction_builder_build(instruction_t *instruction);
