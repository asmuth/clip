/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/drawable.h>

namespace fnordmetric {
namespace ui {

void Drawable::setTitle(const std::string& title) {
  printf("TITLE: %s\n", title.c_str());
}

void Drawable::setSubtitle(const std::string& subtitle) {
  printf("SUBTITLE: %s\n", subtitle.c_str());
}

LegendDefinition* Drawable::addLegend(
    LegendDefinition::kVerticalPosition vert_pos,
    LegendDefinition::kHorizontalPosition horiz_pos,
    LegendDefinition::kPlacement placement,
    const std::string& title) {
  canvas_->addLegend(vert_pos, horiz_pos, placement, title);
}

}
}
