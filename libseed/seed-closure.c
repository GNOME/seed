/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed-closure.c.h
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 *
 * libseed is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libseed is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "seed-private.h"
#include <sys/mman.h>

static ffi_type *get_ffi_type(GITypeInfo * info)
{
	ffi_type *rettype;

	if (g_type_info_is_pointer(info))
		rettype = &ffi_type_pointer;
	else
		switch (g_type_info_get_tag(info))
		{
		case GI_TYPE_TAG_VOID:
			rettype = &ffi_type_void;
			break;
		case GI_TYPE_TAG_BOOLEAN:
			rettype = &ffi_type_uint;
			break;
		case GI_TYPE_TAG_INT8:
			rettype = &ffi_type_sint8;
			break;
		case GI_TYPE_TAG_UINT8:
			rettype = &ffi_type_uint8;
			break;
		case GI_TYPE_TAG_INT16:
			rettype = &ffi_type_sint16;
			break;
		case GI_TYPE_TAG_UINT16:
			rettype = &ffi_type_uint16;
			break;
		case GI_TYPE_TAG_INT32:
			rettype = &ffi_type_sint32;
			break;
		case GI_TYPE_TAG_UINT32:
			rettype = &ffi_type_uint32;
			break;
		case GI_TYPE_TAG_INT64:
			rettype = &ffi_type_sint64;
			break;
		case GI_TYPE_TAG_UINT64:
			rettype = &ffi_type_uint64;
			break;
		case GI_TYPE_TAG_INT:
			rettype = &ffi_type_sint;
			break;
		case GI_TYPE_TAG_UINT:
			rettype = &ffi_type_uint;
			break;
		case GI_TYPE_TAG_SSIZE:	/* FIXME */
		case GI_TYPE_TAG_LONG:
			rettype = &ffi_type_slong;
			break;
		case GI_TYPE_TAG_SIZE:	/* FIXME */
		case GI_TYPE_TAG_TIME_T:	/* May not be portable */
		case GI_TYPE_TAG_ULONG:
			rettype = &ffi_type_ulong;
			break;
		case GI_TYPE_TAG_FLOAT:
			rettype = &ffi_type_float;
			break;
		case GI_TYPE_TAG_DOUBLE:
			rettype = &ffi_type_double;
			break;
		case GI_TYPE_TAG_UTF8:
		case GI_TYPE_TAG_FILENAME:
		case GI_TYPE_TAG_ARRAY:
		case GI_TYPE_TAG_INTERFACE:
		case GI_TYPE_TAG_GLIST:
		case GI_TYPE_TAG_GSLIST:
		case GI_TYPE_TAG_GHASH:
		case GI_TYPE_TAG_ERROR:
			rettype = &ffi_type_pointer;
			break;
		default:
			g_assert_not_reached();
		}

	return rettype;
}

static void seed_closure_finalize(JSObjectRef object)
{
	SeedNativeClosure *privates =
		(SeedNativeClosure *) JSObjectGetPrivate(object);

	g_free(privates->cif->arg_types);
	g_free(privates->cif);
	munmap(privates->closure, sizeof(ffi_closure));
}

