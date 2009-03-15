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

#include <glib.h>
#include <glib-object.h>
#include <girepository.h>

#ifndef _SEED_H
#define _SEED_H

typedef gpointer SeedString;
typedef gpointer SeedValue;
typedef gpointer SeedObject;
typedef gpointer SeedClass;
typedef gpointer SeedException;

typedef gpointer SeedContext;
typedef gpointer SeedGlobalContext;
typedef gpointer SeedContextGroup;

typedef enum
{
  SEED_TYPE_UNDEFINED,
  SEED_TYPE_NULL,
  SEED_TYPE_BOOLEAN,
  SEED_TYPE_NUMBER,
  SEED_TYPE_STRING,
  SEED_TYPE_OBJECT
} SeedType;

typedef enum
{
  SEED_PROPERTY_ATTRIBUTE_NONE = 0,
  SEED_PROPERTY_ATTRIBUTE_READ_ONLY = 1 << 1,
  SEED_PROPERTY_ATTRIBUTE_DONT_ENUM = 1 << 2,
  SEED_PROPERTY_ATTRIBUTE_DONT_DELETE = 1 << 3
} SeedPropertyAttributes;

typedef enum
{
  SEED_CLASS_ATTRIBUTE_NONE = 0,
  SEED_CLASS_ATTRIBUTE_NO_SHARED_PROTOTYPE = 1 << 1
} SeedClassAttributes;

typedef struct _SeedScript SeedScript;

typedef struct _SeedEngine
{
  SeedGlobalContext context;
  SeedValue global;
  gchar **search_path;

  SeedContextGroup group;
} SeedEngine;

/*
 * seed-engine.c 
 */
SeedEngine *seed_init (gint * argc, gchar *** argv);
SeedValue seed_simple_evaluate (SeedContext ctx, gchar * source);

SeedScript *seed_make_script (SeedContext ctx,
			      const gchar * js, const gchar * source_url,
			      gint line_number);
SeedScript *seed_script_new_from_file (SeedContext ctx, gchar * file);
SeedException seed_script_exception (SeedScript * s);
void seed_make_exception (SeedContext ctx, SeedException exception,
			  const gchar * name, const gchar * message);
gchar *seed_exception_get_name (SeedContext ctx, SeedException exception);
gchar *seed_exception_get_message (SeedContext ctx, SeedException exception);
guint seed_exception_get_line (SeedContext ctx, SeedException exception);
gchar *seed_exception_get_file (SeedContext ctx, SeedException exception);
gchar *seed_exception_to_string (SeedContext ctx, SeedException exception);

SeedValue seed_evaluate (SeedContext ctx, SeedScript * s, SeedObject this);

/*
 * seed-api.c
 */

SeedGlobalContext seed_context_create (SeedContextGroup group,
				       SeedClass global_class);
SeedGlobalContext seed_context_ref (SeedGlobalContext ctx);
void seed_context_unref (SeedGlobalContext ctx);

SeedValue seed_make_null (SeedContext ctx);

SeedObject seed_make_object (SeedContext ctx, SeedClass class,
			     gpointer private);

gpointer seed_object_get_private (SeedObject object);
void seed_object_set_private (SeedObject object, gpointer value);

SeedString seed_string_ref (SeedString string);
void seed_string_unref (SeedString string);

gsize seed_string_get_maximum_size (SeedString string);
gsize seed_string_to_utf8_buffer (SeedString string, gchar * buffer,
				  size_t buffer_size);

gboolean seed_string_is_equal (SeedString a, SeedString b);
gboolean seed_string_is_equal_utf8 (SeedString a, const gchar * b);

gboolean seed_value_is_null (SeedContext ctx, SeedValue value);
gboolean seed_value_is_object (SeedContext ctx, SeedValue value);
gboolean seed_value_is_function (SeedContext ctx, SeedObject value);

SeedValue seed_object_call (SeedContext ctx,
			    SeedObject object,
			    SeedObject this,
			    gsize argument_count,
			    const SeedValue arguments[],
			    SeedException * exception);

