/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "griddefinition.h"

namespace signaltk {
namespace chart {

GridDefinition::GridDefinition(
    kPlacement placement) :
    placement_(placement) {}

GridDefinition::kPlacement GridDefinition::placement() const {
  return placement_;
}

void GridDefinition::addTick(double tick_position) {
  ticks_.push_back(tick_position);
}

const std::vector<double> GridDefinition::ticks() const {
  return ticks_;
}

}
}
