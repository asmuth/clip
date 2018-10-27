/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cplot/axisdefinition.h"
#include "cplot/areachart.h"
#include "cplot/barchart.h"
#include "cplot/canvas.h"
#include "cplot/domain.h"
#include "cplot/linechart.h"
#include "cplot/pointchart.h"
#include "cplot/series.h"
#include "cplot/svgtarget.h"
#include "stx/io/fileutil.h"
#include "stx/test/unittest.h"

UNIT_TEST(ChartTest);

using stx::chart::AnyDomain;
using stx::chart::AxisDefinition;
using stx::chart::AreaChart;
using stx::chart::AreaChart2D;
using stx::chart::AreaChart3D;
using stx::chart::BarChart;
using stx::chart::BarChart2D;
using stx::chart::BarChart3D;
using stx::chart::LineChart2D;
using stx::chart::PointChart2D;
using stx::chart::PointChart3D;
using stx::chart::Canvas;
using stx::chart::ContinuousDomain;
using stx::chart::DomainProvider;
using stx::chart::Series;
using stx::chart::Series2D;
using stx::chart::Series3D;
using stx::chart::SVGTarget;
using stx::FileOutputStream;
using stx::FileUtil;
using stx::test::UnitTest;

static void compareChart(
    Canvas* chart,
    const std::string& file_name) {
  auto output_stream = FileOutputStream::openFile(
      FileUtil::joinPaths(UnitTest::tempFilePath(), file_name));

  SVGTarget target(output_stream.get());
  chart->render(&target);

  EXPECT_FILES_EQ(
      FileUtil::joinPaths(UnitTest::tempFilePath(), file_name),
      FileUtil::joinPaths(
          UnitTest::testDataPath(), "chart/testdata/" + file_name));
}

TEST_CASE(ChartTest, TestCanvasWithLeftAxis, [] () {
  Canvas canvas;

  auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
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

  compareChart(
      &canvas,
      "ChartTest_TestCanvasWithLeftAxis_out.svg.html");
});

TEST_CASE(ChartTest, TestCanvasWithLeftAxisAndTitle, [] () {
  Canvas canvas;

  auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
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

  compareChart(
      &canvas,
      "ChartTest_TestCanvasWithLeftAxisAndTitle_out.svg.html");
});

TEST_CASE(ChartTest, TestCanvasWithLeftAndBottomAxis, [] () {
  Canvas canvas;

  auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
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

  auto axis_bottom = canvas.addAxis(AxisDefinition::BOTTOM);
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

  compareChart(
      &canvas,
      "ChartTest_TestCanvasWithLeftAndBottomAxis_out.svg.html");
});

TEST_CASE(ChartTest, TestCanvasWithAllAxis, [] () {
  Canvas canvas;

  auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
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

  auto axis_right = canvas.addAxis(AxisDefinition::RIGHT);
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

  auto axis_bottom = canvas.addAxis(AxisDefinition::BOTTOM);
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

  auto axis_top = canvas.addAxis(AxisDefinition::TOP);
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

  compareChart(
      &canvas,
      "ChartTest_TestCanvasWithAllAxis_out.svg.html");
});

