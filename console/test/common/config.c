/**
 * @file config.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 03-26-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ctest.h"
#include "cfg.h"

// Relative path del CFG file
#define CFG_RELATIVE_PATH "../config/console.cfg"
// Nombre del CFG File
#define CFG_FILE "console"

CTEST(config, when_configInit_does_init)
{
	// Levanta configuraciones?
	ASSERT_EQUAL(SUCCESS, config_init(CFG_FILE));
	config_close();
};

CTEST(config, when_configClose_does_close)
{
	// Solo si abre el logger
	if (config_init(CFG_FILE) EQ SUCCESS)
	{
		config_close();
		ASSERT_NULL(config_instance());
	}
	else
		ASSERT_FAIL();
};
