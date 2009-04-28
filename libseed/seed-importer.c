#include <string.h>

#include <gio/gio.h>

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
static void
seed_gi_importer_handle_function (JSContextRef ctx,
				  JSObjectRef namespace_ref,
				  GIFunctionInfo *info,
				  JSValueRef *exception)
{
  seed_gobject_define_property_from_function_info (ctx, (GIFunctionInfo *) info,
						   namespace_ref, FALSE);
  g_base_info_ref ((GIBaseInfo *) info);
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
			      GIEnumInfo *info,
			      JSValueRef *exception)
{
  JSObjectRef enum_class;
  gint num_vals, j;
  
  enum_class = JSObjectMake (ctx, 0, 0);
  num_vals = g_enum_info_get_n_values (info);
  seed_object_set_property (ctx, namespace_ref, 
			    g_base_info_get_name ((GIBaseInfo *)info), 
			    enum_class);

  for (j = 0; j < num_vals; j++)
    {
      GIValueInfo *val =
	g_enum_info_get_value ((GIEnumInfo *) info, j);
      gint value = g_value_info_get_value (val);
      gchar *name =
	g_strdup (g_base_info_get_name ((GIBaseInfo *) val));
      gint name_len = strlen (name);
      gint j;
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
				GIObjectInfo *info,
				JSValueRef *exception)
{
  GType type;
  JSClassRef class_ref;
  
  type = 
    g_registered_type_info_get_g_type ((GIRegisteredTypeInfo *) info);
  
  if (type != 0)
    {
      GIFunctionInfo *finfo;
      GIFunctionInfoFlags flags;
      JSObjectRef constructor_ref;
      guint i, n_methods;
      
      class_ref = seed_gobject_get_class_for_gtype (ctx, type);
      
      constructor_ref =
	JSObjectMake (ctx,
		      gobject_constructor_class, 
		      (gpointer) type);
		      
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
	      if (strstr (fname, "new_") == fname)
		fname += 4;
	      else if (!strcmp (fname, "new"))
		fname = "c_new";
	      
	      seed_object_set_property (ctx,
					constructor_ref,
					fname, constructor);
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

/*
 * Set up prototype and constructor for structs. Same semantics as objects except
 * for the type.
 */
static void
seed_gi_importer_handle_struct (JSContextRef ctx,
				JSObjectRef namespace_ref,
				GIStructInfo *info,
				JSValueRef *exception)
{
  JSObjectRef struct_ref;
  JSObjectRef proto;
  gint i, n_methods;
  GIFunctionInfo *finfo;
  
  struct_ref =
    JSObjectMake (ctx, seed_struct_constructor_class, info);
  
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
	  const gchar *fname = 
	    g_base_info_get_name ((GIBaseInfo *) finfo);
	  if (g_str_has_prefix (fname, "new_"))
	    fname += 4;
	  else if (!strcmp (fname, "new"))
	    fname = "c_new";
	  
	  seed_object_set_property (ctx, struct_ref, fname, constructor);
	}
      else if (flags & GI_FUNCTION_IS_METHOD)
	g_base_info_unref ((GIBaseInfo *) finfo);
      else
	seed_gobject_define_property_from_function_info
	  (ctx, finfo, struct_ref, FALSE);
    }
  
  proto = seed_struct_prototype (ctx, (GIBaseInfo *)info);
  seed_object_set_property (ctx, struct_ref, "prototype", proto);
  
  seed_object_set_property (ctx, namespace_ref, g_base_info_get_name ((GIBaseInfo *)info), struct_ref);
}

static void
seed_gi_importer_handle_union (JSContextRef ctx,
			       JSObjectRef namespace_ref,
			       GIUnionInfo *info,
			       JSValueRef *exception)
{
  JSObjectRef union_ref;
  JSObjectRef proto;
  gint i, n_methods;
  GIFunctionInfo *finfo;
  
  union_ref =
    JSObjectMake (ctx, seed_struct_constructor_class, info);
  
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
  
  proto = seed_union_prototype (ctx, (GIBaseInfo *)info);
  seed_object_set_property (ctx, union_ref, "prototype", proto);
  
  seed_object_set_property (ctx, namespace_ref, g_base_info_get_name ((GIBaseInfo *)info), union_ref);
}

static void
seed_gi_importer_handle_callback (JSContextRef ctx,
				  JSObjectRef namespace_ref,
				  GICallbackInfo *info,
				  JSValueRef *exception)
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
				  GIConstantInfo *info,
				  JSValueRef *exception)
{
  GArgument argument;
  GITypeInfo *constant_type =
    g_constant_info_get_type (info);
  JSValueRef constant_value;
  
  g_constant_info_get_value (info, &argument);
  constant_value =
    seed_gi_argument_make_js (ctx, &argument,
			      constant_type, exception);
  seed_object_set_property (ctx, namespace_ref,
			    g_base_info_get_name ((GIBaseInfo *) info),
			    constant_value);
  
  g_base_info_unref ((GIBaseInfo *) constant_type);
}

