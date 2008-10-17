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
#include "libseed/seed.h"
#include "readline/readline.h"
#include <stdlib.h>

void seed_exec(int argc, char ** argv)
{
	SeedScript  * script;
	SeedException e;
	gchar * buffer;
	
	g_assert((argc==2));

	g_file_get_contents(argv[1], 
	&buffer, 0, 0);

	if (*buffer == '#')
	{
		while (*buffer != '\n')
			buffer++;
		buffer++;
	}
	script = seed_make_script(buffer, argv[1], 0);
	if (e =seed_script_exception(script))
		g_critical("%s. %s",
			   seed_exception_get_name(e),
			   seed_exception_get_message(e));
	seed_evaluate(script, 0);
	if (e = seed_script_exception(script))
		g_critical("%s. %s",
			   seed_exception_get_name(e),
			   seed_exception_get_message(e));
	
	g_free(script);
}

int main(int argc, char ** argv)
{	
	// TODO: GStreamer doesn't like to be initialized late. I don't remember
	// how to fix this right now.

	// Apparently our name for glib logging gets set in g*_init. can we set
	// that ourselves so that when we do on-the-fly init, we don't lose that?
	gst_init(&argc, &argv);	
	seed_init(&argc, &argv);

	if (!g_irepository_require(g_irepository_get_default(), 
				   "GObject", 0, 0))
		g_critical("Unable to import GObject repository");
	
	if(argc == 2)
		seed_exec(argc, argv);
	else
		printf("Usage: %s file.js\n", argv[0]);
	
	return 0;
}
