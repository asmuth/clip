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

void CairoBackend::beginChart(
    int width,
    int height,
    const std::string& class_name) {}

void CairoBackend::finishChart() {}

void CairoBackend::clear(double r, double g, double b, double a) {
  cairo_set_source_rgba(ctx_, r, g, b, a);
  cairo_rectangle(ctx_, 0, 0, width_, height_);
  cairo_fill(ctx_);
}

void CairoBackend::drawRect(
    double x,
    double y,
    double width,
    double height,
    const std::string& color,
    const std::string& class_name,
    const std::string& label,
    const std::string& series) {}

void CairoBackend::drawLine(
    double x1,
    double y1,
    double x2,
    double y2,
    const std::string& class_name) {
  cairo_set_source_rgb(ctx_, 0, 0, 0);
  cairo_set_line_width(ctx_, 0.5);

  cairo_move_to(ctx_, x1, y1);
  cairo_line_to(ctx_, x2, y2);

  cairo_stroke(ctx_);
}

void CairoBackend::drawText(
    const std::string& text,
    double x,
    double y,
    const std::string& halign,
    const std::string& valign,
    const std::string& class_name,
    double rotate) {}

void CairoBackend::drawPoint(
    double x,
    double y,
    const std::string& point_type,
    double point_size,
    const std::string& color,
    const std::string& class_name /* = "" */,
    const std::string& label /* = "" */,
    const std::string& series /* = "" */) {}

void CairoBackend::drawPath(
    const std::vector<std::pair<double, double>>& points,
    const std::string& line_style,
    double line_width,
    bool smooth,
    const std::string& color,
    const std::string& class_name) {
  if (points.size() < 2) {
    return;
  }

  cairo_set_source_rgba(ctx_, 0, 0, 0, 1);
  cairo_set_line_width(ctx_, line_width);

  cairo_move_to(ctx_, points[0].first, points[0].second);
  for (size_t i = 1; i < points.size(); ++i) {
    cairo_line_to(ctx_, points[i].first, points[i].second);
  }

  cairo_stroke(ctx_);
}

void CairoBackend::beginGroup(const std::string& class_name) {}

void CairoBackend::finishGroup() {}

} // namespace signaltk
