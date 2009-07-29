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

#include "seed-private.h"
#include <sys/mman.h>

typedef  GObject * (*GObjectConstructCallback) (GType, guint, GObjectConstructParam *);

GHashTable *gtype_iinits;

JSClassRef seed_gtype_class;
GIBaseInfo *objectclass_info = NULL;
GIBaseInfo *paramspec_info = NULL;

JSObjectRef seed_gtype_constructor;

typedef struct _SeedGClassPrivates {
  JSObjectRef constructor;
  JSObjectRef func;

  JSObjectRef definition;
} SeedGClassPrivates;

static JSValueRef
seed_property_method_invoked (JSContextRef ctx,
			      JSObjectRef function,
			      JSObjectRef thisObject,
			      gsize argumentCount,
			      const JSValueRef arguments[],
			      JSValueRef * exception)
{
  GParamSpec *spec;
  GObjectClass *class;
  guint property_count;
  JSValueRef newcount, oldcount;

  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Property installation expected 1 argument"
			   " got %zd \n", argumentCount);

      return JSValueMakeNull (ctx);
    }

  if (JSValueIsNull (ctx, arguments[0]) ||
      // Might need to check if JSValueIsObject? Who knows with WebKit.
      !JSValueIsObjectOfClass (ctx, arguments[0], seed_struct_class))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Property installation expected a "
			   "GParamSpec as argument");
      return JSValueMakeNull (ctx);
    }

  spec = (GParamSpec *) seed_pointer_get_pointer (ctx, arguments[0]);

  oldcount = seed_object_get_property (ctx, thisObject, "property_count");
  property_count = seed_value_to_int (ctx, oldcount, exception);

  class = seed_pointer_get_pointer (ctx, thisObject);
  g_object_class_install_property (class, property_count, spec);

  newcount = seed_value_from_int (ctx, property_count + 1, exception);
  seed_object_set_property (ctx, thisObject, "property_count", newcount);

  return oldcount;
}

static JSValueRef
seed_gsignal_method_invoked (JSContextRef ctx,
			     JSObjectRef function,
			     JSObjectRef thisObject,
			     gsize argumentCount,
			     const JSValueRef arguments[],
			     JSValueRef * exception)
{
  // TODO: class_closure, and accumlator. Not useful until we have structs.
  JSValueRef jsname, jstype, jsflags, jsreturn_type, jsparams;
  GType itype, return_type;
  guint n_params = 0;
  GType *param_types = 0;
  gchar *name;
  guint signal_id;
  GSignalFlags flags;

  /* Sanity check */
  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal constructor expected 1 argument"
			   " got %zd \n", argumentCount);
      return (JSObjectRef) JSValueMakeNull (ctx);
    }
  if (JSValueIsNull (ctx, arguments[0])
      || !JSValueIsObject (ctx, arguments[0]))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal constructor expected object"
			   " as first argument");
      return (JSObjectRef) JSValueMakeNull (ctx);
    }

  /* Signal name */
  jsname = seed_object_get_property (ctx, (JSObjectRef) arguments[0], "name");
  /* seed_value_to_string can handle non strings, however the kind
   * of strings we want as a signal name are rather small, so make sure
   * we have an actual string */
  if (JSValueIsNull (ctx, jsname) || !JSValueIsString (ctx, jsname))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal definition needs name property");
      return (JSObjectRef) JSValueMakeNull (ctx);
    }
  name = seed_value_to_string (ctx, jsname, exception);

  /* Type to install on. Comes from class. */
  jstype = seed_object_get_property (ctx, thisObject, "type");
  itype = seed_value_to_int (ctx, jstype, exception);

  SEED_NOTE (GTYPE, "Installing signal with name: %s on type: %s",
	     name, g_type_name (itype));

  /* Signal flags */
  jsflags = seed_object_get_property (ctx,
				      (JSObjectRef) arguments[0], "flags");
  if (JSValueIsNull (ctx, jsflags) || !JSValueIsNumber (ctx, jsflags))
    flags = G_SIGNAL_RUN_LAST;
  else
    flags = seed_value_to_long (ctx, jsflags, exception);

  /* Return type */
  jsreturn_type = seed_object_get_property (ctx, (JSObjectRef) arguments[0],
					    "return_type");
  if (JSValueIsNull (ctx, jsreturn_type) ||
      !JSValueIsNumber (ctx, jsreturn_type))
    return_type = G_TYPE_NONE;
  else
    return_type = seed_value_to_int (ctx, jsreturn_type, exception);

  /* Number of params and types */
  jsparams = seed_object_get_property (ctx, (JSObjectRef) arguments[0],
				       "parameters");
  if (!JSValueIsNull (ctx, jsparams) && JSValueIsObject (ctx, jsparams))
    {
      n_params = seed_value_to_int
	(ctx,
	 seed_object_get_property (ctx, (JSObjectRef) jsparams, "length"),
	 exception);
      if (n_params > 0)
	{
	  guint i;
      JSValueRef ptype;

	  param_types = g_new0 (GType, n_params);
	  for (i = 0; i < n_params; i++)
	    {
	       ptype = JSObjectGetPropertyAtIndex (ctx,
							     (JSObjectRef)
							     jsparams,
							     i,
							     exception);

	      param_types[i] = seed_value_to_int (ctx, ptype, exception);
	    }
	}
    }

  signal_id = g_signal_newv (name, itype,
			     flags, 0, 0, 0,
			     gi_cclosure_marshal_generic,
			     return_type, n_params, param_types);

  g_free (name);
  g_free (param_types);

  return (JSValueRef) seed_value_from_uint (ctx, signal_id, exception);
}

