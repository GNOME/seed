#include <seed.h>
#include <cairo/cairo.h>

#define ENUM_MEMBER(holder, name, value)		\
  seed_object_set_property (ctx, holder, name, seed_value_from_long (ctx, value, NULL))

void
seed_define_cairo_enums (SeedContext ctx,
			 SeedObject namespace_ref)
{
  SeedObject content_holder, format_holder, antialias_holder, fillrule_holder,
    linecap_holder, linejoin_holder, operator_holder;
  
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

  antialias_holder = seed_make_object (ctx, NULL, NULL);
  seed_object_set_property (ctx, namespace_ref, "Antialias", antialias_holder);
  ENUM_MEMBER(antialias_holder, "DEFAULT", CAIRO_ANTIALIAS_DEFAULT);
  ENUM_MEMBER(antialias_holder, "NONE", CAIRO_ANTIALIAS_NONE);
  ENUM_MEMBER(antialias_holder, "GRAY", CAIRO_ANTIALIAS_GRAY);
  ENUM_MEMBER(antialias_holder, "SUBPIXEL", CAIRO_ANTIALIAS_SUBPIXEL);

  fillrule_holder = seed_make_object (ctx, NULL, NULL);
  seed_object_set_property (ctx, namespace_ref, "Fillrule", fillrule_holder);
  ENUM_MEMBER(fillrule_holder, "WINDING", CAIRO_FILL_RULE_WINDING);
  ENUM_MEMBER(fillrule_holder, "EVEN_ODD", CAIRO_FILL_RULE_EVEN_ODD);

  linecap_holder = seed_make_object (ctx, NULL, NULL);
  seed_object_set_property (ctx, namespace_ref, "Linecap", linecap_holder);
  ENUM_MEMBER(linecap_holder, "BUTT", CAIRO_LINE_CAP_BUTT);
  ENUM_MEMBER(linecap_holder, "ROUND", CAIRO_LINE_CAP_ROUND);
  ENUM_MEMBER(linecap_holder, "SQUARE", CAIRO_LINE_CAP_SQUARE);

  linejoin_holder = seed_make_object (ctx, NULL, NULL);
  seed_object_set_property (ctx, namespace_ref, "Linejoin", linejoin_holder);
  ENUM_MEMBER(linejoin_holder, "MITER", CAIRO_LINE_JOIN_MITER);
  ENUM_MEMBER(linejoin_holder, "ROUND", CAIRO_LINE_JOIN_ROUND);
  ENUM_MEMBER(linejoin_holder, "BEVEL", CAIRO_LINE_JOIN_BEVEL);


  operator_holder = seed_make_object (ctx, NULL, NULL);
  seed_object_set_property (ctx, namespace_ref, "Operator", operator_holder);
  ENUM_MEMBER(operator_holder, "CLEAR", CAIRO_OPERATOR_CLEAR);
  ENUM_MEMBER(operator_holder, "SOURCE", CAIRO_OPERATOR_SOURCE);
  ENUM_MEMBER(operator_holder, "OVER", CAIRO_OPERATOR_OVER);
  ENUM_MEMBER(operator_holder, "IN", CAIRO_OPERATOR_IN);
  ENUM_MEMBER(operator_holder, "OUT", CAIRO_OPERATOR_OUT);
  ENUM_MEMBER(operator_holder, "ATOP", CAIRO_OPERATOR_ATOP);
  ENUM_MEMBER(operator_holder, "DEST", CAIRO_OPERATOR_DEST);
  ENUM_MEMBER(operator_holder, "DEST_OVER", CAIRO_OPERATOR_DEST_OVER);
  ENUM_MEMBER(operator_holder, "DEST_IN", CAIRO_OPERATOR_DEST_IN);
  ENUM_MEMBER(operator_holder, "DEST_OUT", CAIRO_OPERATOR_DEST_OUT);
  ENUM_MEMBER(operator_holder, "DEST_ATOP", CAIRO_OPERATOR_DEST_ATOP);
  ENUM_MEMBER(operator_holder, "XOR", CAIRO_OPERATOR_XOR);
  ENUM_MEMBER(operator_holder, "ADD", CAIRO_OPERATOR_ADD);
  ENUM_MEMBER(operator_holder, "SATURATE", CAIRO_OPERATOR_SATURATE);

  
  
}
