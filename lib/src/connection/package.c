/**
 * @file package.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Package class implementation
 * @version 0.1
 * @date 03-27-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <string.h>
#include <stdlib.h>

#include "package.h"

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

package_t *
new_package(opcode_t opcode, buffer_t *buffer)
{
	// The new package instance
	package_t *new_package = NULL;
	new_package = malloc(sizeof(package_t));
	new_package->opcode = opcode;
	new_package->buffer = NULL;
	new_package->buffer = buffer;

	return new_package;
}

package_t *
new_package_for(opcode_t opcode, size_t size, void *stream)
{
	// A new buffer instance
	buffer_t *buffer = NULL;
	buffer = new_buffer(size, stream);
	return new_package(opcode, buffer);
}

package_t *
package_create(opcode_t opcode, char *str_stream)
{
	// String length  + '\0'
	size_t buffer_size = strlen(str_stream) + 1;
	return new_package_for(opcode, buffer_size, str_stream);
}

void package_destroy(package_t *package)
{
	if (package == NULL)
		return;

	if (package->buffer != NULL)
		buffer_destroy(package->buffer);
	package->buffer = NULL;

	free(package);
}

size_t
package_get_real_size(package_t *package)
{
	// Bytes occupied by buffer + Bytes occupied by <size> number + Opcode's bytes)
	return package->buffer->size + sizeof(int) + sizeof(opcode_t);
}

void *package_serialize(package_t *package)
{
	// Bytes occupied by the package
	size_t bytes = package_get_real_size(package);

	// Stream to be copied into
	void *stream = NULL;
	stream = malloc(bytes);

	// Stream offset
	size_t offset = 0;

	// Copy bytes incrementing the offset
	/**
	 *          [ OPCODE ][ DATA_SIZE ][ DATA ]
	 */
	memcpy(stream + offset, &(package->opcode), sizeof(opcode_t));
	offset += sizeof(opcode_t);

	memcpy(stream + offset, &(package->buffer->size), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, package->buffer->stream, package->buffer->size);
	offset += package->buffer->size;

	return stream;
}
