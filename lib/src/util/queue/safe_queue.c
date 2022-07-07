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

void *safe_queue_pop(safe_queue_t *this)
{
	pthread_mutex_lock(&this->_mtx);

	void *e = queue_is_empty(this->_queue) ? NULL : queue_pop(this->_queue);
	pthread_mutex_unlock(&this->_mtx);
	return e;
}

bool safe_queue_is_empty(safe_queue_t *this)
{
	pthread_mutex_lock(&this->_mtx);
	bool is_empty = queue_is_empty(this->_queue);
	pthread_mutex_unlock(&this->_mtx);
	return is_empty;
}

void *safe_queue_peek(safe_queue_t *this)
{
	// The element to be peeked in the queue
	void *e = NULL;

	if (this)
	{
		pthread_mutex_lock(&this->_mtx);
		e = queue_peek(this->_queue);
		pthread_mutex_unlock(&this->_mtx);
	}

	return e;
}

void safe_queue_sort(safe_queue_t *this, bool (*comparator)(void *, void *))
{
	pthread_mutex_lock(&this->_mtx);
	list_sort(this->_queue->elements, comparator);
	pthread_mutex_unlock(&this->_mtx);
}

void *safe_queue_find_by(safe_queue_t *self, bool (*criteria)(void *))
{
	void *value = NULL;

	if (self)
	{
		pthread_mutex_lock(&self->_mtx);
		value = list_find(self->_queue->elements, criteria);
		pthread_mutex_unlock(&self->_mtx);
	}

	return value;
}

bool safe_queue_any(safe_queue_t *self, bool (*matcher)(void *))
{
	bool any = false;
	if (self)
	{
		pthread_mutex_lock(&self->_mtx);
		any = list_any_satisfy(self->_queue->elements, matcher);
		pthread_mutex_unlock(&self->_mtx);
	}

	return any;
}

void *safe_queue_remove_by(safe_queue_t *self, bool (*matcher)(void *))
{
	void *pcb = NULL;

	if (self)
	{
		pthread_mutex_lock(&self->_mtx);
		pcb = list_remove_by_condition(self->_queue->elements, matcher);
		pthread_mutex_unlock(&self->_mtx);
	}

	return pcb;
}
