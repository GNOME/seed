/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
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

#include <string.h>

GQuark qname = 0;
GQuark qprototype = 0;

JSClassRef gobject_signal_class;

GParamSpec ** global_prop_cache;


static void seed_make_exception(JSValueRef * exception, 
								gchar * name, gchar * message)
{
		JSStringRef js_name = 0;
		JSStringRef js_message = 0;
		JSValueRef js_name_ref = 0, js_message_ref = 0;
		JSObjectRef exception_obj;

		if (name)
		{
				js_name = JSStringCreateWithUTF8CString(name);
				js_name_ref = JSValueMakeString(eng->context, js_name); 
		}
		if (message)
		{
				js_message = JSStringCreateWithUTF8CString(message);
				js_message_ref = JSValueMakeString(eng->context, js_message);
		}

		exception_obj = JSObjectMake(eng->context, 0, NULL);
		seed_value_set_property(exception_obj, "message", js_message_ref);
		seed_value_set_property(exception_obj, "name", js_name_ref);

		*exception = exception_obj;

		JSStringRelease(js_name);
		JSStringRelease(js_message);
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
		GObject * gobject;
		// Do we free GParamSpecs...? It's not clear.
		GParamSpec * param_spec;
		gchar * prop_name;
		int i, nparams = 0, length;
		JSObjectRef ret;
		JSPropertyNameArrayRef jsprops = 0;
		JSStringRef jsprop_name;
		JSValueRef jsprop_value;

		type = (GType)JSObjectGetPrivate(constructor);
		if (!type)
				return 0;
		oclass = g_type_class_ref (type);
	
		g_assert(argumentCount <= 1);
	
		if(argumentCount == 1)
		{
				jsprops = JSObjectCopyPropertyNames(eng->context, 
													(JSObjectRef)arguments[0]);
				nparams = JSPropertyNameArrayGetCount(jsprops);
		}
		i = 0;
	
		params = g_new0(GParameter, nparams + 1);
	
		// TODO: make sure we don't die if we get passed something other than an object
	
		while(i < nparams)
		{
				jsprop_name = JSPropertyNameArrayGetNameAtIndex(jsprops, i);
		
				length = JSStringGetMaximumUTF8CStringSize(jsprop_name);
				prop_name = malloc(length * sizeof(gchar));
				JSStringGetUTF8CString(jsprop_name, prop_name, length);
		
				param_spec = g_object_class_find_property (oclass, prop_name);
				if (param_spec == NULL)
						g_error("Constructor called "
								"with invalid property \n");
				
				// TODO: exception handling
				jsprop_value = JSObjectGetProperty(eng->context, 
												   (JSObjectRef)arguments[0],
												   jsprop_name,
												   NULL);
		
				if (!seed_gvalue_from_seed_value(
							jsprop_value,
							G_PARAM_SPEC_VALUE_TYPE(param_spec), 
							&params[i].value))
				{
						g_free(prop_name);
						g_free(params);
						return 0;
				}
				params[i].name = prop_name;
		
				++i;
		}

		if (jsprops)
				JSPropertyNameArrayRelease(jsprops);
	
		gobject = g_object_newv(type, nparams, params);
	
		if (!gobject)
				JSValueMakeNull(eng->context);

		ret = (JSObjectRef)seed_value_from_object(gobject);

		g_type_class_unref(oclass);
	
		g_free(params);
	
		return ret;
}

static JSValueRef
seed_gobject_equals (JSContextRef ctx,
							JSObjectRef function,
							JSObjectRef this_object,
							size_t argumentCount,
							const JSValueRef arguments[],
							JSValueRef * exception)
{
	GObject * this, * that;
	
	g_assert(argumentCount == 1);

	this = seed_value_to_object((JSValueRef)this_object);
	that = seed_value_to_object(arguments[0]);
	
	if (this == that)
		return seed_value_from_boolean(1);
	return seed_value_from_boolean(0);
}

