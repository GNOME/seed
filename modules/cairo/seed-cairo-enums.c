#include <seed.h>
#include <cairo/cairo.h>

#define ENUM_MEMBER(holder, name, value)		\
  seed_object_set_property (ctx, holder, name, seed_value_from_long (ctx, value, NULL))

void
seed_define_cairo_enums (SeedContext ctx,
			 SeedObject namespace_ref)
{
  SeedObject content_holder, format_holder;
  
  content_holder = seed_make_object (ctx, NULL, NULL);
  seed_object_set_property (ctx, namespace_ref, "Content", content_holder);
  ENUM_MEMBER(content_holder, "COLOR", CAIRO_CONTENT_COLOR);
  ENUM_MEMBER(content_holder, "ALPHA", CAIRO_CONTENT_ALPHA);
  ENUM_MEMBER(content_holder, "COLOR_ALPHA", CAIRO_CONTENT_COLOR_ALPHA);

  format_holder = seed_make_object (ctx, NULL, NULL);
  seed_object_set_property (ctx, namespace_ref, "Format", format_holder);
  ENUM_MEMBER(format_holder, "ARGB32", CAIRO_FORMAT_ARGB32);
  ENUM_MEMBER(format_holder, "RGB23", CAIRO_FORMAT_RGB24);
  ENUM_MEMBER(format_holder, "A8", CAIRO_FORMAT_A8);
  ENUM_MEMBER(format_holder, "A1", CAIRO_FORMAT_A1);
}
