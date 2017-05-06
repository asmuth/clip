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
#include "cplot/axisdefinition.h"
#include "cplot/drawable.h"
#include "cplot/legenddefinition.h"

namespace stx {
namespace chart {
class RenderTarget;

class Canvas {
public:
  static const int kAxisPadding = 0; // FIXPAUL make configurable
  static const int kTickLength = 5; // FIXPAUL make configurable
  static const int kAxisLabelHeight = 25; // FIXPAUL make configurable
  static const int kAxisLabelWidth = 50; // FIXPAUL make configurable
  static const int kAxisTitleLength = 20; // FIXPAUL make configurable
  static const int kCharWidth = 6.0f; // FIXPAUL make configurable
  static const int kTitleLineHeight = 20; // FIXPAUL make configurable
  static const int kSubtitleLineHeight = 20; // FIXPAUL make configurable
  static const int kLegendLabelPadding = 20; // FIXPAUL make configurable
  static const int kLegendLineHeight = 20; // FIXPAUL make configurable
  static const int kLegendInsideVertPadding = 10;
  static const int kLegendInsideHorizPadding = 15;
  static const int kLegendOutsideVertPadding = 10;
  static const int kLegendOutsideHorizPadding = 25;
  static const int kLegendPointY = 6;
  static const int kLegendPointWidth = 8;
  static const int kLegendPointSize = 3;

  Canvas();

  /**
   * Set the title for this canvas
   */
  void setTitle(const std::string& title);

  /**
   * Set the subtitle for this canvas
   */
  void setSubtitle(const std::string& subtitle);

  void setDimensions(int width, int height);

  /**
   * FIXPAUL overcomplicated, just accept a ptr
   *
   * The returned pointer is owned by the canvas instance and must not be freed
   * by the caller.
   */
  template <typename ChartType, typename... Args>
  ChartType* addChart(Args... args) {
    auto drawable = new ChartType(this, args...);
    drawables_.emplace_back(drawable);
    return drawable;
  }

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
   * Add a grid to this canvas. Usually grids are not specified manually via
   * this method but through one of the Chart subclasses. However it is safe
   * to call this method to explicitly define a custom grid.
   *
   * The returned pointer is owned by the canvas instance and must not be freed
   * by the caller.
   */
  GridDefinition* addGrid(GridDefinition::kPlacement placement);

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

  // FIXPAUL
  int estimateTextLength(const std::string& str) const {
    return str.size() * kCharWidth;
  }

  /**
   * Render the chart title
   */
  void renderTitle(RenderTarget* target, Viewport* viewport) const;

  /**
   * Render the charts
   */
  void renderCharts(RenderTarget* target, Viewport* viewport) const;

  /**
   * Render the axes
   */
  void renderAxes(RenderTarget* target, Viewport* viewport) const;

  /**
   * Render a top axis
   *
   * @param target the render target
   * @param axis the axis definition
   * @param padding the padding state
   * @param top the top padding for this axis
   */
  void renderTopAxis(
      RenderTarget* target,
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
      RenderTarget* target,
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
      RenderTarget* target,
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
      RenderTarget* target,
      Viewport* viewport,
      AxisDefinition* axis,
      int left) const;

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

  /**
   * Render the grids
   */
  void renderGrids(RenderTarget* target, Viewport* viewport) const;

  // FIXPAUL this belongs into the rendertarget
  int width_;
  int height_;
  std::vector<std::unique_ptr<AxisDefinition>> axes_;
  std::vector<std::unique_ptr<LegendDefinition>> legends_;
  std::vector<std::unique_ptr<GridDefinition>> grids_;
  std::vector<std::unique_ptr<Drawable>> drawables_;
  std::string title_;
  std::string subtitle_;
};

}
}
#endif
