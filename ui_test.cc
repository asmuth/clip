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
#include "ui/barchart.h"
#include "ui/canvas.h"
#include "ui/domain.h"
#include "base/series.h"

namespace fnordmetric {
using namespace fnordmetric::ui;

class UITest {
public:
  UITest() {}

  void run() {
    //testCanvasWithLeftAxis();
    //testCanvasWithLeftAxisAndTitle();
    //testCanvasWithLeftAndBottomAxis();
    //testCanvasWithAllAxis();
    //testCanvasWithAllMultiAxis();
    //testCanvasWithMultiLeftAxis();
    //testCanvasWithAxisFromNumericalDomain();
    //testSimpleBarChart();
    //testMultiSeriesBarChart();
    testHorizontalBarChart();
    testHorizontalMultiSeriesBarChart();
  }

  void testCanvasWithLeftAxis() {
    ui::Canvas canvas;

    auto axis_left = canvas.addAxis(ui::AxisDefinition::LEFT);
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

  void testCanvasWithLeftAxisAndTitle() {
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

  void testCanvasWithLeftAndBottomAxis() {
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

    auto axis_bottom = canvas.addAxis(ui::AxisDefinition::BOTTOM);
    axis_bottom->setTitle("my axis");
    axis_bottom->addTick(0.0);
    axis_bottom->addTick(0.2);
    axis_bottom->addTick(0.4);
    axis_bottom->addTick(0.6);
    axis_bottom->addTick(0.8);
    axis_bottom->addTick(1.0);
    axis_bottom->addLabel(0.2, "A");
    axis_bottom->addLabel(0.4, "B");
    axis_bottom->addLabel(0.6, "C");
    axis_bottom->addLabel(0.8, "D");

    auto svg = canvas.renderSVG();
    //printf("svg data: %s", svg.c_str());
  }

  void testCanvasWithAllAxis() {
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

    auto axis_right = canvas.addAxis(ui::AxisDefinition::RIGHT);
    axis_right->setTitle("my axis");
    axis_right->addTick(0.0);
    axis_right->addTick(0.2);
    axis_right->addTick(0.4);
    axis_right->addTick(0.6);
    axis_right->addTick(0.8);
    axis_right->addTick(1.0);
    axis_right->addLabel(0.0, "0");
    axis_right->addLabel(0.2, "1");
    axis_right->addLabel(0.4, "2");
    axis_right->addLabel(0.6, "3");
    axis_right->addLabel(0.8, "4");
    axis_right->addLabel(1.0, "5");

    auto axis_bottom = canvas.addAxis(ui::AxisDefinition::BOTTOM);
    axis_bottom->setTitle("my axis");
    axis_bottom->addTick(0.0);
    axis_bottom->addTick(0.2);
    axis_bottom->addTick(0.4);
    axis_bottom->addTick(0.6);
    axis_bottom->addTick(0.8);
    axis_bottom->addTick(1.0);
    axis_bottom->addLabel(0.2, "A");
    axis_bottom->addLabel(0.4, "B");
    axis_bottom->addLabel(0.6, "C");
    axis_bottom->addLabel(0.8, "D");

    auto axis_top = canvas.addAxis(ui::AxisDefinition::TOP);
    axis_top->setTitle("my axis");
    axis_top->addTick(0.0);
    axis_top->addTick(0.2);
    axis_top->addTick(0.4);
    axis_top->addTick(0.6);
    axis_top->addTick(0.8);
    axis_top->addTick(1.0);
    axis_top->addLabel(0.2, "A");
    axis_top->addLabel(0.4, "B");
    axis_top->addLabel(0.6, "C");
    axis_top->addLabel(0.8, "D");

    auto svg = canvas.renderSVG();
    //printf("svg data: %s", svg.c_str());
  }

  void testCanvasWithAllMultiAxis() {
    ui::Canvas canvas;

    {
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
    }

    {
      auto axis_left = canvas.addAxis(ui::AxisDefinition::LEFT);
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
    }

    {
      auto axis_right = canvas.addAxis(ui::AxisDefinition::RIGHT);
      axis_right->setTitle("my axis");
      axis_right->addTick(0.0);
      axis_right->addTick(0.2);
      axis_right->addTick(0.4);
      axis_right->addTick(0.6);
      axis_right->addTick(0.8);
      axis_right->addTick(1.0);
      axis_right->addLabel(0.0, "0");
      axis_right->addLabel(0.2, "1");
      axis_right->addLabel(0.4, "2");
      axis_right->addLabel(0.6, "3");
      axis_right->addLabel(0.8, "4");
      axis_right->addLabel(1.0, "5");
    }

    {
      auto axis_right = canvas.addAxis(ui::AxisDefinition::RIGHT);
      axis_right->addTick(0.0);
      axis_right->addTick(0.2);
      axis_right->addTick(0.4);
      axis_right->addTick(0.6);
      axis_right->addTick(0.8);
      axis_right->addTick(1.0);
      axis_right->addLabel(0.0, "0");
      axis_right->addLabel(0.2, "1");
      axis_right->addLabel(0.4, "2");
      axis_right->addLabel(0.6, "3");
      axis_right->addLabel(0.8, "4");
      axis_right->addLabel(1.0, "5");
    }

    {
      auto axis_bottom = canvas.addAxis(ui::AxisDefinition::BOTTOM);
      axis_bottom->setTitle("my axis");
      axis_bottom->addTick(0.0);
      axis_bottom->addTick(0.2);
      axis_bottom->addTick(0.4);
      axis_bottom->addTick(0.6);
      axis_bottom->addTick(0.8);
      axis_bottom->addTick(1.0);
      axis_bottom->addLabel(0.2, "A");
      axis_bottom->addLabel(0.4, "B");
      axis_bottom->addLabel(0.6, "C");
      axis_bottom->addLabel(0.8, "D");
    }

    {
      auto axis_bottom = canvas.addAxis(ui::AxisDefinition::BOTTOM);
      axis_bottom->addTick(0.0);
      axis_bottom->addTick(0.2);
      axis_bottom->addTick(0.4);
      axis_bottom->addTick(0.6);
      axis_bottom->addTick(0.8);
      axis_bottom->addTick(1.0);
      axis_bottom->addLabel(0.2, "A");
      axis_bottom->addLabel(0.4, "B");
      axis_bottom->addLabel(0.6, "C");
      axis_bottom->addLabel(0.8, "D");
    }

    {
      auto axis_top = canvas.addAxis(ui::AxisDefinition::TOP);
      axis_top->setTitle("my axis");
      axis_top->addTick(0.0);
      axis_top->addTick(0.2);
      axis_top->addTick(0.4);
      axis_top->addTick(0.6);
      axis_top->addTick(0.8);
      axis_top->addTick(1.0);
      axis_top->addLabel(0.2, "A");
      axis_top->addLabel(0.4, "B");
      axis_top->addLabel(0.6, "C");
      axis_top->addLabel(0.8, "D");
    }

    {
      auto axis_top = canvas.addAxis(ui::AxisDefinition::TOP);
      axis_top->addTick(0.0);
      axis_top->addTick(0.2);
      axis_top->addTick(0.4);
      axis_top->addTick(0.6);
      axis_top->addTick(0.8);
      axis_top->addTick(1.0);
      axis_top->addLabel(0.2, "A");
      axis_top->addLabel(0.4, "B");
      axis_top->addLabel(0.6, "C");
      axis_top->addLabel(0.8, "D");
    }

    auto svg = canvas.renderSVG();
    //printf("svg data: %s", svg.c_str());
  }

  void testCanvasWithMultiLeftAxis() {
    ui::Canvas canvas;

    auto axis1 = canvas.addAxis(ui::AxisDefinition::LEFT);
    axis1->setTitle("axis 1 / axis 2");
    axis1->addTick(0.0);
    axis1->addTick(0.2);
    axis1->addTick(0.4);
    axis1->addTick(0.6);
    axis1->addTick(0.8);
    axis1->addTick(1.0);
    axis1->addLabel(0.0, "0");
    axis1->addLabel(0.2, "1");
    axis1->addLabel(0.4, "2");
    axis1->addLabel(0.6, "3");
    axis1->addLabel(0.8, "4");
    axis1->addLabel(1.0, "5");

    auto axis2 = canvas.addAxis(ui::AxisDefinition::LEFT);
    axis2->addTick(0.0);
    axis2->addTick(0.3);
    axis2->addTick(0.6);
    axis2->addTick(0.9);
    axis2->addTick(1.0);
    axis2->addLabel(0.0, "A");
    axis2->addLabel(0.3, "B");
    axis2->addLabel(0.6, "C");
    axis2->addLabel(0.9, "D");
    axis2->addLabel(1.0, "E");

    auto svg = canvas.renderSVG();
    //printf("svg data: %s", svg.c_str());
  }


  void testCanvasWithAxisFromNumericalDomain() {
    ui::Canvas canvas;

    NumericalDomain domain(0, 100);
    auto axis1 = canvas.addAxis(ui::AxisDefinition::LEFT, &domain);
    axis1->setTitle("numerical domain");

    auto svg = canvas.renderSVG();
  }

  void testSimpleBarChart() {
    Series2D<std::string, double> series("myseries");
    series.addDatum("A", 34);
    series.addDatum("B", 18);
    series.addDatum("C", 43);
    series.addDatum("D", 19);
    series.addDatum("E", 25);
    series.addDatum("F", 33);

    ui::Canvas canvas;
    auto bar_chart = canvas.addChart<BarChart>(BarChart::O_VERTICAL);
    bar_chart->addSeries(&series);
    bar_chart->addAxis(AxisDefinition::TOP);
    bar_chart->addAxis(AxisDefinition::RIGHT);
    bar_chart->addAxis(AxisDefinition::BOTTOM);
    bar_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testMultiSeriesBarChart() {
    Series2D<std::string, double> series1("myseries1");
    series1.addDatum("A", 40);
    series1.addDatum("B", 35);
    series1.addDatum("C", 30);
    series1.addDatum("D", 25);
    series1.addDatum("E", 20);
    series1.addDatum("F", 15);

    Series2D<std::string, double> series2("myseries2");
    series2.addDatum("A", 23);
    series2.addDatum("B", 65);
    series2.addDatum("C", 43);
    series2.addDatum("D", 12);
    series2.addDatum("E", 54);
    series2.addDatum("F", 31);

    ui::Canvas canvas;
    auto bar_chart = canvas.addChart<BarChart>(BarChart::O_VERTICAL);
    bar_chart->addSeries(&series1);
    bar_chart->addSeries(&series2);
    bar_chart->addAxis(AxisDefinition::TOP);
    bar_chart->addAxis(AxisDefinition::RIGHT);
    bar_chart->addAxis(AxisDefinition::BOTTOM);
    bar_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testHorizontalBarChart() {
    Series2D<std::string, double> series1("myseries1");
    series1.addDatum("A", 40);
    series1.addDatum("B", 35);
    series1.addDatum("C", 30);
    series1.addDatum("D", 25);
    series1.addDatum("E", 20);
    series1.addDatum("F", 15);

    ui::Canvas canvas;
    auto bar_chart = canvas.addChart<BarChart>(BarChart::O_HORIZONTAL);
    bar_chart->addSeries(&series1);
    bar_chart->addAxis(AxisDefinition::TOP);
    bar_chart->addAxis(AxisDefinition::RIGHT);
    bar_chart->addAxis(AxisDefinition::BOTTOM);
    bar_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testHorizontalMultiSeriesBarChart() {
    Series2D<std::string, double> series1("myseries1");
    series1.addDatum("A", 40);
    series1.addDatum("B", 35);
    series1.addDatum("C", 30);
    series1.addDatum("D", 25);
    series1.addDatum("E", 20);
    series1.addDatum("F", 15);

    Series2D<std::string, double> series2("myseries2");
    series2.addDatum("A", 23);
    series2.addDatum("B", 65);
    series2.addDatum("C", 43);
    series2.addDatum("D", 12);
    series2.addDatum("E", 54);
    series2.addDatum("F", 31);

    ui::Canvas canvas;
    auto bar_chart = canvas.addChart<BarChart>(BarChart::O_HORIZONTAL);
    bar_chart->addSeries(&series1);
    bar_chart->addSeries(&series2);
    bar_chart->addAxis(AxisDefinition::TOP);
    bar_chart->addAxis(AxisDefinition::RIGHT);
    bar_chart->addAxis(AxisDefinition::BOTTOM);
    bar_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }


};

}

int main() {
  fnordmetric::UITest test;
  test.run();
  //printf("all tests passed! :)\n");
}
