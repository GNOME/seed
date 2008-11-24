/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed-types.c
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
#include <string.h>
#include <dlfcn.h>

JSClassRef gobject_class;
JSClassRef gobject_method_class;
JSClassRef gobject_constructor_class;
JSClassRef seed_callback_class;
SeedEngine *eng;

static gboolean seed_value_is_gobject(JSValueRef value)
{
	if (!JSValueIsObject(eng->context, value) ||
		JSValueIsNull(eng->context, value))
		return FALSE;

	return JSValueIsObjectOfClass(eng->context, value, gobject_class);
}

void seed_toggle_ref(gpointer data, GObject * object, gboolean is_last_ref)
{
	JSValueRef wrapper;

	wrapper = (JSValueRef) data;

	if (is_last_ref)
	{
		JSValueUnprotect(eng->context, wrapper);
	}
	else
	{
		JSValueProtect(eng->context, wrapper);
	}
}

static void seed_gobject_destroyed(gpointer object)
{
	JSValueUnprotect(eng->context, (JSValueRef) object);
	JSObjectSetPrivate((JSObjectRef) object, 0);
}

static JSValueRef seed_wrap_object(GObject * object)
{
	JSValueRef user_data;
	JSValueRef js_ref;
	JSClassRef class;
	GType type, *interfaces;
	JSValueRef prototype;
	guint i, n;

	type = G_OBJECT_TYPE(object);

	user_data = (JSValueRef) g_object_get_data(object, "js-ref");

	if (user_data)
		return user_data;

	class = seed_gobject_get_class_for_gtype(type);

	while (!class && (type = g_type_parent(type)))
	{
		class = seed_gobject_get_class_for_gtype(type);
	}

	prototype = seed_gobject_get_prototype_for_gtype(type);
	js_ref = JSObjectMake(eng->context, class, object);
	if (prototype)
		JSObjectSetPrototype(eng->context, (JSObjectRef) js_ref, prototype);
	else
	{
		g_assert_not_reached();
	}

	g_object_set_data_full(object, "js-ref", (gpointer) js_ref,
						   seed_gobject_destroyed);

	JSValueProtect(eng->context, js_ref);
	g_object_add_toggle_ref(object, seed_toggle_ref, (gpointer) js_ref);

	return js_ref;
}

static gboolean seed_release_arg(GITransfer transfer,
								 GITypeInfo * type_info,
								 GITypeTag type_tag, GArgument * arg)
{
	GType gtype;

	switch (type_tag)
	{
	case GI_TYPE_TAG_UTF8:
		g_free(arg->v_string);
		break;
	case GI_TYPE_TAG_INTERFACE:
		{
			if (arg->v_pointer)
			{
				GIBaseInfo *interface_info;
				GType interface_type;

				interface_info = g_type_info_get_interface(type_info);

				gtype =
					g_registered_type_info_get_g_type((GIRegisteredTypeInfo *)
													  interface_info);

				if (g_type_is_a(gtype, G_TYPE_OBJECT)
					|| g_type_is_a(gtype, G_TYPE_INTERFACE))
				{
					SEED_NOTE(MISC,
							  "Unreffing object of type: %s in"
							  "argument release. Reference count: %d\n",
							  g_type_name(G_OBJECT_TYPE
										  (G_OBJECT(arg->v_pointer))),
							  G_OBJECT(arg->v_pointer)->ref_count);
					g_object_unref(G_OBJECT(arg->v_pointer));
				}

				g_base_info_unref(interface_info);
			}
			break;
		}
	default:
		break;

	}

	return TRUE;
}

gboolean seed_gi_release_arg(GITransfer transfer,
							 GITypeInfo * type_info, GArgument * arg)
{
	GITypeTag type_tag;

	if (transfer == GI_TRANSFER_NOTHING)
		return TRUE;

	type_tag = g_type_info_get_tag((GITypeInfo *) type_info);

	return seed_release_arg(transfer, type_info, type_tag, arg);
}

