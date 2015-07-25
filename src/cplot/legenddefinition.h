/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_UI_LEGENDDEFINITION_H
#define _libstx_UI_LEGENDDEFINITION_H
#include <tuple>
#include <string>
#include <vector>

namespace stx {
namespace chart {

class LegendDefinition {
public:

  enum kVerticalPosition {
    LEGEND_TOP = 0,
    LEGEND_BOTTOM = 1
  };

  enum kHorizontalPosition {
    LEGEND_LEFT = 0,
    LEGEND_RIGHT = 1
  };

  enum kPlacement {
    LEGEND_INSIDE = 0,
    LEGEND_OUTSIDE = 1
  };

  /**
   * Create a new legend definition
   */
  LegendDefinition(
      kVerticalPosition vert_pos,
      kHorizontalPosition horiz_pos,
      kPlacement placement,
      const std::string& title);

  const std::string title() const;
  kVerticalPosition verticalPosition() const;
  kHorizontalPosition horizontalPosition() const;
  kPlacement placement() const;

  void addEntry(
      const std::string& name,
      const std::string& color,
      const std::string& shape = "circle");

  const std::vector<std::tuple<std::string, std::string, std::string>>
      entries() const;

protected:
  kVerticalPosition vert_pos_;
  kHorizontalPosition horiz_pos_;
  kPlacement placement_;
  const std::string title_;
  std::vector<std::tuple<std::string, std::string, std::string>> entries_;
};

}
}
#endif
