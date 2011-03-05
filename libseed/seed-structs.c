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

#include "seed-private.h"
JSClassRef seed_struct_class = 0;
JSClassRef seed_union_class = 0;
JSClassRef seed_pointer_class = 0;
JSClassRef seed_boxed_class = 0;

typedef struct _seed_struct_privates
{
  gpointer pointer;
  GIBaseInfo *info;

  gboolean free_pointer;
  gboolean slice_alloc;
  gsize size;
} seed_struct_privates;

GHashTable *struct_prototype_hash = NULL;
GHashTable *union_prototype_hash = NULL;

static void
seed_pointer_finalize (JSObjectRef object)
{
  seed_struct_privates *priv =
    (seed_struct_privates *) JSObjectGetPrivate (object);

  SEED_NOTE (STRUCTS, "Finalizing seed_pointer object %p. with "
	     "priv->free_pointer = %d with type: %s, size: %zu",
	     priv->pointer,
	     priv->free_pointer,
	     priv->info ? g_base_info_get_name (priv->info) : "[generic]",
	     priv->size);

  if (priv->free_pointer)
    {
      if (priv->slice_alloc)
	g_slice_free1 (priv->size, priv->pointer);
      else
	g_free (priv->pointer);
    }
  if (priv->info)
    g_base_info_unref (priv->info);

  g_slice_free1 (sizeof (seed_struct_privates), priv);
}

static void
seed_boxed_finalize (JSObjectRef object)
{
  seed_struct_privates *priv =
    (seed_struct_privates *) JSObjectGetPrivate (object);
  GType type;
  GIRegisteredTypeInfo *info =
    (GIRegisteredTypeInfo *) g_base_info_get_type (priv->info);

  SEED_NOTE (STRUCTS, "Finalizing boxed object of type %s \n",
	     g_base_info_get_name (priv->info));

  type = g_registered_type_info_get_g_type (info);
  g_base_info_unref ((GIBaseInfo *) info);

  g_boxed_free (type, priv->pointer);

}

GIFieldInfo *
seed_union_find_field (GIUnionInfo * info, gchar * field_name)
{
  gint n, i;
  GIFieldInfo *field;

  n = g_union_info_get_n_fields (info);
  for (i = 0; i < n; i++)
    {
      const gchar *name;

      field = g_union_info_get_field (info, i);
      name = g_base_info_get_name ((GIBaseInfo *) field);
      if (!g_strcmp0 (name, field_name))
	return field;
      else
	g_base_info_unref ((GIBaseInfo *) field);
    }

  return NULL;
}

GIFieldInfo *
seed_struct_find_field (GIStructInfo * info, gchar * field_name)
{
  gint n, i;
  const gchar *name;
  GIFieldInfo *field;

  n = g_struct_info_get_n_fields (info);
  for (i = 0; i < n; i++)
    {

      field = g_struct_info_get_field (info, i);
      name = g_base_info_get_name ((GIBaseInfo *) field);
      if (!g_strcmp0 (name, field_name))
	return field;
      else
	g_base_info_unref ((GIBaseInfo *) field);
    }

  return NULL;
}

