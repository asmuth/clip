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

Status plot_add(Context* ctx) {
  stack_add(ctx, std::make_unique<PlotConfig>());
  return OK;
}

Status plot_render(Context* ctx) {
  PlotConfig* elem;
  if (auto rc = stack_head_config(ctx, &elem); rc) {
    return rc;
  }

  // draw axes
  for (size_t i = 0; i < elem->axes.size(); ++i) {
    if (auto rc = plot_render_axis(ctx, i); rc) {
      return rc;
    }
  }

  return OK;
}

} // namespace signaltk