static GIBaseInfo *
seed_get_class_info_for_type (GType type)
{
  GIBaseInfo *object_info;

  while ((type = g_type_parent (type)))
    {
      object_info = g_irepository_find_by_gtype (NULL, type);
      if (object_info)
	{
	  return
	    (GIBaseInfo *)g_object_info_get_class_struct
	                  ((GIObjectInfo *)object_info);
	}
      g_base_info_unref (object_info);
    }
  return NULL;
}

static void
seed_attach_methods_to_class_object (JSContextRef ctx,
				     JSObjectRef object,
				     JSValueRef *exception)
{
  seed_create_function (ctx, "c_install_property",
			&seed_property_method_invoked, object);
  seed_create_function (ctx, "install_signal",
			&seed_gsignal_method_invoked, object);
}

static void
seed_gtype_call_construct (GType type, GObject *object)
{
  JSContextRef ctx;
  JSObjectRef func, this_object;
  JSValueRef exception = NULL, args[1];
  gchar * mes;

  func = g_hash_table_lookup (gtype_iinits, GINT_TO_POINTER (type));
  if (func)
    {
      ctx = JSGlobalContextCreateInGroup (context_group, 0);
      seed_prepare_global_context (ctx);

      SEED_NOTE (GTYPE, "Handling constructor for: %p with type: %s",
		 object, g_type_name (type));
      this_object = (JSObjectRef) seed_value_from_object (ctx, object, NULL);
      args[0] = this_object;

      JSObjectCallAsFunction (ctx, func, this_object, 1, args, &exception);
      if (exception)
	{
	  mes = seed_exception_to_string (ctx, exception);
	  g_warning ("Exception in instance construction. %s \n", mes);
	  g_free (mes);
	}
      JSGlobalContextRelease ((JSGlobalContextRef) ctx);
    }


}

static GObject *
seed_gtype_construct (GType type,
		      guint n_construct_params,
		      GObjectConstructParam *construct_params)
{
  GObject *object;
  GType parent;
  GObjectClass *parent_class;

  parent = g_type_parent (type);
  parent_class = g_type_class_ref (parent);
    
  if (parent_class->constructor == seed_gtype_construct)
    {
      parent = g_type_parent (parent);

      g_type_class_unref (parent_class);
      parent_class = g_type_class_ref (parent);

      object = parent_class->constructor (type, n_construct_params, construct_params);

      g_type_class_unref (parent_class);
    }
  else
    object = parent_class->constructor (type, n_construct_params, construct_params);
  
  seed_gtype_call_construct (type, object);
  
  g_type_class_unref (parent_class);

  return object;
}

