/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-struct.h
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

#ifndef _SEED_STRUCT_H
#define _SEED_STRUCT_H
extern JSClassRef seed_struct_class;

JSObjectRef seed_make_union(gpointer younion, GIBaseInfo * info);
JSObjectRef seed_make_struct(gpointer strukt, GIBaseInfo * info);
gpointer seed_struct_get_pointer(JSValueRef strukt);

#endif