static void
seed_handle_closure(ffi_cif * cif, void *result, void **args, void *userdata)
{
	SeedNativeClosure *privates = userdata;
	gint num_args, i;
	JSValueRef *jsargs;
	JSValueRef return_value;
	GITypeTag return_tag;
	GIArgInfo *arg_info;
	GITypeInfo *return_type;
	GArgument rarg;
	GArgument *return_arg = g_new0(GArgument, 1);

	SEED_NOTE(INVOCATION, "Invoking closure of type: %s \n",
			  g_base_info_get_name((GIBaseInfo *) privates->info));

	num_args = g_callable_info_get_n_args(privates->info);
	return_type = g_callable_info_get_return_type(privates->info);
	return_tag = g_type_info_get_tag(return_type);
	jsargs = (JSValueRef *) g_new0(JSValueRef, num_args);

	for (i = 0; i < num_args; i++)
	{
		GITypeInfo *arg_type;
		GITypeTag tag;
		GType type;
		GArgument *arg = &rarg;

		arg_info = g_callable_info_get_arg(privates->info, i);
		arg_type = g_arg_info_get_type(arg_info);
		tag = g_type_info_get_tag(arg_type);

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

				interface = g_type_info_get_interface(arg_type);
				interface_type = g_base_info_get_type(interface);

				if (interface_type == GI_INFO_TYPE_OBJECT ||
					interface_type == GI_INFO_TYPE_INTERFACE)
				{
					arg->v_pointer = *(gpointer *) args[i];
					break;
				}

				else if (interface_type == GI_INFO_TYPE_ENUM ||
						 interface_type == GI_INFO_TYPE_FLAGS)
				{
					arg->v_double = *(double *)args[i];
					break;
				}
				else if (interface_type == GI_INFO_TYPE_STRUCT)
				{
					arg->v_pointer = *(gpointer *) args[i];
					break;
				}

				g_base_info_unref(interface);
			}
		case GI_TYPE_TAG_GLIST:
		case GI_TYPE_TAG_GSLIST:
			arg->v_pointer = *(gpointer *) args[i];
			break;
		default:
			arg->v_pointer = 0;
		}
		jsargs[i] = seed_gi_argument_make_js(arg, arg_type, 0);
		seed_gi_release_in_arg(g_arg_info_get_ownership_transfer(arg_info),
							   arg_type, arg);
		g_base_info_unref((GIBaseInfo *) arg_info);
	}

	return_value = (JSValueRef)
		JSObjectCallAsFunction(privates->ctx,
							   (JSObjectRef) privates->function, 0,
							   num_args, jsargs, 0);

	g_free(jsargs);

	seed_gi_make_argument((JSValueRef) return_value, return_type,
						  return_arg, 0);
	switch (return_tag)
	{

	case GI_TYPE_TAG_BOOLEAN:
		*(gboolean *) result = return_arg->v_boolean;
		break;
	case GI_TYPE_TAG_INT8:
		*(gint8 *) result = return_arg->v_int8;
		break;
	case GI_TYPE_TAG_UINT8:
		*(guint8 *) result = return_arg->v_uint8;
		break;
	case GI_TYPE_TAG_INT16:
		*(gint16 *) result = return_arg->v_int16;
		break;
	case GI_TYPE_TAG_UINT16:
		return_arg->v_uint16 = *(guint16 *) args[i];
		break;
	case GI_TYPE_TAG_INT32:
		*(gint32 *) result = return_arg->v_int32;
		break;
	case GI_TYPE_TAG_UINT32:
		*(guint32 *) result = return_arg->v_uint32;
		break;
	case GI_TYPE_TAG_LONG:
	case GI_TYPE_TAG_INT64:
		*(glong *) result = return_arg->v_int64;
		break;
	case GI_TYPE_TAG_ULONG:
	case GI_TYPE_TAG_UINT64:
		*(glong *) result = return_arg->v_uint64;
		break;
	case GI_TYPE_TAG_INT:
	case GI_TYPE_TAG_SSIZE:
	case GI_TYPE_TAG_SIZE:
		*(gint *) result = return_arg->v_int32;
		break;
	case GI_TYPE_TAG_UINT:
		*(guint *) result = return_arg->v_uint32;
		break;
	case GI_TYPE_TAG_FLOAT:
		*(gfloat *) result = return_arg->v_float;
		break;
	case GI_TYPE_TAG_DOUBLE:
		*(gdouble *) result = return_arg->v_double;
		break;
	case GI_TYPE_TAG_UTF8:
		*(gchar **) result = return_arg->v_string;
		break;
	case GI_TYPE_TAG_INTERFACE:
		{
			GIBaseInfo *interface;
			GIInfoType interface_type;

			interface = g_type_info_get_interface(return_type);
			interface_type = g_base_info_get_type(interface);

			if (interface_type == GI_INFO_TYPE_OBJECT ||
				interface_type == GI_INFO_TYPE_INTERFACE)
			{
				*(gpointer *) result = return_arg->v_pointer;
				break;
			}

			else if (interface_type == GI_INFO_TYPE_ENUM ||
					 interface_type == GI_INFO_TYPE_FLAGS)
			{
				*(double *)result = return_arg->v_double;
				break;
			}
			else if (interface_type == GI_INFO_TYPE_STRUCT)
			{
				*(gpointer *) result = return_arg->v_pointer;
				break;
			}
		}
	case GI_TYPE_TAG_GLIST:
	case GI_TYPE_TAG_GSLIST:
		*(gpointer *) result = return_arg->v_pointer;
		break;
	default:
		*(gpointer *) result = 0;
	}

	g_free(return_arg);
}

