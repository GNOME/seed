/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-builtins.c
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
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <sys/mman.h>

JSValueRef
seed_include(JSContextRef ctx,
	     JSObjectRef function,
	     JSObjectRef this_object,
	     size_t argumentCount,
	     const JSValueRef arguments[], JSValueRef * exception)
{
	JSStringRef file_contents, file_name;
	const gchar *import_file;
	gchar *buffer, *walk;

	if (argumentCount != 1) {
		gchar *mes =
		    g_strdup_printf("Seed.include expected 1 argument, "
				    "got %d", argumentCount);
		seed_make_exception(exception, "ArgumentError", mes);
		return JSValueMakeNull(eng->context);
	}
	import_file = seed_value_to_string(arguments[0], exception);

	g_file_get_contents(import_file, &buffer, 0, 0);

	if (!buffer) {
		//gchar * mes = g_strdup_printf("File not found: %s.\n", import_file);
		//seed_make_exception(exception, "FileNotFound", mes);
		//g_free(mes);
		return 0;	// TODO: raise exception?
	}

	walk = buffer;

	if (*walk == '#') {
		while (*walk != '\n')
			walk++;
		walk++;
	}

	walk = strdup(walk);
	g_free(buffer);

	file_contents = JSStringCreateWithUTF8CString(walk);
	file_name = JSStringCreateWithUTF8CString(import_file);
	JSEvaluateScript(ctx, file_contents, NULL, file_name, 0, NULL);

	JSStringRelease(file_contents);
	JSStringRelease(file_name);

	return 0;
}

JSValueRef
seed_print(JSContextRef ctx,
	   JSObjectRef function,
	   JSObjectRef this_object,
	   size_t argumentCount,
	   const JSValueRef arguments[], JSValueRef * exception)
{
	if (argumentCount < 1) {
		gchar *mes = g_strdup_printf("Seed.print Expected 1 argument,"
					     " got %d", argumentCount);
		seed_make_exception(exception, "ArgumentError", mes);
		return JSValueMakeNull(eng->context);
	}

	gchar *buf = seed_value_to_string(arguments[0], exception);
	printf("%s\n", buf);
	free(buf);

	return JSValueMakeNull(eng->context);
}

JSValueRef
seed_readline(JSContextRef ctx,
	      JSObjectRef function,
	      JSObjectRef this_object,
	      size_t argumentCount,
	      const JSValueRef arguments[], JSValueRef * exception)
{
	// TODO: Should add an interface to rl_bind_key
	// Perhaps Seed.readline_bind('a', function)
	// Then automagically do function stuff and make it happen!

	JSValueRef valstr = 0;
	gchar *str = 0;
	gchar *buf;

	if (argumentCount != 1) {
		gchar *mes =
		    g_strdup_printf("Seed.readline Expected 1 argument, "
				    "got %d", argumentCount);
		seed_make_exception(exception, "ArgumentError", mes);
		return JSValueMakeNull(eng->context);
	}

	buf = seed_value_to_string(arguments[0], exception);

	str = readline(buf);
	if (str && *str) {
		add_history(str);
		valstr = seed_value_from_string(str, exception);
		free(str);
	}

	free(buf);

	if (valstr == 0)
		valstr = JSValueMakeNull(eng->context);

	return valstr;
}

JSValueRef
seed_prototype(JSContextRef ctx,
	       JSObjectRef function,
	       JSObjectRef this_object,
	       size_t argumentCount,
	       const JSValueRef arguments[], JSValueRef * exception)
{
	GType type;

	if (argumentCount != 1)
		return JSValueMakeNull(eng->context);
	if (!JSValueIsObject(eng->context, arguments[0]))
		return JSValueMakeNull(eng->context);

	type = (GType) JSObjectGetPrivate((JSObjectRef) arguments[0]);

	return seed_gobject_get_prototype_for_gtype(type);
}

const char *seed_g_type_name_to_string(GITypeInfo * type)
{
	GITypeTag type_tag = g_type_info_get_tag(type);

	const char *type_name;

	if (type_tag == GI_TYPE_TAG_INTERFACE) {
		GIBaseInfo *interface = g_type_info_get_interface(type);
		type_name = g_base_info_get_name(interface);
		g_base_info_unref(interface);
	} else {
		type_name = g_type_tag_to_string(type_tag);
	}

	return type_name;
}

