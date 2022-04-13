/**
 * @file opcode.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Operation codes definitions
 * @version 0.1
 * @date 03-26-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

// ============================================================================================================
//                                   ***** Definitions  *****
// ============================================================================================================

/**
 * @brief Operation Code enumeration.
 *
 */
typedef enum Opcode
{
	// Disconnected - Desconectado
	DC,
	// Message - mensaje estándar
	MSG,
	// Package - paquete
	PKG,
	// TODO: Agregar los opcodes que vayan surgiendo
} opcode_t;

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

/**
 * @brief Obtains the name of a operation code.
 *
 * @param opcode the opcode number
 * @return the label for the operation code
 */
char *opcode_to_string(opcode_t opcode);
