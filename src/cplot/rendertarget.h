/**
 * This file is part of the "libstx" project
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

namespace stx {
namespace chart {

class RenderTarget {
public:
  virtual ~RenderTarget() {}

  virtual void beginChart(
      int width,
      int height,
      const std::string& class_name) = 0;

  virtual void finishChart() = 0;

  virtual void beginGroup(const std::string& class_name) = 0;
  virtual void finishGroup() = 0;

  virtual void drawLine(
      double x1,
      double y1,
      double x2,
      double y2,
      const std::string& class_name) = 0;

  virtual void drawText(
      const std::string& text,
      double x,
      double y,
      const std::string& halign,
      const std::string& valign,
      const std::string& class_name,
      double rotate = 0.0f) = 0;

  virtual void drawPoint(
      double x,
      double y,
      const std::string& point_type,
      double point_size,
      const std::string& color,
      const std::string& class_name = "",
      const std::string& label = "",
      const std::string& series = "") = 0;

  virtual void drawRect(
      double x,
      double y,
      double width,
      double height,
      const std::string& color,
      const std::string& class_name = "",
      const std::string& label = "",
      const std::string& series = "") = 0;

  virtual void drawPath(
      const std::vector<std::pair<double, double>>& points,
      const std::string& line_style,
      double line_width,
      bool smooth,
      const std::string& color,
      const std::string& class_name = "") = 0;

};


}
}
#endif
