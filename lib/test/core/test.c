/**
 * @file test.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 03-26-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#define CTEST_MAIN

// uncomment lines below to enable/disable features. See README.md for details
#define CTEST_SEGFAULT
//#define CTEST_NO_COLORS
#define CTEST_COLOR_OK

#include "ctest.h"

int main(int argc, const char *argv[])
{
	printf("\033[0;35m");
	puts("# Shared Library :: Tests");
	printf("\033[0m");

	return ctest_main(argc, argv);
}
