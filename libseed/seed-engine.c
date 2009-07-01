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
#include "seed-path.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdarg.h>

JSObjectRef seed_obj_ref;

GQuark qname;
GQuark qprototype;

JSClassRef gobject_signal_class;
JSClassRef gobject_named_constructor_class;
JSClassRef seed_struct_constructor_class;

JSContextGroupRef context_group;

gchar *glib_message = 0;

GIBaseInfo *base_info_info = 0;

GQuark js_ref_quark;

guint seed_debug_flags = 0;	/* global seed debug flag */

__thread JSObjectRef seed_next_gobject_wrapper = NULL;


#ifdef SEED_ENABLE_DEBUG
static const GDebugKey seed_debug_keys[] = {
  {"misc", SEED_DEBUG_MISC},
  {"finalization", SEED_DEBUG_FINALIZATION},
  {"initialization", SEED_DEBUG_INITIALIZATION},
  {"signal", SEED_DEBUG_SIGNAL},
  {"invocation", SEED_DEBUG_INVOCATION},
  {"structs", SEED_DEBUG_STRUCTS},
  {"construction", SEED_DEBUG_CONSTRUCTION},
  {"gtype", SEED_DEBUG_GTYPE},
  {"importer", SEED_DEBUG_IMPORTER},
  {"module", SEED_DEBUG_MODULE}
};
#endif /* SEED_ENABLE_DEBUG */

void
seed_prepare_global_context (JSContextRef ctx)
{
  JSObjectRef global = JSContextGetGlobalObject (ctx);

  seed_object_set_property (ctx, global, "imports", importer);
  seed_object_set_property (ctx, global, "GType", seed_gtype_constructor);
  seed_object_set_property (ctx, global, "Seed", seed_obj_ref);
  seed_object_set_property (ctx, global, "print", seed_print_ref);


  JSEvaluateScript (ctx, defaults_script, NULL, NULL, 0, NULL);
}

static JSObjectRef
seed_struct_constructor_invoked (JSContextRef ctx,
				 JSObjectRef constructor,
				 size_t argumentCount,
				 const JSValueRef arguments[],
				 JSValueRef * exception)
{
  GIBaseInfo *info = JSObjectGetPrivate (constructor);
  JSValueRef ret;
  JSObjectRef parameters = 0;

  if (argumentCount == 1)
    {
      if (!JSValueIsObject (ctx, arguments[0]))
	{
	  seed_make_exception (ctx, exception, "ArgumentError",
			       "Constructor expects object as argument");
	  return (JSObjectRef) JSValueMakeNull (ctx);
	}
      parameters = (JSObjectRef) arguments[0];
    }
  ret = seed_construct_struct_type_with_parameters (ctx, info,
						    parameters, exception);

  return (JSObjectRef) ret;
}

static JSObjectRef
seed_gobject_constructor_invoked (JSContextRef ctx,
				  JSObjectRef constructor,
				  size_t argumentCount,
				  const JSValueRef arguments[],
				  JSValueRef * exception)
{
  GType type;
  GParameter *params;
  GObjectClass *oclass;
  GObject *gobject;
  GParamSpec *param_spec;
  gchar *prop_name;
  gint i, nparams = 0, length, ri;
  JSObjectRef ret;
  JSPropertyNameArrayRef jsprops = 0;
  JSStringRef jsprop_name;
  JSValueRef jsprop_value;

  type = (GType) JSObjectGetPrivate (constructor);
  if (!type)
    return 0;

  oclass = g_type_class_ref (type);

  if (argumentCount > 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Constructor expects"
			   " 1 argument, got %zd", argumentCount);

      return (JSObjectRef) JSValueMakeNull (ctx);
    }

  if (argumentCount == 1)
    {
      if (!JSValueIsObject (ctx, arguments[0]))
	{
	  seed_make_exception (ctx, exception, "ArgumentError",
			       "Constructor expects object as argument");
	  g_type_class_unref (oclass);
	  return (JSObjectRef) JSValueMakeNull (ctx);
	}

      jsprops = JSObjectCopyPropertyNames (ctx, (JSObjectRef) arguments[0]);
      nparams = JSPropertyNameArrayGetCount (jsprops);
    }
  ri = i = 0;

  params = g_new0 (GParameter, nparams + 1);
  SEED_NOTE (INITIALIZATION, "Constructing object of type %s",
	     g_type_name (type));

  seed_next_gobject_wrapper = seed_make_wrapper_for_type (ctx, type);

  while (i < nparams)
    {
      GType type;
      jsprop_name = JSPropertyNameArrayGetNameAtIndex (jsprops, i);

      length = JSStringGetMaximumUTF8CStringSize (jsprop_name);
      prop_name = g_alloca (length * sizeof (gchar));
      JSStringGetUTF8CString (jsprop_name, prop_name, length);

      param_spec = g_object_class_find_property (oclass, prop_name);

      jsprop_value = JSObjectGetProperty (ctx,
					  (JSObjectRef) arguments[0],
					  jsprop_name, NULL);

      if (param_spec == NULL)
	{
	  JSObjectSetProperty (ctx, seed_next_gobject_wrapper, jsprop_name,
			       jsprop_value, 0, NULL);
	  ++i;
	  continue;
	}
      // TODO: exception handling

      if (g_type_is_a (param_spec->value_type, G_TYPE_ENUM))
	type = G_TYPE_INT;
      else
	type = param_spec->value_type;

      seed_gvalue_from_seed_value (ctx, jsprop_value,
				   type, &params[ri].value, exception);

      if (*exception)
	{
	  g_free (params);
	  JSPropertyNameArrayRelease (jsprops);
	  seed_next_gobject_wrapper =  NULL;
	  return 0;
	}
      params[ri].name = prop_name;

      ++i;
      ++ri;
    }

  if (jsprops)
    JSPropertyNameArrayRelease (jsprops);


  gobject = g_object_newv (type, ri, params);


  if (G_IS_INITIALLY_UNOWNED (gobject) &&
      !g_object_is_floating(gobject))
    g_object_ref(gobject);
  else if (g_object_is_floating(gobject))
    g_object_ref_sink(gobject);

  if (!gobject)
    ret = (JSObjectRef) JSValueMakeNull (ctx);
  else
    ret = (JSObjectRef) seed_value_from_object (ctx, gobject, exception);

  for (i = 0; i < ri; i++)
    {
      g_value_unset (&params[i].value);
    }

  g_object_unref (gobject);

  g_type_class_unref (oclass);

  g_free (params);

  return ret;
}

