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
#include <sys/types.h>
#include <sys/mman.h>

JSClassRef seed_native_callback_class;

static void
seed_closure_finalize (JSObjectRef object)
{
  SeedNativeClosure *privates =
    (SeedNativeClosure *) JSObjectGetPrivate (object);

  SEED_NOTE (FINALIZATION, "Finalizing closure object %p with "
	     "GIBaseInfo: %s \n", object,
	     g_base_info_get_name ((GIBaseInfo *) privates->info));

  g_free (privates->cif->arg_types);
  g_free (privates->cif);
  g_callable_info_free_closure (privates->info, privates->closure);
  g_base_info_unref ((GIBaseInfo *) privates->info);

  JSValueUnprotect (eng->context, object);
}

static void
seed_handle_closure (ffi_cif * cif, void *result, void **args, gpointer userdata)
{
  SeedNativeClosure *privates = userdata;
  gint num_args, i;
  JSValueRef *jsargs;
  JSValueRef return_value, exception = 0;
  GITypeTag return_tag;
  GIArgInfo *arg_info;
  GITypeInfo *return_type;
  GITypeInfo *arg_type;
  GITypeTag tag;
  GArgument rarg, return_arg;
  JSContextRef ctx =  JSGlobalContextCreateInGroup (
      context_group, 0);
  GArgument *arg = &rarg;
  gchar *mes;

  seed_prepare_global_context (ctx);

  SEED_NOTE (INVOCATION, "Invoking closure of type: %s \n",
	     g_base_info_get_name ((GIBaseInfo *) privates->info));

  num_args = g_callable_info_get_n_args (privates->info);
  return_type = g_callable_info_get_return_type (privates->info);
  return_tag = g_type_info_get_tag (return_type);
  jsargs = (JSValueRef *) g_newa (JSValueRef, num_args);

  for (i = 0; i < num_args; i++)
    {

      arg_info = g_callable_info_get_arg (privates->info, i);
      arg_type = g_arg_info_get_type (arg_info);
      tag = g_type_info_get_tag (arg_type);

      switch (tag)
	{
#if GOBJECT_INTROSPECTION_VERSION < 0x000900
	case GI_TYPE_TAG_LONG:
	  arg->v_long = *(glong *) args[i];
	  break;
	case GI_TYPE_TAG_ULONG:
	  arg->v_ulong = *(gulong *) args[i];
	  break;
	case GI_TYPE_TAG_INT:
	  arg->v_int = *(gint *) args[i];
	  break;
	case GI_TYPE_TAG_SSIZE:
	  arg->v_ssize = *(gssize *) args[i];
	  break;
	case GI_TYPE_TAG_SIZE:
	  arg->v_size = *(gsize *) args[i];
	  break;
	case GI_TYPE_TAG_UINT:
	  arg->v_uint = *(guint *) args[i];
	  break;
#endif
	case GI_TYPE_TAG_BOOLEAN:
	  arg->v_boolean = *(gboolean *) args[i];
	  break;
	case GI_TYPE_TAG_INT8:
	  arg->v_int8 = *(gint8 *) args[i];
	  break;
	case GI_TYPE_TAG_UINT8:
	  arg->v_uint8 = *(guint8 *) args[i];
	  break;
	case GI_TYPE_TAG_INT16:
	  arg->v_int16 = *(gint16 *) args[i];
	  break;
	case GI_TYPE_TAG_UINT16:
	  arg->v_uint16 = *(guint16 *) args[i];
	  break;
	case GI_TYPE_TAG_INT32:
	  arg->v_int32 = *(gint32 *) args[i];
	  break;
	case GI_TYPE_TAG_UINT32:
	  arg->v_uint32 = *(guint32 *) args[i];
	  break;
	case GI_TYPE_TAG_INT64:
	  arg->v_int64 = *(gint64 *) args[i];
	  break;
	case GI_TYPE_TAG_UINT64:
	  arg->v_uint64 = *(guint64 *) args[i];
	  break;
	case GI_TYPE_TAG_FLOAT:
	  arg->v_float = *(gfloat *) args[i];
	  break;
	case GI_TYPE_TAG_DOUBLE:
	  arg->v_double = *(gdouble *) args[i];
	  break;
	case GI_TYPE_TAG_UTF8:
	  arg->v_string = *(gchar **) args[i];
	  break;
	case GI_TYPE_TAG_INTERFACE:
	  {
	    GIBaseInfo *interface;
	    GIInfoType interface_type;

	    interface = g_type_info_get_interface (arg_type);
	    interface_type = g_base_info_get_type (interface);

	    if (interface_type == GI_INFO_TYPE_OBJECT ||
		interface_type == GI_INFO_TYPE_INTERFACE)
	      {
		arg->v_pointer = *(gpointer *) args[i];
		g_base_info_unref (interface);
		break;
	      }

	    else if (interface_type == GI_INFO_TYPE_ENUM ||
		     interface_type == GI_INFO_TYPE_FLAGS)
	      {
		arg->v_double = *(double *) args[i];
		g_base_info_unref (interface);
		break;
	      }
	    else if (interface_type == GI_INFO_TYPE_STRUCT)
	      {
		arg->v_pointer = *(gpointer *) args[i];
		g_base_info_unref (interface);
		break;
	      }

	    g_base_info_unref (interface);
	  }
	case GI_TYPE_TAG_GLIST:
	case GI_TYPE_TAG_GSLIST:
	  arg->v_pointer = *(gpointer *) args[i];
	  break;
	default:
	  arg->v_pointer = 0;
	}
      jsargs[i] = seed_value_from_gi_argument (ctx, arg, arg_type, 0);
      seed_gi_release_arg (g_arg_info_get_ownership_transfer (arg_info),
			      arg_type, arg);
      g_base_info_unref ((GIBaseInfo *) arg_info);
      g_base_info_unref ((GIBaseInfo *) arg_type);
    }

  return_value = (JSValueRef)
    JSObjectCallAsFunction (ctx,
			    (JSObjectRef) privates->function, 0,
			    num_args, jsargs, &exception);

  if (exception)
    {
      mes = seed_exception_to_string (ctx, exception);
      g_warning ("Exception in closure marshal. %s \n", mes);
      g_free (mes);
      exception = 0;
    }

  seed_value_to_gi_argument (ctx, (JSValueRef) return_value, return_type,
			 &return_arg, 0);
  switch (return_tag)
    {
#if GOBJECT_INTROSPECTION_VERSION < 0x000900
    case GI_TYPE_TAG_LONG:
      *(glong *) result = return_arg.v_long;
      break;
    case GI_TYPE_TAG_ULONG:
      *(gulong *) result = return_arg.v_ulong;
      break;
    case GI_TYPE_TAG_INT:
      *(gint *) result = return_arg.v_int;
      break;
    case GI_TYPE_TAG_SSIZE:
      *(gssize *) result = return_arg.v_ssize;
      break;
    case GI_TYPE_TAG_SIZE:
      *(gsize *) result = return_arg.v_size;
      break;
    case GI_TYPE_TAG_UINT:
      *(guint *) result = return_arg.v_uint;
      break;
#endif
    case GI_TYPE_TAG_BOOLEAN:
      *(gboolean *) result = return_arg.v_boolean;
      break;
    case GI_TYPE_TAG_INT8:
      *(gint8 *) result = return_arg.v_int8;
      break;
    case GI_TYPE_TAG_UINT8:
      *(guint8 *) result = return_arg.v_uint8;
      break;
    case GI_TYPE_TAG_INT16:
      *(gint16 *) result = return_arg.v_int16;
      break;
    case GI_TYPE_TAG_UINT16:
      *(guint16 *) result = return_arg.v_uint16;
      break;
    case GI_TYPE_TAG_INT32:
      *(gint32 *) result = return_arg.v_int32;
      break;
    case GI_TYPE_TAG_UINT32:
      *(guint32 *) result = return_arg.v_uint32;
      break;
    case GI_TYPE_TAG_INT64:
      *(gint64 *) result = return_arg.v_int64;
      break;
    case GI_TYPE_TAG_UINT64:
      *(guint64 *) result = return_arg.v_uint64;
      break;
    case GI_TYPE_TAG_FLOAT:
      *(gfloat *) result = return_arg.v_float;
      break;
    case GI_TYPE_TAG_DOUBLE:
      *(gdouble *) result = return_arg.v_double;
      break;
    case GI_TYPE_TAG_UTF8:
      *(gchar **) result = return_arg.v_string;
      break;
    case GI_TYPE_TAG_INTERFACE:
      {
	GIBaseInfo *interface;
	GIInfoType interface_type;

	interface = g_type_info_get_interface (return_type);
	interface_type = g_base_info_get_type (interface);

	if (interface_type == GI_INFO_TYPE_OBJECT ||
	    interface_type == GI_INFO_TYPE_INTERFACE)
	  {
	    *(gpointer *) result = return_arg.v_pointer;
	    break;
	  }

	else if (interface_type == GI_INFO_TYPE_ENUM ||
		 interface_type == GI_INFO_TYPE_FLAGS)
	  {
	    *(double *) result = return_arg.v_double;
	    break;
	  }
	else if (interface_type == GI_INFO_TYPE_STRUCT)
	  {
	    *(gpointer *) result = return_arg.v_pointer;
	    break;
	  }
      }
    case GI_TYPE_TAG_GLIST:
    case GI_TYPE_TAG_GSLIST:
      *(gpointer *) result = return_arg.v_pointer;
      break;
    default:
      *(gpointer *) result = 0;
    }
  g_base_info_unref ((GIBaseInfo *) return_type);
  
  JSGarbageCollect(ctx);

  JSGlobalContextRelease ((JSGlobalContextRef) ctx);
}