static  JSValueRef 
seed_gobject_method_invoked (JSContextRef ctx,
							 JSObjectRef function, 
							 JSObjectRef this_object, 
							 size_t argumentCount, 
							 const JSValueRef arguments[], 
							 JSValueRef* exception)
{
		GIBaseInfo * info;
		GObject * object;
		gboolean instance_method = TRUE;
		GArgument retval;
		GArgument *in_args;
		GArgument *out_args;
		int n_args, n_in_args, n_out_args, i;
		GIArgInfo * arg_info;
		GITypeInfo * type_info;
		GIDirection dir;
		JSValueRef retval_ref;
		GError * error = 0;
	
		info = JSObjectGetPrivate(function);
		if (!(object = seed_value_to_object(this_object)))
				instance_method = FALSE;

		n_args = g_callable_info_get_n_args((GICallableInfo *) info);

	
		in_args = g_new0(GArgument, n_args+1);
		out_args = g_new0(GArgument, n_args+1);
		n_in_args = n_out_args = 0;
	
		if(instance_method)
				in_args[n_in_args++].v_pointer = object;
		for (i = 0;(i < (n_args)); i++)
		{
				arg_info = g_callable_info_get_arg((GICallableInfo *) info,
												   i);
				dir = g_arg_info_get_direction(arg_info);
				type_info = g_arg_info_get_type(arg_info);
				if (i+1 > argumentCount)
				{
						in_args[n_in_args++].v_pointer = 0;
				}
				else if (dir == GI_DIRECTION_IN || dir == GI_DIRECTION_INOUT)
				{
			
						if(!seed_gi_make_argument(arguments[i],
												  type_info,
												  &in_args[n_in_args++]))
						{
								g_error("Unable to make argument %d for"
										" function: %s. \n",
										i+1, g_base_info_get_name(
												(GIBaseInfo*)info));
						}
						if (dir == GI_DIRECTION_INOUT)
								n_out_args++;
			    


				}
				else if (dir == GI_DIRECTION_OUT)
				{
						n_out_args++;
				}

				g_base_info_unref((GIBaseInfo *) type_info);
				g_base_info_unref((GIBaseInfo *) arg_info);
		}
		if (g_function_info_invoke((GIFunctionInfo*)info,
								   in_args,
								   n_in_args,
								   out_args,
								   n_out_args,
								   &retval,
								   &error))
		{
				type_info = 
						g_callable_info_get_return_type((GICallableInfo *)
														info);
				if (g_type_info_get_tag(type_info) == GI_TYPE_TAG_VOID)
						retval_ref = JSValueMakeNull(eng->context);
				else
						retval_ref = 
								seed_gi_argument_make_js(&retval, type_info);
				g_base_info_unref((GIBaseInfo*)type_info);
		}
		else
		{
				g_error("Error invoking function, %s. %s \n",
						g_base_info_get_name((GIBaseInfo *) info),
						error->message);
		}

		g_free(in_args);
		g_free(out_args);
		return retval_ref;
}

void seed_gobject_define_property_from_function_info(GIFunctionInfo *info, 
													 JSObjectRef object, 
													 gboolean instance)
{
		GIFunctionInfoFlags flags;
		int n_args, i;
		GIArgInfo *arg_info;
		GITypeInfo *type_info;
		GIDirection dir;
		SeedValue method_ref;
		const gchar * name;

		//if (g_base_info_is_deprecated ((GIBaseInfo *) info))
		//	g_printf("Not defining deprecated symbol: %s \n",
		//		 g_base_info_get_name((GIBaseInfo *)info));
	
		flags = g_function_info_get_flags (info);
	
		if (instance && ((flags & GI_FUNCTION_IS_CONSTRUCTOR) ||
			(flags & GI_FUNCTION_IS_GETTER) ||
			 (flags & GI_FUNCTION_IS_SETTER)))
		{
				return;
		}
	
		n_args = g_callable_info_get_n_args((GICallableInfo *) info);
		for (i = 0; i < n_args; i++)
		{
				arg_info = g_callable_info_get_arg((GICallableInfo *) info, i);
				dir = g_arg_info_get_direction(arg_info);

				type_info = g_arg_info_get_type(arg_info);

		
				if (!seed_gi_supports_type(type_info))
						g_base_info_unref((GIBaseInfo *)type_info);
		
		}
	
		method_ref = JSObjectMake(eng->context, gobject_method_class, info);

		name = g_base_info_get_name((GIBaseInfo *) info);
		if (!strcmp(name, "new"))
			name = "_new";
		seed_value_set_property(object, 
								name,
								method_ref);

}


