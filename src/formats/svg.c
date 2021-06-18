// SPDX-License-Identifier: MIT
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

//
// SVG image format support
//

#include "config.h"
#ifndef HAVE_LIBRSVG
#error Invalid build configuration
#endif

#include "../image.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <librsvg/rsvg.h>

// SVG signature
static const uint8_t signature[] = { '<' };

// SVG loader implementation
struct image* load_svg(const uint8_t* data, size_t size)
{
    RsvgHandle* svg;
    RsvgDimensionData dim;
    GError* err = NULL;
    cairo_t* cr;
    struct image* img = NULL;

    // check signature
    if (size < sizeof(signature) || memcmp(data, signature, sizeof(signature))) {
        return NULL;
    }

    svg = rsvg_handle_new_from_data(data, size, &err);
    if (!svg) {
        fprintf(stderr, "Invalid SVG format");
        if (err && err->message) {
            fprintf(stderr, ": %s\n", err->message);
        } else {
            fprintf(stderr, "\n");
        }
        return NULL;
    }

    rsvg_handle_get_dimensions(svg, &dim);

    // create image instance
    img = create_image(CAIRO_FORMAT_ARGB32, dim.width, dim.height);
    if (!img) {
        g_object_unref(svg);
        return NULL;
    }
    set_image_meta(img, "SVG");

    // render svg to surface
    cr = cairo_create(img->surface);
    rsvg_handle_render_cairo(svg, cr);
    cairo_destroy(cr);

    g_object_unref(svg);

    return img;
}
