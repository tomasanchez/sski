#include <stdlib.h>

#include "safe_queue.h"
#include "smartqueue.h"

// ============================================================================================================
//                               ***** Elements *****
// ============================================================================================================

safe_queue_t *
new_safe_queue(void)
{
	safe_queue_t *q = malloc(sizeof(safe_queue_t));
	pthread_mutex_init(&q->_mtx, NULL);
	q->_queue = queue_create();
	return q;
}

void safe_queue_destroy(safe_queue_t *q, void (*destroyer)(void *))
{
	pthread_mutex_destroy(&q->_mtx);
	queue_smart_destroy(q->_queue, destroyer);
	free(q);
}

void safe_queue_push(safe_queue_t *this, void *element)
{
	pthread_mutex_lock(&this->_mtx);
	queue_smart_push(this->_queue, element);
	pthread_mutex_unlock(&this->_mtx);
}

void safe_queue_pop(safe_queue_t *this)
{
	pthread_mutex_lock(&this->_mtx);
	queue_pop(this->_queue);
	pthread_mutex_unlock(&this->_mtx);
}

void *safe_queue_peek(safe_queue_t *this)
{
	// The element to be peeked in the queue
	void *e = NULL;

	if (this)
	{
		pthread_mutex_lock(&this->_mtx);
		e = queue_peek(this);
		pthread_mutex_unlock(&this->_mtx);
	}

	return e;
}
