#ifndef _SEED_MODULE_H_
#define _SEED_MODULE_H_

#include <seed.h>

#define PLURALIZE_ARG_COUNT(c) \
	((c == 1) ? #c " argument" : #c " arguments")

#define CHECK_ARG_COUNT(name, argnum) \
	if ( argument_count != argnum ) \
	{ \
		char * msg; \
		asprintf(&msg, "%s expected %s got %Zd", name, \
		         PLURALIZE_ARG_COUNT(argnum), argument_count); \
		seed_make_exception (ctx, exception, "ArgumentError", "%s", msg); \
		return seed_make_undefined (ctx); \
	}

#endif
