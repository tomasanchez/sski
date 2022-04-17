/**
 * lib.h
 *
 * @file Header que acompla todas las funciones que conforman la libreria
 * @author Tomás Sánchez
 * @since  04.16.2021
 */

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>

// Sin errores
#define OK 0

// Equals
#define EQ ==

// Is
#define is =

// Error
#define ERROR -1

// No error
#define SUCCESS 1

// FLEX + BISON
#define YYACCEPT 0
#define YYABORT 1
#define YYNOMEM 2

enum
{
	LOG_INITIALIZATION_ERROR = -255,
	CONFIGURATION_INITIALIZATION_ERROR,
	SERVER_RUNTIME_ERROR,
	NO_INPUT_FILE_NAME_ERROR,
	NO_PROCESS_SIZE_ERROR,
};

// Not Equals
#define NE !=

#define not !

// Máximos caracteres de un path
#define MAX_CHARS FILENAME_MAX

// IP de prueba
#define IP "127.0.0.1"

// Puerto de Prueba
#define PUERTO "5001"

// Un texto vacio
#define EMPTY_STR ""

// Tiempo de espera en segundos
#define TIEMPO_ESPERA 5

// Process ID
#define PIDS 1024
// Process ID inválido
#define UNDEFINED (PIDS + 1)
