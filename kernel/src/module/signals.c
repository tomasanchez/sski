#include <commons/collections/list.h>

#include <signal.h>

#include "server.h"
#include "signals.h"
#include "lib.h"
#include "log.h"
#include "kernel.h"

extern kernel_t g_kernel;

static void
_sigint()
{
	LOG_WARNING("Se capturó la señal SIGINT");
	on_before_exit(&(g_kernel), SIGINT);
}

static void
_sigusr1()
{
	LOG_WARNING("Se capturó la señal SIGUSR1");
}

static void
_sigusr2()
{
	LOG_WARNING("Se capturó la señal SIGUSR2");
}

static void __handler(int __sig)
{
	switch (__sig)
	{
	case SIGINT:
		_sigint();
		break;

	case SIGUSR1:
		_sigusr1();
		break;

	case SIGUSR2:
		_sigusr2();
		break;

	default:
		break;
	}
}

void signals_init()
{
	int signals[3] = {SIGINT, SIGUSR1, SIGUSR2};

	for (long unsigned int i = 0; i < sizeof(signals) / sizeof(int); i++)
		signal(signals[i], __handler);
}