SeedNativeClosure *seed_make_native_closure(JSContextRef ctx,
											GICallableInfo * info,
											JSValueRef function)
{
	ffi_cif *cif;
	ffi_closure *closure;
	ffi_type **arg_types;
	ffi_arg result;
	ffi_status status;
	GITypeInfo *return_type;
	GIArgInfo *arg_info;
	gint num_args, i;
	SeedNativeClosure *privates;
	JSObjectRef cached;

	cached =
		(JSObjectRef) seed_object_get_property((JSObjectRef) function,
											   "__seed_native_closure");
	if (cached
		&& JSValueIsObjectOfClass(ctx, cached,
								  seed_native_callback_class))
	{
		return (SeedNativeClosure *) JSObjectGetPrivate(cached);
	}

	num_args = g_callable_info_get_n_args(info);
	return_type = g_callable_info_get_return_type(info);
	arg_types = (ffi_type **) g_new0(ffi_type *, num_args + 1);
	cif = g_new0(ffi_cif, 1);

	privates = g_new0(SeedNativeClosure, 1);
	privates->info = info;
	privates->function = function;
	privates->cif = cif;
	privates->ctx = ctx;

	for (i = 0; i < num_args; i++)
	{
		arg_info = g_callable_info_get_arg(info, i);
		arg_types[i] = get_ffi_type(g_arg_info_get_type(arg_info));
	}
	arg_types[num_args] = 0;

	closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE |
				   PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
	privates->closure = closure;

	ffi_prep_cif(cif, FFI_DEFAULT_ABI, num_args,
				 get_ffi_type(return_type), arg_types);
	ffi_prep_closure(closure, cif, seed_handle_closure, privates);

	seed_object_set_property((JSObjectRef) function,
							 "__seed_native_closure",
							 (JSValueRef) JSObjectMake(ctx,
													   seed_native_callback_class,
													   privates));

	return privates;
}

SeedClosure *seed_make_gclosure(JSContextRef ctx,
								JSObjectRef function, 
								JSObjectRef this)
{
	GClosure *closure;

	closure = g_closure_new_simple(sizeof(SeedClosure), 0);
	g_closure_set_marshal(closure, seed_signal_marshal_func);

	((SeedClosure *) closure)->function = function;
	((SeedClosure *) closure)->object = 0;
	if (this && !JSValueIsNull(ctx, this))
	{
		JSValueProtect(ctx, this);
		((SeedClosure *) closure)->this = this;
	}
	else
		((SeedClosure *) closure)->this = 0;

	JSValueProtect(ctx, function);

	return (SeedClosure *) closure;
}

JSClassDefinition seed_native_callback_def = {
	0,							/* Version, always 0 */
	0,
	"seed_native_callback",		/* Class Name */
	0,							/* Parent Class */
	NULL,						/* Static Values */
	NULL,						/* Static Functions */
	NULL,
	seed_closure_finalize,		/* Finalize */
	NULL,						/* Has Property */
	NULL,						/* Get Property */
	NULL,						/* Set Property */
	NULL,						/* Delete Property */
	NULL,						/* Get Property Names */
	NULL,						/* Call As Function */
	NULL,						/* Call As Constructor */
	NULL,						/* Has Instance */
	NULL						/* Convert To Type */
};

void seed_closures_init(void)
{
	seed_native_callback_class = JSClassCreate(&seed_native_callback_def);
	JSClassRetain(seed_native_callback_class);
}
