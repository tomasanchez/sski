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

// ============================================================================================================
//                               ***** Dispatcher -  Definiciones *****
// ============================================================================================================

// Los PIDs
bool pids[PIDS];


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
