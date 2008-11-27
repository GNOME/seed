/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed.h
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

#include <glib.h>
#include <glib-object.h>
#include <girepository.h>

#ifndef _SEED_H
#define _SEED_H

typedef gpointer SeedValue;
typedef gpointer SeedObject;
typedef gpointer SeedClass;
typedef gpointer SeedException;
typedef gpointer SeedFunction;

typedef gpointer SeedContext;

typedef struct _SeedScript SeedScript;

typedef struct _SeedEngine {
	SeedContext context;
	SeedValue global;
} SeedEngine;

typedef enum {
	SEED_CLASS_ATTRIBUTE_NONE = 0,
	SEED_CLASS_ATTRIBUTE_NO_SHARED_PROTOTYPE = 1 << 1
} SeedClassAttributes;


/*
 * seed-engine.c 
 */
SeedEngine * seed_init(gint * argc, gchar *** argv);

SeedScript *seed_make_script(SeedContext ctx,
							 const gchar * s, const gchar * source_url,
							 gint line_number);
SeedException seed_script_exception(SeedScript * s);
void seed_make_exception(SeedContext ctx, SeedException e, 
						 gchar * name, gchar * message);
gchar *seed_exception_get_name(SeedContext ctx, SeedException e);
gchar *seed_exception_get_message(SeedContext ctx, SeedException e);
guint seed_exception_get_line(SeedContext ctx, SeedException e);
gchar *seed_exception_get_file(SeedContext ctx, SeedException e);
gchar *seed_exception_to_string(SeedContext ctx, SeedException e);

SeedValue seed_evaluate(SeedContext ctx, 
						SeedScript * s, SeedObject this);

/*
 * seed-api.c
 */

SeedObject seed_make_object(SeedContext ctx,
							SeedClass class, gpointer private);

/*
 * seed-types.c 
 */
gboolean seed_object_set_property(SeedObject object,
								  const gchar * name,
								  SeedValue value);
SeedValue seed_object_get_property(SeedObject object, 
								   const gchar * name);

gboolean seed_value_to_boolean(SeedContext ctx,
							   SeedValue val,
							   SeedException * exception);

SeedValue seed_value_from_boolean(SeedContext ctx,
								  gboolean val, 
								  SeedException * exception);

guint seed_value_to_uint(SeedContext ctx,
						 SeedValue val, 
						 SeedException * exception);

SeedValue seed_value_from_uint(SeedContext ctx,
							   guint val,
							   SeedException * exception);

gint seed_value_to_int(SeedContext ctx, 
					   SeedValue val,
					   SeedException * exception);

SeedValue seed_value_from_int(SeedContext ctx,
							  gint val,
							  SeedException * exception);

gchar seed_value_to_char(SeedContext ctx,
						 SeedValue val,
						 SeedException * exception);
SeedValue seed_value_from_char(SeedContext ctx,
							   gchar val,
							   SeedException * exception);

guchar seed_value_to_uchar(SeedContext ctx,
						   SeedValue val,
						   SeedException * exception);
SeedValue seed_value_from_uchar(SeedContext ctx,
								guchar val,
								SeedException * exception);

glong seed_value_to_long(SeedContext ctx,
						 SeedValue val,
						 SeedException * exception);
SeedValue seed_value_from_long(SeedContext ctx,
							   glong val,
							   SeedException * exception);

gulong seed_value_to_ulong(SeedContext ctx,
						   SeedValue val,
						   SeedException * exception);
SeedValue seed_value_from_ulong(SeedContext ctx,
								gulong val,
								SeedException * exception);

gint64 seed_value_to_int64(SeedContext ctx,
						   SeedValue val,
						   SeedException * exception);
SeedValue seed_value_from_int64(SeedContext ctx,
								gint64 val,
								SeedException * exception);

guint64 seed_value_to_uint64(SeedContext ctx,
							 SeedValue val,
							 SeedException * exception);
SeedValue seed_value_from_uint64(SeedContext ctx,
								 guint64 val,
								 SeedException * exception);

gfloat seed_value_to_float(SeedContext ctx,
						   SeedValue val,
						   SeedException * exception);
SeedValue seed_value_from_float(SeedContext ctx,
								gfloat val,
								SeedException * exception);

gdouble seed_value_to_double(SeedContext ctx,
							 SeedValue val,
							 SeedException * exception);
SeedValue seed_value_from_double(SeedContext ctx,
								 gdouble val,
								 SeedException * exception);

gchar *seed_value_to_string(SeedContext ctx,
							SeedValue val,
							SeedException * exception);
SeedValue seed_value_from_string(SeedContext ctx,
								 gchar * val, 
								 SeedException * exception);

GObject *seed_value_to_object(SeedContext ctx,
							  SeedValue val,
							  SeedException * exception);
SeedValue seed_value_from_object(SeedContext ctx,
								 GObject * val,
								 SeedException * exception);

typedef void (*SeedFunctionCallback) (SeedContext ctx,
									  SeedObject function,
									  SeedObject this_object,
									  size_t argument_count,
									  const SeedValue arguments[],
									  SeedException * exception);

void seed_create_function(SeedContext ctx,
						  gchar * name, SeedFunctionCallback callback,
						  SeedObject object);


typedef void (*SeedModuleInitCallback) (SeedEngine * eng);

#endif
