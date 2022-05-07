
#define CTEST_MAIN

// uncomment lines below to enable/disable features. See README.md for details
#define CTEST_SEGFAULT
//#define CTEST_NO_COLORS
#define CTEST_COLOR_OK

#include "ctest.h"
#include "memory_module.h"

memory_t g_memory;

int main(int argc, const char *argv[])
{
	printf("\033[0;35m");
	puts("# Memory :: Tests");
	printf("\033[0m");

	return ctest_main(argc, argv);
}
