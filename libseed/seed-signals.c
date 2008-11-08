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
} signal_privates;

JSClassRef signal_holder_class;

static void
seed_add_signal_to_object(JSObjectRef object_ref,
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

    signal_ref = JSObjectMake(eng->context, gobject_signal_class, priv);

    priv->signal_id = signal->signal_id;
    priv->object = obj;

    seed_value_set_property(object_ref, js_signal_name, signal_ref);
    g_free(js_signal_name);
}

static void
seed_add_signals_for_type(JSObjectRef object_ref, GObject * obj, GType type)
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
	    seed_add_signal_to_object(object_ref, obj, &query);
	}
    }
    g_free(signal_ids);
}

void seed_add_signals_to_object(JSObjectRef object_ref, GObject * obj)
{
    GType type;
    GType *interfaces;
    guint n, i;
    JSObjectRef signals_ref;

    g_assert(obj);

    type = G_OBJECT_TYPE(obj);

    signals_ref = JSObjectMake(eng->context, signal_holder_class, 0);

    while (type != 0)
    {
	seed_add_signals_for_type(signals_ref, obj, type);

	interfaces = g_type_interfaces(type, &n);
	for (i = 0; i < n; i++)
	    seed_add_signals_for_type(signals_ref, obj, interfaces[i]);

	type = g_type_parent(type);

	g_free(interfaces);
    }

    seed_value_set_property(object_ref, "signal", signals_ref);
}

void
seed_signal_marshal_func(GClosure * closure,
			 GValue * return_value,
			 guint n_param_values,
			 const GValue * param_values,
			 gpointer invocation_hint, gpointer marshall_data)
{
    SeedClosure *seed_closure = (SeedClosure *) closure;
    JSValueRef *args;
    gint i;

    args = g_newa(JSValueRef, n_param_values);

    for (i = 0; i < n_param_values; i++)
    {
	args[i] = seed_value_from_gvalue((GValue *) & param_values[i], 0);

	if (!args[i])
	    g_error("Error in signal marshal. "
		    "Unable to convert argument of type: %s \n",
		    g_type_name(param_values[i].g_type));

    }

    JSObjectCallAsFunction(eng->context, seed_closure->function,
			   seed_closure->this, n_param_values, args, 0);
}

static JSValueRef
seed_gobject_signal_connect(JSContextRef ctx,
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

    g_assert((argumentCount <= 2));

    closure = g_closure_new_simple(sizeof(SeedClosure), 0);
    g_closure_set_marshal(closure, seed_signal_marshal_func);
    // Losing a ref here. Fix please.
    // g_closure_add_finalize_notifier(closure, NULL, NULL);
    ((SeedClosure *) closure)->function = (JSObjectRef) arguments[0];
    ((SeedClosure *) closure)->object =
	g_object_get_data(privates->object, "js-ref");
    if (argumentCount == 2 && !JSValueIsNull(eng->context, arguments[1]))
    {
	JSValueProtect(eng->context, (JSObjectRef) arguments[1]);
	((SeedClosure *) closure)->this = (JSObjectRef) arguments[1];
    }
    else
	((SeedClosure *) closure)->this = 0;

    JSValueProtect(eng->context, (JSObjectRef) arguments[0]);

    g_signal_connect_closure_by_id(privates->object,
				   privates->signal_id, 0, closure, FALSE);
    return 0;
}

JSStaticFunction signal_static_functions[] =
    { {"connect", seed_gobject_signal_connect, 0}
,
{0, 0, 0}
};

JSClassDefinition gobject_signal_def = {
    0,				/* Version, always 0 */
    kJSClassAttributeNoAutomaticPrototype,
    "gobject_signal",		/* Class Name */
    NULL,			/* Parent Class */
    NULL,			/* Static Values */
    signal_static_functions,	/* Static Functions */
    NULL,
    NULL,			/* Finalize */
    NULL,			/* Has Property */
    NULL,			/* Get Property */
    NULL,			/* Set Property */
    NULL,			/* Delete Property */
    NULL,			/* Get Property Names */
    NULL,			/* Call As Function */
    NULL,			/* Call As Constructor */
    NULL,			/* Has Instance */
    NULL			/* Convert To Type */
};

JSClassDefinition *seed_get_signal_class(void)
{
    JSClassDefinition signal_holder = kJSClassDefinitionEmpty;

    signal_holder.className = "gobject_signals";
    signal_holder_class = JSClassCreate(&signal_holder);
    JSClassRetain(signal_holder_class);

    return &gobject_signal_def;
}
