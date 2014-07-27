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

using namespace fnordmetric::cli;

UNIT_TEST(CLITest);

static fnordmetric::util::UnitTest::TestCase __test_simple_sql_to_svg_(
    &CLITest, "TestSimpleSQLToSVG", [] () {
  std::vector<std::string> args = {
    "test/fixtures/gdp_bar_chart.sql",
    "-f", "svg",
    "-o", "build/tests/tmp/CLITest_TestSimpleSQLToSVG_out.svg.html"
  };

  FlagParser flag_parser;

  flag_parser.defineFlag(
      "verbose",
      FlagParser::T_SWITCH,
      false,
      "v",
      NULL,
      "Be verbose");

  flag_parser.defineFlag(
      "format",
      FlagParser::T_STRING,
      false,
      "f",
      "human",
      "The output format (svg,csv,human)",
      "<output_format>");

  flag_parser.defineFlag(
      "output",
      FlagParser::T_STRING,
      false,
      "o",
      NULL,
      "The output format (svg,csv,human)",
      "<output_format>");

  flag_parser.parseArgv(args);
  CLI::execute(flag_parser);

  EXPECT_FILES_EQ(
    "test/fixtures/CLITest_TestSimpleSQLToSVG_out.svg.html",
    "build/tests/tmp/CLITest_TestSimpleSQLToSVG_out.svg.html");
});

