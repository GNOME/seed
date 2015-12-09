/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2 of
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

#include <gio/gio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "seed-private.h"

JSClassRef importer_class;
JSObjectRef importer;

JSClassRef gi_importer_class;
JSObjectRef gi_importer;
JSObjectRef gi_importer_versions;

JSObjectRef importer_search_path;

JSClassRef importer_dir_class;

GHashTable *gi_imports;
GHashTable *file_imports;

/*
 * Some high level documentation of the importer object.
 * > imports
 *   - An imports object is declared at the top level of every context
 *   > gi
 *      - The imports object has a special property named gi
 *      - gi.NameSpace represents the namespace object for a specific
 *        girepository namespace.
 *      - The first attempt to access gi.Foo creates the object, all
 *        subsequent attempts access the SAME object.
 *      > versions
 *        - the gi.versions object can be used to set versions of gi imports
 *        - gi.versions.NameSpace being set to "0.8" will cause NameSpace to
 *          require 0.8
 *   > searchPath
 *      - Should be an array, containing a list of paths to be searched for
 *        importing javascript files and native modules.
 *      - Default is set in Seed.js
 *      - If set to an invalid value, this will not become evident until the
 *        next time the imports object is used, at which point it will throw an
 *        exception
 *   > Accessing any other property (call it prop) will proceed as follows
 *      - Look in the search path for a file prop.*
 *      - If we find a file prop.*, see if it is a regular file or directory
 *      - If it is a directory, return a directory object that behaves as does
 *        the toplevel imports object, with the searchPath being exclusively
 *        that directory
 *      - If it is a file and ends in G_MODULE_SUFFIX, attempt to load it as a
 *        native module and return the module object.
 *      - If it is a file and does not end in G_MODULE_SUFFIX, evaluate it as a
 *        JavaScript file in a NEW global context, and return the global object
 *        for that context.
 */

/*
* Handle definition of toplevel functions in a namespace.
* i.e. Gtk.main
*/

static gboolean
seed_gi_importer_is_init (GIFunctionInfo * info)
{
  if (g_strcmp0 (g_base_info_get_name ((GIBaseInfo *) info), "init"))
    {
      return FALSE;
    }
  if (g_callable_info_get_n_args ((GICallableInfo *) info) != 2)
    return FALSE;

  return TRUE;
}

static void
seed_gi_importer_handle_function (JSContextRef ctx,
				  JSObjectRef namespace_ref,
				  GIFunctionInfo * info,
				  JSValueRef * exception)
{
  if (!seed_gi_importer_is_init (info))
    seed_gobject_define_property_from_function_info (ctx,
						     (GIFunctionInfo *) info,
						     namespace_ref, FALSE);
  else
    {
      JSObjectRef init_method;

      init_method = JSObjectMake (ctx, gobject_init_method_class,
				  g_base_info_ref ((GIBaseInfo *) info));
      seed_object_set_property (ctx, namespace_ref, "init", init_method);
    }
}

/*
 * Handle definition of enums in a namespace.
 * Each enum class gets an object containing
 * a value for each member, all in uppercase
 * i.e. Gtk.WindowType.NORMAL
 */
static void
seed_gi_importer_handle_enum (JSContextRef ctx,
			      JSObjectRef namespace_ref,
			      GIEnumInfo * info, JSValueRef * exception)
{
  JSObjectRef enum_class;
  guint num_vals, i, j;
  gsize name_len;
  gint value; // TODO: investigate what's up with the glong/gint mystery here
  gchar *name;
  GIValueInfo *val;

  enum_class = JSObjectMake (ctx, 0, 0);
  num_vals = g_enum_info_get_n_values (info);
  seed_object_set_property (ctx, namespace_ref,
			    g_base_info_get_name ((GIBaseInfo *) info),
			    enum_class);

  for (i = 0; i < num_vals; i++)
    {
      val = g_enum_info_get_value ((GIEnumInfo *) info, i);
      value = g_value_info_get_value (val);
      name = g_strdup (g_base_info_get_name ((GIBaseInfo *) val));
      name_len = strlen (name);
      JSValueRef value_ref;

      value_ref = JSValueMakeNumber (ctx, value);
      JSValueProtect (ctx, (JSValueRef) value_ref);

      for (j = 0; j < name_len; j++)
	{
	  if (name[j] == '-')
	    name[j] = '_';
	  name[j] = g_ascii_toupper (name[j]);
	}

      seed_object_set_property (ctx, enum_class, name, value_ref);

      g_free (name);
      g_base_info_unref ((GIBaseInfo *) val);

    }
}

