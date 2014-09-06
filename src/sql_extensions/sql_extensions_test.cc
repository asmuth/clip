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
#include <fnordmetric/sql/compile.h>
#include <fnordmetric/sql/execute.h>
#include <fnordmetric/sql/parser.h>
#include <fnordmetric/sql/svalue.h>
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
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 0);
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithAxes, [] () {
  auto parser = parseTestQuery("DRAW BARCHART AXIS LEFT AXIS RIGHT;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 2);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_AXIS);
  EXPECT(*stmt->getChildren()[1] == ASTNode::T_AXIS);
  EXPECT(*stmt->getChildren()[0]->getToken() == Token::T_AXIS);
  EXPECT(*stmt->getChildren()[1]->getToken() == Token::T_AXIS);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 1);
  EXPECT(stmt->getChildren()[1]->getChildren().size() == 1);
  EXPECT(stmt->getChildren()[0]->getToken() != nullptr);
  EXPECT(stmt->getChildren()[1]->getToken() != nullptr);
  EXPECT(
      *stmt->getChildren()[0]->getChildren()[0] == ASTNode::T_AXIS_POSITION);
  EXPECT(
      *stmt->getChildren()[1]->getChildren()[0] == ASTNode::T_AXIS_POSITION);
  EXPECT(stmt->getChildren()[0]->getChildren()[0]->getToken() != nullptr);
  EXPECT(stmt->getChildren()[1]->getChildren()[0]->getToken() != nullptr);
  EXPECT(
      *stmt->getChildren()[0]->getChildren()[0]->getToken() == Token::T_LEFT);
  EXPECT(
      *stmt->getChildren()[1]->getChildren()[0]->getToken() == Token::T_RIGHT);
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithExplicitYDomain, [] () {
  auto parser = parseTestQuery("DRAW BARCHART YDOMAIN 0, 100;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_DOMAIN);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0]->getChildren()[0] == ASTNode::T_DOMAIN_SCALE);
  EXPECT(stmt->getChildren()[0]->getChildren()[0]->getChildren().size() == 2);
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithScaleLogInvYDomain, [] () {
  auto parser = parseTestQuery(
      "DRAW BARCHART YDOMAIN 0, 100 LOGARITHMIC INVERT;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_DOMAIN);
  EXPECT(stmt->getChildren().size() == 1);
  auto domain = stmt->getChildren()[0];
  EXPECT(domain->getChildren().size() == 3);
  EXPECT(*domain->getChildren()[0] == ASTNode::T_DOMAIN_SCALE);
  EXPECT(domain->getChildren()[0]->getChildren().size() == 2);
  EXPECT(*domain->getChildren()[1] == ASTNode::T_PROPERTY);
  EXPECT(domain->getChildren()[1]->getToken() != nullptr);
  EXPECT(*domain->getChildren()[1]->getToken() == Token::T_LOGARITHMIC);
  EXPECT(*domain->getChildren()[2] == ASTNode::T_PROPERTY);
  EXPECT(domain->getChildren()[2]->getToken() != nullptr);
  EXPECT(*domain->getChildren()[2]->getToken() == Token::T_INVERT);
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithLogInvYDomain, [] () {
  auto parser = parseTestQuery(
      "DRAW BARCHART YDOMAIN LOGARITHMIC INVERT;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_DOMAIN);
  EXPECT(stmt->getChildren().size() == 1);
  auto domain = stmt->getChildren()[0];
  EXPECT(domain->getChildren().size() == 2);
  EXPECT(*domain->getChildren()[0] == ASTNode::T_PROPERTY);
  EXPECT(domain->getChildren()[0]->getToken() != nullptr);
  EXPECT(*domain->getChildren()[0]->getToken() == Token::T_LOGARITHMIC);
  EXPECT(*domain->getChildren()[1] == ASTNode::T_PROPERTY);
  EXPECT(domain->getChildren()[1]->getToken() != nullptr);
  EXPECT(*domain->getChildren()[1]->getToken() == Token::T_INVERT);
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithGrid, [] () {
  auto parser = parseTestQuery(
      "DRAW BARCHART GRID HORIZONTAL VERTICAL;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_GRID);
  auto grid = stmt->getChildren()[0];
  EXPECT(grid->getChildren().size() == 2);
  EXPECT(*grid->getChildren()[0] == ASTNode::T_PROPERTY);
  EXPECT(grid->getChildren()[0]->getToken() != nullptr);
  EXPECT(*grid->getChildren()[0]->getToken() == Token::T_HORIZONTAL);
  EXPECT(*grid->getChildren()[1] == ASTNode::T_PROPERTY);
  EXPECT(grid->getChildren()[1]->getToken() != nullptr);
  EXPECT(*grid->getChildren()[1]->getToken() == Token::T_VERTICAL);
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithTitle, [] () {
  auto parser = parseTestQuery(
      "DRAW BARCHART TITLE 'fnordtitle';");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_PROPERTY);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 1);
  auto title_expr = stmt->getChildren()[0]->getChildren()[0];
  auto title = executeSimpleConstExpression(title_expr).toString();
  EXPECT_EQ(title, "fnordtitle");
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithSubtitle, [] () {
  auto parser = parseTestQuery(
      "DRAW BARCHART SUBTITLE 'fnordsubtitle';");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_PROPERTY);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 1);
  auto title_expr = stmt->getChildren()[0]->getChildren()[0];
  auto title = executeSimpleConstExpression(title_expr).toString();
  EXPECT_EQ(title, "fnordsubtitle");
});


TEST_CASE(SQLExtensionsTest, TestDrawStatementWithTitleAndSubtitle, [] () {
  auto parser = parseTestQuery(
      "DRAW BARCHART TITLE 'fnordtitle' SUBTITLE 'fnordsubtitle';");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 2);

  EXPECT(*stmt->getChildren()[0] == ASTNode::T_PROPERTY);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 1);
  auto title_expr = stmt->getChildren()[0]->getChildren()[0];
  auto title = executeSimpleConstExpression(title_expr).toString();
  EXPECT_EQ(title, "fnordtitle");

  EXPECT(*stmt->getChildren()[1] == ASTNode::T_PROPERTY);
  EXPECT(stmt->getChildren()[1]->getChildren().size() == 1);
  auto subtitle_expr = stmt->getChildren()[1]->getChildren()[0];
  auto subtitle = executeSimpleConstExpression(subtitle_expr).toString();
  EXPECT_EQ(subtitle, "fnordsubtitle");
});

// AXIS LABEL, TICKS
