/**
 * smartlist.h
 *
 * @file  API de SMART list
 * @author Tomás Sánchez
 * @since  05.20.2021
 */

#pragma once

#include <commons/collections/list.h>

/**
 * @brief Crea una lista si es NULL
 *
 * @param list la lista a crear
 */
t_list *list_smart_create(t_list *list);

/**
 * @brief Añade un elemento
 *
 * @param list la lista a la cual añadir
 * @param element el item a añadir.
 */
void list_smart_add(void *list, void *element);

/**
 * @brief Añade un elemento y devuelve la lista
 *
 * @param list la lista a la cual añadir
 * @param element el item a añadir.
 * @return la lista a la cual se añadió
 */
t_list *list_smart_add_ret(t_list *list, void *element);

/**
 * @brief Destruye una lista junto con sus elementos (Si los hay)
 *
 * @param list la lista a destruir
 * @param destroyer el destructor de elementos
 */
void list_smart_destroy(void *list, void (*destroyer)(void *));

/**
 * @brief Destruye una lista junto con sus elementos (Si los hay)
 *
 * @param list la lista a destruir utilizando FREE
 */
void list_smart_fast_destroy(void *list);