JSValueRef
seed_introspect(JSContextRef ctx,
		JSObjectRef function,
		JSObjectRef this_object,
		size_t argumentCount,
		const JSValueRef arguments[], JSValueRef * exception)
{
	// TODO: LEAKY!

	GICallableInfo *info;
	JSObjectRef data_obj, args_obj;
	int i;

	if (argumentCount != 1)
		return JSValueMakeNull(eng->context);
	if (!JSValueIsObject(eng->context, arguments[0]))
		return JSValueMakeNull(eng->context);
	if (!JSValueIsObjectOfClass(eng->context, arguments[0],
				    gobject_method_class))
		return JSValueMakeNull(eng->context);

	info =
	    (GICallableInfo *) JSObjectGetPrivate((JSObjectRef) arguments[0]);
	data_obj = JSObjectMake(eng->context, NULL, NULL);

	seed_value_set_property(data_obj, "name",
				(JSValueRef)
				seed_value_from_string(g_base_info_get_name
									   ((GIBaseInfo *) info), exception));

	seed_value_set_property(data_obj, "return_type",
				seed_value_from_string
				(seed_g_type_name_to_string
				 (g_callable_info_get_return_type(info)), exception));

	args_obj = JSObjectMake(eng->context, NULL, NULL);

	seed_value_set_property(data_obj, "args", args_obj);

	for (i = 0; i < g_callable_info_get_n_args(info); ++i) {
		JSObjectRef argument = JSObjectMake(eng->context, NULL, NULL);

		const gchar *arg_name =
		    seed_g_type_name_to_string(g_arg_info_get_type
					       (g_callable_info_get_arg
						(info, i)));

		seed_value_set_property(argument, "type",
								seed_value_from_string(arg_name, exception));

		JSObjectSetPropertyAtIndex(eng->context, args_obj, i, argument,
					   NULL);
	}

	return data_obj;
}

JSValueRef
seed_check_syntax(JSContextRef ctx,
		  JSObjectRef function,
		  JSObjectRef this_object,
		  size_t argumentCount,
		  const JSValueRef arguments[], JSValueRef * exception)
{
	if (argumentCount != 0) {
		JSStringRef jsstr = JSValueToStringCopy(eng->context,
							arguments[0],
							exception);
		JSCheckScriptSyntax(ctx, jsstr, 0, 0, exception);
		if (jsstr)
			JSStringRelease(jsstr);
	} else {
		gchar *mes = g_strdup_printf("Seed.check_syntax expected"
					     "1 argument, got %d",
					     argumentCount);
	}
	return JSValueMakeNull(eng->context);
}

JSValueRef
seed_fork(JSContextRef ctx,
	  JSObjectRef function,
	  JSObjectRef this_object,
	  size_t argumentCount,
	  const JSValueRef arguments[], JSValueRef * exception)
{
	pid_t child;

	child = fork();
	return seed_value_from_int(child, exception);
}

static gboolean seed_timeout_function(gpointer user_data)
{
	// Evaluate timeout script

	const JSStringRef script = (const JSStringRef)user_data;
	JSEvaluateScript(eng->context, script, NULL, NULL, 0, NULL);
	JSStringRelease(script);

	return FALSE;		// remove timeout from main loop
}

JSValueRef
seed_set_timeout(JSContextRef ctx,
		 JSObjectRef function,
		 JSObjectRef this_object,
		 size_t argumentCount,
		 const JSValueRef arguments[], JSValueRef * exception)
{
	// TODO: check if a main loop is running. if not, return failure.

	//GMainLoop* loop = g_main_loop_new(NULL,FALSE);
	//if (!g_main_loop_is_running(loop))
	//      return JSValueMakeBoolean(ctx, 0);

	// TODO: convert to use an exception! (Matt!)

	if (argumentCount != 2)
		return JSValueMakeBoolean(ctx, 0);

	JSStringRef jsstr = JSValueToStringCopy(eng->context,
						arguments[0],
						exception);

	guint interval = seed_value_to_uint(arguments[1], exception);
	g_timeout_add(interval, &seed_timeout_function, jsstr);

	return JSValueMakeBoolean(ctx, 1);
}

