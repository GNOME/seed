#include "../../libseed/seed.h"
#include <stdlib.h>
#include "test-common.h"

/* This is a bit of sugar for adding new conformance tests:
 *
 * - It adds an extern function definition just to save maintaining a header
 *   that lists test entry points.
 * - It sets up callbacks for a fixture, which lets us share initialization
 *   *code* between tests. (see test-conform-common.c)
 * - It passes in a shared *data* pointer that is initialised once in main(),
 *   that gets passed to the fixture setup and test functions. (See the
 *   definition in test-conform-common.h)
 */
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

	//g_test_bug_base ("http://bugzilla.openedhand.com/show_bug.cgi?id=%s");

	SeedEngine * eng = seed_init(NULL, NULL);

	shared_state->argc_addr = &argc;
	shared_state->argv_addr = &argv;

	TEST_SIMPLE ("/tests", basica);
	TEST_SIMPLE ("/tests", basicb);

	return g_test_run ();
}

