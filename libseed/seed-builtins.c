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

JSValueRef
seed_include(JSContextRef ctx,
				 JSObjectRef function,
				 JSObjectRef this_object,
				 size_t argumentCount,
				 const JSValueRef arguments[],
				 JSValueRef * exception)
{
	const gchar * import_file;
	gchar * buffer, * walk;
	GValue gval = {0};
	
	seed_gvalue_from_seed_value((SeedValue)arguments[0], G_TYPE_STRING, &gval);
	g_assert(G_VALUE_HOLDS_STRING(&gval));
	
	import_file = g_value_get_string(&gval);
	
	g_file_get_contents(import_file, &buffer, 0, 0);
	
	if(!buffer)
	{
		//gchar * mes = g_strdup_printf("File not found: %s.\n", import_file);
		//seed_make_exception(exception, "FileNotFound", mes);
		//g_free(mes);
		return 0; // TODO: raise exception?
	}
	
	walk = buffer;
	
	if(*walk == '#')
	{
		while(*walk != '\n')
			walk++;
		walk++;
	}
	
	walk = strdup(walk);
	g_free(buffer);
	
	JSEvaluateScript(ctx, JSStringCreateWithUTF8CString(walk), 
					 NULL, JSStringCreateWithUTF8CString(import_file), 0, NULL);
	
	return 0;
}

JSValueRef
seed_print(JSContextRef ctx,
			  JSObjectRef function,
			  JSObjectRef this_object,
			  size_t argumentCount,
			  const JSValueRef arguments[],
			  JSValueRef * exception)
{
	if(argumentCount < 1)
		return JSValueMakeNull(eng->context);
	
	gchar * buf = seed_value_to_string(arguments[0]);
	printf("%s\n", buf);
	free(buf);
	
	return JSValueMakeNull(eng->context);
}

JSValueRef
seed_readline(JSContextRef ctx,
			  	  JSObjectRef function,
			 	  JSObjectRef this_object,
			     size_t argumentCount,
			     const JSValueRef arguments[],
			     JSValueRef * exception)
{
	// TODO: Should add an interface to rl_bind_key
	// Perhaps Seed.readline_bind('a', function)
	// Then automagically do function stuff and make it happen!
	
	JSValueRef valstr = 0;
	gchar * str = 0;
	gchar * buf = seed_value_to_string(arguments[0]);
	
	str = readline(buf);
	if(str && *str)
	{
		add_history(str);
		valstr = seed_value_from_string(str);
		free(str);
	}
	
	free(buf);
	
	if(valstr == 0)
		valstr = JSValueMakeNull(eng->context);
	
	return valstr;
}

void seed_init_builtins(int * argc, char *** argv)
{
	int i;
	JSObjectRef arrayObj;
	JSValueRef argcref;
	JSObjectRef obj = (JSObjectRef)seed_value_get_property(eng->global, "Seed");
	
	seed_create_function("include", &seed_include, obj);
	seed_create_function("print", &seed_print, obj);
	seed_create_function("readline", &seed_readline, obj);
	
	arrayObj = JSObjectMake(eng->context, NULL, NULL);
	
	for(i = 0; i < *argc; ++i)
	{
		// TODO: exceptions!
		
		JSObjectSetPropertyAtIndex(eng->context, arrayObj, i,
								   seed_value_from_string((*argv)[i]), NULL);
	}
	
	argcref = seed_value_from_int(*argc);

	seed_value_set_property(arrayObj, "length", argcref);
	seed_value_set_property(obj, "argv", arrayObj);
}