/*
 * Handle setting up the prototype and constructor for a GObject type
 * Namespace.Type will be the constructor and Namespace.Type.prototype is
 * the prototype object. Namespace.Type.type will be the GType.
 */
static void
seed_gi_importer_handle_object (JSContextRef ctx,
				JSObjectRef namespace_ref,
				GIObjectInfo * info, JSValueRef * exception)
{
  GType type;
  JSClassRef class_ref;

  type = g_registered_type_info_get_g_type ((GIRegisteredTypeInfo *) info);

  if (type != 0)
    {
      GIFunctionInfo *finfo;
      GIFunctionInfoFlags flags;
      JSObjectRef constructor_ref;
      guint i, n_methods;

      class_ref = seed_gobject_get_class_for_gtype (ctx, type);

      constructor_ref =
	JSObjectMake (ctx, gobject_constructor_class, (gpointer) type);

      seed_object_set_property (ctx, constructor_ref,
				"type",
				seed_value_from_long (ctx, type, exception));
      n_methods = g_object_info_get_n_methods (info);
      for (i = 0; i < n_methods; i++)
	{
	  finfo = g_object_info_get_method (info, i);
	  flags = g_function_info_get_flags (finfo);
	  if (flags & GI_FUNCTION_IS_CONSTRUCTOR)
	    {
	      JSObjectRef constructor = JSObjectMake (ctx,
						      gobject_named_constructor_class,
						      finfo);
	      const gchar *fname =
		g_base_info_get_name ((GIBaseInfo *) finfo);
	      if (g_strrstr (fname, "new_") == fname) {
		      // To be compatible with gjs, we need to have a method with new_, too.
		      seed_object_set_property (ctx,
						constructor_ref, fname, constructor);
		      fname += 4;
	      }

	      else if (!g_strcmp0 (fname, "new")) {
		      // To be compatible with gjs, we need to have new as function, too.
		      seed_object_set_property (ctx,
						constructor_ref, fname, constructor);
		      fname = "c_new";
	      }

	      seed_object_set_property (ctx,
					constructor_ref, fname, constructor);
	    }
	  else if (!(flags & GI_FUNCTION_IS_METHOD))
	    {
	      seed_gobject_define_property_from_function_info
		(ctx, finfo, constructor_ref, FALSE);
	    }
	  else
	    {
	      g_base_info_unref ((GIBaseInfo *) finfo);
	    }
	}

      seed_object_set_property (ctx, namespace_ref,
				g_base_info_get_name ((GIBaseInfo *) info),
				constructor_ref);
      seed_object_set_property (ctx, constructor_ref,
				"prototype",
				seed_gobject_get_prototype_for_gtype (type));
    }
}

static void
seed_gi_importer_handle_iface (JSContextRef ctx,
			       JSObjectRef namespace_ref,
			       GIObjectInfo * info, JSValueRef * exception)
{
  GType type;

  type = g_registered_type_info_get_g_type ((GIRegisteredTypeInfo *) info);

  if (type != 0)
    {
      GIFunctionInfo *finfo;
      GIFunctionInfoFlags flags;
      JSObjectRef constructor_ref;
      guint i, n_methods;

      constructor_ref =
	JSObjectMake (ctx, gobject_constructor_class, (gpointer) type);

      seed_object_set_property (ctx, constructor_ref,
				"type",
				seed_value_from_long (ctx, type, exception));
      n_methods = g_interface_info_get_n_methods (info);
      for (i = 0; i < n_methods; i++)
	{
	  finfo = g_interface_info_get_method (info, i);
	  flags = g_function_info_get_flags (finfo);
	  if (!(flags & GI_FUNCTION_IS_METHOD))
	    {
	      seed_gobject_define_property_from_function_info
		(ctx, finfo, constructor_ref, FALSE);
	    }
	  else
	    {
	      g_base_info_unref ((GIBaseInfo *) finfo);
	    }
	}

      seed_object_set_property (ctx, namespace_ref,
				g_base_info_get_name ((GIBaseInfo *) info),
				constructor_ref);
      seed_object_set_property (ctx, constructor_ref,
				"prototype",
				seed_gobject_get_prototype_for_gtype (type));
    }
}

