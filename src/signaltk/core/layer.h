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
  Layer();
  Layer(uint32_t width, uint32_t height, uint32_t dpi = 100);
  ~Layer();
  Layer(const Layer&) = delete;
  Layer& operator=(const Layer&) = delete;

  bool loadPNG(const char* path);
  bool writePNG(const char* path) const;
  bool writePNG(const std::string& path) const;

  void clear(const Colour& c);

  uint32_t width;
  uint32_t height;
  uint32_t dpi;
  cairo_surface_t* surface;
  cairo_t* ctx;
};

} // namespace signaltk