JSValueRef
seed_field_get_value (JSContextRef ctx,
		      gpointer object,
		      GIFieldInfo * field, JSValueRef * exception)
{
  GITypeInfo *field_type;
  GIBaseInfo *interface;
  GArgument field_value;
  JSValueRef ret = JSValueMakeNull (ctx);
  gint offset;

  field_type = g_field_info_get_type (field);
  if (!g_field_info_get_field (field, object, &field_value))
    {
      GITypeTag tag;

      tag = g_type_info_get_tag (field_type);
      if (tag == GI_TYPE_TAG_INTERFACE)
	{
	  interface = g_type_info_get_interface (field_type);
	  offset = g_field_info_get_offset (field);

	  g_base_info_unref ((GIBaseInfo *) field_type);
	  switch (g_base_info_get_type (interface))
	    {
	    case GI_INFO_TYPE_STRUCT:
	      ret = seed_make_struct (ctx, (object + offset), interface);
	      break;
	    case GI_INFO_TYPE_UNION:
	      ret = seed_make_union (ctx, (object + offset), interface);
	      break;
	    case GI_INFO_TYPE_BOXED:
	      ret = seed_make_boxed (ctx, (object + offset), interface);
	      break;
	    default:
	      break;
	    }
	  g_base_info_unref (interface);

	  return ret;
	}

      return JSValueMakeNull (ctx);
    }

  // Maybe need to release argument.
  ret = seed_value_from_gi_argument (ctx, &field_value, field_type, exception);
  if (field_type)
    g_base_info_unref ((GIBaseInfo *) field_type);
  return ret;
}

static JSValueRef
seed_union_get_property (JSContextRef context,
			 JSObjectRef object,
			 JSStringRef property_name, JSValueRef * exception)
{
  gchar *cproperty_name;
  gsize length;
  seed_struct_privates *priv = JSObjectGetPrivate (object);
  GIFieldInfo *field = 0;
  JSValueRef ret;

  length = JSStringGetMaximumUTF8CStringSize (property_name);
  cproperty_name = g_alloca (length * sizeof (gchar));
  JSStringGetUTF8CString (property_name, cproperty_name, length);

  SEED_NOTE (STRUCTS, "Getting property on union of type: %s "
	     "with name %s \n",
	     g_base_info_get_name (priv->info), cproperty_name);

  field = seed_union_find_field ((GIUnionInfo *) priv->info, cproperty_name);
  if (!field)
    {
      return 0;
    }

  ret = seed_field_get_value (context, priv->pointer, field, exception);

  g_base_info_unref ((GIBaseInfo *) field);

  return ret;
}

static bool
seed_union_set_property (JSContextRef context,
			 JSObjectRef object,
			 JSStringRef property_name,
			 JSValueRef value, JSValueRef * exception)
{
  gsize length;
  GArgument field_value;
  GIFieldInfo *field;
  gchar *cproperty_name;
  GITypeInfo *field_type;
  seed_struct_privates *priv =
    (seed_struct_privates *) JSObjectGetPrivate (object);
  gboolean ret;

  length = JSStringGetMaximumUTF8CStringSize (property_name);
  cproperty_name = g_alloca (length * sizeof (gchar));
  JSStringGetUTF8CString (property_name, cproperty_name, length);

  SEED_NOTE (STRUCTS, "Setting property on union of type: %s  "
	     "with name %s \n",
	     g_base_info_get_name (priv->info), cproperty_name);

  field = seed_union_find_field ((GIUnionInfo *) priv->info, cproperty_name);

  if (!field)
    {
      return FALSE;
    }

  field_type = g_field_info_get_type (field);

  seed_value_to_gi_argument (context, value, field_type, &field_value, exception);
  ret = g_field_info_set_field (field, priv->pointer, &field_value);

  g_base_info_unref ((GIBaseInfo *) field_type);
  g_base_info_unref ((GIBaseInfo *) field);

  return TRUE;
}

