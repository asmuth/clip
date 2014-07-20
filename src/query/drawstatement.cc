/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include "drawstatement.h"
#include "../ui/canvas.h"
#include "../ui/barchart.h"

namespace fnordmetric {
namespace query {

void DrawStatement::execute(ui::Canvas* canvas) {
  printf("execute!");
  switch (type_) {
    case T_BAR_CHART:
      return executeDrawable(new ui::BarChart(canvas));
  }
}

}
}