SeedNativeClosure *
seed_make_native_closure (JSContextRef ctx,
			  GICallableInfo * info,
			  JSValueRef function)
{
  ffi_cif *cif;
  ffi_closure *closure;
  GITypeInfo *return_type;
  SeedNativeClosure *privates;
  JSObjectRef cached;

  cached =
    (JSObjectRef) seed_object_get_property (ctx, (JSObjectRef) function,
					    "__seed_native_closure");
  if (cached
      && JSValueIsObjectOfClass (ctx, cached, seed_native_callback_class))
    {
      return (SeedNativeClosure *) JSObjectGetPrivate (cached);
    }

  return_type = g_callable_info_get_return_type (info);
  cif = g_new0 (ffi_cif, 1);

  privates = g_new0 (SeedNativeClosure, 1);
  privates->ctx = ctx;
  privates->info = (GICallableInfo *) g_base_info_ref ((GIBaseInfo *) info);
  privates->function = function;
  privates->cif = cif;

  closure =
    g_callable_info_prepare_closure (info, cif, seed_handle_closure,
				     privates);
  privates->closure = closure;

  JSValueProtect (ctx, function);

  seed_object_set_property (ctx, (JSObjectRef) function,
			    "__seed_native_closure",
			    (JSValueRef) JSObjectMake (ctx,
						       seed_native_callback_class,
						       privates));

  g_base_info_unref ((GIBaseInfo *) return_type);

  return privates;
}

