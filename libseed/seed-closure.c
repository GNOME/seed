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
 
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 */

#include "seed-private.h"
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
seed_handle_closure (ffi_cif * cif, void *result, void **args, void *userdata)
{
  SeedNativeClosure *privates = userdata;
  gint num_args, i;
  JSValueRef *jsargs;
  JSValueRef return_value;
  JSValueRef exception = 0;
  GITypeTag return_tag;
  GIArgInfo *arg_info;
  GITypeInfo *return_type;
  GArgument rarg;
  GArgument return_arg;
  JSContextRef ctx = JSGlobalContextCreateInGroup (context_group, 0);
  
  seed_prepare_global_context (ctx);

  SEED_NOTE (INVOCATION, "Invoking closure of type: %s \n",
	     g_base_info_get_name ((GIBaseInfo *) privates->info));

  num_args = g_callable_info_get_n_args (privates->info);
  return_type = g_callable_info_get_return_type (privates->info);
  return_tag = g_type_info_get_tag (return_type);
  jsargs = (JSValueRef *) g_newa (JSValueRef, num_args);

  for (i = 0; i < num_args; i++)
    {
      GITypeInfo *arg_type;
      GITypeTag tag;
      GArgument *arg = &rarg;

      arg_info = g_callable_info_get_arg (privates->info, i);
      arg_type = g_arg_info_get_type (arg_info);
      tag = g_type_info_get_tag (arg_type);

      switch (tag)
	{
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
	case GI_TYPE_TAG_LONG:
	case GI_TYPE_TAG_INT64:
	  arg->v_int64 = *(glong *) args[i];
	  break;
	case GI_TYPE_TAG_ULONG:
	case GI_TYPE_TAG_UINT64:
	  arg->v_uint64 = *(glong *) args[i];
	  break;
	case GI_TYPE_TAG_INT:
	case GI_TYPE_TAG_SSIZE:
	case GI_TYPE_TAG_SIZE:
	  arg->v_int32 = *(gint *) args[i];
	  break;
	case GI_TYPE_TAG_UINT:
	  arg->v_uint32 = *(guint *) args[i];
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
      jsargs[i] = seed_gi_argument_make_js (ctx, arg, arg_type, 0);
      seed_gi_release_in_arg (g_arg_info_get_ownership_transfer (arg_info),
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
      gchar *mes = seed_exception_to_string (ctx,
					     exception);
      g_warning ("Exception in closure marshal. %s \n", mes);
      g_free (mes);
      exception = 0;
    }

  seed_gi_make_argument (ctx, (JSValueRef) return_value, return_type,
			 &return_arg, 0);
  switch (return_tag)
    {

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
      return_arg.v_uint16 = *(guint16 *) args[i];
      break;
    case GI_TYPE_TAG_INT32:
      *(gint32 *) result = return_arg.v_int32;
      break;
    case GI_TYPE_TAG_UINT32:
      *(guint32 *) result = return_arg.v_uint32;
      break;
    case GI_TYPE_TAG_LONG:
    case GI_TYPE_TAG_INT64:
      *(glong *) result = return_arg.v_int64;
      break;
    case GI_TYPE_TAG_ULONG:
    case GI_TYPE_TAG_UINT64:
      *(glong *) result = return_arg.v_uint64;
      break;
    case GI_TYPE_TAG_INT:
    case GI_TYPE_TAG_SSIZE:
    case GI_TYPE_TAG_SIZE:
      *(gint *) result = return_arg.v_int32;
      break;
    case GI_TYPE_TAG_UINT:
      *(guint *) result = return_arg.v_uint32;
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
  JSGlobalContextRelease ((JSGlobalContextRef) ctx);
}

SeedNativeClosure *
seed_make_native_closure (JSContextRef ctx,
			  GICallableInfo * info, JSValueRef function)
{
  ffi_cif *cif;
  ffi_closure *closure;
  ffi_type **arg_types;
  GITypeInfo *return_type;
  gint num_args;
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

  num_args = g_callable_info_get_n_args (info);
  return_type = g_callable_info_get_return_type (info);
  arg_types = (ffi_type **) g_new0 (ffi_type *, num_args + 1);
  cif = g_new0 (ffi_cif, 1);

  privates = g_new0 (SeedNativeClosure, 1);
  privates->info = (GICallableInfo *) g_base_info_ref ((GIBaseInfo *) info);
  privates->function = function;
  privates->cif = cif;

  closure = g_callable_info_prepare_closure (info, cif, seed_handle_closure, privates);
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
  JSContextRef ctx = JSGlobalContextCreateInGroup (context_group,
						   0);
  SeedClosure *closure = (SeedClosure *) c;
  
  SEED_NOTE (FINALIZATION, "Finalizing closure.");
  if (closure->user_data && !JSValueIsUndefined(ctx, closure->user_data))
    JSValueUnprotect(ctx, closure->user_data);
  if (!JSValueIsUndefined (ctx, closure->function))
    JSValueUnprotect (ctx, closure->function);

  JSGlobalContextRelease ((JSGlobalContextRef) ctx);
}

JSObjectRef
seed_closure_get_callable (GClosure *c)
{
  return ((SeedClosure *)c)->function;
}

JSValueRef
seed_closure_invoke (GClosure *closure, JSValueRef *args, guint argc, JSValueRef *exception)
{
  JSContextRef ctx = JSGlobalContextCreateInGroup (context_group, 0);
  JSValueRef *real_args = g_newa (JSValueRef, argc +1);
  JSValueRef ret;
  guint i;
  
  seed_prepare_global_context (ctx);
  for (i = 0; i < argc; i++)
    real_args[i] = args[i];
  args[argc] = ((SeedClosure *)closure)->user_data ? ((SeedClosure *)closure)->user_data : JSValueMakeNull (ctx);
  
  ret = JSObjectCallAsFunction (ctx, ((SeedClosure *)closure)->function, NULL, argc+1, real_args, exception);
  JSGlobalContextRelease ((JSGlobalContextRef) ctx);
  
  return ret;
}

JSValueRef
seed_closure_invoke_with_context (JSContextRef ctx, GClosure *closure, JSValueRef *args, guint argc, JSValueRef *exception)
{
  JSValueRef *real_args = g_newa (JSValueRef, argc +1);
  JSValueRef ret;
  guint i;
  
  for (i = 0; i < argc; i++)
    real_args[i] = args[i];
  args[argc] = ((SeedClosure *)closure)->user_data ? ((SeedClosure *)closure)->user_data : JSValueMakeNull (ctx);
  
  ret = JSObjectCallAsFunction (ctx, ((SeedClosure *)closure)->function, NULL, argc+1, real_args, exception);
  
  return ret;
}

GClosure *
seed_make_gclosure (JSContextRef ctx, JSObjectRef function, JSObjectRef user_data)
{
  GClosure *closure;

  closure = g_closure_new_simple (sizeof (SeedClosure), 0);
  g_closure_add_finalize_notifier (closure, 0, closure_invalidated);
  g_closure_set_marshal (closure, seed_signal_marshal_func);

  ((SeedClosure *) closure)->function = function;
  ((SeedClosure *) closure)->object = 0;
  if (user_data && !JSValueIsNull (ctx, user_data))
    {
      ((SeedClosure *) closure)->user_data = user_data;
      JSValueProtect(ctx, user_data);
    }
  else
    ((SeedClosure *) closure)->user_data = NULL;
  
  JSValueProtect (eng->context, function);

  return closure;
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