static void seed_gobject_add_methods_for_interfaces(GIObjectInfo * oinfo, JSObjectRef object)
{
		GIInterfaceInfo * interface;
		gint n_interfaces, i;
	
		n_interfaces = g_object_info_get_n_interfaces(oinfo);
	
		for (i = 0; i < n_interfaces; i++)
		{
				GIFunctionInfo * function;
				gint n_functions, k;
				interface = g_object_info_get_interface(oinfo, i);
		
				n_functions = g_interface_info_get_n_methods(interface);
				for (k = 0; k < n_functions; k++)
				{
						function = g_interface_info_get_method(interface, k);
						seed_gobject_define_property_from_function_info
							(function, object, TRUE);
				}
				//	g_base_info_unref((GIBaseInfo*)interface);
		}
}

static void seed_gobject_add_methods_for_type(GIObjectInfo * oinfo, JSObjectRef object)
{
		int n_methods;
		int i;
		GIFunctionInfo * info;

		n_methods = g_object_info_get_n_methods(oinfo);

		for (i = 0; i < n_methods; i++)
		{
				info = g_object_info_get_method(oinfo, i);
				seed_gobject_define_property_from_function_info(info, object, TRUE);
		}
}

JSClassRef
seed_gobject_get_class_for_gtype(GType type)
{
		JSClassDefinition def;
		GType parent;
		JSClassRef ref;
		JSClassRef parent_class = 0; 
		GIBaseInfo * info;
		JSObjectRef prototype_obj;
		JSObjectRef parent_prototype;
	
		if ((ref = g_type_get_qdata (type, qname)) != NULL)
		{
				return ref;
		}

		info = g_irepository_find_by_gtype(g_irepository_get_default(),
										   type);


		
		if (!info || !(g_base_info_get_type(info) == GI_INFO_TYPE_OBJECT))
				return 0;

		memset(&def, 0, sizeof(JSClassDefinition));

	
		def.className = g_type_name(type);
		if ((parent = g_type_parent(type)))
				parent_class = seed_gobject_get_class_for_gtype(parent);
		def.parentClass = parent_class;
		def.attributes = kJSClassAttributeNoAutomaticPrototype;

		prototype_obj = JSObjectMake(eng->context, 0, 0);
		if (parent)
		{
				parent_prototype = seed_gobject_get_prototype_for_gtype(parent);
				if (parent_prototype)
						JSObjectSetPrototype(eng->context, 
											 prototype_obj, parent_prototype);
		}
		seed_gobject_add_methods_for_type((GIObjectInfo *)info, prototype_obj);
		seed_gobject_add_methods_for_interfaces((GIObjectInfo *) info, 
												prototype_obj);
	
		ref = JSClassCreate(&def);
		JSClassRetain(ref);
	
		JSValueProtect(eng->context, prototype_obj);
	
		g_type_set_qdata(type, qname, ref);
		g_type_set_qdata(type, qprototype, prototype_obj);



		return ref;
}

JSObjectRef seed_gobject_get_prototype_for_gtype(GType type)
{

		return g_type_get_qdata(type, qprototype);
}

static void seed_gobject_finalize(JSObjectRef object)
{
		GObject * gobject;
	
		gobject = seed_value_to_object((JSValueRef)object);
		if (!gobject)
				return;

		g_object_unref(gobject);
}

static void seed_gobject_initialize(JSContextRef ctx,				    
									JSObjectRef object)
{
		GObject * gobject;
		GIBaseInfo * base;
	
		gobject = seed_value_to_object((JSValueRef)object);
		if (!gobject)
				return;

		base = g_irepository_find_by_gtype(g_irepository_get_default(),
										   G_OBJECT_TYPE(gobject));

		seed_add_signals_to_object(object, gobject);
		if (!base)
		{
				return;
		}
		g_assert(g_base_info_get_type(base) == GI_INFO_TYPE_OBJECT);
	
}

