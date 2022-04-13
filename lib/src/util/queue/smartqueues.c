/**
 * smartqueues.c
 *
 * @file API de Queues
 * @author Tomás Sánchez
 * @since  05.22.2021
 */

#include "lib.h"
#include "smartqueue.h"

#include <stdlib.h>

// ============================================================================================================
//                                   ***** 	Funciones Públicas  *****
// ============================================================================================================

// ------------------------------------------------------------
// Constructor y Destructor
// ------------------------------------------------------------

t_queue *queue_smart_create(t_queue *queue)
{
	// Si no hay queuea, la creo
	if (queue EQ NULL)
		queue = queue_create();

	return queue;
}

void queue_smart_destroy(void *queue, void (*destroyer)(void *))
{
	if (queue)
	{
		if (queue_is_empty((t_queue *)queue))
			queue_destroy((t_queue *)queue);
		else
			queue_destroy_and_destroy_elements((t_queue *)queue, destroyer);
	}
}

void queue_smart_fast_destroy(void *queue)
{
	queue_smart_destroy(queue, free);
}
// ------------------------------------------------------------
// Adders
// ------------------------------------------------------------

void queue_smart_push(void *queue, void *element)
{
	if (element)
	{
		queue_smart_create((t_queue *)queue);
		queue_push((t_queue *)queue, element);
	}
}

t_queue *queue_smart_push_ret(t_queue *queue, void *element)
{
	t_queue *l = queue_smart_create(queue);
	queue_smart_push(l, element);
	return l;
}