static JSValueRef
seed_gobject_property_type (JSContextRef ctx,
			    JSObjectRef function,
			    JSObjectRef this_object,
			    size_t argumentCount,
			    const JSValueRef arguments[],
			    JSValueRef * exception)
{
  GParamSpec *spec;
  gchar *name;
  GObject *this;

  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "__property_type expects 1 argument"
			   "got %zd", argumentCount);
      return JSValueMakeNull (ctx);
    }

  this = seed_value_to_object (ctx, this_object, exception);
  name = seed_value_to_string (ctx, arguments[0], exception);

  spec = g_object_class_find_property (G_OBJECT_GET_CLASS (this), name);
  g_free (name);

  return seed_value_from_long (ctx, spec->value_type, exception);
}

static JSValueRef
seed_gobject_ref_count (JSContextRef ctx,
			JSObjectRef function,
			JSObjectRef this_object,
			size_t argumentCount,
			const JSValueRef arguments[], JSValueRef * exception)
{
  GObject *this;

  this = seed_value_to_object (ctx, (JSValueRef) this_object, exception);

  return seed_value_from_int (ctx, this->ref_count, exception);
}

static void
seed_gobject_method_finalize (JSObjectRef method)
{
  GIBaseInfo *info = (GIBaseInfo *) JSObjectGetPrivate (method);
  if (info)
    g_base_info_unref (info);
}

typedef void (*InitMethodCallback) (gint *argc, gchar ***argv);

static gboolean
seed_gobject_init_build_argv (JSContextRef ctx,
			      JSObjectRef array,
			      SeedArgvPrivates *priv,
			      JSValueRef *exception)
{
  guint i,length;
  JSValueRef jsl;

  jsl = seed_object_get_property (ctx, array, "length");
  if (JSValueIsNull (ctx, jsl) || JSValueIsUndefined (ctx, jsl))
    return FALSE;

  length = seed_value_to_uint (ctx, jsl, exception);
  priv->argv = g_new(gchar *, length);
  priv->argc = length;

  for (i = 0; i < length; i++)
    {
      priv->argv[i] = seed_value_to_string (ctx,
					    JSObjectGetPropertyAtIndex (ctx, array, i,
									exception),
					    exception);
    }
  return TRUE;

}

static JSValueRef
seed_gobject_init_method_invoked (JSContextRef ctx,
				  JSObjectRef function,
				  JSObjectRef this_object,
				  size_t argumentCount,
				  const JSValueRef arguments[],
				  JSValueRef * exception)
{
  GIBaseInfo *info;
  GTypelib *typelib;
  InitMethodCallback c;
  SeedArgvPrivates *priv = NULL;
  gboolean allocated = FALSE;

  if (argumentCount != 1 && argumentCount != 2)
    {
      seed_make_exception (ctx, exception,
			   "ArgumentError", "init method expects 1 argument, got %zd",
			   argumentCount);
      return JSValueMakeUndefined (ctx);
    }

  if (argumentCount ==1)
    {
      if (JSValueIsNull (ctx, arguments[0]) || !JSValueIsObject (ctx, arguments[0]))

	{
	  seed_make_exception (ctx, exception,
			       "ArgumentError", "init method expects an array object as argument");
	  return JSValueMakeUndefined (ctx);
	}
      if(JSValueIsObjectOfClass (ctx, arguments[0], seed_argv_class))
	{
	  priv = JSObjectGetPrivate ((JSObjectRef)arguments[0]);
	}
      else
	{
	  priv = g_newa (SeedArgvPrivates,1);
	  if (!seed_gobject_init_build_argv (ctx,
					     (JSObjectRef)arguments[0],
					     priv,
					     exception))
	    {
	      seed_make_exception (ctx, exception, "ArgumentError",
				   "Init method expects an array as argument");
	      return JSValueMakeUndefined (ctx);

	    }
	  allocated = TRUE;
	}
    }

  info = JSObjectGetPrivate (function);
  typelib = g_base_info_get_typelib (info);
  g_typelib_symbol (typelib, g_function_info_get_symbol ((GIFunctionInfo *)info), (gpointer *)&c);
  // Backwards compatibility
  if (!priv)
    {
      c(NULL, NULL);
      return JSValueMakeUndefined (ctx);
    }

  c(&priv->argc, &priv->argv);

  if (allocated)
    g_free (priv->argv);

  return JSValueMakeUndefined (ctx);
}

