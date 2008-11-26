/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * seed-signals.c
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

#include <string.h>

#include "seed-private.h"

typedef struct _signal_privates {
	guint signal_id;
	GObject *object;
	const gchar * signal_name;
} signal_privates;

JSClassRef signal_holder_class;

static void seed_signal_finalize(JSObjectRef object)
{
	signal_privates *sig_priv = JSObjectGetPrivate(object);

	g_free(sig_priv);
}

static void
seed_add_signal_to_object(JSContextRef ctx, JSObjectRef object_ref,
						  GObject * obj, GSignalQuery * signal)
{
	guint k;
	JSObjectRef signal_ref;
	signal_privates *priv = g_malloc(sizeof(signal_privates));
	gchar *js_signal_name = g_strdup(signal->signal_name);
	g_assert(signal);

	for (k = 0; k < strlen(js_signal_name); k++)
	{
		if (js_signal_name[k] == '-')
			js_signal_name[k] = '_';
	}

	signal_ref = JSObjectMake(ctx, gobject_signal_class, priv);

	priv->signal_id = signal->signal_id;
	priv->object = obj;
	priv->signal_name = signal->signal_name;

	seed_object_set_property(ctx, object_ref, js_signal_name, signal_ref);
	g_free(js_signal_name);
}

static void
seed_add_signals_for_type(JSContextRef ctx, 
						  JSObjectRef object_ref, 
						  GObject * obj, GType type)
{
	guint n, i;
	guint *signal_ids;
	GSignalQuery query;
	signal_ids = g_signal_list_ids(type, &n);
	for (i = 0; i < n; i++)
	{
		g_signal_query(signal_ids[i], &query);
		if (query.signal_id != 0)
		{
			seed_add_signal_to_object(ctx, object_ref, obj, &query);
		}
	}
	g_free(signal_ids);
}

static void seed_gobject_signal_connect(JSContextRef ctx,
										const gchar * signal_name,
										GObject * on_obj,
										JSObjectRef func,
										JSObjectRef this_obj,
										JSObjectRef user_data)
{
	GSignalQuery query;
	GClosure * closure;
	
	g_signal_query(g_signal_lookup(signal_name, G_OBJECT_TYPE(on_obj)), &query);

	closure = g_closure_new_simple(sizeof(SeedClosure), 0);
	g_closure_set_marshal(closure, seed_signal_marshal_func);

	((SeedClosure *) closure)->function = func;
	//((SeedClosure *) closure)->object = on_obj;
	((SeedClosure *) closure)->return_type = query.return_type;
	
	if (this_obj && !JSValueIsNull(ctx, this_obj))
	{
		JSValueProtect(ctx, this_obj);
		((SeedClosure *) closure)->this = this_obj;
	}
	else
	{
		((SeedClosure *) closure)->this = 0;
	}
	
	if (user_data && !JSValueIsNull(ctx, user_data))
	{
		((SeedClosure *) closure)->user_data = user_data;
		JSValueProtect(ctx, user_data);
	}

	JSValueProtect(ctx, (JSObjectRef) func);
	
	g_signal_connect_closure(on_obj, signal_name, closure, FALSE);
}

static JSValueRef
seed_gobject_signal_connect_by_name(JSContextRef ctx,
									JSObjectRef function,
									JSObjectRef thisObject,
									size_t argumentCount,
									const JSValueRef arguments[],
									JSValueRef * exception)
{
	GType obj_type;
	JSObjectRef user_data = NULL;
	gchar * signal_name;
	GObject * obj;
	
	if(argumentCount < 2 || argumentCount > 3)
	{
		gchar *mes = g_strdup_printf("Signal connection expected"
									 " 2 or 3 arguments. Got "
									 "%d", argumentCount);
		seed_make_exception(ctx, exception, "ArgumentError", mes);

		g_free(mes);
		return JSValueMakeNull(ctx);
	}
	
	if(argumentCount == 3)
	{
		user_data = (JSObjectRef)arguments[2];
	}
	
	signal_name = seed_value_to_string(ctx, arguments[0], NULL);
	obj = (GObject *)JSObjectGetPrivate(thisObject);
	obj_type = G_OBJECT_TYPE(obj);
	
	seed_gobject_signal_connect(ctx, signal_name, obj, 
								(JSObjectRef) arguments[1],
								NULL, user_data);
	
	g_free(signal_name);
}

void seed_add_signals_to_object(JSContextRef ctx, 
								JSObjectRef object_ref, 
								GObject * obj)
{
	GType type;
	GType *interfaces;
	guint n, i;
	JSObjectRef signals_ref, connect_func;

	g_assert(obj);

	type = G_OBJECT_TYPE(obj);

	signals_ref = JSObjectMake(ctx, signal_holder_class, obj);

	while (type != 0)
	{
		seed_add_signals_for_type(ctx, signals_ref, obj, type);

		interfaces = g_type_interfaces(type, &n);
		for (i = 0; i < n; i++)
			seed_add_signals_for_type(ctx, signals_ref, obj, interfaces[i]);

		type = g_type_parent(type);

		g_free(interfaces);
	}

	seed_object_set_property(ctx, object_ref, "signal", signals_ref);

	connect_func =
		JSObjectMakeFunctionWithCallback(ctx, NULL,
										 &seed_gobject_signal_connect_by_name);
	JSValueProtect(ctx, connect_func);	

	seed_object_set_property(ctx, signals_ref, "connect", connect_func);
}

