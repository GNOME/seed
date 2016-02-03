/* -*- mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 2; -*- */

/*
 * This file is part of Seed, the GObject Introspection<->Javascript bindings.
 *
 * Seed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2 of
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

#include <seed.h>
#include <cairo/cairo.h>
#include "seed-cairo.h"

#define ENUM_MEMBER(holder, name, value)                                       \
    seed_object_set_property(ctx, holder, name,                                \
                             seed_value_from_long(ctx, value, NULL))

SeedValue
seed_cairo_status_to_string(SeedContext ctx,
                            SeedObject function,
                            SeedObject this_object,
                            gsize argument_count,
                            const SeedValue arguments[],
                            SeedException* exception)
{
    cairo_status_t status;

    if (argument_count != 1) {
        EXPECTED_EXCEPTION("status_to_string", "1 argument");
    }
    status = seed_value_to_long(ctx, arguments[0], exception);

    return seed_value_from_string(ctx, cairo_status_to_string(status),
                                  exception);
}

void
seed_define_cairo_enums(SeedContext ctx, SeedObject namespace_ref)
{
    SeedObject content_holder, format_holder, antialias_holder, fillrule_holder,
      linecap_holder, linejoin_holder, operator_holder, status_holder;

    content_holder = seed_make_object(ctx, NULL, NULL);
    seed_object_set_property(ctx, namespace_ref, "Content", content_holder);
    ENUM_MEMBER(content_holder, "COLOR", CAIRO_CONTENT_COLOR);
    ENUM_MEMBER(content_holder, "ALPHA", CAIRO_CONTENT_ALPHA);
    ENUM_MEMBER(content_holder, "COLOR_ALPHA", CAIRO_CONTENT_COLOR_ALPHA);

    format_holder = seed_make_object(ctx, NULL, NULL);
    seed_object_set_property(ctx, namespace_ref, "Format", format_holder);
    ENUM_MEMBER(format_holder, "ARGB32", CAIRO_FORMAT_ARGB32);
    ENUM_MEMBER(format_holder, "RGB23", CAIRO_FORMAT_RGB24);
    ENUM_MEMBER(format_holder, "A8", CAIRO_FORMAT_A8);
    ENUM_MEMBER(format_holder, "A1", CAIRO_FORMAT_A1);

    antialias_holder = seed_make_object(ctx, NULL, NULL);
    seed_object_set_property(ctx, namespace_ref, "Antialias", antialias_holder);
    ENUM_MEMBER(antialias_holder, "DEFAULT", CAIRO_ANTIALIAS_DEFAULT);
    ENUM_MEMBER(antialias_holder, "NONE", CAIRO_ANTIALIAS_NONE);
    ENUM_MEMBER(antialias_holder, "GRAY", CAIRO_ANTIALIAS_GRAY);
    ENUM_MEMBER(antialias_holder, "SUBPIXEL", CAIRO_ANTIALIAS_SUBPIXEL);

    fillrule_holder = seed_make_object(ctx, NULL, NULL);
    seed_object_set_property(ctx, namespace_ref, "Fillrule", fillrule_holder);
    ENUM_MEMBER(fillrule_holder, "WINDING", CAIRO_FILL_RULE_WINDING);
    ENUM_MEMBER(fillrule_holder, "EVEN_ODD", CAIRO_FILL_RULE_EVEN_ODD);

    linecap_holder = seed_make_object(ctx, NULL, NULL);
    seed_object_set_property(ctx, namespace_ref, "Linecap", linecap_holder);
    ENUM_MEMBER(linecap_holder, "BUTT", CAIRO_LINE_CAP_BUTT);
    ENUM_MEMBER(linecap_holder, "ROUND", CAIRO_LINE_CAP_ROUND);
    ENUM_MEMBER(linecap_holder, "SQUARE", CAIRO_LINE_CAP_SQUARE);

    linejoin_holder = seed_make_object(ctx, NULL, NULL);
    seed_object_set_property(ctx, namespace_ref, "Linejoin", linejoin_holder);
    ENUM_MEMBER(linejoin_holder, "MITER", CAIRO_LINE_JOIN_MITER);
    ENUM_MEMBER(linejoin_holder, "ROUND", CAIRO_LINE_JOIN_ROUND);
    ENUM_MEMBER(linejoin_holder, "BEVEL", CAIRO_LINE_JOIN_BEVEL);

    operator_holder = seed_make_object(ctx, NULL, NULL);
    seed_object_set_property(ctx, namespace_ref, "Operator", operator_holder);
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

    status_holder = seed_make_object(ctx, NULL, NULL);
    seed_object_set_property(ctx, namespace_ref, "Status", status_holder);
    ENUM_MEMBER(status_holder, "SUCCESS", CAIRO_STATUS_SUCCESS);
    ENUM_MEMBER(status_holder, "NO_MEMORY", CAIRO_STATUS_NO_MEMORY);
    ENUM_MEMBER(status_holder, "INVALID_RESTORE", CAIRO_STATUS_INVALID_RESTORE);
    ENUM_MEMBER(status_holder, "INVALID_POP_GROUP",
                CAIRO_STATUS_INVALID_POP_GROUP);
    ENUM_MEMBER(status_holder, "NO_CURRENT_POINT",
                CAIRO_STATUS_NO_CURRENT_POINT);
    ENUM_MEMBER(status_holder, "INVALID_MATRIX", CAIRO_STATUS_INVALID_MATRIX);
    ENUM_MEMBER(status_holder, "INVALID_STATUS", CAIRO_STATUS_INVALID_STATUS);
    ENUM_MEMBER(status_holder, "NULL_POINTER", CAIRO_STATUS_NULL_POINTER);
    ENUM_MEMBER(status_holder, "INVALID_STRING", CAIRO_STATUS_INVALID_STRING);
    ENUM_MEMBER(status_holder, "INVALID_PATH_DATA",
                CAIRO_STATUS_INVALID_PATH_DATA);
    ENUM_MEMBER(status_holder, "READ_ERROR", CAIRO_STATUS_READ_ERROR);
    ENUM_MEMBER(status_holder, "WRITE_ERROR", CAIRO_STATUS_WRITE_ERROR);
    ENUM_MEMBER(status_holder, "SURFACE_FINISHED",
                CAIRO_STATUS_SURFACE_FINISHED);
    ENUM_MEMBER(status_holder, "SURFACE_TYPE_MISMATCH",
                CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
    ENUM_MEMBER(status_holder, "PATTERN_TYPE_MISMATCH",
                CAIRO_STATUS_PATTERN_TYPE_MISMATCH);
    ENUM_MEMBER(status_holder, "INVALID_CONTENT", CAIRO_STATUS_INVALID_CONTENT);
    ENUM_MEMBER(status_holder, "INVALID_FORMAT", CAIRO_STATUS_INVALID_FORMAT);
    ENUM_MEMBER(status_holder, "INVALID_VISUAL", CAIRO_STATUS_INVALID_VISUAL);
    ENUM_MEMBER(status_holder, "FILE_NOT_FOUND", CAIRO_STATUS_FILE_NOT_FOUND);
    ENUM_MEMBER(status_holder, "INVALID_DASH", CAIRO_STATUS_INVALID_DASH);
    ENUM_MEMBER(status_holder, "INVALID_DSC_COMMENT",
                CAIRO_STATUS_INVALID_DSC_COMMENT);
    ENUM_MEMBER(status_holder, "INVALID_INDEX", CAIRO_STATUS_INVALID_INDEX);
    ENUM_MEMBER(status_holder, "CLIP_NOT_REPRESENTABLE",
                CAIRO_STATUS_CLIP_NOT_REPRESENTABLE);
    ENUM_MEMBER(status_holder, "TEMP_FILE_ERROR", CAIRO_STATUS_TEMP_FILE_ERROR);
    ENUM_MEMBER(status_holder, "INVALID_STRIDE", CAIRO_STATUS_INVALID_STRIDE);
    ENUM_MEMBER(status_holder, "FONT_TYPE_MISMATCH",
                CAIRO_STATUS_FONT_TYPE_MISMATCH);
    ENUM_MEMBER(status_holder, "USER_FONT_IMMUTABLE",
                CAIRO_STATUS_USER_FONT_IMMUTABLE);
    ENUM_MEMBER(status_holder, "USER_FONT_ERROR", CAIRO_STATUS_USER_FONT_ERROR);
    ENUM_MEMBER(status_holder, "NEGATIVE_COUNT", CAIRO_STATUS_NEGATIVE_COUNT);
    ENUM_MEMBER(status_holder, "INVALID_CLUSTERS",
                CAIRO_STATUS_INVALID_CLUSTERS);
    ENUM_MEMBER(status_holder, "INVALID_SLANT", CAIRO_STATUS_INVALID_SLANT);
    ENUM_MEMBER(status_holder, "INVALID_WEIGHT", CAIRO_STATUS_INVALID_WEIGHT);

    seed_create_function(ctx, "to_string", seed_cairo_status_to_string,
                         status_holder);
}
