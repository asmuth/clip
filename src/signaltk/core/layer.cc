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

Layer::Layer() : width(0), height(0), surface(nullptr), ctx(nullptr) {}

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

void Layer::clear(const Colour& c) {
  cairo_set_source_rgba(ctx, c.red(), c.green(), c.blue(), c.alpha());
  cairo_rectangle(ctx, 0, 0, width, height);
  cairo_fill(ctx);
}

bool Layer::loadPNG(const char* path) {
  if (surface || ctx) {
    return false;
  }

  surface = cairo_image_surface_create_from_png(path);
  if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
    return false;
  }

  ctx = cairo_create(surface);

  double extent[4];
  cairo_clip_extents(ctx, extent + 0, extent + 1, extent + 2, extent + 3);
  width = extent[2] - extent[0];
  height = extent[3] - extent[1];

  return true;
}

bool Layer::writePNG(const char* path) const {
  if (!surface || !ctx) {
    return false;
  }

  auto rc = cairo_surface_write_to_png(surface, path);
  return rc == CAIRO_STATUS_SUCCESS;
}

} // namespace signaltk

