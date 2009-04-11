#include <string.h>

#include "seed-private.h"

JSClassRef importer_class;
JSObjectRef importer;

JSClassRef gi_importer_class;
JSObjectRef gi_importer;
JSObjectRef gi_importer_versions;

GHashTable *gi_imports;

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
      seed_object_set_property (ctx, namespace_ref,
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

static JSObjectRef
seed_gi_importer_do_namespace (JSContextRef ctx,
			       gchar *namespace,
			       JSValueRef *exception)
{
  GIBaseInfo *info;
  JSObjectRef namespace_ref;
  GError *e = NULL;
  guint n, i;
  
  if (namespace_ref = g_hash_table_lookup (gi_imports, namespace))
    {
      return namespace_ref;
    }
  
  // TODO: Versions.
  if (!g_irepository_require (NULL, namespace,
			      NULL, 0, &e))
    {
      seed_make_exception_from_gerror (ctx, exception, e);
      return NULL;
    }
  
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

static JSValueRef
seed_importer_get_property (JSContextRef ctx,
			    JSObjectRef object,
			    JSStringRef property_name, 
			    JSValueRef *exception)
{
  guint len;
  gchar *prop;
  
  len = JSStringGetMaximumUTF8CStringSize (property_name);
  prop = g_alloca (len * sizeof (gchar));
  JSStringGetUTF8CString (property_name, prop, len);
  
  if (!strcmp (prop, "gi"))
    return gi_importer;
  
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
  NULL,				/* Set Property */
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

void seed_initialize_importer(JSContextRef ctx,
			      JSObjectRef global)
{
  importer_class = JSClassCreate (&importer_class_def);
  importer = JSObjectMake (ctx, importer_class, NULL);
  
  gi_importer_class = JSClassCreate (&gi_importer_class_def);
  gi_importer = JSObjectMake (ctx, gi_importer_class, NULL);
  gi_importer_versions = JSObjectMake (ctx, gi_importer_class, NULL);
  
  gi_imports = g_hash_table_new (g_str_hash, g_str_equal);
  
  seed_object_set_property (ctx, global, "imports", importer);
}
