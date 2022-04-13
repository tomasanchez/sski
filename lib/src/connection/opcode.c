/**
 * @file opcode.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Operation code enum methods
 * @version 0.1
 * @date 03-26-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "opcode.h"

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

char *opcode_to_string(opcode_t opcode)
{
	switch (opcode)
	{
	// Disconnected - Desconectado
	case DC:
		return "Desconectado";
	// Message - mensaje estándar
	case MSG:
		return "Mensaje";
	// Package - paquete
	case PKG:
		return "Paquete";
	// TODO : Agregar los casos correspondientes
	default:
		return "Código de operación no reconocido";
	}
}