JSValueRef
seed_closure(JSContextRef ctx,
		 JSObjectRef function,
		 JSObjectRef this_object,
		 size_t argumentCount,
		 const JSValueRef arguments[], JSValueRef * exception)
{
		GClosure * closure;

		if (argumentCount < 1 || argumentCount > 2)
		{
				gchar * mes = g_strdup_printf("Seed.closure expected 1 or 2"
											  "arguments, got %d",
											  argumentCount);
				seed_make_exception(exception, "ArgumentError", mes);
				g_free(mes);
				return JSValueMakeNull(ctx);
		}
		
		closure = g_closure_new_simple(sizeof(SeedClosure), 0);
		g_closure_set_marshal(closure, seed_signal_marshal_func);
		
		((SeedClosure *) closure)->function = (JSObjectRef) arguments[0];
		((SeedClosure *) closure)->object =
				0;
		if (argumentCount == 2 && !JSValueIsNull(eng->context, arguments[1])) {
				JSValueProtect(eng->context, (JSObjectRef) arguments[1]);
				((SeedClosure *) closure)->this = (JSObjectRef) arguments[1];
		} else
				((SeedClosure *) closure)->this = 0;
		
		JSValueProtect(eng->context, (JSObjectRef) arguments[0]);
		
		return (JSValueRef)seed_make_struct(closure, 0);
}

static ffi_type *
get_ffi_type (GITypeInfo *info)
{
		ffi_type *rettype;
		
		if (g_type_info_is_pointer (info))
				rettype = &ffi_type_pointer;
		else
				switch (g_type_info_get_tag (info))
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
				case GI_TYPE_TAG_SSIZE: /* FIXME */
				case GI_TYPE_TAG_LONG:
						rettype = &ffi_type_slong;
						break;
				case GI_TYPE_TAG_SIZE: /* FIXME */
				case GI_TYPE_TAG_TIME_T: /* May not be portable */
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
						g_assert_not_reached ();
      }
		
		return rettype;
}
 
typedef struct _SeedClosurePrivates
{
		GICallableInfo * info;
		JSValueRef function;
} SeedClosurePrivates;

static void
seed_handle_closure(ffi_cif *cif, 
					void * result, 
				    void ** args, 
					void * userdata)
{
		SeedClosurePrivates * privates = userdata;
		int num_args, i;
		JSValueRef * jsargs;
		JSValueRef * return_value;
		GIArgInfo * arg_info;
		GITypeInfo * return_type;
		GArgument rarg;
		
		num_args = g_callable_info_get_n_args(privates->info);
		jsargs = (JSValueRef *)g_new0(JSValueRef, num_args);
		
		for (i = 0; i < num_args; i++)
		{
				GITypeInfo * arg_type;
				GITypeTag tag;
				GType type;
				GArgument * arg = &rarg;
				
				arg_info = g_callable_info_get_arg(privates->info, i);
				arg_type = g_arg_info_get_type(arg_info);
				tag = g_type_info_get_tag(arg_type);
				
				switch(tag)
				{
				case GI_TYPE_TAG_BOOLEAN:
						arg->v_boolean = *(gboolean*)args[i];
						break;
				case GI_TYPE_TAG_INT8:
						arg->v_int8 = *(gint8*)args[i];
						break;
				case GI_TYPE_TAG_UINT8:
						arg->v_uint8 = *(guint8*)args[i];
						break;
				case GI_TYPE_TAG_INT16:
						arg->v_int16 = *(gint16*)args[i];
						break;
				case GI_TYPE_TAG_UINT16:
						arg->v_uint16 = *(guint16*)args[i];
						break;
				case GI_TYPE_TAG_INT32:
						arg->v_int32 = *(gint32*)args[i];
						break;
				case GI_TYPE_TAG_UINT32:
						arg->v_uint32 = *(guint32*)args[i];
						break;
				case GI_TYPE_TAG_LONG:
				case GI_TYPE_TAG_INT64:
						arg->v_int64 = *(glong*)args[i];
						break;
				case GI_TYPE_TAG_ULONG:
				case GI_TYPE_TAG_UINT64:
						arg->v_uint64 = *(glong*)args[i];
						break;
				case GI_TYPE_TAG_INT:
				case GI_TYPE_TAG_SSIZE:
				case GI_TYPE_TAG_SIZE:
						arg->v_int32 = *(gint*)args[i];
						break;
				case GI_TYPE_TAG_UINT:
						arg->v_uint32 = *(guint*)args[i];
						break;
				case GI_TYPE_TAG_FLOAT:
						arg->v_float = *(gfloat*)args[i];
						break;
				case GI_TYPE_TAG_DOUBLE:
						arg->v_double = *(gdouble*)args[i];
						break;
				case GI_TYPE_TAG_UTF8:
						arg->v_string = (gchar *)args[i];
						break;
				case GI_TYPE_TAG_INTERFACE:
				{
						GIBaseInfo *interface;
						GIInfoType interface_type;
						
						interface = g_type_info_get_interface(arg_type);
						interface_type = g_base_info_get_type(interface);
						
						if (interface_type == GI_INFO_TYPE_OBJECT ||
							interface_type == GI_INFO_TYPE_INTERFACE) {
 								arg->v_pointer =  *(gpointer*)args[i];
								break;
						}
						
						else if (interface_type == GI_INFO_TYPE_ENUM ||
								 interface_type == GI_INFO_TYPE_FLAGS) {
								arg->v_double = *(double*)args[i];
								break;
						} else if (interface_type == GI_INFO_TYPE_STRUCT) {
								arg->v_pointer = *(gpointer*)args[i];
								break;
						}
				}
				case GI_TYPE_TAG_GLIST:
				case GI_TYPE_TAG_GSLIST:
						arg->v_pointer = *(gpointer*)args[i];
						break;
				default:
						arg->v_pointer = 0;
				}
				jsargs[i] = seed_gi_argument_make_js(arg, arg_type, 0);
		}
		
		JSObjectCallAsFunction(eng->context, (JSObjectRef)privates->function,
							   0, num_args, jsargs, 0);
}

