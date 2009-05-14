#include <seed.h>
#include <cairo/cairo.h>
#include "seed-cairo-surface.h"
SeedEngine *eng;
SeedObject namespace_ref;

cairo_user_data_key_t *
seed_get_cairo_key ()
{
  static cairo_user_data_key_t foobaz;
  
  return &foobaz;
}

void
seed_cairo_destroy_func (void *obj)
{
  SeedObject object = (SeedObject)obj;
  seed_object_set_private (object, NULL);
}

SeedObject
seed_module_init(SeedEngine * local_eng)
{
  eng = local_eng;
  namespace_ref = seed_make_object (eng->context, NULL, NULL);
  
  // Temporary hack until API changes.
  seed_value_protect (eng->context, namespace_ref);
  seed_define_cairo_surface (eng->context, namespace_ref);
  
  return namespace_ref;
}