static JSValueRef
seed_gobject_method_invoked (JSContextRef ctx,
			     JSObjectRef function,
			     JSObjectRef this_object,
			     size_t argumentCount,
			     const JSValueRef arguments[],
			     JSValueRef * exception)
{
  GIBaseInfo *info;
  GObject *object = NULL;
  gboolean instance_method = TRUE;
  GArgument retval;
  GArgument *in_args;
  GArgument *out_args;
  GArgument *out_values;
  guint n_args, n_in_args, n_out_args, i;
  GIArgInfo *arg_info;
  GITypeInfo *type_info;
  GIDirection dir;
  JSValueRef retval_ref;
  GError *error = 0;

  info = JSObjectGetPrivate (function);
  // We just want to check if there IS an object, not actually throw an
  // exception if we don't
  // get it.
  if (!this_object || !
      ((object = seed_value_to_object (ctx, this_object, 0)) ||
       (object = seed_pointer_get_pointer (ctx, this_object))))
    instance_method = FALSE;

  n_args = g_callable_info_get_n_args ((GICallableInfo *) info);

  in_args = g_new0 (GArgument, n_args + 1);
  out_args = g_new0 (GArgument, n_args + 1);
  out_values = g_new0 (GArgument, n_args + 1);
  n_in_args = n_out_args = 0;

  if (instance_method)
    in_args[n_in_args++].v_pointer = object;

  for (i = 0; (i < (n_args)); i++)
    {
      SEED_NOTE (INVOCATION, "Converting arg: %d of function %s, exception is %p", i, g_base_info_get_name (info), exception);
      arg_info = g_callable_info_get_arg ((GICallableInfo *) info, i);
      dir = g_arg_info_get_direction (arg_info);
      type_info = g_arg_info_get_type (arg_info);
      if (i + 1 > argumentCount)
	{
	  in_args[n_in_args++].v_pointer = 0;
	}
      else if (dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT)
	{

	  if (!seed_gi_make_argument (ctx, arguments[i],
				      type_info, arg_info,
				      &in_args[n_in_args++], exception))
	    {
	      seed_make_exception (ctx, exception,
				   "ArgumentError",
				   "Unable to make argument %d for"
				   " function: %s. \n",
				   i + 1,
				   g_base_info_get_name ((GIBaseInfo *) info));

	      g_base_info_unref ((GIBaseInfo *) type_info);
	      g_base_info_unref ((GIBaseInfo *) arg_info);
	      g_free (in_args);
	      g_free (out_args);
	      g_free (out_values);

	      return JSValueMakeNull (ctx);
	    }
	  if (dir == GI_DIRECTION_INOUT)
	    {
	      GArgument *out_value = &out_values[n_out_args];
	      out_args[n_out_args++].v_pointer = out_value;
	    }

	}
      else if (dir == GI_DIRECTION_OUT)
	{
	  GArgument *out_value = &out_values[n_out_args];
	  out_args[n_out_args++].v_pointer = out_value;
	}

      g_base_info_unref ((GIBaseInfo *) type_info);
      g_base_info_unref ((GIBaseInfo *) arg_info);
    }
  SEED_NOTE (INVOCATION, "Invoking method: %s with %d in arguments"
	     " and %d out arguments",
	     g_base_info_get_name (info), n_in_args, n_out_args);
  if (g_function_info_invoke ((GIFunctionInfo *) info,
			      in_args,
			      n_in_args,
			      out_args, n_out_args, &retval, &error))
    {
      GITypeTag tag;

      type_info = g_callable_info_get_return_type ((GICallableInfo *) info);
      tag = g_type_info_get_tag (type_info);
      if (tag == GI_TYPE_TAG_VOID)
	retval_ref = JSValueMakeUndefined (ctx);
      else
	{
	  GIBaseInfo *interface;
	  gboolean sunk = FALSE;

	  if (tag == GI_TYPE_TAG_INTERFACE)
	    {
	      GIInfoType interface_type;

	      interface = g_type_info_get_interface (type_info);
	      interface_type = g_base_info_get_type (interface);
	      g_base_info_unref (interface);

	      if (interface_type == GI_INFO_TYPE_OBJECT ||
		  interface_type == GI_INFO_TYPE_INTERFACE)
		{
		  if (G_IS_OBJECT (retval.v_pointer))
		    {
		      sunk =
			G_IS_INITIALLY_UNOWNED (G_OBJECT (retval.v_pointer));
		      if (sunk)
			g_object_ref_sink (G_OBJECT (retval.v_pointer));
		    }
		}

	    }
	  retval_ref =
	    seed_gi_argument_make_js (ctx, &retval, type_info, exception);

	  if (sunk)
	    g_object_unref (G_OBJECT (retval.v_pointer));
	  else
	    seed_gi_release_arg (g_callable_info_get_caller_owns
				 ((GICallableInfo *) info),
				 type_info, &retval);
	}
      g_base_info_unref ((GIBaseInfo *) type_info);
    }
  else
    {
      seed_make_exception_from_gerror (ctx, exception, error);

      g_free (in_args);
      g_free (out_args);
      g_error_free (error);
      g_free (out_values);

      return JSValueMakeNull (ctx);
    }
  for (i = 0; (i < n_args); i++)
    {
      JSValueRef jsout_val;
      arg_info = g_callable_info_get_arg ((GICallableInfo *) info, i);
      dir = g_arg_info_get_direction (arg_info);
      type_info = g_arg_info_get_type (arg_info);

      if (dir == GI_DIRECTION_IN)
	{
	  seed_gi_release_in_arg (g_arg_info_get_ownership_transfer
				  (arg_info), type_info,
				  &in_args[i + (instance_method ? 1 : 0)]);

	  g_base_info_unref ((GIBaseInfo *) type_info);
	  g_base_info_unref ((GIBaseInfo *) arg_info);
	  continue;
	}
      jsout_val = seed_gi_argument_make_js (ctx, &out_values[i],
					    type_info, exception);
      if (!JSValueIsNull (ctx, arguments[i]) &&
	  JSValueIsObject (ctx, arguments[i]))
	{
	  seed_object_set_property (ctx, (JSObjectRef) arguments[i],
				    "value", jsout_val);
	}

      g_base_info_unref ((GIBaseInfo *) arg_info);
      g_base_info_unref ((GIBaseInfo *) type_info);
    }

  g_free (in_args);
  g_free (out_args);
  g_free (out_values);
  return retval_ref;
}

