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
#include "elements/plot/plot_element.h"

namespace signaltk {

static constexpr const char* ID = "plot";

Status plot_add(Context* ctx) {
  ctx->element_add<PlotConfig>();
  return OK;
}

Status plot_render(Context* ctx) {
  PlotConfig* elem;
  if (auto rc = ctx->element_config(&elem); rc) {
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

