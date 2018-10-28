/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include "backend_cairo.h"

namespace signaltk {

CairoBackend::CairoBackend() :
  width_(1000),
  height_(600) {
  surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
  ctx_ = cairo_create(surface_);
}

CairoBackend::~CairoBackend() {
  cairo_destroy(ctx_);
  cairo_surface_destroy(surface_);
}

void CairoBackend::writePNG(const char* path) {
  cairo_surface_write_to_png(surface_, path);
}

void CairoBackend::clear(double r, double g, double b, double a) {
  cairo_set_source_rgba(ctx_, r, g, b, a);
  cairo_rectangle(ctx_, 0, 0, width_, height_);
  cairo_fill(ctx_);
}

void CairoBackend::drawText(
    const std::string& text,
    double x,
    double y,
    const std::string& halign,
    const std::string& valign,
    const std::string& class_name,
    double rotate) {
  // here be dragons
}

void CairoBackend::strokePath(
    const PathData* point_data,
    size_t point_count,
    const StrokeStyle& style) {
  if (point_count < 2) {
    return;
  }

  cairo_set_source_rgba(
      ctx_,
      style.colour.red(),
      style.colour.green(),
      style.colour.blue(),
      style.colour.alpha());

  cairo_set_line_width(ctx_, style.line_width);

  for (size_t i = 0; i < point_count; ++i) {
    const auto& cmd = point_data[i];
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        cairo_move_to(ctx_, cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        cairo_line_to(ctx_, cmd[0], cmd[1]);
        break;
    }
  }

  cairo_stroke(ctx_);
}

} // namespace signaltk
