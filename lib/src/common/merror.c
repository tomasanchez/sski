/**
 * merror.c
 *
 * @author Roberto Nicolás Savinelli
 */

#include <stdlib.h>
#include "merror.h"

static unsigned short _null_byte_size = sizeof('\0');

void error_create(error_t *error)
{
	error->message = NULL;
	error->code = UNDEFINED_ERROR;
}

void error_erase(error_t *error)
{
	if (error != NULL)
	{
		if (error->message)
		{
			free(error->message);
			error->message = NULL;
		}
		error->code = UNDEFINED_ERROR;
	}
}

void error_set(error_t *error, int code, char *message, unsigned int message_size)
{
	error_erase(error);

	error->message = (char *)malloc(sizeof(char) * (message_size) + _null_byte_size);
	strcpy(error->message, message);
	error->code = code;
}
