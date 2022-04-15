/**
 * server.c
 *
 * @file  Test de Servidor
 * @author Tomás Sánchez
 * @since  09.11.2021
 */

#include "ctest.h"
#include "console.h"
#include "conexion.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <commons/string.h>

// ------------------------------------------------------------
//  Cliente - client_read
// ------------------------------------------------------------

// Read line
#define LINE_STR "Hello world!"

CTEST_DATA(client_read)
{
	bool status;
};

// @BeforeEach
CTEST_SETUP(client_read)
{
	data->status = RUNNING;
}

CTEST2(client_read, se_lee_una_linea)
{

	ASSERT_STR(LINE_STR, on_client_read(LINE_STR, &data->status));
}

CTEST2(client_read, una_linea_no_termina_proceso)
{
	on_client_read(LINE_STR, &data->status);
	ASSERT_TRUE(data->status);
}

CTEST2(client_read, linea_vacia_devuelve_NULL)
{
	ASSERT_NULL(on_client_read(string_new(), &data->status));
}

CTEST2(client_read, linea_vacia_corta_ejecucion)
{
	on_client_read(string_new(), &data->status);
	ASSERT_FALSE(data->status);
}

// ------------------------------------------------------------
//  Cliente - client_send
// ------------------------------------------------------------

// IP host
#define HOSTNAME "localhost"
// Open port
#define PORT "5001"

CTEST_DATA(client_send)
{
	conexion_t conexion;
};

// @BeforeEach
CTEST_SETUP(client_send)
{
	data->conexion = conexion_cliente_create(HOSTNAME, PORT);
}

// @AfterEach
CTEST_TEARDOWN(client_send)
{
	if (&data->conexion)
		conexion_destroy(&data->conexion);
}

CTEST2(client_send, no_se_envia_null)
{
	int result = on_client_send((void *)&data->conexion, NULL);
	ASSERT_EQUAL(ERROR, result);
}

CTEST2(client_send, no_se_envia_sin_conexion)
{
	int result = on_client_send((void *)&data->conexion, strdup(LINE_STR));
	ASSERT_EQUAL(ERROR, result);
}
