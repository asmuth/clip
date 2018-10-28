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
#ifndef _libstx_RENDERTARGET_H
#define _libstx_RENDERTARGET_H
#include <stdlib.h>
#include <vector>
#include <string>
#include "colour.h"
#include "path.h"
#include "brush.h"

namespace signaltk {
namespace chart {

class Layer {
public:
  virtual ~Layer() {}

  virtual void drawText(
      const std::string& text,
      double x,
      double y,
      const std::string& halign,
      const std::string& valign,
      const std::string& class_name,
      double rotate = 0.0f) = 0;

  virtual void strokePath(
      const PathData* point_data,
      size_t point_count,
      const StrokeStyle& style) = 0;

  void strokeLine(
      double x1,
      double y1,
      double x2,
      double y2,
      const StrokeStyle& style) {
    Path p;
    p.moveTo(x1, y1);
    p.lineTo(x2, y2);
    strokePath(p.data(), p.size(), style);
  }

};


}
}
#endif