/*
 * Set up prototype and constructor for structs. Same semantics as objects except
 * for the type.
 */
static void
seed_gi_importer_handle_struct (JSContextRef ctx,
				JSObjectRef namespace_ref,
				GIStructInfo * info, JSValueRef * exception)
{
  JSObjectRef struct_ref;
  JSObjectRef proto;
  gint i, n_methods;
  GIFunctionInfo *finfo;

  struct_ref = JSObjectMake (ctx, seed_struct_constructor_class, info);
  g_base_info_ref (info);
  
  n_methods = g_struct_info_get_n_methods (info);

  for (i = 0; i < n_methods; i++)
    {
      GIFunctionInfoFlags flags;
      finfo = g_struct_info_get_method (info, i);

      flags = g_function_info_get_flags (finfo);

      if (flags & GI_FUNCTION_IS_CONSTRUCTOR)
	{
	  JSObjectRef constructor = JSObjectMake (ctx,
						  gobject_named_constructor_class,
						  finfo);
	  const gchar *fname = g_base_info_get_name ((GIBaseInfo *) finfo);
      if (g_strrstr (fname, "new_") == fname) {
	      // To be compatible with gjs, we need to have a method with new_, too.
	      seed_object_set_property (ctx, struct_ref, fname, constructor);
	      fname += 4;
      }

      else if (!g_strcmp0 (fname, "new")) {
	      // To be compatible with gjs, we need to have new as function, too.
	      seed_object_set_property (ctx, struct_ref, fname, constructor);
	      fname = "c_new";
      }

	  seed_object_set_property (ctx, struct_ref, fname, constructor);
	}
      else if (flags & GI_FUNCTION_IS_METHOD)
	g_base_info_unref ((GIBaseInfo *) finfo);
      else
	seed_gobject_define_property_from_function_info
	  (ctx, finfo, struct_ref, FALSE);
    }

  proto = seed_struct_prototype (ctx, (GIBaseInfo *) info);
  seed_object_set_property (ctx, struct_ref, "prototype", proto);

  seed_object_set_property (ctx, namespace_ref,
			    g_base_info_get_name ((GIBaseInfo *) info),
			    struct_ref);
}

static void
seed_gi_importer_handle_union (JSContextRef ctx,
			       JSObjectRef namespace_ref,
			       GIUnionInfo * info, JSValueRef * exception)
{
  JSObjectRef union_ref;
  JSObjectRef proto;
  guint i, n_methods;
  GIFunctionInfo *finfo;

  union_ref = JSObjectMake (ctx, seed_struct_constructor_class, info);
  g_base_info_ref (info);

  n_methods = g_union_info_get_n_methods (info);

  for (i = 0; i < n_methods; i++)
    {
      GIFunctionInfoFlags flags;
      finfo = g_union_info_get_method (info, i);

      flags = g_function_info_get_flags (finfo);

      if (flags & GI_FUNCTION_IS_METHOD)
	g_base_info_unref ((GIBaseInfo *) finfo);
      else
	seed_gobject_define_property_from_function_info
	  (ctx, finfo, union_ref, FALSE);
    }

  proto = seed_union_prototype (ctx, (GIBaseInfo *) info);
  seed_object_set_property (ctx, union_ref, "prototype", proto);

  seed_object_set_property (ctx, namespace_ref,
			    g_base_info_get_name ((GIBaseInfo *) info),
			    union_ref);
}

static void
seed_gi_importer_handle_callback (JSContextRef ctx,
				  JSObjectRef namespace_ref,
				  GICallbackInfo * info,
				  JSValueRef * exception)
{
  JSObjectRef callback_ref = JSObjectMake (ctx,
					   seed_callback_class,
					   info);
  seed_object_set_property (ctx, namespace_ref,
			    g_base_info_get_name ((GIBaseInfo *) info),
			    (JSValueRef) callback_ref);
}

/*
 * Define constants toplevel. Uses the casing as in the typelib
 */
static void
seed_gi_importer_handle_constant (JSContextRef ctx,
				  JSObjectRef namespace_ref,
				  GIConstantInfo * info,
				  JSValueRef * exception)
{
  GArgument argument;
  GITypeInfo *constant_type = g_constant_info_get_type (info);
  JSValueRef constant_value;

  g_constant_info_get_value (info, &argument);
  constant_value =
    seed_value_from_gi_argument (ctx, &argument, constant_type, exception);
  seed_object_set_property (ctx, namespace_ref,
			    g_base_info_get_name ((GIBaseInfo *) info),
			    constant_value);

  g_base_info_unref ((GIBaseInfo *) constant_type);
}