static void
seed_gtype_install_signals (JSContextRef ctx,
			    JSObjectRef definition,
			    GType type)
{
  JSObjectRef signals, signal_def;
  JSValueRef jslength;
  guint i, j, length;
  GType return_type;
  GType *param_types = NULL;
  guint n_params = 0;
  GSignalFlags flags;
  JSValueRef jsname, jsflags, jsreturn_type, jsparams;
  gchar *name;

  signals = (JSObjectRef) seed_object_get_property (ctx, definition, "signals");
  if (JSValueIsNull(ctx, signals) || !JSValueIsObject (ctx, signals))
    return;

  jslength = seed_object_get_property (ctx, signals, "length");
  if (JSValueIsNull (ctx, jslength))
    return;

  length = seed_value_to_uint (ctx, jslength, NULL);
  for (i = 0; i < length; i++)
    {
       signal_def = (JSObjectRef)JSObjectGetPropertyAtIndex (ctx,
									(JSObjectRef) signals,
									i,
									NULL);

      if (JSValueIsNull (ctx, signal_def) || !JSValueIsObject(ctx, signal_def))
	continue;

      // TODO: Error checking
      jsname = seed_object_get_property (ctx, signal_def, "name");
      name = seed_value_to_string (ctx, jsname, NULL);

      SEED_NOTE(GTYPE, "Installing signal with name: %s on type: %s",
		name, g_type_name (type));

      jsflags = seed_object_get_property (ctx, (JSObjectRef) signal_def, "flags");
      if (JSValueIsNull (ctx, jsflags) || !JSValueIsNumber (ctx, jsflags))
        flags = G_SIGNAL_RUN_LAST;
      else
        flags = seed_value_to_long (ctx, jsflags, NULL);

      jsreturn_type = seed_object_get_property (ctx, signal_def, "return_type");
      if (JSValueIsNull (ctx, jsreturn_type) || !JSValueIsNumber (ctx, jsreturn_type))
	return_type = G_TYPE_NONE;
      else
	return_type = seed_value_to_long (ctx, jsreturn_type, NULL);

      jsparams = seed_object_get_property (ctx, signal_def, "parameters");

      if (!JSValueIsNull (ctx, jsparams) && JSValueIsObject (ctx, jsparams))
	{
	  n_params = seed_value_to_int (ctx, seed_object_get_property (ctx, (JSObjectRef) jsparams, "length"), NULL);
	  if (n_params > 0)
	    {
	      param_types = g_alloca (sizeof (GType)*n_params);
	      for (j = 0; j < n_params; j++)
		{
		  JSValueRef ptype = JSObjectGetPropertyAtIndex (ctx,
								 (JSObjectRef)
								 jsparams,
								 j,
								 NULL);
		  param_types[j] = (GType) seed_value_to_long (ctx, ptype, NULL);
		}
	    }
	}

      g_signal_newv (name, type,
		     flags, 0, 0, 0,
		     gi_cclosure_marshal_generic,
		     return_type, n_params, param_types);
      g_free (name);

    }
}

static void
seed_gtype_class_init (gpointer g_class,
		       gpointer class_data)
{
  SeedGClassPrivates *priv;
  GIBaseInfo *class_info;
  JSContextRef ctx;
  JSValueRef jsargs[2];
  GType type;
  gchar *mes;
  JSValueRef exception = NULL;

  priv = (SeedGClassPrivates *)class_data;

  ((GObjectClass *)g_class)->constructor = seed_gtype_construct;

  ctx = JSGlobalContextCreateInGroup (context_group, 0);

  type = (GType) JSObjectGetPrivate (priv->constructor);
  seed_gtype_install_signals (ctx, priv->definition, type);

  if (!priv->func)
    {
      JSGlobalContextRelease ((JSGlobalContextRef) ctx);
      return;
    }

  seed_prepare_global_context (ctx);

  class_info = seed_get_class_info_for_type (type);

  jsargs[0] = seed_make_struct (ctx, g_class, class_info);
  jsargs[1] = seed_gobject_get_prototype_for_gtype (type);

  seed_attach_methods_to_class_object (ctx, (JSObjectRef) jsargs[0], &exception);

  g_base_info_unref ((GIBaseInfo *) class_info);

  SEED_NOTE (GTYPE, "Marshalling class init for type: %s",
	     g_type_name (type));

  seed_object_set_property (ctx, (JSObjectRef) jsargs[0],
			    "type", seed_value_from_int (ctx, type, 0));
  seed_object_set_property (ctx, (JSObjectRef) jsargs[0],
			    "property_count", seed_value_from_int (ctx, 1,
								   0));

  JSObjectCallAsFunction (ctx, priv->func, 0, 2, jsargs, &exception);
  if (exception)
    {
      mes = seed_exception_to_string (ctx, exception);
      g_warning ("Exception in class init closure. %s \n", mes);
      g_free (mes);
    }

  JSGlobalContextRelease ((JSGlobalContextRef) ctx);
}

