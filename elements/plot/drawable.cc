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
#include <string>
#include "canvas.h"
#include "drawable.h"

namespace plotfx {
namespace chart {

Drawable::Drawable(Canvas* canvas) : canvas_(canvas) {}

Drawable::~Drawable() {
  for (auto series : all_series_) {
    delete series;
  }
}

void Drawable::setTitle(const std::string& title) {
  canvas_->setTitle(title);
}

void Drawable::setSubtitle(const std::string& subtitle) {
  canvas_->setSubtitle(subtitle);
}

LegendDefinition* Drawable::addLegend(
    LegendDefinition::kVerticalPosition vert_pos,
    LegendDefinition::kHorizontalPosition horiz_pos,
    LegendDefinition::kPlacement placement,
    const std::string& title) {
  auto legend = canvas_->addLegend(vert_pos, horiz_pos, placement, title);
  updateLegend();
  return legend;
}

void Drawable::addSeries(Series* series) {
  all_series_.push_back(series);
  updateLegend();
}

void Drawable::updateLegend() {
  auto legend = canvas_->legend();

  if (legend == nullptr) {
    return;
  }

  for (const auto& series : all_series_) {
    legend->addEntry(
        series->name(),
        series->getProperty(Series::P_COLOR),
        "circle");
  }
}

}
}
