/**
 * @file buffer.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Buffer Class definition
 * @version 0.1
 * @date 03-26-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <stdio.h>

// ============================================================================================================
//                               ***** Class Definition *****
// ============================================================================================================

/**
 * @brief Buffer is a generic implementation of a stream with defined size.
 *
 */
typedef struct Buffer
{
	// The stream size.
	int size;
	// A stream of bytes.
	void *stream;
} buffer_t;

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

/**
 * @brief Instantiates a Buffer.
 *
 * @param size the buffer size
 * @returns a new buffer instance with the specified size
 */
buffer_t *buffer_create(size_t size);

/**
 * @brief Instantiates a new buffer with the specified size and stream.
 *
 * @param size the size of the buffer
 * @param stream the stream to be bufferized
 * @return a new buffer with the specified strea
 */
buffer_t *new_buffer(size_t size, void *stream);

/**
 * @brief De-allocates a buffer instance.
 *
 * @param buffer the instance to be deleted
 */
void buffer_destroy(buffer_t *buffer);