gboolean seed_gi_release_in_arg(GITransfer transfer,
								GITypeInfo * type_info, GArgument * arg)
{
	GITypeTag type_tag;

	if (transfer == GI_TRANSFER_EVERYTHING)
		return;

	type_tag = g_type_info_get_tag((GITypeInfo *) type_info);

	switch (type_tag)
	{
	case GI_TYPE_TAG_UTF8:
		return seed_release_arg(GI_TRANSFER_EVERYTHING,
								type_info, type_tag, arg);
	}

	return TRUE;
}

gboolean
seed_gi_make_argument(JSValueRef value,
					  GITypeInfo * type_info, GArgument * arg,
					  JSValueRef * exception)
{
	GITypeTag gi_tag = g_type_info_get_tag(type_info);

	// FIXME: Some types are not "nullable", also need to check if argument
	// can be null before doing this.
	if (!value || JSValueIsNull(eng->context, value))
	{
		arg->v_pointer = 0;
		return 1;
	}

	switch (gi_tag)
	{
	case GI_TYPE_TAG_VOID:
		break;
	case GI_TYPE_TAG_BOOLEAN:
		arg->v_boolean = seed_value_to_boolean(value, exception);
		break;
	case GI_TYPE_TAG_INT8:
		arg->v_int8 = seed_value_to_char(value, exception);
		break;
	case GI_TYPE_TAG_UINT8:
		arg->v_uint8 = seed_value_to_uchar(value, exception);
		break;
	case GI_TYPE_TAG_INT16:
		arg->v_int16 = seed_value_to_int(value, exception);
		break;
	case GI_TYPE_TAG_UINT16:
		arg->v_uint16 = seed_value_to_uint(value, exception);
		break;
	case GI_TYPE_TAG_INT32:
		arg->v_int32 = seed_value_to_int(value, exception);
		break;
	case GI_TYPE_TAG_UINT32:
		arg->v_uint32 = seed_value_to_uint(value, exception);
		break;
	case GI_TYPE_TAG_LONG:
	case GI_TYPE_TAG_INT64:
		arg->v_int64 = seed_value_to_long(value, exception);
		break;
	case GI_TYPE_TAG_ULONG:
	case GI_TYPE_TAG_UINT64:
		arg->v_uint64 = seed_value_to_ulong(value, exception);
		break;
	case GI_TYPE_TAG_INT:
		arg->v_int = seed_value_to_int(value, exception);
		break;
	case GI_TYPE_TAG_UINT:
		arg->v_uint = seed_value_to_uint(value, exception);
		break;
	case GI_TYPE_TAG_SIZE:
	case GI_TYPE_TAG_SSIZE:
		arg->v_int = seed_value_to_int(value, exception);
		break;
	case GI_TYPE_TAG_FLOAT:
		arg->v_float = seed_value_to_float(value, exception);
		break;
	case GI_TYPE_TAG_DOUBLE:
		arg->v_double = seed_value_to_double(value, exception);
		break;
	case GI_TYPE_TAG_UTF8:
		arg->v_string = seed_value_to_string(value, exception);
		break;
	case GI_TYPE_TAG_INTERFACE:
		{
			GIBaseInfo *interface;
			GIInfoType interface_type;
			GType required_gtype;
			GObject *gobject;

			interface = g_type_info_get_interface(type_info);
			interface_type = g_base_info_get_type(interface);

			arg->v_pointer = NULL;

			if (interface_type == GI_INFO_TYPE_OBJECT
				|| interface_type == GI_INFO_TYPE_INTERFACE)
			{
				gobject = seed_value_to_object(value, exception);
				required_gtype =
					g_registered_type_info_get_g_type((GIRegisteredTypeInfo *)
													  interface);

				// FIXME: Not clear if the g_type_is_a check is desired here.
				// Possibly 'breaks things' when we don't have introspection
				// data for some things in an interface hierarchy. Hasn't
				// provided any problems so far.
				if (!gobject
					|| !g_type_is_a(G_OBJECT_TYPE(gobject), required_gtype))
				{
					g_base_info_unref(interface);
					return FALSE;
				}

				arg->v_pointer = gobject;
				g_base_info_unref(interface);
				break;
			}
			else if (interface_type == GI_INFO_TYPE_ENUM ||
					 interface_type == GI_INFO_TYPE_FLAGS)
			{
				arg->v_long = seed_value_to_long(value, exception);
				g_base_info_unref(interface);
				break;
			}
			else if (interface_type == GI_INFO_TYPE_STRUCT)
			{
				if (JSValueIsObjectOfClass(eng->context,
										   value, seed_struct_class))
					arg->v_pointer = seed_pointer_get_pointer(value);
				else
				{
					GType type =
						g_registered_type_info_get_g_type((GIRegisteredTypeInfo
														   *) interface);
					if (!type)
					{
						g_base_info_unref(interface);
						return FALSE;
					}
					// Automatically convert between functions and 
					// GClosures where expected.
					else if (g_type_is_a(type, G_TYPE_CLOSURE))
					{
						if (JSObjectIsFunction
							(eng->context, (JSObjectRef) value))
						{
							arg->v_pointer =
								seed_make_gclosure((JSObjectRef) value, 0);
						}
					}
					else
					{
						JSObjectRef strukt = 
							seed_construct_struct_type_with_parameters(
								interface, (JSObjectRef)value, exception);
						arg->v_pointer = seed_pointer_get_pointer(strukt);
					}
				}
				g_base_info_unref(interface);
				break;
			}
			else if (interface_type == GI_INFO_TYPE_CALLBACK)
			{
				if (JSValueIsNull(eng->context, value))
				{
					arg->v_pointer = NULL;
					g_base_info_unref(interface);
					break;
				}
				// Someone passes in a wrapper around a method where a 
				// callback is expected, i.e Clutter.sine_inc_func, as an alpha
				// Have to dlsym the symbol to be able to do this.
				// NOTE: Some cases where dlsym(NULL, symbol) doesn't work depending
				// On how libseed is loaded.
				else if (JSValueIsObjectOfClass(eng->context,
												value, gobject_method_class))
				{
					GIFunctionInfo *info =
						JSObjectGetPrivate((JSObjectRef) value);
					const gchar *symbol = g_function_info_get_symbol(info);
					gchar *error;
					void *fp;

					dlerror();
					fp = (void *)dlsym(0, symbol);
					if ((error = dlerror()) != NULL)
					{
						g_critical("dlerror: %s \n", error);
					}
					else
					{
						arg->v_pointer = fp;
						g_base_info_unref(interface);
						break;
					}
				}
				// Somewhat deprecated from when it was necessary to manually
				// create closure objects...
				else if (JSValueIsObjectOfClass(eng->context,
												value,
												seed_native_callback_class))
				{
					SeedNativeClosure *privates =
						(SeedNativeClosure *)
						JSObjectGetPrivate((JSObjectRef) value);
					arg->v_pointer = privates->closure;
					g_base_info_unref(interface);
					break;
				}
				// Automagically create closure around function passed in as 
				// callback. 
				else if (JSObjectIsFunction(eng->context, (JSObjectRef) value))
				{
					SeedNativeClosure *privates =
						seed_make_native_closure((GICallableInfo *) interface,
												 value);
					arg->v_pointer = privates->closure;
					break;
				}

			}
		}

	default:
		return FALSE;

	}
	return TRUE;

}

