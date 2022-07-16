/**
 * @file safe_list.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Thread Safe smartlist class definition.
 * @version 0.1
 * @date 03-19-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

// ============================================================================================================
//                               		***** Includes *****
// ============================================================================================================

#include <pthread.h>
#include <commons/collections/list.h>

// ============================================================================================================
//                               		***** Class Declarations *****
// ============================================================================================================

/**
 * @brief Collection implementation with thread safe methods.
 *
 * @author Tomás Sánchez
 */
typedef struct SafeList
{

	// ! Private:

	/**
	 * @brief Thread safety ensurer.
	 * @private
	 */
	pthread_mutex_t _mtx;

	/**
	 * @brief Collection implementation.
	 * @private
	 */
	t_list *_list;

	// ! Public:

} safe_list_t;

// ============================================================================================================
//                               		***** Functions Declarations *****
// ============================================================================================================

/**
 * @brief Instances a new empty list.
 *
 * @return an empty thread safe list.
 */
safe_list_t *new_safe_list();

/**
 * @brief Destroys a list without destroying its elements.
 *
 */
void safe_list_destroy();

/**
 * @brief Destroys the list and all elements. If no elements are present, it only destroys the list.
 *
 * @param list the safelist itself
 * @param destroyer a consumer function to destroy elements of the list
 */
void safe_list_destroy_with(safe_list_t *list, void (*destroyer)(void *));

/**
 * @brief Destroys the list and FREEs all the elements. If no elements are present, it only destroys the list.
 *
 * @param list  the safelist itself
 */
void safe_list_fast_destroy(safe_list_t *list);

/**
 * @brief Adds an element to the list.
 *
 * @param list a safe list instance
 * @param element to be added
 */
void safe_list_add(safe_list_t *list, void *element);
