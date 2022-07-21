/**
 * @file swap.c
 * @author Francisco Maver (fmaver@frba.utn.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "swap.h"

static char* path_swap;
static long retardo_swap;
static uint32_t tam_pagina;
static t_config* configuracion;
static registro_t* buff;
static pthread_mutex_t mutex_swap;
static t_list* marcos_virtuales;
static uint32_t cantidad_marcos;
static uint32_t memoria_size;

typedef struct MarcoVirtual{
	swap_t* swap;
	registro_t* registro;
}virtual_t;



swap_t
*swap_crearSwap(uint32_t pid)
{
	swap_t* swap = malloc(sizeof(swap_t));
	swap->swap_name = malloc(40);
	swap->items = list_create();

	LOG_TRACE("[SWAP] :=> Archivo swap creado con nombre: #%s#, para el proceso %u\n", swap->swap_name, pid);

	swap->archivo_swap = fopen(swap->swap_name,"w+");

	return swap;
}

void swap_destruirSwap(swap_t* swap)
{
	pthread_mutex_lock(&mutex_swap);

	LOG_TRACE("[SWAP] :=> Archivo swap eliminado de nombre: #%s#\n", swap->swap_name);

	list_destroy(swap->items);
	fclose(swap->archivo_swap);
	remove(swap->swap_name);
	free(swap->swap_name);
	free(swap);

	pthread_mutex_unlock(&mutex_swap);
}

void swapInit(){
	configuracion = config_create("./settings.conf");
	retardo_swap = config_get_int_value(configuracion, "RETARDO_SWAP");
	path_swap = config_get_string_value(configuracion, "PATH_SWAP");
	tam_pagina = config_get_int_value(configuracion, "TAM_PAGINA");
	memoria_size = config_get_int_value(configuracion, "TAM_MEMORIA");
	pthread_mutex_init(&mutex_swap,NULL);
	cantidad_marcos = memoria_size / tam_pagina + 1;
	marcos_virtuales = list_create();
}

bool esRegistro(void* item){
	return ((item_t*) item)->registro == buff;
}

bool swap_estaEnSwap(swap_t* swap,registro_t* registro){
	buff = registro;
	bool x =list_any_satisfy(swap->items,esRegistro);
	return x;
}

void swap_escribirEnSwap(swap_t* swap,registro_t* registro){
	pthread_mutex_lock(&mutex_swap);
	usleep(retardo_swap * 1000);

	if(!swap_estaEnSwap(swap,registro)){

		item_t* item = malloc(sizeof(item_t));
		item->registro = registro;
		item->indice = ftell(swap->archivo_swap) / tam_pagina;
		void* datos = ram_getDatosDeMarco(registro->valor);
		fwrite(datos,tam_pagina,1,swap->archivo_swap);
		free(datos);
		list_add(swap->items,item);

	} else if (ram_estaModificado(registro->valor)){
		buff = registro;
		item_t* item = list_find(swap->items,esRegistro);

		fseek(swap->archivo_swap,item->indice * tam_pagina,SEEK_SET);
		void* datos = ram_getDatosDeMarco(registro->valor);

		fwrite(datos,tam_pagina,1,swap->archivo_swap);
		fseek(swap->archivo_swap,0,SEEK_END);
		free(datos);
	}

	pthread_mutex_unlock(&mutex_swap);
}

void swap_escribirEnMemoria(swap_t* swap ,registro_t* registro){
	pthread_mutex_lock(&mutex_swap);

	usleep(retardo_swap * 1000);


	buff = registro;
	item_t* item = list_find(swap->items,esRegistro);


	fseek(swap->archivo_swap,item->indice * tam_pagina,SEEK_SET);
	void* datos = malloc(tam_pagina);

	fread(datos,tam_pagina,1,swap->archivo_swap);

	ram_escribirDatos(registro->valor,datos);


	pthread_mutex_unlock(&mutex_swap);
}