JSValueRef
seed_gi_argument_make_js(GArgument * arg, GITypeInfo * type_info,
						 JSValueRef * exception)
{
	GITypeTag gi_tag = g_type_info_get_tag(type_info);
	switch (gi_tag)
	{
	case GI_TYPE_TAG_VOID:
		return 0;
	case GI_TYPE_TAG_BOOLEAN:
		return seed_value_from_boolean(arg->v_boolean, exception);
	case GI_TYPE_TAG_INT8:
		return seed_value_from_char(arg->v_int8, exception);
	case GI_TYPE_TAG_UINT8:
		return seed_value_from_uchar(arg->v_uint8, exception);
	case GI_TYPE_TAG_INT16:
		return seed_value_from_int(arg->v_int16, exception);
	case GI_TYPE_TAG_UINT16:
		return seed_value_from_uint(arg->v_uint16, exception);
	case GI_TYPE_TAG_INT32:
		return seed_value_from_int(arg->v_int32, exception);
	case GI_TYPE_TAG_UINT32:
		return seed_value_from_uint(arg->v_uint32, exception);
	case GI_TYPE_TAG_LONG:
	case GI_TYPE_TAG_INT64:
		return seed_value_from_long(arg->v_int64, exception);
	case GI_TYPE_TAG_ULONG:
	case GI_TYPE_TAG_UINT64:
		return seed_value_from_ulong(arg->v_uint64, exception);
	case GI_TYPE_TAG_INT:
		return seed_value_from_int(arg->v_int32, exception);
	case GI_TYPE_TAG_UINT:
		return seed_value_from_uint(arg->v_uint32, exception);
	case GI_TYPE_TAG_SSIZE:
	case GI_TYPE_TAG_SIZE:
		return seed_value_from_int(arg->v_int, exception);
	case GI_TYPE_TAG_FLOAT:
		return seed_value_from_float(arg->v_float, exception);
	case GI_TYPE_TAG_DOUBLE:
		return seed_value_from_double(arg->v_double, exception);
	case GI_TYPE_TAG_UTF8:
		return seed_value_from_string(arg->v_string, exception);
	case GI_TYPE_TAG_INTERFACE:
		{
			GIBaseInfo *interface;
			GIInfoType interface_type;

			interface = g_type_info_get_interface(type_info);
			interface_type = g_base_info_get_type(interface);

			if (interface_type == GI_INFO_TYPE_OBJECT ||
				interface_type == GI_INFO_TYPE_INTERFACE)
			{
				if (arg->v_pointer == 0)
				{
					g_base_info_unref(interface);
					return JSValueMakeNull(eng->context);
				}
				g_base_info_unref(interface);
				return seed_value_from_object(arg->v_pointer, exception);
			}
			else if (interface_type == GI_INFO_TYPE_ENUM
					 || interface_type == GI_INFO_TYPE_FLAGS)
			{
				g_base_info_unref(interface);
				return seed_value_from_double(arg->v_double, exception);
			}
			else if (interface_type == GI_INFO_TYPE_STRUCT)
			{
				JSValueRef strukt;

				strukt = seed_make_struct(arg->v_pointer, interface);
				g_base_info_unref(interface);

				return strukt;
			}
		}
	case GI_TYPE_TAG_GLIST:
		{
			GITypeInfo *list_type;
			JSObjectRef ret;
			GArgument larg;
			gint i = 0;
			GList *list = arg->v_pointer;

			ret = JSObjectMake(eng->context, NULL, NULL);
			list_type = g_type_info_get_param_type(type_info, 0);

			for (; list != NULL; list = list->next)
			{
				JSValueRef ival;

				larg.v_pointer = list->data;
				ival =
					(JSValueRef) seed_gi_argument_make_js(&larg,
														  list_type, exception);
				JSObjectSetPropertyAtIndex(eng->context, ret, i, ival, NULL);
				i++;
			}
			return ret;

		}
	case GI_TYPE_TAG_GSLIST:
		{
			GITypeInfo *list_type;
			JSObjectRef ret;
			GArgument larg;
			gint i = 0;
			GSList *list = arg->v_pointer;

			ret = JSObjectMake(eng->context, NULL, NULL);
			list_type = g_type_info_get_param_type(type_info, 0);

			for (; list != NULL; list = list->next)
			{
				JSValueRef ival;

				larg.v_pointer = list->data;
				ival =
					(JSValueRef) seed_gi_argument_make_js(&larg,
														  list_type, exception);
				JSObjectSetPropertyAtIndex(eng->context, ret, i, ival, NULL);
				i++;
			}
			return ret;
		}

	default:
		return FALSE;

	}
	return 0;
}

