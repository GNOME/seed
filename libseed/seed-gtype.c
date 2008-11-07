/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-gtype.c
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

JSClassRef seed_gtype_class;

static JSObjectRef seed_gtype_constructor_invoked(JSContextRef ctx,
												  JSObjectRef constructor,
												  size_t argumentCount,
												  const JSValueRef arguments[],
												  JSValueRef * exception)
{
		JSValueRef class_init, instance_init, name, parent_ref;
		GType parent_type, new_type;
		gchar * new_name;
		GTypeInfo type_info = {
				0,
				(GBaseInitFunc) NULL,
				(GBaseFinalizeFunc) NULL,
				(GClassInitFunc) NULL,
				(GClassFinalizeFunc) NULL,
				NULL,
				0,
				0,
				NULL
		};
				
		GTypeQuery query;

		if (argumentCount != 1)
		{
				gchar * mes = g_strdup_printf("GType constructor expected 1 "
											  "argument, got %d \n", 
											  argumentCount);
				seed_make_exception(exception, "ArgumentError", mes);
				g_free(mes);
				return (JSObjectRef)JSValueMakeNull(eng->context);
 		}
		if (!JSValueIsObject(eng->context, arguments[0]))
		{
				seed_make_exception(exception, "ArgumentError",
									"GType constructor expected a"
									"class definition object. Got a nonobject");
		}
		parent_ref = seed_value_get_property(arguments[0], "parent");
		class_init = seed_value_get_property(arguments[0], "class_init");
		instance_init = seed_value_get_property(arguments[0], "instance_init");
		name = seed_value_get_property(arguments[0], "name");
		
		new_name = seed_value_to_string(name, exception);
		if (!JSValueIsObjectOfClass(eng->context, 
									parent_ref,
									gobject_constructor_class))
		{
				seed_make_exception(exception, "TypeError",
									"GType constructor expected"
									" GObject type for parent");
		}
		parent_type = (GType)JSObjectGetPrivate((JSObjectRef)parent_ref);
		
		
		g_type_query(parent_type, &query);
		type_info.class_size = query.class_size;
		type_info.instance_size = query.instance_size;
		
		new_type = g_type_register_static(parent_type,
										  new_name,
										  &type_info, 0);
		seed_gobject_get_class_for_gtype(new_type);
		printf("Type_name: %s \n", g_type_name(new_type));
		
		
		g_free(new_name);
		return JSObjectMake(eng->context, gobject_constructor_class, 
							(gpointer)new_type);
}

void seed_gtype_init(void)
{
		JSClassDefinition def = kJSClassDefinitionEmpty;
		JSObjectRef gtype_constructor;

		def.callAsConstructor = seed_gtype_constructor_invoked;
		seed_gtype_class = JSClassCreate(&def);
		JSClassRetain(seed_gtype_class);
		
		gtype_constructor = JSObjectMake(eng->context, seed_gtype_class, 0);
		
		seed_value_set_property(eng->global, "GType", gtype_constructor);
		
}



