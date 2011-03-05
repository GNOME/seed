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

#include "../../libseed/seed.h"
#include <stdlib.h>
#include "test-common.h"

// Test stuff stolen from Clutter

#define TEST_SIMPLE(NAMESPACE, FUNC)					\
  extern void FUNC (TestSimpleFixture *fixture, gconstpointer data);	\
  g_test_add (NAMESPACE "/" #FUNC,					\
	      TestSimpleFixture,					\
	      shared_state,						\
	      test_simple_fixture_setup,				\
	      FUNC,							\
	      test_simple_fixture_teardown);

void test_simple_fixture_setup(TestSimpleFixture * fixture, gconstpointer data)
{
  TestSharedState *state = (TestSharedState *) data;
  fixture->context = seed_context_create(state->eng->group, NULL);
}

void test_simple_fixture_teardown(TestSimpleFixture * fixture,
				  gconstpointer data)
{
  seed_context_unref(fixture->context);
}

int main(int argc, char **argv)
{
  TestSharedState *shared_state = g_new0(TestSharedState, 1);
  const gchar *display = g_getenv("DISPLAY");

  if (!display || *display == '\0')
    {
      g_print("No DISPLAY found. Unable to run the test suite without X11.");
      return EXIT_SUCCESS;
    }

  g_test_init(&argc, &argv, NULL);

  SeedEngine *eng = seed_init(NULL, NULL);

  shared_state->argc_addr = &argc;
  shared_state->argv_addr = &argv;
  shared_state->eng = eng;

  TEST_SIMPLE("/", basic);
  TEST_SIMPLE("/", closures);
  TEST_SIMPLE("/types/", basic_types);
  TEST_SIMPLE("/js-signal-from-c/", js_signal_from_c);

  return g_test_run();
}