static JSObjectRef
seed_gtype_constructor_invoked (JSContextRef ctx,
                                JSObjectRef constructor,
                                gsize argumentCount,
                                const JSValueRef arguments[],
                                JSValueRef * exception)
{
  JSValueRef class_init, instance_init, name, parent_ref;
  GType parent_type, new_type;
  gchar *new_name;
  GTypeInfo type_info = {
    0,
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) NULL,
    (GClassFinalizeFunc) NULL,
    NULL,
    0,
    0,
    NULL
  };
  GTypeQuery query;
  JSObjectRef constructor_ref;
  SeedGClassPrivates *priv;

  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "GType constructor expected 1 "
			   "argument, got %zd \n", argumentCount);
      return (JSObjectRef) JSValueMakeNull (ctx);
    }
  if (!JSValueIsObject (ctx, arguments[0]))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "GType constructor expected a"
			   "class definition object. Got a nonobject");

      return (JSObjectRef) JSValueMakeNull (ctx);
    }
  parent_ref = seed_object_get_property (ctx,
					 (JSObjectRef) arguments[0],
					 "parent");
  class_init =
    seed_object_get_property (ctx, (JSObjectRef) arguments[0], "class_init");
  instance_init =
    seed_object_get_property (ctx, (JSObjectRef) arguments[0], "init");
  name = seed_object_get_property (ctx, (JSObjectRef) arguments[0], "name");

  new_name = seed_value_to_string (ctx, name, exception);
  if (!JSValueIsNumber (ctx, parent_ref))
    {
      seed_make_exception (ctx, exception, "TypeError",
			   "GType constructor expected GType for parent");

      return (JSObjectRef) JSValueMakeNull (ctx);
    }


  parent_type = (GType) seed_value_to_int (ctx, parent_ref, exception);

  SEED_NOTE (GTYPE, "Registering new GType with name: %s as child of %s.",
	     new_name, g_type_name (parent_type));

  g_type_query (parent_type, &query);
  type_info.class_size = query.class_size;
  type_info.instance_size = query.instance_size;
  type_info.class_init = seed_gtype_class_init;

  priv = g_slice_alloc (sizeof (SeedGClassPrivates));

  if (!JSValueIsNull (ctx, class_init) &&
      JSValueIsObject (ctx, class_init) &&
      JSObjectIsFunction (ctx, (JSObjectRef) class_init))
    {
      priv->func = (gpointer) class_init;
      JSValueProtect (ctx, class_init);
    }


  constructor_ref = JSObjectMake (ctx, gobject_constructor_class,
				  (gpointer) 0);
  JSValueProtect (ctx, constructor_ref);

  priv->constructor = constructor_ref;

  JSValueProtect (ctx, arguments[0]);
  priv->definition = (JSObjectRef) arguments[0];

  type_info.class_data = priv;

  new_type = g_type_register_static (parent_type, new_name, &type_info, 0);
  seed_gobject_get_class_for_gtype (ctx, new_type);
  JSObjectSetPrivate (constructor_ref, (gpointer) new_type);

  seed_object_set_property (ctx, constructor_ref,
			    "type", seed_value_from_int (ctx, new_type,
							 exception));

  if (!JSValueIsNull (ctx, instance_init) &&
      JSValueIsObject (ctx, instance_init) &&
      JSObjectIsFunction (ctx, (JSObjectRef) instance_init))
    {
      g_hash_table_insert (gtype_iinits, GINT_TO_POINTER (new_type), (gpointer) instance_init);
      JSValueProtect (ctx, instance_init);
    }

  g_free (new_name);
  return constructor_ref;
}

void
seed_define_gtype_functions (JSContextRef ctx)
{
  JSObjectRef proto;

  objectclass_info = g_irepository_find_by_name (NULL,
						 "GObject", "ObjectClass");

  proto = seed_struct_prototype (ctx, objectclass_info);
}

void
seed_gtype_init (SeedEngine * local_eng)
{
  JSClassDefinition gtype_def = kJSClassDefinitionEmpty;

  gtype_def.callAsConstructor = seed_gtype_constructor_invoked;
  seed_gtype_class = JSClassCreate (&gtype_def);
  JSClassRetain (seed_gtype_class);

  seed_gtype_constructor = JSObjectMake (local_eng->context, seed_gtype_class, 0);

  seed_object_set_property (local_eng->context,
			    local_eng->global, "GType", seed_gtype_constructor);

  seed_define_gtype_functions (local_eng->context);
  
  gtype_iinits = g_hash_table_new (g_int_hash, g_str_equal);
}

