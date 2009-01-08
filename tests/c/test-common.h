
/* Stuff you put in here is setup once in main() and gets passed around to
 * all test functions and fixture setup/teardown functions in the data
 * argument */
typedef struct _TestSharedState
{
  int	 *argc_addr;
  char ***argv_addr;
} TestSharedState;


/* This fixture structure is allocated by glib, and before running each test
 * the test_conform_simple_fixture_setup func (see below) is called to
 * initialise it, and test_conform_simple_fixture_teardown is called when
 * the test is finished. */
typedef struct _TestSimpleFixture
{
  /**/
} TestSimpleFixture;

