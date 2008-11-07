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
#include <sys/mman.h>

JSClassRef seed_gtype_class;

static void seed_handle_class_init_closure(ffi_cif * cif,
										   void * result,
										   void ** args,
										   void * userdata)
{
		JSObjectRef function = (JSObjectRef) userdata;
		JSValueRef jsargs[2];
		
		jsargs[0] = seed_make_struct(*(gpointer*)args[0], 0);
		jsargs[1] = seed_value_from_int(*(gint*)args[1],0);
		
		JSObjectCallAsFunction(eng->context,
							   function,
							   0, 2, jsargs, 0);
}

static void seed_handle_instance_init_closure(ffi_cif * cif,
											  void * result,
											  void ** args,
											  void * userdata)
{
		JSObjectRef function = (JSObjectRef) userdata;
		JSValueRef jsargs[2];
		
		jsargs[0] = seed_value_from_object(*(GObject**)args[0], 0);
		jsargs[1] = seed_make_struct(*(gpointer*)args[1],0);
		
		JSObjectCallAsFunction(eng->context,
							   function,
							   0, 2, jsargs, 0);
}

static ffi_closure * seed_make_class_init_closure(JSObjectRef function)
{
		ffi_cif * cif;
		ffi_closure * closure;
		ffi_type ** arg_types;;
		ffi_arg result;
		ffi_status status;
		
		JSValueProtect(eng->context, function);
		
		cif = g_new0(ffi_cif, 1);
		arg_types = g_new0(ffi_type *, 3);
		
		arg_types[0] = &ffi_type_pointer;
		arg_types[1] = &ffi_type_uint;
		arg_types[2] = 0;
		
		closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE |
					   PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
		
		ffi_prep_cif(cif, FFI_DEFAULT_ABI, 2,
					 &ffi_type_void, arg_types);
		ffi_prep_closure(closure, cif, seed_handle_class_init_closure, 
						 function);
		return closure;
}

static ffi_closure * seed_make_instance_init_closure(JSObjectRef function)
{
		ffi_cif * cif;
		ffi_closure * closure;
		ffi_type ** arg_types;;
		ffi_arg result;
		ffi_status status;
		
		JSValueProtect(eng->context, function);
		
		cif = g_new0(ffi_cif, 1);
		arg_types = g_new0(ffi_type *, 3);
		
		arg_types[0] = &ffi_type_pointer;
		arg_types[1] = &ffi_type_pointer;
		arg_types[2] = 0;
		
		closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE |
					   PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
		
		ffi_prep_cif(cif, FFI_DEFAULT_ABI, 2,
					 &ffi_type_void, arg_types);
		ffi_prep_closure(closure, cif, seed_handle_instance_init_closure, 
						 function);
		return closure;
}

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
		ffi_closure * init_closure = 0;
		ffi_closure * instance_init_closure = 0;
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
		if (!JSValueIsNull(eng->context, class_init) &&
			JSObjectIsFunction(eng->context, (JSObjectRef)class_init))
		{
				init_closure = seed_make_class_init_closure((JSObjectRef)class_init);
		}
		if (!JSValueIsNull(eng->context, instance_init) &&
			JSObjectIsFunction(eng->context, (JSObjectRef)instance_init))
		{
				instance_init_closure = 
						seed_make_instance_init_closure((JSObjectRef)instance_init);
		}
		parent_type = (GType)JSObjectGetPrivate((JSObjectRef)parent_ref);
		
		
		g_type_query(parent_type, &query);
		type_info.class_size = query.class_size;
		type_info.instance_size = query.instance_size;
		type_info.class_init = (GClassInitFunc)init_closure;
		type_info.instance_init = (GInstanceInitFunc)instance_init_closure;
		
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