static JSValueRef seed_gobject_get_property(JSContextRef context, 
											JSObjectRef object, 
											JSStringRef property_name,
											JSValueRef * exception)
{
		GParamSpec * spec;
		GObject * b;
		GValue gval = {0};
		char * cproperty_name;
		int length;
		SeedValue ret;
		int i, len;

		b = seed_value_to_object((JSValueRef)object);
		if (!b)
			return 0;

		length = JSStringGetMaximumUTF8CStringSize(property_name);
		cproperty_name = malloc(length * sizeof(gchar));
		JSStringGetUTF8CString(property_name, cproperty_name, length); 
	
	
		spec = g_object_class_find_property(G_OBJECT_GET_CLASS(b),
											cproperty_name);

		if (!spec)
		{
				len = strlen(cproperty_name);
				for (i = 0; i < len-1; i++)
				{
						if (cproperty_name[i] == '_')
								cproperty_name[i]='-';
				} 
				spec = g_object_class_find_property(G_OBJECT_GET_CLASS(b),
													cproperty_name);
				g_free(cproperty_name);
				return 0;
		}

	
	
		g_value_init(&gval, spec->value_type);
		g_object_get_property(b, cproperty_name, &gval);
		ret = seed_value_from_gvalue(&gval);
		g_value_unset(&gval);

		g_free(cproperty_name);
		return (JSValueRef) ret;
}

