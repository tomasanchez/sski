/**
 * @file lts.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Long Time Scheduler Class
 * @version 0.1
 * @date 05-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

/**
 * @brief Schedules in the long term.
 *
 * @param kernel_ref a kernel reference
 */
void *
long_time_schedule(void *kernel_ref);
