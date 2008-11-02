/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-structs.h
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 *
 * libseed is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libseed is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "seed-private.h"
#include <string.h>
JSClassRef seed_struct_class = 0;


static void seed_struct_finalize(JSObjectRef strukt)
{
		g_free(JSObjectGetPrivate(strukt));
}

JSClassDefinition gobject_struct_def = {
		0, /* Version, always 0 */
		0,
		"seed_struct", /* Class Name */
		NULL, /* Parent Class */
		NULL, /* Static Values */
		NULL, /* Static Functions */
		NULL,
		seed_struct_finalize, /* Finalize */
		NULL, /* Has Property */
		0, 
		NULL,  /* Set Property */
		NULL, /* Delete Property */
		NULL, /* Get Property Names */
	        NULL, /* Call As Function */
		NULL, /* Call As Constructor */
		NULL, /* Has Instance */
		NULL  /* Convert To Type */
};

gpointer seed_struct_get_pointer(JSValueRef strukt)
{
	if (JSValueIsObjectOfClass(eng->context, strukt, seed_struct_class))
		return JSObjectGetPrivate((JSObjectRef)strukt);
	return 0;
}

JSObjectRef seed_make_union(gpointer younion)
{
	if (!seed_struct_class)
		seed_struct_class = JSClassCreate(&gobject_struct_def);
	
	return JSObjectMake(eng->context, seed_struct_class, younion);
}

JSObjectRef seed_make_struct(gpointer strukt)
{
		return seed_make_union(strukt);
}
