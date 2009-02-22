/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
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

#include <glib.h>
#include <glib-object.h>
#include <stdio.h>
#include "../libseed/seed.h"
#include "../libseed/seed-debug.h"
#include <stdlib.h>
#include <girepository.h>

SeedEngine *eng;

void seed_repl(int argc, char **argv)
{
	SeedScript *script;

	script = seed_make_script(eng->context, "Seed.import_namespace('readline');"
							  "while(1) { try { Seed.print(eval("
							  "readline.readline(\"> \"))); } catch(e) {"
							  "Seed.print(e.name + \" \" + e.message);}}",
							  NULL, 0);
	seed_evaluate(eng->context, script, 0);

	g_free(script);
}

void seed_exec(gint argc, gchar ** argv)
{
	SeedScript *script;
	SeedException e;
	gchar *buffer;

	g_file_get_contents(argv[1], &buffer, 0, 0);

	if (!buffer)
	{
		g_critical("File %s not found!", argv[1]);
		exit(1);
	}

	if (*buffer == '#')
	{
		while (*buffer != '\n')
			buffer++;
		buffer++;
	}
	script = seed_make_script(eng->context, buffer, argv[1], 1);
	if ((e = seed_script_exception(script)))
	{
		g_critical("%s. %s in %s at line %d",
				   seed_exception_get_name(eng->context, e),
				   seed_exception_get_message(eng->context, e),
				   seed_exception_get_file(eng->context, e),
				   seed_exception_get_line(eng->context, e));
		exit(1);
	}
	seed_evaluate(eng->context, script, 0);
	if ((e = seed_script_exception(script)))
		g_critical("%s. %s in %s at line %d",
				   seed_exception_get_name(eng->context, e),
				   seed_exception_get_message(eng->context, e),
				   seed_exception_get_file(eng->context, e),
				   seed_exception_get_line(eng->context, e));

	g_free(script);
}

int main(gint argc, gchar ** argv)
{
	g_set_prgname("seed");
	g_thread_init(0);
	eng = seed_init(&argc, &argv);

	if (!g_irepository_require(g_irepository_get_default(), "GObject", 0, 0, 0))
		g_critical("Unable to import GObject repository");

	if (argc == 1)
		seed_repl(argc, argv);
	else
		seed_exec(argc, argv);

	return 0;
}
