/**
 * @file pcb_state.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

/**
 * @brief A Process State.
 *
 */
typedef enum ProcessState
{
	NEW,
	READY,
	EXECUTING,
	SUSPENDED_READY,
	BLOCKED,
	SUSPENDED_BLOCKED,
	TERMINATED
} pcb_state_t;
