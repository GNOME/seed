/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-engine.h
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

#ifndef _SEED_ENGINE_H
#define _SEED_ENGINE_H

#include "seed-private.h"

extern JSClassRef gobject_class;
extern JSClassRef gobject_method_class;
extern JSClassRef gobject_constructor_class;
extern JSClassRef seed_callback_class;
extern SeedEngine *eng;

typedef struct _SeedScript {
	JSStringRef script;
	JSValueRef exception;

	JSStringRef source_url;
	gint line_number;
} SeedScript;

JSObjectRef seed_gobject_get_prototype_for_gtype(GType type);
JSClassRef seed_gobject_get_class_for_gtype(GType type);

void seed_gobject_define_property_from_function_info(GIFunctionInfo * info,
						     JSObjectRef object,
						     gboolean instance);
void seed_create_function(gchar *name, gpointer func, JSObjectRef obj);
void seed_make_exception(JSValueRef * exception,
			 const gchar * name, const gchar * message);
#endif
