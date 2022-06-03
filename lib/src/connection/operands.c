#include <inttypes.h>
#include "operands.h"


void *operandos_to_stream(void *operandos){

	operands_t *this = (operands_t *)operandos;
	void *stream = malloc(sizeof(operands_t));

	size_t offset = 0lu;

	memcpy(stream + offset, &this->op1, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &this->op2, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}

operands_t operandos_from_stream(void *stream)
{
	operands_t ret;

	uint32_t op1 = 0u, op2 = 0u;

	size_t offset = 0lu;
	memcpy(&op1, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&op2, stream + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	ret.op1 = op1;
	ret.op2 = op2;

	return ret;
}