JSValueRef 
seed_closure_native(JSContextRef ctx,
		 JSObjectRef function,
		 JSObjectRef this_object,
		 size_t argumentCount,
		 const JSValueRef arguments[], JSValueRef * exception)
{
		ffi_cif * cif;
		ffi_closure *closure;
		ffi_type ** arg_types;
		ffi_arg result;
		ffi_status status;
		GICallableInfo * info;
		GITypeInfo * return_type;
		GIArgInfo * arg_info;
		gint num_args, i;
		SeedClosurePrivates * privates;
		
		if (argumentCount != 2)
		{
				gchar * mes = g_strdup_printf("Seed.closure_native expected"
											  " 2 arguments, got %d", 
											  argumentCount);
											  
				seed_make_exception(exception, "ArgumentError", mes);
				g_free(mes);
				
				return JSValueMakeNull(eng->context);
		}
		
		info = (GICallableInfo *)
				JSObjectGetPrivate((JSObjectRef)arguments[1]);

		num_args = g_callable_info_get_n_args(info);
		return_type = g_callable_info_get_return_type(info);
		arg_types = (ffi_type **)g_new0(ffi_type, num_args+1);
		cif = g_new0(ffi_cif, 1);
		 
		privates = g_new0(SeedClosurePrivates, 1);
		privates->info = info;
		privates->function = arguments[0];
		//Leaks the function? Would need a new class for closures and finalize
		//handler.
		JSValueProtect(eng->context, privates->function);

		for (i = 0; i < num_args; i++)
		{
				arg_info = g_callable_info_get_arg(info, i);
				arg_types[i] = get_ffi_type(g_arg_info_get_type(arg_info));
		}
		arg_types[num_args] = 0;

		
		closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE | 
					  	   PROT_EXEC,
					   MAP_ANON | MAP_PRIVATE, -1, 0);

		ffi_prep_cif(cif, FFI_DEFAULT_ABI, 2, 
					 &ffi_type_void, arg_types);
		ffi_prep_closure(closure, cif, seed_handle_closure, privates);

		
		return seed_make_struct(closure, 0);
}


void seed_init_builtins(int *argc, char ***argv)
{
	int i;
	JSObjectRef arrayObj;
	JSValueRef argcref;
	JSObjectRef obj =
	    (JSObjectRef) seed_value_get_property(eng->global, "Seed");

	seed_create_function("include", &seed_include, obj);
	seed_create_function("print", &seed_print, obj);
	seed_create_function("readline", &seed_readline, obj);
	seed_create_function("prototype", &seed_prototype, obj);
	seed_create_function("check_syntax", &seed_check_syntax, obj);
	seed_create_function("introspect", &seed_introspect, obj);
	seed_create_function("fork", &seed_fork, obj);
	seed_create_function("closure", &seed_closure, obj);
	seed_create_function("setTimeout", &seed_set_timeout, obj);
	seed_create_function("closure_native", &seed_closure_native, obj);



	arrayObj = JSObjectMake(eng->context, NULL, NULL);

	for (i = 0; i < *argc; ++i) {
		// TODO: exceptions!

		JSObjectSetPropertyAtIndex(eng->context, arrayObj, i,
								   seed_value_from_string((*argv)[i], 0),
					   NULL);
	}

	argcref = seed_value_from_int(*argc, 0);

	seed_value_set_property(arrayObj, "length", argcref);
	seed_value_set_property(obj, "argv", arrayObj);
}
