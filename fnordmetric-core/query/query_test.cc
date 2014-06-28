/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "parser.h"
#include "token.h"
#include "tokenize.h"
#include "query.h"
#include "executable.h"
#include "tableref.h"
#include "tablescan.h"
#include "tablerepository.h"

namespace fnordmetric {
namespace query {

class QueryTest {
public:
  QueryTest() {}

  void run() {
    testTokenizerSimple();
    testTokenizerEscaping();
    testTokenizerAsClause();
    testSelectMustBeFirstAssert();
    testSelectWildcard();
    testSelectTableWildcard();
    testSelectDerivedColumn();
    testSelectDerivedColumnWithTableName();
    testSimpleValueExpression();
    testArithmeticValueExpression();
    testArithmeticValueExpressionParens();
    testArithmeticValueExpressionPrecedence();
    testNegatedValueExpression();
    testMethodCallValueExpression();
    testFromList();
    testWhereClause();
    testGroupByClause();
    testOrderByClause();
    testHavingClause();
    testLimitClause();
    testLimitOffsetClause();
    testComplexQueries();
    testSeriesStatement();
    testDerivedSeriesStatement();
    testSelectOnlyQuery();
    testSimpleTableScanQuery();
    testTableScanWhereQuery();
    testTableScanWhereLimitQuery();
    testTableScanGroupByQuery();
    testTableScanGroupByCountQuery();
    testTableScanGroupBySumQuery();
    testTableScanGroupWithoutGroupClause();
    testNamedSeriesQuery();
    testDerivedSeriesQuery();
    testRenderSeriesQuery();
  }

  Parser parseTestQuery(const char* query) {
    Parser parser;
    parser.parse(query, strlen(query));
    return parser;
  }

