/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed.h
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


#ifndef _SEED_H
#define _SEED_H

typedef gpointer SeedValue;
typedef gpointer SeedObject;
typedef gpointer SeedClass;
typedef gpointer SeedException;
typedef gpointer SeedFunction;

typedef struct _SeedScript SeedScript;

/* seed-engine.c */
gboolean seed_init(int * argc, char *** argv);

SeedScript * seed_make_script(const gchar *s, const gchar * source_url,
			      int line_number);
SeedException seed_script_exception(SeedScript *s);
SeedException seed_make_exception(gchar * name, gchar * message);
gchar * seed_exception_get_name(SeedException e);
gchar * seed_exception_get_message(SeedException e);

SeedValue seed_evaluate(SeedScript *s, SeedObject this);

gchar * seed_value_to_string(SeedValue obj);

/* seed-types.c */

gboolean	seed_value_to_boolean(SeedValue val);
SeedValue	seed_value_from_boolean(gboolean val);

guint		seed_value_to_uint(SeedValue val);
SeedValue	seed_value_from_uint(guint val);

gint		seed_value_to_int(SeedValue val);
SeedValue	seed_value_from_int(gint val);

gchar		seed_value_to_char(SeedValue val);
SeedValue	seed_value_from_char(gchar val);

guchar		seed_value_to_uchar(SeedValue val);
SeedValue	seed_value_from_uchar(guchar val);

glong		seed_value_to_long(SeedValue val);
SeedValue	seed_value_from_long(glong val);

gulong		seed_value_to_ulong(SeedValue val);
SeedValue	seed_value_from_ulong(gulong val);

gint64		seed_value_to_int64(SeedValue val);
SeedValue	seed_value_from_int64(gint64 val);

guint64		seed_value_to_uint64(SeedValue val);
SeedValue	seed_value_from_uint64(guint64 val);

gfloat		seed_value_to_float(SeedValue val);
SeedValue	seed_value_from_float(gfloat val);

gdouble		seed_value_to_double(SeedValue val);
SeedValue	seed_value_from_double(gdouble val);

gchar *		seed_value_to_string(SeedValue val);
SeedValue	seed_value_from_string(gchar * val);

GObject *	seed_value_to_object(SeedValue val);
SeedValue	seed_value_from_object(GObject * val);

#endif