static void
closure_invalidated (gpointer data, GClosure * c)
{
  SeedClosure *closure = (SeedClosure *) c;

  SEED_NOTE (FINALIZATION, "Finalizing closure.");
  if (closure->user_data
      && !JSValueIsUndefined (eng->context, closure->user_data))
    JSValueUnprotect (eng->context, closure->user_data);
  if (!JSValueIsUndefined (eng->context, closure->function))
    JSValueUnprotect (eng->context, closure->function);

  g_free (closure->description);

}

JSObjectRef
seed_closure_get_callable (GClosure * c)
{
  return ((SeedClosure *) c)->function;
}

JSValueRef
seed_closure_invoke (GClosure * closure, JSValueRef * args, guint argc,
		     JSValueRef * exception)
{
  JSContextRef ctx = JSGlobalContextCreateInGroup (context_group, 0);
  JSValueRef ret;

  seed_prepare_global_context (ctx);

  ret =
    seed_closure_invoke_with_context (ctx, closure, args, argc, exception);

  JSGlobalContextRelease ((JSGlobalContextRef) ctx);

  return ret;
}

JSValueRef
seed_closure_invoke_with_context (JSContextRef ctx, GClosure * closure,
				  JSValueRef * args, guint argc,
				  JSValueRef * exception)
{
  JSValueRef *real_args = g_newa (JSValueRef, argc + 1);
  guint i;

  for (i = 0; i < argc; i++)
    real_args[i] = args[i];
  real_args[argc] =
    ((SeedClosure *) closure)->user_data ? ((SeedClosure *) closure)->
    user_data : JSValueMakeNull (ctx);

  return
    JSObjectCallAsFunction (ctx, ((SeedClosure *) closure)->function, NULL,
			    argc + 1, real_args, exception);
}

GClosure *
seed_closure_new_for_signal (JSContextRef ctx, JSObjectRef function,
			     JSObjectRef user_data, const gchar * description,
			     guint signal_id)
{
  GClosure *closure;

  closure = g_closure_new_simple (sizeof (SeedClosure), 0);
  g_closure_add_finalize_notifier (closure, 0, closure_invalidated);
  g_closure_set_meta_marshal (closure, GUINT_TO_POINTER (signal_id),
                              seed_signal_marshal_func);

  JSValueProtect (ctx, function);
  ((SeedClosure *) closure)->function = function;
  if (user_data && !JSValueIsNull (ctx, user_data))
    {
      ((SeedClosure *) closure)->user_data = user_data;
      JSValueProtect (ctx, user_data);
    }

  if (description)
    ((SeedClosure *) closure)->description = g_strdup (description);

  return closure;
}

GClosure *
seed_closure_new (JSContextRef ctx, JSObjectRef function,
		  JSObjectRef user_data, const gchar * description)
{
  return seed_closure_new_for_signal (ctx, function, user_data, description,
                                      0);
}

void
seed_closure_warn_exception (GClosure * c,
			     JSContextRef ctx, JSValueRef exception)
{
  JSObjectRef callable = seed_closure_get_callable (c);
  gchar *name = seed_value_to_string (ctx,
				      seed_object_get_property (ctx, callable,
								"name"),
				      NULL);
  gchar *mes = seed_exception_to_string (ctx, exception);

  g_warning ("Exception in closure (%p) for %s (handler %s). %s", c,
	     ((SeedClosure *) c)->description,
	     *name == '\0' ? "[anonymous]" : name, mes);

  g_free (name);
  g_free (mes);
}

JSClassDefinition seed_native_callback_def = {
  0,				/* Version, always 0 */
  0,
  "seed_native_callback",	/* Class Name */
  0,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,
  seed_closure_finalize,	/* Finalize */
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

void
seed_closures_init (void)
{
  seed_native_callback_class = JSClassCreate (&seed_native_callback_def);
  JSClassRetain (seed_native_callback_class);
}
