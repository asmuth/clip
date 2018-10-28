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
class Layer;

class PlotLayout {
public:
  static const int kAxisPadding = 0; // FIXPAUL make configurable
  static const int kTickLength = 5; // FIXPAUL make configurable
  static const int kAxisLabelHeight = 25; // FIXPAUL make configurable
  static const int kAxisLabelWidth = 50; // FIXPAUL make configurable
  static const int kAxisTitleLength = 20; // FIXPAUL make configurable
  static const int kCharWidth = 6.0f; // FIXPAUL make configurable

  PlotLayout();

  void setDimensions(int width, int height);

  /**
   * Add an axis to this canvas. Usually axes are not specified manually via
   * this method but through one of the Chart subclasses. However it is safe
   * to call this method to explicitly define a custom axis.
   *
   * The returned pointer is owned by the canvas instance and must not be freed
   * by the caller.
   */
  AxisDefinition* addAxis(AxisDefinition::kPosition position);

  /**
   * Add a grid to this canvas. Usually grids are not specified manually via
   * this method but through one of the Chart subclasses. However it is safe
   * to call this method to explicitly define a custom grid.
   *
   * The returned pointer is owned by the canvas instance and must not be freed
   * by the caller.
   */
  GridDefinition* addGrid(GridDefinition::kPlacement placement);

  /**
   * Render the contents of this canvas to the provided render target
   *
   * @param target a Layer subclass instance. Does not transfer ownership
   */
  void render(Layer* target) const;

protected:

  // FIXPAUL
  int estimateTextLength(const std::string& str) const {
    return str.size() * kCharWidth;
  }

  /**
   * Render the axes
   */
  void renderAxes(Layer* target, Viewport* viewport) const;

  /**
   * Render a top axis
   *
   * @param target the render target
   * @param axis the axis definition
   * @param padding the padding state
   * @param top the top padding for this axis
   */
  void renderTopAxis(
      Layer* target,
      Viewport* viewport,
      AxisDefinition* axis,
      int top) const;

  /**
   * Render a right axis
   *
   * @param target the render target
   * @param axis the axis definition
   * @param padding the padding state
   * @param right the right padding for this axis
   */
  void renderRightAxis(
      Layer* target,
      Viewport* viewport,
      AxisDefinition* axis,
      int right) const;

  /**
   * Render a bottom axis
   *
   * @param target the render target
   * @param axis the axis definition
   * @param padding the padding state
   * @param bottom the bottom padding for this axis
   */
  void renderBottomAxis(
      Layer* target,
      Viewport* viewport,
      AxisDefinition* axis,
      int bottom) const;

  /**
   * Render a left axis
   *
   * @param target the render target
   * @param axis the axis definition
   * @param padding the padding state
   * @param left the left padding for this axis
   */
  void renderLeftAxis(
      Layer* target,
      Viewport* viewport,
      AxisDefinition* axis,
      int left) const;

  /**
   * Render the grids
   */
  void renderGrids(Layer* target, Viewport* viewport) const;

  // FIXPAUL this belongs into the layer
  int width_;
  int height_;

  std::vector<std::unique_ptr<AxisDefinition>> axes_;
  std::vector<std::unique_ptr<GridDefinition>> grids_;
};

}
}
#endif
