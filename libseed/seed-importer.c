#include <string.h>

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
      
      if (flags & GI_FUNCTION_IS_METHOD)
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
  
  if (namespace_ref = g_hash_table_lookup (gi_imports, namespace))
    {
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
  guint len;
  gchar *prop;
  
  len = JSStringGetMaximumUTF8CStringSize (property_name);
  prop = g_alloca (len * sizeof (gchar));
  JSStringGetUTF8CString (property_name, prop, len);
  
  if (!strcmp(prop, "versions"))
    return gi_importer_versions;
  // Nasty hack
  else if (!strcmp(prop, "toString"))
    return 0;

  return seed_gi_importer_do_namespace (ctx, prop, exception);
  
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

  if (module_obj = g_hash_table_lookup (file_imports, file_path))
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
  
  g_free (file_path);
      
  return module_obj;
}

static JSObjectRef
seed_importer_handle_file (JSContextRef ctx,
			   const gchar *dir,
			   const gchar *file,
			   JSValueRef *exception)
{
  JSContextRef nctx;
  JSObjectRef global;
  JSStringRef file_contents, file_name;
  gchar *contents, *walk, *file_path;
  
  file_path = g_strconcat (dir, "/", file, NULL);
  
  if (global = g_hash_table_lookup (file_imports, file_path))
    {
      g_free (file_path);
      return global;
    }

  if (!g_file_test (file_path, G_FILE_TEST_IS_REGULAR))
    {
      if (g_file_test (file_path, G_FILE_TEST_IS_DIR))
	{
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
  global = JSContextGetGlobalObject (nctx);
  seed_object_set_property (ctx, global, "imports", importer);

  JSEvaluateScript (nctx, file_contents, NULL, file_name, 0, exception);


  g_hash_table_insert (file_imports, file_path, global);
  
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
      while (entry = g_dir_read_name(dir))
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
  while (entry = g_dir_read_name (dir))
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
  seed_gi_importer_get_property,	/* Get Property */
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
  NULL,				/* Finalize */
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

void seed_initialize_importer(JSContextRef ctx,
			      JSObjectRef global)
{
  importer_class = JSClassCreate (&importer_class_def);
  importer = JSObjectMake (ctx, importer_class, NULL);
  
  gi_importer_class = JSClassCreate (&gi_importer_class_def);
  gi_importer = JSObjectMake (ctx, gi_importer_class, NULL);
  gi_importer_versions = JSObjectMake (ctx, NULL, NULL);
  
  importer_dir_class = JSClassCreate (&importer_dir_class_def);
  
  gi_imports = g_hash_table_new (g_str_hash, g_str_equal);
  file_imports = g_hash_table_new (g_str_hash, g_str_equal);
  
  seed_object_set_property (ctx, global, "imports", importer);
}
