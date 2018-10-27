/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_CANVAS_H
#define _libstx_CANVAS_H
#include <memory>
#include <vector>
#include <string>
#include <tuple>
#include "axisdefinition.h"
#include "drawable.h"
#include "legenddefinition.h"

namespace signaltk {
namespace chart {
class RenderTarget;

class Legend {
public:
  static const int kLegendLabelPadding = 20; // FIXPAUL make configurable
  static const int kLegendLineHeight = 20; // FIXPAUL make configurable
  static const int kLegendInsideVertPadding = 10;
  static const int kLegendInsideHorizPadding = 15;
  static const int kLegendOutsideVertPadding = 10;
  static const int kLegendOutsideHorizPadding = 25;
  static const int kLegendPointY = 6;
  static const int kLegendPointWidth = 8;
  static const int kLegendPointSize = 3;

  Legend();

  /**
   * Add a leged to this canvas. Usually legends are not specified manually via
   * this method but through one of the Chart subclasses. However it is safe
   * to call this method to explicitly define a custom legend.
   *
   * The returned pointer is owned by the canvas instance and must not be freed
   * by the caller.
   */
  LegendDefinition* addLegend(
      LegendDefinition::kVerticalPosition vert_pos,
      LegendDefinition::kHorizontalPosition horiz_pos,
      LegendDefinition::kPlacement placement,
      const std::string& title);
  /**
   * Return the legend or nullptr
   */
  LegendDefinition* legend() const;

  /**
   * Render the contents of this canvas to the provided render target
   *
   * @param target a RenderTarget subclass instance. Does not transfer ownership
   */
  void render(RenderTarget* target) const;

protected:

  /**
   * Render the legends
   */
  void renderOutsideLegends(RenderTarget* target, Viewport* viewport) const;

  /**
   * Render the legends
   */
  void renderInsideLegends(RenderTarget* target, Viewport* viewport) const;

  void renderRightLegend(
      RenderTarget* target,
      Viewport* viewport,
      LegendDefinition* legend,
      double horiz_padding,
      bool bottom,
      bool outside) const;

  void renderLeftLegend(
      RenderTarget* target,
      Viewport* viewport,
      LegendDefinition* legend,
      double horiz_padding,
      bool bottom,
      bool outside) const;

  std::vector<std::unique_ptr<LegendDefinition>> legends_;
};

}
}
#endif
