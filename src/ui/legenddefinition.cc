/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/ui/legenddefinition.h>

namespace fnordmetric {
namespace ui {

LegendDefinition::LegendDefinition(
    kVerticalPosition vert_pos,
    kHorizontalPosition horiz_pos,
    kPlacement placement,
    const std::string& title) :
    vert_pos_(vert_pos),
    horiz_pos_(horiz_pos),
    placement_(placement),
    title_ (title) {}

}
}
