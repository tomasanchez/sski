#pragma once

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/**
 * @brief Operandos contains two uint32_t.
 *
 */
typedef struct Operandos{

	// first operando
	uint32_t op1;

	// second operando
	uint32_t op2;
}operands_t;

void *operandos_to_stream(void *operandos);

operands_t operandos_from_stream(void *stream);

