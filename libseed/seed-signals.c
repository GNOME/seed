/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */
/* vim: set sw=2 ts=2 sts=2 et: */

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

typedef struct _signal_privates
{
  GObject *object;
  gchar *signal_name;
} signal_privates;

JSClassRef signal_holder_class;

static void
seed_signal_finalize (JSObjectRef object)
{
  signal_privates *sig_priv = JSObjectGetPrivate (object);

  g_free (sig_priv->signal_name);
  g_slice_free1 (sizeof (signal_privates), sig_priv);
}


gulong
seed_gobject_signal_connect (JSContextRef ctx,
			     const gchar * signal_name,
			     GObject * on_obj,
			     JSObjectRef func,
			     JSObjectRef this_obj, JSObjectRef user_data)
{
  GSignalQuery query;
  GClosure *closure;

  if (g_str_has_prefix (signal_name, "notify::"))
    g_signal_query (g_signal_lookup ("notify", G_OBJECT_TYPE (on_obj)),
                    &query);
  else
    g_signal_query (g_signal_lookup (signal_name, G_OBJECT_TYPE (on_obj)),
                    &query);

#ifdef SEED_ENABLE_DEBUG
  {
    guint function_arity = seed_value_to_uint (ctx,
					       seed_object_get_property (ctx,
									 func,
									 "length"),
					       NULL);
    if (function_arity != query.n_params)
      {
	SEED_MARK ();
	SEED_NOTE (SIGNAL,
		   "Connecting signal: %s. Function has arity %d, signal expects %d",
		   query.signal_name, function_arity, query.n_params);
	SEED_MARK ();
      }
  }
#endif

  closure = seed_closure_new_for_signal (ctx, func, user_data, "signal handler", query.signal_id);

  // This seems wrong...
  ((SeedClosure *) closure)->return_type = query.return_type;
  return g_signal_connect_closure (on_obj, signal_name, closure, FALSE);
}

static JSValueRef
seed_gobject_signal_connect_by_name (JSContextRef ctx,
				     JSObjectRef function,
				     JSObjectRef thisObject,
				     size_t argumentCount,
				     const JSValueRef arguments[],
				     JSValueRef * exception)
{
  GType obj_type;
  JSObjectRef user_data = NULL;
  gchar *signal_name;
  GObject *obj;
  gulong id;

  if (argumentCount < 2 || argumentCount > 3)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal connection expected"
			   " 2 or 3 arguments. Got " "%zd", argumentCount);

      return JSValueMakeNull (ctx);
    }

  if (JSValueIsNull (ctx, arguments[1]) ||
      !JSValueIsObject (ctx, arguments[1]) ||
      !JSObjectIsFunction (ctx, (JSObjectRef) arguments[1]))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal connection by name "
			   "requires a function" " as second argument");
      return JSValueMakeNull (ctx);
    }

  if (argumentCount == 3)
    {
      user_data = (JSObjectRef) arguments[2];
    }

  signal_name = seed_value_to_string (ctx, arguments[0], exception);
  obj = (GObject *) JSObjectGetPrivate (thisObject);
  obj_type = G_OBJECT_TYPE (obj);

  id = seed_gobject_signal_connect (ctx, signal_name, obj,
				    (JSObjectRef) arguments[1], NULL,
				    user_data);

  g_free (signal_name);

  return seed_value_from_ulong (ctx, id, exception);
}

void
seed_add_signals_to_object (JSContextRef ctx,
			    JSObjectRef object_ref, GObject * obj)
{
  GType type;
  JSObjectRef signals_ref;

  g_assert (obj);

  type = G_OBJECT_TYPE (obj);

  signals_ref = JSObjectMake (ctx, signal_holder_class, obj);

  seed_object_set_property (ctx, object_ref, "signal", signals_ref);
}

