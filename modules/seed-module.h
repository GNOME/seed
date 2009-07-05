#ifndef _SEED_MODULE_H_
#define _SEED_MODULE_H_

#include "../libseed/seed.h"

// TODO: Move [example sqlite canvas Multiprocessing
//             os sandbox dbus libxml cairo]
//       towards utilization of this header.

// Check that the required number of arguments were passed into a seed callback.
// If this is not true, raise an exception and return null.
// This requires the callback to use "argument_count", "ctx", and "exception"
// as the names of the various function arguments.
#define CHECK_ARG_COUNT(name, argnum) \
	if ( argument_count != argnum ) \
	{ \
		seed_make_exception (ctx, exception, "ArgumentError", \
		                     "wrong number of arguments; expected %s, got %Zd", \
		                     #argnum, argument_count); \
		return seed_make_undefined (ctx); \
	}

// Defines a property on holder for the given enum member
#define DEFINE_ENUM_MEMBER(holder, member) \
	seed_object_set_property(ctx, holder, #member, \
	                         seed_value_from_long(ctx, member, NULL))

// Defines a property on holder for the given enum member, with the given name
#define DEFINE_ENUM_MEMBER_EXT(holder, name, val) \
	seed_object_set_property(ctx, holder, name, \
	                         seed_value_from_long(ctx, val, NULL))

#endif
