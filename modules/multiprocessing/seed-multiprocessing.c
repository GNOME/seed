/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 * Seed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with Seed.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) Robert Carr 2009 <carrr@rpi.edu>
 */

#include <seed-module.h>

#include <unistd.h>
#include <stdio.h>

SeedEngine *eng;
SeedObject namespace_ref;
SeedClass pipe_class;

typedef struct _pipe_priv {
  GIOChannel *read;
  GIOChannel *write;
} pipe_priv;

void pipe_finalize(SeedObject pipeobj)
{
  pipe_priv *priv = seed_object_get_private(pipeobj);
  g_io_channel_unref(priv->read);
  g_io_channel_unref(priv->write);
  g_free(priv);
}

SeedObject seed_construct_pipe(SeedContext ctx,
			       SeedObject constructor,
			       size_t argument_count,
			       const SeedValue arguments[],
			       SeedException * exception)
{
  SeedObject jsone, jstwo, jsret;
  int fd1[2], fd2[2];
  pipe_priv *priv_one, *priv_two;
  
  CHECK_ARG_COUNT("multiprocessing.pipe constructor", 0);

  if (pipe(fd1) < 0)
    {
      perror("Pipe failed. Make me a javascript exception");
      return seed_make_null(ctx);
    }
  if (pipe(fd2) < 0)
    {
      perror("Pipe failed. Make me a javascript exception");
      return seed_make_null(ctx);
    }

  priv_one = g_new0(pipe_priv, 1);
  priv_two = g_new0(pipe_priv, 1);

  priv_one->read = g_io_channel_unix_new(fd1[0]);
  priv_one->write = g_io_channel_unix_new(fd2[1]);
  priv_two->read = g_io_channel_unix_new(fd2[0]);
  priv_two->write = g_io_channel_unix_new(fd1[1]);

  g_io_channel_set_close_on_unref(priv_one->read, TRUE);
  g_io_channel_set_close_on_unref(priv_one->write, TRUE);
  g_io_channel_set_close_on_unref(priv_two->read, TRUE);
  g_io_channel_set_close_on_unref(priv_two->write, TRUE);

  jsret = seed_make_object(ctx, 0, 0);
  jsone = seed_make_object(ctx, pipe_class, priv_one);
  jstwo = seed_make_object(ctx, pipe_class, priv_two);

  seed_object_set_property_at_index(ctx, jsret, 0, jsone, exception);
  seed_object_set_property_at_index(ctx, jsret, 1, jstwo, exception);

  return jsret;
}

#define GET_CHANNEL pipe_priv * priv =		\
    seed_object_get_private(this_object)

SeedValue seed_pipe_read(SeedContext ctx,
			 SeedObject function,
			 SeedObject this_object,
			 size_t argument_count,
			 const SeedValue arguments[], SeedException * exception)
{
  SeedValue ret;
  gchar *read;
  GET_CHANNEL;
  
  CHECK_ARG_COUNT("multiprocessing.pipe.read", 0);

  g_io_channel_read_line(priv->read, &read, 0, 0, 0);
  ret = seed_value_from_string(ctx, read, exception);
  g_free(read);

  return ret;
}

typedef struct _marshal_privates {
  SeedObject function;
  SeedObject source;
  SeedValue user_data;
} marshal_privates;

static gboolean gio_marshal_func(GIOChannel * source,
				 GIOCondition condition, gpointer data)
{
  SeedGlobalContext ctx = seed_context_create(eng->group, 0);
  SeedValue jscondition = seed_value_from_long(ctx, (glong) condition, 0);
  SeedValue args[3], ret;
  marshal_privates *priv = (marshal_privates *) data;
  gboolean bret;

  args[0] = priv->source;
  args[1] = jscondition;
  args[2] = priv->user_data;

  ret = seed_object_call(ctx, priv->function, 0, 3, args, 0);

  bret = seed_value_to_boolean(ctx, ret, 0);
  seed_context_unref(ctx);

  if (!bret)
    g_free(priv);

  return bret;
}

SeedValue seed_pipe_write(SeedContext ctx,
			  SeedObject function,
			  SeedObject this_object,
			  size_t argument_count,
			  const SeedValue arguments[],
			  SeedException * exception)
{
  gchar *data;
  gsize written;
  gchar eol = '\n';
  GET_CHANNEL;
  
  CHECK_ARG_COUNT("multiprocessing.pipe.write", 1);

  data = seed_value_to_string(ctx, arguments[0], exception);
  g_io_channel_write_chars(priv->write, data, -1, &written, 0);
  g_io_channel_write_chars(priv->write, &eol, 1, 0, 0);
  g_io_channel_flush(priv->write, 0);

  return seed_value_from_int(ctx, written, exception);
}

SeedValue seed_pipe_add_watch(SeedContext ctx,
			      SeedObject function,
			      SeedObject this_object,
			      size_t argument_count,
			      const SeedValue arguments[],
			      SeedException * exception)
{
  GET_CHANNEL;
  
  marshal_privates *mpriv = g_malloc0(sizeof(marshal_privates));
  glong condition = seed_value_to_long(ctx, arguments[0], exception);

  mpriv->function = arguments[1];
  mpriv->source = this_object;
  mpriv->user_data = argument_count == 3 ? arguments[2] : seed_make_null(ctx);

  g_io_add_watch(priv->read, condition, gio_marshal_func, mpriv);

  return seed_value_from_boolean(ctx, TRUE, exception);
}

seed_static_function pipe_funcs[] = {
  {"read", seed_pipe_read, 0},
  {"write", seed_pipe_write, 0},
  {"add_watch", seed_pipe_add_watch, 0},
  {NULL, NULL, 0}
};

SeedObject
seed_module_init(SeedEngine * local_eng)
{
  SeedObject pipe_constructor;
  seed_class_definition pipe_class_def = seed_empty_class;
  eng = local_eng;

  namespace_ref = seed_make_object(eng->context, 0, 0);

  pipe_class_def.class_name = "Pipe";
  pipe_class_def.static_functions = pipe_funcs;
  pipe_class_def.finalize = pipe_finalize;

  pipe_class = seed_create_class(&pipe_class_def);

  pipe_constructor = seed_make_constructor(eng->context,
					   0, seed_construct_pipe);

  seed_object_set_property(eng->context,
			   namespace_ref, "Pipe", pipe_constructor);

  return namespace_ref;
}