void
seed_signal_marshal_func (GClosure * closure,
			  GValue * return_value,
			  guint n_param_values,
			  const GValue * param_values,
			  gpointer invocation_hint, gpointer marshal_data)
{
  SeedClosure *seed_closure = (SeedClosure *) closure;
  JSValueRef *args, exception = 0;
  JSValueRef ret = 0;
  guint i;
  gchar *mes;
  GSignalQuery signal_query = { 0, };

  if (marshal_data)
    {
      /* Inspired from gjs/gi/value.c:closure_marshal() */
      /* we are used for a signal handler */
      guint signal_id;

      signal_id = GPOINTER_TO_UINT(marshal_data);

      g_signal_query(signal_id, &signal_query);

      if (!signal_query.signal_id)
          g_error("Signal handler being called on invalid signal");

      if (signal_query.n_params + 1 != n_param_values)
          g_error("Signal handler being called with wrong number of parameters");
  }

  JSContextRef ctx = JSGlobalContextCreateInGroup (context_group,
						   0);

  seed_prepare_global_context (ctx);
  SEED_NOTE (INVOCATION, "Signal Marshal: ");

  args = g_newa (JSValueRef, n_param_values + 1);

  for (i = 0; i < n_param_values; i++)
    {
      args[i] = seed_value_from_gvalue_for_signal (ctx,
                    (GValue *) & param_values[i], 0, &signal_query, i);

      if (!args[i])
	g_error ("Error in signal marshal. "
		 "Unable to convert argument of type: %s \n",
		 g_type_name (param_values[i].g_type));

    }

  if (seed_closure->user_data)
    args[i] = seed_closure->user_data;
  else
    args[i] = JSValueMakeNull (ctx);

  ret = JSObjectCallAsFunction (ctx, seed_closure->function,
				NULL, n_param_values + 1, args, &exception);

  if (exception)
    {
      seed_closure_warn_exception (closure, ctx, exception);
      exception = NULL;
    }

  if (ret && !JSValueIsNull (ctx, ret)
      && (seed_closure->return_type != G_TYPE_NONE))
    {
      seed_value_to_gvalue (ctx, ret, seed_closure->return_type,
			    return_value, &exception);
    }

  if (exception)
    {
      mes = seed_exception_to_string (ctx, exception);
      g_warning ("Exception in signal handler return value. %s \n", mes);
      g_free (mes);
    }

  JSGlobalContextRelease ((JSGlobalContextRef) ctx);
  JSGarbageCollect(ctx);

}

static JSValueRef
seed_gobject_signal_emit (JSContextRef ctx,
			  JSObjectRef function,
			  JSObjectRef thisObject,
			  size_t argumentCount,
			  const JSValueRef arguments[],
			  JSValueRef * exception)
{
  JSValueRef ret;
  GValue *params;
  GValue ret_value = { 0 };
  GSignalQuery query;

  signal_privates *privates;
  guint i, signal_id;

  privates = JSObjectGetPrivate (thisObject);

  signal_id = g_signal_lookup (privates->signal_name,
			       G_OBJECT_TYPE (privates->object));

  g_signal_query (signal_id, &query);

  if (argumentCount != query.n_params)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal: %s for type %s expected %u "
			   "arguments, got %zd",
			   query.signal_name,
			   g_type_name (query.itype),
			   query.n_params, argumentCount);

      return JSValueMakeNull (ctx);
    }

  params = g_new0 (GValue, argumentCount + 1);

  g_value_init (&params[0], G_TYPE_OBJECT);
  g_value_set_object (&params[0], privates->object);
  for (i = 0; i < argumentCount; i++)
    seed_value_to_gvalue (ctx, arguments[i],
			  query.param_types[i],
			  &params[i + 1], exception);


  if (query.return_type != G_TYPE_NONE)
    g_value_init (&ret_value, query.return_type);
  g_signal_emitv (params, signal_id, 0, &ret_value);

  for (i = 0; i < argumentCount; i++)
    g_value_unset (&params[i]);
  g_free (params);

  ret = seed_value_from_gvalue (ctx, &ret_value, exception);

  if (query.return_type != G_TYPE_NONE)
    g_value_unset (&ret_value);

  return ret;
}

static JSValueRef
seed_gobject_signal_disconnect (JSContextRef ctx,
				JSObjectRef function,
				JSObjectRef thisObject,
				size_t argumentCount,
				const JSValueRef arguments[],
				JSValueRef * exception)
{
  gulong id;
  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal disconnection expects 1 argument"
			   " got %zd", argumentCount);
      return JSValueMakeUndefined (ctx);
    }
  id = seed_value_to_ulong (ctx, arguments[0], exception);
  g_signal_handler_disconnect (JSObjectGetPrivate (thisObject), id);

  return JSValueMakeUndefined (ctx);
}

