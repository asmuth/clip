/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * signaltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "brush.h"

namespace signaltk {

void strokePath(
    Layer* layer,
    const PathData* point_data,
    size_t point_count,
    const StrokeStyle& style) {
  auto ctx = layer->ctx;

  if (point_count < 2) {
    return;
  }

  cairo_set_source_rgba(
      ctx,
      style.colour.red(),
      style.colour.green(),
      style.colour.blue(),
      style.colour.alpha());

  cairo_set_line_width(ctx, style.line_width);

  for (size_t i = 0; i < point_count; ++i) {
    const auto& cmd = point_data[i];
    switch (cmd.command) {
      case PathCommand::MOVE_TO:
        cairo_move_to(ctx, cmd[0], cmd[1]);
        break;
      case PathCommand::LINE_TO:
        cairo_line_to(ctx, cmd[0], cmd[1]);
        break;
    }
  }

  cairo_stroke(ctx);
}

void strokeLine(
    Layer* layer,
    double x1,
    double y1,
    double x2,
    double y2,
    const StrokeStyle& style) {
  Path p;
  p.moveTo(x1, y1);
  p.lineTo(x2, y2);
  strokePath(layer, p.data(), p.size(), style);
}

} // namespace signaltk

