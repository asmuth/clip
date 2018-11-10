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
#include "signaltk.h"
#include "axes.h"
#include "plot_domain.h"
#include <memory>

namespace signaltk {

struct PlotConfig {
  PlotDomain x_domain;
  double x_min;
  double x_max;
  PlotDomain y_domain;
  double y_min;
  double y_max;
  std::vector<std::unique_ptr<AxisDefinition>> axes;
};

} // namespace signaltk

