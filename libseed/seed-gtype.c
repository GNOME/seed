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

#include "seed-private.h"
#include <sys/mman.h>

typedef GObject *(*GObjectConstructCallback) (GType, guint,
					      GObjectConstructParam *);

JSClassRef seed_gtype_class;
GIBaseInfo *objectclass_info = NULL;
GIBaseInfo *paramspec_info = NULL;

JSObjectRef seed_gtype_constructor;

GQuark qgetter;
GQuark qsetter;

GQuark qiinit;
GQuark qcinit;

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
						  jsparams, i, exception);

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

static void
seed_gtype_builtin_set_property (GObject * object,
				 guint property_id,
				 const GValue * value, GParamSpec * spec)
{
  JSContextRef ctx = JSGlobalContextCreateInGroup (context_group, 0);
  gchar *name = g_strjoin (NULL, "_", spec->name, NULL);
  JSObjectRef jsobj = (JSObjectRef) seed_value_from_object (ctx, object, 0);

  seed_prepare_global_context (ctx);

  seed_object_set_property (ctx,
			    jsobj,
			    name,
			    seed_value_from_gvalue (ctx, (GValue *) value,
						    0));

  g_free (name);
  JSGlobalContextRelease ((JSGlobalContextRef) ctx);
}

static void
seed_gtype_builtin_get_property (GObject * object,
				 guint property_id,
				 GValue * value, GParamSpec * spec)
{
  // TODO: Exceptions
  JSContextRef ctx = JSGlobalContextCreateInGroup (context_group, 0);
  gchar *name = g_strjoin (NULL, "_", spec->name, NULL);
  JSObjectRef jsobj = (JSObjectRef) seed_value_from_object (ctx, object, 0);
  JSValueRef jsval = seed_object_get_property (ctx, jsobj,
					       name);

  seed_prepare_global_context (ctx);

  seed_value_to_gvalue (ctx, jsval, spec->value_type, value, 0);

  g_free (name);
  JSGlobalContextRelease ((JSGlobalContextRef) ctx);
}

static void
seed_gtype_set_property (GObject * object,
			 guint property_id,
			 const GValue * value, GParamSpec * spec)
{
  gpointer data = g_param_spec_get_qdata (spec, qsetter);

  if (!data)
    {
      seed_gtype_builtin_set_property (object, property_id, value, spec);
      return;
    }
}

static void
seed_gtype_get_property (GObject * object,
			 guint property_id, GValue * value, GParamSpec * spec)
{
  gpointer data = g_param_spec_get_qdata (spec, qgetter);

  if (!data)
    {
      seed_gtype_builtin_get_property (object, property_id, value, spec);
      return;
    }
}

static GIBaseInfo *
seed_get_class_info_for_type (GType type)
{
  GIBaseInfo *object_info;
  
  // Note to self: Investigate the entire premise of this function.
  while ((type = g_type_parent (type)))
    {
      GIBaseInfo *ret;

      object_info = g_irepository_find_by_gtype (NULL, type);
      if (object_info)
	{
	  ret = g_object_info_get_class_struct ((GIObjectInfo *)object_info);
	  g_base_info_unref (object_info);
	  
	  return ret;
	}
    }
  
  return NULL;
}

static void
seed_attach_methods_to_class_object (JSContextRef ctx,
				     JSObjectRef object,
				     JSValueRef * exception)
{
  seed_create_function (ctx, "c_install_property",
			&seed_property_method_invoked, object);
  seed_create_function (ctx, "install_signal",
			&seed_gsignal_method_invoked, object);
}

