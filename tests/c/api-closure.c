#include "../../libseed/seed.h"
#include "test-common.h"

SeedValue do_some_math(SeedContext ctx,
					   SeedObject function,
					   SeedObject this_object,
					   size_t argument_count,
					   const SeedValue arguments[], SeedException * exception)
{
	g_assert(argument_count == 4);

	guint a = seed_value_to_uint(ctx, arguments[0], NULL);
	gdouble b = seed_value_to_int(ctx, arguments[1], NULL);
	gint c = seed_value_to_int(ctx, arguments[2], NULL);
	gchar *d = seed_value_to_string(ctx, arguments[3], NULL);

	g_assert(d[0] == 'a');

	return seed_value_from_double(ctx, (a + b) / c, NULL);
}

void closures(TestSimpleFixture * fixture, gconstpointer _data)
{
	TestSharedState *state = (TestSharedState *) _data;

	seed_create_function(state->eng->context, "do_some_math",
						 (SeedFunctionCallback) do_some_math,
						 (SeedObject) state->eng->global);

	SeedValue *val = seed_simple_evaluate(state->eng->context,
										  "do_some_math(5, 8.66, -2, 'a')", NULL);
	g_assert(seed_value_to_double(state->eng->context, val, NULL) == -6.5);

	SeedObject *dsm_obj = seed_object_get_property(state->eng->context,
												   (SeedObject) state->eng->
												   global,
												   "do_some_math");
	SeedValue args[4];
	args[0] = seed_value_from_uint(state->eng->context, 5, NULL);
	args[1] = seed_value_from_double(state->eng->context, 8.66, NULL);
	args[2] = seed_value_from_int(state->eng->context, -2, NULL);
	args[3] = seed_value_from_string(state->eng->context, "a", NULL);
	val = seed_object_call(state->eng->context, dsm_obj, NULL, 4, args, NULL);

	g_assert(seed_value_to_double(state->eng->context, val, NULL) == -6.5);
}