static JSValueRef
seed_gobject_signal_connect_on_property (JSContextRef ctx,
					 JSObjectRef function,
					 JSObjectRef thisObject,
					 size_t argumentCount,
					 const JSValueRef arguments[],
					 JSValueRef * exception)
{
  gulong id = 0;
  JSObjectRef this_obj;
  signal_privates *privates;

  privates = (signal_privates *) JSObjectGetPrivate (thisObject);
  if (!privates)
    g_error ("Signal constructed with invalid parameters"
	     "in namespace import \n");

  this_obj =
    (JSObjectRef) seed_value_from_object (ctx, privates->object, exception);

  if ((argumentCount > 2) || (argumentCount == 0))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal connection expected"
			   " 1, or 2 arguments. Got " "%zd", argumentCount);

      return JSValueMakeNull (ctx);
    }

  if (JSValueIsNull (ctx, arguments[0]) ||
      !JSValueIsObject (ctx, arguments[0]) ||
      !JSObjectIsFunction (ctx, (JSObjectRef) arguments[0]))
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "Signal connection requires a function"
			   " as first argument");
      return JSValueMakeNull (ctx);
    }

  if (argumentCount == 1)
    {
      id = seed_gobject_signal_connect (ctx, privates->signal_name,
					privates->object,
					(JSObjectRef) arguments[0], this_obj,
					NULL);

    }
  else if (argumentCount == 2)
    {
      id = seed_gobject_signal_connect (ctx, privates->signal_name,
					privates->object,
					(JSObjectRef) arguments[0],
					this_obj, (JSObjectRef) arguments[1]);
    }

  return seed_value_from_ulong (ctx, id, exception);
}

JSStaticFunction signal_static_functions[] = {
  {"connect", seed_gobject_signal_connect_on_property, 0}
  ,
  {"emit", seed_gobject_signal_emit, 0}
  ,
  {0, 0, 0}
};

JSStaticFunction signal_holder_static_functions[] = {
  {"connect", seed_gobject_signal_connect_by_name, 0}
  ,
  {"disconnect", seed_gobject_signal_disconnect, 0}
  ,
  {0, 0, 0}
};

JSClassDefinition gobject_signal_def = {
  0,				/* Version, always 0 */
  kJSClassAttributeNoAutomaticPrototype,
  "gobject_signal",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  signal_static_functions,	/* Static Functions */
  NULL,
  seed_signal_finalize,		/* Finalize */
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

static JSValueRef
seed_signal_holder_get_property (JSContextRef ctx,
				 JSObjectRef object,
				 JSStringRef property_name,
				 JSValueRef * exception)
{
  GObject *gobj = JSObjectGetPrivate (object);
  signal_privates *priv;
  guint length = JSStringGetMaximumUTF8CStringSize (property_name);
  gchar *signal_name = g_malloc (length * sizeof (gchar));
  JSObjectRef signal_ref;

  JSStringGetUTF8CString (property_name, signal_name, length);

  if (!
      (g_strcmp0 (signal_name, "connect")
       && g_strcmp0 (signal_name, "disconnect")))
    {
      g_free (signal_name);
      return NULL;
    }

  if (!g_str_has_prefix (signal_name, "notify::") &&
      !g_signal_lookup (signal_name, G_OBJECT_TYPE (gobj)))
    {
      seed_make_exception (ctx, exception, "InvalidSignalName",
			   "Failed to connect to %s. "
			   "Invalid signal name.", signal_name);
      g_free (signal_name);
      return NULL;
    }

  priv = g_slice_alloc (sizeof (signal_privates));

  priv->object = gobj;
  priv->signal_name = signal_name;

  signal_ref = JSObjectMake (ctx, gobject_signal_class, priv);

  return signal_ref;
}

JSClassDefinition *
seed_get_signal_class (void)
{
  JSClassDefinition signal_holder = kJSClassDefinitionEmpty;

  signal_holder.className = "gobject_signals";
  signal_holder.getProperty = seed_signal_holder_get_property;
  signal_holder.staticFunctions = signal_holder_static_functions;
  signal_holder_class = JSClassCreate (&signal_holder);
  JSClassRetain (signal_holder_class);

  return &gobject_signal_def;
}
