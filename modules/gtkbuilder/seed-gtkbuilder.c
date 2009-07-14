#include <seed-module.h>

#include <gtk/gtk.h>

typedef struct _builder_ud {
  SeedContext ctx;
  SeedObject obj;
  SeedObject user_data;
} builder_ud;

static void
seed_builder_connect_func (GtkBuilder *builder,
			   GObject *object,
			   const gchar *signal_name,
			   const gchar *handler_name,
			   GObject *connect_object,
			   GConnectFlags flags,
			   gpointer user_data)
{
  SeedContext ctx;
  SeedObject obj, func;
  builder_ud *priv = (builder_ud *)user_data;
  GClosure *closure;

  ctx = priv->ctx;
  obj = priv->obj;

  func = seed_object_get_property (ctx, obj, handler_name);
  if (!seed_value_is_object (ctx, func) || !seed_value_is_function (ctx, func))
    return;

  closure = seed_closure_new (ctx, func, priv->user_data,
                              "signal handler (GtkBuilder)");
  if (connect_object != NULL)
    g_object_watch_closure (connect_object, closure);

  g_signal_connect_closure (object, signal_name, closure, FALSE);
}

SeedValue
seed_gtk_builder_connect_signals(SeedContext ctx,
				 SeedObject function,
				 SeedObject this_object,
				 gsize argument_count,
				 const SeedValue arguments[],
				 SeedException *exception)
{
  builder_ud ud;
  GtkBuilder *b;
  
  CHECK_ARG_COUNT("GtkBuilder.connect_signals", 1);

  if (!seed_value_is_object (ctx, arguments[0]))
    {
      seed_make_exception (ctx, exception, "TypeError",
			   "connect_signals expects one object as the first argument");
      return seed_make_undefined (ctx);
    }

  b = GTK_BUILDER (seed_value_to_object (ctx, this_object, exception));
  ud.ctx = ctx;
  ud.obj = arguments[0];
  if (argument_count == 2)
    ud.user_data = arguments[1];
  else
    ud.user_data = NULL;
  gtk_builder_connect_signals_full(b, seed_builder_connect_func, &ud);

  return seed_make_undefined (ctx);
}

SeedObject
seed_module_init(SeedEngine *eng)
{
  SeedObject gtkbuilder_proto;

  gtkbuilder_proto = seed_simple_evaluate (eng->context,
					   "imports.gi.Gtk.Builder.prototype",
					   NULL);
  seed_create_function (eng->context,
			"connect_signals",
			seed_gtk_builder_connect_signals,
			gtkbuilder_proto);

  return seed_make_object (eng->context, NULL, NULL);
}