static bool
seed_struct_set_property (JSContextRef context,
			  JSObjectRef object,
			  JSStringRef property_name,
			  JSValueRef value, JSValueRef * exception)
{
  gsize length;
  GArgument field_value;
  GIFieldInfo *field;
  gchar *cproperty_name;
  GITypeInfo *field_type;
  seed_struct_privates *priv =
    (seed_struct_privates *) JSObjectGetPrivate (object);
  gboolean ret;

  length = JSStringGetMaximumUTF8CStringSize (property_name);
  cproperty_name = g_alloca (length * sizeof (gchar));
  JSStringGetUTF8CString (property_name, cproperty_name, length);

  SEED_NOTE (STRUCTS, "Setting property on struct of type: %s  "
	     "with name %s \n",
	     g_base_info_get_name (priv->info), cproperty_name);

  field =
    seed_struct_find_field ((GIStructInfo *) priv->info, cproperty_name);

  if (!field)
    {
      return FALSE;
    }

  field_type = g_field_info_get_type (field);

  seed_value_to_gi_argument (context, value, field_type, &field_value, exception);
  ret = g_field_info_set_field (field, priv->pointer, &field_value);

  if (!ret) 
    g_warning("Setting property failed on struct of type: %s  "
			"with name %s \n",
			g_base_info_get_name (priv->info), cproperty_name);
  
  g_base_info_unref ((GIBaseInfo *) field_type);
  g_base_info_unref ((GIBaseInfo *) field);

  return TRUE;
}

static JSValueRef
seed_struct_get_property (JSContextRef context,
			  JSObjectRef object,
			  JSStringRef property_name, JSValueRef * exception)
{
  gchar *cproperty_name;
  gsize length;
  seed_struct_privates *priv = JSObjectGetPrivate (object);
  GIFieldInfo *field = NULL;
  JSValueRef ret;

  length = JSStringGetMaximumUTF8CStringSize (property_name);
  cproperty_name = g_alloca (length * sizeof (gchar));
  JSStringGetUTF8CString (property_name, cproperty_name, length);

  SEED_NOTE (STRUCTS, "Getting property on struct of type: %s  "
	     "with name %s \n",
	     g_base_info_get_name (priv->info), cproperty_name);

  // for a gvalue, it has a special property 'value' (read-only)
  GType gtype = g_registered_type_info_get_g_type ((GIRegisteredTypeInfo *) priv->info);

  if (g_type_is_a (gtype, G_TYPE_VALUE) && !g_strcmp0 (cproperty_name, "value")) 
    {
      return seed_value_from_gvalue ( context, ( GValue *) priv->pointer,  exception);
 
    }

  field =
    seed_struct_find_field ((GIStructInfo *) priv->info, cproperty_name);

  if (!field)
    {
      return NULL;
    }

  ret = seed_field_get_value (context, priv->pointer, field, exception);

  g_base_info_unref ((GIBaseInfo *) field);

  return ret;
}

static void
seed_enumerate_structlike_properties (JSContextRef ctx,
				      JSObjectRef object,
				      JSPropertyNameAccumulatorRef
				      propertyNames)
{
  GIFieldInfo *field;
  gint i, n;
  guchar type = 0;
  seed_struct_privates *priv =
    (seed_struct_privates *) JSObjectGetPrivate (object);
  GIBaseInfo *info = priv->info;

  if (!info)
    return;

  if (JSValueIsObjectOfClass (ctx, object, seed_struct_class))
    type = 1;
  else if (JSValueIsObjectOfClass (ctx, object, seed_union_class))
    type = 2;
  else
    g_assert_not_reached ();

  (type == 1) ?
    (n = g_struct_info_get_n_fields ((GIStructInfo *) info)) :
    (n = g_union_info_get_n_fields ((GIUnionInfo *) info));

  for (i = 0; i < n; i++)
    {
      JSStringRef jname;

      (type == 1) ?
	(field = g_struct_info_get_field ((GIStructInfo *) info, i)) :
	(field = g_union_info_get_field ((GIUnionInfo *) info, i));

      jname =
	JSStringCreateWithUTF8CString (g_base_info_get_name
				       ((GIBaseInfo *) field));

      g_base_info_unref ((GIBaseInfo *) field);
      JSPropertyNameAccumulatorAddName (propertyNames, jname);

      JSStringRelease (jname);
    }
}