static void
seed_gtype_call_construct (GType type, GObject * object)
{
  JSContextRef ctx;
  JSObjectRef func, this_object;
  JSValueRef exception = NULL, args[1];
  gchar *mes;

  func = g_type_get_qdata (type, qiinit);

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
		      GObjectConstructParam * construct_params)
{
  GObject *object;
  GType parent;
  GObjectClass *parent_class;

  parent = g_type_parent (type);
  parent_class = g_type_class_ref (parent);

  if (parent_class->constructor == seed_gtype_construct)
    {
      GType t = parent;
      parent = g_type_parent (parent);

      g_type_class_unref (parent_class);
      parent_class = g_type_class_ref (parent);

      object =
	parent_class->constructor (type, n_construct_params,
				   construct_params);
      
      seed_gtype_call_construct (t, object);

      g_type_class_unref (parent_class);
    }
  else
    object =
      parent_class->constructor (type, n_construct_params, construct_params);

  seed_gtype_call_construct (type, object);

  g_type_class_unref (parent_class);

  return object;
}

static void
seed_gtype_install_signals (JSContextRef ctx,
			    JSObjectRef definition, GType type,
			    JSValueRef * exception)
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

  signals =
    (JSObjectRef) seed_object_get_property (ctx, definition, "signals");
  if (JSValueIsNull (ctx, signals) || !JSValueIsObject (ctx, signals))
    return;

  jslength = seed_object_get_property (ctx, signals, "length");
  if (JSValueIsNull (ctx, jslength))
    return;

  length = seed_value_to_uint (ctx, jslength, exception);
  for (i = 0; i < length; i++)
    {
      signal_def = (JSObjectRef) JSObjectGetPropertyAtIndex (ctx,
							     (JSObjectRef)
							     signals, i,
							     exception);

      if (JSValueIsNull (ctx, signal_def)
	  || !JSValueIsObject (ctx, signal_def))
	continue;

      // TODO: Error checking
      jsname = seed_object_get_property (ctx, signal_def, "name");
      name = seed_value_to_string (ctx, jsname, exception);

      SEED_NOTE (GTYPE, "Installing signal with name: %s on type: %s",
		 name, g_type_name (type));

      jsflags =
	seed_object_get_property (ctx, (JSObjectRef) signal_def, "flags");
      if (JSValueIsNull (ctx, jsflags) || !JSValueIsNumber (ctx, jsflags))
	flags = G_SIGNAL_RUN_LAST;
      else
	flags = seed_value_to_long (ctx, jsflags, exception);

      jsreturn_type =
	seed_object_get_property (ctx, signal_def, "return_type");
      if (JSValueIsNull (ctx, jsreturn_type)
	  || !JSValueIsNumber (ctx, jsreturn_type))
	return_type = G_TYPE_NONE;
      else
	return_type = seed_value_to_long (ctx, jsreturn_type, exception);

      jsparams = seed_object_get_property (ctx, signal_def, "parameters");

      if (!JSValueIsNull (ctx, jsparams) && JSValueIsObject (ctx, jsparams))
	{
	  n_params =
	    seed_value_to_int (ctx,
			       seed_object_get_property (ctx,
							 (JSObjectRef)
							 jsparams, "length"),
			       exception);
	  if (n_params > 0)
	    {
	      param_types = g_alloca (sizeof (GType) * n_params);
	      for (j = 0; j < n_params; j++)
		{
		  JSValueRef ptype = JSObjectGetPropertyAtIndex (ctx,
								 (JSObjectRef)
								 jsparams,
								 j,
								 exception);
		  param_types[j] =
		    (GType) seed_value_to_long (ctx, ptype, exception);
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

static unsigned int
seed_gtype_install_properties (JSContextRef ctx,
			       JSObjectRef definition,
			       GType type, GObjectClass *g_class,
			       JSValueRef * exception)
{
  JSObjectRef properties, property_def;
  JSValueRef jslength;
  guint i, length;
  GType property_type, object_type;
  GParamFlags flags;
  JSValueRef jsname, jsflags, jsproperty_type, jsdefault_value, jsobject_type;
  JSValueRef jsnick, jsblurb, jsmin_value, jsmax_value;
  gchar *name , *nick, *blurb;
  GParamSpec *pspec;
  guint property_count = 0;

  properties = (JSObjectRef) seed_object_get_property (ctx, definition,
						       "properties");
  if (JSValueIsNull (ctx, properties) || !JSValueIsObject (ctx, properties))
    return 0;

  jslength = seed_object_get_property (ctx, properties, "length");
  if (JSValueIsNull (ctx, jslength))
    return 0;

  length = seed_value_to_uint (ctx, jslength, exception);
  for (i = 0; i < length; i++)
    {
      property_def = (JSObjectRef) JSObjectGetPropertyAtIndex (ctx,
							       (JSObjectRef)
							       properties, i,
							       NULL);

      if (JSValueIsNull (ctx, property_def)
	  || !JSValueIsObject (ctx, property_def))
	continue;

      jsname = seed_object_get_property (ctx, property_def, "name");
      if (!JSValueIsString(ctx, jsname))
	{
	  seed_make_exception (ctx, exception, "PropertyInstallationError",
			       "Property requires name attribute");
	  return property_count;
	}
      name = seed_value_to_string (ctx, jsname, exception);

      // Check for "nick" property; set to name if nonexistent
      jsnick = seed_object_get_property (ctx, property_def, "nick");
      if (!JSValueIsString(ctx, jsnick))
	nick = name;
      else
	nick = seed_value_to_string (ctx, jsnick, exception);

      // Check for "blurb" property; set to name if nonexistent
      jsblurb = seed_object_get_property (ctx, property_def, "blurb");
      if (!JSValueIsString(ctx, jsblurb))
	blurb = name;
      else
	blurb = seed_value_to_string (ctx, jsblurb, exception);

      SEED_NOTE (GTYPE, "Installing property with name: %s on type: %s",
		 name, g_type_name (type));

      // Flags default to read/write, non-construct
      jsflags =
	seed_object_get_property (ctx, (JSObjectRef) property_def, "flags");
      if (JSValueIsNull (ctx, jsflags) || !JSValueIsNumber (ctx, jsflags))
	flags = G_PARAM_READABLE | G_PARAM_WRITABLE;
      else
	flags = seed_value_to_long (ctx, jsflags, exception);

      jsproperty_type = seed_object_get_property (ctx, property_def, "type");

      if (JSValueIsNull (ctx, jsproperty_type) ||
	  !JSValueIsNumber (ctx, jsproperty_type))
	property_type = G_TYPE_NONE;
      else
	property_type = seed_value_to_long (ctx, jsproperty_type, exception);

      jsdefault_value = seed_object_get_property (ctx, property_def,
						  "default_value");

      if (JSValueIsNull (ctx, jsdefault_value) ||
	  JSValueIsUndefined (ctx, jsdefault_value))
	{
	  if(property_type == G_TYPE_OBJECT)
	    continue;

	  seed_make_exception (ctx, exception, "PropertyInstallationError",
			       "Property of type %s requires default_value attribute",
			       g_type_name(property_type));
	  return property_count;
	}

      jsmin_value = seed_object_get_property (ctx, property_def,
					      "minimum_value");
      jsmax_value = seed_object_get_property (ctx, property_def,
					      "maximum_value");

      // Make sure min/max properties are defined, based on type
      if(property_type == G_TYPE_CHAR ||
	 property_type == G_TYPE_UCHAR ||
	 property_type == G_TYPE_INT ||
	 property_type == G_TYPE_UINT ||
	 property_type == G_TYPE_INT64 ||
	 property_type == G_TYPE_UINT64 ||
	 property_type == G_TYPE_FLOAT ||
	 property_type == G_TYPE_DOUBLE)
	{
	  if (JSValueIsNull (ctx, jsmin_value) ||
	      !JSValueIsNumber (ctx, jsmin_value))
	    {
	      seed_make_exception (ctx, exception, "PropertyInstallationError",
				   "Property of type %s requires minimum_value attribute",
				   g_type_name(property_type));
	      return property_count;
	    }
	  if (JSValueIsNull (ctx, jsmax_value) ||
	      !JSValueIsNumber (ctx, jsmax_value))
	    {
	      seed_make_exception (ctx, exception, "PropertyInstallationError",
				   "Property of type %s requires maximum_value attribute",
				   g_type_name(property_type));
	      return property_count;
	    }
	}


      switch(property_type)
	{
	case G_TYPE_BOOLEAN:
	  pspec = g_param_spec_boolean(name, nick, blurb,
				       seed_value_to_boolean(ctx,
							     jsdefault_value,
							     exception), flags);
	  break;
	case G_TYPE_CHAR:
	  pspec = g_param_spec_char(name, nick, blurb,
				    seed_value_to_char(ctx,
						       jsmin_value,
						       exception),
				    seed_value_to_char(ctx,
						       jsmax_value,
						       exception),
				    seed_value_to_char(ctx,
						       jsdefault_value,
						       exception), flags);
	  break;
	case G_TYPE_UCHAR:
	  pspec = g_param_spec_uchar(name, nick, blurb,
				     seed_value_to_uchar(ctx,
							 jsmin_value,
							 exception),
				     seed_value_to_uchar(ctx,
							 jsmax_value,
							 exception),
				     seed_value_to_uchar(ctx,
							 jsdefault_value,
							 exception), flags);
	  break;
	case G_TYPE_INT:
	  pspec = g_param_spec_int(name, nick, blurb,
				   seed_value_to_int(ctx,
						     jsmin_value,
						     exception),
				   seed_value_to_int(ctx,
						     jsmax_value,
						     exception),
				   seed_value_to_int(ctx,
						     jsdefault_value,
						     exception), flags);
	  break;
	case G_TYPE_UINT:
	  pspec = g_param_spec_uint(name, nick, blurb,
				    seed_value_to_uint(ctx,
						       jsmin_value,
						       exception),
				    seed_value_to_uint(ctx,
						       jsmax_value,
						       exception),
				    seed_value_to_uint(ctx,
						       jsdefault_value,
						       exception), flags);
	  break;
	case G_TYPE_LONG:
	  pspec = g_param_spec_long(name, nick, blurb,
				    seed_value_to_long(ctx,
						       jsmin_value,
						       exception),
				    seed_value_to_long(ctx,
						       jsmax_value,
						       exception),
				    seed_value_to_long(ctx,
						       jsdefault_value,
						       exception), flags);
	  break;
	case G_TYPE_ULONG:
	  pspec = g_param_spec_ulong(name, nick, blurb,
				     seed_value_to_ulong(ctx,
							 jsmin_value,
							 exception),
				     seed_value_to_ulong(ctx,
							 jsmax_value,
							 exception),
				     seed_value_to_ulong(ctx,
							 jsdefault_value,
							 exception), flags);
	  break;
	case G_TYPE_INT64:
	  pspec = g_param_spec_int64(name, nick, blurb,
				     seed_value_to_int64(ctx,
							 jsmin_value,
							 exception),
				     seed_value_to_int64(ctx,
							 jsmax_value,
							 exception),
				     seed_value_to_int64(ctx,
							 jsdefault_value,
							 exception), flags);
	  break;
	case G_TYPE_UINT64:
	  pspec = g_param_spec_uint64(name, nick, blurb,
				      seed_value_to_uint64(ctx,
							   jsmin_value,
							   exception),
				      seed_value_to_uint64(ctx,
							   jsmax_value,
							   exception),
				      seed_value_to_uint64(ctx,
							   jsdefault_value,
							   exception), flags);
	  break;
	case G_TYPE_FLOAT:
	  pspec = g_param_spec_float(name, nick, blurb,
				     seed_value_to_float(ctx,
							 jsmin_value,
							 exception),
				     seed_value_to_float(ctx,
							 jsmax_value,
							 exception),
				     seed_value_to_float(ctx,
							 jsdefault_value,
							 exception), flags);
	  break;
	case G_TYPE_DOUBLE:
	  pspec = g_param_spec_double(name, nick, blurb,
				      seed_value_to_double(ctx,
							   jsmin_value,
							   exception),
				      seed_value_to_double(ctx,
							   jsmax_value,
							   exception),
				      seed_value_to_double(ctx,
							   jsdefault_value,
							   exception), flags);
	  break;
	  // TODO: support enums/flags/params
	case G_TYPE_STRING:
	  // TODO: leaky?
	  pspec = g_param_spec_string(name, nick, blurb,
				      seed_value_to_string(ctx,
							   jsdefault_value,
							   exception), flags);
	  break;
	case G_TYPE_OBJECT:
	  jsobject_type = seed_object_get_property (ctx, property_def,
						    "object_type");

	  if (JSValueIsNull (ctx, jsobject_type) ||
	      !JSValueIsNumber (ctx, jsobject_type))
	    object_type = G_TYPE_NONE;
	  else
	    object_type = seed_value_to_long (ctx, jsobject_type, exception);

	  if(object_type == G_TYPE_NONE)
	    {
	      seed_make_exception (ctx, exception, "PropertyInstallationError",
				   "Property of type %s requires object_type attribute",
				   g_type_name(property_type));
	      return property_count;
	    }

	  pspec = g_param_spec_object(name, nick, blurb,
				      object_type, flags);
	  break;
	case G_TYPE_BOXED: // Boxed types TODO: this is almost certainly wrong
	  pspec = g_param_spec_boxed(name, nick, blurb, type, flags);
	  break;
	default:
	case G_TYPE_NONE:
	  seed_make_exception (ctx, exception, "PropertyInstallationError",
			       "Property requires type attribute");
	  return property_count;
	  break;
	}

      g_object_class_install_property (g_class, ++property_count, pspec);

      if (nick != name)
	g_free (nick);

      if (blurb != name)
	g_free (blurb);

      g_free (name);
    }

  return property_count;
}

static void
seed_gtype_class_init (gpointer g_class, gpointer class_data)
{
  SeedGClassPrivates *priv;
  GIBaseInfo *class_info;
  JSContextRef ctx;
  JSValueRef jsargs[2];
  GType type;
  JSValueRef exception = NULL;
  int initial_prop_count = 1;
  GQuark class_init_exception_q = 
  	g_quark_from_static_string("type-class-init-exception");

  priv = (SeedGClassPrivates *) class_data;

  ((GObjectClass *)g_class)->get_property = seed_gtype_get_property;
  ((GObjectClass *)g_class)->set_property = seed_gtype_set_property;
  ((GObjectClass *)g_class)->constructor = seed_gtype_construct;

  ctx = JSGlobalContextCreateInGroup (context_group, 0);

  type = (GType) JSObjectGetPrivate (priv->constructor);
  seed_gtype_install_signals (ctx, priv->definition, type, &exception);
  initial_prop_count = seed_gtype_install_properties (ctx,
						      priv->definition,
						      type,
						      (GObjectClass *) g_class,
						      &exception);

  if (!priv->func)
    {
      JSGlobalContextRelease ((JSGlobalContextRef) ctx);
      if (exception)
	{
	  g_type_set_qdata(type, class_init_exception_q, (gpointer)exception);
	}
      return;
    }

  seed_prepare_global_context (ctx);

  class_info = seed_get_class_info_for_type (type);

  jsargs[0] = seed_make_struct (ctx, g_class, class_info);
  jsargs[1] = seed_gobject_get_prototype_for_gtype (type);

  seed_attach_methods_to_class_object (ctx, (JSObjectRef) jsargs[0],
				       &exception);

  g_base_info_unref ((GIBaseInfo *) class_info);

  SEED_NOTE (GTYPE, "Marshalling class init for type: %s",
	     g_type_name (type));

  seed_object_set_property (ctx, (JSObjectRef) jsargs[0],
			    "type", seed_value_from_int (ctx, type, 0));
  seed_object_set_property (ctx, (JSObjectRef) jsargs[0],
			    "property_count",
			    seed_value_from_int (ctx, initial_prop_count + 1,
						 0));

  JSObjectCallAsFunction (ctx, priv->func, 0, 2, jsargs, &exception);
  if (exception)
    {
      g_type_set_qdata(type, class_init_exception_q, (gpointer)exception);
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


  // TODO: GType is of variable length, so this is an incorrect fix
  parent_type = (GType) seed_value_to_long (ctx, parent_ref, exception);

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
      g_type_set_qdata (new_type, qiinit, (gpointer) instance_init);
      JSValueProtect (ctx, instance_init);
    }

  g_free (new_name);
  return constructor_ref;
}

static JSValueRef
seed_param_getter_invoked (JSContextRef ctx,
			   JSObjectRef function,
			   JSObjectRef thisObject,
			   gsize argumentCount,
			   const JSValueRef arguments[],
			   JSValueRef * exception)
{
  GParamSpec *pspec = seed_pointer_get_pointer (ctx, thisObject);

  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "ParamSpec.get expected "
			   "1 argument, got %zd", argumentCount);

      return JSValueMakeNull (ctx);
    }
  else if (JSValueIsNull (ctx, arguments[0]) ||
	   !JSValueIsObject (ctx, arguments[0]) ||
	   !JSObjectIsFunction (ctx, (JSObjectRef) arguments[0]))
    {
      // Maybe should  accept C functions
      seed_make_exception (ctx, exception, "ArgumentError",
			   "ParamSpec.get expected a function");
      return JSValueMakeNull (ctx);
    }

  g_param_spec_set_qdata (pspec, qgetter, (gpointer) arguments[0]);

  return seed_value_from_boolean (ctx, TRUE, exception);
}

static JSValueRef
seed_param_setter_invoked (JSContextRef ctx,
			   JSObjectRef function,
			   JSObjectRef thisObject,
			   gsize argumentCount,
			   const JSValueRef arguments[],
			   JSValueRef * exception)
{
  GParamSpec *pspec = seed_pointer_get_pointer (ctx, thisObject);

  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "ParamSpec.set expected "
			   "1 argument, got %zd", argumentCount);

      return JSValueMakeNull (ctx);
    }
  else if (JSValueIsNull (ctx, arguments[0]) ||
	   !JSValueIsObject (ctx, arguments[0]) ||
	   !JSObjectIsFunction (ctx, (JSObjectRef) arguments[0]))
    {
      // Maybe should  accept C functions
      seed_make_exception (ctx, exception, "ArgumentError",
			   "ParamSpec.set expected a function");
      return JSValueMakeNull (ctx);
    }

  g_param_spec_set_qdata (pspec, qsetter, (gpointer) arguments[0]);

  return seed_value_from_boolean (ctx, TRUE, exception);
}

void
seed_define_gtype_functions (JSContextRef ctx)
{
  JSObjectRef proto;

  objectclass_info = g_irepository_find_by_name (NULL,
						 "GObject", "ObjectClass");

  proto = seed_struct_prototype (ctx, objectclass_info);

  paramspec_info = g_irepository_find_by_name (NULL, "GObject", "ParamSpec");
  proto = seed_struct_prototype (ctx, paramspec_info);

  seed_create_function (ctx, "get", &seed_param_getter_invoked, proto);
  seed_create_function (ctx, "set", &seed_param_setter_invoked, proto);
}

void
seed_gtype_init (SeedEngine * local_eng)
{
  JSClassDefinition gtype_def = kJSClassDefinitionEmpty;

  gtype_def.callAsConstructor = seed_gtype_constructor_invoked;
  seed_gtype_class = JSClassCreate (&gtype_def);
  JSClassRetain (seed_gtype_class);

  seed_gtype_constructor =
    JSObjectMake (local_eng->context, seed_gtype_class, 0);

  seed_object_set_property (local_eng->context,
			    local_eng->global, "GType",
			    seed_gtype_constructor);

  qiinit = g_quark_from_static_string("js-instance-init");

  seed_define_gtype_functions (local_eng->context);
}
