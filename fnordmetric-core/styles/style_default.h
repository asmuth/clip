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

  .chart.bar rect.color1, .chart.bar rect {
    fill: steelblue;
  }

  .chart.bar rect.color2 {
    fill: orange;
  }

  .chart.bar rect.color3 {
    fill: lightgreen;
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