static JSObjectRef
seed_gobject_named_constructor_invoked (JSContextRef ctx,
					JSObjectRef constructor,
					size_t argumentCount,
					const JSValueRef arguments[],
					JSValueRef * exception)
{
  return (JSObjectRef) seed_gobject_method_invoked (ctx, constructor,
						    NULL, argumentCount,
						    arguments, exception);
}

void
seed_gobject_define_property_from_function_info (JSContextRef ctx,
						 GIFunctionInfo * info,
						 JSObjectRef object,
						 gboolean instance)
{
  GIFunctionInfoFlags flags;
  JSObjectRef method_ref;
  const gchar *name;

  //if (g_base_info_is_deprecated ((GIBaseInfo *) info))
  //g_printf("Not defining deprecated symbol: %s \n",
  //g_base_info_get_name((GIBaseInfo *)info));

  flags = g_function_info_get_flags (info);

  if (instance && (flags & GI_FUNCTION_IS_CONSTRUCTOR))
    {
      return;
    }

  method_ref = JSObjectMake (ctx, gobject_method_class,
			     g_base_info_ref ((GIBaseInfo *) info));

  name = g_base_info_get_name ((GIBaseInfo *) info);
  if (!strcmp (name, "new"))
    name = "c_new";
  seed_object_set_property (ctx, object, name, method_ref);
  seed_object_set_property (ctx, method_ref, "info",
			    seed_make_struct (ctx,
					      g_base_info_ref ((GIBaseInfo *)
							       info),
					      base_info_info));

}

static void
seed_gobject_add_methods_for_interfaces (JSContextRef ctx,
					 GIObjectInfo * oinfo,
					 JSObjectRef object)
{
  GIInterfaceInfo *interface;
  gint n_interfaces, i;

  n_interfaces = g_object_info_get_n_interfaces (oinfo);

  for (i = 0; i < n_interfaces; i++)
    {
      GIFunctionInfo *function;
      gint n_functions, k;
      interface = g_object_info_get_interface (oinfo, i);

      n_functions = g_interface_info_get_n_methods (interface);
      for (k = 0; k < n_functions; k++)
	{
	  function = g_interface_info_get_method (interface, k);
	  seed_gobject_define_property_from_function_info
	    (ctx, function, object, TRUE);
	}
      // g_base_info_unref((GIBaseInfo*)interface);
    }
}

static void
seed_gobject_add_methods_for_type (JSContextRef ctx,
				   GIObjectInfo * oinfo, JSObjectRef object)
{
  gint n_methods;
  gint i;
  GIFunctionInfo *info;

  n_methods = g_object_info_get_n_methods (oinfo);

  for (i = 0; i < n_methods; i++)
    {
      info = g_object_info_get_method (oinfo, i);
      seed_gobject_define_property_from_function_info (ctx,
						       info, object, TRUE);
      g_base_info_unref ((GIBaseInfo *) info);
    }
}

