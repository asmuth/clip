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
#include "ui/axisdefinition.h"
#include "ui/canvas.h"

namespace fnordmetric {
using namespace fnordmetric::ui;

class UITest {
public:
  UITest() {}

  void run() {
    testCanvasWithAxes();
  }

  void testCanvasWithAxes() {
    ui::Canvas canvas;

    auto axis_left = canvas.addAxis(ui::AxisDefinition::LEFT);
    axis_left->setTitle("my axis");
    axis_left->addTick(0.0);
    axis_left->addTick(0.2);
    axis_left->addTick(0.4);
    axis_left->addTick(0.6);
    axis_left->addTick(0.8);
    axis_left->addTick(1.0);
    axis_left->addLabel(0.0, "0");
    axis_left->addLabel(0.2, "1");
    axis_left->addLabel(0.4, "2");
    axis_left->addLabel(0.6, "3");
    axis_left->addLabel(0.8, "4");
    axis_left->addLabel(1.0, "5");

    auto svg = canvas.renderSVG();
    //printf("svg data: %s", svg.c_str());
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
  //printf("all tests passed! :)\n");
}
