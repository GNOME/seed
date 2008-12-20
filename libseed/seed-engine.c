/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed-engine.c
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

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

GQuark qname = 0;
GQuark qprototype = 0;

JSClassRef gobject_signal_class;
JSClassRef seed_struct_constructor_class;

JSContextGroupRef context_group;

gchar *glib_message = 0;

GIBaseInfo * base_info_info = 0;

guint seed_debug_flags = 0;		/* global seed debug flag */

#ifdef SEED_ENABLE_DEBUG
static const GDebugKey seed_debug_keys[] = {
	{"misc", SEED_DEBUG_MISC},
	{"finalization", SEED_DEBUG_FINALIZATION},
	{"initialization", SEED_DEBUG_INITIALIZATION},
	{"signal", SEED_DEBUG_SIGNAL},
	{"invocation", SEED_DEBUG_INVOCATION},
	{"structs", SEED_DEBUG_STRUCTS},
	{"construction", SEED_DEBUG_CONSTRUCTION},
	{"gtype", SEED_DEBUG_GTYPE}
};
#endif							/* SEED_ENABLE_DEBUG */


// TODO: JSON field initialization.
static JSObjectRef
seed_struct_constructor_invoked(JSContextRef ctx,
								 JSObjectRef constructor,
								 size_t argumentCount,
								 const JSValueRef arguments[],
								 JSValueRef * exception)
{
	GIBaseInfo * info = JSObjectGetPrivate(constructor);
	JSValueRef ret;
	JSObjectRef parameters = 0;

	if (argumentCount == 1)
	{
		if (!JSValueIsObject(ctx, arguments[0]))
		{
			seed_make_exception(ctx, exception, "ArgmuentError",
								"Constructor expects object as argument");
			return (JSObjectRef) JSValueMakeNull(ctx);		
		}
		parameters = (JSObjectRef)arguments[0];
	}
	ret = seed_construct_struct_type_with_parameters(ctx, info,
													 parameters,
													 exception);
		
}

static JSObjectRef
seed_gobject_constructor_invoked(JSContextRef ctx,
								 JSObjectRef constructor,
								 size_t argumentCount,
								 const JSValueRef arguments[],
								 JSValueRef * exception)
{
	GType type;
	GParameter *params;
	GObjectClass *oclass;
	GObject *gobject;
	// Do we free GParamSpecs...? It's not clear.
	GParamSpec *param_spec;
	gchar *prop_name;
	gint i, nparams = 0, length;
	JSObjectRef ret;
	JSPropertyNameArrayRef jsprops = 0;
	JSStringRef jsprop_name;
	JSValueRef jsprop_value;

	type = (GType) JSObjectGetPrivate(constructor);
	if (!type)
		return 0;

	oclass = g_type_class_ref(type);

	if (argumentCount > 1)
	{
		gchar *mes = g_strdup_printf("Constructor expects"
									 " 1 argument, got %d", argumentCount);
		seed_make_exception(ctx, exception, "ArgumentError", mes);
		g_free(mes);

		return (JSObjectRef) JSValueMakeNull(ctx);
	}

	if (argumentCount == 1)
	{
		if (!JSValueIsObject(ctx, arguments[0]))
		{
			seed_make_exception(ctx, exception, "ArgmuentError",
								"Constructor expects object as argument");
			g_type_class_unref(oclass);
			return (JSObjectRef) JSValueMakeNull(ctx);
		}

		jsprops = JSObjectCopyPropertyNames(ctx,
											(JSObjectRef) arguments[0]);
		nparams = JSPropertyNameArrayGetCount(jsprops);
	}
	i = 0;

	params = g_new0(GParameter, nparams + 1);
	SEED_NOTE(INITIALIZATION, "Constructing object of type %s",
			  g_type_name(type));

	while (i < nparams)
	{
		GType type;
		jsprop_name = JSPropertyNameArrayGetNameAtIndex(jsprops, i);

		length = JSStringGetMaximumUTF8CStringSize(jsprop_name);
		prop_name = g_malloc(length * sizeof(gchar));
		JSStringGetUTF8CString(jsprop_name, prop_name, length);

		param_spec = g_object_class_find_property(oclass, prop_name);
		if (param_spec == NULL)
		{
			gchar *mes =
				g_strdup_printf("Invalid property for construction: %s",
								prop_name);
			seed_make_exception(ctx, exception, "PropertyError", mes);

			g_free(mes);
			g_free(params);
			
			JSPropertyNameArrayRelease(jsprops);
			return (JSObjectRef) JSValueMakeNull(ctx);
		}
		// TODO: exception handling
		jsprop_value = JSObjectGetProperty(ctx,
										   (JSObjectRef) arguments[0],
										   jsprop_name, NULL);

		if (g_type_is_a(param_spec->value_type, G_TYPE_ENUM))
			type = G_TYPE_INT;
		else
			type = param_spec->value_type;

		seed_gvalue_from_seed_value(ctx, jsprop_value,
									type, &params[i].value, exception);

		if (*exception)
		{

			g_free(prop_name);
			g_free(params);
			JSPropertyNameArrayRelease(jsprops);
			return 0;
		}
		params[i].name = prop_name;

		++i;
	}

	if (jsprops)
		JSPropertyNameArrayRelease(jsprops);

	gobject = g_object_newv(type, nparams, params);

	g_object_ref_sink(gobject);

	if (!gobject)
		ret = (JSObjectRef) JSValueMakeNull(ctx);
	else
		ret = (JSObjectRef) seed_value_from_object(ctx, gobject, exception);
	
	for (i = 0; i < nparams; i++)
	{
		g_value_unset(&params[i].value);
		g_free((gchar *)params[i].name);
	}

	g_object_unref(gobject);

	g_type_class_unref(oclass);

	g_free(params);

	return ret;
}

