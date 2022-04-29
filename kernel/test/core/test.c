/**
 * test.c
 *
 * @file  Test SUITE
 * @author Tomás Sánchez
 * @since  06.19.2021
 */

#define CTEST_MAIN

// uncomment lines below to enable/disable features. See README.md for details
#define CTEST_SEGFAULT
//#define CTEST_NO_COLORS
#define CTEST_COLOR_OK

#include "ctest.h"
#include "kernel.h"

kernel_t g_kernel;

int main(int argc, const char *argv[])
{
	printf("\033[0;35m");
	puts("# Server :: Tests");
	printf("\033[0m");

	return ctest_main(argc, argv);
}
