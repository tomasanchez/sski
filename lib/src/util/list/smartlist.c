/**
 * smartlist.c
 *
 * @file  API de Smart Lists
 * @author Tomás Sánchez
 * @since  05.20.2021
 */

#include "smartlist.h"
#include "lib.h"

#include <stdlib.h>

void smart_free(void *e);

// ============================================================================================================
//                                   ***** 	Funciones Públicas  *****
// ============================================================================================================

// ------------------------------------------------------------
// Constructor y Destructors
// ------------------------------------------------------------

t_list *list_smart_create(t_list *list)
{
	// Si no hay lista, la creo
	if (list EQ NULL)
		return list_create();

	return list;
}

void list_smart_destroy(void *list, void (*destroyer)(void *))
{
	if (list)
	{
		if (list_is_empty((t_list *)list))
			list_destroy((t_list *)list);
		else
			list_destroy_and_destroy_elements((t_list *)list, destroyer);
	}
}

void smart_free(void *e)
{
	if (e)
		free(e);
}

void list_smart_fast_destroy(void *list)
{
	list_smart_destroy(list, smart_free);
}

// ------------------------------------------------------------
// Adders
// ------------------------------------------------------------

void list_smart_add(void *list, void *element)
{
	if (element)
	{
		list_smart_create((t_list *)list);
		list_add((t_list *)list, element);
	}
}

t_list *list_smart_add_ret(t_list *list, void *element)
{
	t_list *l = list_smart_create(list);
	list_smart_add(l, element);
	return l;
}
