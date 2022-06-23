/**
 * @file cpu_dispatch_controller.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-21-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "cpu_controller.h"
#include "pcb.h"
#include "kernel.h"

cpu_controller_t this;

void init_cpu_controller(void)
{
	cpu_controller_on_init(&this);
}

void destroy_cpu_controller(void)
{
	cpu_controller_destroy(&this);
}

cpu_controller_t *cpu_controller_on_init(cpu_controller_t *controller)
{
	pthread_mutex_init(&controller->cpu_dispatch, NULL);
	pthread_mutex_init(&controller->cpu_interrupt, NULL);
	return controller;
}

void cpu_controller_destroy(cpu_controller_t *cpu_controller)
{
	pthread_mutex_destroy(&cpu_controller->cpu_dispatch);
	pthread_mutex_destroy(&cpu_controller->cpu_interrupt);
}

ssize_t cpu_controller_send_pcb(conexion_t connection_dispatch, opcode_t opcode, pcb_t *pcb)
{
	ssize_t bytes_sent = -1;
	void *stream = pcb_to_stream(pcb);

	SAFE_STATEMENT(&this.cpu_dispatch, bytes_sent = conexion_enviar_stream(connection_dispatch, opcode, stream, pcb_bytes_size(pcb)));

	free(stream);

	return bytes_sent;
}

ssize_t cpu_controller_send_interrupt(conexion_t connection_interrupt)
{
	ssize_t bytes_sent = -1;
	uint32_t placeholder = 0;
	SAFE_STATEMENT(&this.cpu_interrupt, bytes_sent = conexion_enviar_stream(connection_interrupt, DC, &placeholder, sizeof(placeholder)));
	return bytes_sent;
}

void *
cpu_controller_receive_pcb(conexion_t connection_dispatch)
{
	ssize_t bytes_received = 0;
	void *stream = conexion_recibir_stream(connection_dispatch.socket, &bytes_received);
	pcb_t *recovered_pcb = pcb_from_stream(stream + sizeof(opcode_t));
	free(stream);
	return recovered_pcb;
}
