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

typedef struct _SeedStructPrivates
{
	gpointer object;
	GIBaseInfo * info;
} SeedStructPrivates;

static void seed_struct_finalize(JSObjectRef younion)
{
		g_free(JSObjectGetPrivate(younion));
}

JSClassDefinition gobject_struct_def = {
		0, /* Version, always 0 */
		0,
		"gobject_union", /* Class Name */
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

JSObjectRef seed_make_union(gpointer younion, GIBaseInfo * info)
{
	SeedStructPrivates * privates;
	if (!seed_struct_class)
		seed_struct_class = JSClassCreate(&gobject_struct_def);
	
	privates = g_new0(SeedStructPrivates, 1);

	privates->object = younion;
	privates->info = info;
	
	return JSObjectMake(eng->context, seed_struct_class, privates);
}

JSObjectRef seed_make_struct(gpointer strukt, GIBaseInfo * info)
{
		return seed_make_union(strukt, info);
}
