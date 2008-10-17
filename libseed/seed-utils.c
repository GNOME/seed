/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-utils.c
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


#include "seed-private.h"

#include <string.h>

gchar * seed_value_to_string(JSValueRef obj)
{
	// TODO: there's /definitely/ still a way to crash this (pass it a JSStringRef)
	
	JSValueRef func, str;
	gchar * buf;
	
	if(obj == NULL)
		return NULL;	
	
	if(JSValueIsBoolean(eng->context, obj) || JSValueIsNumber(eng->context, obj))
	{
		buf = malloc(1000 * sizeof(gchar));
		
		snprintf(buf, 1000, "%f", JSValueToNumber(eng->context, obj, NULL));
	}
	else if(JSValueIsNull(eng->context, obj) || JSValueIsUndefined(eng->context, obj))
	{
		buf = strdup("[null]");
	}
	else if(JSValueIsString(eng->context, obj))
	{
		buf = seed_value_to_locale_string(obj);
	}
	else
	{
		func = seed_value_get_property(obj, "toString");
		str = JSObjectCallAsFunction(eng->context, (JSObjectRef)func, (JSObjectRef)obj, 0, NULL, NULL);
		// TODO: exceptions!!
		buf = seed_value_to_locale_string(str);
	}
	
	return buf;
}
