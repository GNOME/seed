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

#include "../../libseed/seed.h"
#include "test-common.h"
#include <string.h>

// The biggest problem here is everything gets cast to a double in JSCore!

void basic_types(TestSimpleFixture * fixture, gconstpointer _data)
{
	// bool to/from JS equality

	gboolean bool_test_in = TRUE;
	SeedValue *bool_test = seed_value_from_boolean(fixture->context,
												   bool_test_in, NULL);
	gboolean bool_test_out = seed_value_to_boolean(fixture->context,
												   bool_test, NULL);

	g_assert(bool_test_in == bool_test_out);

	// uint to/from JS equality

	guint uint_test_in = 2946623;
	SeedValue *uint_test = seed_value_from_uint(fixture->context,
												uint_test_in, NULL);
	guint uint_test_out = seed_value_to_uint(fixture->context,
											 uint_test, NULL);

	g_assert(uint_test_in == uint_test_out);

	// int to/from JS equality

	gint int_test_in = -54374;
	SeedValue *int_test = seed_value_from_int(fixture->context,
											  int_test_in, NULL);
	gint int_test_out = seed_value_to_int(fixture->context,
										  int_test, NULL);

	g_assert(int_test_in == int_test_out);

	// char to/from JS equality

	gchar char_test_in = -126;
	SeedValue *char_test = seed_value_from_char(fixture->context,
												char_test_in, NULL);
	gchar char_test_out = seed_value_to_char(fixture->context,
											 char_test, NULL);

	g_assert(char_test_in == char_test_out);

	// uchar to/from JS equality

	guchar uchar_test_in = 250;
	SeedValue *uchar_test = seed_value_from_uchar(fixture->context,
												  uchar_test_in, NULL);
	guchar uchar_test_out = seed_value_to_uchar(fixture->context,
												uchar_test, NULL);

	g_assert(uchar_test_in == uchar_test_out);

	// long to/from JS equality

	glong long_test_in = -454250;
	SeedValue *long_test = seed_value_from_long(fixture->context,
												long_test_in, NULL);
	glong long_test_out = seed_value_to_long(fixture->context,
											 long_test, NULL);

	g_assert(long_test_in == long_test_out);

	// ulong to/from JS equality

	gulong ulong_test_in = 250;
	SeedValue *ulong_test = seed_value_from_ulong(fixture->context,
												  ulong_test_in, NULL);
	gulong ulong_test_out = seed_value_to_ulong(fixture->context,
												ulong_test, NULL);

	g_assert(ulong_test_in == ulong_test_out);

	// int64 to/from JS equality

	gint64 int64_test_in = -54374;
	SeedValue *int64_test = seed_value_from_int64(fixture->context,
												  int64_test_in, NULL);
	gint64 int64_test_out = seed_value_to_int64(fixture->context,
												int64_test, NULL);

	g_assert(int64_test_in == int64_test_out);

	// uint64 to/from JS equality

	guint64 uint64_test_in = 2946623;
	SeedValue *uint64_test = seed_value_from_uint64(fixture->context,
													uint64_test_in, NULL);
	guint64 uint64_test_out = seed_value_to_uint64(fixture->context,
												   uint64_test, NULL);

	g_assert(uint64_test_in == uint64_test_out);

	// float to/from JS equality

	gfloat float_test_in = 1.618;
	SeedValue *float_test = seed_value_from_float(fixture->context,
												  float_test_in, NULL);
	gfloat float_test_out = seed_value_to_float(fixture->context,
												float_test, NULL);

	g_assert(float_test_in == float_test_out);

	// double to/from JS equality

	gdouble double_test_in = 1.6134857638;
	SeedValue *double_test = seed_value_from_double(fixture->context,
													double_test_in, NULL);
	gdouble double_test_out = seed_value_to_double(fixture->context,
												   double_test, NULL);

	g_assert(double_test_in == double_test_out);

	// string to/from JS equality

	gchar *string_test_in = "Hello, world!";
	SeedValue *string_test = seed_value_from_string(fixture->context,
													string_test_in, NULL);
	gchar *string_test_out = seed_value_to_string(fixture->context,
												  string_test, NULL);

	g_assert(strncmp(string_test_in, string_test_out,
					 strlen(string_test_in)) == 0);

	// filename to/from JS equality

	gchar *filename_test_in = "/bin";
	SeedValue *filename_test = seed_value_from_filename(fixture->context,
														filename_test_in,
														NULL);
	gchar *filename_test_out = seed_value_to_filename(fixture->context,
													  filename_test, NULL);

	g_assert(strncmp(filename_test_in, filename_test_out,
					 strlen(filename_test_in)) == 0);
	SeedValue si[2];
	si[0] = seed_value_from_string (fixture->context, "Hi", NULL);
	si[1] = seed_value_from_int (fixture->context, 1, NULL);
	gint ni;
	gchar *ns;

	seed_value_to_format (fixture->context, "si", si, NULL, &ns, &ni, NULL);
	g_assert (ni == 1);
	g_assert (!strcmp(ns, "Hi"));


}
