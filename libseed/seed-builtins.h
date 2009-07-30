/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
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

#ifndef _SEED_BUILTINS_H_
#define _SEED_BUILTINS_H_

#include "seed-private.h"

extern JSValueRef seed_print_ref;

void seed_init_builtins (SeedEngine * local_eng, gint * argc, gchar *** argv);

extern JSClassRef seed_argv_class;
typedef struct _SeedArgvPrivates
{
  gchar **argv;
  gint argc;
} SeedArgvPrivates;


#endif