void
seed_signal_marshal_func(GClosure * closure,
						 GValue * return_value,
						 guint n_param_values,
						 const GValue * param_values,
						 gpointer invocation_hint, gpointer marshall_data)
{
	SeedClosure *seed_closure = (SeedClosure *) closure;
	JSValueRef *args, exception = 0;
	JSValueRef ret = 0;
	gint i;
	
	JSContextRef ctx = JSGlobalContextCreateInGroup(context_group,
													0);

	args = g_newa(JSValueRef, n_param_values + 1);

	for (i = 0; i < n_param_values; i++)
	{
		args[i] = seed_value_from_gvalue(ctx,
										 (GValue *) & param_values[i],
										 0);

		if (!args[i])
			g_error("Error in signal marshal. "
					"Unable to convert argument of type: %s \n",
					g_type_name(param_values[i].g_type));

	}

	if (seed_closure->user_data)
		args[i] = seed_closure->user_data;
	else
		args[i] = JSValueMakeNull(ctx);

	ret = JSObjectCallAsFunction(ctx, seed_closure->function,
								 seed_closure->this,
								 n_param_values + 1, args, &exception);

	if (exception)
	{
		gchar *mes = seed_exception_to_string(ctx, 
											  exception);
		g_warning("Exception in signal handler. %s \n", mes, 0);
		g_free(mes);
		exception = 0;
	}

	if (ret && !JSValueIsNull(ctx, ret)
		&& (seed_closure->return_type != G_TYPE_NONE))
	{
		seed_gvalue_from_seed_value(ctx, ret, seed_closure->return_type,
									return_value, &exception);
	}

	if (exception)
	{
		gchar *mes = seed_exception_to_string(ctx, exception);
		g_warning("Exception in signal handler return value. %s \n", mes, 0);
		g_free(mes);
	}
	
	JSGlobalContextRelease((JSGlobalContextRef)ctx);

}

static JSValueRef
seed_gobject_signal_emit(JSContextRef ctx,
						 JSObjectRef function,
						 JSObjectRef thisObject,
						 size_t argumentCount,
						 const JSValueRef arguments[], JSValueRef * exception)
{
	JSValueRef ret;
	GValue *params;
	GValue ret_value = { 0 };
	GSignalQuery query;

	signal_privates *privates;
	guint i;

	privates = JSObjectGetPrivate(thisObject);

	g_signal_query(privates->signal_id, &query);

	if (argumentCount != query.n_params)
	{
		gchar *mes = g_strdup_printf("Signal: %s for type %s expected %d "
									 "arguments, got %d",
									 query.signal_name,
									 g_type_name(query.itype),
									 query.n_params,
									 argumentCount);
		seed_make_exception(ctx, exception, "ArgumentError", mes);
		g_free(mes);
		return JSValueMakeNull(ctx);
	}

	params = g_new0(GValue, argumentCount + 1);

	g_value_init(&params[0], G_TYPE_OBJECT);
	g_value_set_object(&params[0], privates->object);
	for (i = 0; i < argumentCount; i++)
		seed_gvalue_from_seed_value(ctx, arguments[i],
									query.param_types[i],
									&params[i + 1], exception);

	g_signal_emitv(params, privates->signal_id, 0, &ret_value);

	for (i = 0; i < argumentCount; i++)
		g_value_unset(&params[i]);
	g_free(params);

	ret = seed_value_from_gvalue(ctx, &ret_value, exception);

	return ret;
}

static JSValueRef
seed_gobject_signal_connect_on_property(JSContextRef ctx,
										JSObjectRef function,
										JSObjectRef thisObject,
										size_t argumentCount,
										const JSValueRef arguments[],
										JSValueRef * exception)
{
	signal_privates *privates;
	GClosure *closure;
	
	privates = (signal_privates *) JSObjectGetPrivate(thisObject);
	if (!privates)
		g_error("Signal constructed with invalid parameters"
				"in namespace import \n");

	if ((argumentCount > 3) || (argumentCount == 0))
	{
		gchar *mes = g_strdup_printf("Signal connection expected"
									 " 1, 2, or 3 arguments. Got "
									 "%d", argumentCount);
		seed_make_exception(ctx, exception, "ArgumentError", mes);

		g_free(mes);
		return JSValueMakeNull(ctx);
	}
	
	if (argumentCount == 1)
		seed_gobject_signal_connect(ctx, privates->signal_name,
									privates->object,
									(JSObjectRef) arguments[0], NULL, NULL);
	
	if (argumentCount == 2)
		seed_gobject_signal_connect(ctx, privates->signal_name,
									privates->object,
									(JSObjectRef) arguments[0],
									(JSObjectRef) arguments[1], NULL);

	if (argumentCount == 3)
		seed_gobject_signal_connect(ctx, privates->signal_name,
									privates->object,
									(JSObjectRef) arguments[0],
									(JSObjectRef) arguments[1],
									(JSObjectRef) arguments[2]);
	
	return 0;
}

JSStaticFunction signal_static_functions[] =
	{ {"connect", seed_gobject_signal_connect_on_property, 0},
	  {"emit", seed_gobject_signal_emit, 0},
	  {0, 0, 0} };

JSClassDefinition gobject_signal_def = {
	0,							/* Version, always 0 */
	kJSClassAttributeNoAutomaticPrototype,
	"gobject_signal",			/* Class Name */
	NULL,						/* Parent Class */
	NULL,						/* Static Values */
	signal_static_functions,	/* Static Functions */
	NULL,
	seed_signal_finalize,		/* Finalize */
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

JSClassDefinition *seed_get_signal_class(void)
{
	JSClassDefinition signal_holder = kJSClassDefinitionEmpty;

	signal_holder.className = "gobject_signals";
	signal_holder_class = JSClassCreate(&signal_holder);
	JSClassRetain(signal_holder_class);

	return &gobject_signal_def;
}
