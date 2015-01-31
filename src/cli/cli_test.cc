/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <environment.h>
#include <io/fileutil.h>
#include <util/unittest.h>
#include <cli/cli.h>
#include <cli/flagparser.h>
#include <util/outputstream.h>

using namespace fnordmetric::cli;

UNIT_TEST(CLITest);

static fnordmetric::util::UnitTest::TestCase __test_simple_sql_to_svg_(
    &CLITest, "TestSimpleSQLToSVG", [] () {
  fnord::io::FileUtil::mkdir_p("build/tests/tmp");

  std::vector<std::string> args = {
    "test/fixtures/gdp_bar_chart.sql",
    "-f", "svg",
    "-o", "build/tests/tmp/CLITest_TestSimpleSQLToSVG_out.svg.html"
  };

  fnordmetric::Environment myenv;
  CLI::parseArgs(&myenv, args);
  CLI::execute(&myenv);

  EXPECT_FILES_EQ(
    "test/fixtures/CLITest_TestSimpleSQLToSVG_out.svg.html",
    "build/tests/tmp/CLITest_TestSimpleSQLToSVG_out.svg.html");
});

