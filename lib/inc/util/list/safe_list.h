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

/**
 * @brief Coloca un elemento en una de la posiciones de la lista.
 *
 * @param index
 * @param element
 * @return EL valor anterior.
 */
void safe_list_replace(safe_list_t *, int index, void *element);

/**
 * @brief Remueve un elemento de la lista de una determinada posicion.
 *
 * @param index
 * @return El elemento removido.
 */
void safe_list_remove(safe_list_t *, int index);

/**
 * @brief Coloca un elemento en una de la posiciones de la lista.
 *
 * @param index
 * @param element
 */
void safe_list_add_in_index(safe_list_t *, int index, void *element);

/**
 * @brief list get
 *
 * @param index
 * @return Retorna el contenido de una posicion determinada de la lista
 */
void* safe_list_get(safe_list_t *, int index);
