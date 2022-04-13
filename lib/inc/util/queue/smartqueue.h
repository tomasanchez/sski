/**
 * smartqueue.h
 *
 * @file Smart queues
 * @author Tomás Sánchez
 * @since  05.22.2021
 */

#pragma once

#include <commons/collections/queue.h>

/**
 * @brief Crea una queue si es NULL
 *
 * @param queue la queue a crear
 */
t_queue *queue_smart_create(t_queue *queue);

/**
 * @brief Añade un elemento
 *
 * @param queue la queue a la cual añadir
 * @param element el item a añadir.
 */
void queue_smart_push(void *queue, void *element);

/**
 * @brief Añade un elemento y devuelve la queue
 *
 * @param queue la queue a la cual añadir
 * @param element el item a añadir.
 * @return la queue a la cual se añadió
 */
t_queue *queue_smart_push_ret(t_queue *queue, void *element);

/**
 * @brief Destruye una queue junto con sus elementos (Si los hay)
 *
 * @param queue la queue a destruir
 * @param destroyer el destructor de elementos
 */
void queue_smart_destroy(void *queue, void (*destroyer)(void *));

/**
 * @brief Destruye una queue junto con sus elementos (Si los hay)
 *
 * @param queue la queue a destruir utilizando FREE
 */
void queue_smart_fast_destroy(void *queue);