static JSValueRef
seed_gobject_equals(JSContextRef ctx,
					JSObjectRef function,
					JSObjectRef this_object,
					size_t argumentCount,
					const JSValueRef arguments[], JSValueRef * exception)
{
	GObject *this, *that;

	if (argumentCount != 1)
	{
		gchar *mes = g_strdup_printf("GObject equals comparison expected"
									 " 1 argument, got %d", argumentCount);
		seed_make_exception(ctx, exception, "ArgumentError", mes);
     		g_free(mes);

		return JSValueMakeNull(ctx);
	}

	this = seed_value_to_object(ctx, (JSValueRef) this_object, exception);
	that = seed_value_to_object(ctx, arguments[0], exception);

	if (this == that)
		return seed_value_from_boolean(ctx, 1, 0);
	return seed_value_from_boolean(ctx, 0, 0);
}

static JSValueRef
seed_gobject_ref_count(JSContextRef ctx,
					   JSObjectRef function,
					   JSObjectRef this_object,
					   size_t argumentCount,
					   const JSValueRef arguments[], JSValueRef * exception)
{
	GObject *this;

	this = seed_value_to_object(ctx, (JSValueRef) this_object, exception);

	return seed_value_from_int(ctx, this->ref_count, exception);
}

static void
seed_gobject_method_finalize(JSObjectRef method)
{
	GIBaseInfo * info = (GIBaseInfo *)JSObjectGetPrivate(method);
	if (info)
		g_base_info_unref(info);
}

