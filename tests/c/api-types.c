#include "../../libseed/seed.h"
#include "test-common.h"

void basic_types(TestSimpleFixture * fixture, gconstpointer _data)
{
	TestSharedState * state = (TestSharedState *)_data;
	
	// bool to/from JS equality
	
	gboolean bool_test_in = TRUE;
	SeedValue * bool_test = seed_value_from_boolean(state->eng->context,
													bool_test_in, NULL);
	gboolean bool_test_out = seed_value_to_boolean(state->eng->context,
												   bool_test, NULL);
	
	g_assert(bool_test_in == bool_test_out);
	
	// uint to/from JS equality
	
	guint uint_test_in = 2946623;
	SeedValue * uint_test = seed_value_from_uint(state->eng->context,
												 uint_test_in, NULL);
	guint uint_test_out = seed_value_to_uint(state->eng->context,
											 uint_test, NULL);
	
	g_assert(uint_test_in == uint_test_out);
	
	// int to/from JS equality
	
	gint int_test_in = -54374;
	SeedValue * int_test = seed_value_from_int(state->eng->context,
											   int_test_in, NULL);
	gint int_test_out = seed_value_to_int(state->eng->context,
										  int_test, NULL);
	
	g_assert(int_test_in == int_test_out);
	
	// char to/from JS equality
	
	gchar char_test_in = -126;
	SeedValue * char_test = seed_value_from_char(state->eng->context,
												 char_test_in, NULL);
	gchar char_test_out = seed_value_to_char(state->eng->context,
											 char_test, NULL);
	
	g_assert(char_test_in == char_test_out);
	
	// uchar to/from JS equality
	
	guchar uchar_test_in = 250;
	SeedValue * uchar_test = seed_value_from_uchar(state->eng->context,
												   uchar_test_in, NULL);
	guchar uchar_test_out = seed_value_to_uchar(state->eng->context,
												uchar_test, NULL);
	
	g_assert(uchar_test_in == uchar_test_out);
	
	// float to/from JS equality
	
	gfloat float_test_in = 1.618;
	SeedValue * float_test = seed_value_from_float(state->eng->context,
												   float_test_in, NULL);
	gfloat float_test_out = seed_value_to_float(state->eng->context,
												float_test, NULL);
	
	g_assert(float_test_in == float_test_out);

}

