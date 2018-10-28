/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "layer.h"

namespace signaltk {

Layer::Layer(
  uint32_t w,
  uint32_t h) :
  width(w),
  height(h) {
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  ctx = cairo_create(surface);
}

Layer::~Layer() {
  cairo_destroy(ctx);
  cairo_surface_destroy(surface);
}

void Layer::clear(double r, double g, double b, double a) {
  cairo_set_source_rgba(ctx, r, g, b, a);
  cairo_rectangle(ctx, 0, 0, width, height);
  cairo_fill(ctx);
}

void Layer::writePNG(const char* path) {
  cairo_surface_write_to_png(surface, path);
}

} // namespace signaltk

