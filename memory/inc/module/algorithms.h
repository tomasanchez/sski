/**
 * @file algorithms.h
 * @author Francisco Maver (fmaver@frba.utn.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "operands.h"

/**
 * @brief Selects a frame to be replaced according to the algorithm CLOCK second chance.
 * ! DOES NOT VERIFY IF PAGE IS ALREADY EXISTS IN THE TABLE.
 *
 * @param memory The memory module.
 * @param table_index the index of Talbe LVL 1 to be used
 * @return the frame to be replaced
 */
uint32_t clock_selector(void *memory, uint32_t table_index);

/**
 * @brief Selects a frame to be replaced according to the algorithm CLOCK Improved.
 * ! DOES NOT VERIFY IF PAGE IS ALREADY EXISTS IN THE TABLE.
 *
 * @param memory The memory module.
 * @param table_index the index of Talbe LVL 1 to be used
 * @return the frame to be replaced
 */
uint32_t improved_clock_selector(void *memory, uint32_t table_index);
