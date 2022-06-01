/**
 * @file instruction_lambdas.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include "instruction_lambdas.h"
#include "instruction.h"

void *instructions_fold(t_list *list)
{
	//  The stream seed.
	void *stream = NULL;
	// Elements count
	uint32_t size = list_size(list);
	// Size in Bytes of list
	size_t insturctions_size = sizeof(instruction_t) * size;
	// The stream serializer offset
	uint32_t offset = 0;

	stream = malloc(sizeof(uint32_t) + insturctions_size);

	memcpy(stream, &size, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	// The streamerized list
	t_list *stream_list = list_map(list, instruction_to_stream);

	void *reducer(void *acc, void *next)
	{
		memcpy(acc + offset, next, sizeof(instruction_t));
		offset += sizeof(instruction_t);
		return acc;
	}

	void *ret = list_fold(stream_list, stream, reducer);

	list_smart_fast_destroy(stream_list);

	return ret;
}
