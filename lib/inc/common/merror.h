/**
 * merror.h
 *
 * @author Roberto Nicolás Savinelli
 */

#pragma once

#include <string.h>

#include "lib.h"

#define UNDEFINED_ERROR 0

struct error
{
	char *message;
	int code;
};

typedef struct error error_t;

void error_create(error_t *error);

void error_erase(error_t *error);

void error_set(error_t *error, int code, char *message, unsigned int message_size);
