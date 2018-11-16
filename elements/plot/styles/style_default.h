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

#ifndef _libstx_STYLE_DEFAULT_H
#define _libstx_STYLE_DEFAULT_H
#include <stdlib.h>
#include <string.h>

namespace plotfx {

static const std::string kStyleSheetDefault = R"(
  .fm-chart text, .fm-tooltip {
    font-family: "Helvetica Neue", Helvetica, Arial, "Lucida Grande", sans-serif;
    font: 10px sans-serif;
  }

  .fm-chart .axis .stroke {
    stroke: #aaa;
    stroke-width: 1px;
  }

  .fm-chart .axis .tick {
    stroke: #aaa;
    stroke-width: 1px;
  }

  .fm-chart .axis .label, .axis .title {
    fill: #666;
  }

  .fm-chart .axis .label {
    font-size: 10px;
  }

  .fm-chart .axis .title {
    font-size: 10px;
  }

  .fm-chart .chart-title {
    font-size: 12px;
    font-weight: bold;
  }

  .fm-chart .chart-subtitle {
    font-size: 10px;
  }

  .fm-chart text.label {
    fill: #666;
  }

  .fm-chart .legend text {
    font-size: 10px;
    fill: #333;
  }

  .fm-chart .legend text.title {
    fill: #666;
  }

  .fm-chart .line {
    fill: none;
    stroke: #000;
  }

  .fm-chart .area {
    opacity: 0.8;
  }

  .fm-chart .grid .gridline {
    stroke: #ddd;
    stroke-width: 0.02em;
  }

  .fm-chart .bar,
  .fm-chart .point,
  .fm-chart .area {
    fill: #000;
  }

  .fm-chart .bar.color6,
  .fm-chart .point.color6,
  .fm-chart .area.color6 {
    fill: #db843d;
  }

  .fm-chart .lines .line.color6 {
    strok: #db843d;
  }

  .fm-chart .bar.color5,
  .fm-chart .point.color5,
  .fm-chart .area.color5 {
    fill: #3d96ae;
  }

  .fm-chart .lines .line.color5 {
    stroke: #3d96ae;
  }

  .fm-chart .bar.color4,
  .fm-chart .point.color4,
  .fm-chart .area.color4 {
    fill: #80699b;
  }

  .fm-chart .line.color4 {
    stroke: #80699b;
  }

  .fm-chart .bar.color3,
  .fm-chart .point.color3,
  .fm-chart .area.color3 {
    fill: #89a54e;
  }

  .fm-chart .line.color3 {
    stroke: #89a54e;
  }

  .fm-chart .bar.color2,
  .fm-chart .point.color2,
  .fm-chart .area.color2 {
    fill: #aa4643;
  }

  .fm-chart .line.color2 {
    stroke: #aa4643;
  }

  .fm-chart .bar.color1,
  .fm-chart .point.color1,
  .fm-chart .area.color1 {
    fill: #4572a7;
  }

  .fm-chart .line.color1 {
    stroke: #4572a7;
  }

  .fm-chart.bar.vertical .axis.left .stroke,
  .fm-chart.bar.vertical .axis.left .tick,
  .fm-chart.bar.vertical .axis.right .stroke,
  .fm-chart.bar.vertical .axis.right .tick {
    display: none;
  }

  .fm-tooltip {
    text-align: center;
    padding: 5px 7px;
    color: #fff;
    background: #333;
    border-radius: 3px;
    font-size: 13px;
  }
)";

}
#endif
