#include "../../libseed/seed.h"
#include <stdlib.h>
#include "test-common.h"

// Test stuff stolen from Clutter

#define TEST_SIMPLE(NAMESPACE, FUNC)									\
	extern void FUNC (TestSimpleFixture *fixture, gconstpointer data);	\
	g_test_add (NAMESPACE "/" #FUNC,									\
				TestSimpleFixture,										\
				shared_state,											\
				test_simple_fixture_setup,								\
				FUNC,													\
				test_simple_fixture_teardown);

void test_simple_fixture_setup (TestSimpleFixture *fixture, gconstpointer data)
{
}

void test_simple_fixture_teardown (TestSimpleFixture *fixture, gconstpointer data)
{
}

int main (int argc, char **argv)
{
	TestSharedState *shared_state = g_new0(TestSharedState, 1);
	const gchar * display = g_getenv ("DISPLAY");

	if (!display || *display == '\0')
	{
		g_print ("No DISPLAY found. Unable to run the test suite without X11.");
		return EXIT_SUCCESS;
	}

	g_test_init (&argc, &argv, NULL);

	SeedEngine * eng = seed_init(NULL, NULL);

	shared_state->argc_addr = &argc;
	shared_state->argv_addr = &argv;
	shared_state->eng = eng;

	TEST_SIMPLE ("/", basic);
	TEST_SIMPLE ("/types/", basic_types);

	return g_test_run ();
}

