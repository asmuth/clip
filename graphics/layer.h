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

#include "colour.h"
#include "signaltk.h"
#include "text_shaper.h"
#include "rasterize.h"
#include "image.h"

namespace signaltk {

struct Layer {
  Layer();
  Layer(double width, double height, double rem = 12, double dpi = 96);
  ~Layer();
  Layer(const Layer&) = delete;
  Layer& operator=(const Layer&) = delete;

  Status writeToFile(const std::string& path);
  Status loadFromFile(const std::string& path) const;

  void clear(const Colour& c);

  double width;
  double height;
  double rem;
  double dpi;
  Image pixmap;
  text::TextShaper text_shaper;
  Rasterizer rasterizer;
};

double from_rem(const Layer& l, double v);
double from_px(const Layer& l, double v);
double from_pt(const Layer& l, double v);
double to_pt(const Layer& l, double v);

} // namespace signaltk

