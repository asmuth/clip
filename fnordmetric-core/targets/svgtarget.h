/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_SVGTARGET_H
#define _FNORDMETRIC_SVGTARGET_H
#include <stdlib.h>
#include <vector>
#include <string>
#include "../rendertarget.h"

namespace fnordmetric {

class SVGTarget : public ChartRenderTarget {
public:

  SVGTarget() : indent_(0) {}

#define appendLine(...) printf(__VA_ARGS__);

  void beginChart(int width, int height) {
    appendLine("<svg class='chart' width='%i' height='%i'>\n", width, height);
    indent_++;
  }

  void finishChart() {
    indent_--;
    appendLine("</svg>\n");
  }

  void drawRect(
      double x,
      double y,
      double width,
      double height) override {
    appendLine(
        "<rect x='%f' y='%f' width='%f' height='%f'></rect>\n",
        x,
        y,
        width,
        height);
  }


protected:

  int indent_;
};


}
#endif
