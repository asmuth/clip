/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <cairo.h>
#include <vector>
#include <string>
#include "rendertarget.h"

namespace signaltk {

class CairoBackend : public chart::RenderTarget {
public:

  CairoBackend();
  ~CairoBackend();

  void writePNG(const char* path);

  void beginChart(
      int width,
      int height,
      const std::string& class_name);

  void finishChart();

  void clear(double r, double g, double b, double a);

  void drawText(
      const std::string& text,
      double x,
      double y,
      const std::string& halign,
      const std::string& valign,
      const std::string& class_name,
      double rotate = 0.0f) override;

  void drawPoint(
      double x,
      double y,
      const std::string& point_type,
      double point_size,
      const std::string& color,
      const std::string& class_name /* = "" */,
      const std::string& label /* = "" */,
      const std::string& series /* = "" */) override;

  void strokePath(
      const PathData* point_data,
      size_t point_count,
      const StrokeStyle& stroke_style) override;

  void beginGroup(const std::string& class_name) override;

  void finishGroup() override;

  uint32_t width() const {
    return width_;
  }

  uint32_t height() const {
    return height_;
  }

protected:
  uint32_t width_;
  uint32_t height_;
  cairo_surface_t* surface_;
  cairo_t* ctx_;
};


} // namespace signaltk
