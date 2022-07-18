/**
 * @file safe_list.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Safe list implementation.
 * @version 0.1
 * @date 03-19-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>

#include "safe_list.h"
#include "smartlist.h"

// ============================================================================================================
//                               ***** Memory Allocation *****
// ============================================================================================================

safe_list_t *new_safe_list()
{
	safe_list_t *new_list = malloc(sizeof(safe_list_t));
	pthread_mutex_init(&new_list->_mtx, NULL);
	new_list->_list = list_create();

	return new_list;
}

void safe_list_destroy(safe_list_t *this)
{
	pthread_mutex_destroy(&this->_mtx);
	list_destroy(this->_list);
	free(this);
}

void safe_list_destroy_with(safe_list_t *this, void (*destroyer)(void *))
{
	pthread_mutex_lock(&this->_mtx);
	list_smart_destroy(this->_list, destroyer);
	pthread_mutex_unlock(&this->_mtx);
	free(this);
}

void safe_list_fast_destroy(safe_list_t *self)
{
	pthread_mutex_lock(&self->_mtx);
	list_smart_fast_destroy(self->_list);
	pthread_mutex_unlock(&self->_mtx);
}

// ============================================================================================================
//                               ***** Elements *****
// ============================================================================================================

void safe_list_add(safe_list_t *this, void *element)
{
	pthread_mutex_lock(&this->_mtx);
	list_smart_add(this->_list, element);
	pthread_mutex_unlock(&this->_mtx);
}

void safe_list_add_all_safe(safe_list_t *this, safe_list_t *that)
{
	pthread_mutex_lock(&this->_mtx);

	pthread_mutex_lock(&that->_mtx);
	list_add_all(this->_list, that->_list);
	pthread_mutex_unlock(&that->_mtx);

	pthread_mutex_unlock(&this->_mtx);
}

void safe_list_add_all(safe_list_t *this, t_list *list)
{
	pthread_mutex_lock(&this->_mtx);
	list_add_all(this->_list, list);
	pthread_mutex_unlock(&this->_mtx);
}

void safe_list_replace(safe_list_t *this, int index, void *element){
	pthread_mutex_lock(&this->_mtx);
	list_replace(this->_list, index, element);
	pthread_mutex_unlock(&this->_mtx);
}

void safe_list_remove(safe_list_t *this, int index){
	pthread_mutex_lock(&this->_mtx);
	list_remove(this->_list, index);
	pthread_mutex_unlock(&this->_mtx);
}

void safe_list_add_in_index(safe_list_t *this, int index, void *element){
	pthread_mutex_lock(&this->_mtx);
	list_add_in_index(this->_list, index, element);
	pthread_mutex_unlock(&this->_mtx);
}

void * safe_list_get(safe_list_t *this, int index){
	void *e = NULL;
	pthread_mutex_lock(&this->_mtx);
	e = list_get(this->_list, index);
	pthread_mutex_unlock(&this->_mtx);
	return e;
}
