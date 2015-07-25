/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "cplot/legenddefinition.h"

namespace stx {
namespace chart {

LegendDefinition::LegendDefinition(
    kVerticalPosition vert_pos,
    kHorizontalPosition horiz_pos,
    kPlacement placement,
    const std::string& title) :
    vert_pos_(vert_pos),
    horiz_pos_(horiz_pos),
    placement_(placement),
    title_ (title) {}

const std::string LegendDefinition::title() const {
  return title_;
}

LegendDefinition::kVerticalPosition LegendDefinition::verticalPosition() 
    const {
  return vert_pos_;
}

LegendDefinition::kHorizontalPosition LegendDefinition::horizontalPosition() 
    const {
  return horiz_pos_;
}

LegendDefinition::kPlacement LegendDefinition::placement() const {
  return placement_;
}

void LegendDefinition::addEntry(
    const std::string& name,
    const std::string& color,
    const std::string& shape /* = "circle" */) {
  entries_.emplace_back(name, color, shape);
}

const std::vector<std::tuple<std::string, std::string, std::string>>
    LegendDefinition::entries() const {
  return entries_;
}

}
}
