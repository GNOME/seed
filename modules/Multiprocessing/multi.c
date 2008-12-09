#include <seed.h>
#include <unistd.h>
#include <stdio.h>

SeedEngine * eng;
SeedObject namespace_ref;
SeedClass pipe_class;

SeedObject seed_construct_pipe(SeedContext ctx,
							   SeedObject constructor,
							   size_t argument_count,
							   const SeedValue arguments[],
							   SeedException * exception)
{
	GIOChannel *one, *two;
	SeedObject jsone, jstwo, jsret;
	int fd[2];
	
	if (pipe(fd) < 0)
	{
		perror("Pipe failed. Make me a javascript exception");
		return seed_make_null(ctx);
	}
	
	one = g_io_channel_unix_new(fd[0]);
	two = g_io_channel_unix_new(fd[1]);
	
	jsret = seed_make_object(ctx, 0, 0);
	jsone = seed_make_object(ctx, pipe_class, one);
	jstwo = seed_make_object(ctx, pipe_class, two);
	
	seed_object_set_property_at_index(ctx, jsret, 0, jsone, exception);
	seed_object_set_property_at_index(ctx, jsret, 1, jstwo, exception);
	
	return jsret;
}

#define GET_CHANNEL GIOChannel * channel =  \
		seed_object_get_private(this_object)


SeedValue seed_pipe_read (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	SeedValue ret;
	gchar * read;
	GET_CHANNEL;

	g_io_channel_read_line(channel, &read, 0, 0, 0);
	ret = seed_value_from_string(ctx, read, exception);
	g_free(read);
	
	return ret;
}

typedef struct _marshal_privates
{
	SeedObject function;
	SeedObject source;
	SeedValue user_data;
} marshal_privates;

static gboolean gio_marshal_func(GIOChannel * source,
								 GIOCondition condition,
								 gpointer data)
{
	SeedGlobalContext ctx = seed_context_create(eng->group, 0);
	SeedValue jscondition = seed_value_from_long(ctx, (glong)condition, 0);
	SeedValue args[3], ret;
	marshal_privates * priv = (marshal_privates *) data;
	gboolean bret;
	
	args[0] = priv->source;
	args[1] = jscondition;
	args[2] = priv->user_data;
	
	ret = seed_object_call(ctx, priv->function, 0,
					 3, args, 0);
	
	bret = seed_value_to_boolean(ctx, ret, 0);
	seed_context_unref(ctx);

	return bret;
}

SeedValue seed_pipe_write (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	gchar * data;
	gsize written;
	gchar eol = '\n';
	GET_CHANNEL;
	
	data = seed_value_to_string(ctx, arguments[0], exception);
	g_io_channel_write_chars(channel, data, -1, &written, 0);
	g_io_channel_write_chars(channel, &eol, 1, 0, 0);
	g_io_channel_flush(channel, 0);

	return seed_value_from_int(ctx, written, exception);
}

SeedValue seed_pipe_add_watch (SeedContext ctx,
								 SeedObject function,
								 SeedObject this_object,
								 size_t argument_count,
								 const SeedValue arguments[],
								 SeedException * exception)
{
	GET_CHANNEL;
	marshal_privates * priv = g_malloc0(sizeof(marshal_privates));
	glong condition = seed_value_to_long(ctx, arguments[0], exception);
	
	priv->function = arguments[1];
	priv->source = this_object;
	priv->user_data = argument_count == 3 ? arguments[2] : seed_make_null(ctx);
	
	g_io_add_watch(channel, condition, gio_marshal_func, priv);
	
	return seed_value_from_boolean(ctx, TRUE, exception);
}

seed_static_function pipe_funcs[] = {
	{"read", seed_pipe_read, 0},
	{"write", seed_pipe_write, 0},
	{"add_watch", seed_pipe_add_watch, 0}
};

void seed_module_init(SeedEngine * local_eng)
{
	SeedObject pipe_constructor;
	seed_class_definition pipe_class_def = seed_empty_class;
	eng = local_eng;
	
	namespace_ref = seed_make_object(eng->context, 0, 0);
	
	pipe_class_def.class_name = "Pipe";
	pipe_class_def.static_functions = pipe_funcs;

	pipe_class = seed_create_class(&pipe_class_def);
	
	seed_object_set_property(eng->context,
							 eng->global, 
							 "Multiprocessing", 
							 namespace_ref);


	pipe_constructor = seed_make_constructor(eng->context,
											 0,
											 seed_construct_pipe);
	
	seed_object_set_property(eng->context,
							 namespace_ref,
							 "Pipe",
							 pipe_constructor);
	
}
