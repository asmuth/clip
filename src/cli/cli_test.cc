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
#include <fnordmetric/cli/flagparser.h>
#include <fnordmetric/util/outputstream.h>

using namespace fnordmetric::cli;

UNIT_TEST(CLITest);

static fnordmetric::util::UnitTest::TestCase __test_simple_sql_to_svg_(
    &CLITest, "TestSimpleSQLToSVG", [] () {
  std::vector<std::string> args = {
    "test/fixtures/gdp_bar_chart.sql",
    "-f", "svg",
    "-o", "build/tests/tmp/CLITest_TestSimpleSQLToSVG_out.svg.html"
  };

  auto flag_parser = CLI::getDefaultFlagParser();
  auto error_stream = fnordmetric::util::OutputStream::getStderr();
  flag_parser.parseArgv(args);
  CLI::execute(flag_parser, error_stream.get());

  EXPECT_FILES_EQ(
    "test/fixtures/CLITest_TestSimpleSQLToSVG_out.svg.html",
    "build/tests/tmp/CLITest_TestSimpleSQLToSVG_out.svg.html");
});

