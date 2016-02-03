#ifndef _SEED_MATRIX_H
#define _SEED_MATRIX_H
#include <seed.h>
#include <cairo/cairo.h>

void seed_define_cairo_matrix(SeedContext ctx, SeedObject namespace_ref);

gboolean seed_value_to_cairo_matrix(SeedContext ctx,
                                    SeedValue value,
                                    cairo_matrix_t* matrix,
                                    SeedException* exception);

SeedValue seed_value_from_cairo_matrix(SeedContext ctx,
                                       const cairo_matrix_t* matrix,
                                       SeedException* exception);

#endif