JSValueRef seed_value_from_gvalue(GValue * gval, JSValueRef * exception)
{
	if (!G_IS_VALUE(gval))
	{
		return false;
	}
	switch (G_VALUE_TYPE(gval))
	{
	case G_TYPE_BOOLEAN:
		return seed_value_from_boolean(g_value_get_boolean(gval), exception);
	case G_TYPE_CHAR:
		return seed_value_from_char(g_value_get_char(gval), exception);
	case G_TYPE_UCHAR:
		return seed_value_from_uchar(g_value_get_uchar(gval), exception);
	case G_TYPE_INT:
		return seed_value_from_int(g_value_get_int(gval), exception);
	case G_TYPE_UINT:
		return seed_value_from_uint(g_value_get_uint(gval), exception);
	case G_TYPE_LONG:
		return seed_value_from_long(g_value_get_long(gval), exception);
	case G_TYPE_ULONG:
		return seed_value_from_ulong(g_value_get_ulong(gval), exception);
	case G_TYPE_INT64:
		return seed_value_from_int64(g_value_get_int64(gval), exception);
	case G_TYPE_UINT64:
		return seed_value_from_uint64(g_value_get_uint64(gval), exception);
	case G_TYPE_FLOAT:
		return seed_value_from_float(g_value_get_float(gval), exception);
	case G_TYPE_DOUBLE:
		return seed_value_from_double(g_value_get_double(gval), exception);
	case G_TYPE_STRING:
		return seed_value_from_string((gchar *)
									  g_value_get_string(gval), exception);
	case G_TYPE_POINTER:
		return seed_make_pointer(g_value_get_pointer(gval));
	case G_TYPE_PARAM:
		// Might need to dup and make a boxed.
		return seed_make_pointer(g_value_get_param(gval));
	}

	if (g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_ENUM) ||
		g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_FLAGS))
		return seed_value_from_long(gval->data[0].v_long, exception);
	else if (g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_ENUM))
		return seed_value_from_long(gval->data[0].v_long, exception);
	else if (g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_OBJECT))
	{
		GObject *obj = g_value_get_object(gval);
		return seed_value_from_object(obj, exception);
	}
	else
	{
		GIBaseInfo *info;
		GIInfoType type;

		info = g_irepository_find_by_gtype(0, G_VALUE_TYPE(gval));
		if (!info)
			return;
		type = g_base_info_get_type(info);

		if (type == GI_INFO_TYPE_UNION)
		{
			return seed_make_union(g_value_peek_pointer(gval), info);
		}
		else if (type == GI_INFO_TYPE_STRUCT)
		{
			return seed_make_struct(g_value_peek_pointer(gval), info);
		}
		else if (type == GI_INFO_TYPE_BOXED)
		{
			return seed_make_boxed(g_value_dup_boxed(gval), info);
		}

	}

	return NULL;
}

