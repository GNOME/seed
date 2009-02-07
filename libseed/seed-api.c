/*
 * -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */

/*
 *	This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 *		Seed is free software: you can redistribute it and/or modify 
 *		it under the terms of the GNU Lesser General Public License as
 *		published by the Free Software Foundation, either version 3 of
 *		the License, or (at your option) any later version. 
 *		Seed is distributed in the hope that it will be useful, 
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *		GNU Lesser General Public License for more details. 
 *		You should have received a copy of the GNU Lesser General Public License 
 *		along with Seed.  If not, see <http://www.gnu.org/licenses/>. 
 *
 *		Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 */

#include "seed-private.h"

void seed_value_protect(JSContextRef ctx, JSValueRef value)
{
	JSValueProtect(ctx, value);
}

void seed_value_unprotect(JSContextRef ctx, JSValueRef value)
{
	JSValueUnprotect(ctx, value);
}

JSGlobalContextRef seed_context_create(JSContextGroupRef group,
				       JSClassRef global_class)
{
	return JSGlobalContextCreateInGroup(group, global_class);
}

JSGlobalContextRef seed_context_ref(JSGlobalContextRef ctx)
{
	return JSGlobalContextRetain(ctx);
}

void seed_context_unref(JSGlobalContextRef ctx)
{
	JSGlobalContextRelease(ctx);
}

JSValueRef seed_make_null(JSContextRef ctx)
{
	return JSValueMakeNull(ctx);
}

JSObjectRef seed_make_object(JSContextRef ctx,
			     JSClassRef class, gpointer private)
{
	return JSObjectMake(ctx, class, private);
}

void seed_object_set_property_at_index(JSContextRef ctx,
				       JSObjectRef object,
				       gint index,
				       JSValueRef value, JSValueRef * exception)
{
	JSObjectSetPropertyAtIndex(ctx, object, index, value, exception);
}

JSValueRef seed_object_call(JSContextRef ctx,
			    JSObjectRef object,
			    JSObjectRef this,
			    size_t argument_count,
			    const JSValueRef arguments[],
			    JSValueRef * exception)
{
	return JSObjectCallAsFunction(ctx, object, this,
				      argument_count, arguments, exception);
}

SeedScript *seed_make_script(JSContextRef ctx,
			     const gchar * js,
			     const gchar * source_url, gint line_number)
{
	SeedScript *ret = g_new0(SeedScript, 1);

	if (js)
		ret->script = JSStringCreateWithUTF8CString(js);
	else
		ret->script = JSStringCreateWithUTF8CString("");

	if (source_url)
	{
		ret->source_url = JSStringCreateWithUTF8CString(source_url);
	}
	ret->line_number = line_number;

	JSCheckScriptSyntax(ctx, ret->script,
			    ret->source_url, ret->line_number, &ret->exception);

	return ret;
}

SeedScript *seed_script_new_from_file(JSContextRef ctx, 
				      gchar *file)
{
	SeedScript *script;
	GError *e = NULL;
	gchar *contents = NULL;
	
	g_file_get_contents(file, &contents, NULL, &e);
	script = seed_make_script(ctx, contents, file, 0);
	if (e)
	{
		seed_make_exception_from_gerror(ctx, &script->exception, e);
		g_error_free(e);
	}
	
	return script;	
}

JSValueRef seed_evaluate(JSContextRef ctx, SeedScript * js, JSObjectRef this)
{
	JSValueRef ret;

	js->exception = 0;
	ret = JSEvaluateScript(ctx,
			       js->script, this, js->source_url,
			       js->line_number, &js->exception);

	return ret;
}

JSValueRef seed_simple_evaluate(JSContextRef ctx, gchar * source)
{
	JSValueRef ret;
	JSStringRef script = JSStringCreateWithUTF8CString(source);

	ret = JSEvaluateScript(ctx, script, NULL, NULL, 0, NULL);

	JSStringRelease(script);
	return ret;
}

JSValueRef seed_script_exception(SeedScript * s)
{
	return s->exception;
}

gsize seed_string_get_maximum_size(JSStringRef string)
{
	return JSStringGetMaximumUTF8CStringSize(string);
}

gsize seed_string_to_utf8_buffer(JSStringRef string, gchar * buffer,
				 size_t buffer_size)
{
	return JSStringGetUTF8CString(string, buffer, buffer_size);
}

gboolean seed_string_is_equal(JSStringRef a, JSStringRef b)
{
	return JSStringIsEqual(a, b);
}

gboolean seed_string_is_equal_utf8(JSStringRef a, const gchar * b)
{
	return JSStringIsEqualToUTF8CString(a, b);
}

JSStringRef seed_string_ref(JSStringRef string)
{
	return JSStringRetain(string);
}

void seed_string_unref(JSStringRef string)
{
	JSStringRelease(string);
}

JSClassRef seed_create_class(JSClassDefinition * def)
{
	return JSClassCreate(def);
}

JSObjectRef seed_make_constructor(JSContextRef ctx,
				  JSClassRef class,
				  JSObjectCallAsConstructorCallback constructor)
{
	return JSObjectMakeConstructor(ctx, class, constructor);
}

gpointer seed_object_get_private(JSObjectRef object)
{
	return (gpointer) JSObjectGetPrivate(object);
}

void seed_object_set_private(JSObjectRef object, gpointer value)
{
	JSObjectSetPrivate(object, value);
}

gboolean seed_value_is_null(JSContextRef ctx,
			    JSValueRef value)
{
	return JSValueIsNull(ctx, value);
}

gboolean seed_value_is_object(JSContextRef ctx,
			      JSValueRef value)
{
	return !seed_value_is_null(ctx, value) &&
		JSValueIsObject(ctx, value);
}
gboolean seed_value_is_function(JSContextRef ctx,
				JSObjectRef value)
{
	return seed_value_is_object(ctx, value) && 
		JSObjectIsFunction(ctx, value);
}

