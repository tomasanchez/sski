/**
 * @file thread_manager_test.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-30-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ctest.h"
#include "thread_manager.h"

static void *
mock_routine(void *data)
{
	pthread_t id = pthread_self();

	for (;;)
	{
		if (!data)
			sleep(5);
	}

	return NULL;
}

CTEST(thread_manager, does_init)
{
	thread_manager_t tm = new_thread_manager();
	ASSERT_TRUE(tm.init);
	thread_manager_destroy(&tm);
}

CTEST(thread_manager, does_launch_threads)
{
	thread_manager_t tm = new_thread_manager();

	thread_manager_launch(&tm, mock_routine, NULL);

	thread_manager_destroy(&tm);
}
