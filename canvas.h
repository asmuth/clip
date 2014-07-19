/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_CANVAS_H
#define _FNORDMETRIC_CANVAS_H
#include <vector>
#include <string>
#include <tuple>
#include "axisdefinition.h"

namespace fnordmetric {
class SeriesDefinition;
namespace ui {
class RenderTarget;

class Canvas {
public:
  // static const int kNumTicks = 6; // FIXPAUL make configurable;
  static const int kAxisPadding = 5; // FIXPAUL make configurable
  static const int kTickLength = 5; // FIXPAUL make configurable
  static const int kAxisLabelHeight = 35.0f; // FIXPAUL make configurable
  static const int kAxisLabelWidth = 50.0f; // FIXPAUL make configurable
  static const int kAxisTitleLength = 20.0f; // FIXPAUL make configurable

  Canvas();

  /**
   * Add an axis to this canvas. Usually axes are not specified manually using
   * this method but through one of the Chart subclasses. However it is safe
   * to call this method to explicitly define a custom axis.
   *
   * The returned pointer is owned by the canvas instance and must not be freed
   * by the caller.
   *
   * @param axis_position the position of the new axis
   */
  AxisDefinition* addAxis(AxisDefinition::kPosition axis_position);

  /**
   * Render the contents of this canvas to the provided render target
   *
   * @param target a RenderTarget subclass instance. Does not transfer ownership
   */
  void render(RenderTarget* target) const;

  /**
   * Render the contents of this canvas as a scalabale vector graphic and return
   * as string
   */
   std::string renderSVG() const;

protected:

  /**
   * Render the axes
   *
   * @param target the render target
   * @param padding the padding state
   */
  void renderAxes(
      RenderTarget* target,
      std::tuple<int, int, int, int>* padding) const;

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
      AxisDefinition* axis,
      std::tuple<int, int, int, int>* padding,
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
      AxisDefinition* axis,
      std::tuple<int, int, int, int>* padding,
      int left) const;

  int width_;
  int height_;
  std::vector<std::unique_ptr<AxisDefinition>> axes_;
  std::vector<SeriesDefinition*> series_;
};

}
}
#endif
