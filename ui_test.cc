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
#include "ui/areachart.h"
#include "ui/axisdefinition.h"
#include "ui/barchart.h"
#include "ui/linechart.h"
#include "ui/pointchart.h"
#include "ui/canvas.h"
#include "ui/domain.h"
#include "base/series.h"

namespace fnordmetric {
using namespace fnordmetric::ui;

class UITest {
public:
  UITest() {}

  void run() {
    testCanvasWithLeftAxis();
    testCanvasWithLeftAxisAndTitle();
    testCanvasWithLeftAndBottomAxis();
    testCanvasWithAllAxis();
    testCanvasWithAllMultiAxis();
    testCanvasWithMultiLeftAxis();
    testCanvasWithAxisFromNumericalDomain();
    testSimpleBarChart();
    testMultiSeriesBarChart();
    testStackedBarChart();
    testHorizontalBarChart();
    testHorizontalMultiSeriesBarChart();
    testHorizontalStackedBarChart();
    testRangeBarChart();
    testHorizontalRangeBarChart();
    testSimplePointChart();
    testVariableSizePointChart();
    testSimpleLineChart();
    testPointLineChart();
    testMultiChart();
    testSimpleAreaChart();
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

  void testStackedBarChart() {
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
    auto bar_chart = canvas.addChart<BarChart>(BarChart::O_VERTICAL, true);
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

  void testHorizontalStackedBarChart() {
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
    auto bar_chart = canvas.addChart<BarChart>(BarChart::O_HORIZONTAL, true);
    bar_chart->addSeries(&series1);
    bar_chart->addSeries(&series2);
    bar_chart->addAxis(AxisDefinition::TOP);
    bar_chart->addAxis(AxisDefinition::RIGHT);
    bar_chart->addAxis(AxisDefinition::BOTTOM);
    bar_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testRangeBarChart() {
    Series3D<std::string, double, double> series1("myseries1");
    series1.addDatum("A", -40, +40);
    series1.addDatum("B", -35, +35);
    series1.addDatum("C", -30, +30);
    series1.addDatum("D", -25, +25);
    series1.addDatum("E", -20, +20);
    series1.addDatum("F", -15, +15);

    Series3D<std::string, double, double> series2("myseries2");
    series2.addDatum("A", -23, +23);
    series2.addDatum("B", -65, +65);
    series2.addDatum("C", -43, +43);
    series2.addDatum("D", -12, +12);
    series2.addDatum("E", -54, +54);
    series2.addDatum("F", -31, +31);

    ui::Canvas canvas;
    auto bar_chart = canvas.addChart<BarChart>(BarChart::O_VERTICAL);
    bar_chart->addSeries(&series1);
    bar_chart->addSeries(&series2);
    bar_chart->addAxis(AxisDefinition::TOP);
    bar_chart->addAxis(AxisDefinition::RIGHT);
    bar_chart->addAxis(AxisDefinition::BOTTOM);
    bar_chart->addAxis(AxisDefinition::LEFT)->setTitle("myaxis");

    auto svg = canvas.renderSVG();
  }

  void testHorizontalRangeBarChart() {
    Series3D<std::string, double, double> series1("myseries1");
    series1.addDatum("A", -40, +40);
    series1.addDatum("B", -35, +35);
    series1.addDatum("C", -30, +30);
    series1.addDatum("D", -25, +25);
    series1.addDatum("E", -20, +20);
    series1.addDatum("F", -15, +15);

    Series3D<std::string, double, double> series2("myseries2");
    series2.addDatum("A", -23, +23);
    series2.addDatum("B", -65, +65);
    series2.addDatum("C", -43, +43);
    series2.addDatum("D", -12, +12);
    series2.addDatum("E", -54, +54);
    series2.addDatum("F", -31, +31);

    ui::Canvas canvas;
    auto bar_chart = canvas.addChart<BarChart>(BarChart::O_HORIZONTAL);
    bar_chart->addSeries(&series1);
    bar_chart->addSeries(&series2);
    bar_chart->addAxis(AxisDefinition::TOP);
    bar_chart->addAxis(AxisDefinition::RIGHT);
    bar_chart->addAxis(AxisDefinition::BOTTOM)->setTitle("myaxis");
    bar_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testSimplePointChart() {
    Series2D<double, double> series1("myseries1");
    series1.addDatum(10, 34);
    series1.addDatum(15, -18);
    series1.addDatum(20, 43);
    series1.addDatum(30, -19);
    series1.addDatum(40, 25);
    series1.addDatum(50, 33);

    Series2D<double, double> series2("myseries1");
    series2.addDatum(-10, 34);
    series2.addDatum(-15, 18);
    series2.addDatum(-20, -43);
    series2.addDatum(-30, 19);
    series2.addDatum(-40, -25);
    series2.addDatum(-50, -33);

    ui::Canvas canvas;
    auto point_chart = canvas.addChart<PointChart>();
    point_chart->addSeries(&series1);
    point_chart->addSeries(&series2);
    point_chart->addAxis(AxisDefinition::TOP);
    point_chart->addAxis(AxisDefinition::RIGHT);
    point_chart->addAxis(AxisDefinition::BOTTOM);
    point_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testVariableSizePointChart() {
    Series3D<double, double, double> series1("myseries1");
    series1.addDatum(10, 34, 5);
    series1.addDatum(15, -18, 23);
    series1.addDatum(20, 43, 11);
    series1.addDatum(30, -19, 9);
    series1.addDatum(40, 25, 14);
    series1.addDatum(50, 33, 8);

    Series3D<double, double, double> series2("myseries1");
    series2.addDatum(-10, 34, 5);
    series2.addDatum(-15, 18, 8);
    series2.addDatum(-20, -4, 23);
    series2.addDatum(-30, 19, 2);
    series2.addDatum(-40, -25, 4);
    series2.addDatum(-50, -23, 17);

    ui::Canvas canvas;
    auto point_chart = canvas.addChart<PointChart>();
    point_chart->addSeries(&series1);
    point_chart->addSeries(&series2);
    point_chart->addAxis(AxisDefinition::TOP);
    point_chart->addAxis(AxisDefinition::RIGHT);
    point_chart->addAxis(AxisDefinition::BOTTOM);
    point_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testSimpleLineChart() {
    Series2D<double, double> series1("myseries1");
    series1.addDatum(10, 34);
    series1.addDatum(15, 38);
    series1.addDatum(20, 43);
    series1.addDatum(30, 33);
    series1.addDatum(40, 21);
    series1.addDatum(50, 33);

    Series2D<double, double> series2("myseries1");
    series2.addDatum(10, 19);
    series2.addDatum(15, 18);
    series2.addDatum(20, 22);
    series2.addDatum(30, 23);
    series2.addDatum(40, 18);
    series2.addDatum(50, 21);

    ui::NumericalDomain x_domain(10, 50, false);
    ui::NumericalDomain y_domain(0, 50, false);

    ui::Canvas canvas;
    auto line_chart = canvas.addChart<LineChart>(&x_domain, &y_domain);
    line_chart->addSeries(&series1);
    line_chart->addSeries(&series2);
    line_chart->addAxis(AxisDefinition::TOP);
    line_chart->addAxis(AxisDefinition::RIGHT);
    line_chart->addAxis(AxisDefinition::BOTTOM);
    line_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testPointLineChart() {
    Series2D<double, double> series1("myseries1");
    series1.addDatum(10, 34);
    series1.addDatum(15, 38);
    series1.addDatum(20, 43);
    series1.addDatum(30, 33);
    series1.addDatum(40, 21);
    series1.addDatum(50, 33);

    Series2D<double, double> series2("myseries1");
    series2.addDatum(10, 19);
    series2.addDatum(15, 18);
    series2.addDatum(20, 22);
    series2.addDatum(30, 23);
    series2.addDatum(40, 18);
    series2.addDatum(50, 21);

    ui::NumericalDomain x_domain(10, 50, false);
    ui::NumericalDomain y_domain(0, 50, false);

    ui::Canvas canvas;

    auto line_chart = canvas.addChart<LineChart>(&x_domain, &y_domain);
    line_chart->addSeries(&series1, "solid", 2, "circle", 4);
    line_chart->addSeries(&series2, "solid", 2, "circle", 4);
    line_chart->addAxis(AxisDefinition::BOTTOM);
    line_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

  void testMultiChart() {
    Series2D<double, double> series1("myseries1");
    series1.addDatum(10, 34);
    series1.addDatum(15, 38);
    series1.addDatum(20, 43);
    series1.addDatum(30, 33);
    series1.addDatum(40, 21);
    series1.addDatum(50, 33);

    Series2D<double, double> series2("myseries1");
    series2.addDatum(10, 19);
    series2.addDatum(15, 18);
    series2.addDatum(20, 22);
    series2.addDatum(30, 23);
    series2.addDatum(40, 18);
    series2.addDatum(50, 21);

    Series2D<std::string, double> series3("myseries");
    series3.addDatum("A", 34);
    series3.addDatum("B", 38);
    series3.addDatum("C", 43);
    series3.addDatum("D", 33);
    series3.addDatum("E", 21);
    series3.addDatum("F", 33);

    Series2D<std::string, double> series4("myseries");
    series4.addDatum("A", 19);
    series4.addDatum("B", 18);
    series4.addDatum("C", 22);
    series4.addDatum("D", 23);
    series4.addDatum("E", 18);
    series4.addDatum("F", 21);

    ui::NumericalDomain x_domain(10, 50, false);
    ui::NumericalDomain y_domain(0, 50, false);
    ui::NumericalDomain bar_domain(0, 300, false);

    ui::Canvas canvas;

    auto line_chart = canvas.addChart<LineChart>(&x_domain, &y_domain);
    line_chart->addSeries(&series1);
    line_chart->addSeries(&series2);
    line_chart->addAxis(AxisDefinition::TOP)->setTitle("x1");
    line_chart->addAxis(AxisDefinition::LEFT)->setTitle("y1");

    auto point_chart = canvas.addChart<PointChart>(&x_domain, &y_domain);
    point_chart->addSeries(&series1, "circle", 4);
    point_chart->addSeries(&series2, "circle", 4);

    auto bar_chart = canvas.addChart<BarChart>(
        BarChart::O_VERTICAL,
        false,
        &bar_domain);

    bar_chart->addSeries(&series3);
    bar_chart->addSeries(&series4);
    bar_chart->addAxis(AxisDefinition::RIGHT)->setTitle("x2");
    bar_chart->addAxis(AxisDefinition::BOTTOM)->setTitle("y2");

    auto svg = canvas.renderSVG();
  }

  void testSimpleAreaChart() {
    Series2D<double, double> series1("myseries1");
    series1.addDatum(10, 34);
    series1.addDatum(15, 38);
    series1.addDatum(20, 43);
    series1.addDatum(30, 33);
    series1.addDatum(40, 21);
    series1.addDatum(50, 33);

    ui::NumericalDomain x_domain(10, 50, false);
    ui::NumericalDomain y_domain(0, 50, false);

    ui::Canvas canvas;
    auto line_chart = canvas.addChart<AreaChart>(&x_domain, &y_domain);
    line_chart->addSeries(&series1, "solid", 2, "circle", 4);
    line_chart->addAxis(AxisDefinition::TOP);
    line_chart->addAxis(AxisDefinition::RIGHT);
    line_chart->addAxis(AxisDefinition::BOTTOM);
    line_chart->addAxis(AxisDefinition::LEFT);

    auto svg = canvas.renderSVG();
  }

};

}

int main() {
  fnordmetric::UITest test;
  test.run();
  //printf("all tests passed! :)\n");
}
