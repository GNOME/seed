#include <seed.h>
#include <cairo/cairo.h>

#define ENUM_MEMBER(holder, name, value)		\
  seed_object_set_property (ctx, holder, name, seed_value_from_long (ctx, value, NULL))

void
seed_define_cairo_enums (SeedContext ctx,
			 SeedObject namespace_ref)
{
  SeedObject content_holder;
  
  content_holder = seed_make_object (ctx, NULL, NULL);
  seed_object_set_property (ctx, namespace_ref, "Content", content_holder);
  ENUM_MEMBER(content_holder, "COLOR", CAIRO_CONTENT_COLOR);
  ENUM_MEMBER(content_holder, "ALPHA", CAIRO_CONTENT_ALPHA);
  ENUM_MEMBER(content_holder, "COLOR_ALPHA", CAIRO_CONTENT_COLOR_ALPHA);
}