  void testSimpleValueExpression() {
    auto parser = parseTestQuery("SELECT 23 + 5.123 FROM sometable;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(*stmt == ASTNode::T_SELECT);
    assert(stmt->getChildren().size() == 2);
    const auto& sl = stmt->getChildren()[0];
    assert(*sl == ASTNode::T_SELECT_LIST);
    assert(sl->getChildren().size() == 1);
    auto derived = sl->getChildren()[0];
    assert(*derived == ASTNode::T_DERIVED_COLUMN);
    assert(derived->getChildren().size() == 1);
    auto expr = derived->getChildren()[0];
    assert(*expr == ASTNode::T_ADD_EXPR);
    assert(expr->getChildren().size() == 2);
    assert(*expr->getChildren()[0] == ASTNode::T_LITERAL);
    assert(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
    assert(*expr->getChildren()[0]->getToken() == "23");
    assert(*expr->getChildren()[1] == ASTNode::T_LITERAL);
    assert(*expr->getChildren()[1]->getToken() == Token::T_NUMERIC);
    assert(*expr->getChildren()[1]->getToken() == "5.123");
    const auto& from = stmt->getChildren()[1];
    assert(*from == ASTNode::T_FROM);
  }

  void testArithmeticValueExpression() {
    auto parser = parseTestQuery("SELECT 1 + 2 / 3;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    auto expr = parser.getStatements()[0]
        ->getChildren()[0]->getChildren()[0]->getChildren()[0];
    assert(*expr == ASTNode::T_ADD_EXPR);
    assert(expr->getChildren().size() == 2);
    assert(*expr->getChildren()[0] == ASTNode::T_LITERAL);
    assert(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
    assert(*expr->getChildren()[0]->getToken() == "1");
    assert(*expr->getChildren()[1] == ASTNode::T_DIV_EXPR);
    assert(expr->getChildren()[1]->getChildren().size() == 2);
  }

  void testArithmeticValueExpressionParens() {
    auto parser = parseTestQuery("SELECT (1 * 2) + 3;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    auto expr = parser.getStatements()[0]
        ->getChildren()[0]->getChildren()[0]->getChildren()[0];
    assert(*expr == ASTNode::T_ADD_EXPR);
    assert(expr->getChildren().size() == 2);
    assert(*expr->getChildren()[0] == ASTNode::T_MUL_EXPR);
    assert(expr->getChildren()[0]->getChildren().size() == 2);
    assert(*expr->getChildren()[1] == ASTNode::T_LITERAL);
    assert(*expr->getChildren()[1]->getToken() == Token::T_NUMERIC);
    assert(*expr->getChildren()[1]->getToken() == "3");
  }

  void testArithmeticValueExpressionPrecedence() {
    {
      auto parser = parseTestQuery("SELECT 1 * 2 + 3;");
      assert(parser.getErrors().size() == 0);
      assert(parser.getStatements().size() == 1);
      auto expr = parser.getStatements()[0]
          ->getChildren()[0]->getChildren()[0]->getChildren()[0];
      assert(*expr == ASTNode::T_ADD_EXPR);
      assert(expr->getChildren().size() == 2);
      assert(*expr->getChildren()[0] == ASTNode::T_MUL_EXPR);
      assert(expr->getChildren()[0]->getChildren().size() == 2);
      assert(*expr->getChildren()[1] == ASTNode::T_LITERAL);
      assert(*expr->getChildren()[1]->getToken() == Token::T_NUMERIC);
      assert(*expr->getChildren()[1]->getToken() == "3");
    }
    {
      auto parser = parseTestQuery("SELECT 1 + 2 * 3;");
      assert(parser.getErrors().size() == 0);
      assert(parser.getStatements().size() == 1);
      auto expr = parser.getStatements()[0]
          ->getChildren()[0]->getChildren()[0]->getChildren()[0];
      assert(*expr == ASTNode::T_ADD_EXPR);
      assert(expr->getChildren().size() == 2);
      assert(*expr->getChildren()[0] == ASTNode::T_LITERAL);
      assert(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
      assert(*expr->getChildren()[0]->getToken() == "1");
      assert(*expr->getChildren()[1] == ASTNode::T_MUL_EXPR);
      assert(expr->getChildren()[1]->getChildren().size() == 2);
    }
  }

  void testMethodCallValueExpression() {
    auto parser = parseTestQuery("SELECT 1 + sum(23, 4 + 1) FROM sometable;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(*stmt == ASTNode::T_SELECT);
    assert(stmt->getChildren().size() == 2);
    const auto& sl = stmt->getChildren()[0];
    assert(*sl == ASTNode::T_SELECT_LIST);
    assert(sl->getChildren().size() == 1);
    auto derived = sl->getChildren()[0];
    assert(*derived == ASTNode::T_DERIVED_COLUMN);
    assert(derived->getChildren().size() == 1);
    auto expr = derived->getChildren()[0];
    assert(*expr == ASTNode::T_ADD_EXPR);
    assert(expr->getChildren().size() == 2);
    assert(*expr->getChildren()[0] == ASTNode::T_LITERAL);
    assert(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
    assert(*expr->getChildren()[0]->getToken() == "1");
    auto mcall = expr->getChildren()[1];
    assert(*mcall == ASTNode::T_METHOD_CALL);
    assert(*mcall->getToken() == Token::T_IDENTIFIER);
    assert(*mcall->getToken() == "sum");
    assert(mcall->getChildren().size() == 2);
    assert(*mcall->getChildren()[0] == ASTNode::T_LITERAL);
    assert(*mcall->getChildren()[0]->getToken() == Token::T_NUMERIC);
    assert(*mcall->getChildren()[0]->getToken() == "23");
    assert(*mcall->getChildren()[1] == ASTNode::T_ADD_EXPR);
    assert(mcall->getChildren()[1]->getChildren().size() == 2);
    const auto& from = stmt->getChildren()[1];
    assert(*from == ASTNode::T_FROM);
  }

  void testNegatedValueExpression() {
    auto parser = parseTestQuery("SELECT -(23 + 5.123) AS fucol FROM tbl;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(*stmt == ASTNode::T_SELECT);
    assert(stmt->getChildren().size() == 2);
    const auto& sl = stmt->getChildren()[0];
    assert(*sl == ASTNode::T_SELECT_LIST);
    assert(sl->getChildren().size() == 1);
    auto derived = sl->getChildren()[0];
    assert(*derived == ASTNode::T_DERIVED_COLUMN);
    assert(derived->getChildren().size() == 2);
    auto neg_expr = derived->getChildren()[0];
    assert(*neg_expr == ASTNode::T_NEGATE_EXPR);
    assert(neg_expr->getChildren().size() == 1);
    auto expr = neg_expr->getChildren()[0];
    assert(*expr == ASTNode::T_ADD_EXPR);
    assert(expr->getChildren().size() == 2);
    assert(*expr->getChildren()[0] == ASTNode::T_LITERAL);
    assert(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
    assert(*expr->getChildren()[0]->getToken() == "23");
    assert(*expr->getChildren()[1] == ASTNode::T_LITERAL);
    assert(*expr->getChildren()[1]->getToken() == Token::T_NUMERIC);
    assert(*expr->getChildren()[1]->getToken() == "5.123");
    auto col_name = derived->getChildren()[1];
    assert(*col_name == ASTNode::T_COLUMN_ALIAS);
    assert(*col_name->getToken() == Token::T_IDENTIFIER);
    assert(*col_name->getToken() == "fucol");
    const auto& from = stmt->getChildren()[1];
    assert(*from == ASTNode::T_FROM);
  }

  void testSelectWildcard() {
    auto parser = parseTestQuery("SELECT * FROM sometable;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(*stmt == ASTNode::T_SELECT);
    assert(stmt->getChildren().size() == 2);
    const auto& sl = stmt->getChildren()[0];
    assert(*sl == ASTNode::T_SELECT_LIST);
    assert(sl->getChildren().size() == 1);
    assert(*sl->getChildren()[0] == ASTNode::T_ALL);
    assert(sl->getChildren()[0]->getToken() == nullptr);
    const auto& from = stmt->getChildren()[1];
    assert(*from == ASTNode::T_FROM);
  }

  void testSelectTableWildcard() {
    auto parser = parseTestQuery("SELECT mytablex.* FROM sometable;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(*stmt == ASTNode::T_SELECT);
    assert(stmt->getChildren().size() == 2);
    const auto& sl = stmt->getChildren()[0];
    assert(*sl == ASTNode::T_SELECT_LIST);
    assert(sl->getChildren().size() == 1);
    const auto& all = sl->getChildren()[0];
    assert(*all == ASTNode::T_ALL);
    assert(all->getToken() != nullptr);
    assert(*all->getToken() == Token::T_IDENTIFIER);
    assert(*all->getToken() == "mytablex");
    const auto& from = stmt->getChildren()[1];
    assert(*from == ASTNode::T_FROM);
  }

  void testSelectDerivedColumn() {
    auto parser = parseTestQuery("SELECT somecol AS another FROM sometable;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(*stmt == ASTNode::T_SELECT);
    assert(stmt->getChildren().size() == 2);
    const auto& sl = stmt->getChildren()[0];
    assert(*sl == ASTNode::T_SELECT_LIST);
    assert(sl->getChildren().size() == 1);
    const auto& derived = sl->getChildren()[0];
    assert(*derived == ASTNode::T_DERIVED_COLUMN);
    assert(derived->getChildren().size() == 2);
    assert(*derived->getChildren()[0] == ASTNode::T_COLUMN_NAME);
    assert(*derived->getChildren()[0]->getToken() == Token::T_IDENTIFIER);
    assert(*derived->getChildren()[0]->getToken() == "somecol");
    assert(*derived->getChildren()[1] == ASTNode::T_COLUMN_ALIAS);
    assert(*derived->getChildren()[1]->getToken() == Token::T_IDENTIFIER);
    assert(*derived->getChildren()[1]->getToken() == "another");
    const auto& from = stmt->getChildren()[1];
    assert(*from == ASTNode::T_FROM);
  }

  void testSelectDerivedColumnWithTableName() {
    auto parser = parseTestQuery("SELECT tbl.col AS another FROM sometable;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(*stmt == ASTNode::T_SELECT);
    assert(stmt->getChildren().size() == 2);
    const auto& sl = stmt->getChildren()[0];
    assert(*sl == ASTNode::T_SELECT_LIST);
    assert(sl->getChildren().size() == 1);
    const auto& derived = sl->getChildren()[0];
    assert(*derived == ASTNode::T_DERIVED_COLUMN);
    assert(derived->getChildren().size() == 2);
    auto tbl = derived->getChildren()[0];
    assert(*tbl == ASTNode::T_TABLE_NAME);
    assert(*tbl->getToken() == Token::T_IDENTIFIER);
    assert(*tbl->getToken() == "tbl");
    assert(tbl->getChildren().size() == 1);
    auto col = tbl->getChildren()[0];
    assert(*col->getToken() == Token::T_IDENTIFIER);
    assert(*col->getToken() == "col");
    assert(*derived->getChildren()[0] == ASTNode::T_TABLE_NAME);
    assert(*derived->getChildren()[0]->getToken() == Token::T_IDENTIFIER);
    assert(*derived->getChildren()[0]->getToken() == "tbl");
    assert(*derived->getChildren()[1] == ASTNode::T_COLUMN_ALIAS);
    assert(*derived->getChildren()[1]->getToken() == Token::T_IDENTIFIER);
    assert(*derived->getChildren()[1]->getToken() == "another");
    const auto& from = stmt->getChildren()[1];
    assert(*from == ASTNode::T_FROM);
  }

  void testSelectMustBeFirstAssert() {
    auto parser = parseTestQuery("GROUP BY SELECT");
    assert(parser.getErrors().size() == 1);
    assert(parser.getErrors()[0].type == Parser::ERR_UNEXPECTED_TOKEN);
  }

  void testFromList() {
    auto parser = parseTestQuery("SELECT a FROM tbl1, tbl2;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    const auto& from = stmt->getChildren()[1];
    assert(*from == ASTNode::T_FROM);
    assert(from->getChildren().size() == 2);
    assert(*from->getChildren()[0] == ASTNode::T_TABLE_NAME);
    assert(*from->getChildren()[0]->getToken() == "tbl1");
    assert(*from->getChildren()[1] == ASTNode::T_TABLE_NAME);
    assert(*from->getChildren()[1]->getToken() == "tbl2");
  }

  void testWhereClause() {
    auto parser = parseTestQuery("SELECT x FROM t WHERE a=1 AND a+1=2 OR b=3;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(stmt->getChildren().size() == 3);
    const auto& where = stmt->getChildren()[2];
    assert(*where == ASTNode::T_WHERE);
    assert(where->getChildren().size() == 1);
    assert(*where->getChildren()[0] == ASTNode::T_OR_EXPR);
  }

  void testGroupByClause() {
    auto parser = parseTestQuery("select count(x), y from t GROUP BY x;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(stmt->getChildren().size() == 3);
    const auto& where = stmt->getChildren()[2];
    assert(*where == ASTNode::T_GROUP_BY);
    assert(where->getChildren().size() == 1);
    assert(*where->getChildren()[0] == ASTNode::T_COLUMN_NAME);
  }

  void testOrderByClause() {
    auto parser = parseTestQuery("select a FROM t ORDER BY a DESC;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(stmt->getChildren().size() == 3);
    const auto& order_by = stmt->getChildren()[2];
    assert(*order_by == ASTNode::T_ORDER_BY);
    assert(order_by->getChildren().size() == 1);
    assert(*order_by->getChildren()[0] == ASTNode::T_SORT_SPEC);
    assert(*order_by->getChildren()[0]->getToken() == Token::T_DESC);
  }

  void testHavingClause() {
    auto parser = parseTestQuery("select a FROM t HAVING 1=1;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(stmt->getChildren().size() == 3);
    const auto& having = stmt->getChildren()[2];
    assert(*having == ASTNode::T_HAVING);
    assert(having->getChildren().size() == 1);
    assert(*having->getChildren()[0] == ASTNode::T_EQ_EXPR);
  }

  void testLimitClause() {
    auto parser = parseTestQuery("select a FROM t LIMIT 10;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(stmt->getChildren().size() == 3);
    const auto& limit = stmt->getChildren()[2];
    assert(*limit == ASTNode::T_LIMIT);
    assert(limit->getChildren().size() == 0);
    assert(*limit->getToken() == "10");
  }

  void testLimitOffsetClause() {
    auto parser = parseTestQuery("select a FROM t LIMIT 10 OFFSET 23;");
    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(stmt->getChildren().size() == 3);
    const auto& limit = stmt->getChildren()[2];
    assert(*limit == ASTNode::T_LIMIT);
    assert(limit->getChildren().size() == 1);
    assert(*limit->getToken() == "10");
    assert(*limit->getChildren()[0] == ASTNode::T_OFFSET);
    assert(*limit->getChildren()[0]->getToken() == "23");
  }

  void testTokenizerEscaping() {
    auto parser = parseTestQuery(" SELECT  fnord,sum(blah) from fubar blah.id"
        "= 'fnor\\'dbar' + 123.5;");
    const auto& tl = parser.token_list_;
    assert(tl.size() == 17);
    assert(tl[0].type_ == Token::T_SELECT);
    assert(tl[1].type_ == Token::T_IDENTIFIER);
    assert(tl[1] == "fnord");
    assert(tl[2].type_ == Token::T_COMMA);
    assert(tl[3].type_ == Token::T_IDENTIFIER);
    assert(tl[3] == "sum");
    assert(tl[4].type_ == Token::T_LPAREN);
    assert(tl[5].type_ == Token::T_IDENTIFIER);
    assert(tl[5] == "blah");
    assert(tl[6].type_ == Token::T_RPAREN);
    assert(tl[7].type_ == Token::T_FROM);
    assert(tl[8].type_ == Token::T_IDENTIFIER);
    assert(tl[8] == "fubar");
    assert(tl[9].type_ == Token::T_IDENTIFIER);
    assert(tl[9] == "blah");
    assert(tl[10].type_ == Token::T_DOT);
    assert(tl[11].type_ == Token::T_IDENTIFIER);
    assert(tl[11] == "id");
    assert(tl[12].type_ == Token::T_EQUAL);
    assert(tl[13].type_ == Token::T_STRING);
    //assert(tl[13] == "fnord'bar"); // FIXPAUL
    assert(tl[14].type_ == Token::T_PLUS);
    assert(tl[15].type_ == Token::T_NUMERIC);
    assert(tl[15] == "123.5");
    assert(tl[16].type_ == Token::T_SEMICOLON);
  }

  void testTokenizerSimple() {
    auto parser = parseTestQuery(" SELECT  fnord,sum(`blah-field`) from fubar"
        " blah.id= \"fn'o=,rdbar\" + 123;");
    auto tl = &parser.token_list_;
    assert((*tl)[0].type_ == Token::T_SELECT);
    assert((*tl)[1].type_ == Token::T_IDENTIFIER);
    assert((*tl)[1] == "fnord");
    assert((*tl)[2].type_ == Token::T_COMMA);
    assert((*tl)[3].type_ == Token::T_IDENTIFIER);
    assert((*tl)[3] == "sum");
    assert((*tl)[4].type_ == Token::T_LPAREN);
    assert((*tl)[5].type_ == Token::T_IDENTIFIER);
    assert((*tl)[5] == "blah-field");
    assert((*tl)[6].type_ == Token::T_RPAREN);
    assert((*tl)[7].type_ == Token::T_FROM);
    assert((*tl)[8].type_ == Token::T_IDENTIFIER);
    assert((*tl)[8] == "fubar");
    assert((*tl)[9].type_ == Token::T_IDENTIFIER);
    assert((*tl)[9] == "blah");
    assert((*tl)[10].type_ == Token::T_DOT);
    assert((*tl)[11].type_ == Token::T_IDENTIFIER);
    assert((*tl)[11] == "id");
    assert((*tl)[12].type_ == Token::T_EQUAL);
    assert((*tl)[13].type_ == Token::T_STRING);
    assert((*tl)[13] == "fn'o=,rdbar");
    assert((*tl)[14].type_ == Token::T_PLUS);
    assert((*tl)[15].type_ == Token::T_NUMERIC);
    assert((*tl)[15] == "123");
    assert((*tl)[16].type_ == Token::T_SEMICOLON);
  }

  void testTokenizerAsClause() {
    auto parser = parseTestQuery(" SELECT fnord As blah from");
    auto tl = &parser.token_list_;
    assert((*tl)[0].type_ == Token::T_SELECT);
    assert((*tl)[1].type_ == Token::T_IDENTIFIER);
    assert((*tl)[1] == "fnord");
    assert((*tl)[2].type_ == Token::T_AS);
    assert((*tl)[3].type_ == Token::T_IDENTIFIER);
    assert((*tl)[3] == "blah");
  }


  void testComplexQueries() {
    std::vector<const char*> queries;
    queries.push_back("SELECT -sum(fnord) + (123 * 4);");
    queries.push_back("SELECT (-blah + sum(fnord) / (123 * 4)) as myfield;");
    queries.push_back(
        "SELECT concat(fnord + 5, -somefunc(myotherfield)) + (123 * 4);");
    queries.push_back(
        "  SELECT"
        "     l_orderkey,"
        "     sum( l_extendedprice * ( 1 - l_discount) ) AS revenue,"
        "     o_orderdate,"
        "     o_shippriority"
        "  FROM"
        "     customer,"
        "     orders,"
        "     lineitem "
        "  WHERE"
        "    c_mktsegment = 'FURNITURE' AND"
        "    c_custkey = o_custkey AND"
        "    l_orderkey = o_orderkey AND"
        "    o_orderdate < \"2013-12-21\" AND"
        "    l_shipdate > \"2014-01-06\""
        "  GROUP BY"
        "    l_orderkey,"
        "    o_orderdate,"
        "    o_shippriority"
        "  ORDER BY"
        "    revenue,"
        "    o_orderdate;");

    for (auto query : queries) {
      auto parser = parseTestQuery(query);
      assert(parser.getErrors().size() == 0);
      assert(parser.getStatements().size() == 1);
    }
  }

  void testSeriesStatement() {
    auto parser = parseTestQuery(
        "  SERIES \"myseries\" FROM"
        "    SELECT * FROM tbl;");

    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(stmt->getChildren().size() == 2);
    assert(*stmt == ASTNode::T_SERIES);

    const auto& name = stmt->getChildren()[0];
    assert(*name == ASTNode::T_SERIES_NAME);
    assert(*name->getToken() == "myseries");

    const auto& select = stmt->getChildren()[1];
    assert(*select == ASTNode::T_SELECT);
    assert(select->getChildren().size() == 2);
  }

  void testDerivedSeriesStatement() {
    auto parser = parseTestQuery(
        "  SERIES fnord FROM"
        "    SELECT fnord, blah FROM tbl;");

    assert(parser.getErrors().size() == 0);
    assert(parser.getStatements().size() == 1);
    const auto& stmt = parser.getStatements()[0];
    assert(stmt->getChildren().size() == 2);
    assert(*stmt == ASTNode::T_SERIES);

    const auto& expr = stmt->getChildren()[0];
    assert(*expr == ASTNode::T_COLUMN_NAME);
    assert(*expr->getToken() == "fnord");

    const auto& select = stmt->getChildren()[1];
    assert(*select == ASTNode::T_SELECT);
    assert(select->getChildren().size() == 2);
  }

  void testSelectOnlyQuery() {
    TableRepository repo;
    std::vector<std::unique_ptr<Query>> dst;

    auto query = Query(
        "  SELECT"
        "    13 + 2 * 5 as fnord,"
        "    2 ^ 2 ^ 3 as fubar,"
        "    13 * (8 % 3) + -5 as baz,"
        "    true one,"
        "    !(true) as two,"
        "    NOT NOT true as three;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumColumns() == 6);
    assert(results.getNumRows() == 1);
    const auto& cols = results.getColumns();
    assert(cols[0] == "fnord");
    assert(cols[1] == "fubar");
    assert(cols[2] == "baz");
    assert(cols[3] == "one");
    assert(cols[4] == "two");
    assert(cols[5] == "three");
    const auto& row = results.getRow(0);
    assert(row[0]->getInteger() == 23);
    assert(row[1]->getInteger() == 256);
    assert(row[2]->getInteger() == 21);
    assert(row[3]->getBool() == true);
    assert(row[4]->getBool() == false);
    assert(row[5]->getBool() == true);
  }

  class TestTableRef : public TableRef {
    int getColumnIndex(const std::string& name) override {
      if (name == "one") return 0;
      if (name == "two") return 1;
      if (name == "three") return 2;
      return -1;
    }
    void executeScan(TableScan* scan) override {
      int64_t one = 0;
      int64_t two = 100;
      for (int i = two; i > 0; --i) {
        std::vector<SValue> row;
        row.emplace_back(SValue(++one));
        row.emplace_back(SValue(two--));
        row.emplace_back(SValue((int64_t) (i % 2 ? 10 : 20)));
        if (!scan->nextRow(row.data(), row.size())) {
          return;
        }
      }
    }
  };

  class TestTable2Ref : public TableRef {
    int getColumnIndex(const std::string& name) override {
      if (name == "one") return 0;
      if (name == "two") return 1;
      if (name == "three") return 2;
      return -1;
    }
    void executeScan(TableScan* scan) override {
      for (int i = 10; i > 0; --i) {
        std::vector<SValue> row;
        row.emplace_back(SValue((int64_t) i));
        row.emplace_back(SValue((int64_t) (i * 2)));
        row.emplace_back(SValue((int64_t) (i % 2 ? 100 : 200)));
        if (!scan->nextRow(row.data(), row.size())) {
          return;
        }
      }
    }
  };

  void testSimpleTableScanQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTableRef()));

    auto query = Query(
        "  SELECT one + 50, two FROM testtable",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumColumns() == 2);
    assert(results.getNumRows() == 100);

    for (int i = 0; i<100; ++i) {
      const auto& row = results.getRow(i);
      assert(row[0]->getInteger() == 51 + i);
      assert(row[1]->getInteger() == 100 - i);
    }
  }

  void testTableScanWhereQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTableRef()));

    auto query = Query(
        "  SELECT"
        "    one + 1 as fnord,"
        "    two"
        "  FROM"
        "    testtable"
        "  WHERE"
        "    one > two or one = 3;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumRows() == 51);
  }

  void testTableScanWhereLimitQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTableRef()));

    auto query = Query(
        "  SELECT"
        "    one + 1 as fnord,"
        "    two"
        "  FROM"
        "    testtable"
        "  WHERE"
        "    one > two or one = 3"
        "  LIMIT 10 OFFSET 5;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumRows() == 10);
    const auto& row = results.getRow(0);
    assert(row[0]->getInteger() == 56);
    assert(row[1]->getInteger() == 46);
  }

  // select count(*), one, two, three from testtable2 group by case three when
  // 200 then 100 else 100 end;

  void testTableScanGroupByQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTable2Ref()));

    auto query = Query(
        "  SELECT"
        "    one,"
        "    two,"
        "    three"
        "  FROM"
        "    testtable"
        "  GROUP BY"
        "    three, "
        "    two % 8;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumRows() == 4);
  }

  void testTableScanGroupByCountQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTable2Ref()));

    auto query = Query(
        "  SELECT"
        "    count(one),"
        "    one,"
        "    two,"
        "    three"
        "  FROM"
        "    testtable"
        "  GROUP BY"
        "    three, "
        "    two % 8;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    int sum = 0;
    for (int i = 0; i < results.getNumRows(); ++i) {
      const auto& row = results.getRow(i);
      sum += row[0]->getInteger();
    }
    assert(sum == 10);
  }

  void testTableScanGroupBySumQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTable2Ref()));

    auto query = Query(
        "  SELECT"
        "    sum(one),"
        "    three"
        "  FROM"
        "    testtable"
        "  GROUP BY"
        "    three;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumRows() == 2);
    for (int i = 0; i<2; ++i) {
      const auto& row = results.getRow(i);
      assert(
        (row[0]->getInteger() == 25 && row[1]->getInteger() == 100) ||
        (row[0]->getInteger() == 30 && row[1]->getInteger() == 200));
    }
  }

  void testTableScanGroupWithoutGroupClause() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTable2Ref()));

    auto query = Query(
        "  SELECT"
        "    sum(one)"
        "  FROM"
        "    testtable;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumRows() == 1);
    assert(results.getRow(0)[0]->getInteger() == 55);
  }

  void testNamedSeriesQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTable2Ref()));

    auto query = Query(
        "  SERIES \"myseries\" FROM"
        "    SELECT"
        "      one, two"
        "    FROM"
        "      testtable;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumRows() == 10);
    assert(results.getNumColumns() == 3);
    for (int i = 0; i < results.getNumRows(); ++i) {
      const auto& row = results.getRow(i);
      assert(row[0]->getString() == "myseries");
      assert(row[1]->getInteger() == 10 - i);
      assert(row[2]->getInteger() == 20 - i * 2);
    }
  }

  void testDerivedSeriesQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTable2Ref()));

    auto query = Query(
        "  SERIES one * 5 FROM"
        "    SELECT"
        "      one, two"
        "    FROM"
        "      testtable;",
        &repo);
    query.execute();

    const auto& results = query.getResults(0);
    assert(results.getNumRows() == 10);
    assert(results.getNumColumns() == 3);
    for (int i = 0; i < results.getNumRows(); ++i) {
      const auto& row = results.getRow(i);
      assert(atoi(row[0]->getString().c_str()) == row[1]->getInteger() * 5);
    }
  }


  void testRenderSeriesQuery() {
    TableRepository repo;
    repo.addTableRef("testtable",
        std::unique_ptr<TableRef>(new TestTable2Ref()));

    auto query = Query(
        "  DRAW BAR CHART;"
        ""
        "  SERIES \"myseries\" FROM"
        "    SELECT"
        "      one, two"
        "    FROM"
        "      testtable;",
        &repo);

    std::vector<std::unique_ptr<Drawable>> drawables;
    query.execute(&drawables);
    assert(drawables.size() == 1);
  }

};

}
}

int main() {
  fnordmetric::query::QueryTest test;
  test.run();
  printf("all tests passed! :)\n");
}
