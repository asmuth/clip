/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnordmetric/sql/parser.h>
#include <fnordmetric/util/unittest.h>
#include <fnordmetric/util/runtimeexception.h>

using namespace fnordmetric::query;

UNIT_TEST(SQLExtensionsTest);

static Parser parseTestQuery(const char* query) {
  Parser parser;
  parser.parse(query, strlen(query));
  return parser;
}

TEST_CASE(SQLExtensionsTest, TestSimpleDrawStatement, [] () {
  auto parser = parseTestQuery("DRAW BARCHART;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getChildren().size() == 0);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
});
