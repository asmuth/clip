/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "unittest.h"
#include "metricd/config/config_parser.h"

using namespace fnordmetric;

UNIT_TEST(ConfigParserTest);

//TEST_CASE(ConfigParserTest, TestTokenize, [] () {
//  std::string confstr =
//      R"(metric users_online {
//        aggregation sum
//      })";
//
//  ConfigParser parser(confstr.data(), confstr.size());
//  ConfigParser::TokenType ttype;
//  std::string tbuf;
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == true);
//  EXPECT(ttype == ConfigParser::T_STRING);
//  EXPECT(tbuf == "metric");
//  parser.consumeToken();
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == true);
//  EXPECT(ttype == ConfigParser::T_STRING);
//  EXPECT(tbuf == "users_online");
//  parser.consumeToken();
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == true);
//  EXPECT(ttype == ConfigParser::T_LCBRACE);
//  EXPECT(tbuf == "");
//  parser.consumeToken();
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == true);
//  EXPECT(ttype == ConfigParser::T_ENDLINE);
//  EXPECT(tbuf == "");
//  parser.consumeToken();
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == true);
//  EXPECT(ttype == ConfigParser::T_STRING);
//  EXPECT(tbuf == "aggregation");
//  parser.consumeToken();
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == true);
//  EXPECT(ttype == ConfigParser::T_STRING);
//  EXPECT(tbuf == "sum");
//  parser.consumeToken();
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == true);
//  EXPECT(ttype == ConfigParser::T_ENDLINE);
//  EXPECT(tbuf == "");
//  parser.consumeToken();
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == true);
//  EXPECT(ttype == ConfigParser::T_RCBRACE);
//  EXPECT(tbuf == "");
//  parser.consumeToken();
//
//  EXPECT(parser.getToken(&ttype, &tbuf) == false);
//});
//
//TEST_CASE(ConfigParserTest, TestParseMetricDefinition, [] () {
//  std::string confstr =
//      R"(metric users_online {
//        aggregation sum
//        granularity 60
//      })";
//
//  ConfigList config;
//  ConfigParser parser(confstr.data(), confstr.size());
//  auto rc = parser.parse(&config);
//  std::cerr << rc.getMessage() << std::endl;
//  EXPECT(rc.isSuccess());
//
//  EXPECT(config.getMetricConfigs().size() == 1);
//  {
//    auto mc = config.getMetricConfig("users_online");
//    EXPECT(mc != nullptr);
//    EXPECT(mc->aggregation == MetricAggregationType::SUM);
//    EXPECT(mc->granularity == 60);
//  }
//});

TEST_CASE(ConfigParserTest, TestParseMetricUnitStanza, [] () {
  std::string confstr =
      R"(metric users_online {
        unit bytes
      })";

  ConfigList config;
  ConfigParser parser(confstr.data(), confstr.size());
  auto rc = parser.parse(&config);
  EXPECT(rc.isSuccess());

  EXPECT(config.getMetricConfigs().size() == 1);
  {
    auto mc = config.getMetricConfig("users_online");
    EXPECT(mc != nullptr);
    EXPECT(mc->unit_id == "bytes");
  }
});



