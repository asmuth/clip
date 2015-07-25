/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _libstx_STYLE_DEFAULT_H
#define _libstx_STYLE_DEFAULT_H
#include <stdlib.h>
#include <string.h>

namespace stx {

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