gboolean
seed_gvalue_from_seed_value(JSValueRef val, GType type, GValue * ret,
							JSValueRef * exception)
{
	switch (type)
	{
	case G_TYPE_BOOLEAN:
		{
			g_value_init(ret, G_TYPE_BOOLEAN);
			g_value_set_boolean(ret, seed_value_to_boolean(val, exception));
			return TRUE;
		}
	case G_TYPE_INT:
	case G_TYPE_UINT:
		{
			g_value_init(ret, type);
			if (type == G_TYPE_INT)
				g_value_set_int(ret, seed_value_to_int(val, exception));
			else
				g_value_set_uint(ret, seed_value_to_uint(val, exception));
			return TRUE;
		}
	case G_TYPE_CHAR:
		{
			g_value_init(ret, G_TYPE_CHAR);
			g_value_set_char(ret, seed_value_to_char(val, exception));
			return TRUE;
		}
	case G_TYPE_UCHAR:
		{
			g_value_init(ret, G_TYPE_UCHAR);
			g_value_set_uchar(ret, seed_value_to_uchar(val, exception));
			return TRUE;
		}
	case G_TYPE_LONG:
	case G_TYPE_ULONG:
	case G_TYPE_INT64:
	case G_TYPE_UINT64:
	case G_TYPE_FLOAT:
	case G_TYPE_DOUBLE:
		{
			switch (type)
			{
			case G_TYPE_LONG:
				g_value_init(ret, G_TYPE_LONG);
				g_value_set_long(ret, seed_value_to_long(val, exception));
				break;
			case G_TYPE_ULONG:
				g_value_init(ret, G_TYPE_ULONG);
				g_value_set_ulong(ret, seed_value_to_ulong(val, exception));
				break;
			case G_TYPE_INT64:
				g_value_init(ret, G_TYPE_INT64);
				g_value_set_int64(ret, seed_value_to_int64(val, exception));
				break;
			case G_TYPE_UINT64:
				g_value_init(ret, G_TYPE_UINT64);
				g_value_set_uint64(ret, seed_value_to_uint64(val, exception));
				break;
			case G_TYPE_FLOAT:
				g_value_init(ret, G_TYPE_FLOAT);
				g_value_set_float(ret, seed_value_to_float(val, exception));
				break;
			case G_TYPE_DOUBLE:
				g_value_init(ret, G_TYPE_DOUBLE);
				g_value_set_double(ret, seed_value_to_double(val, exception));
				break;
			}
			return TRUE;
		}
	case G_TYPE_STRING:
		{
			gchar *cval = seed_value_to_string(val, exception);

			g_value_init(ret, G_TYPE_STRING);
			g_value_take_string(ret, cval);

			return TRUE;
		}
	default:
		{
			switch (JSValueGetType(eng->context, val))
			{
			case kJSTypeBoolean:
				{
					g_value_init(ret, G_TYPE_BOOLEAN);
					g_value_set_boolean(ret,
										seed_value_to_boolean(val, exception));
					return TRUE;
				}
			case kJSTypeNumber:
				{
					g_value_init(ret, G_TYPE_DOUBLE);
					g_value_set_double(ret,
									   seed_value_to_double(val, exception));
					return TRUE;
				}
			case kJSTypeString:
				{
					gchar *cv = seed_value_to_string(val,
													 exception);

					g_value_init(ret, G_TYPE_STRING);
					g_value_take_string(ret, cv);
					return TRUE;
				}
			default:
				break;
			}
			break;
		}
	}

	if (g_type_is_a(type, G_TYPE_ENUM) && JSValueIsNumber(eng->context, val))
	{
		g_value_init(ret, type);
		ret->data[0].v_long = seed_value_to_long(val, exception);
		return TRUE;
	}
	else if (g_type_is_a(type, G_TYPE_FLAGS)
			 && JSValueIsNumber(eng->context, val))
	{
		g_value_init(ret, type);
		ret->data[0].v_long = seed_value_to_long(val, exception);
		return TRUE;
	}
	else if (g_type_is_a(type, G_TYPE_OBJECT)
			 && (JSValueIsNull(eng->context, val)
				 || seed_value_is_gobject(val)))
	{
		GObject *o = seed_value_to_object(val, exception);

		if (o == NULL || g_type_is_a(G_OBJECT_TYPE(o), type))
		{
			g_value_init(ret, G_TYPE_OBJECT);
			g_value_set_object(ret, o);

			return TRUE;
		}
	}
	/* Boxed handling is broken. Will be fixed in struct overhall. */
	else if (g_type_is_a(type, G_TYPE_BOXED))
	{
		gpointer p = seed_pointer_get_pointer(val);
		if (p)
		{
			g_value_init(ret, type);
			g_value_set_boxed(ret, p);
			return TRUE;
		}
		else
		{
			if (JSValueIsObject(eng->context, val))
			{
				GIBaseInfo * info = g_irepository_find_by_gtype(0, type);
				JSObjectRef new_struct;
				if (!info)
					return FALSE;

				new_struct	= seed_construct_struct_type_with_parameters(info,
															 (JSObjectRef)val,
															       exception);
				p = seed_pointer_get_pointer(new_struct);
				if (p)
				{
					g_value_init(ret, type);
					g_value_set_boxed(ret, p);
					g_base_info_unref(info);
					return TRUE;
				}
				g_base_info_unref(info);
			}
		}
	}

	return FALSE;
}

