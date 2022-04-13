#include <stdlib.h>

#include "safe_queue.h"
#include "smartqueue.h"

// ============================================================================================================
//                               ***** Elements *****
// ============================================================================================================

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