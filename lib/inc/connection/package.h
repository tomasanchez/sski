/**
 * @file package.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Stream package
 * @version 0.1
 * @date 03-26-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "buffer.h"
#include "opcode.h"

// ============================================================================================================
//                                   ***** Public Class  *****
// ============================================================================================================

/**
 * @brief A generic model for data packages sent/received.
 * @class
 * @author Tomás Sánchez
 */
typedef struct Package
{
	// Operation Code
	opcode_t opcode;
	// Internal buffer
	buffer_t *buffer;
} package_t;

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

/**
 * @brief Creates a new package for a message.
 *
 * @param opcode the operation code
 * @param str_stream the message to be packaged
 * @return a new packaged message
 */
package_t *package_create(opcode_t opcode, char *str_stream);

/**
 * @brief Creates a new package for a buffer.
 *
 * @param opcode the operation code
 * @param buffer the buffer to be packaged
 * @return a new packaged buffer
 */
package_t *new_package(opcode_t opcode, buffer_t *buffer);

/**
 * @brief Creates a new package for a stream.
 *
 * @param opcode
 * @param size the size of the stream
 * @param stream a stream to be bufferized
 * @return a new packaged buffer
 */
package_t *new_package_for(opcode_t opcode, size_t size, void *stream);

/**
 * @brief De-allocates a package instance.
 *
 * @param package_instance the instance to be freed
 */
void package_destroy(package_t *package_instance);

/**
 * @brief Get the package size in bytes.
 *
 * @param package the package itself
 * @return the number of bytes the package occupies.
 */
size_t package_get_real_size(package_t *package);

/**
 * @brief Serialize a package into a stream.
 *
 * @param package to be serialized
 * @return a serialized stream
 */
void *package_serialize(package_t *package);
