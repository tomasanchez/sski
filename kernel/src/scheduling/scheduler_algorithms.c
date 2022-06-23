/**
 * @file scheduler_algorithms.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-23-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "scheduler_algorithms.h"

void *get_next_fifo(void *scheduler)
{
	scheduler_t *s = (scheduler_t *)scheduler;
	return safe_queue_pop(s->ready);
}
