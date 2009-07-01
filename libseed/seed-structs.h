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

#ifndef _SEED_STRUCT_H
#define _SEED_STRUCT_H
extern JSClassRef seed_struct_class;

JSObjectRef seed_make_union (JSContextRef ctx, gpointer younion,
			     GIBaseInfo * info);
JSObjectRef seed_make_struct (JSContextRef ctx, gpointer strukt,
			      GIBaseInfo * info);
JSObjectRef seed_make_boxed (JSContextRef ctx,
			     gpointer boxed, GIBaseInfo * info);

JSObjectRef seed_make_pointer (JSContextRef ctx, gpointer pointer);

JSValueRef
seed_field_get_value (JSContextRef ctx,
		      gpointer object,
		      GIFieldInfo * field, JSValueRef * exception);

gpointer seed_pointer_get_pointer (JSContextRef ctx, JSValueRef strukt);
void seed_pointer_set_free (JSContextRef ctx,
			    JSValueRef pointer, gboolean free_pointer);

GIFieldInfo *seed_struct_find_field (GIStructInfo * info, gchar * field_name);
GIFieldInfo *seed_union_find_field (GIUnionInfo * info, gchar * field_name);

JSObjectRef
seed_construct_struct_type_with_parameters (JSContextRef ctx,
					    GIBaseInfo * info,
					    JSObjectRef parameters,
					    JSValueRef * exception);

JSObjectRef seed_union_prototype (JSContextRef ctx, GIBaseInfo * info);
JSObjectRef seed_struct_prototype (JSContextRef ctx, GIBaseInfo * info);

void seed_structs_init ();

#endif