static bool seed_gobject_set_property(JSContextRef context,
									  JSObjectRef object,
									  JSStringRef property_name,
									  JSValueRef value,
									  JSValueRef * exception)
{
		GParamSpec * spec = 0;
		GObject * obj;
		GValue gval = {0};
		GType type;
		gchar * cproperty_name;
		int length;

		if (JSValueIsNull(eng->context, value))
				return 0;

		obj = seed_value_to_object(object);
		if (!obj || !G_IS_OBJECT(obj))
				return FALSE;
	
	
		length = JSStringGetMaximumUTF8CStringSize(property_name);
		cproperty_name = malloc(length * sizeof(gchar));
		JSStringGetUTF8CString(property_name, cproperty_name, length); 

		spec = g_object_class_find_property(G_OBJECT_GET_CLASS(obj),
											cproperty_name);

		if (!spec)
		{
				int i, len;
				len = strlen(cproperty_name);
				for (i = 0; i < len; i++)
				{
						if (cproperty_name[i] == '_')
								cproperty_name[i]='-';
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
			type = G_TYPE_INT;
		else
			type = spec->value_type;

		if (!seed_gvalue_from_seed_value(value, type, &gval))
		{
				gchar * mes = g_strdup_printf("Not able to set property %s"
											  "on object of type %s."
											  " Expected type: %s. \n",
											  cproperty_name, 
											  g_type_name(G_OBJECT_TYPE(obj)),
											  g_type_name(spec->value_type));
				seed_make_exception(exception, "InvalidPropertyValue", mes);
				g_free(mes);
				g_free(cproperty_name);
				return 0;
		}

		g_object_set_property(obj, cproperty_name, &gval);
		g_free(cproperty_name);
	
		return TRUE;
}

static JSValueRef
seed_gi_import_namespace(JSContextRef ctx,
						 JSObjectRef function,
						 JSObjectRef this_object,
						 size_t argumentCount,
						 const JSValueRef arguments[],
						 JSValueRef * exception)
{
		GIBaseInfo * info;
		const gchar * namespace;
		const gchar * extension;
		const gchar * version = 0;
		JSObjectRef namespace_ref;
		JSStringRef extension_script;
		int n,i;
	
		namespace = seed_value_to_string(arguments[0]);
		if (argumentCount == 2)
		{
				version = seed_value_to_string(arguments[1]);
		}
	
		if(!g_irepository_require (g_irepository_get_default (), namespace,
								   version, 0, NULL))
				g_error("No namespace: %s \n", namespace);

		n = g_irepository_get_n_infos(g_irepository_get_default(),
									  namespace);
	
		namespace_ref = JSObjectMake(eng->context,
									 NULL, NULL);
		JSValueProtect(eng->context, namespace_ref);
		seed_value_set_property(eng->global, namespace, namespace_ref);

		for (i = 0; i < n; i++)
		{
				info = g_irepository_get_info(g_irepository_get_default(),
											  namespace, i);
				if (info && 
					(g_base_info_get_type(info) == GI_INFO_TYPE_FUNCTION))
				{
						seed_gobject_define_property_from_function_info(
							(GIFunctionInfo *) info, namespace_ref, FALSE);
				}
				else if (info && 
						 (g_base_info_get_type(info) == GI_INFO_TYPE_ENUM))
				{
						int num_vals = 
								g_enum_info_get_n_values((GIEnumInfo *) info);
						int j;
						JSObjectRef enum_class = JSObjectMake(eng->context,
															  0,0);
						JSValueProtect(eng->context, (JSValueRef) enum_class);
						seed_value_set_property(namespace_ref, 
												g_base_info_get_name(info),
												enum_class);


						for (j = 0; j < num_vals; j++)
						{
								GIValueInfo * val = 
										g_enum_info_get_value((GIEnumInfo *)
															  info, j);
								gint value = g_value_info_get_value(val);
								gchar * name = g_strdup(
										g_base_info_get_name(
												(GIBaseInfo *) val));
								int name_len = strlen(name);
								int j;
								JSValueRef value_ref;

								value_ref = JSValueMakeNumber
										(eng->context, value);
								JSValueProtect
										(eng->context, (JSValueRef)value_ref);

								for (j = 0; j < name_len; j++)
								{
									if (name[j]=='-')
										name[j]='_';
								}
								
								seed_value_set_property(
										enum_class,
										name,
										value_ref);

			


								g_free(name);
								
						}
				}
				else if (info && 
						 (g_base_info_get_type(info) == GI_INFO_TYPE_OBJECT))
				{
						GType type;
						JSClassRef class_ref;
			
						type = g_registered_type_info_get_g_type(
								(GIRegisteredTypeInfo *) info);

						if (type != 0)
						{
								JSObjectRef constructor_ref;
								int i, n_methods;
								GIFunctionInfo * finfo;
								GIFunctionInfoFlags flags;

								class_ref = 
										seed_gobject_get_class_for_gtype(type);
								
								constructor_ref = 
										JSObjectMake(eng->context, 
													 gobject_constructor_class, 
													 (gpointer)type);


								n_methods = 
									g_object_info_get_n_methods((GIObjectInfo *)info);
								for (i = 0; i < n_methods; i++)
								{
										finfo = 
											g_object_info_get_method(
													 (GIObjectInfo *)info, i);
										flags = g_function_info_get_flags(finfo);
										if (flags & GI_FUNCTION_IS_CONSTRUCTOR)
										{
												seed_gobject_define_property_from_function_info(
													finfo,
													constructor_ref,
													FALSE);
										}
										else
										{
												g_base_info_unref((GIBaseInfo*)finfo);
										}
								}
				


								seed_value_set_property(namespace_ref,
														g_base_info_get_name(
																info), 
														constructor_ref);
								JSValueProtect(eng->context, 
											   (JSValueRef)constructor_ref);
						}

				}
		}
		

		extension = g_strdup_printf("Seed.include(\"/usr/local/share/seed/%s.js\")", 
									namespace);
		extension_script = JSStringCreateWithUTF8CString(extension);
		JSEvaluateScript(eng->context, extension_script, NULL, NULL, 0, NULL);
		JSStringRelease(extension_script);

		g_free((gchar *)namespace);

		return JSValueMakeNull(eng->context);
}

JSStaticFunction gobject_static_funcs[] = {
	{"equals", seed_gobject_equals, 0},
	{0, 0, 0}
};

JSClassDefinition gobject_def = {
		0, /* Version, always 0 */
		kJSClassAttributeNoAutomaticPrototype, /* JSClassAttributes */
		"gobject", /* Class Name */
		NULL, /* Parent Class */
		NULL, /* Static Values */
	    gobject_static_funcs, /* Static Functions */
		seed_gobject_initialize,  /* Initialize */
		seed_gobject_finalize, /* Finalize */
		NULL, /* Has Property */
		seed_gobject_get_property, /* Get Property */
		seed_gobject_set_property,  /* Set Property */
		NULL, /* Delete Property */
		NULL, /* Get Property Names */
		NULL, /* Call As Function */
		NULL, /* Call As Constructor */
		NULL, /* Has Instance */
		NULL  /* Convert To Type */
};

JSClassDefinition gobject_method_def = {
		0, /* Version, always 0 */
		0,
		"gobject_method", /* Class Name */
		NULL, /* Parent Class */
		NULL, /* Static Values */
		NULL, /* Static Functions */
		NULL,
		NULL, /* Finalize */
		NULL, /* Has Property */
		NULL, /* Get Property */
		NULL,  /* Set Property */
		NULL, /* Delete Property */
		NULL, /* Get Property Names */
		seed_gobject_method_invoked, /* Call As Function */
		NULL, /* Call As Constructor */
		NULL, /* Has Instance */
		NULL  /* Convert To Type */
};
 
JSClassDefinition gobject_constructor_def = {
		0, /* Version, always 0 */
		0,
		"gobject_constructor", /* Class Name */
		NULL, /* Parent Class */
		NULL, /* Static Values */
		NULL, /* Static Functions */
		NULL,
		NULL, /* Finalize */
		NULL, /* Has Property */
		NULL, /* Get Property */
		NULL,  /* Set Property */
		NULL, /* Delete Property */
		NULL, /* Get Property Names */
		NULL, /* Call As Function */
		seed_gobject_constructor_invoked, /* Call As Constructor */
		NULL, /* Has Instance */
		NULL  /* Convert To Type */
};

void seed_create_function(char * name, gpointer func, JSObjectRef obj)
{
		JSObjectRef oref;
	
		oref = JSObjectMakeFunctionWithCallback(eng->context, NULL, func);
		JSValueProtect(eng->context, oref);
		seed_value_set_property(obj, name, oref);
}

gboolean seed_init(int * argc, char *** argv)
{
	JSObjectRef seed_obj_ref;

	g_type_init ();

	qname = g_quark_from_static_string("js-type");
	qprototype = g_quark_from_static_string("js-prototype");
	
	eng = (SeedEngine *)malloc(sizeof(SeedEngine));
	
	eng->context = JSGlobalContextCreateInGroup(NULL,NULL);
	eng->global = JSContextGetGlobalObject(eng->context);
	gobject_class = JSClassCreate(&gobject_def);
	JSClassRetain(gobject_class);
	gobject_method_class = JSClassCreate(&gobject_method_def);
	JSClassRetain(gobject_method_class);
	gobject_constructor_class = JSClassCreate(&gobject_constructor_def);
	JSClassRetain(gobject_constructor_class);
	gobject_signal_class = JSClassCreate(seed_get_signal_class());
	JSClassRetain(gobject_signal_class);
	
	g_type_set_qdata(G_TYPE_OBJECT, qname, gobject_class);
	
	seed_obj_ref = JSObjectMake(eng->context, NULL, NULL);
	seed_value_set_property(eng->global, "Seed", seed_obj_ref);
	JSValueProtect(eng->context, seed_obj_ref);

	seed_create_function("import_namespace", &seed_gi_import_namespace, seed_obj_ref);
	seed_init_builtins(argc, argv);
	
	return TRUE;

}

SeedScript * seed_make_script(const gchar * js, const gchar * source_url,
							  int line_number)
{
		SeedScript * ret = g_new0(SeedScript, 1);

		ret->script = JSStringCreateWithUTF8CString(js);
		JSCheckScriptSyntax(eng->context, ret->script,
							0, 0, &ret->exception);
	
		if (source_url)
		{
				ret->source_url = JSStringCreateWithUTF8CString(source_url);
		}
		ret->line_number = line_number;

		return ret;
}

JSValueRef seed_evaluate(SeedScript * js, JSObjectRef this)
{
		JSValueRef ret;

		js->exception = 0;
		ret = JSEvaluateScript(eng->context, 
							   js->script, this, js->source_url,
							   js->line_number, &js->exception);

		return ret;	
}

SeedValue seed_script_exception(SeedScript *s)
{
		return s->exception;
}

gchar * seed_exception_get_name(JSValueRef e)
{
		SeedValue name;
		g_assert((e));
		if (!JSValueIsObject(eng->context, e))
				return 0;

		name = seed_value_get_property(e, "name");
		return seed_value_to_string(name);
}

gchar * seed_exception_get_message(JSValueRef e)
{
		SeedValue name;
		g_assert((e));
		if (!JSValueIsObject(eng->context, e))
				return 0;

		name = seed_value_get_property(e, "message");
		return seed_value_to_string(name);
}

guint seed_exception_get_line(JSValueRef e)
{
	SeedValue line;
	g_assert((e));
	if (!JSValueIsObject(eng->context, e))
			return 0;
	line = seed_value_get_property(e, "line");
	return seed_value_to_uint(line);
}

gchar * seed_exception_get_file(JSValueRef e)
{
	SeedValue file;
	g_assert((e));
	if (!JSValueIsObject(eng->context, e))
			return 0;
	file = seed_value_get_property(e, "sourceURL");
	return seed_value_to_string(file);
}

