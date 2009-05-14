#include <seed.h>
#include <cairo/cairo.h>

#include "seed-cairo.h"
SeedClass seed_matrix_class;

seed_static_function matrix_funcs[] = {
  {0, 0, 0}
};

SeedValue
seed_value_from_cairo_matrix (SeedContext ctx,
			      const cairo_matrix_t *matrix,
			      SeedException *exception)
{
  SeedValue elems[6];
  
  elems[0] = seed_value_from_double(ctx, matrix->xx, exception);
  elems[1] = seed_value_from_double(ctx, matrix->yx, exception);
  elems[2] = seed_value_from_double(ctx, matrix->xy, exception);
  elems[3] = seed_value_from_double(ctx, matrix->yy, exception);
  elems[4] = seed_value_from_double(ctx, matrix->x0, exception);
  elems[5] = seed_value_from_double(ctx, matrix->y0, exception);
  
  return seed_make_array (ctx, elems, 6, exception);
}

gboolean
seed_value_to_cairo_matrix (SeedContext ctx,
			    SeedValue value,
			    cairo_matrix_t *matrix,
			    SeedException *exception)
{
  if (!seed_value_is_object (ctx, value))
    return FALSE;
  
  matrix->xx = seed_value_to_double (ctx, seed_object_get_property_at_index (ctx, (SeedObject) value, 0, exception), exception);
  matrix->yx = seed_value_to_double (ctx, seed_object_get_property_at_index (ctx, (SeedObject) value, 1, exception), exception);
  matrix->xy = seed_value_to_double (ctx, seed_object_get_property_at_index (ctx, (SeedObject) value, 2, exception), exception);
  matrix->yy = seed_value_to_double (ctx, seed_object_get_property_at_index (ctx, (SeedObject) value, 3, exception), exception);
  matrix->x0 = seed_value_to_double (ctx, seed_object_get_property_at_index (ctx, (SeedObject) value, 4, exception), exception);
  matrix->y0 = seed_value_to_double (ctx, seed_object_get_property_at_index (ctx, (SeedObject) value, 5, exception), exception);
  
  return TRUE;
}

void
seed_define_cairo_matrix (SeedContext ctx,
			  SeedObject namespace_ref)
{
  seed_class_definition matrix_def = seed_empty_class;
  
  matrix_def.class_name = "Matrix";
  matrix_def.static_functions = matrix_funcs;
  seed_matrix_class = seed_create_class (&matrix_def);
  
  seed_object_set_property (ctx, namespace_ref, "Matrix", seed_make_object (ctx, seed_matrix_class, NULL));
}