void seed_value_unprotect (SeedContext ctx, SeedValue value);
void seed_value_protect (SeedContext ctx, SeedValue value);
/*
 * seed-types.c 
 */
gboolean seed_object_set_property (SeedContext ctx,
				   SeedObject object,
				   const gchar * name, SeedValue value);
SeedValue seed_object_get_property (SeedContext ctx,
				    SeedObject object, const gchar * name);

void seed_object_set_property_at_index (SeedContext ctx,
					SeedObject object,
					gint index,
					SeedValue value,
					SeedException * exception);

gboolean seed_value_to_boolean (SeedContext ctx,
				SeedValue val, SeedException * exception);

SeedValue seed_value_from_boolean (SeedContext ctx,
				   gboolean val, SeedException * exception);

guint seed_value_to_uint (SeedContext ctx,
			  SeedValue val, SeedException * exception);

SeedValue seed_value_from_uint (SeedContext ctx,
				guint val, SeedException * exception);

gint seed_value_to_int (SeedContext ctx,
			SeedValue val, SeedException * exception);

SeedValue seed_value_from_int (SeedContext ctx,
			       gint val, SeedException * exception);

gchar seed_value_to_char (SeedContext ctx,
			  SeedValue val, SeedException * exception);
SeedValue seed_value_from_char (SeedContext ctx,
				gchar val, SeedException * exception);

guchar seed_value_to_uchar (SeedContext ctx,
			    SeedValue val, SeedException * exception);
SeedValue seed_value_from_uchar (SeedContext ctx,
				 guchar val, SeedException * exception);

glong seed_value_to_long (SeedContext ctx,
			  SeedValue val, SeedException * exception);
SeedValue seed_value_from_long (SeedContext ctx,
				glong val, SeedException * exception);

gulong seed_value_to_ulong (SeedContext ctx,
			    SeedValue val, SeedException * exception);
SeedValue seed_value_from_ulong (SeedContext ctx,
				 gulong val, SeedException * exception);

gint64 seed_value_to_int64 (SeedContext ctx,
			    SeedValue val, SeedException * exception);
SeedValue seed_value_from_int64 (SeedContext ctx,
				 gint64 val, SeedException * exception);

guint64 seed_value_to_uint64 (SeedContext ctx,
			      SeedValue val, SeedException * exception);
SeedValue seed_value_from_uint64 (SeedContext ctx,
				  guint64 val, SeedException * exception);

gfloat seed_value_to_float (SeedContext ctx,
			    SeedValue val, SeedException * exception);
SeedValue seed_value_from_float (SeedContext ctx,
				 gfloat val, SeedException * exception);

gdouble seed_value_to_double (SeedContext ctx,
			      SeedValue val, SeedException * exception);
SeedValue seed_value_from_double (SeedContext ctx,
				  gdouble val, SeedException * exception);

gchar *seed_value_to_string (SeedContext ctx,
			     SeedValue val, SeedException * exception);
SeedValue seed_value_from_string (SeedContext ctx,
				  gchar * val, SeedException * exception);

gchar *seed_value_to_filename (SeedContext ctx,
			       SeedValue val, SeedValue * exception);
SeedValue seed_value_from_filename (SeedContext ctx,
				    const gchar * filename,
				    SeedValue * exception);

GObject *seed_value_to_object (SeedContext ctx,
			       SeedValue val, SeedException * exception);
SeedValue seed_value_from_object (SeedContext ctx,
				  GObject * val, SeedException * exception);

gpointer seed_pointer_get_pointer (SeedContext ctx, SeedValue pointer);

typedef void (*SeedFunctionCallback) (SeedContext ctx,
				      SeedObject function,
				      SeedObject this_object,
				      gsize argument_count,
				      const SeedValue arguments[],
				      SeedException * exception);

void seed_create_function (SeedContext ctx,
			   gchar * name, SeedFunctionCallback callback,
			   SeedObject object);

typedef void (*SeedModuleInitCallback) (SeedEngine * eng);

