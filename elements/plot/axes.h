/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <utility>
#include <string>
#include <vector>
#include <graphics/layer.h>
#include <graphics/viewport.h>

namespace signaltk {

static const double kDefaultLabelPaddingVertREM = 0.8f;
static const double kDefaultLabelPaddingHorizREM = 1.0f;
static const double kDefaultTickLengthREM = 0.4f;

enum class AxisPosition {
  TOP,
  RIGHT,
  BOTTOM,
  LEFT,
  CENTER_HORIZ,
  CENTER_VERT
};

enum class AxisMode {
  DISABLED,
  AUTO,
  MANUAL
};

enum class AxisLabelPlacement {
  LEFT,
  RIGHT,
  TOP,
  BOTTOM,
  OFF
};

struct AxisDefinition {
  AxisDefinition();
  AxisPosition position;
  AxisMode mode;
  std::string title;
  std::vector<double> ticks;
  std::vector<std::pair<double, std::string>> labels;
  AxisLabelPlacement label_placement;
  double label_padding_horiz_rem;
  double label_padding_vert_rem;
  double tick_length_rem;
};

Status plot_axis_add(Context* ctx, AxisPosition pos);
Status plot_axis_addtick(Context* ctx, float offset);
Status plot_axis_addlabel(Context* ctx, float offset, const char* label);
Status plot_axis_setauto(Context* ctx);
Status plot_axis_setmanual(Context* ctx);

Status plot_render_axis(Context* ctx, int idx);

} // namespace signaltk

