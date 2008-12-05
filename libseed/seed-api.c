#include "seed-private.h"

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
							 JSClassRef class,
							 gpointer private)
{
	return JSObjectMake(ctx, class, private);
}

JSValueRef seed_object_call(JSContextRef ctx,
							  JSObjectRef object,
							  JSObjectRef this,
							  size_t argument_count,
							  const JSValueRef arguments[],
							  JSValueRef * exception)
{
	return JSObjectCallAsFunction(ctx, object, this,
								  argument_count, arguments, 
								  exception);
}

SeedScript *seed_make_script(JSContextRef ctx,
							 const gchar * js,
							 const gchar * source_url,
							 gint line_number)
{
	SeedScript *ret = g_new0(SeedScript, 1);

	ret->script = JSStringCreateWithUTF8CString(js);

	if (source_url)
	{
		ret->source_url = JSStringCreateWithUTF8CString(source_url);
	}
	ret->line_number = line_number;

	JSCheckScriptSyntax(ctx, ret->script,
						ret->source_url, ret->line_number, &ret->exception);

	return ret;
}

JSValueRef seed_evaluate(JSContextRef ctx,
						 SeedScript * js, 
						 JSObjectRef this)
{
	JSValueRef ret;

	js->exception = 0;
	ret = JSEvaluateScript(ctx,
						   js->script, this, js->source_url,
						   js->line_number, &js->exception);

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
	JSStringRetain(string);
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
	return (gpointer)JSObjectGetPrivate(object);
}

void seed_object_set_private(JSObjectRef object, gpointer value)
{
	JSObjectSetPrivate(object, value);
}