JSClassRef
seed_gobject_get_class_for_gtype (JSContextRef ctx, GType type)
{
  JSClassDefinition def;
  GType parent;
  JSClassRef ref;
  JSClassRef parent_class = 0;
  GIBaseInfo *info;
  JSObjectRef prototype_obj;
  JSObjectRef parent_prototype;

  if ((ref = g_type_get_qdata (type, qname)) != NULL)
    {
      return ref;
    }

  info = g_irepository_find_by_gtype (g_irepository_get_default (), type);

  memset (&def, 0, sizeof (JSClassDefinition));

  def.className = g_type_name (type);
  if ((parent = g_type_parent (type)))
    parent_class = seed_gobject_get_class_for_gtype (ctx, parent);
  def.parentClass = parent_class;
  def.attributes = kJSClassAttributeNoAutomaticPrototype;

  prototype_obj = JSObjectMake (ctx, 0, 0);
  if (parent)
    {
      parent_prototype = seed_gobject_get_prototype_for_gtype (parent);
      if (parent_prototype)
	JSObjectSetPrototype (ctx, prototype_obj, parent_prototype);
    }

  ref = JSClassCreate (&def);
  JSClassRetain (ref);

  JSValueProtect (ctx, prototype_obj);

  g_type_set_qdata (type, qname, ref);
  g_type_set_qdata (type, qprototype, prototype_obj);

  if (info && (g_base_info_get_type (info) == GI_INFO_TYPE_OBJECT))
    {
      seed_gobject_add_methods_for_type (ctx,
					 (GIObjectInfo *) info,
					 prototype_obj);
      seed_gobject_add_methods_for_interfaces (ctx, (GIObjectInfo *) info,
					       prototype_obj);
      g_base_info_unref (info);
    }
  else
    {
      GType *interfaces;
      GIFunctionInfo *function;
      GIBaseInfo *interface;
      gint n_functions, k, i;
      guint n;

      interfaces = g_type_interfaces (type, &n);
      for (i = 0; i < n; i++)
	{
	  interface = g_irepository_find_by_gtype (0, interfaces[i]);
	  if (!interface)
	    break;
	  n_functions =
	    g_interface_info_get_n_methods ((GIInterfaceInfo *) interface);
	  for (k = 0; k < n_functions; k++)
	    {
	      function =
		g_interface_info_get_method ((GIInterfaceInfo
					      *) interface, k);
	      seed_gobject_define_property_from_function_info
		(ctx, function, prototype_obj, TRUE);
	    }
	}
    }

  return ref;
}

JSObjectRef
seed_gobject_get_prototype_for_gtype (GType type)
{
  JSObjectRef prototype = 0;
  while (type && !prototype)
    {
      prototype = g_type_get_qdata (type, qprototype);
      type = g_type_parent (type);
    }

  return prototype;
}

static void
seed_gobject_finalize (JSObjectRef object)
{
  GObject *gobject;

  gobject = (GObject *) JSObjectGetPrivate ((JSObjectRef) object);
  if (!gobject)
    {
      SEED_NOTE (FINALIZATION,
		 "Attempting to finalize already destroyed object.");
      return;
    }

  SEED_NOTE (FINALIZATION, "%s at %p (%d refs)",
	     g_type_name (G_OBJECT_TYPE (gobject)), gobject,
	     gobject->ref_count);

  if (g_object_get_data (gobject, "js-ref"))
    {
      g_object_set_data_full (gobject, "js-ref", NULL, NULL);

      g_object_remove_toggle_ref (gobject, seed_toggle_ref, 0);
    }
  g_object_run_dispose (gobject);
}

static JSValueRef
seed_gobject_get_property (JSContextRef context,
			   JSObjectRef object,
			   JSStringRef property_name, JSValueRef * exception)
{
  GParamSpec *spec;
  GObject *b;
  GValue gval = { 0 };
  char *cproperty_name;
  gint length;
  JSValueRef ret;
  gint i, len;

  b = seed_value_to_object (context, (JSValueRef) object, exception);
  if (!b)
    return 0;

  length = JSStringGetMaximumUTF8CStringSize (property_name);
  cproperty_name = g_alloca (length * sizeof (gchar));
  JSStringGetUTF8CString (property_name, cproperty_name, length);

  spec =
    g_object_class_find_property (G_OBJECT_GET_CLASS (b), cproperty_name);

  if (!spec)
    {

      len = strlen (cproperty_name);
      for (i = 0; i < len - 1; i++)
	{
	  if (cproperty_name[i] == '_')
	    cproperty_name[i] = '-';
	}
      spec = g_object_class_find_property (G_OBJECT_GET_CLASS (b),
					   cproperty_name);
      if (spec)
	goto found;
      else
	{
	  GIFieldInfo *field = 0;
	  GIBaseInfo *info = (GIBaseInfo *)
	    g_irepository_find_by_gtype (0, G_OBJECT_TYPE (b));
	  gint n;
	  const gchar *name;

	  for (i = 0; i < len - 1; i++)
	    {
	      if (cproperty_name[i] == '-')
		cproperty_name[i] = '_';
	    }

	  if (!info)
	    {
	      return 0;
	    }

	  n = g_object_info_get_n_fields ((GIObjectInfo *) info);
	  for (i = 0; i < n; i++)
	    {
	      field = g_object_info_get_field ((GIObjectInfo *) info, i);
	      name = g_base_info_get_name ((GIBaseInfo *) field);

	      if (!strcmp (name, cproperty_name))
		goto found_field;
	      else
		{
		  g_base_info_unref ((GIBaseInfo *) field);
		  field = 0;
		}
	    }
	found_field:
	  if (field)
	    {
	      ret = seed_field_get_value (context, b, field, exception);
	      g_base_info_unref ((GIBaseInfo *) info);
	      return ret;
	    }
	  g_base_info_unref ((GIBaseInfo *) info);
	}
      return 0;
    }
found:

  g_value_init (&gval, spec->value_type);
  g_object_get_property (b, cproperty_name, &gval);
  ret = seed_value_from_gvalue (context, &gval, exception);
  g_value_unset (&gval);

  return (JSValueRef) ret;
}

