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
#include <fnordmetric/query/query.h>
#include <fnordmetric/query/queryservice.h>
#include <fnordmetric/sql/backends/csv/csvtableref.h>
#include <fnordmetric/sql/queryplannode.h>
#include <fnordmetric/sql/parser.h>
#include <fnordmetric/sql/resultlist.h>
#include <fnordmetric/sql/tableref.h>
#include <fnordmetric/sql/tablescan.h>
#include <fnordmetric/sql/tablerepository.h>
#include <fnordmetric/sql/token.h>
#include <fnordmetric/sql/tokenize.h>
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/svgtarget.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/unittest.h>
#include <fnordmetric/util/runtimeexception.h>

using namespace fnordmetric::query;

UNIT_TEST(SQLTest);

static Parser parseTestQuery(const char* query) {
  Parser parser;
  parser.parse(query, strlen(query));
  return parser;
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

TEST_CASE(SQLTest, TestSimpleValueExpression, [] () {
  auto parser = parseTestQuery("SELECT 23 + 5.123 FROM sometable;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_SELECT);
  EXPECT(stmt->getChildren().size() == 2);
  const auto& sl = stmt->getChildren()[0];
  EXPECT(*sl == ASTNode::T_SELECT_LIST);
  EXPECT(sl->getChildren().size() == 1);
  auto derived = sl->getChildren()[0];
  EXPECT(*derived == ASTNode::T_DERIVED_COLUMN);
  EXPECT(derived->getChildren().size() == 1);
  auto expr = derived->getChildren()[0];
  EXPECT(*expr == ASTNode::T_ADD_EXPR);
  EXPECT(expr->getChildren().size() == 2);
  EXPECT(*expr->getChildren()[0] == ASTNode::T_LITERAL);
  EXPECT(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
  EXPECT(*expr->getChildren()[0]->getToken() == "23");
  EXPECT(*expr->getChildren()[1] == ASTNode::T_LITERAL);
  EXPECT(*expr->getChildren()[1]->getToken() == Token::T_NUMERIC);
  EXPECT(*expr->getChildren()[1]->getToken() == "5.123");
  const auto& from = stmt->getChildren()[1];
  EXPECT(*from == ASTNode::T_FROM);
});

TEST_CASE(SQLTest, TestArithmeticValueExpression, [] () {
  auto parser = parseTestQuery("SELECT 1 + 2 / 3;");
  EXPECT(parser.getStatements().size() == 1);
  auto expr = parser.getStatements()[0]
      ->getChildren()[0]->getChildren()[0]->getChildren()[0];
  EXPECT(*expr == ASTNode::T_ADD_EXPR);
  EXPECT(expr->getChildren().size() == 2);
  EXPECT(*expr->getChildren()[0] == ASTNode::T_LITERAL);
  EXPECT(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
  EXPECT(*expr->getChildren()[0]->getToken() == "1");
  EXPECT(*expr->getChildren()[1] == ASTNode::T_DIV_EXPR);
  EXPECT(expr->getChildren()[1]->getChildren().size() == 2);
});

TEST_CASE(SQLTest, TestArithmeticValueExpressionParens, [] () {
  auto parser = parseTestQuery("SELECT (1 * 2) + 3;");
  EXPECT(parser.getStatements().size() == 1);
  auto expr = parser.getStatements()[0]
      ->getChildren()[0]->getChildren()[0]->getChildren()[0];
  EXPECT(*expr == ASTNode::T_ADD_EXPR);
  EXPECT(expr->getChildren().size() == 2);
  EXPECT(*expr->getChildren()[0] == ASTNode::T_MUL_EXPR);
  EXPECT(expr->getChildren()[0]->getChildren().size() == 2);
  EXPECT(*expr->getChildren()[1] == ASTNode::T_LITERAL);
  EXPECT(*expr->getChildren()[1]->getToken() == Token::T_NUMERIC);
  EXPECT(*expr->getChildren()[1]->getToken() == "3");
});

TEST_CASE(SQLTest, TestArithmeticValueExpressionPrecedence, [] () {
  {
    auto parser = parseTestQuery("SELECT 1 * 2 + 3;");
    EXPECT(parser.getStatements().size() == 1);
    auto expr = parser.getStatements()[0]
        ->getChildren()[0]->getChildren()[0]->getChildren()[0];
    EXPECT(*expr == ASTNode::T_ADD_EXPR);
    EXPECT(expr->getChildren().size() == 2);
    EXPECT(*expr->getChildren()[0] == ASTNode::T_MUL_EXPR);
    EXPECT(expr->getChildren()[0]->getChildren().size() == 2);
    EXPECT(*expr->getChildren()[1] == ASTNode::T_LITERAL);
    EXPECT(*expr->getChildren()[1]->getToken() == Token::T_NUMERIC);
    EXPECT(*expr->getChildren()[1]->getToken() == "3");
  }
  {
    auto parser = parseTestQuery("SELECT 1 + 2 * 3;");
    EXPECT(parser.getStatements().size() == 1);
    auto expr = parser.getStatements()[0]
        ->getChildren()[0]->getChildren()[0]->getChildren()[0];
    EXPECT(*expr == ASTNode::T_ADD_EXPR);
    EXPECT(expr->getChildren().size() == 2);
    EXPECT(*expr->getChildren()[0] == ASTNode::T_LITERAL);
    EXPECT(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
    EXPECT(*expr->getChildren()[0]->getToken() == "1");
    EXPECT(*expr->getChildren()[1] == ASTNode::T_MUL_EXPR);
    EXPECT(expr->getChildren()[1]->getChildren().size() == 2);
  }
});

TEST_CASE(SQLTest, TestMethodCallValueExpression, [] () {
  auto parser = parseTestQuery("SELECT 1 + sum(23, 4 + 1) FROM sometable;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_SELECT);
  EXPECT(stmt->getChildren().size() == 2);
  const auto& sl = stmt->getChildren()[0];
  EXPECT(*sl == ASTNode::T_SELECT_LIST);
  EXPECT(sl->getChildren().size() == 1);
  auto derived = sl->getChildren()[0];
  EXPECT(*derived == ASTNode::T_DERIVED_COLUMN);
  EXPECT(derived->getChildren().size() == 1);
  auto expr = derived->getChildren()[0];
  EXPECT(*expr == ASTNode::T_ADD_EXPR);
  EXPECT(expr->getChildren().size() == 2);
  EXPECT(*expr->getChildren()[0] == ASTNode::T_LITERAL);
  EXPECT(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
  EXPECT(*expr->getChildren()[0]->getToken() == "1");
  auto mcall = expr->getChildren()[1];
  EXPECT(*mcall == ASTNode::T_METHOD_CALL);
  EXPECT(*mcall->getToken() == Token::T_IDENTIFIER);
  EXPECT(*mcall->getToken() == "sum");
  EXPECT(mcall->getChildren().size() == 2);
  EXPECT(*mcall->getChildren()[0] == ASTNode::T_LITERAL);
  EXPECT(*mcall->getChildren()[0]->getToken() == Token::T_NUMERIC);
  EXPECT(*mcall->getChildren()[0]->getToken() == "23");
  EXPECT(*mcall->getChildren()[1] == ASTNode::T_ADD_EXPR);
  EXPECT(mcall->getChildren()[1]->getChildren().size() == 2);
  const auto& from = stmt->getChildren()[1];
  EXPECT(*from == ASTNode::T_FROM);
});

TEST_CASE(SQLTest, TestNegatedValueExpression, [] () {
  auto parser = parseTestQuery("SELECT -(23 + 5.123) AS fucol FROM tbl;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_SELECT);
  EXPECT(stmt->getChildren().size() == 2);
  const auto& sl = stmt->getChildren()[0];
  EXPECT(*sl == ASTNode::T_SELECT_LIST);
  EXPECT(sl->getChildren().size() == 1);
  auto derived = sl->getChildren()[0];
  EXPECT(*derived == ASTNode::T_DERIVED_COLUMN);
  EXPECT(derived->getChildren().size() == 2);
  auto neg_expr = derived->getChildren()[0];
  EXPECT(*neg_expr == ASTNode::T_NEGATE_EXPR);
  EXPECT(neg_expr->getChildren().size() == 1);
  auto expr = neg_expr->getChildren()[0];
  EXPECT(*expr == ASTNode::T_ADD_EXPR);
  EXPECT(expr->getChildren().size() == 2);
  EXPECT(*expr->getChildren()[0] == ASTNode::T_LITERAL);
  EXPECT(*expr->getChildren()[0]->getToken() == Token::T_NUMERIC);
  EXPECT(*expr->getChildren()[0]->getToken() == "23");
  EXPECT(*expr->getChildren()[1] == ASTNode::T_LITERAL);
  EXPECT(*expr->getChildren()[1]->getToken() == Token::T_NUMERIC);
  EXPECT(*expr->getChildren()[1]->getToken() == "5.123");
  auto col_name = derived->getChildren()[1];
  EXPECT(*col_name == ASTNode::T_COLUMN_ALIAS);
  EXPECT(*col_name->getToken() == Token::T_IDENTIFIER);
  EXPECT(*col_name->getToken() == "fucol");
  const auto& from = stmt->getChildren()[1];
  EXPECT(*from == ASTNode::T_FROM);
});

TEST_CASE(SQLTest, TestSelectWildcard, [] () {
  auto parser = parseTestQuery("SELECT * FROM sometable;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_SELECT);
  EXPECT(stmt->getChildren().size() == 2);
  const auto& sl = stmt->getChildren()[0];
  EXPECT(*sl == ASTNode::T_SELECT_LIST);
  EXPECT(sl->getChildren().size() == 1);
  EXPECT(*sl->getChildren()[0] == ASTNode::T_ALL);
  EXPECT(sl->getChildren()[0]->getToken() == nullptr);
  const auto& from = stmt->getChildren()[1];
  EXPECT(*from == ASTNode::T_FROM);
});

TEST_CASE(SQLTest, TestSelectTableWildcard, [] () {
  auto parser = parseTestQuery("SELECT mytablex.* FROM sometable;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_SELECT);
  EXPECT(stmt->getChildren().size() == 2);
  const auto& sl = stmt->getChildren()[0];
  EXPECT(*sl == ASTNode::T_SELECT_LIST);
  EXPECT(sl->getChildren().size() == 1);
  const auto& all = sl->getChildren()[0];
  EXPECT(*all == ASTNode::T_ALL);
  EXPECT(all->getToken() != nullptr);
  EXPECT(*all->getToken() == Token::T_IDENTIFIER);
  EXPECT(*all->getToken() == "mytablex");
  const auto& from = stmt->getChildren()[1];
  EXPECT(*from == ASTNode::T_FROM);
});

TEST_CASE(SQLTest, TestSelectDerivedColumn, [] () {
  auto parser = parseTestQuery("SELECT somecol AS another FROM sometable;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_SELECT);
  EXPECT(stmt->getChildren().size() == 2);
  const auto& sl = stmt->getChildren()[0];
  EXPECT(*sl == ASTNode::T_SELECT_LIST);
  EXPECT(sl->getChildren().size() == 1);
  const auto& derived = sl->getChildren()[0];
  EXPECT(*derived == ASTNode::T_DERIVED_COLUMN);
  EXPECT(derived->getChildren().size() == 2);
  EXPECT(*derived->getChildren()[0] == ASTNode::T_COLUMN_NAME);
  EXPECT(*derived->getChildren()[0]->getToken() == Token::T_IDENTIFIER);
  EXPECT(*derived->getChildren()[0]->getToken() == "somecol");
  EXPECT(*derived->getChildren()[1] == ASTNode::T_COLUMN_ALIAS);
  EXPECT(*derived->getChildren()[1]->getToken() == Token::T_IDENTIFIER);
  EXPECT(*derived->getChildren()[1]->getToken() == "another");
  const auto& from = stmt->getChildren()[1];
  EXPECT(*from == ASTNode::T_FROM);
});

TEST_CASE(SQLTest, TestSelectDerivedColumnWithTableName, [] () {
  auto parser = parseTestQuery("SELECT tbl.col AS another FROM sometable;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(*stmt == ASTNode::T_SELECT);
  EXPECT(stmt->getChildren().size() == 2);
  const auto& sl = stmt->getChildren()[0];
  EXPECT(*sl == ASTNode::T_SELECT_LIST);
  EXPECT(sl->getChildren().size() == 1);
  const auto& derived = sl->getChildren()[0];
  EXPECT(*derived == ASTNode::T_DERIVED_COLUMN);
  EXPECT(derived->getChildren().size() == 2);
  auto tbl = derived->getChildren()[0];
  EXPECT(*tbl == ASTNode::T_TABLE_NAME);
  EXPECT(*tbl->getToken() == Token::T_IDENTIFIER);
  EXPECT(*tbl->getToken() == "tbl");
  EXPECT(tbl->getChildren().size() == 1);
  auto col = tbl->getChildren()[0];
  EXPECT(*col->getToken() == Token::T_IDENTIFIER);
  EXPECT(*col->getToken() == "col");
  EXPECT(*derived->getChildren()[0] == ASTNode::T_TABLE_NAME);
  EXPECT(*derived->getChildren()[0]->getToken() == Token::T_IDENTIFIER);
  EXPECT(*derived->getChildren()[0]->getToken() == "tbl");
  EXPECT(*derived->getChildren()[1] == ASTNode::T_COLUMN_ALIAS);
  EXPECT(*derived->getChildren()[1]->getToken() == Token::T_IDENTIFIER);
  EXPECT(*derived->getChildren()[1]->getToken() == "another");
  const auto& from = stmt->getChildren()[1];
  EXPECT(*from == ASTNode::T_FROM);
});

TEST_CASE(SQLTest, TestSelectMustBeFirstAssert, [] () {
  const char* err_msg = "unexpected token T_GROUP, expected one of SELECT, "
      "DRAW or IMPORT";

  EXPECT_EXCEPTION(err_msg, [] () {
    auto parser = parseTestQuery("GROUP BY SELECT");
  });
});

TEST_CASE(SQLTest, TestFromList, [] () {
  auto parser = parseTestQuery("SELECT a FROM tbl1, tbl2;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  const auto& from = stmt->getChildren()[1];
  EXPECT(*from == ASTNode::T_FROM);
  EXPECT(from->getChildren().size() == 2);
  EXPECT(*from->getChildren()[0] == ASTNode::T_TABLE_NAME);
  EXPECT(*from->getChildren()[0]->getToken() == "tbl1");
  EXPECT(*from->getChildren()[1] == ASTNode::T_TABLE_NAME);
  EXPECT(*from->getChildren()[1]->getToken() == "tbl2");
});

TEST_CASE(SQLTest, TestWhereClause, [] () {
  auto parser = parseTestQuery("SELECT x FROM t WHERE a=1 AND a+1=2 OR b=3;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(stmt->getChildren().size() == 3);
  const auto& where = stmt->getChildren()[2];
  EXPECT(*where == ASTNode::T_WHERE);
  EXPECT(where->getChildren().size() == 1);
  EXPECT(*where->getChildren()[0] == ASTNode::T_OR_EXPR);
});

TEST_CASE(SQLTest, TestGroupByClause, [] () {
  auto parser = parseTestQuery("select count(x), y from t GROUP BY x;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(stmt->getChildren().size() == 3);
  const auto& where = stmt->getChildren()[2];
  EXPECT(*where == ASTNode::T_GROUP_BY);
  EXPECT(where->getChildren().size() == 1);
  EXPECT(*where->getChildren()[0] == ASTNode::T_COLUMN_NAME);
});

TEST_CASE(SQLTest, TestOrderByClause, [] () {
  auto parser = parseTestQuery("select a FROM t ORDER BY a DESC;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(stmt->getChildren().size() == 3);
  const auto& order_by = stmt->getChildren()[2];
  EXPECT(*order_by == ASTNode::T_ORDER_BY);
  EXPECT(order_by->getChildren().size() == 1);
  EXPECT(*order_by->getChildren()[0] == ASTNode::T_SORT_SPEC);
  EXPECT(*order_by->getChildren()[0]->getToken() == Token::T_DESC);
});

TEST_CASE(SQLTest, TestHavingClause, [] () {
  auto parser = parseTestQuery("select a FROM t HAVING 1=1;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(stmt->getChildren().size() == 3);
  const auto& having = stmt->getChildren()[2];
  EXPECT(*having == ASTNode::T_HAVING);
  EXPECT(having->getChildren().size() == 1);
  EXPECT(*having->getChildren()[0] == ASTNode::T_EQ_EXPR);
});

TEST_CASE(SQLTest, TestLimitClause, [] () {
  auto parser = parseTestQuery("select a FROM t LIMIT 10;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(stmt->getChildren().size() == 3);
  const auto& limit = stmt->getChildren()[2];
  EXPECT(*limit == ASTNode::T_LIMIT);
  EXPECT(limit->getChildren().size() == 0);
  EXPECT(*limit->getToken() == "10");
});

TEST_CASE(SQLTest, TestLimitOffsetClause, [] () {
  auto parser = parseTestQuery("select a FROM t LIMIT 10 OFFSET 23;");
  EXPECT(parser.getStatements().size() == 1);
  const auto& stmt = parser.getStatements()[0];
  EXPECT(stmt->getChildren().size() == 3);
  const auto& limit = stmt->getChildren()[2];
  EXPECT(*limit == ASTNode::T_LIMIT);
  EXPECT(limit->getChildren().size() == 1);
  EXPECT(*limit->getToken() == "10");
  EXPECT(*limit->getChildren()[0] == ASTNode::T_OFFSET);
  EXPECT(*limit->getChildren()[0]->getToken() == "23");
});

TEST_CASE(SQLTest, TestTokenizerEscaping, [] () {
  std::vector<Token> tl;
  tokenizeQuery(
      " SELECT  fnord,sum(blah) from fubar blah.id"
      "= 'fnor\\'dbar' + 123.5;",
      &tl);

  EXPECT(tl.size() == 17);
  EXPECT(tl[0].getType() == Token::T_SELECT);
  EXPECT(tl[1].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[1] == "fnord");
  EXPECT(tl[2].getType() == Token::T_COMMA);
  EXPECT(tl[3].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[3] == "sum");
  EXPECT(tl[4].getType() == Token::T_LPAREN);
  EXPECT(tl[5].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[5] == "blah");
  EXPECT(tl[6].getType() == Token::T_RPAREN);
  EXPECT(tl[7].getType() == Token::T_FROM);
  EXPECT(tl[8].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[8] == "fubar");
  EXPECT(tl[9].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[9] == "blah");
  EXPECT(tl[10].getType() == Token::T_DOT);
  EXPECT(tl[11].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[11] == "id");
  EXPECT(tl[12].getType() == Token::T_EQUAL);
  EXPECT(tl[13].getType() == Token::T_STRING);
  //EXPECT(tl[13] == "fnord'bar"); // FIXPAUL
  EXPECT(tl[14].getType() == Token::T_PLUS);
  EXPECT(tl[15].getType() == Token::T_NUMERIC);
  EXPECT(tl[15] == "123.5");
  EXPECT(tl[16].getType() == Token::T_SEMICOLON);
});

TEST_CASE(SQLTest, TestTokenizerSimple, [] () {
  std::vector<Token> tl;
  tokenizeQuery(
      " SELECT  fnord,sum(`blah-field`) from fubar"
      " WHERE blah.id= \"fn'o=,rdbar\" + 123;",
      &tl);

  EXPECT(tl[0].getType() == Token::T_SELECT);
  EXPECT(tl[1].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[1] == "fnord");
  EXPECT(tl[2].getType() == Token::T_COMMA);
  EXPECT(tl[3].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[3] == "sum");
  EXPECT(tl[4].getType() == Token::T_LPAREN);
  EXPECT(tl[5].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[5] == "blah-field");
  EXPECT(tl[6].getType() == Token::T_RPAREN);
  EXPECT(tl[7].getType() == Token::T_FROM);
  EXPECT(tl[8].getType() == Token::T_IDENTIFIER);
  EXPECT(tl[8] == "fubar");
  //EXPECT(tl[9].getType() == Token::T_IDENTIFIER);
  //EXPECT(tl[9] == "blah");
  //EXPECT(tl[10].getType() == Token::T_DOT);
  //EXPECT(tl[11].getType() == Token::T_IDENTIFIER);
  //EXPECT(tl[11] == "id");
  //EXPECT(tl[12].getType() == Token::T_EQUAL);
  //EXPECT(tl[13].getType() == Token::T_STRING);
  //EXPECT(tl[13] == "fn'o=,rdbar");
  //EXPECT(tl[14].getType() == Token::T_PLUS);
  //EXPECT(tl[15].getType() == Token::T_NUMERIC);
  //EXPECT(tl[15] == "123");
  //EXPECT(tl[16].getType() == Token::T_SEMICOLON);
});


TEST_CASE(SQLTest, TestTokenizerAsClause, [] () {
  auto parser = parseTestQuery(" SELECT fnord As blah from asd;");
  auto tl = &parser.getTokenList();
  EXPECT((*tl)[0].getType() == Token::T_SELECT);
  EXPECT((*tl)[1].getType() == Token::T_IDENTIFIER);
  EXPECT((*tl)[1] == "fnord");
  EXPECT((*tl)[2].getType() == Token::T_AS);
  EXPECT((*tl)[3].getType() == Token::T_IDENTIFIER);
  EXPECT((*tl)[3] == "blah");
});

TEST_INITIALIZER(SQLTest, InitializeComplexQueries, [] () {
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
    new fnordmetric::util::UnitTest::TestCase(
        &SQLTest,
        "TestComplexQueries",
        [query] () {
          auto parser = parseTestQuery(query);
          EXPECT(parser.getStatements().size() == 1);
        });
  }
});

TEST_CASE(SQLTest, TestSelectOnlyQuery, [] () {
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
  auto results = query.getResultList(0);
  EXPECT(results->getNumColumns() == 6);
  EXPECT(results->getNumRows() == 1);
  const auto& cols = results->getColumns();
  EXPECT(cols[0] == "fnord");
  EXPECT(cols[1] == "fubar");
  EXPECT(cols[2] == "baz");
  EXPECT(cols[3] == "one");
  EXPECT(cols[4] == "two");
  EXPECT(cols[5] == "three");
  const auto& row = results->getRow(0);
  EXPECT(row[0] == "23");
  EXPECT(row[1] == "256");
  EXPECT(row[2] == "21");
  //EXPECT(row[3] == "true");
  //EXPECT(row[4] == "false");
  //EXPECT(row[5] == "true");
});

TEST_CASE(SQLTest, TestSimpleTableScanQuery, [] () {
  TableRepository repo;
  repo.addTableRef("testtable",
      std::unique_ptr<TableRef>(new TestTableRef()));

  auto query = Query(
      "  SELECT one + 50, two FROM testtable",
      &repo);

  query.execute();
  auto results = query.getResultList(0);
  EXPECT(results->getNumColumns() == 2);
  EXPECT(results->getNumRows() == 100);

  for (int i = 0; i<100; ++i) {
    const auto& row = results->getRow(i);
    EXPECT(atoi(row[0].c_str()) == 51 + i);
    EXPECT(atoi(row[1].c_str()) == 100 - i);
  }
});

TEST_CASE(SQLTest, TestTableScanWhereQuery, [] () {
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
  auto results = query.getResultList(0);
  EXPECT(results->getNumRows() == 51);
});

TEST_CASE(SQLTest, TestTableScanWhereLimitQuery, [] () {
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
  auto results = query.getResultList(0);
  EXPECT(results->getNumRows() == 10);
  const auto& row = results->getRow(0);
  EXPECT(row[0] == "56");
  EXPECT(row[1] == "46");
});

// select count(*), one, two, three from testtable2 group by case three when
// 200 then 100 else 100 end;

TEST_CASE(SQLTest, TestTableScanGroupByQuery, [] () {
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
  auto results = query.getResultList(0);
  EXPECT(results->getNumRows() == 4);
});

TEST_CASE(SQLTest, TestTableScanGroupByCountQuery, [] () {
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
  auto results = query.getResultList(0);
  int sum = 0;
  for (int i = 0; i < results->getNumRows(); ++i) {
    const auto& row = results->getRow(i);
    sum += atoi(row[0].c_str());
  }
  EXPECT(sum == 10);
});

TEST_CASE(SQLTest, TestTableScanGroupBySumQuery, [] () {
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
  auto results = query.getResultList(0);
  EXPECT(results->getNumRows() == 2);
  for (int i = 0; i<2; ++i) {
    const auto& row = results->getRow(i);
    EXPECT(
      (atoi(row[0].c_str()) == 25 && atoi(row[1].c_str()) == 100) ||
      (atoi(row[0].c_str()) == 30 && atoi(row[1].c_str()) == 200));
  }
});

TEST_CASE(SQLTest, TestTableScanGroupWithoutGroupClause, [] () {
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
  auto results = query.getResultList(0);
  EXPECT(results->getNumRows() == 1);
  EXPECT(results->getRow(0)[0] == "55");
});

TEST_CASE(SQLTest, TestSimpleSelectFromCSV, [] () {
  auto csv_table = new csv_backend::CSVTableRef(
      csv_backend::CSVInputStream::openFile(
          "test/fixtures/gbp_per_country_simple.csv"), 
      true);

  TableRepository repo;
  repo.addTableRef("gbp_per_country",
      std::unique_ptr<csv_backend::CSVTableRef>(csv_table));

  auto query = Query(
      "  SELECT"
      "    country as country, gbp as gbp"
      "  FROM"
      "    gbp_per_country;",
      &repo);

  query.execute();
  auto results = query.getResultList(0);
  EXPECT(results->getNumRows() == 191);
});

TEST_CASE(SQLTest, TestSimpleAggregateFromCSV, [] () {
  auto csv_table = new csv_backend::CSVTableRef(
      csv_backend::CSVInputStream::openFile(
          "test/fixtures/gbp_per_country_simple.csv"), 
      true);

  TableRepository repo;
  repo.addTableRef("gbp_per_country",
      std::unique_ptr<csv_backend::CSVTableRef>(csv_table));

  auto query = Query(
      "  SELECT"
      "    sum(gbp) as global_gbp"
      "  FROM"
      "    gbp_per_country;",
      &repo);

  query.execute();
  auto results = query.getResultList(0);
  EXPECT(std::stof(results->getRow(0)[0]) == 74209240);
});

TEST_CASE(SQLTest, TestNoSuchColumnError, [] () {
  EXPECT_EXCEPTION("no such column: 'fnord'", [] () {
    auto csv_table = new csv_backend::CSVTableRef(
        csv_backend::CSVInputStream::openFile(
            "test/fixtures/gbp_per_country_simple.csv"), 
        false);

    TableRepository repo;
    repo.addTableRef("gbp_per_country",
        std::unique_ptr<csv_backend::CSVTableRef>(csv_table));

    auto query = Query(
        "  SELECT"
        "    sum(fnord) as global_gbp"
        "  FROM"
        "    gbp_per_country;",
        &repo);
    query.execute();
  });
});

TEST_CASE(SQLTest, TestTypeError, [] () {
  EXPECT_EXCEPTION("can't convert String 'United States' to Float", [] () {
    auto csv_table = new csv_backend::CSVTableRef(
        csv_backend::CSVInputStream::openFile(
            "test/fixtures/gbp_per_country.csv"), 
        false);

    TableRepository repo;
    repo.addTableRef("gbp_per_country",
        std::unique_ptr<csv_backend::CSVTableRef>(csv_table));

    auto query = Query(
        "  SELECT"
        "    sum(col4) as global_gbp"
        "  FROM"
        "    gbp_per_country;",
        &repo);
    query.execute();
  });
});


TEST_CASE(SQLTest, TestImportCSVTable, [] () {
  TableRepository repo;

  auto query = Query(
      "  IMPORT TABLE gbp_per_country "
      "     FROM CSV 'test/fixtures/gbp_per_country_simple.csv' HEADER;"
      ""
      "  SELECT"
      "    sum(gbp) as global_gbp"
      "  FROM"
      "    gbp_per_country;",
      &repo);

  query.execute();
  auto results = query.getResultList(0);
  EXPECT(std::stof(results->getRow(0)[0]) == 74209240);
});
