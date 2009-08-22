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

#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>
#include "../libseed/seed.h"
#include "../libseed/seed-debug.h"
#include <girepository.h>

#define DEFAULT_PATH "."

SeedEngine *eng;

static void
seed_repl ()
{
  SeedScript *script;

  script = seed_make_script (eng->context, "repl = imports.repl", NULL, 0);
  seed_evaluate (eng->context, script, 0);

  g_free (script);
}

static void
seed_exec (gchar * filename)
{
  SeedObject global;
  SeedScript *script;
  SeedException e;
  gchar *buffer;

  g_file_get_contents (filename, &buffer, 0, 0);

  if (!buffer)
    {
      g_critical ("File %s not found!", filename);
      exit (EXIT_FAILURE);
    }

  if (*buffer == '#')
    {
      while (*buffer != '\n')
	buffer++;
      buffer++;
    }

  script = seed_make_script (eng->context, buffer, filename, 1);

  if ((e = seed_script_exception (script)))
    {
      g_critical ("%s", seed_exception_to_string (eng->context, e));
      exit (EXIT_FAILURE);
    }

  global = seed_context_get_global_object (eng->context);
  seed_importer_add_global (global, filename);

  seed_evaluate (eng->context, script, 0);
  if ((e = seed_script_exception (script)))
    g_critical ("%s", seed_exception_to_string (eng->context, e));

  g_free (script);
}

gint
main (gint argc, gchar ** argv)
{
  g_set_prgname ("seed");
  g_thread_init (0);
  eng = seed_init (&argc, &argv);

  seed_engine_set_search_path (eng, DEFAULT_PATH);

  if (argc == 1)
    seed_repl ();
  else
    seed_exec (argv[1]);

  return EXIT_SUCCESS;
}
