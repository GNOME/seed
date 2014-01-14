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

#ifndef _SEED_MODULE_H_
#define _SEED_MODULE_H_

#include "../libseed/seed.h"

// TODO: Move [example sqlite canvas Multiprocessing
//             os sandbox dbus libxml cairo]
//       towards utilization of this header.

/**
 * CHECK_ARG_COUNT:
 * @name: The name of the function being called from, pretty-printed
 * @argnum: The number of arguments which should be passed into the function
 *
 * Check that the required number of arguments were passed into a
 * #SeedFunctionCallback. If this is not true, raise an exception and
 * return %NULL. This requires the callback to use "argument_count",
 * "ctx", and "exception" as the names of the various function arguments.
 *
 * @name should be of form "namespace.function_name"
 *
 * At the moment, there is no way to specify more than one acceptable
 * argument count.
 *
 */

#define CHECK_ARG_COUNT(name, argnum) \
	if ( argument_count != argnum ) \
	{ \
		seed_make_exception (ctx, exception, "ArgumentError", \
		                     "wrong number of arguments; expected %s, got %zd", \
		                     #argnum, argument_count); \
		return seed_make_undefined (ctx); \
	}

/**
 * DEFINE_ENUM_MEMBER:
 * @holder: The object on which to define the enum member
 * @member: The enum member, as it is named in C
 *
 * Defines a property on @holder which is named the same as @member, and
 * is assigned the value that @member has in C.
 *
 * This macro works for defining properties from constants and
 * &num;defines as well.
 *
 */
#define DEFINE_ENUM_MEMBER(holder, member) \
	seed_object_set_property(ctx, holder, #member, \
	                         seed_value_from_long(ctx, member, NULL))

/**
 * DEFINE_ENUM_MEMBER_EXT:
 * @holder: The object on which to define the enum member
 * @name: The enum member, as it should be named in JavaScript
 * @val: The enum member, as it is named in C
 *
 * Defines a property on @holder which is named @name, and is assigned the
 * value that @member has in C. This allows for an override of the enum
 * member's name, most often to remove a common prefix. For example, to declare
 * a property named VERSION_MAJOR on the namespace from mfpr's version
 * constant MPFR_VERSION_MAJOR:
 *
 * <programlisting>
 * DEFINE_ENUM_MEMBER_EXT(ns, "VERSION_MAJOR", MPFR_VERSION_MAJOR);
 * </programlisting>
 *
 */
#define DEFINE_ENUM_MEMBER_EXT(holder, name, val) \
	seed_object_set_property(ctx, holder, name, \
	                         seed_value_from_long(ctx, val, NULL))

#endif
