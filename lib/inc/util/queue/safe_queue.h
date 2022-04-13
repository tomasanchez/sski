#pragma once

#include <pthread.h>
#include <commons/collections/queue.h>

typedef struct SafeQueue
{

	// ! Private:

	/**
	 * @brief Thread safety ensurer.
	 * @private
	 */
	pthread_mutex_t _mtx;

	/**
	 * @brief queue implementation.
	 * @private
	 */
	t_queue *_queue;

	// ! Public:

} safe_queue_t;


/**
 * @brief safely add an element at the last of the queue.
 * 
 * @param queue the safe queue itself
 * 
 * @param element the element in order to add to the safe queue
 *
 * @return void
 */
void safe_queue_push(safe_queue_t *queue, void *element);

/**
 * @brief safely remove the last element of the queue.
 *
 * @param queue the safe queue itself
 * 
 * @return void
 */
void safe_queue_pop(safe_queue_t *queue);