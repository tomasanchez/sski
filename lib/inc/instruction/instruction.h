/**
 * @file instruction.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-15-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <inttypes.h>
#include "opcode.h"

/**
 * @brief Instruction ID Code
 *
 */
typedef enum InstructionCode
{
	EXIT,
	READ,
	WRITE,
	COPY,
	IO,
	NO_OP
} instcode_t;

/**
 * @brief Pseudo-code's instruction.
 *
 */
typedef struct Instruction
{
	// The id of the instruction.
	instcode_t icode;

	// First parameter of the instruction.
	uint32_t param0;

	// Second parameter of the instruction.
	uint32_t param1;

} instruction_t;

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

/**
 * @brief Instantiates a new instruction.
 *
 * @param icode the operation id of the instruction.
 * @return a new instruction instance
 */
instruction_t *new_instruction(instcode_t icode);

/**
 * @brief Instantiates a new instruction.
 *
 * @param icode the operation id of the instruction.
 * @param param0 the first parameter of the instruction
 * @return a new instruction instance
 */
instruction_t *instruction_new(instcode_t icode, uint32_t param0);

/**
 * @brief Instantiates a new instruction.
 *
 * @param icode the operation id of the instruction
 * @param param0 the first parameter of the instruction
 * @param param1 the second parameter of the instruction
 * @return a new instruction instance
 */
instruction_t *instruction_create(instcode_t icode, uint32_t param0, uint32_t param1);

/**
 * @brief Deallocates an instruction.
 *
 * @param instruction to be deleted
 */
void instruction_destroy(void *instruction);

/**
 * @brief Maps an instruction to a Stream.
 *
 * @param instruction to be mapped
 * @return a serialized stream containing an instruction.
 */
void *instruction_to_stream(void *instruction);

/**
 * @brief Instantiates an instruction from a stream.
 *
 * @param stream stream containing
 * @return instruction_t*
 */
instruction_t *instruction_from_stream(void *stream);

/**
 * @brief Reduces an instruction into a buffer.
 *
 * @param buffer the accumulator
 * @param next the next instruction
 * @return the accumulator
 */
void *instruction_reduce(void *buffer, void *next);