static JSValueRef
seed_gobject_method_invoked(JSContextRef ctx,
							JSObjectRef function,
							JSObjectRef this_object,
							size_t argumentCount,
							const JSValueRef arguments[],
							JSValueRef * exception)
{
	GIBaseInfo *info;
	GObject *object;
	gboolean instance_method = TRUE;
	GArgument retval;
	GArgument *in_args;
	GArgument *out_args;
	GArgument *out_values;
	gint n_args, n_in_args, n_out_args, i;
	GIArgInfo *arg_info;
	GITypeInfo *type_info;
	GIDirection dir;
	JSValueRef retval_ref;
	GError *error = 0;
	
	info = JSObjectGetPrivate(function);
	// We just want to check if there IS an object, not actually throw an
	// exception if we don't
	// get it.
	if (!
		((object = seed_value_to_object(ctx, this_object, 0)) ||
		 (object = seed_pointer_get_pointer(ctx, this_object))))
		instance_method = FALSE;

	n_args = g_callable_info_get_n_args((GICallableInfo *) info);

	in_args = g_new0(GArgument, n_args + 1);
	out_args = g_new0(GArgument, n_args + 1);
	out_values = g_new0(GArgument, n_args + 1);
	n_in_args = n_out_args = 0;

	if (instance_method)
		in_args[n_in_args++].v_pointer = object;

	for (i = 0; (i < (n_args)); i++)
	{
		arg_info = g_callable_info_get_arg((GICallableInfo *) info, i);
		dir = g_arg_info_get_direction(arg_info);
		type_info = g_arg_info_get_type(arg_info);
		if (i + 1 > argumentCount)
		{
			in_args[n_in_args++].v_pointer = 0;
		}
		else if (dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT)
		{

			if (!seed_gi_make_argument(ctx, arguments[i],
									   type_info,
									   &in_args[n_in_args++], exception))
			{
				gchar *mes =
					g_strdup_printf
					("Unable to make argument %d for" " function: %s. \n",
					 i + 1,
					 g_base_info_get_name((GIBaseInfo *) info));
				seed_make_exception(ctx, exception, "ArgumentError", mes);

				g_free(mes);
				g_base_info_unref((GIBaseInfo *) type_info);
				g_base_info_unref((GIBaseInfo *) arg_info);
				g_free(in_args);
				g_free(out_args);

				return JSValueMakeNull(ctx);
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

		g_base_info_unref((GIBaseInfo *) type_info);
		g_base_info_unref((GIBaseInfo *) arg_info);
	}
	SEED_NOTE(INVOCATION, "Invoking method: %s with %d in arguments"
			  " and %d out arguments",
			  g_base_info_get_name(info), n_in_args, n_out_args);
	if (g_function_info_invoke((GIFunctionInfo *) info,
							   in_args,
							   n_in_args,
							   out_args, n_out_args, &retval, &error))
	{
		GITypeTag tag;

		type_info = g_callable_info_get_return_type((GICallableInfo *) info);
		tag = g_type_info_get_tag(type_info);
		if (tag == GI_TYPE_TAG_VOID)
			retval_ref = JSValueMakeNull(ctx);
		else
		{
			GIBaseInfo *interface;
			GIInfoType type;
			gboolean sunk = TRUE;

			if (tag == GI_TYPE_TAG_INTERFACE)
			{
				GIFunctionInfoFlags flags =
					g_function_info_get_flags((GIFunctionInfo *) info);

				GIBaseInfo *interface;
				GIInfoType interface_type;
				
				interface = g_type_info_get_interface(type_info);
				interface_type = g_base_info_get_type(interface);
				g_base_info_unref(interface);
				
				if (interface_type == GI_INFO_TYPE_OBJECT ||
					interface_type == GI_INFO_TYPE_INTERFACE)
				{
					if (G_IS_OBJECT(retval.v_pointer))
					{
						sunk =
							g_object_is_floating(G_OBJECT(retval.v_pointer));
						g_object_ref_sink(G_OBJECT(retval.v_pointer));
					}
				}
				
			}
			retval_ref =
				seed_gi_argument_make_js(ctx, &retval, type_info, exception);

			if (!sunk)
				g_object_unref(G_OBJECT(retval.v_pointer));
			
			seed_gi_release_arg(
				g_callable_info_get_caller_owns((GICallableInfo*) info),
								type_info, &retval);
		}
		g_base_info_unref((GIBaseInfo *) type_info);
	}
	else
	{
		seed_make_exception_from_gerror(ctx, exception, error);

		g_free(in_args);
		g_free(out_args);
		g_error_free(error);

		return JSValueMakeNull(ctx);
	}
	for (i = 0; (i < n_args); i++)
	{
		JSValueRef jsout_val;
		arg_info = g_callable_info_get_arg((GICallableInfo *) info, i);
		dir = g_arg_info_get_direction(arg_info);
		type_info = g_arg_info_get_type(arg_info);

		if (dir == GI_DIRECTION_IN)
		{
			seed_gi_release_in_arg(g_arg_info_get_ownership_transfer(arg_info),
								   type_info,
								   &in_args[i + (instance_method ? 1 : 0)]);

			g_base_info_unref((GIBaseInfo *) type_info);
			g_base_info_unref((GIBaseInfo *) arg_info);
			continue;
		}
		jsout_val = seed_gi_argument_make_js(ctx, &out_values[i],
											 type_info, exception);
		if (!JSValueIsNull(ctx, arguments[i]) &&
			JSValueIsObject(ctx, arguments[i]))
		{
			seed_object_set_property(ctx, (JSObjectRef) arguments[i],
									 "value", jsout_val);
		}

		g_base_info_unref((GIBaseInfo *) arg_info);
		g_base_info_unref((GIBaseInfo *) type_info);
	}

	g_free(in_args);
	g_free(out_args);
	g_free(out_values);
	return retval_ref;
}

void
seed_gobject_define_property_from_function_info(JSContextRef ctx,
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
	

	flags = g_function_info_get_flags(info);

	if (instance && (flags & GI_FUNCTION_IS_CONSTRUCTOR))
	{
		return;
	}

	method_ref = JSObjectMake(ctx, gobject_method_class, 
							  g_base_info_ref((GIBaseInfo *)info));

	name = g_base_info_get_name((GIBaseInfo *) info);
	if (!strcmp(name, "new"))
		name = "_new";
	seed_object_set_property(ctx, object, name, method_ref);
	seed_object_set_property(ctx, method_ref, "info", 
							 seed_make_struct(ctx, 
							  g_base_info_ref((GIBaseInfo *)info), 
											  base_info_info));

}

static void
seed_gobject_add_methods_for_interfaces(JSContextRef ctx,
										GIObjectInfo * oinfo,
										JSObjectRef object)
{
	GIInterfaceInfo *interface;
	gint n_interfaces, i;

	n_interfaces = g_object_info_get_n_interfaces(oinfo);

	for (i = 0; i < n_interfaces; i++)
	{
		GIFunctionInfo *function;
		gint n_functions, k;
		interface = g_object_info_get_interface(oinfo, i);

		n_functions = g_interface_info_get_n_methods(interface);
		for (k = 0; k < n_functions; k++)
		{
			function = g_interface_info_get_method(interface, k);
			seed_gobject_define_property_from_function_info
				(ctx, function, object, TRUE);
		}
		// g_base_info_unref((GIBaseInfo*)interface);
	}
}

static void
seed_gobject_add_methods_for_type(JSContextRef ctx, 
								  GIObjectInfo * oinfo, JSObjectRef object)
{
	gint n_methods;
	gint i;
	GIFunctionInfo *info;

	n_methods = g_object_info_get_n_methods(oinfo);

	for (i = 0; i < n_methods; i++)
	{
		info = g_object_info_get_method(oinfo, i);
		seed_gobject_define_property_from_function_info(ctx, 
														info, object, TRUE);
	}
}

JSClassRef seed_gobject_get_class_for_gtype(JSContextRef ctx, GType type)
{
	JSClassDefinition def;
	GType parent;
	JSClassRef ref;
	JSClassRef parent_class = 0;
	GIBaseInfo *info;
	JSObjectRef prototype_obj;
	JSObjectRef parent_prototype;

	if ((ref = g_type_get_qdata(type, qname)) != NULL)
	{
		return ref;
	}

	info = g_irepository_find_by_gtype(g_irepository_get_default(), type);

	memset(&def, 0, sizeof(JSClassDefinition));

	def.className = g_type_name(type);
	if ((parent = g_type_parent(type)))
		parent_class = seed_gobject_get_class_for_gtype(ctx, parent);
	def.parentClass = parent_class;
	def.attributes = kJSClassAttributeNoAutomaticPrototype;

	prototype_obj = JSObjectMake(ctx, 0, 0);
	if (parent)
	{
		parent_prototype = seed_gobject_get_prototype_for_gtype(parent);
		if (parent_prototype)
			JSObjectSetPrototype(ctx, prototype_obj, parent_prototype);
	}

	ref = JSClassCreate(&def);
	JSClassRetain(ref);

	JSValueProtect(ctx, prototype_obj);

	g_type_set_qdata(type, qname, ref);
	g_type_set_qdata(type, qprototype, prototype_obj);

	if (info && (g_base_info_get_type(info) == GI_INFO_TYPE_OBJECT))
	{
		seed_gobject_add_methods_for_type(ctx, 
										  (GIObjectInfo *) info, prototype_obj);
		seed_gobject_add_methods_for_interfaces(ctx, (GIObjectInfo *) info,
												prototype_obj);
	}
	else
	{
		GType *interfaces;
		GIFunctionInfo *function;
		GIBaseInfo *interface;
		gint n_functions, k, i, n;

		interfaces = g_type_interfaces(type, &n);
		for (i = 0; i < n; i++)
		{
			interface = g_irepository_find_by_gtype(0, interfaces[i]);
			if (!interface)
				break;
			n_functions =
				g_interface_info_get_n_methods((GIInterfaceInfo *) interface);
			for (k = 0; k < n_functions; k++)
			{
				function =
					g_interface_info_get_method((GIInterfaceInfo
												 *) interface, k);
				seed_gobject_define_property_from_function_info
					(ctx, function, prototype_obj, TRUE);
			}
		}
	}

	return ref;
}

JSObjectRef seed_gobject_get_prototype_for_gtype(GType type)
{
	JSObjectRef prototype = 0;
	while (type && !prototype)
	{
		prototype = g_type_get_qdata(type, qprototype);
		type = g_type_parent(type);
	}

	return prototype;
}

static void seed_gobject_finalize(JSObjectRef object)
{
	GObject *gobject;

	gobject = (GObject *)JSObjectGetPrivate((JSObjectRef)object);
	if (!gobject)
	{
		SEED_NOTE(FINALIZATION,
				  "Attempting to finalize already destroyed object.");
		return;
	}

	SEED_NOTE(FINALIZATION, "%s at %p (%d refs)",
			  g_type_name(G_OBJECT_TYPE(gobject)), gobject, gobject->ref_count);

	if (g_object_get_data(gobject, "js-ref"))
	{
		g_object_set_data_full(gobject, "js-ref", NULL, NULL);

		g_object_remove_toggle_ref(gobject, seed_toggle_ref, 0);
	}
	g_object_run_dispose(gobject);
}

static void seed_gobject_initialize(JSContextRef ctx, JSObjectRef object)
{
	GObject *gobject;
	GIBaseInfo *base;

	gobject = seed_value_to_object(ctx, (JSValueRef) object, 0);
	if (!gobject)
		return;

	base = g_irepository_find_by_gtype(g_irepository_get_default(),
									   G_OBJECT_TYPE(gobject));

	seed_add_signals_to_object(ctx, object, gobject);
	if (!base)
		return;

	g_assert(g_base_info_get_type(base) == GI_INFO_TYPE_OBJECT);
}

static JSValueRef
seed_gobject_get_property(JSContextRef context,
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

	b = seed_value_to_object(context, (JSValueRef) object, exception);
	if (!b)
		return 0;

	length = JSStringGetMaximumUTF8CStringSize(property_name);
	cproperty_name = g_malloc(length * sizeof(gchar));
	JSStringGetUTF8CString(property_name, cproperty_name, length);

	spec = g_object_class_find_property(G_OBJECT_GET_CLASS(b), cproperty_name);

	if (!spec)
	{

		len = strlen(cproperty_name);
		for (i = 0; i < len - 1; i++)
		{
			if (cproperty_name[i] == '_')
				cproperty_name[i] = '-';
		}
		spec = g_object_class_find_property(G_OBJECT_GET_CLASS(b),
											cproperty_name);
		if (spec)
			goto found;
		else
		{
			GIFieldInfo *field;
			GIBaseInfo *info = (GIBaseInfo *)
				g_irepository_find_by_gtype(0, G_OBJECT_TYPE(b));
			gint n;
			const gchar *name;

			for (i = 0; i < len - 1; i++)
			{
				if (cproperty_name[i] == '-')
					cproperty_name[i] = '_';
			}

			if (!info)
			{
				g_free(cproperty_name);
				return 0;
			}

			n = g_object_info_get_n_fields((GIObjectInfo *) info);
			for (i = 0; i < n; i++)
			{
				field = g_object_info_get_field((GIObjectInfo *) info, i);
				name = g_base_info_get_name((GIBaseInfo *) field);

				if (!strcmp(name, cproperty_name))
					goto found_field;
				else
				{
					g_base_info_unref((GIBaseInfo *) field);
					field = 0;
				}
			}
 found_field:
			if (field)
			{
				ret = seed_field_get_value(context, b, 
										   field, exception);
				g_base_info_unref((GIBaseInfo *) info);
				g_free(cproperty_name);
				return ret;		
			}
			g_base_info_unref((GIBaseInfo *) info);
		}
		g_free(cproperty_name);
		return 0;
	}
 found:

	g_value_init(&gval, spec->value_type);
	g_object_get_property(b, cproperty_name, &gval);
	ret = seed_value_from_gvalue(context, &gval, exception);
	g_value_unset(&gval);

	g_free(cproperty_name);
	return (JSValueRef) ret;
}

static bool
seed_gobject_set_property(JSContextRef context,
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

	if (JSValueIsNull(context, value))
		return 0;

	obj = seed_value_to_object(context, object, 0);

	length = JSStringGetMaximumUTF8CStringSize(property_name);
	cproperty_name = g_malloc(length * sizeof(gchar));
	JSStringGetUTF8CString(property_name, cproperty_name, length);

	spec = g_object_class_find_property(G_OBJECT_GET_CLASS(obj),
										cproperty_name);

	if (!spec)
	{
		gint i, len;
		len = strlen(cproperty_name);
		for (i = 0; i < len; i++)
		{
			if (cproperty_name[i] == '_')
				cproperty_name[i] = '-';
		}
		spec = g_object_class_find_property(G_OBJECT_GET_CLASS(obj),
											cproperty_name);
		if (!spec)
		{
			g_free(cproperty_name);
			return 0;
		}
	}

	if (g_type_is_a(spec->value_type, G_TYPE_ENUM))
		type = G_TYPE_LONG;
	else
		type = spec->value_type;

	seed_gvalue_from_seed_value(context, value, type, &gval, exception);
	if (*exception)
	{
		g_free(cproperty_name);
		return 0;
	}

	if (glib_message)
	{
		g_free(glib_message);
		glib_message = 0;
	}
	g_object_set_property(obj, cproperty_name, &gval);
	if (glib_message != 0)
	{
		seed_make_exception(context, exception, "PropertyError", glib_message);

		return FALSE;
	}

	g_free(cproperty_name);
	g_value_unset(&gval);

	return TRUE;
}

static GModule * seed_try_load_extension(gchar * name)
{
	GModule * module;
	gchar * path;
	gchar * lowername;

	lowername = g_utf8_strdown(name, -1);

	path = g_module_build_path("/usr/local/lib/seed", lowername);
	
	module = g_module_open(path, G_MODULE_BIND_LAZY);

	if (!module)
	{
		g_free(path);
		path = g_module_build_path("/usr/lib/seed", lowername);
		
		module = g_module_open(path, G_MODULE_BIND_LAZY);
	}
	g_free(path);
	g_free(lowername);
	return module;	
}

static JSValueRef
seed_gi_import_namespace(JSContextRef ctx,
						 JSObjectRef function,
						 JSObjectRef this_object,
						 size_t argumentCount,
						 const JSValueRef arguments[], JSValueRef * exception)
{
	GIBaseInfo *info;
	GError *e = 0;
	gchar *namespace;
	const gchar *version = 0;
	gchar * jsextension;
	JSObjectRef namespace_ref;
	JSStringRef extension_script;
	gint n, i;
	GModule * extension;

	if (argumentCount == 0)
	{
		seed_make_exception(ctx, exception,
							"ArgumentError",
							"Seed.import_namespace"
							" expected 1 or 2 arguments, got 0");
		return JSValueMakeNull(ctx);
	}

	namespace = seed_value_to_string(ctx, arguments[0], exception);
	if (argumentCount == 2)
	{
		version = seed_value_to_string(ctx, arguments[1], exception);
	}
	
	extension = seed_try_load_extension(namespace);
	if (extension)
	{
		SeedModuleInitCallback init;
		
		g_module_symbol(extension,
						"seed_module_init",
						(gpointer *)&init);

		(*init)(eng);
		
		g_free(namespace);
		return JSValueMakeNull(ctx);
	}

	if (!g_irepository_require(g_irepository_get_default(), namespace,
							   version, 0, &e))
	{
		seed_make_exception_from_gerror(ctx, exception, e);
		return JSValueMakeNull(ctx);
	}

	n = g_irepository_get_n_infos(g_irepository_get_default(), namespace);

	namespace_ref = JSObjectMake(ctx, NULL, NULL);
	JSValueProtect(ctx, namespace_ref);
	seed_object_set_property(ctx, eng->global, namespace, namespace_ref);

	for (i = 0; i < n; i++)
	{
		info = g_irepository_get_info(g_irepository_get_default(),
									  namespace, i);
		if (info && (g_base_info_get_type(info) == GI_INFO_TYPE_FUNCTION))
		{
			seed_gobject_define_property_from_function_info(ctx, 
															(GIFunctionInfo *)
															info, namespace_ref,
															FALSE);
		}
		else if (info
				 && (g_base_info_get_type(info) == GI_INFO_TYPE_ENUM
					 || g_base_info_get_type(info) == GI_INFO_TYPE_FLAGS))
		{
			gint num_vals = g_enum_info_get_n_values((GIEnumInfo *) info);
			gint j;
			JSObjectRef enum_class = JSObjectMake(ctx,
												  0, 0);
			JSValueProtect(ctx, (JSValueRef) enum_class);
			seed_object_set_property(ctx, namespace_ref,
									 g_base_info_get_name(info), enum_class);

			for (j = 0; j < num_vals; j++)
			{
				GIValueInfo *val =
					g_enum_info_get_value((GIEnumInfo *) info, j);
				gint value = g_value_info_get_value(val);
				gchar *name =
					g_strdup(g_base_info_get_name((GIBaseInfo *) val));
				gint name_len = strlen(name);
				gint j;
				JSValueRef value_ref;

				value_ref = JSValueMakeNumber(ctx, value);
				JSValueProtect(ctx, (JSValueRef) value_ref);

				for (j = 0; j < name_len; j++)
				{
					if (name[j] == '-')
						name[j] = '_';
				}
				
				// Non ascii enums? :P
				*name = g_ascii_toupper(*name);

				seed_object_set_property(ctx, enum_class, name, value_ref);

				g_free(name);

			}
		}
		else if (info && (g_base_info_get_type(info) == GI_INFO_TYPE_OBJECT))
		{
			GType type;
			JSClassRef class_ref;

			type =
				g_registered_type_info_get_g_type((GIRegisteredTypeInfo *)
												  info);

			if (type != 0)
			{
				JSObjectRef constructor_ref;
				gint i, n_methods;
				GIFunctionInfo *finfo;
				GIFunctionInfoFlags flags;

				class_ref = seed_gobject_get_class_for_gtype(ctx, 
															 type);

				constructor_ref =
					JSObjectMake(ctx,
								 gobject_constructor_class, (gpointer) type);

				seed_object_set_property(ctx, constructor_ref,
										 "type",
										 seed_value_from_int(ctx, type, 
															 exception));

				n_methods = g_object_info_get_n_methods((GIObjectInfo *) info);
				for (i = 0; i < n_methods; i++)
				{
					finfo = g_object_info_get_method((GIObjectInfo *) info, i);
					flags = g_function_info_get_flags(finfo);
					if (!(flags & GI_FUNCTION_IS_METHOD))
					{
						seed_gobject_define_property_from_function_info
							(ctx, finfo, constructor_ref, FALSE);
					}
					else
					{
						g_base_info_unref((GIBaseInfo *) finfo);
					}
				}

				seed_object_set_property(ctx, namespace_ref,
										 g_base_info_get_name
										 (info), constructor_ref);
				seed_object_set_property(ctx, constructor_ref,
										 "prototype",
          		    seed_gobject_get_prototype_for_gtype(type));
				JSValueProtect(ctx, (JSValueRef) constructor_ref);
			}
		}
		else if (info && (g_base_info_get_type(info) == GI_INFO_TYPE_STRUCT))
		{
			JSObjectRef struct_ref;
			gint i, n_methods;
			GIFunctionInfo *finfo;

			struct_ref = JSObjectMake(ctx, 
									  seed_struct_constructor_class, 
									  info);

			n_methods = g_struct_info_get_n_methods((GIStructInfo *) info);

			for (i = 0; i < n_methods; i++)
			{
				finfo = g_struct_info_get_method((GIStructInfo *) info, i);
				seed_gobject_define_property_from_function_info
					(ctx, finfo, struct_ref, FALSE);

			}

			seed_object_set_property(ctx, namespace_ref,
									 g_base_info_get_name(info), struct_ref);

			JSValueProtect(ctx, (JSValueRef) struct_ref);
		}
		else if (info && (g_base_info_get_type(info) == GI_INFO_TYPE_UNION))
		{
			JSObjectRef struct_ref;
			gint i, n_methods;
			GIFunctionInfo *finfo;

			struct_ref = JSObjectMake(ctx, 
									  seed_struct_constructor_class, 
									  info);

			n_methods = g_union_info_get_n_methods((GIUnionInfo *) info);

			for (i = 0; i < n_methods; i++)
			{
				finfo = g_union_info_get_method((GIUnionInfo *) info, i);
				seed_gobject_define_property_from_function_info
					(ctx, finfo, struct_ref, FALSE);

			}

			seed_object_set_property(ctx, namespace_ref,
									 g_base_info_get_name(info), struct_ref);

			JSValueProtect(ctx, (JSValueRef) struct_ref);
		}
		else if (info && (g_base_info_get_type(info) == GI_INFO_TYPE_CALLBACK))
		{
			JSObjectRef callback_ref = JSObjectMake(ctx,
													seed_callback_class,
													info);
			seed_object_set_property(ctx, namespace_ref,
									 g_base_info_get_name(info),
									 (JSValueRef) callback_ref);
		}
		else if (info && (g_base_info_get_type(info) == GI_INFO_TYPE_CONSTANT))
		{
			GArgument argument;
			JSValueRef constant_value;

			g_constant_info_get_value((GIConstantInfo *) info, &argument);
			constant_value =
				seed_gi_argument_make_js(ctx, &argument,
			     g_constant_info_get_type((GIConstantInfo *) info), exception);
			seed_object_set_property(ctx, namespace_ref,
									 g_base_info_get_name(info),
									 constant_value);

		}
		g_base_info_unref(info);
	}

	jsextension =
		g_strdup_printf("try{Seed.include(\"/usr/share/seed/%s.js\");}"
						"catch(e){}"
						"Seed.include(\"/usr/local/share/seed/%s.js\");",
						namespace, namespace);
	extension_script = JSStringCreateWithUTF8CString(jsextension);
	JSEvaluateScript(ctx, extension_script, NULL, NULL, 0, NULL);
	JSStringRelease(extension_script);

	g_free((gchar *) namespace);
	g_free(jsextension);

	return JSValueMakeNull(ctx);
}

JSStaticFunction gobject_static_funcs[] = {
	{"equals", seed_gobject_equals, 0}
	,
	{"__debug_ref_count", seed_gobject_ref_count, 0}
	,
	{0, 0, 0}
};

JSClassDefinition gobject_def = {
	0,							/* Version, always 0 */
	kJSClassAttributeNoAutomaticPrototype,	/* JSClassAttributes */
	"gobject",					/* Class Name */
	NULL,						/* Parent Class */
	NULL,						/* Static Values */
	gobject_static_funcs,		/* Static Functions */
	seed_gobject_initialize,	/* Initialize */
	seed_gobject_finalize,		/* Finalize */
	NULL,						/* Has Property */
	seed_gobject_get_property,	/* Get Property */
	seed_gobject_set_property,	/* Set Property */
	NULL,						/* Delete Property */
	NULL,						/* Get Property Names */
	NULL,						/* Call As Function */
	NULL,						/* Call As Constructor */
	NULL,						/* Has Instance */
	NULL						/* Convert To Type */
};

JSClassDefinition gobject_method_def = {
	0,							/* Version, always 0 */
	0,
	"gobject_method",			/* Class Name */
	NULL,						/* Parent Class */
	NULL,						/* Static Values */
	NULL,						/* Static Functions */
	NULL,
	seed_gobject_method_finalize,						/* Finalize */
	NULL,						/* Has Property */
	NULL,						/* Get Property */
	NULL,						/* Set Property */
	NULL,						/* Delete Property */
	NULL,						/* Get Property Names */
	seed_gobject_method_invoked,	/* Call As Function */
	NULL,						/* Call As Constructor */
	NULL,						/* Has Instance */
	NULL						/* Convert To Type */
};

JSClassDefinition seed_callback_def = {
	0,							/* Version, always 0 */
	0,
	"seed_callback",			/* Class Name */
	NULL,						/* Parent Class */
	NULL,						/* Static Values */
	NULL,						/* Static Functions */
	NULL,
	NULL,						/* Finalize */
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

JSClassDefinition gobject_constructor_def = {
	0,							/* Version, always 0 */
	0,
	"gobject_constructor",		/* Class Name */
	NULL,						/* Parent Class */
	NULL,						/* Static Values */
	NULL,						/* Static Functions */
	NULL,
	NULL,						/* Finalize */
	NULL,						/* Has Property */
	NULL,						/* Get Property */
	NULL,						/* Set Property */
	NULL,						/* Delete Property */
	NULL,						/* Get Property Names */
	NULL,						/* Call As Function */
	seed_gobject_constructor_invoked,	/* Call As Constructor */
	NULL,						/* Has Instance */
	NULL						/* Convert To Type */
};

JSClassDefinition struct_constructor_def = {
	0,							/* Version, always 0 */
	0,
	"struct_constructor",		/* Class Name */
	NULL,						/* Parent Class */
	NULL,						/* Static Values */
	NULL,						/* Static Functions */
	NULL,
	NULL,						/* Finalize */
	NULL,						/* Has Property */
	NULL,						/* Get Property */
	NULL,						/* Set Property */
	NULL,						/* Delete Property */
	NULL,						/* Get Property Names */
	NULL,						/* Call As Function */
	seed_struct_constructor_invoked,	/* Call As Constructor */
	NULL,						/* Has Instance */
	NULL						/* Convert To Type */
};

void seed_create_function(JSContextRef ctx,
						  gchar * name, 
						  gpointer func, JSObjectRef obj)
{
	JSObjectRef oref;

	oref = JSObjectMakeFunctionWithCallback(ctx, NULL, func);
	JSValueProtect(ctx, oref);
	seed_object_set_property(ctx, obj, name, oref);
}

static void
seed_log_handler(const gchar * domain,
				 GLogLevelFlags log_level,
				 const gchar * message, gpointer user_data)
{
	glib_message = g_strdup(message);
}

#ifdef SEED_ENABLE_DEBUG
static gboolean
seed_arg_debug_cb(const char *key, const char *value, gpointer user_data)
{
	seed_debug_flags |=
		g_parse_debug_string(value,
							 seed_debug_keys, G_N_ELEMENTS(seed_debug_keys));
	return TRUE;
}

static gboolean
seed_arg_no_debug_cb(const char *key, const char *value, gpointer user_data)
{
	seed_debug_flags &=
		~g_parse_debug_string(value,
							  seed_debug_keys, G_N_ELEMENTS(seed_debug_keys));
	return TRUE;
}
#endif							/* SEED_ENABLE_DEBUG */

static GOptionEntry seed_args[] = {
#ifdef SEED_ENABLE_DEBUG
	{"seed-debug", 0, 0, G_OPTION_ARG_CALLBACK, seed_arg_debug_cb,
	 "Seed debugging flags to set", "FLAGS"},
	{"seed-no-debug", 0, 0, G_OPTION_ARG_CALLBACK, seed_arg_no_debug_cb,
	 "Seed debugging flags to unset", "FLAGS"},
#endif							/* SEED_ENABLE_DEBUG */
	{NULL,},
};

GOptionGroup *seed_get_option_group(void)
{
	GOptionGroup *group;

	group = g_option_group_new("seed",
							   "Seed Options", "Show Seed Options", NULL, NULL);

	g_option_group_add_entries(group, seed_args);

	return group;
}

static gboolean seed_parse_args(int *argc, char ***argv)
{
	GOptionContext *option_context;
	GOptionGroup *seed_group;
	GError *error = NULL;
	gboolean ret = TRUE;

	option_context = g_option_context_new(NULL);
	g_option_context_set_ignore_unknown_options(option_context, TRUE);
	g_option_context_set_help_enabled(option_context, FALSE);

	/* Initiate any command line options from the backend */

	seed_group = seed_get_option_group();
	g_option_context_set_main_group(option_context, seed_group);

	if (!g_option_context_parse(option_context, argc, argv, &error))
	{
		if (error)
		{
			g_warning("%s", error->message);
			g_error_free(error);
		}

		ret = FALSE;
	}

	g_option_context_free(option_context);

	return ret;
}

SeedEngine * seed_init(gint * argc, gchar *** argv)
{
	JSObjectRef seed_obj_ref;
	JSStringRef defaults_script;
	
	signal(SIGCHLD, SIG_IGN);

	g_type_init();
	g_log_set_handler("GLib-GObject", G_LOG_LEVEL_WARNING, seed_log_handler, 0);

	if (seed_parse_args(argc, argv) == FALSE)
	{
		SEED_NOTE(MISC, "failed to parse arguments.");
		return false;
	}

	qname = g_quark_from_static_string("js-type");
	qprototype = g_quark_from_static_string("js-prototype");

	eng = (SeedEngine *) g_malloc(sizeof(SeedEngine));
	
	context_group = JSContextGroupCreate();

	eng->context = JSGlobalContextCreateInGroup(context_group, NULL);
	eng->global = JSContextGetGlobalObject(eng->context);
	eng->group = context_group;
	
	gobject_class = JSClassCreate(&gobject_def);
	JSClassRetain(gobject_class);
	gobject_method_class = JSClassCreate(&gobject_method_def);
	JSClassRetain(gobject_method_class);
	gobject_constructor_class = JSClassCreate(&gobject_constructor_def);
	JSClassRetain(gobject_constructor_class);
	gobject_signal_class = JSClassCreate(seed_get_signal_class());
	JSClassRetain(gobject_signal_class);
	seed_callback_class = JSClassCreate(&seed_callback_def);
	JSClassRetain(seed_callback_class);
	seed_struct_constructor_class = JSClassCreate(&struct_constructor_def);
	JSClassRetain(seed_struct_constructor_class);

	g_type_set_qdata(G_TYPE_OBJECT, qname, gobject_class);

	seed_obj_ref = JSObjectMake(eng->context, NULL, NULL);
	seed_object_set_property(eng->context, eng->global, "Seed", seed_obj_ref);
	JSValueProtect(eng->context, seed_obj_ref);

	seed_create_function(eng->context, "import_namespace", &seed_gi_import_namespace,
						 seed_obj_ref);
	seed_init_builtins(eng, argc, argv);
	seed_closures_init();
	seed_structs_init();

	seed_gtype_init(eng);

	defaults_script =
		JSStringCreateWithUTF8CString("Seed.import_namespace(\""
									  "GIRepository\");"
                                      "try{Seed.include(\"/usr/share/"
									  "seed/Seed.js\");} catch(e){}"
									  "Seed.include(\"/usr/local/share"
									  "/seed/Seed.js\");");
	JSEvaluateScript(eng->context, defaults_script, NULL, NULL, 0, NULL);
	JSStringRelease(defaults_script);
	
	base_info_info = g_irepository_find_by_name(0, "GIRepository",
												"IBaseInfo");

	return eng;
}

