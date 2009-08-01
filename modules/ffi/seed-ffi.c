/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

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
 * Copyright (C) Robert Carr 2009 <carrr@rpi.edu>
 */

#include <glib.h>
#include <seed-module.h>

SeedEngine *eng;
SeedObject namespace_ref;

SeedClass ffi_library_class;

static SeedValue
seed_ffi_library_get_property (SeedContext ctx,
			       SeedObject this_object,
			       SeedString property_name,
			       SeedException *exception)
{
  GModule *mod;
  gchar *prop;
  gsize len = seed_string_get_maximum_size (property_name);
  gpointer symbol;
  
  prop = g_alloca (len);
  seed_string_to_utf8_buffer (property_name, prop, len);
  
  mod = seed_object_get_private (this_object);
  
  if (!g_module_symbol (mod, prop, &symbol))
    {
      return NULL;
    }
  return seed_value_from_boolean (ctx, TRUE, exception);
}
			       

static SeedObject
seed_ffi_construct_library (SeedContext ctx,
			    SeedObject constructor,
			    size_t argument_count,
			    const SeedValue arguments[],
			    SeedException *exception)
{
  GModule *mod;
  SeedObject ret;
  gchar *filename;
  
  if (argument_count != 1)
    {
      seed_make_exception (ctx, exception, 
			   "ArgumentError", 
			   "ffi.Library constructor expects 1 argument (filename), got %d", 
			   argument_count);
      return seed_make_null (ctx);
    }
  filename = seed_value_to_string (ctx, arguments[0], exception);
  
  mod = g_module_open (filename, G_MODULE_BIND_LOCAL | G_MODULE_BIND_LAZY);
  if (!mod)
    {
      seed_make_exception (ctx, exception, "GModuleError",
			   "Opening module (%s) failed with: %s",
			   filename, g_module_error ());
      return seed_make_null (ctx);
    }
  
  ret = seed_make_object (ctx, ffi_library_class, mod);
  
  return ret;
}

static void
seed_ffi_library_finalize (SeedObject obj)
{
  GModule *mod = seed_object_get_private (obj);
  
  g_module_close (mod);
}

SeedObject
seed_module_init(SeedEngine *local_eng)
{
  SeedObject library_constructor;
  seed_class_definition ffi_library_def = seed_empty_class;

  ffi_library_def.class_name = "FFILibrary";
  ffi_library_def.finalize = seed_ffi_library_finalize;
  ffi_library_def.get_property = seed_ffi_library_get_property;
  
  ffi_library_class = seed_create_class (&ffi_library_def);

  eng = local_eng;
  namespace_ref = seed_make_object (eng->context, NULL, NULL);
  
  library_constructor = seed_make_constructor (eng->context, ffi_library_class, seed_ffi_construct_library);
  seed_object_set_property (eng->context, namespace_ref, "Library", library_constructor);
  
  return namespace_ref;
}
