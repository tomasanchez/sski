/**
 * @file instruction_lambdas.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "smartlist.h"

/**
 * @brief Reduces a List of instructions
 *
 * @param instructions list to be reduced
 * @return a stream of an instruction list
 */
void *instructions_fold(t_list *instructions);
