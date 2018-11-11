/**
 * This file is part of the "FnordMetric" project
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
#include "signaltk.h"
#include "../unittest.h"
#include "elements/element_spec_parser.h"

using namespace signaltk;

UNIT_TEST(SpecParserTest);

TEST_CASE(SpecParserTest, TestTokenize, [] () {
  std::string confstr =
      R"(element {
        prop: 1;
      })";

  SpecParser parser(confstr.data(), confstr.size());
  SpecParser::TokenType ttype;
  std::string tbuf;

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "element");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_LCBRACE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "prop");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_COLON);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "1");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_SEMICOLON);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_RCBRACE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == false);
});

TEST_CASE(SpecParserTest, TestTokenizeWithQuoting, [] () {
  std::string confstr =
      R"(element {
        prop1: "hello";
        prop2: 'world';
      })";

  SpecParser parser(confstr.data(), confstr.size());
  SpecParser::TokenType ttype;
  std::string tbuf;

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "element");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_LCBRACE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "prop1");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_COLON);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "hello");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_SEMICOLON);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "prop2");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_COLON);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "world");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_SEMICOLON);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_RCBRACE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == false);
});

TEST_CASE(SpecParserTest, TestTokenizeWithComments, [] () {
  std::string confstr =
      R"(element {
        # blah
        prop: 1;
      })";

  SpecParser parser(confstr.data(), confstr.size());
  SpecParser::TokenType ttype;
  std::string tbuf;

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "element");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_LCBRACE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "prop");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_COLON);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_STRING);
  EXPECT(tbuf == "1");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_SEMICOLON);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_ENDLINE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == true);
  EXPECT(ttype == SpecParser::T_RCBRACE);
  EXPECT(tbuf == "");
  parser.consumeToken();

  EXPECT(parser.getToken(&ttype, &tbuf) == false);
});

TEST_CASE(SpecParserTest, TestParseSimple, [] () {
  std::string confstr =
      R"(element {
        prop0: 1337;
        prop1: "hello";
        prop2: 'world';
      })";

  SpecParser parser(confstr.data(), confstr.size());
  PropertyList plist;
  CHECK_RC(parser.parse(&plist));

});

