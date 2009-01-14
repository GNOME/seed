/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed-types.h
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

#ifndef _SEED_TYPES_H
#define _SEED_TYPES_H

#include "seed-private.h"

JSValueRef seed_value_from_gvalue(JSContextRef ctx,
								  GValue * gval, JSValueRef * exception);

JSValueRef seed_object_get_property(JSContextRef ctx,
									JSObjectRef val, const gchar * name);

gboolean seed_object_set_property(JSContextRef ctx, JSObjectRef object,
								  const gchar * name, JSValueRef value);
gboolean seed_gvalue_from_seed_value(JSContextRef ctx,
									 JSValueRef val, GType type,
									 GValue * gval, JSValueRef * exception);

gboolean seed_gi_make_argument(JSContextRef ctx,
							   JSValueRef value,
							   GITypeInfo * type_info,
							   GArgument * arg, JSValueRef * exception);
JSValueRef seed_gi_argument_make_js(JSContextRef ctx,
									GArgument * arg,
									GITypeInfo * type_info,
									JSValueRef * exception);

gboolean seed_gi_release_arg(GITransfer transfer,
							 GITypeInfo * type_info, GArgument * arg);

gboolean seed_gi_release_in_arg(GITransfer transfer,
								GITypeInfo * type_info, GArgument * arg);

gboolean seed_value_to_boolean(JSContextRef ctx,
							   JSValueRef val, JSValueRef * exception);

JSValueRef seed_value_from_boolean(JSContextRef ctx,
								   gboolean val, JSValueRef * exception);

guint seed_value_to_uint(JSContextRef ctx,
						 JSValueRef val, JSValueRef * exception);

JSValueRef seed_value_from_uint(JSContextRef ctx,
								guint val, JSValueRef * exception);

gint seed_value_to_int(JSContextRef ctx,
					   JSValueRef val, JSValueRef * exception);

JSValueRef seed_value_from_int(JSContextRef ctx,
							   gint val, JSValueRef * exception);

gchar seed_value_to_char(JSContextRef ctx,
						 JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_char(JSContextRef ctx,
								gchar val, JSValueRef * exception);

guchar seed_value_to_uchar(JSContextRef ctx,
						   JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_uchar(JSContextRef ctx,
								 guchar val, JSValueRef * exception);

glong seed_value_to_long(JSContextRef ctx,
						 JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_long(JSContextRef ctx,
								glong val, JSValueRef * exception);

gulong seed_value_to_ulong(JSContextRef ctx,
						   JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_ulong(JSContextRef ctx,
								 gulong val, JSValueRef * exception);

gint64 seed_value_to_int64(JSContextRef ctx,
						   JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_int64(JSContextRef ctx,
								 gint64 val, JSValueRef * exception);

guint64 seed_value_to_uint64(JSContextRef ctx,
							 JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_uint64(JSContextRef ctx,
								  guint64 val, JSValueRef * exception);

gfloat seed_value_to_float(JSContextRef ctx,
						   JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_float(JSContextRef ctx,
								 gfloat val, JSValueRef * exception);

gdouble seed_value_to_double(JSContextRef ctx,
							 JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_double(JSContextRef ctx,
								  gdouble val, JSValueRef * exception);

gchar *seed_value_to_filename(JSContextRef ctx,
							  JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_filename(JSContextRef ctx,
									const gchar * filename,
									JSValueRef * exception);

gchar *seed_value_to_string(JSContextRef ctx,
							JSValueRef val, JSValueRef * exception);

JSValueRef seed_value_from_string(JSContextRef ctx,
								  const gchar * val, JSValueRef * exception);

GObject *seed_value_to_object(JSContextRef ctx,
							  JSValueRef val, JSValueRef * exception);
JSValueRef seed_value_from_object(JSContextRef ctx,
								  GObject * val, JSValueRef * exception);

void seed_toggle_ref(gpointer data, GObject * object, gboolean is_last_ref);

#endif
