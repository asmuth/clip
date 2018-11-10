/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "elements/context.h"
#include "elements/plot/plot_layout.h"
#include "elements/plot/axes.h"

namespace signaltk {

Status plot_render(Context* ctx) {
  double padding = 80;

  // draw left axis
  {
    AxisDefinition axis;
    axis.label_placement = AxisDefinition::LABELS_LEFT;
    axis.addTick(0.0);
    axis.addTick(0.2);
    axis.addTick(0.4);
    axis.addTick(0.6);
    axis.addTick(0.8);
    axis.addTick(1.0);
    axis.addLabel(0.0, "x");
    axis.addLabel(0.2, "x");
    axis.addLabel(0.4, "x");
    axis.addLabel(0.6, "x");
    axis.addLabel(0.8, "x");
    axis.addLabel(1.0, "x");
    auto rc = renderAxisVertical(axis, padding, padding, ctx->frame.height - padding, &ctx->frame);

    if (!rc) {
      return rc;
    }
  }

  return OK;
}

} // namespace signaltk

