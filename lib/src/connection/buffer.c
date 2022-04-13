/**
 * @file buffer.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Buffer class implementation
 * @version 0.1
 * @date 03-26-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

buffer_t *buffer_create(size_t size)
{
	// A new buffer to be instaciated
	buffer_t *buffer = NULL;
	buffer = malloc(sizeof(buffer_t));
	buffer->size = size;
	buffer->stream = malloc(buffer->size);
	return buffer;
}

buffer_t *new_buffer(size_t size, void *stream)
{
	buffer_t *buffer = buffer_create(size);
	memcpy(buffer->stream, stream, size);
	return buffer;
}

void buffer_destroy(buffer_t *buffer_instance)
{
	if (buffer_instance == NULL)
		return;

	if (buffer_instance->stream != NULL)
		free(buffer_instance->stream);
	buffer_instance->stream = NULL;

	free(buffer_instance);
}
