/**
 * @file time.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-02-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <sys/time.h>

#define time_diff_ms(t0, t1) (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