static bool
seed_gobject_set_property (JSContextRef context,
			   JSObjectRef object,
			   JSStringRef property_name,
			   JSValueRef value, JSValueRef * exception)
{
  GParamSpec *spec = 0;
  GObject *obj;
  GValue gval = { 0 };
  GType type;
  gchar *cproperty_name;
  gint length;

  if (seed_next_gobject_wrapper || JSValueIsNull (context, value))
    return 0;


  obj = seed_value_to_object (context, object, 0);

  length = JSStringGetMaximumUTF8CStringSize (property_name);
  cproperty_name = g_alloca (length * sizeof (gchar));
  JSStringGetUTF8CString (property_name, cproperty_name, length);

  spec = g_object_class_find_property (G_OBJECT_GET_CLASS (obj),
				       cproperty_name);

  if (!spec)
    {
      gint i, len;
      len = strlen (cproperty_name);
      for (i = 0; i < len; i++)
	{
	  if (cproperty_name[i] == '_')
	    cproperty_name[i] = '-';
	}
      spec = g_object_class_find_property (G_OBJECT_GET_CLASS (obj),
					   cproperty_name);
      if (!spec)
	{
	  return 0;
	}
    }

  if (g_type_is_a (spec->value_type, G_TYPE_ENUM))
    type = G_TYPE_LONG;
  else
    type = spec->value_type;

  seed_gvalue_from_seed_value (context, value, type, &gval, exception);
  if (*exception)
    {
      return 0;
    }

  if (glib_message)
    {
      g_free (glib_message);
      glib_message = 0;
    }
  g_object_set_property (obj, cproperty_name, &gval);
  if (glib_message != 0)
    {
      seed_make_exception (context, exception, "PropertyError",
			   glib_message, NULL);

      return FALSE;
    }

  g_value_unset (&gval);

  return TRUE;
}

static JSValueRef
seed_gobject_constructor_convert_to_type (JSContextRef ctx,
					  JSObjectRef object,
					  JSType type,
					  JSValueRef *exception)
{
  GType gtype;

  if (type == kJSTypeString)
    {
      JSValueRef ret;
      gchar *as_string;
      gtype = (GType) JSObjectGetPrivate (object);

      as_string = g_strdup_printf("[gobject_constructor %s]", g_type_name (gtype));
      ret = seed_value_from_string (ctx, as_string, exception);
      g_free (as_string);

      return ret;
    }
  return FALSE;
}

JSStaticFunction gobject_static_funcs[] = {
  {"__debug_ref_count", seed_gobject_ref_count, 0},
  {"__property_type", seed_gobject_property_type, 0},
  {0, 0, 0}
};

