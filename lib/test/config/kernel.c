#include <stdlib.h>
#include <string.h>

#include "cfg.h"
#include "ctest.h"

CTEST(config_kernel, when_kernel_config_init_then_it_does_init)
{
	if (config_init("../config/kernel.cfg") == ERROR)
		ASSERT_FAIL();

	config_close();
}
