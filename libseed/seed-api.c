#include "seed-private.h"

JSObjectRef seed_make_object(JSContextRef ctx, 
							 JSClassRef class,
							 gpointer private)
{
	return JSObjectMake(ctx, class, private);
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

	JSCheckScriptSyntax(eng->context, ret->script,
						ret->source_url, ret->line_number, &ret->exception);

	return ret;
}

JSValueRef seed_evaluate(JSContextRef ctx,
						 SeedScript * js, 
						 JSObjectRef this)
{
	JSValueRef ret;

	js->exception = 0;
	ret = JSEvaluateScript(eng->context,
						   js->script, this, js->source_url,
						   js->line_number, &js->exception);

	return ret;
}

JSValueRef seed_script_exception(SeedScript * s)
{
	return s->exception;
}
