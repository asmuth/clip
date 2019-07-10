/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "element_factory.h"
#include "elements/areas.h"
#include "elements/axis.h"
#include "elements/bars.h"
#include "elements/box.h"
#include "elements/gridlines.h"
#include "elements/legend.h"
#include "elements/lines.h"
#include "elements/plot.h"
#include "elements/points.h"

#include <unordered_map>

namespace plotfx {

static std::unordered_map<std::string, ElementBuilder> elems = {
  {
    "areas",
    elem_builder<plot::area::PlotAreaConfig>(
        &plot::area::configure,
        &plot::area::draw)
  },
  {
    "axis",
    elem_builder<AxisDefinition>(
        &axis::configure,
        &axis::reflow,
        &axis::draw)
  },
  {
    "bars",
    elem_builder<plot::bars::PlotBarsConfig>(
        &plot::bars::configure,
        &plot::bars::draw)
  },
  {
    "box",
    elem_builder<box::BoxConfig>(
        &box::configure,
        &box::draw)
  },
  {
    "gridlines",
    elem_builder<gridlines::GridlineDefinition>(
        &gridlines::configure,
        &gridlines::draw)
  },
  {
    "legend",
    elem_builder<LegendConfig>(
        &legend::configure,
        &legend::reflow,
        &legend::draw)
  },
  {
    "lines",
    elem_builder<plot::lines::PlotLinesConfig>(
        &plot::lines::configure,
        &plot::lines::draw)
  },
  {
    "plot",
    elem_builder<plot::PlotConfig>(
        &plot::configure,
        &plot::draw)
  },
  {
    "points",
    elem_builder<plot::points::PlotPointsConfig>(
        &plot::points::configure,
        &plot::points::draw)
  },
};

ReturnCode buildElement(
    const std::string& name,
    const plist::PropertyList& plist,
    const Environment& env,
    ElementRef* elem) {
  const auto& elem_entry = elems.find(name);
  if (elem_entry == elems.end()) {
    return ReturnCode::errorf("NOTFOUND", "no such element: $0", name);
  }

  return elem_entry->second(plist, env, elem);
}

} // namespace plotfx

