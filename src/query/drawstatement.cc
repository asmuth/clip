/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include "drawstatement.h"
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/barchart.h>
#include <fnordmetric/ui/linechart.h>

namespace fnordmetric {
namespace query {

void DrawStatement::execute(ui::Canvas* canvas) {
  switch (type_) {
    case T_BAR_CHART:
      return executeDrawable(canvas->addChart<ui::BarChart>());
    case T_LINE_CHART:
      return executeDrawable(canvas->addChart<ui::LineChart>());
  }
}

}
}
