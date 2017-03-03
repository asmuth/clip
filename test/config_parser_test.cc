/**
 * This file is part of the "FnordTable" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordTable is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "unittest.h"
#include "fnordmetric/config_parser.h"

using namespace fnordmetric;

UNIT_TEST(ConfigParserTest);

TEST_CASE(ConfigParserTest, TestTokenize, [] () {
  std::string confstr =
      R"(metric users_online {
        summarize_group sum
      })";

  ConfigParser parser(confstr.data(), confstr.size());
  ConfigParser::TokenType ttype;
  std::string tbuf;

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == ConfigParser::T_STRING);
  EXPECT(tbuf == "metric");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == ConfigParser::T_STRING);
  EXPECT(tbuf == "users_online");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == ConfigParser::T_LCBRACE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == ConfigParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == ConfigParser::T_STRING);
  EXPECT(tbuf == "summarize_group");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == ConfigParser::T_STRING);
  EXPECT(tbuf == "sum");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == ConfigParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == ConfigParser::T_RCBRACE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == false);
});

//TEST_CASE(ConfigParserTest, TestParseSensorThreads, [] () {
//  std::string confstr = R"(sensor_threads 23)";
//
//  ConfigList config;
//  ConfigParser parser(confstr.data(), confstr.size());
//  auto rc = parser.parse(&config);
//  EXPECT(rc.isSuccess());
//  EXPECT(config.getSensorThreads() == 23);
//});

TEST_CASE(ConfigParserTest, TestParseTableIntervalStanza, [] () {
  std::string confstr =
      R"(table users_online {
        interval 1m
      })";

  ConfigList config;
  ConfigParser parser(confstr.data(), confstr.size());
  auto rc = parser.parse(&config);
  EXPECT(rc.isSuccess());

  EXPECT(config.getTableConfigs().size() == 1);
  {
    auto mc = config.getTableConfig("users_online");
    EXPECT(mc != nullptr);
    EXPECT(mc->interval == 60000000);
  }
});

TEST_CASE(ConfigParserTest, TestParseTableLabelStanza, [] () {
  std::string confstr =
      R"(table users_online {
        label datacenter
        label host
      })";

  ConfigList config;
  ConfigParser parser(confstr.data(), confstr.size());
  auto rc = parser.parse(&config);
  EXPECT(rc.isSuccess());

  EXPECT(config.getTableConfigs().size() == 1);
  {
    auto mc = config.getTableConfig("users_online");
    EXPECT(mc != nullptr);
    EXPECT(mc->labels.size() == 2);
    EXPECT(mc->labels[0].column_name == "datacenter");
    EXPECT(mc->labels[0].type == DataType::STRING);
    EXPECT(mc->labels[1].column_name == "host");
    EXPECT(mc->labels[1].type == DataType::STRING);
  }
});

TEST_CASE(ConfigParserTest, TestParseTableMeasureStanza, [] () {
  std::string confstr =
      R"(table users_online {
        measure load_avg max(float64)
        measure request_count sum(uint64)
      })";

  ConfigList config;
  ConfigParser parser(confstr.data(), confstr.size());
  auto rc = parser.parse(&config);
  EXPECT(rc.isSuccess());

  EXPECT(config.getTableConfigs().size() == 1);
  {
    auto mc = config.getTableConfig("users_online");
    EXPECT(mc != nullptr);
    EXPECT(mc->measures.size() == 2);
  }
});

//TEST_CASE(ConfigParserTest, TestParseSensorTableIDRewriteStanza, [] () {
//  std::string confstr =
//      R"(sensor_http sensor1 {
//        metric_id_rewrite ".*" "blah-$1"
//      })";
//
//  ConfigList config;
//  ConfigParser parser(confstr.data(), confstr.size());
//  auto rc = parser.parse(&config);
//  EXPECT(rc.isSuccess());
//
//  EXPECT(config.getSensorConfigs().size() == 1);
//  {
//    auto sc = config.getSensorConfig("sensor1");
//    EXPECT(sc != nullptr);
//    EXPECT(sc->metric_id_rewrite_enabled == true);
//    EXPECT(sc->series_id_rewrite_enabled == false);
//  }
//});
//
//
//TEST_CASE(ConfigParserTest, TestParseSensorSeriesIDRewriteStanza, [] () {
//  std::string confstr =
//      R"(sensor_http sensor1 {
//        series_id_rewrite ".*" "blah-$1"
//      })";
//
//  ConfigList config;
//  ConfigParser parser(confstr.data(), confstr.size());
//  auto rc = parser.parse(&config);
//  EXPECT(rc.isSuccess());
//
//  EXPECT(config.getSensorConfigs().size() == 1);
//  {
//    auto sc = config.getSensorConfig("sensor1");
//    EXPECT(sc != nullptr);
//    EXPECT(sc->series_id_rewrite_enabled == true);
//    EXPECT(sc->metric_id_rewrite_enabled == false);
//  }
//});
//
//TEST_CASE(ConfigParserTest, TestParseHTTPSensor, [] () {
//  std::string confstr =
//      R"(sensor_http sensor1 {
//        http_url "http://example.com/stats"
//      })";
//
//  ConfigList config;
//  ConfigParser parser(confstr.data(), confstr.size());
//  auto rc = parser.parse(&config);
//  EXPECT(rc.isSuccess());
//
//  EXPECT(config.getSensorConfigs().size() == 1);
//  {
//    auto sc = config.getSensorConfig("sensor1");
//    EXPECT(sc != nullptr);
//    EXPECT(sc->sensor_id == "sensor1");
//  }
//});

