#ifndef _SEED_CAIRO_PATTERN_H
#define _SEED_CAIRO_PATTERN_H

#include <seed.h>
#include <cairo/cairo.h>

SeedClass
seed_get_cairo_pattern_class ();

cairo_pattern_t *
seed_object_to_cairo_pattern (SeedContext ctx, SeedObject obj, SeedException *exception);

SeedObject
seed_object_from_cairo_pattern (SeedContext ctx, cairo_pattern_t *pat);

void
seed_define_cairo_pattern (SeedContext ctx,
			   SeedObject namespace_ref);


#endif
