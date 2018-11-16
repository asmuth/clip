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
#include "legenddefinition.h"

namespace plotfx {
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
