/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnordmetric/util/unittest.h>
#include <fnordmetric/cli/cli.h>

using namespace fnordmetric::cli;

UNIT_TEST(CLITest);

static fnordmetric::util::UnitTest::TestCase __test_simple_sql_to_svg_(
    &CLITest, "TestSimpleSQLToSVG", [] () {
  std::vector<std::string> args = {
    "test/fixtures/gdp_bar_chart.sql",
    "-f", "svg",
    "-o", "build/test/tmp/CLITest_TestSimpleSQLToSVG_out.svg.html"
  };

  CLI::execute(args);

  EXPECT_FILES_EQ(
    "test/fixtures/CLITest_TestSimpleSQLToSVG_out.svg.html",
    "build/test/tmp/CLITest_TestSimpleSQLToSVG_out.svg.html");
});

