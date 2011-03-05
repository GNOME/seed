/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2 of
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
#include "config.h"

extern gboolean seed_interpreter_arg_print_version;
extern gchar *seed_interpreter_arg_exec_string;

static GOptionEntry seed_args[] = {
  {"version", 0, 0, G_OPTION_ARG_NONE, &seed_interpreter_arg_print_version,
   "Print interpreter version", 0},
  {"execute", 'e', 0, G_OPTION_ARG_STRING, &seed_interpreter_arg_exec_string,
   "Pass program in as string", "expression"},
  {NULL,},
};


static GOptionGroup *
seed_interpreter_get_option_group (void)
{
  GOptionGroup *group;

  group = g_option_group_new ("seed-interpreter", "Interpreter Options",
			      "Show Interpreter Options", NULL, NULL);
  g_option_group_add_entries (group, seed_args);

  return group;
}

gboolean
seed_interpreter_parse_args (int *argc, char ***argv)
{
  GOptionContext *option_context;
  GOptionGroup *interpreter_group;

  GError *error = NULL;
  gboolean ret = TRUE;

  option_context = g_option_context_new (NULL);
  g_option_context_set_ignore_unknown_options (option_context, TRUE);
  g_option_context_set_help_enabled (option_context, TRUE);

  /* Initiate any command line options from the backend */

  interpreter_group = seed_interpreter_get_option_group ();
  g_option_context_set_main_group (option_context, interpreter_group);
  g_option_context_add_group (option_context, seed_get_option_group ());

  if (!g_option_context_parse (option_context, argc, argv, &error))
    {
      if (error)
	{
	  g_warning ("%s", error->message);
	  g_error_free (error);
	}

      ret = FALSE;
    }

  g_option_context_free (option_context);

  return ret;
}
