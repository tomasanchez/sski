#pragma once

#define CONTEXT_AVAILABLE 0
#define CONTEXT_UNAVAILABLE -1

/**
 * @brief Context of Console Module.
 *
 */
typedef struct Context
{
	// The input file path.
	char *input_file_name;
	// The process size.
	int process_size;
	// The context status code.
	int status;
} context_t;

/**
 * @brief Evaluates the context of the console.
 *
 * @param argc the number of arguments passed to the console
 * @param argv the arguments list
 * @return a context object
 */
context_t context_evaluate(int argc, char *argv[]);
