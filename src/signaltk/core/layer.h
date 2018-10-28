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
#pragma once
#include <stdlib.h>
#include <vector>
#include <string>
#include <cairo.h>
#include "colour.h"

namespace signaltk {

struct Layer {
  Layer(uint32_t width, uint32_t height);
  ~Layer();
  Layer(const Layer&) = delete;
  Layer& operator=(const Layer&) = delete;

  void writePNG(const char* path);

  void clear(double r, double g, double b, double a);

  const uint32_t width;
  const uint32_t height;
  cairo_surface_t* surface;
  cairo_t* ctx;
};

} // namespace signaltk

