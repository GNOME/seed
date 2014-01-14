/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2 of
 * the License, or (at your option) any later version.
 * * Seed is distributed in the hope that it will be useful,
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

typedef struct _seed_ffi_library_priv {
  GModule *mod;
  GHashTable *symbols;
} seed_ffi_library_priv;

static ffi_type *
gtype_to_ffi_type (SeedContext ctx, 
		   SeedValue value,
		   GType otype,
		   GArgument *garg,
		   gpointer *arg,
		   SeedException *exception)
{
  ffi_type *return_type;
  GType type = g_type_fundamental (otype);
  g_assert (type != G_TYPE_INVALID);

  switch (type)
    {
    case G_TYPE_BOOLEAN:
    case G_TYPE_CHAR:
    case G_TYPE_INT:
      return_type = &ffi_type_sint;
      garg->v_int = seed_value_to_int (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_UCHAR:
    case G_TYPE_UINT:
      return_type = &ffi_type_uint;
      garg->v_uint = seed_value_to_uint (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_STRING:
      return_type = &ffi_type_pointer;
      garg->v_pointer = seed_value_to_string (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_OBJECT:
      //    case G_TYPE_BOXED:
    case G_TYPE_POINTER:
      return_type = &ffi_type_pointer;
      garg->v_pointer = seed_pointer_get_pointer (ctx, value);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_FLOAT:
      return_type = &ffi_type_float;
      garg->v_float = seed_value_to_float (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_DOUBLE:
      return_type = &ffi_type_double;
      garg->v_double = seed_value_to_double (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_LONG:
      return_type = &ffi_type_slong;
      garg->v_uint = seed_value_to_uint (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_ULONG:
      return_type = &ffi_type_ulong;
      garg->v_ulong = seed_value_to_ulong (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_INT64:
      return_type = &ffi_type_sint64;
      garg->v_int64 = seed_value_to_int64 (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    case G_TYPE_UINT64:
      return_type = &ffi_type_uint64;
      garg->v_uint64 = seed_value_to_uint64 (ctx, value, exception);
      *arg = (gpointer)garg;
      break;
    default:
      g_warning ("Unsupported fundamental in type: %s", g_type_name (type));
      return_type = &ffi_type_pointer;
      garg->v_pointer = NULL;
      *arg = (garg->v_pointer);
      break;
    }
  return return_type;
}

static ffi_type *
return_type_to_ffi_type (GType otype)
{
  GType type = g_type_fundamental (otype);
  g_assert (type != G_TYPE_INVALID);

  switch (type)
    {
    case G_TYPE_BOOLEAN:
    case G_TYPE_CHAR:
    case G_TYPE_INT:
      return &ffi_type_sint;
      break;
    case G_TYPE_UCHAR:
    case G_TYPE_UINT:
      return &ffi_type_uint;
      break;
    case G_TYPE_STRING:
      return &ffi_type_pointer;
      break;
    case G_TYPE_OBJECT:
      //    case G_TYPE_BOXED:
    case G_TYPE_POINTER:
      return &ffi_type_pointer;
      break;
    case G_TYPE_FLOAT:
      return &ffi_type_float;
      break;
    case G_TYPE_DOUBLE:
      return &ffi_type_double;
      break;
    case G_TYPE_LONG:
      return &ffi_type_slong;
      break;
    case G_TYPE_ULONG:
      return &ffi_type_ulong;
      break;
    case G_TYPE_INT64:
      return &ffi_type_sint64;
      break;
    case G_TYPE_UINT64:
      return &ffi_type_uint64;
      break;
    default:
      g_warning ("Unsupported fundamental out type: %s", g_type_name (type));
      return &ffi_type_pointer;
      break;
    }
}

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
      priv->args[i] = seed_value_to_int (ctx, type, exception);
    }
  priv->ret_val = seed_value_to_int (ctx, ret_type_ref, exception);
  
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
  g_slice_free1 (sizeof(seed_ffi_function_priv), priv); 

  seed_value_unprotect (eng->context, priv->module_obj);
}

static SeedObject
seed_ffi_make_function (SeedContext ctx, SeedObject module_obj, gpointer symbol, const gchar *name, GHashTable *symbols)
{
  SeedValue ret;
  seed_ffi_function_priv *priv = 
    g_slice_alloc0 (sizeof (seed_ffi_function_priv));
  
  priv->symbol = symbol;
  priv->module_obj = module_obj;
  priv->name = g_strdup (name);
  
  ret = seed_make_object (ctx, ffi_function_class, priv);
  seed_value_protect (ctx, ret);

  g_hash_table_insert (symbols, g_strdup (name), ret);
  return ret;
}

static SeedValue
value_from_ffi_type (SeedContext ctx, 
		     GType otype,
		     GArgument *value, 
		     SeedException *exception)
{
  switch (g_type_fundamental (otype))
    {
    case G_TYPE_INT:
      return seed_value_from_int (ctx, value->v_int, exception);
      break;
    case G_TYPE_FLOAT:
      return seed_value_from_float (ctx, value->v_float, exception);
      break;
    case G_TYPE_DOUBLE:
      return seed_value_from_double (ctx, value->v_double, exception);
      break;
    case G_TYPE_BOOLEAN:
      return seed_value_from_boolean (ctx, value->v_boolean, exception);
      break;
    case G_TYPE_STRING:
      return seed_value_from_string (ctx, value->v_pointer, exception);
      break;
    case G_TYPE_CHAR:
      return seed_value_from_char (ctx, value->v_int, exception);
      break;
    case G_TYPE_UCHAR:
      return seed_value_from_uchar (ctx, value->v_uint, exception);
      break;
    case G_TYPE_UINT:
      return seed_value_from_uint (ctx, value->v_uint, exception);
      break;
    case G_TYPE_POINTER:
      return seed_make_pointer (ctx, value->v_pointer);
      break;
    case G_TYPE_LONG:
      return seed_value_from_long (ctx, value->v_long, exception);
      break;
    case G_TYPE_ULONG:
      return seed_value_from_ulong (ctx, value->v_ulong, exception);
      break;
    case G_TYPE_INT64:
      return seed_value_from_int64 (ctx, value->v_int64, exception);
      break;
    case G_TYPE_UINT64:
      return seed_value_from_uint64 (ctx, value->v_uint64, exception);
      break;
    case G_TYPE_NONE:
      return seed_make_null (ctx);
    default:
      g_warning ("Unsupported fundamental type in value_from_ffi_type: %s",
		 g_type_name(g_type_fundamental (otype)));
      return seed_make_null (ctx);
    }
}

static SeedValue
seed_ffi_function_call (SeedContext ctx,
			SeedObject function,
			SeedObject this_object,
			gsize argument_count,
			const SeedValue arguments[],
			SeedException *exception)
{
  GArgument rvalue;
  GArgument *gargs;
  ffi_type *rtype;
  ffi_type **atypes;
  gpointer *args;
  int i;
  ffi_cif cif;

  seed_ffi_function_priv *priv = seed_object_get_private (function);
  
  if (argument_count != priv->n_args)
    {
      seed_make_exception (ctx, exception, "ArgumentError", "%s expected %d arguments got %zd",
			   priv->name, priv->n_args, argument_count);
      return seed_make_null (ctx);
    }
  atypes = g_alloca (sizeof (ffi_type *) * (argument_count));
  args = g_alloca (sizeof (gpointer) * (argument_count));
  gargs = g_alloca (sizeof (GArgument) * (argument_count));

  for (i = 0; i < argument_count; i++)
    {
      atypes[i] = gtype_to_ffi_type (ctx, arguments[i], priv->args[i], &(gargs[i]), &args[i],exception);
    }
  rtype = return_type_to_ffi_type (priv->ret_val);
		   
  if (ffi_prep_cif (&cif, FFI_DEFAULT_ABI, argument_count, rtype, atypes) != FFI_OK)
    g_assert_not_reached();
  
  ffi_call (&cif, priv->symbol, &rvalue, args);
  
  return value_from_ffi_type (ctx, priv->ret_val, &rvalue, exception);
}

static SeedValue
seed_ffi_library_get_property (SeedContext ctx,
			       SeedObject this_object,
			       SeedString property_name,
			       SeedException *exception)
{
  SeedValue ret;
  GModule *mod;
  gchar *prop;
  gsize len = seed_string_get_maximum_size (property_name);
  gpointer symbol;
  seed_ffi_library_priv *priv;
  
  prop = g_alloca (len);
  seed_string_to_utf8_buffer (property_name, prop, len);
  
  priv = seed_object_get_private (this_object);
  mod = priv->mod;
  
  if ((ret = g_hash_table_lookup (priv->symbols, prop)))
    return ret;

  if (!g_module_symbol (mod, prop, &symbol))
    {
      return NULL;
    }
  return seed_ffi_make_function (ctx, this_object, symbol, prop, priv->symbols);
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
  seed_ffi_library_priv *priv;
  
  if (argument_count != 1 && argument_count != 0)
    {
      seed_make_exception (ctx, exception, 
			   "ArgumentError", 
			   "ffi.Library constructor expects 1 argument (filename, or none to use NULL GModule), got %zd", 
			   argument_count);
      return seed_make_null (ctx);
    }
  if (argument_count == 1)
    filename = seed_value_to_string (ctx, arguments[0], exception);
  else
    filename = NULL;
  
  mod = g_module_open (filename, G_MODULE_BIND_LOCAL | G_MODULE_BIND_LAZY);
  if (!mod)
    {
      seed_make_exception (ctx, exception, "GModuleError",
			   "Opening module (%s) failed with: %s",
			   filename, g_module_error ());
      g_free (filename);
      return seed_make_null (ctx);
    }
  
  priv = g_slice_alloc (sizeof (seed_ffi_library_priv));
  priv->mod = mod;
  
  // TODO: Value destroy function.
  priv->symbols = g_hash_table_new_full (g_str_hash, g_str_equal, 
					 g_free, NULL);
  
  ret = seed_make_object (ctx, ffi_library_class, priv);
  
  g_free (filename);
  
  return ret;
}

static void
seed_ffi_library_finalize (SeedObject obj)
{
  seed_ffi_library_priv *priv;
  priv = seed_object_get_private (obj);
  
  g_module_close (priv->mod);
}

seed_static_value ffi_function_values [] = {
  {"signature", seed_ffi_get_signature, seed_ffi_set_signature, SEED_PROPERTY_ATTRIBUTE_DONT_DELETE},
  {NULL, NULL, NULL, 0}
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