JSClassDefinition gobject_def = {
  0,				/* Version, always 0 */
  kJSClassAttributeNoAutomaticPrototype,	/* JSClassAttributes */
  "gobject",			/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  gobject_static_funcs,		/* Static Functions */
  NULL,
  seed_gobject_finalize,	/* Finalize */
  NULL,				/* Has Property */
  seed_gobject_get_property,	/* Get Property */
  seed_gobject_set_property,	/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition gobject_method_def = {
  0,				/* Version, always 0 */
  0,
  "gobject_method",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  seed_gobject_method_finalize,	/* Finalize */
  NULL,				/* Has Property */
  NULL,				/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  seed_gobject_method_invoked,	/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition gobject_init_method_def = {
  0,				/* Version, always 0 */
  0,
  "init_method",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  seed_gobject_method_finalize,	/* Finalize */
  NULL,				/* Has Property */
  NULL,				/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  seed_gobject_init_method_invoked,	/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition seed_callback_def = {
  0,				/* Version, always 0 */
  0,
  "seed_callback",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  NULL,				/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,				/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

JSClassDefinition gobject_constructor_def = {
  0,				/* Version, always 0 */
  0,
  "gobject_constructor",	/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  NULL,				/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  seed_gobject_constructor_invoked,	/* Call As Constructor */
  NULL,				/* Has Instance */
  seed_gobject_constructor_convert_to_type
};

JSClassDefinition gobject_named_constructor_def = {
  0,				/* Version, always 0 */
  0,
  "gobject_named_constructor",	/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  NULL,				/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  seed_gobject_named_constructor_invoked,	/* Call As Constructor */
  NULL,				/* Has Instance */
  seed_gobject_constructor_convert_to_type
};

JSClassDefinition struct_constructor_def = {
  0,				/* Version, always 0 */
  0,
  "struct_constructor",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  NULL,				/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  seed_struct_constructor_invoked,	/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

void
seed_create_function (JSContextRef ctx,
		      gchar * name,
		      gpointer func,
		      JSObjectRef obj)
{
  JSObjectRef oref;

  oref = JSObjectMakeFunctionWithCallback (ctx, NULL, func);
  seed_object_set_property (ctx, obj, name, oref);
}

void
seed_repl_expose (JSContextRef ctx, ...)
{
  va_list argp;
  void *expose;
  JSObjectRef arrayObj;
  int i = 0;
  JSStringRef script;
  JSObjectRef seed = (JSObjectRef) seed_object_get_property (ctx,
							     JSContextGetGlobalObject
							     (ctx),
							     "Seed");
  va_start (argp, ctx);

  arrayObj = JSObjectMake (ctx, NULL, NULL);

  g_print ("Seed Debug REPL\n\nExposing:\n");

  while ((expose = va_arg (argp, void *)))
    {
      g_print ("  Seed.debug_argv[%d] = %p\n", i, expose);
      JSObjectSetPropertyAtIndex (ctx, arrayObj, i++, expose, NULL);
    }

  g_print ("\n");

  seed_object_set_property (ctx, seed, "debug_argv", arrayObj);

  script = JSStringCreateWithUTF8CString ("readline = imports.readline;"
					  "while(1) { try { print(eval("
					  "readline.readline(\"> \"))); } catch(e) {"
					  "print(e.name + \" \" + e.message);}}");

  JSEvaluateScript (ctx, script, NULL, NULL, 0, NULL);

  JSStringRelease (script);

  va_end (argp);
}

static void
seed_log_handler (const gchar * domain,
		  GLogLevelFlags log_level,
		  const gchar * message, gpointer user_data)
{
  if (glib_message)
    g_free (glib_message);
  glib_message = g_strdup (message);
}

#ifdef SEED_ENABLE_DEBUG
static gboolean
seed_arg_debug_cb (const char *key, const char *value, gpointer user_data)
{
  seed_debug_flags |=
    g_parse_debug_string (value,
			  seed_debug_keys, G_N_ELEMENTS (seed_debug_keys));
  return TRUE;
}

static gboolean
seed_arg_no_debug_cb (const char *key, const char *value, gpointer user_data)
{
  seed_debug_flags &=
    ~g_parse_debug_string (value,
			   seed_debug_keys, G_N_ELEMENTS (seed_debug_keys));
  return TRUE;
}
#endif /* SEED_ENABLE_DEBUG */

static GOptionEntry seed_args[] = {
#ifdef SEED_ENABLE_DEBUG
  {"seed-debug", 0, 0, G_OPTION_ARG_CALLBACK, seed_arg_debug_cb,
   "Seed debugging messages to show. Comma separated list of: all, misc, finalization, initialization, construction, invocation, signal, structs, gtype.",
   "FLAGS"},
  {"seed-no-debug", 0, 0, G_OPTION_ARG_CALLBACK, seed_arg_no_debug_cb,
   "Disable Seed debugging", "FLAGS"},
#endif /* SEED_ENABLE_DEBUG */
  {NULL,},
};

GOptionGroup *
seed_get_option_group (void)
{
  GOptionGroup *group;

  group = g_option_group_new ("seed", "Seed Options",
			      "Show Seed Options", NULL, NULL);
  g_option_group_add_entries (group, seed_args);

  return group;
}

static gboolean
seed_parse_args (int *argc, char ***argv)
{
  GOptionContext *option_context;
  GOptionGroup *seed_group;
  GError *error = NULL;
  gboolean ret = TRUE;

  option_context = g_option_context_new (NULL);
  g_option_context_set_ignore_unknown_options (option_context, TRUE);
  g_option_context_set_help_enabled (option_context, TRUE);

  /* Initiate any command line options from the backend */

  seed_group = seed_get_option_group ();
  g_option_context_set_main_group (option_context, seed_group);

  if (!g_option_context_parse (option_context, argc, argv, &error))
    {
      if (error)
	{
	  g_warning ("%s", error->message);
	  g_error_free (error);
	}

      ret = FALSE;
    }

  g_option_context_free (option_context);

  return ret;
}

/**
 * seed_init:
 * @argc: A reference to the number of arguments remaining to parse.
 * @argv: A reference to an array of string arguments remaining to parse.
 *
 * Initializes a new #SeedEngine.
 *
 * Return value: The newly created and initialized #SeedEngine.
 *
 */
SeedEngine *
seed_init (gint * argc, gchar *** argv)
{
  g_type_init ();
  g_log_set_handler ("GLib-GObject", G_LOG_LEVEL_WARNING, seed_log_handler,
		     0);

  if ((argc != 0) && seed_parse_args (argc, argv) == FALSE)
    {
      SEED_NOTE (MISC, "failed to parse arguments.");
      return false;
    }

  qname = g_quark_from_static_string ("js-type");
  qprototype = g_quark_from_static_string ("js-prototype");
  js_ref_quark = g_quark_from_static_string ("js-ref");

  eng = (SeedEngine *) g_malloc (sizeof (SeedEngine));

  context_group = JSContextGroupCreate ();

  eng->context = JSGlobalContextCreateInGroup (context_group, NULL);
  eng->global = JSContextGetGlobalObject (eng->context);
  eng->group = context_group;
  eng->search_path = NULL;

  gobject_class = JSClassCreate (&gobject_def);
  JSClassRetain (gobject_class);
  gobject_method_class = JSClassCreate (&gobject_method_def);
  JSClassRetain (gobject_method_class);
  gobject_constructor_class = JSClassCreate (&gobject_constructor_def);
  JSClassRetain (gobject_constructor_class);
  gobject_named_constructor_class =
    JSClassCreate (&gobject_named_constructor_def);
  JSClassRetain (gobject_named_constructor_class);
  gobject_signal_class = JSClassCreate (seed_get_signal_class ());
  JSClassRetain (gobject_signal_class);
  seed_callback_class = JSClassCreate (&seed_callback_def);
  JSClassRetain (seed_callback_class);
  seed_struct_constructor_class = JSClassCreate (&struct_constructor_def);
  JSClassRetain (seed_struct_constructor_class);
  gobject_init_method_class = JSClassCreate (&gobject_init_method_def);
  JSClassRetain (gobject_init_method_class);

  g_type_set_qdata (G_TYPE_OBJECT, qname, gobject_class);

  seed_obj_ref = JSObjectMake (eng->context, NULL, NULL);
  seed_object_set_property (eng->context, eng->global, "Seed", seed_obj_ref);
  JSValueProtect (eng->context, seed_obj_ref);

  g_irepository_require (g_irepository_get_default (), "GObject", NULL, 0, 0);
  g_irepository_require (g_irepository_get_default (), "GIRepository",
			 NULL, 0, 0);

  seed_initialize_importer (eng->context, eng->global);

  seed_init_builtins (eng, argc, argv);
  seed_closures_init ();
  seed_structs_init ();

  seed_gtype_init (eng);



  defaults_script =
	  JSStringCreateWithUTF8CString ("Seed.include(\""SEED_PREFIX_PATH"extensions/Seed.js\");");

  JSEvaluateScript (eng->context, defaults_script, NULL, NULL, 0, NULL);

  base_info_info =
    g_irepository_find_by_name (0, "GIRepository", "IBaseInfo");

  return eng;
}



SeedEngine *
seed_init_with_context_group (gint * argc,
			      gchar *** argv,
			      JSContextGroupRef group)
{
  JSStringRef defaults_script;

  g_type_init ();
  g_log_set_handler ("GLib-GObject", G_LOG_LEVEL_WARNING, seed_log_handler,
		     0);

  if ((argc != 0) && seed_parse_args (argc, argv) == FALSE)
    {
      SEED_NOTE (MISC, "failed to parse arguments.");
      return false;
    }

  qname = g_quark_from_static_string ("js-type");
  qprototype = g_quark_from_static_string ("js-prototype");
  js_ref_quark = g_quark_from_static_string ("js-ref");

  eng = (SeedEngine *) g_malloc (sizeof (SeedEngine));

  context_group = group;

  eng->context = JSGlobalContextCreateInGroup (context_group, NULL);
  eng->global = JSContextGetGlobalObject (eng->context);
  eng->group = context_group;
  eng->search_path = NULL;

  gobject_class = JSClassCreate (&gobject_def);
  JSClassRetain (gobject_class);
  gobject_method_class = JSClassCreate (&gobject_method_def);
  JSClassRetain (gobject_method_class);
  gobject_constructor_class = JSClassCreate (&gobject_constructor_def);
  JSClassRetain (gobject_constructor_class);
  gobject_named_constructor_class =
    JSClassCreate (&gobject_named_constructor_def);
  JSClassRetain (gobject_named_constructor_class);
  gobject_signal_class = JSClassCreate (seed_get_signal_class ());
  JSClassRetain (gobject_signal_class);
  seed_callback_class = JSClassCreate (&seed_callback_def);
  JSClassRetain (seed_callback_class);
  seed_struct_constructor_class = JSClassCreate (&struct_constructor_def);
  JSClassRetain (seed_struct_constructor_class);
  gobject_init_method_class = JSClassCreate (&gobject_init_method_def);
  JSClassRetain (gobject_init_method_class);

  g_type_set_qdata (G_TYPE_OBJECT, qname, gobject_class);

  seed_obj_ref = JSObjectMake (eng->context, NULL, NULL);
  seed_object_set_property (eng->context, eng->global, "Seed", seed_obj_ref);
  JSValueProtect (eng->context, seed_obj_ref);

  g_irepository_require (g_irepository_get_default (), "GObject", NULL, 0, 0);
  g_irepository_require (g_irepository_get_default (), "GIRepository",
			 NULL, 0, 0);

  seed_initialize_importer (eng->context, eng->global);

  seed_init_builtins (eng, argc, argv);
  seed_closures_init ();
  seed_structs_init ();

  seed_gtype_init (eng);

  defaults_script =
	  JSStringCreateWithUTF8CString ("Seed.include(\""SEED_PREFIX_PATH"extensions/Seed.js\");");
  JSEvaluateScript (eng->context, defaults_script, NULL, NULL, 0, NULL);
  JSStringRelease (defaults_script);

  base_info_info =
    g_irepository_find_by_name (0, "GIRepository", "IBaseInfo");

  return eng;
}