JSValueRef seed_object_get_property(JSObjectRef val, const gchar * name)
{

	JSStringRef jname = JSStringCreateWithUTF8CString(name);
	JSValueRef ret = JSObjectGetProperty(eng->context,
										 (JSObjectRef) val,
										 jname, NULL);

	JSStringRelease(jname);

	return ret;
}

gboolean
seed_object_set_property(JSObjectRef object,
						 const gchar * name, JSValueRef value)
{
	JSStringRef jname = JSStringCreateWithUTF8CString(name);

	if (value)
	{
		JSObjectSetProperty(eng->context, (JSObjectRef) object,
							jname, value, 0, 0);
	}

	JSStringRelease(jname);

	return TRUE;
}

/* TODO: Make some macros or something for making exceptions, code is littered
   with annoyingness right now */
gboolean seed_value_to_boolean(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsBoolean(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
		{
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to boolean");
			return 0;
		}

		return 0;
	}

	return JSValueToBoolean(eng->context, val);
}

JSValueRef seed_value_from_boolean(gboolean val, JSValueRef * exception)
{
	return JSValueMakeBoolean(eng->context, val);
}

guint seed_value_to_uint(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
		{
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to"
								" boolean");
		}
		return 0;
	}

	return (guint) JSValueToNumber(eng->context, val, NULL);
}

