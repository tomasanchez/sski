/**
 * @file sem.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief A semphatore shortcut helper according to the standard of OS Course in UTN-FRBA.
 * @version 0.1
 * @date 03-20-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <semaphore.h>
#include <pthread.h>

#define SHARE_BETWEEN_THREADS 0

/**
 * @brief Shortcut for sem_wait.
 *
 * @param sem the semaphore to be locked
 * @return the applied function result of sem_wait
 */
#define WAIT(sem) sem_wait(&sem)

/**
 * @brief Shortcut for sem_post.
 *
 * @param sem the sempahore to be signal
 * @return the applied function result sem_post
 */
#define SIGNAL(sem) sem_post(&sem)

/**
 * @brief  Shortcut for using a statement between a mutex exclusion.
 *
 * @param mutex the mutual exclusor
 * @param statement to be executed
 */
#define SAFE_STATEMENT(mutex, statement) \
	pthread_mutex_lock(mutex);           \
	statement;                           \
	pthread_mutex_unlock(mutex)
