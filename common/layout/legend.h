/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

namespace plotfx {
namespace chart {
class Layer;

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
   * @param target a Layer subclass instance. Does not transfer ownership
   */
  void render(Layer* target) const;

protected:

  /**
   * Render the legends
   */
  void renderOutsideLegends(Layer* target, Viewport* viewport) const;

  /**
   * Render the legends
   */
  void renderInsideLegends(Layer* target, Viewport* viewport) const;

  void renderRightLegend(
      Layer* target,
      Viewport* viewport,
      LegendDefinition* legend,
      double horiz_padding,
      bool bottom,
      bool outside) const;

  void renderLeftLegend(
      Layer* target,
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
