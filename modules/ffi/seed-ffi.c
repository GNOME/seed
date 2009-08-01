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

#include <ffi.h>

SeedEngine *eng;
SeedObject namespace_ref;

SeedClass ffi_library_class;
SeedClass ffi_function_class;

typedef struct _seed_ffi_function_priv {
  gchar *name;
  gpointer symbol;
  
  GType *args;
  gint n_args;
  
  GType ret_val;
  
  SeedObject signature_obj;
  SeedObject module_obj;
} seed_ffi_function_priv;

static SeedValue
seed_ffi_get_signature (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedException *exception)
{
  seed_ffi_function_priv *priv = seed_object_get_private (this_object);
  
  if (priv->signature_obj)
    return priv->signature_obj;
  else
    return seed_make_null (ctx);
}

static gboolean
seed_ffi_build_signature (SeedContext ctx,
			  seed_ffi_function_priv *priv,
			  SeedObject sig,
			  SeedException *exception)
{
  SeedObject arguments;
  SeedValue ret_type_ref, length_ref;
  guint length, i;
  
  arguments = seed_object_get_property (ctx, sig, "arguments");
  ret_type_ref = seed_object_get_property (ctx, sig, "returns");
  
  if (!seed_value_is_object (ctx, arguments))
    {
      seed_make_exception (ctx, exception, "FFIError", 
			   "Signature arguments member must be an array describing argument types");
      return FALSE;
    }
  length_ref = seed_object_get_property (ctx, arguments, "length");
  
  length = seed_value_to_uint (ctx, length_ref, exception);
  priv->n_args = length;
  priv->args = g_slice_alloc (length * sizeof (GType));
  
  for (i = 0; i < length; i++)
    {
      SeedValue type = seed_object_get_property_at_index (ctx, arguments, 
							  i, exception);
      priv->args[i] = seed_value_to_uint (ctx, type, exception);
    }
  priv->ret_val = seed_value_to_uint (ctx, ret_type_ref, exception);
  
  priv->signature_obj = sig;
  seed_value_protect (ctx, sig);

  return TRUE;
}

static gboolean
seed_ffi_set_signature (SeedContext ctx,
			  SeedObject this_object,
			  SeedString property_name,
			  SeedValue value,
			  SeedException *exception)
{
  seed_ffi_function_priv *priv = seed_object_get_private (this_object);
  
  if (priv->signature_obj)
    {
      seed_make_exception (ctx, exception, "FFIError",
			   "Can not reset signature of function once set");
      return FALSE;
    }
  else if (!seed_value_is_object (ctx, value))
    {
      seed_make_exception (ctx, exception, "FFIError",
			   "Signature must be an object");
      return FALSE;
    }
  else
    {
      if (!seed_ffi_build_signature (ctx, priv, (SeedObject) value, exception))
	return FALSE;
    }
  return TRUE;
}

static void
seed_ffi_function_finalize (SeedObject obj)
{
  seed_ffi_function_priv *priv = seed_object_get_private (obj);
  
  if (priv->signature_obj)
    {
      seed_value_unprotect (eng->context, priv->signature_obj);
      
      g_slice_free1 (priv->n_args * sizeof(GType), priv->args);
    }
  
  g_free (priv->name);
  seed_value_unprotect (eng->context, priv->module_obj);
}

static SeedObject
seed_ffi_make_function (SeedContext ctx, SeedObject module_obj, gpointer symbol, const gchar *name)
{
  seed_ffi_function_priv *priv = 
    g_slice_alloc0 (sizeof (seed_ffi_function_priv));
  
  priv->symbol = symbol;
  priv->module_obj = module_obj;
  priv->name = g_strdup (name);
  
  return seed_make_object (ctx, ffi_function_class, priv);
}

static SeedValue
seed_ffi_function_call (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			gsize argument_count,
			const SeedValue arguments[],
			SeedException *exception)
{
  ffi_type *rtype;
  void *rvalue;
  int n_args;
  ffi_type **atypes;
  void **args;
  int i;
  ffi_cif cif;

  seed_ffi_function_priv *priv = seed_object_get_private (this_object);
  
  if (argument_count != priv->n_args)
    {
      seed_make_exception (ctx, exception, "ArgumentError", "%s expected %d arguments got %d",
			   priv->name, priv->n_args, argument_count);
      return seed_make_null (ctx);
    }
}

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
  return seed_ffi_make_function (ctx, this_object, symbol, prop);
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

seed_static_value ffi_function_values [] = {
  {"signature", seed_ffi_get_signature, seed_ffi_set_signature, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {0, 0, 0, 0}
};

SeedObject
seed_module_init(SeedEngine *local_eng)
{
  SeedObject library_constructor;
  seed_class_definition ffi_library_def = seed_empty_class;
  seed_class_definition ffi_function_def = seed_empty_class;

  ffi_library_def.class_name = "FFILibrary";
  ffi_library_def.finalize = seed_ffi_library_finalize;
  ffi_library_def.get_property = seed_ffi_library_get_property;
  
  ffi_function_def.class_name = "FFIFunction";
  ffi_function_def.finalize = seed_ffi_function_finalize;
  ffi_function_def.static_values = ffi_function_values;
  ffi_function_def.call_as_function = seed_ffi_function_call;
  
  ffi_library_class = seed_create_class (&ffi_library_def);
  ffi_function_class = seed_create_class (&ffi_function_def);

  eng = local_eng;
  namespace_ref = seed_make_object (eng->context, NULL, NULL);
  
  library_constructor = seed_make_constructor (eng->context, ffi_library_class, seed_ffi_construct_library);
  seed_object_set_property (eng->context, namespace_ref, "Library", library_constructor);
  
  return namespace_ref;
}
