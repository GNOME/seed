#ifndef _SEED_CAIRO_SURFACE_H
#define _SEED_CAIRO_SURFACE_H

SeedClass seed_get_cairo_surface_class ();

void seed_define_cairo_surface (SeedContext ctx, SeedObject namespace_ref);

cairo_surface_t *seed_object_to_cairo_surface (SeedContext ctx, SeedObject obj);
SeedObject seed_object_from_cairo_surface (SeedContext ctx, cairo_surface_t *surf);

#endif