static gchar *
seed_gi_importer_get_version (JSContextRef ctx,
			      gchar *namespace,
			      JSValueRef *exception)
{
  JSValueRef version_ref;
  gchar *version = NULL;
  
  version_ref = seed_object_get_property (ctx, gi_importer_versions, namespace);
  if (!JSValueIsUndefined(ctx, version_ref))
    version = seed_value_to_string (ctx, version_ref, exception);
  
  return version;  
}

static JSObjectRef
seed_gi_importer_do_namespace (JSContextRef ctx,
			       gchar *namespace,
			       JSValueRef *exception)
{
  GIBaseInfo *info;
  JSObjectRef namespace_ref;
  GError *e = NULL;
  guint n, i;
  gchar *version;
  gchar *jsextension;
  JSStringRef extension_script;
  
  if ((namespace_ref = g_hash_table_lookup (gi_imports, namespace)))
    {
      SEED_NOTE (IMPORTER, "Using existing namespace ref (%p) for %s",
		 namespace_ref, namespace);
      return namespace_ref;
    }
  
  version = seed_gi_importer_get_version (ctx, namespace, exception);
  if (!g_irepository_require (NULL, namespace,
			      version, 0, &e))
    {
      seed_make_exception_from_gerror (ctx, exception, e);
      g_error_free (e);
      return NULL;
    }
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
	  seed_gi_importer_handle_function (ctx, namespace_ref, (GIFunctionInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_ENUM:
	case GI_INFO_TYPE_FLAGS:
	  seed_gi_importer_handle_enum (ctx, namespace_ref, (GIEnumInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_OBJECT:
	  seed_gi_importer_handle_object (ctx, namespace_ref, (GIObjectInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_STRUCT:
	  seed_gi_importer_handle_struct (ctx, namespace_ref, (GIStructInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_UNION:
	  seed_gi_importer_handle_union (ctx, namespace_ref, (GIUnionInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_CALLBACK:
	  seed_gi_importer_handle_callback (ctx, namespace_ref, (GICallbackInfo *) info, exception);
	  break;
	case GI_INFO_TYPE_CONSTANT:
	  seed_gi_importer_handle_constant (ctx, namespace_ref, (GIConstantInfo *) info, exception);
	  break;	  
	default:
	  break;
	}
      g_base_info_unref (info);
    }
  
  g_hash_table_insert (gi_imports, g_strdup(namespace), namespace_ref);
  
  jsextension = g_strdup_printf ("imports.extensions.%s",
				 namespace);
  extension_script = JSStringCreateWithUTF8CString (jsextension);
  JSEvaluateScript (ctx, extension_script, NULL, NULL, 0, NULL);
  JSStringRelease (extension_script);
  g_free (jsextension);

  
  return namespace_ref;
  
} 

static JSValueRef
seed_gi_importer_get_property (JSContextRef ctx,
			       JSObjectRef object,
			       JSStringRef property_name, 
			       JSValueRef *exception)
{
  JSObjectRef ret;
  guint len;
  gchar *prop;
  
  len = JSStringGetMaximumUTF8CStringSize (property_name);
  prop = g_alloca (len * sizeof (gchar));
  JSStringGetUTF8CString (property_name, prop, len);
  
  SEED_NOTE (IMPORTER, "seed_gi_importer_get_property with %s", prop);
  
  if (!strcmp(prop, "versions"))
    return gi_importer_versions;
  // Nasty hack
  else if (!strcmp(prop, "toString"))
    return 0;
  if (!strcmp (prop, "valueOf")) // HACK
    return NULL;


  ret = seed_gi_importer_do_namespace (ctx, prop, exception);
  SEED_NOTE (IMPORTER, "Result (%p) from attempting to import %s: %s",
	     ret, prop, seed_value_to_string (ctx, ret, exception));

  return ret;  
}

static void
seed_importer_free_search_path (GSList *path)
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
seed_importer_get_search_path (JSContextRef ctx,
			       JSValueRef *exception)
{
  GSList *path = NULL;
  JSValueRef search_path_ref, length_ref;
  guint length, i;
  
  search_path_ref = seed_object_get_property (ctx, importer, "searchPath");
  if (!JSValueIsObject(ctx, search_path_ref))
    {
      seed_make_exception (ctx, exception, "ArgumentError", "Importer searchPath object is not an array");
      return NULL;
    }
  
  length_ref = seed_object_get_property (ctx, (JSObjectRef) search_path_ref, "length");
  length = seed_value_to_uint (ctx, length_ref, exception);
  
  for (i = 0; i < length; i++)
    {
      JSValueRef entry_ref;
      gchar *entry;
      
      entry_ref = JSObjectGetPropertyAtIndex (ctx, (JSObjectRef) search_path_ref, i, exception);
      entry = seed_value_to_string (ctx, entry_ref, exception);
      
      path = g_slist_append (path, entry);
    }
  
  return path;
}

static JSObjectRef
seed_importer_handle_native_module (JSContextRef ctx,
				    const gchar *dir,
				    const gchar *file,
				    JSValueRef *exception)

{
  GModule *module;
  JSObjectRef module_obj;
  SeedModuleInitCallback init;
  gchar *file_path = g_strconcat (dir, "/", file, NULL);
  
  SEED_NOTE (IMPORTER, "Trying native module: %s", file_path);

  if ((module_obj = g_hash_table_lookup (file_imports, file_path)))
    {
      g_free (file_path);
      return module_obj;
    }
  
  module = g_module_open (file_path, G_MODULE_BIND_LAZY);
  
  if (!module)
    {
      gchar *mes = g_strdup_printf ("Error loading native module at %s: %s",
				    file_path,
				    g_module_error());
      // Could be a better exception
      seed_make_exception (ctx, exception, "ModuleError",
			   mes);
      g_free (file_path);
      g_free (mes);
      
      return NULL;
    }
  g_module_symbol (module, "seed_module_init", (gpointer *) &init);
  module_obj = (*init) (eng);
  g_hash_table_insert (file_imports, file_path, module_obj);
  SEED_NOTE (IMPORTER, "Loaded native module");
  
  g_free (file_path);
      
  return module_obj;
}

static gchar *
seed_importer_canonicalize_path (gchar *path)
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
			   const gchar *dir,
			   const gchar *file,
			   JSValueRef *exception)
{
  JSContextRef nctx;
  JSObjectRef global, c_global;
  JSStringRef file_contents, file_name;
  gchar *contents, *walk, *file_path, *canonical;
  
  file_path = g_strconcat (dir, "/", file, NULL);
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
	  return JSObjectMake (ctx, importer_dir_class, file_path);
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
  file_name = JSStringCreateWithUTF8CString (file);
  
  nctx = JSGlobalContextCreateInGroup (context_group, 0);
  seed_prepare_global_context (nctx);
  
  global = JSContextGetGlobalObject (nctx);
  c_global = JSContextGetGlobalObject (ctx);
  JSValueProtect (eng->context, global);
  
  g_hash_table_insert (file_imports, canonical, global);
  g_free (file_path);

  JSEvaluateScript (nctx, file_contents, c_global, file_name, 0, exception);

  // Does leak...but it's a debug statement.
  SEED_NOTE (IMPORTER, "Evaluated file, exception: %s", 
	     *exception ? seed_exception_to_string (ctx, *exception) : "(null)");
  
  JSGlobalContextRelease ((JSGlobalContextRef) nctx);
  
  JSStringRelease (file_contents);
  JSStringRelease (file_name);
  g_free (walk);
  
  return global;
}

static JSObjectRef
seed_importer_search (JSContextRef ctx,
		      gchar *prop,
		      JSValueRef *exception)
{
  GSList *path, *walk;
  gchar *prop_as_lib = g_strconcat ("lib", prop, NULL);
  
  path = seed_importer_get_search_path (ctx, exception);
  
  walk = path;
  while (walk)
    {
      GError *e = NULL;
      GDir *dir;
      const gchar *entry;

      dir = g_dir_open ((gchar *)walk->data, 0, &e);
      if (e)
	{
	  g_error_free (e);
	  
	  walk = walk->next;
	  continue;
	}
      while ((entry = g_dir_read_name(dir)))
	{
	  guint i;
	  gchar *mentry = g_strdup (entry);

	  for (i = 0; i < strlen (mentry); i++)
	    {
	      if (mentry[i] == '.')
		mentry[i] = '\0';
	    }
	  if (!strcmp (mentry, prop))
	    {
	      JSObjectRef ret;
	      
	      ret = seed_importer_handle_file (ctx, walk->data, entry, exception);

	      g_dir_close (dir);
	      g_free (mentry);
	      g_free (prop_as_lib);
	      seed_importer_free_search_path (path);

	      return ret;
	    }
	  else if (g_str_has_prefix (mentry, prop_as_lib) && g_str_has_suffix (entry, G_MODULE_SUFFIX))
	    {
	      JSObjectRef ret;
	      
	      ret = seed_importer_handle_native_module (ctx, walk->data, entry, exception);
	      g_dir_close (dir);
	      g_free (mentry);
	      g_free (prop_as_lib);
	      seed_importer_free_search_path (path);

	      return ret;
	    }
	  
	  g_free (mentry);
	}
      g_dir_close (dir);
      
      walk = walk->next;
    }
  
  seed_importer_free_search_path (path);
  g_free (prop_as_lib);
  return NULL;
}

static JSValueRef
seed_importer_get_property (JSContextRef ctx,
			    JSObjectRef object,
			    JSStringRef property_name, 
			    JSValueRef *exception)
{
  JSValueRef ret;
  guint len;
  gchar *prop;
  
  len = JSStringGetMaximumUTF8CStringSize (property_name);
  prop = g_alloca (len * sizeof (gchar));
  JSStringGetUTF8CString (property_name, prop, len);
  
  if (!strcmp (prop, "gi"))
    return gi_importer;
  if (!strcmp (prop, "searchPath"))
    return NULL;
  if (!strcmp (prop, "toString")) // HACK
    return NULL;
  
  ret = seed_importer_search (ctx, prop, exception);
  
  return ret;
}

static JSValueRef
seed_importer_dir_get_property (JSContextRef ctx,
				JSObjectRef object,
				JSStringRef property_name, 
				JSValueRef *exception)
{
  GError *e = NULL;
  GDir *dir;
  guint len;
  const gchar *entry;
  gchar *dir_path, *prop;

  
  dir_path = JSObjectGetPrivate (object);
  
  len = JSStringGetMaximumUTF8CStringSize (property_name);
  prop = g_alloca (len * sizeof (gchar));
  JSStringGetUTF8CString (property_name, prop, len);
  
  // TODO: GError
  dir = g_dir_open (dir_path, 0, &e);
  if (e)
    {
      seed_make_exception_from_gerror (ctx, exception, e);
      g_error_free (e);
      
      return NULL;
    }
  while ((entry = g_dir_read_name (dir)))
    {
      gchar *mentry = g_strdup (entry);
      guint i;
      
      for (i = 0; i < strlen (mentry); i++)
	{
	  if (mentry[i] == '.')
	    mentry[i] = '\0';
	}

      if (!strcmp (mentry, prop))
	{
	  JSObjectRef ret;
	  
	  ret = seed_importer_handle_file (ctx, dir_path, entry, exception);
	  g_dir_close (dir);
	  g_free (mentry);
	  
	  return ret;
	}
      g_free (mentry);
    }
  g_dir_close (dir);
  
  return NULL;
}

static void
seed_importer_dir_finalize (JSObjectRef dir)
{
  gchar *dir_path = (gchar *) JSObjectGetPrivate (dir);
  g_free (dir_path);
}


JSClassDefinition importer_class_def = {
  0,				/* Version, always 0 */
  0,
  "importer",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,                         /* Initialize */
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  seed_importer_get_property,	/* Get Property */
  NULL,                         /* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,	/* Call As Constructor */
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
  NULL,                         /* Initialize */
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  seed_gi_importer_get_property,/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,	/* Call As Constructor */
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
  NULL,                         /* Initialize */
  seed_importer_dir_finalize,	/* Finalize */
  NULL,				/* Has Property */
  seed_importer_dir_get_property,	/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,	/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

void
seed_importer_add_global(JSObjectRef global,
			 gchar *name)
{
  JSValueProtect (eng->context, global);
  g_hash_table_insert (file_imports, seed_importer_canonicalize_path (name), global);
}

void 
seed_importer_set_search_path(JSContextRef ctx,
			      gchar **search_path)
{
  JSObjectRef imports, array;
  JSValueRef *array_elem;
  guint length = g_strv_length (search_path), i;
  
  array_elem = g_alloca (length * sizeof (array_elem));
  imports = (JSObjectRef) seed_object_get_property (ctx, JSContextGetGlobalObject (ctx), "imports");
  
  for (i = 0; i < length; i++)
    {
      array_elem[i] = seed_value_from_string (ctx, search_path[i], NULL);
    }
  
  array = JSObjectMakeArray (ctx, length, array_elem, NULL);
  seed_object_set_property (ctx, imports, "searchPath", array);  
  
}


void seed_initialize_importer(JSContextRef ctx,
			      JSObjectRef global)
{
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
					     
  seed_object_set_property (ctx, global, "imports", importer);
}
