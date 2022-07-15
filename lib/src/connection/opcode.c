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
	// System Call - syscall
	case SYS:
		return "Syscall";
	// Command - instruction
	case CMD:
		return "Instrucción";
	case PCB:
		return "PCB";
	case SWAP_PCB:
		return "Swap PCB";
	case RETRIEVE_SWAPPED_PCB:
		return "Retrieve Swapped PCB";
	case RETRIEVED_PCB:
		return "Retrieve Swapped PCB response";
	case RD:
		return "Memory Read";
	case INT:
		return "Interruption";
	case SZ:
		return "Page Size";
	case ENTRIES:
		return "Entries per Page";
	case FRAME:
		return "Frame";
	case SND_PAGE:
		return "Second Level Page";
	case WT:
		return "Memory Write";
	case MEMORY_INIT:
		return "Init Memory for PCB";
	default:
		return "Unrecognized";
	}
}