TEST_CASE(ChartTest, TestCanvasWithAllMultiAxis, [] () {
  Canvas canvas;

  {
    auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
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
    auto axis_left = canvas.addAxis(AxisDefinition::LEFT);
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
    auto axis_right = canvas.addAxis(AxisDefinition::RIGHT);
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
    auto axis_right = canvas.addAxis(AxisDefinition::RIGHT);
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
    auto axis_bottom = canvas.addAxis(AxisDefinition::BOTTOM);
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
    auto axis_bottom = canvas.addAxis(AxisDefinition::BOTTOM);
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
    auto axis_top = canvas.addAxis(AxisDefinition::TOP);
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
    auto axis_top = canvas.addAxis(AxisDefinition::TOP);
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

  compareChart(
      &canvas,
      "ChartTest_TestCanvasWithAllMultiAxis_out.svg.html");
});

TEST_CASE(ChartTest, TestCanvasWithMultiLeftAxis, [] () {
  Canvas canvas;

  auto axis1 = canvas.addAxis(AxisDefinition::LEFT);
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

  auto axis2 = canvas.addAxis(AxisDefinition::LEFT);
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

  compareChart(
      &canvas,
      "ChartTest_TestCanvasWithMultiLeftAxis_out.svg.html");
});

TEST_CASE(ChartTest, TestCanvasWithAxisFromNumericalDomain, [] () {
  Canvas canvas;

  ContinuousDomain<double> domain(0, 100);
  auto axis1 = canvas.addAxis(AxisDefinition::LEFT);
  DomainProvider domain_adapter(&domain);
  axis1->setDomain(&domain_adapter);
  axis1->setTitle("numerical domain");

  compareChart(
      &canvas,
      "ChartTest_TestCanvasWithAxisFromNumericalDomain_out.svg.html");
});


static stx::test::UnitTest::TestCase __test_simple_bar_chart_(
    &ChartTest, "TestSimpleBarChart", [] () {
  auto series = new Series2D<std::string, double>("myseries");

  series->addDatum("A", 34);
  series->addDatum("B", 18);
  series->addDatum("C", 43);
  series->addDatum("D", 19);
  series->addDatum("E", 25);
  series->addDatum("F", 33);

  Canvas canvas;
  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
      BarChart::O_VERTICAL);

  bar_chart->addSeries(series);
  bar_chart->addAxis(AxisDefinition::TOP);
  bar_chart->addAxis(AxisDefinition::RIGHT);
  bar_chart->addAxis(AxisDefinition::BOTTOM);
  bar_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestSimpleBarChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_mulitseries_bar_chart_(
    &ChartTest, "TestMultiSeriesBarChart", [] () {
  auto series1 = new Series2D<std::string, double>("myseries1");
  series1->addDatum("A", 40);
  series1->addDatum("B", 35);
  series1->addDatum("C", 30);
  series1->addDatum("D", 25);
  series1->addDatum("E", 20);
  series1->addDatum("F", 15);

  auto series2 = new Series2D<std::string, double>("myseries2");
  series2->addDatum("A", 23);
  series2->addDatum("B", 65);
  series2->addDatum("C", 43);
  series2->addDatum("D", 12);
  series2->addDatum("E", 54);
  series2->addDatum("F", 31);

  Canvas canvas;
  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
      BarChart::O_VERTICAL);

  bar_chart->addSeries(series1);
  bar_chart->addSeries(series2);
  bar_chart->addAxis(AxisDefinition::TOP);
  bar_chart->addAxis(AxisDefinition::RIGHT);
  bar_chart->addAxis(AxisDefinition::BOTTOM);
  bar_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestMultiSeriesBarChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_stacked_bar_chart_(
    &ChartTest, "TestStackedBarChart", [] () {
  auto series1 = new Series2D<std::string, double>("myseries1");
  series1->addDatum("A", 40);
  series1->addDatum("B", 35);
  series1->addDatum("C", 30);
  series1->addDatum("D", 25);
  series1->addDatum("E", 20);
  series1->addDatum("F", 15);

  auto series2 = new Series2D<std::string, double>("myseries2");
  series2->addDatum("A", 23);
  series2->addDatum("B", 65);
  series2->addDatum("C", 43);
  series2->addDatum("D", 12);
  series2->addDatum("E", 54);
  series2->addDatum("F", 31);

  Canvas canvas;
  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
      BarChart::O_VERTICAL, true);

  bar_chart->addSeries(series1);
  bar_chart->addSeries(series2);
  bar_chart->addAxis(AxisDefinition::TOP);
  bar_chart->addAxis(AxisDefinition::RIGHT);
  bar_chart->addAxis(AxisDefinition::BOTTOM);
  bar_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestStackedBarChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_horizontal_bar_chart_(
    &ChartTest, "TestHorizontalBarChart", [] () {
  auto series1 = new Series2D<std::string, double>("myseries1");
  series1->addDatum("A", 40);
  series1->addDatum("B", 35);
  series1->addDatum("C", 30);
  series1->addDatum("D", 25);
  series1->addDatum("E", 20);
  series1->addDatum("F", 15);

  Canvas canvas;
  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
      BarChart::O_HORIZONTAL);

  bar_chart->addSeries(series1);
  bar_chart->addAxis(AxisDefinition::TOP);
  bar_chart->addAxis(AxisDefinition::RIGHT);
  bar_chart->addAxis(AxisDefinition::BOTTOM);
  bar_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestHorizontalBarChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_horizontal_mulit_bar_chart_(
    &ChartTest, "TestHorizontalMulitSeriesBarChart", [] () {
  auto series1 = new Series2D<std::string, double>("myseries1");
  series1->addDatum("A", 40);
  series1->addDatum("B", 35);
  series1->addDatum("C", 30);
  series1->addDatum("D", 25);
  series1->addDatum("E", 20);
  series1->addDatum("F", 15);

  auto series2 = new Series2D<std::string, double>("myseries2");
  series2->addDatum("A", 23);
  series2->addDatum("B", 65);
  series2->addDatum("C", 43);
  series2->addDatum("D", 12);
  series2->addDatum("E", 54);
  series2->addDatum("F", 31);

  Canvas canvas;
  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
      BarChart::O_HORIZONTAL);

  bar_chart->addSeries(series1);
  bar_chart->addSeries(series2);
  bar_chart->addAxis(AxisDefinition::TOP);
  bar_chart->addAxis(AxisDefinition::RIGHT);
  bar_chart->addAxis(AxisDefinition::BOTTOM);
  bar_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestHorizontalMulitSeriesBarChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_horiz_stacked_bar_chart_(
    &ChartTest, "TestHorizontalStackedBarChart", [] () {
  auto series1 = new Series2D<std::string, double>("myseries1");
  series1->addDatum("A", 40);
  series1->addDatum("B", 35);
  series1->addDatum("C", 30);
  series1->addDatum("D", 25);
  series1->addDatum("E", 20);
  series1->addDatum("F", 15);

  auto series2 = new Series2D<std::string, double>("myseries2");
  series2->addDatum("A", 23);
  series2->addDatum("B", 65);
  series2->addDatum("C", 43);
  series2->addDatum("D", 12);
  series2->addDatum("E", 54);
  series2->addDatum("F", 31);

  Canvas canvas;
  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
      BarChart::O_HORIZONTAL, true);
  bar_chart->addSeries(series1);
  bar_chart->addSeries(series2);
  bar_chart->addAxis(AxisDefinition::TOP);
  bar_chart->addAxis(AxisDefinition::RIGHT);
  bar_chart->addAxis(AxisDefinition::BOTTOM);
  bar_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestHorizontalStackedBarChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_range_bar_chart_(
    &ChartTest, "TestRangeBarChart", [] () {
  auto series1 = new Series3D<std::string, double, double>("myseries1");
  series1->addDatum("A", -40, +40);
  series1->addDatum("B", -35, +35);
  series1->addDatum("C", -30, +30);
  series1->addDatum("D", -25, +25);
  series1->addDatum("E", -20, +20);
  series1->addDatum("F", -15, +15);

  auto series2 = new Series3D<std::string, double, double>("myseries2");
  series2->addDatum("A", -23, +23);
  series2->addDatum("B", -65, +65);
  series2->addDatum("C", -43, +43);
  series2->addDatum("D", -12, +12);
  series2->addDatum("E", -54, +54);
  series2->addDatum("F", -31, +31);

  Canvas canvas;
  auto bar_chart = canvas.addChart<BarChart3D<std::string, double, double>>(
      BarChart::O_VERTICAL);
  bar_chart->addSeries(series1);
  bar_chart->addSeries(series2);
  bar_chart->addAxis(AxisDefinition::TOP);
  bar_chart->addAxis(AxisDefinition::RIGHT);
  bar_chart->addAxis(AxisDefinition::BOTTOM);
  bar_chart->addAxis(AxisDefinition::LEFT)->setTitle("myaxis");

  compareChart(
      &canvas,
      "ChartTest_TestRangeBarChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_horiz_range_bar_chart_(
    &ChartTest, "TestHorizontalRangeBarChart", [] () {
  auto series1 = new Series3D<std::string, double, double>("myseries1");
  series1->addDatum("A", -40, +40);
  series1->addDatum("B", -35, +35);
  series1->addDatum("C", -30, +30);
  series1->addDatum("D", -25, +25);
  series1->addDatum("E", -20, +20);
  series1->addDatum("F", -15, +15);

  auto series2 = new Series3D<std::string, double, double>("myseries2");
  series2->addDatum("A", -23, +23);
  series2->addDatum("B", -65, +65);
  series2->addDatum("C", -43, +43);
  series2->addDatum("D", -12, +12);
  series2->addDatum("E", -54, +54);
  series2->addDatum("F", -31, +31);

  Canvas canvas;
  auto bar_chart = canvas.addChart<BarChart3D<std::string, double, double>>(
      BarChart::O_HORIZONTAL);
  bar_chart->addSeries(series1);
  bar_chart->addSeries(series2);
  bar_chart->addAxis(AxisDefinition::TOP);
  bar_chart->addAxis(AxisDefinition::RIGHT);
  bar_chart->addAxis(AxisDefinition::BOTTOM)->setTitle("myaxis");
  bar_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestHorizontalRangeBarChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_simple_point_chart_(
    &ChartTest, "TestSimplePointChart", [] () {
  auto series1 = new Series2D<double, double>("myseries1");
  series1->addDatum(10, 34);
  series1->addDatum(15, -18);
  series1->addDatum(20, 43);
  series1->addDatum(30, -19);
  series1->addDatum(40, 25);
  series1->addDatum(50, 33);

  auto series2 = new Series2D<double, double>("myseries1");
  series2->addDatum(-10, 34);
  series2->addDatum(-15, 18);
  series2->addDatum(-20, -43);
  series2->addDatum(-30, 19);
  series2->addDatum(-40, -25);
  series2->addDatum(-50, -33);

  Canvas canvas;
  auto point_chart = canvas.addChart<PointChart2D<double, double>>();
  point_chart->addSeries(series1);
  point_chart->addSeries(series2);
  point_chart->addAxis(AxisDefinition::TOP);
  point_chart->addAxis(AxisDefinition::RIGHT);
  point_chart->addAxis(AxisDefinition::BOTTOM);
  point_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestSimplePointChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_variablesize_point_chart_(
    &ChartTest, "TestVariableSizePointChart", [] () {
  auto series1 = new Series3D<double, double, double>("myseries1");
  series1->addDatum(10, 34, 5);
  series1->addDatum(15, -18, 23);
  series1->addDatum(20, 43, 11);
  series1->addDatum(30, -19, 9);
  series1->addDatum(40, 25, 14);
  series1->addDatum(50, 33, 8);

  auto series2 = new Series3D<double, double, double>("myseries1");
  series2->addDatum(-10, 34, 5);
  series2->addDatum(-15, 18, 8);
  series2->addDatum(-20, -4, 23);
  series2->addDatum(-30, 19, 2);
  series2->addDatum(-40, -25, 4);
  series2->addDatum(-50, -23, 17);

  Canvas canvas;
  auto point_chart = canvas.addChart<PointChart3D<double, double, double>>();
  point_chart->addSeries(series1);
  point_chart->addSeries(series2);
  point_chart->addAxis(AxisDefinition::TOP);
  point_chart->addAxis(AxisDefinition::RIGHT);
  point_chart->addAxis(AxisDefinition::BOTTOM);
  point_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestVariableSizePointChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_simple_line_chart_(
    &ChartTest, "TestSimpleLineChart", [] () {
  auto series1 = new Series2D<double, double>("myseries1");
  series1->addDatum(10, 34);
  series1->addDatum(15, 38);
  series1->addDatum(20, 43);
  series1->addDatum(30, 33);
  series1->addDatum(40, 21);
  series1->addDatum(50, 33);

  auto series2 = new Series2D<double, double>("myseries1");
  series2->addDatum(10, 19);
  series2->addDatum(15, 18);
  series2->addDatum(20, 22);
  series2->addDatum(30, 23);
  series2->addDatum(40, 18);
  series2->addDatum(50, 21);

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(0, 50, false);

  Canvas canvas;
  auto line_chart = canvas.addChart<LineChart2D<double, double>>(
      &x_domain, &y_domain);
  line_chart->addSeries(series1);
  line_chart->addSeries(series2);
  line_chart->addAxis(AxisDefinition::TOP);
  line_chart->addAxis(AxisDefinition::RIGHT);
  line_chart->addAxis(AxisDefinition::BOTTOM);
  line_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestSimpleLineChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_point_line_chart_(
    &ChartTest, "TestPointLineChart", [] () {
  auto series1 = new Series2D<double, double>("myseries1");
  series1->addDatum(10, 34);
  series1->addDatum(15, 38);
  series1->addDatum(20, 43);
  series1->addDatum(30, 33);
  series1->addDatum(40, 21);
  series1->addDatum(50, 33);
  series1->setDefaultProperty(Series::P_POINT_STYLE, "circle");

  auto series2 = new Series2D<double, double>("myseries1");
  series2->addDatum(10, 19);
  series2->addDatum(15, 18);
  series2->addDatum(20, 22);
  series2->addDatum(30, 23);
  series2->addDatum(40, 18);
  series2->addDatum(50, 21);
  series2->setDefaultProperty(Series::P_POINT_STYLE, "circle");

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(0, 50, false);

  Canvas canvas;
  auto line_chart = canvas.addChart<LineChart2D<double, double>>(
      &x_domain, &y_domain);

  line_chart->addSeries(series1);
  line_chart->addSeries(series2);
  line_chart->addAxis(AxisDefinition::BOTTOM);
  line_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestPointLineChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_multi_chart_(
    &ChartTest, "TestMultiChart", [] () {
  auto series1 = new Series2D<double, double>("myseries1");
  series1->addDatum(10, 34);
  series1->addDatum(15, 38);
  series1->addDatum(20, 43);
  series1->addDatum(30, 33);
  series1->addDatum(40, 21);
  series1->addDatum(50, 33);

  auto series2 = new Series2D<double, double>("myseries1");
  series2->addDatum(10, 19);
  series2->addDatum(15, 18);
  series2->addDatum(20, 22);
  series2->addDatum(30, 23);
  series2->addDatum(40, 18);
  series2->addDatum(50, 21);

  auto series3 = new Series2D<std::string, double>("myseries");
  series3->addDatum("A", 34);
  series3->addDatum("B", 38);
  series3->addDatum("C", 43);
  series3->addDatum("D", 33);
  series3->addDatum("E", 21);
  series3->addDatum("F", 33);

  auto series4 = new Series2D<std::string, double>("myseries");
  series4->addDatum("A", 19);
  series4->addDatum("B", 18);
  series4->addDatum("C", 22);
  series4->addDatum("D", 23);
  series4->addDatum("E", 18);
  series4->addDatum("F", 21);

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(0, 50, false);

  Canvas canvas;

  auto line_chart = canvas.addChart<LineChart2D<double, double>>(
      &x_domain, &y_domain);

  line_chart->addSeries(series1);
  line_chart->addSeries(series2);
  line_chart->addAxis(AxisDefinition::TOP)->setTitle("x1");
  line_chart->addAxis(AxisDefinition::LEFT)->setTitle("y1");

  auto point_chart = canvas.addChart<PointChart2D<double, double>>(
      &x_domain, &y_domain);
  point_chart->addSeries(series1);
  point_chart->addSeries(series2);

  auto bar_chart = canvas.addChart<BarChart2D<std::string, double>>(
      BarChart::O_VERTICAL,
      false);

  bar_chart->addSeries(series3);
  bar_chart->addSeries(series4);
  bar_chart->addAxis(AxisDefinition::RIGHT)->setTitle("x2");
  bar_chart->addAxis(AxisDefinition::BOTTOM)->setTitle("y2");

  auto bar_domain = dynamic_cast<ContinuousDomain<double>*>(
      bar_chart->getDomain(AnyDomain::DIM_Y));
  EXPECT_TRUE(bar_domain != nullptr);
  bar_domain->setMin(0);
  bar_domain->setMax(200);

  compareChart(
      &canvas,
      "ChartTest_TestMultiChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_simple_area_chart_(
    &ChartTest, "TestSimpleAreaChart", [] () {
  auto series1 = new Series2D<double, double>("myseries1");
  series1->addDatum(10, 34);
  series1->addDatum(15, 38);
  series1->addDatum(20, 43);
  series1->addDatum(30, 33);
  series1->addDatum(40, 21);
  series1->addDatum(50, 33);
  series1->setDefaultProperty(Series::P_LINE_STYLE, "solid");
  series1->setDefaultProperty(Series::P_POINT_STYLE, "circle");

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(0, 50, false);

  Canvas canvas;
  auto area_chart = canvas.addChart<AreaChart2D<double, double>>(
      &x_domain, &y_domain);
  area_chart->addSeries(series1);
  area_chart->addAxis(AxisDefinition::TOP);
  area_chart->addAxis(AxisDefinition::RIGHT);
  area_chart->addAxis(AxisDefinition::BOTTOM);
  area_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestSimpleAreaChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_range_area_chart_(
    &ChartTest, "TestRangeAreaChart", [] () {
  auto series1 = new Series3D<double, double, double>("myseries1");
  series1->addDatum(10, -34, 34);
  series1->addDatum(15, -30, 38);
  series1->addDatum(20, -20, 43);
  series1->addDatum(30, -24, 33);
  series1->addDatum(40, -18, 21);
  series1->addDatum(50, -32, 33);
  series1->setDefaultProperty(Series::P_LINE_STYLE, "solid");
  series1->setDefaultProperty(Series::P_POINT_STYLE, "circle");

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(-50, 50, false);

  Canvas canvas;
  auto area_chart = canvas.addChart<AreaChart3D<double, double, double>>(
      &x_domain, &y_domain);
  area_chart->addSeries(series1);
  area_chart->addAxis(AxisDefinition::TOP);
  area_chart->addAxis(AxisDefinition::RIGHT);
  area_chart->addAxis(AxisDefinition::BOTTOM);
  area_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestRangeAreaChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_multi_range_area_chart_(
    &ChartTest, "TestMultiRangeAreaChart", [] () {
  auto series1 = new Series3D<double, double, double>("myseries1");
  series1->addDatum(10, -34, 34);
  series1->addDatum(15, -30, 38);
  series1->addDatum(20, -20, 43);
  series1->addDatum(30, -24, 33);
  series1->addDatum(40, -18, 21);
  series1->addDatum(50, -32, 33);

  auto series2 = new Series3D<double, double, double>("myseries2");
  series2->addDatum(10, -24, 31);
  series2->addDatum(15, -20, 33);
  series2->addDatum(20, -15, 38);
  series2->addDatum(30, -20, 27);
  series2->addDatum(40, -7, 13);
  series2->addDatum(50, -18, 30);

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(-50, 50, false);

  Canvas canvas;
  auto area_chart = canvas.addChart<AreaChart3D<double, double, double>>(
      &x_domain, &y_domain);
  area_chart->addSeries(series1);
  area_chart->addSeries(series2);
  area_chart->addAxis(AxisDefinition::TOP);
  area_chart->addAxis(AxisDefinition::RIGHT);
  area_chart->addAxis(AxisDefinition::BOTTOM);
  area_chart->addAxis(AxisDefinition::LEFT);

  compareChart(
      &canvas,
      "ChartTest_TestMultiRangeAreaChart_out.svg.html");
});

static stx::test::UnitTest::TestCase __test_multi_range_area_line_(
    &ChartTest, "TestMultiRangeAreaLineChart", [] () {
  auto series1 = new Series3D<double, double, double>("myseries1");
  series1->addDatum(10, -34, 34);
  series1->addDatum(15, -30, 38);
  series1->addDatum(20, -20, 43);
  series1->addDatum(30, -24, 33);
  series1->addDatum(40, -18, 21);
  series1->addDatum(50, -32, 33);

  auto series2 = new Series3D<double, double, double>("myseries2");
  series2->addDatum(10, -24, 31);
  series2->addDatum(15, -20, 33);
  series2->addDatum(20, -15, 38);
  series2->addDatum(30, -20, 27);
  series2->addDatum(40, -7, 13);
  series2->addDatum(50, -18, 30);

  auto series3 = new Series2D<double, double>("myseries3");
  series3->addDatum(10, 4);
  series3->addDatum(15, 8);
  series3->addDatum(20, 13);
  series3->addDatum(30, 2);
  series3->addDatum(40, 1);
  series3->addDatum(50, 9);
  series3->setDefaultProperty(Series::P_COLOR, "black");

  ContinuousDomain<double> x_domain(10, 50, false);
  ContinuousDomain<double> y_domain(-50, 50, false);

  Canvas canvas;
  auto area_chart = canvas.addChart<AreaChart3D<double, double, double>>(
      &x_domain, &y_domain);
  area_chart->addSeries(series1);
  area_chart->addSeries(series2);
  area_chart->addAxis(AxisDefinition::TOP);
  area_chart->addAxis(AxisDefinition::RIGHT);
  area_chart->addAxis(AxisDefinition::BOTTOM);
  area_chart->addAxis(AxisDefinition::LEFT);

  auto line_chart = canvas.addChart<LineChart2D<double, double>>(
      &x_domain, &y_domain);
  line_chart->addSeries(series3);

  compareChart(
      &canvas,
      "ChartTest_TestMultiRangeAreaLineChart_out.svg.html");
});
