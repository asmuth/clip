/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_STYLE_DEFAULT_H
#define _FNORDMETRIC_STYLE_DEFAULT_H
#include <stdlib.h>
#include <string.h>

namespace fnordmetric {

static const std::string kStyleSheetDefault = R"(
  .chart text {
    font-family: "Helvetica Neue", Helvetica, Arial, "Lucida Grande", sans-serif;
    font: 10pt sans-serif;
  }

  .axis .stroke {
    stroke: rgb(0,0,0);
    stroke-width: 1px;
  }

  .axis .tick {
    stroke: rgb(0,0,0);
    stroke-width: 1px;
  }

  .axis .label, .axis .title {
    fill: #000;
  }

  .axis .label {
    font-size: 9pt;
  }

  .axis .title {
    font-size: 11pt;
  }

  .chart .lines .line {
    fill: none;
    stroke: #db843d;
  }

  .chart .bars .bar,
  .chart .points .point {
    fill: #db843d;
  }

  .chart .bars .bar.color5, .chart .points .point.color5 {
    fill: #3d96ae;
  }

  .chart .lines .line.color5 {
    stroke: #3d96ae;
  }

  .chart .bars .bar.color4, .chart .points .point.color4 {
    fill: #80699b;
  }

  .chart .lines .line.color4 {
    stroke: #80699b;
  }

  .chart .bars .bar.color3, .chart .points .point.color3 {
    fill: #89a54e;
  }

  .chart .lines .line.color3 {
    stroke: #89a54e;
  }

  .chart .bars .bar.color2, .chart .points .point.color2 {
    fill: #aa4643;
  }

  .chart .lines .line.color2 {
    stroke: #aa4643;
  }

  .chart .bars .bar.color1, .chart .points .point.color1 {
    fill: #4572a7;
  }

  .chart .lines .line.color1 {
    stroke: #4572a7;
  }

  .chart.bar.vertical .axis.left .stroke,
  .chart.bar.vertical .axis.left .tick,
  .chart.bar.vertical .axis.right .stroke,
  .chart.bar.vertical .axis.right .tick {
    display: none;
  }
)";

}
#endif
