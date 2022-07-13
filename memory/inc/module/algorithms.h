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

#include <commons/string.h>
#include <commons/collections/list.h>

#include "memory_dispatcher.h"
#include "accion.h"
#include "instruction.h"
#include "thread_manager.h"
#include "server.h"
#include "smartlist.h"
#include "accion.h"
#include "log.h"
#include "lib.h"
#include "page_table.h"

uint32_t seleccionClock(page_table_lvl_1_t* tabla);


uint32_t seleccionClockM(page_table_lvl_1_t* tabla);
