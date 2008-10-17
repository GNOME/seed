/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-types.h
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 *
 * libseed is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libseed is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _SEED_TYPES_H
#define _SEED_TYPES_H

#include "seed-private.h"

gchar * seed_value_to_locale_string(SeedValue val);
SeedValue seed_value_from_gvalue(GValue * gval);
SeedValue seed_value_get_property(SeedValue val,
				  const char * name);

gboolean seed_value_set_property(JSObjectRef object,
				 const char * name, JSValueRef value);
gboolean seed_gvalue_from_seed_value(SeedValue val,
				     GType type,
				     GValue * gval);
gboolean seed_gi_supports_type(GITypeInfo * type_info);
gboolean seed_gi_make_argument(SeedValue value, GITypeInfo *type_info,
			       GArgument * arg);
JSValueRef seed_gi_argument_make_js(GArgument * arg, GITypeInfo *type_info);

GType seed_gi_type_to_gtype(GITypeInfo *type_info, GITypeTag tag);

#endif

