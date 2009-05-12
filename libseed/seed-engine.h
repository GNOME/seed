/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version. 
 * Seed is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Lesser General Public License for more details. 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with Seed.  If not, see <http://www.gnu.org/licenses/>. 
 * 
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 */

#ifndef _SEED_ENGINE_H
#define _SEED_ENGINE_H

#include "seed-private.h"

extern JSClassRef gobject_class;
extern JSClassRef gobject_method_class;
extern JSClassRef gobject_constructor_class;
extern JSClassRef gobject_named_constructor_class;
extern JSClassRef seed_struct_constructor_class; 
extern JSClassRef gobject_init_method_class;

extern JSClassRef seed_callback_class;
extern SeedEngine *eng;

extern JSObjectRef seed_obj_ref;

extern JSContextGroupRef context_group;

typedef struct _SeedScript
{
  JSStringRef script;
  JSValueRef exception;

  JSStringRef source_url;
  gint line_number;
} SeedScript;

JSObjectRef seed_gobject_get_prototype_for_gtype (GType type);
JSClassRef seed_gobject_get_class_for_gtype (JSContextRef ctx, GType type);

void
seed_gobject_define_property_from_function_info (JSContextRef ctx,
						 GIFunctionInfo * info,
						 JSObjectRef object,
						 gboolean instance);
void seed_create_function (JSContextRef ctx, gchar * name,
			   gpointer func, JSObjectRef obj);

void seed_repl_expose (JSContextRef ctx, ...);

typedef JSObjectRef (*SeedModuleInitCallback) (SeedEngine * eng);

void seed_prepare_global_context (JSContextRef ctx);

#endif