typedef void (*SeedObjectInitializeCallback) (SeedContext ctx,
					      SeedObject object);

/* Using any functions that require a context from 
 *this callback has undefined results */
typedef void (*SeedObjectFinalizeCallback) (SeedObject object);

typedef gboolean (*SeedObjectHasPropertyCallback) (SeedContext ctx,
						   SeedObject object,
						   SeedString string);
typedef SeedValue (*SeedObjectGetPropertyCallback) (SeedContext ctx,
						    SeedObject object,
						    SeedString property_name,
						    SeedException * e);
typedef gboolean (*SeedObjectSetPropertyCallback) (SeedContext ctx,
						   SeedObject object,
						   SeedString property_name,
						   SeedValue value,
						   SeedException * e);
typedef gboolean (*SeedObjectDeletePropertyCallback) (SeedContext ctx,
						      SeedObject object,
						      SeedString
						      property_name,
						      SeedValue value,
						      SeedException * e);
/* TODO: Have to decide on accumulator API
//typedef void (*SeedObjectGetPropertyNamesCallback) (SeedContext ctx, */

typedef void (*SeedObjectGetPropertyNamesCallback) (void);

typedef SeedValue (*SeedObjectCallAsFunctionCallback) (SeedContext ctx,
						       SeedObject function,
						       SeedObject this_object,
						       gsize argument_count,
						       const SeedValue
						       arguments[],
						       SeedException *
						       exception);
typedef SeedValue (*SeedObjectCallAsConstructorCallback) (SeedContext ctx,
							  SeedObject
							  constructor,
							  gsize
							  argument_count,
							  const SeedValue
							  arguments[],
							  SeedException *
							  exception);

typedef gboolean (*SeedObjectHasInstanceCallback) (SeedContext ctx,
						   SeedObject constructor,
						   SeedObject instance_p,
						   SeedException * exception);

typedef SeedValue (*SeedObjectConvertToTypeCallback) (SeedContext ctx,
						      SeedObject object,
						      SeedType type,
						      SeedException *
						      exception);

typedef struct _seed_static_value
{
  const gchar *const name;
  SeedObjectGetPropertyCallback get_property;
  SeedObjectSetPropertyCallback set_property;
  SeedPropertyAttributes attributes;
} seed_static_value;

typedef struct _seed_static_function
{
  const gchar *const name;
  SeedObjectCallAsFunctionCallback callback;
  SeedPropertyAttributes attributes;
} seed_static_function;

typedef struct _seed_class_definition
{
  int version;			/* Always 0 */
  SeedClassAttributes attributes;

  const gchar *class_name;
  SeedClass parent_class;

  const seed_static_value *static_values;
  const seed_static_function *static_functions;

  SeedObjectInitializeCallback initialize;
  SeedObjectFinalizeCallback finalize;
  SeedObjectHasPropertyCallback has_property;
  SeedObjectGetPropertyCallback get_property;
  SeedObjectSetPropertyCallback set_property;
  SeedObjectDeletePropertyCallback delete_property;
  SeedObjectGetPropertyNamesCallback get_property_names;
  SeedObjectCallAsFunctionCallback call_as_function;
  SeedObjectCallAsConstructorCallback call_as_constructor;
  SeedObjectHasInstanceCallback has_instance;
  SeedObjectConvertToTypeCallback convert_to_type;
} seed_class_definition;

#define seed_empty_class { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};

SeedClass seed_create_class (seed_class_definition * def);

SeedObject seed_make_constructor (SeedContext ctx,
				  SeedClass class,
				  SeedObjectCallAsConstructorCallback
				  constructor);

void seed_engine_set_search_path (SeedEngine * eng, gchar ** path);
gchar **seed_engine_get_search_path (SeedEngine * eng);

void
seed_signal_connect (SeedContext ctx,
		     GObject *object,
		     const gchar *signal,
		     const gchar *script);

void
seed_signal_connect_value (SeedContext ctx,
			   GObject *object,
			   const gchar *signal,
			   SeedValue function,
			   SeedValue user_data);			   



#endif
