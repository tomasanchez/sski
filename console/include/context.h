#pragma once

#define CONTEXT_AVAILABLE 0
#define CONTEXT_UNAVAILABLE -1

typedef struct context
{
	char *input_file_name;
	int process_size;
	int status;
} context_t;

context_t read_context(int argc, char *argv[]);
