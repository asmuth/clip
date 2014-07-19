/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "ui/canvas.h"

namespace fnordmetric {
using namespace fnordmetric::ui;

class UITest {
public:
  UITest() {}

  void run() {
    testCanvasWithAxes();
    printf("here be dragons\n");
  }

  void testCanvasWithAxes() {
    ui::Canvas canvas;

    auto svg = canvas.renderSVG();
    printf("svg data: %s", svg.c_str());
  }

  void testSimpleBarChart() {
    //ui::Canvas canvas;
    //ui::BarChart bar_chart(&canvas);

    //Series test_series;
    //bar_chart.addSeries(test_series);

    //auto svg = canvas.renderSVG();
    //printf("svg data: %s", svg.c_str());
  }

};

}

int main() {
  fnordmetric::UITest test;
  test.run();
  printf("all tests passed! :)\n");
}
