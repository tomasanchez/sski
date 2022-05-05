#include <commons/string.h>
#include <commons/collections/list.h>

#include "dispatcher.h"
#include "accion.h"
#include "instruction.h"
#include "thread_manager.h"
#include "server.h"
#include "smartlist.h"
#include "accion.h"
#include "log.h"
#include "lib.h"
#include "cfg.h"
#include "pcb.h"
#include "cpu.h"

// ============================================================================================================
//                               ***** Dispatcher -  Definiciones *****
// ============================================================================================================

extern cpu_t g_cpu;

// ============================================================================================================
//                               ***** Funciones Privadas - Definiciones *****
// ============================================================================================================

void *dispatch_imprimir_mensaje(void *args)
{
	char *msg = ((char *)args);

	THREAD_SAFE(LOG_INFO("Mensaje: %s", msg));

	free(msg);

	return NULL;
}