JSValueRef seed_value_from_uint(guint val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

gint seed_value_to_int(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to" " int");
		return 0;
	}

	return (gint) JSValueToNumber(eng->context, val, NULL);
}

JSValueRef seed_value_from_int(gint val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

gchar seed_value_to_char(JSValueRef val, JSValueRef * exception)
{
	gint cv;

	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to" " gchar");
		return 0;
	}

	cv = JSValueToNumber(eng->context, val, NULL);

	if (cv < G_MININT8 || cv > G_MAXINT8)
	{
		seed_make_exception(exception, "ConversionError",
							"Javascript number out of range of gchar");
		return 0;
	}

	return (char)cv;
}

JSValueRef seed_value_from_char(gchar val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

guchar seed_value_to_uchar(JSValueRef val, JSValueRef * exception)
{
	guint cv;

	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to"
								" guchar");
		return 0;
	}

	cv = JSValueToNumber(eng->context, val, NULL);

	if (cv > G_MAXUINT8)
	{
		seed_make_exception(exception, "ConversionError",
							"Javascript number out of range of guchar");
		return 0;
	}

	return (guchar) cv;
}

JSValueRef seed_value_from_uchar(guchar val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

glong seed_value_to_long(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to" " long");
		return 0;
	}

	return (glong) JSValueToNumber(eng->context, val, NULL);
}

JSValueRef seed_value_from_long(glong val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

gulong seed_value_to_ulong(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to" " ulong");

		return 0;
	}

	return (gulong) JSValueToNumber(eng->context, val, NULL);
}