static gchar *
seed_gi_importer_get_version (JSContextRef ctx,
			      gchar * namespace, JSValueRef * exception)
{
  JSValueRef version_ref;
  gchar *version = NULL;

  version_ref =
    seed_object_get_property (ctx, gi_importer_versions, namespace);
  if (!JSValueIsUndefined (ctx, version_ref))
    version = seed_value_to_string (ctx, version_ref, exception);

  return version;
}

JSObjectRef
seed_gi_importer_do_namespace (JSContextRef ctx,
			       gchar * namespace, JSValueRef * exception)
{
  GIBaseInfo *info;
  JSObjectRef namespace_ref;
  GError *e = NULL;
  guint n, i;
  gchar *version = NULL;
  gchar *jsextension;
  JSStringRef extension_script;

  if (gi_imports == NULL)
    gi_imports = g_hash_table_new (g_str_hash, g_str_equal);

  if ((namespace_ref = g_hash_table_lookup (gi_imports, namespace)))
    {
      SEED_NOTE (IMPORTER, "Using existing namespace ref (%p) for %s",
		 namespace_ref, namespace);
      return namespace_ref;
    }

  if (gi_importer_versions != NULL)
    version = seed_gi_importer_get_version (ctx, namespace, exception);
  if (!g_irepository_require (NULL, namespace, version, 0, &e))
    {
      seed_make_exception_from_gerror (ctx, exception, e);
      g_error_free (e);
      g_free (version);
      return NULL;
    }
  if (version != NULL)
    g_free (version);

  n = g_irepository_get_n_infos (NULL, namespace);

  namespace_ref = JSObjectMake (ctx, NULL, NULL);
  SEED_NOTE (IMPORTER, "Constructing namespace ref (%p) for %s",
	     namespace_ref, namespace);

  JSValueProtect (ctx, namespace_ref);

  for (i = 0; i < n; i++)
    {
      GIInfoType info_type;

      info = g_irepository_get_info (NULL, namespace, i);
      info_type = g_base_info_get_type (info);

      switch (info_type)
	{
	case GI_INFO_TYPE_FUNCTION:
	  seed_gi_importer_handle_function (ctx, namespace_ref,
					    (GIFunctionInfo *) info,
					    exception);
	  break;
	case GI_INFO_TYPE_ENUM:
	case GI_INFO_TYPE_FLAGS:
	  seed_gi_importer_handle_enum (ctx, namespace_ref,
					(GIEnumInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_OBJECT:
	  seed_gi_importer_handle_object (ctx, namespace_ref,
					  (GIObjectInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_INTERFACE:
	  seed_gi_importer_handle_iface (ctx, namespace_ref,
					 (GIObjectInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_STRUCT:
	  seed_gi_importer_handle_struct (ctx, namespace_ref,
					  (GIStructInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_UNION:
	  seed_gi_importer_handle_union (ctx, namespace_ref,
					 (GIUnionInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_CALLBACK:
	  seed_gi_importer_handle_callback (ctx, namespace_ref,
					    (GICallbackInfo *) info,
					    exception);
	  break;
	case GI_INFO_TYPE_CONSTANT:
	  seed_gi_importer_handle_constant (ctx, namespace_ref,
					    (GIConstantInfo *) info,
					    exception);
	  break;
	default:
	  SEED_NOTE (IMPORTER, "Unhandled type %s for %s",
	             g_info_type_to_string (info_type),
	             g_base_info_get_name (info));
	  break;
	}
      g_base_info_unref (info);
    }

  g_hash_table_insert (gi_imports, g_strdup (namespace), namespace_ref);

  jsextension = g_strdup_printf ("imports.extensions.%s", namespace);
  extension_script = JSStringCreateWithUTF8CString (jsextension);
  JSEvaluateScript (ctx, extension_script, NULL, NULL, 0, exception);
  JSStringRelease (extension_script);
  g_free (jsextension);


  return namespace_ref;

}

static JSValueRef
seed_gi_importer_get_property (JSContextRef ctx,
			       JSObjectRef object,
			       JSStringRef property_name,
			       JSValueRef * exception)
{
  JSObjectRef ret;
  guint len;
  gchar *prop;

  len = JSStringGetMaximumUTF8CStringSize (property_name);
  prop = g_alloca (len * sizeof (gchar));
  JSStringGetUTF8CString (property_name, prop, len);

  SEED_NOTE (IMPORTER, "seed_gi_importer_get_property with %s", prop);

  if (!g_strcmp0 (prop, "versions"))
    return gi_importer_versions;
  // Nasty hack
  else if (!g_strcmp0 (prop, "toString"))
    return 0;
  if (!g_strcmp0 (prop, "valueOf"))	// HACK
    return NULL;


  ret = seed_gi_importer_do_namespace (ctx, prop, exception);
  SEED_NOTE (IMPORTER, "Result (%p) from attempting to import %s: %s",
	     ret, prop, seed_value_to_string (ctx, ret, exception));

  return ret;
}

static JSObjectRef
seed_make_importer_dir (JSContextRef ctx, gchar * path)
{
  gchar *init;
  JSObjectRef dir;

  dir = JSObjectMake (ctx, importer_dir_class, path);

  init = g_build_filename (path, "__init__.js", NULL);
  if (g_file_test (init, G_FILE_TEST_IS_REGULAR))
    {
      SeedScript *s;
      SEED_NOTE (IMPORTER, "Found __init__.js (%s)", path);

      s = seed_script_new_from_file (ctx, init);
      seed_evaluate (ctx, s, dir);
      seed_script_destroy (s);
    }

  g_free (init);
  return dir;
}

static void
seed_importer_free_search_path (GSList * path)
{
  GSList *walk = path;

  while (walk)
    {
      g_free (walk->data);
      walk = walk->next;
    }

  g_slist_free (path);
}


GSList *
seed_importer_get_search_path (JSContextRef ctx, JSValueRef * exception)
{
  GSList *path = NULL;
  gchar *entry;
  JSValueRef search_path_ref, length_ref;
  guint length, i;

  search_path_ref = seed_object_get_property (ctx, importer, "searchPath");
  if (!JSValueIsObject (ctx, search_path_ref))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Importer searchPath object is not an array");
      return NULL;
    }

  length_ref =
    seed_object_get_property (ctx, (JSObjectRef) search_path_ref, "length");
  length = seed_value_to_uint (ctx, length_ref, exception);

  for (i = 0; i < length; i++)
    {
      JSValueRef entry_ref;

      entry_ref =
	JSObjectGetPropertyAtIndex (ctx, (JSObjectRef) search_path_ref, i,
				    exception);
      entry = seed_value_to_string (ctx, entry_ref, exception);

      if (g_file_test (entry, G_FILE_TEST_EXISTS) == TRUE)
        path = g_slist_append (path, entry);
    }

  return path;
}

/* forward reference */
static JSObjectRef
seed_importer_handle_file (JSContextRef ctx,
			   const gchar * dir,
			   const gchar * file, 
			   JSValueRef * exception);



static JSObjectRef
seed_importer_handle_native_module (JSContextRef ctx,
				    const gchar * dir,
				    const gchar * prop,
				    JSValueRef * exception)
{
  GModule *module;
  JSObjectRef module_obj;
  SeedModuleInitCallback init;
  gchar *file_path = g_strconcat (dir, "/libseed_", prop, ".", G_MODULE_SUFFIX, NULL);

  SEED_NOTE (IMPORTER, "Trying native module: %s", file_path);

  if ((module_obj = g_hash_table_lookup (file_imports, file_path)))
    {
      SEED_NOTE (IMPORTER, "Using existing global");
      g_free (file_path);
      return module_obj;
    }

  module = g_module_open (file_path, 0);

  if (!module)
    {
      // Could be a better exception
      seed_make_exception (ctx, exception, "ModuleError",
			   "Error loading native module at %s: %s",
			   file_path, g_module_error ());
      g_free (file_path);

      return NULL;
    }
  g_module_symbol (module, "seed_module_init", (gpointer *) & init);
  module_obj = (*init) (eng);
  g_hash_table_insert (file_imports, file_path, module_obj);
  SEED_NOTE (IMPORTER, "Loaded native module");

  //protect module_obj since the GC won't find the module in our file_imports cache
  JSValueProtect (ctx, module_obj);
  
  file_path = g_strconcat ("libseed_", prop, ".js", NULL);
  seed_importer_handle_file (ctx, dir, file_path, exception);
  g_free (file_path);

  return module_obj;
}

static gchar *
seed_importer_canonicalize_path (gchar * path)
{
  GFile *file;
  gchar *absolute_path;

  file = g_file_new_for_path (path);
  absolute_path = g_file_get_path (file);
  g_object_unref (file);

  return absolute_path;
}

static JSObjectRef
seed_importer_handle_file (JSContextRef ctx,
			   const gchar * dir,
			   const gchar * file, JSValueRef * exception)
{
  JSContextRef nctx;
  JSValueRef js_file_dirname;
  JSObjectRef global, c_global;
  JSStringRef file_contents, file_name;
  gchar *contents, *walk, *file_path, *canonical, *absolute_path;
  char *normalized_path;

  file_path = g_build_filename (dir, file, NULL);
  canonical = seed_importer_canonicalize_path (file_path);
  SEED_NOTE (IMPORTER, "Trying to import file: %s", file_path);

  if ((global = g_hash_table_lookup (file_imports, canonical)))
    {
      SEED_NOTE (IMPORTER, "Using existing global");

      g_free (file_path);
      g_free (canonical);
      return global;
    }

  if (!g_file_test (file_path, G_FILE_TEST_IS_REGULAR))
    {
      if (g_file_test (file_path, G_FILE_TEST_IS_DIR))
	{
	  SEED_NOTE (IMPORTER, "File is directory");
	  return seed_make_importer_dir (ctx, file_path);
	}
      return NULL;
    }

  g_file_get_contents (file_path, &contents, 0, NULL);
  walk = contents;
  if (*walk == '#')
    {
      while (*walk != '\n')
	walk++;
      walk++;
    }
  walk = g_strdup (walk);
  g_free (contents);

  file_contents = JSStringCreateWithUTF8CString (walk);
  file_name = JSStringCreateWithUTF8CString (canonical);

  nctx = JSGlobalContextCreateInGroup (context_group, 0);
  seed_prepare_global_context (nctx);

  global = JSContextGetGlobalObject (nctx);
  c_global = JSContextGetGlobalObject (ctx);
  JSValueProtect (eng->context, global);

  absolute_path = g_path_get_dirname (file_path);
  if (!g_path_is_absolute (absolute_path))
    {
      g_free (absolute_path);
      absolute_path = g_build_filename (g_get_current_dir (),
					g_path_get_dirname (file_path), NULL);
    }

  normalized_path = realpath (absolute_path, NULL);

  js_file_dirname = seed_value_from_string (ctx, normalized_path, NULL);

  seed_object_set_property (nctx, global, "__script_path__", js_file_dirname);

  g_hash_table_insert (file_imports, canonical, global);
  g_free (file_path);
  g_free (absolute_path);
  g_free (normalized_path);

  JSEvaluateScript (nctx, file_contents, NULL, file_name, 0, exception);

  // Does leak...but it's a debug statement.
  SEED_NOTE (IMPORTER, "Evaluated file, exception: %s",
	     *exception ? seed_exception_to_string (ctx,
						    *exception) : "(null)");

  JSGlobalContextRelease ((JSGlobalContextRef) nctx);

  JSStringRelease (file_contents);
  JSStringRelease (file_name);
  g_free (walk);

  return global;
}

static JSObjectRef
seed_importer_search_dirs (JSContextRef ctx, GSList *path, gchar *prop, JSValueRef *exception)
{
    GSList *walk;
    JSObjectRef ret, global;
    JSValueRef script_path_prop;
    gchar *prop_as_lib, *prop_as_js, *script_path;

    prop_as_lib = g_strconcat ("libseed_", prop, ".", G_MODULE_SUFFIX, NULL);
    prop_as_js = g_strconcat (prop, ".js", NULL);

    // get the current script_path      
    global = JSContextGetGlobalObject (ctx);
    script_path_prop = seed_object_get_property (ctx, global, "__script_path__");
    if (script_path_prop==NULL || JSValueIsUndefined (ctx, script_path_prop))
        script_path = NULL;
    else
        script_path = seed_value_to_string (ctx, script_path_prop, exception);

    ret = NULL;
    walk = path;
    while (walk) {
        gchar *test_path = walk->data;
        gchar *file_path;
        
        // replace '.' with current script_path if not null
        if(script_path && !g_strcmp0(".",test_path))
            test_path = script_path;

        // check if prop is a file or dir (imports['foo.js'] or imports.mydir)
        file_path = g_build_filename (test_path, prop, NULL);
        if (g_file_test (file_path, G_FILE_TEST_IS_REGULAR | G_FILE_TEST_IS_DIR)) {
            ret = seed_importer_handle_file (ctx, test_path, prop, exception);
            g_free (file_path);
            break;
        }
        g_free (file_path);

        // check if prop is file ending with '.js'
        file_path = g_build_filename (test_path, prop_as_js, NULL);
        if (g_file_test (file_path, G_FILE_TEST_IS_REGULAR)) {
            ret = seed_importer_handle_file (ctx, test_path, prop_as_js, exception);
            g_free (file_path);
            break;
        }
        g_free (file_path);
      
        // check if file is native module
        file_path = g_build_filename (test_path, prop_as_lib, NULL);
        if (g_file_test (file_path, G_FILE_TEST_IS_REGULAR)) {
            ret = seed_importer_handle_native_module (ctx, test_path, prop, exception);
            g_free (file_path);
            break;
        }
        g_free (file_path);

        walk = walk->next;
    }

    g_free (prop_as_lib);
    g_free (prop_as_js);
    g_free (script_path);
    
    return ret;
}

static JSObjectRef
seed_importer_search (JSContextRef ctx, gchar *prop, JSValueRef *exception)
{
    JSObjectRef ret = NULL;
    GSList *path = seed_importer_get_search_path (ctx, exception);
    ret = seed_importer_search_dirs (ctx, path, prop, exception);
    seed_importer_free_search_path (path);
    return ret;
}

static JSValueRef
seed_importer_get_property (JSContextRef ctx,
			    JSObjectRef object,
			    JSStringRef property_name, JSValueRef * exception)
{
  JSValueRef ret;
  guint len;
  gchar *prop;

  len = JSStringGetMaximumUTF8CStringSize (property_name);
  prop = g_alloca (len * sizeof (gchar));
  JSStringGetUTF8CString (property_name, prop, len);

  if (!g_strcmp0 (prop, "gi"))
    return gi_importer;
  if (!g_strcmp0 (prop, "searchPath"))
    return NULL;
  if (!g_strcmp0 (prop, "toString"))	// HACK
    return NULL;

  ret = seed_importer_search (ctx, prop, exception);

  return ret;
}

static JSValueRef
seed_importer_dir_get_property (JSContextRef ctx,
                               JSObjectRef object,
                               JSStringRef property_name,
                               JSValueRef * exception)
{
    guint len;
    gchar *prop;
    GSList path;

    path.data = JSObjectGetPrivate (object);
    path.next = NULL;

    len = JSStringGetMaximumUTF8CStringSize (property_name);
    prop = g_alloca (len * sizeof (gchar));
    JSStringGetUTF8CString (property_name, prop, len);

    /* These prevent print(imports.somefile) running "somefile/toString.js" 
       Which is more than a little unexpected.. */

    if (!g_strcmp0 (prop, "toString"))
        return NULL;
    if (!g_strcmp0 (prop, "valueOf"))
        return NULL;
    
    return seed_importer_search_dirs(ctx, &path, prop, exception);
}

static void
seed_importer_dir_finalize (JSObjectRef dir)
{
  gchar *dir_path = (gchar *) JSObjectGetPrivate (dir);
  g_free (dir_path);
}

void
seed_importer_add_global (JSObjectRef global, gchar * name)
{
  JSValueProtect (eng->context, global);
  g_hash_table_insert (file_imports, seed_importer_canonicalize_path (name),
		       global);
}

static void
seed_importer_dir_enumerate_properties (JSContextRef ctx,
					JSObjectRef object,
					JSPropertyNameAccumulatorRef
					propertyNames)
{
  const gchar *entry;
  GDir *dir;
  GError *e = NULL;
  gchar *path = JSObjectGetPrivate (object);


  dir = g_dir_open (path, 0, &e);
  if (e)
    {
      SEED_NOTE (IMPORTER,
		 "Error in g_dir_open in seed_importer_enumerate_dir_properties: %s",
		 e->message);
      g_error_free (e);
      // Not much we can do here.
      return;
    }

  while ((entry = g_dir_read_name (dir)))
    {
      JSStringRef jname;

      jname = JSStringCreateWithUTF8CString (entry);
      JSPropertyNameAccumulatorAddName (propertyNames, jname);
      JSStringRelease (jname);
    }
  g_dir_close (dir);
}

JSObjectRef
seed_importer_construct_dir (JSContextRef ctx,
			     JSObjectRef constructor,
			     gsize argumentCount,
			     const JSValueRef arguments[],
			     JSValueRef * exception)
{
  gchar *path;
  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Directory constructor expects 1 argument");
      return (JSObjectRef) JSValueMakeUndefined (ctx);
    }
  path = seed_value_to_string (ctx, arguments[0], exception);

  if (!g_file_test (path, G_FILE_TEST_IS_DIR))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Path (%s) is not a directory", path);
      g_free (path);
      return (JSObjectRef) JSValueMakeUndefined (ctx);
    }

  return seed_make_importer_dir (ctx, path);
}

void
seed_importer_set_search_path (JSContextRef ctx, gchar ** search_path)
{
  JSObjectRef imports, array;
  JSValueRef *array_elem;
  guint length = g_strv_length (search_path), i;

  array_elem = g_alloca (length * sizeof (array_elem));
  imports =
    (JSObjectRef) seed_object_get_property (ctx,
					    JSContextGetGlobalObject (ctx),
					    "imports");

  for (i = 0; i < length; i++)
    {
      array_elem[i] = seed_value_from_string (ctx, search_path[i], NULL);
    }

  array = JSObjectMakeArray (ctx, length, array_elem, NULL);
  seed_object_set_property (ctx, imports, "searchPath", array);
}

void
seed_importer_add_search_path (JSContextRef ctx, gchar *search_path)
{
  GSList *paths, *l;
  GPtrArray *tmp;

  tmp = g_ptr_array_new ();

  paths = seed_importer_get_search_path (ctx, NULL);
  for (l = paths; l != NULL; l = g_slist_next (l))
    {
      g_ptr_array_add (tmp, l->data);
    }

  g_ptr_array_add (tmp, search_path);
  g_ptr_array_add (tmp, NULL);

  seed_importer_set_search_path (ctx, (gchar **) tmp->pdata);

  g_ptr_array_unref (tmp);
  seed_importer_free_search_path (paths);
}

JSClassDefinition importer_class_def = {
  0,				/* Version, always 0 */
  0,
  "importer",			/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,				/* Initialize */
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  seed_importer_get_property,	/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition gi_importer_class_def = {
  0,				/* Version, always 0 */
  0,
  "gi_importer",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,				/* Initialize */
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  seed_gi_importer_get_property,	/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition importer_dir_class_def = {
  0,				/* Version, always 0 */
  0,
  "importer_dir",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,				/* Initialize */
  seed_importer_dir_finalize,	/* Finalize */
  NULL,				/* Has Property */
  seed_importer_dir_get_property,	/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  seed_importer_dir_enumerate_properties,	/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

void
seed_initialize_importer (JSContextRef ctx, JSObjectRef global)
{
  JSObjectRef dir_constructor;

  importer_class = JSClassCreate (&importer_class_def);
  importer = JSObjectMake (ctx, importer_class, NULL);

  gi_importer_class = JSClassCreate (&gi_importer_class_def);
  gi_importer = JSObjectMake (ctx, gi_importer_class, NULL);
  gi_importer_versions = JSObjectMake (ctx, NULL, NULL);

  JSValueProtect (ctx, gi_importer);
  JSValueProtect (ctx, gi_importer_versions);

  importer_dir_class = JSClassCreate (&importer_dir_class_def);

  gi_imports = g_hash_table_new (g_str_hash, g_str_equal);
  file_imports = g_hash_table_new (g_str_hash, g_str_equal);

  /* Passing nonnull for class requires a webkit fix that most people wont have yet. It also has minimal benefit */
  //  dir_constructor = JSObjectMakeConstructor (ctx, importer_dir_class, seed_importer_construct_dir);
  dir_constructor =
    JSObjectMakeConstructor (ctx, NULL, seed_importer_construct_dir);
  seed_object_set_property (ctx, importer, "Directory", dir_constructor);

  seed_object_set_property (ctx, global, "imports", importer);
}
