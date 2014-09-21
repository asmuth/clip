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
#include <fnordmetric/sql/parser/parser.h>
#include <fnordmetric/sql/runtime/compile.h>
#include <fnordmetric/sql/runtime/execute.h>
#include <fnordmetric/sql/runtime/runtime.h>
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
  Runtime runtime;
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
  Runtime runtime;
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
  Runtime runtime;
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
  Runtime runtime;
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
  Runtime runtime;
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
  Runtime runtime;
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
  auto title = executeSimpleConstExpression(
      runtime.compiler(),
      title_expr).toString();
  EXPECT_EQ(title, "fnordtitle");
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithSubtitle, [] () {
  Runtime runtime;
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
  auto title = executeSimpleConstExpression(
      runtime.compiler(),
      title_expr).toString();
  EXPECT_EQ(title, "fnordsubtitle");
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithTitleAndSubtitle, [] () {
  Runtime runtime;
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
  auto title = executeSimpleConstExpression(
      runtime.compiler(),
      title_expr).toString();
  EXPECT_EQ(title, "fnordtitle");

  EXPECT(*stmt->getChildren()[1] == ASTNode::T_PROPERTY);
  EXPECT(stmt->getChildren()[1]->getChildren().size() == 1);
  auto subtitle_expr = stmt->getChildren()[1]->getChildren()[0];
  auto subtitle = executeSimpleConstExpression(
      runtime.compiler(),
      subtitle_expr).toString();
  EXPECT_EQ(subtitle, "fnordsubtitle");
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithAxisTitle, [] () {
  Runtime runtime;
  auto parser = parseTestQuery("DRAW BARCHART AXIS LEFT TITLE 'axistitle';");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_AXIS);
  EXPECT(*stmt->getChildren()[0]->getToken() == Token::T_AXIS);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 2);
  EXPECT(
      *stmt->getChildren()[0]->getChildren()[0] == ASTNode::T_AXIS_POSITION);
  EXPECT(*stmt->getChildren()[0]->getChildren()[1] == ASTNode::T_PROPERTY);
  EXPECT(stmt->getChildren()[0]->getChildren()[1]->getChildren().size() == 1);
  auto title_expr = stmt->getChildren()[0]->getChildren()[1]->getChildren()[0];
  auto title = executeSimpleConstExpression(
      runtime.compiler(),
      title_expr).toString();
  EXPECT_EQ(title, "axistitle");
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithAxisLabelPos, [] () {
  Runtime runtime;
  auto parser = parseTestQuery("DRAW BARCHART AXIS LEFT TICKS INSIDE;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_AXIS);
  EXPECT(*stmt->getChildren()[0]->getToken() == Token::T_AXIS);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 2);
  EXPECT(
      *stmt->getChildren()[0]->getChildren()[0] == ASTNode::T_AXIS_POSITION)
  auto labels = stmt->getChildren()[0]->getChildren()[1];
  EXPECT(*labels == ASTNode::T_AXIS_LABELS)
  EXPECT(labels->getChildren().size() == 1);
  EXPECT(*labels->getChildren()[0] == ASTNode::T_PROPERTY);
  EXPECT(labels->getChildren()[0]->getToken() != nullptr);
  EXPECT(*labels->getChildren()[0]->getToken() == Token::T_INSIDE);
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithAxisLabelRotate, [] () {
  Runtime runtime;
  auto parser = parseTestQuery("DRAW BARCHART AXIS LEFT TICKS ROTATE 45;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_AXIS);
  EXPECT(*stmt->getChildren()[0]->getToken() == Token::T_AXIS);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 2);
  EXPECT(
      *stmt->getChildren()[0]->getChildren()[0] == ASTNode::T_AXIS_POSITION)
  auto labels = stmt->getChildren()[0]->getChildren()[1];
  EXPECT(*labels == ASTNode::T_AXIS_LABELS)
  EXPECT(labels->getChildren().size() == 1);
  EXPECT(*labels->getChildren()[0] == ASTNode::T_PROPERTY);
  EXPECT(labels->getChildren()[0]->getToken() != nullptr);
  EXPECT(*labels->getChildren()[0]->getToken() == Token::T_ROTATE);
  EXPECT(labels->getChildren()[0]->getChildren().size() == 1);
  auto deg_expr = labels->getChildren()[0]->getChildren()[0];
  auto deg = executeSimpleConstExpression(
      runtime.compiler(),
      deg_expr).toString();
  EXPECT_EQ(deg, "45");
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithAxisLabelPosAndRotate, [] () {
  Runtime runtime;
  auto parser = parseTestQuery(
      "DRAW BARCHART AXIS LEFT TICKS OUTSIDE ROTATE 45;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_AXIS);
  EXPECT(*stmt->getChildren()[0]->getToken() == Token::T_AXIS);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 2);
  EXPECT(
      *stmt->getChildren()[0]->getChildren()[0] == ASTNode::T_AXIS_POSITION)
  auto labels = stmt->getChildren()[0]->getChildren()[1];
  EXPECT(*labels == ASTNode::T_AXIS_LABELS)
  EXPECT(labels->getChildren().size() == 2);
  EXPECT(*labels->getChildren()[0] == ASTNode::T_PROPERTY);
  EXPECT(labels->getChildren()[0]->getToken() != nullptr);
  EXPECT(*labels->getChildren()[0]->getToken() == Token::T_OUTSIDE);
  EXPECT(*labels->getChildren()[1] == ASTNode::T_PROPERTY);
  EXPECT(labels->getChildren()[1]->getToken() != nullptr);
  EXPECT(*labels->getChildren()[1]->getToken() == Token::T_ROTATE);
  EXPECT(labels->getChildren()[1]->getChildren().size() == 1);
  auto deg_expr = labels->getChildren()[1]->getChildren()[0];
  auto deg = executeSimpleConstExpression(
      runtime.compiler(),
      deg_expr).toString();
  EXPECT_EQ(deg, "45");
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithSimpleLegend, [] () {
  Runtime runtime;
  auto parser = parseTestQuery("DRAW BARCHART LEGEND TOP LEFT INSIDE;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_LEGEND);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 3);
  auto props = stmt->getChildren()[0]->getChildren();
  EXPECT(*props[0] == ASTNode::T_PROPERTY);
  EXPECT(props[0]->getToken() != nullptr);
  EXPECT(*props[0]->getToken() == Token::T_TOP);
  EXPECT(*props[1] == ASTNode::T_PROPERTY);
  EXPECT(props[1]->getToken() != nullptr);
  EXPECT(*props[1]->getToken() == Token::T_LEFT);
  EXPECT(*props[2] == ASTNode::T_PROPERTY);
  EXPECT(props[2]->getToken() != nullptr);
  EXPECT(*props[2]->getToken() == Token::T_INSIDE);
});

TEST_CASE(SQLExtensionsTest, TestDrawStatementWithLegendWithTitle, [] () {
  Runtime runtime;
  auto parser = parseTestQuery(
      "DRAW BARCHART LEGEND TOP LEFT INSIDE TITLE 'fnordylegend';");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_DRAW);
  EXPECT(stmt->getToken() != nullptr);
  EXPECT(*stmt->getToken() == Token::T_BARCHART);
  EXPECT(stmt->getChildren().size() == 1);
  EXPECT(*stmt->getChildren()[0] == ASTNode::T_LEGEND);
  EXPECT(stmt->getChildren()[0]->getChildren().size() == 4);
  auto props = stmt->getChildren()[0]->getChildren();
  EXPECT(*props[0] == ASTNode::T_PROPERTY);
  EXPECT(props[0]->getToken() != nullptr);
  EXPECT(*props[0]->getToken() == Token::T_TOP);
  EXPECT(*props[1] == ASTNode::T_PROPERTY);
  EXPECT(props[1]->getToken() != nullptr);
  EXPECT(*props[1]->getToken() == Token::T_LEFT);
  EXPECT(*props[2] == ASTNode::T_PROPERTY);
  EXPECT(props[2]->getToken() != nullptr);
  EXPECT(*props[2]->getToken() == Token::T_INSIDE);
  EXPECT(props[3]->getChildren().size() == 1);
  auto title_expr = props[3]->getChildren()[0];
  auto title = executeSimpleConstExpression(
      runtime.compiler(),
      title_expr).toString();
  EXPECT_EQ(title, "fnordylegend");
});