JSValueRef seed_value_from_ulong(gulong val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

gint64 seed_value_to_int64(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to"
								" gint64");

		return 0;
	}

	return (gint64) JSValueToNumber(eng->context, val, NULL);
}

JSValueRef seed_value_from_int64(gint64 val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

guint64 seed_value_to_uint64(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to"
								" guint64");

		return 0;
	}

	return (guint64) JSValueToNumber(eng->context, val, NULL);
}

JSValueRef seed_value_from_uint64(guint64 val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

gfloat seed_value_to_float(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to"
								" gfloat");
		return 0;
	}

	return (gfloat) JSValueToNumber(eng->context, val, NULL);
}

JSValueRef seed_value_from_float(gfloat val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

gdouble seed_value_to_double(JSValueRef val, JSValueRef * exception)
{
	if (!JSValueIsNumber(eng->context, val))
	{
		if (!JSValueIsNull(eng->context, val))
			seed_make_exception(exception, "ConversionError",
								"Can not convert Javascript value to"
								" double");
		return 0;
	}

	return (gdouble) JSValueToNumber(eng->context, val, NULL);
}

JSValueRef seed_value_from_double(gdouble val, JSValueRef * exception)
{
	return JSValueMakeNumber(eng->context, (gdouble) val);
}

gchar *seed_value_to_string(JSValueRef val, JSValueRef * exception)
{
	JSStringRef jsstr = 0;
	JSValueRef func, str;
	gchar *buf = 0;
	gint length;

	if (val == NULL)
		return NULL;

	if (JSValueIsBoolean(eng->context, val)
		|| JSValueIsNumber(eng->context, val))
	{
		buf = g_strdup_printf("%f", JSValueToNumber(eng->context, val, NULL));
	}
	else if (JSValueIsNull(eng->context, val)
			 || JSValueIsUndefined(eng->context, val))
	{
		buf = strdup("[null]");
	}
	else
	{
		if (!JSValueIsString(eng->context, val))	// In this case,
			// it's an object
		{
			func = seed_object_get_property((JSObjectRef) val, "toString");
			str =
				JSObjectCallAsFunction(eng->context,
									   (JSObjectRef) func,
									   (JSObjectRef) val, 0, NULL, NULL);
		}

		jsstr = JSValueToStringCopy(eng->context, val, NULL);
		length = JSStringGetMaximumUTF8CStringSize(jsstr);
		if (length > 0)
		{
			buf = g_malloc(length * sizeof(gchar));
			JSStringGetUTF8CString(jsstr, buf, length);
		}
		if (jsstr)
			JSStringRelease(jsstr);
	}

	return buf;
}

JSValueRef seed_value_from_string(const gchar * val, JSValueRef * exception)
{
	JSStringRef jsstr = JSStringCreateWithUTF8CString(val);
	JSValueRef valstr = JSValueMakeString(eng->context, jsstr);
	JSStringRelease(jsstr);

	return valstr;
}

GObject *seed_value_to_object(JSValueRef val, JSValueRef * exception)
{
	GObject *gobject;

	/* Worth investigating if this is the best way to handle null. Some of 
	   the existing code depends on null Objects not throwing an exception however.
	   needs testing at higher level if value can be null (through GI) */
	if (JSValueIsNull(eng->context, val))
		return 0;
	if (!seed_value_is_gobject(val))
	{
		seed_make_exception(exception, "ConversionError",
							"Attempt to convert from"
							" non GObject to GObject");
		return NULL;
	}

	gobject = (GObject *) JSObjectGetPrivate((JSObjectRef) val);

	return gobject;
}

JSValueRef seed_value_from_object(GObject * val, JSValueRef * exception)
{
	if (val == NULL)
		return JSValueMakeNull(eng->context);
	else
		return seed_wrap_object(val);
}