JSClassDefinition seed_pointer_def = {
  0,				/* Version, always 0 */
  0,
  "seed_pointer",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  seed_pointer_finalize,
  NULL,				/* Has Property */
  0,
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition seed_struct_def = {
  0,				/* Version, always 0 */
  kJSClassAttributeNoAutomaticPrototype,
  "seed_struct",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  NULL,
  NULL,				/* Has Property */
  seed_struct_get_property,
  seed_struct_set_property,	/* Set Property */
  NULL,				/* Delete Property */
  seed_enumerate_structlike_properties,	/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition seed_union_def = {
  0,				/* Version, always 0 */
  kJSClassAttributeNoAutomaticPrototype,
  "seed_union",			/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  NULL,
  NULL,				/* Has Property */
  seed_union_get_property,
  seed_union_set_property,	/* Set Property */
  NULL,				/* Delete Property */
  seed_enumerate_structlike_properties,	/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition seed_boxed_def = {
  0,				/* Version, always 0 */
  kJSClassAttributeNoAutomaticPrototype,
  "seed_boxed",			/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  seed_boxed_finalize,
  NULL,				/* Has Property */
  NULL,
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

gpointer
seed_pointer_get_pointer (JSContextRef ctx, JSValueRef pointer)
{
  if (JSValueIsObjectOfClass (ctx, pointer, seed_pointer_class))
    {
      seed_struct_privates *priv = JSObjectGetPrivate ((JSObjectRef) pointer);
      return priv->pointer;
    }
  return NULL;
}

void
seed_pointer_set_free (JSContextRef ctx,
		       JSValueRef pointer, gboolean free_pointer)
{
  if (JSValueIsObjectOfClass (ctx, pointer, seed_pointer_class))
    {
      seed_struct_privates *priv = JSObjectGetPrivate ((JSObjectRef) pointer);
      priv->free_pointer = free_pointer;
    }
}

static void
seed_pointer_set_slice (JSContextRef ctx,
			JSValueRef pointer, gboolean free_pointer, gsize size)
{
  seed_struct_privates *priv = JSObjectGetPrivate ((JSObjectRef) pointer);
  priv->slice_alloc = free_pointer;
  priv->size = size;
}

JSObjectRef
seed_make_pointer (JSContextRef ctx, gpointer pointer)
{
  seed_struct_privates *priv = g_slice_alloc (sizeof (seed_struct_privates));
  priv->pointer = pointer;
  priv->info = 0;
  priv->free_pointer = FALSE;

  return JSObjectMake (ctx, seed_pointer_class, priv);
}

JSObjectRef
seed_union_prototype (JSContextRef ctx, GIBaseInfo * info)
{
  JSObjectRef proto;
  const gchar *namespace, *name;
  gchar *key;
  gint n_methods, i;
  GIFunctionInfo *finfo;

  name = g_base_info_get_name (info);
  namespace = g_base_info_get_namespace (info);
  key = g_strjoin (NULL, namespace, name, NULL);

  proto = (JSObjectRef) g_hash_table_lookup (union_prototype_hash, key);
  
  if (!proto)
    {
      proto = JSObjectMake (ctx, 0, 0);
      JSValueProtect (eng->context, proto);

      n_methods = g_union_info_get_n_methods ((GIUnionInfo *) info);
      for (i = 0; i < n_methods; i++)
	{
	  finfo = g_union_info_get_method ((GIUnionInfo *) info, i);

	  seed_gobject_define_property_from_function_info (ctx,
							   (GIFunctionInfo *)
							   finfo, proto,
							   TRUE);

	  g_base_info_unref ((GIBaseInfo *) finfo);
	}

      g_hash_table_insert (union_prototype_hash, key, proto);
    }
  else
    {
      g_free (key);
    }

  return proto;
}

JSObjectRef
seed_make_union (JSContextRef ctx, gpointer younion, GIBaseInfo * info)
{
  JSObjectRef object;

  if (younion == NULL)
    {
      return (JSObjectRef) JSValueMakeNull (ctx);
    }

  seed_struct_privates *priv = g_slice_alloc (sizeof (seed_struct_privates));

  priv->pointer = younion;
  priv->info = info ? g_base_info_ref (info) : 0;
  priv->free_pointer = FALSE;

  object = JSObjectMake (ctx, seed_union_class, priv);

  if (info)
    {
      JSObjectRef proto = seed_union_prototype (ctx, info);
      if (proto)
	JSObjectSetPrototype (ctx, object, proto);
      else
	g_assert_not_reached ();
    }

  return object;
}

JSObjectRef
seed_make_boxed (JSContextRef ctx, gpointer boxed, GIBaseInfo * info)
{
  JSObjectRef object;
  seed_struct_privates *priv = g_slice_alloc (sizeof (seed_struct_privates));

  priv->info = info ? g_base_info_ref (info) : 0;
  priv->pointer = boxed;
  // Boxed finalize handler handles freeing.
  priv->free_pointer = FALSE;

  object = JSObjectMake (ctx, seed_boxed_class, priv);

  // FIXME: Instance methods?

  return object;
}

JSObjectRef
seed_struct_prototype (JSContextRef ctx, GIBaseInfo * info)
{
  JSObjectRef proto;
  const gchar *namespace, *name;
  gchar *key;
  gint n_methods, i;

  name = g_base_info_get_name (info);
  namespace = g_base_info_get_namespace (info);
  key = g_strjoin (NULL, namespace, name, NULL);

  proto = (JSObjectRef) g_hash_table_lookup (struct_prototype_hash, key);

  if (!proto)
    {
      proto = JSObjectMake (ctx, 0, 0);
      JSValueProtect (eng->context, proto);

      n_methods = g_struct_info_get_n_methods ((GIStructInfo *) info);
      for (i = 0; i < n_methods; i++)
	{
	  GIFunctionInfo *finfo;

	  finfo = g_struct_info_get_method ((GIStructInfo *) info, i);

	  seed_gobject_define_property_from_function_info (ctx,
							   (GIFunctionInfo *)
							   finfo, proto,
							   TRUE);

	  g_base_info_unref ((GIBaseInfo *) finfo);
	}

      g_hash_table_insert (struct_prototype_hash, key, proto);
    }
  else
    {
      g_free (key);
    }

  return proto;
}

JSObjectRef
seed_make_struct (JSContextRef ctx, gpointer strukt, GIBaseInfo * info)
{
  JSObjectRef object, proto;

  if (strukt == NULL)
    {
      return (JSObjectRef) JSValueMakeNull (ctx);
    }

  seed_struct_privates *priv = g_slice_alloc (sizeof (seed_struct_privates));

  priv->info = info ? g_base_info_ref (info) : 0;
  priv->pointer = strukt;
  priv->free_pointer = FALSE;

  object = JSObjectMake (ctx, seed_struct_class, priv);
  // Examine cases where struct is being used without info.
  if (info)
    {
      proto = seed_struct_prototype (ctx, info);
      if (proto)
	JSObjectSetPrototype (ctx, object, proto);
      else
	g_assert_not_reached ();
    }

  return object;
}

void
seed_structs_init (void)
{
  seed_pointer_class = JSClassCreate (&seed_pointer_def);
  seed_struct_def.parentClass = seed_pointer_class;
  seed_struct_class = JSClassCreate (&seed_struct_def);
  seed_union_def.parentClass = seed_pointer_class;
  seed_union_class = JSClassCreate (&seed_union_def);
  seed_boxed_def.parentClass = seed_struct_class;
  seed_boxed_class = JSClassCreate (&seed_boxed_def);

  struct_prototype_hash = g_hash_table_new (g_str_hash, g_str_equal);
  union_prototype_hash = g_hash_table_new (g_str_hash, g_str_equal);
}

JSObjectRef
seed_construct_struct_type_with_parameters (JSContextRef ctx,
					    GIBaseInfo * info,
					    JSObjectRef parameters,
					    JSValueRef * exception)
{
  gsize size = 0;
  gpointer object;
  GIInfoType type = g_base_info_get_type (info);
  JSObjectRef ret;
  gboolean set_ret;
  gint nparams, i = 0;
  gsize length;
  GIFieldInfo *field = 0;
  JSPropertyNameArrayRef jsprops;
  JSStringRef jsprop_name;
  JSValueRef jsprop_value;
  GArgument field_value;
  gchar *prop_name;
  GITypeInfo *field_type;

  if (type == GI_INFO_TYPE_STRUCT)
    {
      GType gtype = g_registered_type_info_get_g_type ((GIRegisteredTypeInfo *) info);
      if (g_type_is_a (gtype, G_TYPE_VALUE)) 
        {
          GValue *gval = g_slice_alloc0 (sizeof (GValue));
          if (!parameters) 
            {
              seed_make_exception (ctx, exception, "ArgumentError",  "Missing Type in GValue constructor");
              return (JSObjectRef) JSValueMakeNull (ctx);
            }
          SEED_NOTE (CONSTRUCTION, "Created a GValue  struct");
          seed_value_to_gvalue (ctx,   parameters , 0,  gval, exception);
          ret = seed_make_struct (ctx, (gpointer)gval, info);
          return ret;
        }

      size = g_struct_info_get_size ((GIStructInfo *) info);
    }
  else
    {
      size = g_union_info_get_size ((GIUnionInfo *) info);
    }
  if (!size)
    {
      g_critical ("Struct/union of type: %s has size 0 in introspection data. Please check GIR",
		  g_base_info_get_name (info));
      g_assert(size);
    }
  object = g_slice_alloc0 (size);

  SEED_NOTE (CONSTRUCTION,
	     "Constructing struct/union of type: %s. Size: %zu \n",
	     g_base_info_get_name (info), size);

  if (type == GI_INFO_TYPE_STRUCT)
    ret = seed_make_struct (ctx, object, info);
  else
    ret = seed_make_union (ctx, object, info);

  seed_pointer_set_free (ctx, ret, TRUE);
  seed_pointer_set_slice (ctx, ret, TRUE, size);

  if (!parameters)
    return ret;

  jsprops = JSObjectCopyPropertyNames (ctx, (JSObjectRef) parameters);
  nparams = JSPropertyNameArrayGetCount (jsprops);

  while (i < nparams)
    {
      jsprop_name = JSPropertyNameArrayGetNameAtIndex (jsprops, i);

      length = JSStringGetMaximumUTF8CStringSize (jsprop_name);
      prop_name = g_alloca (length * sizeof (gchar));
      JSStringGetUTF8CString (jsprop_name, prop_name, length);

      if (type == GI_INFO_TYPE_STRUCT)
	field = seed_struct_find_field ((GIStructInfo *) info, prop_name);
      else
	field = seed_union_find_field ((GIUnionInfo *) info, prop_name);
      if (!field)
	{
	  seed_make_exception (ctx, exception, "PropertyError",
			       "Invalid property for construction: %s",
			       prop_name);

	  JSPropertyNameArrayRelease (jsprops);
	  return (JSObjectRef) JSValueMakeNull (ctx);
	}
      field_type = g_field_info_get_type (field);

      jsprop_value = JSObjectGetProperty (ctx,
					  (JSObjectRef) parameters,
					  jsprop_name, NULL);

      seed_value_to_gi_argument (ctx, jsprop_value, field_type, &field_value,
				 exception);
      set_ret = g_field_info_set_field (field, object, &field_value);

	  if (!set_ret) 
		g_warning("Constructor setting property failed on struct of type: %s "
			"with name %s \n",
			g_base_info_get_name (info), prop_name);
  
      g_base_info_unref ((GIBaseInfo *) field_type);
      g_base_info_unref ((GIBaseInfo *) field);

      i++;
    }
  JSPropertyNameArrayRelease (jsprops);

  return ret;
}
