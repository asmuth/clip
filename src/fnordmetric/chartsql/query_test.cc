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
#include <fnordmetric/chartsql/query.h>
#include <fnordmetric/chartsql/queryservice.h>
#include <fnordmetric/sql/backends/csv/csvbackend.h>
#include <fnordmetric/sql/backends/tableref.h>
#include <fnordmetric/sql/parser/parser.h>
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/sql/parser/tokenize.h>
#include <fnordmetric/sql/runtime/defaultruntime.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/sql/runtime/resultlist.h>
#include <fnordmetric/sql/runtime/tablescan.h>
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <ui/canvas.h>
#include <ui/svgtarget.h>
#include <util/inputstream.h>
#include <util/outputstream.h>
#include <util/unittest.h>
#include <fnord-base/uri.h>
#include <fnord-base/exception.h>

using namespace fnordmetric::query;

UNIT_TEST(QueryTest);

static Parser parseTestQuery(const char* query) {
  Parser parser;
  parser.parse(query, strlen(query));
  return parser;
}

class TestTableRef : public TableRef {
  std::vector<std::string> columns() override {
    return {"one", "two", "three"};
  }
  int getColumnIndex(const std::string& name) override {
    if (name == "one") return 0;
    if (name == "two") return 1;
    if (name == "three") return 2;
    return -1;
  }
  std::string getColumnName(int index) override {
    return columns()[index];
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

class TestBackend : public Backend {
public:

  bool openTables(
    const std::vector<std::string>& table_names,
    const fnord::URI& source_uri,
    std::vector<std::unique_ptr<TableRef>>* target) {
    EXPECT_EQ(source_uri.scheme(), "testtable");
    target->emplace_back(new TestTableRef());
    return true;
  }

};

static void compareChart(
    fnordmetric::ui::Canvas* chart,
    const std::string& file_name) {
  auto output_stream = fnordmetric::util::FileOutputStream::openFile(
      "build/tests/tmp/" + file_name);

  fnordmetric::ui::SVGTarget target(output_stream.get());
  chart->render(&target);

  EXPECT_FILES_EQ(
    "test/fixtures/" + file_name,
    "build/tests/tmp/" + file_name);
}

/*
TEST_CASE(QueryTest, TestDrawQueryNeedsSeriesColAssert, [] () {
  DefaultRuntime runtime;
  runtime.addBackend(std::unique_ptr<Backend>(new TestBackend()));

  auto query = Query(
      "  IMPORT TABLE testtable FROM 'testtable:';"
      "  DRAW BARCHART;"
      ""
      "  SELECT"
      "    'series1' as fnord, one AS x, two AS y"
      "  FROM"
      "    testtable;",
      &runtime);

  const char err[] = "can't draw SELECT because it has no 'series' column";

  EXPECT_EXCEPTION(err, [&query] () {
    query.execute();
  });
});
*/

TEST_CASE(QueryTest, TestDrawQueryNeedsXColAssert, [] () {
  DefaultRuntime runtime;
  runtime.addBackend(std::unique_ptr<Backend>(new TestBackend()));

  auto query = Query(
      "  IMPORT TABLE testtable FROM 'testtable:';"
      "  DRAW BARCHART;"
      ""
      "  SELECT"
      "    'series1' as series, one AS f, two AS y"
      "  FROM"
      "    testtable;",
      &runtime);

  //const char err[] = "can't draw SELECT because it has no 'x' column";

  //EXPECT_EXCEPTION(err, [&query] () {
  //  query.execute();
  //});
});

TEST_CASE(QueryTest, TestDrawQueryNeedsYColAssert, [] () {
  DefaultRuntime runtime;
  runtime.addBackend(std::unique_ptr<Backend>(new TestBackend()));

  auto query = Query(
      "  IMPORT TABLE testtable FROM 'testtable:';"
      "  DRAW BARCHART;"
      ""
      "  SELECT"
      "    'series1' as series, one AS x, two AS f"
      "  FROM"
      "    testtable;",
      &runtime);

  // FIXPAUL check that we get an informative error message
  //const char err[] = "can't draw SELECT because it has no 'y' column";

  //EXPECT_EXCEPTION(err, [&query] () {
  //  query.execute();
  //});
});

TEST_CASE(QueryTest, TestSimpleDrawQuery, [] () {
  DefaultRuntime runtime;
  runtime.addBackend(std::unique_ptr<Backend>(new TestBackend()));

  auto query = Query(
      "  IMPORT TABLE testtable FROM 'testtable:';"
      "  DRAW LINECHART AXIS LEFT;"
      ""
      "  SELECT"
      "    'series1' as series, one AS x, two AS y"
      "  FROM"
      "    testtable;"
      ""
      "  SELECT"
      "    'series2' as series, one as x, two + 5 as y"
      "  from"
      "    testtable;"
      ""
      "  SELECT"
      "    'series3' as series, one as x, two / 2 + 4 as y"
      "  FROM"
      "    testtable;"
      "",
      &runtime);

  query.execute();
  auto chart = query.getChart(0);

  // FIXPAUL!
  //compareChart(
  //    chart,
  //    "QueryTest_TestSimpleDrawQuery_out.svg.html");
});

TEST_CASE(QueryTest, TestDerivedSeriesDrawQuery, [] () {
  DefaultRuntime runtime;
  runtime.addBackend(std::unique_ptr<Backend>(new TestBackend()));

  auto query = Query(
      "  IMPORT TABLE testtable FROM 'testtable:';"
      "  DRAW LINECHART AXIS LEFT;"
      ""
      "  SELECT"
      "    one % 3 as series, one / 3 as x, two + one AS y"
      "  FROM"
      "    testtable;",
      &runtime);

  query.execute();
  auto chart = query.getChart(0);

  // FIXPAUL!!
  //compareChart(
  //    chart,
  //    "QueryTest_TestDerivedSeriesDrawQuery_out.svg.html");
});

TEST_CASE(QueryTest, SimpleEndToEndTest, [] () {
  DefaultRuntime runtime;
  runtime.addBackend(std::unique_ptr<Backend>(new csv_backend::CSVBackend()));

  auto query = Query(
      "  IMPORT TABLE gbp_per_country "
      "     FROM 'csv:test/fixtures/gbp_per_country_simple.csv?headers=true';"
      ""
      "  DRAW BARCHART AXIS LEFT;"
      ""
      "  SELECT"
      "    'gross domestic product per country' as series,"
      "    country as x,"
      "    gbp as y"
      "  FROM"
      "    gbp_per_country"
      "  LIMIT 30;",
      &runtime);

  query.execute();
  auto chart = query.getChart(0);

  // FIXPAUL!!
  //compareChart(
  //    chart,
  //    "QueryTest_SimpleEndToEndTest_out.svg.html");
});

TEST_CASE(QueryTest, TestQueryService, [] () {
  auto query =
      "  IMPORT TABLE gbp_per_country "
      "     FROM 'csv:test/fixtures/gbp_per_country_simple.csv?headers=true';"
      ""
      "  DRAW BARCHART"
      "    AXIS BOTTOM"
      "    AXIS LEFT;"
      ""
      "  SELECT"
      "    'gross domestic product per country' as series,"
      "    country as x,"
      "    gbp as y"
      "  FROM"
      "    gbp_per_country"
      "  LIMIT 30;";

  QueryService query_service;
  query_service.registerBackend(
      std::unique_ptr<fnordmetric::query::Backend>(
          new fnordmetric::query::csv_backend::CSVBackend));

  auto input = fnordmetric::util::StringInputStream::fromString(query);
  auto output = fnordmetric::util::FileOutputStream::openFile(
      "build/tests/tmp/QueryTest_TestQueryService_out.svg.html");

  query_service.executeQuery(
      std::move(input),
      QueryService::FORMAT_SVG,
      std::move(output));

  EXPECT_FILES_EQ(
      "test/fixtures/QueryTest_TestQueryService_out.svg.html",
      "build/tests/tmp/QueryTest_TestQueryService_out.svg.html");
});

TEST_CASE(QueryTest, TestParseCity, [] () {
  std::string query_string;
  auto query_stream = fnordmetric::util::FileInputStream::openFile(
      "test/fixtures/city_temperatures.sql");
  query_stream->readUntilEOF(&query_string);

  auto parser = parseTestQuery(query_string.c_str());
  EXPECT(parser.getStatements().size() == 4);
  EXPECT(*parser.getStatements()[0] == ASTNode::T_IMPORT);
  EXPECT(*parser.getStatements()[1] == ASTNode::T_DRAW);
  EXPECT(*parser.getStatements()[2] == ASTNode::T_SELECT);
  EXPECT(*parser.getStatements()[3] == ASTNode::T_SELECT);
});

TEST_CASE(QueryTest, TestFourSelectFromCSVQuery, [] () {
  std::string query_str;
  auto query_stream = fnordmetric::util::FileInputStream::openFile(
      "test/fixtures/queries/gdpfourselects.sql");
  query_stream->readUntilEOF(&query_str);

  DefaultRuntime runtime;
  runtime.addBackend(std::unique_ptr<Backend>(new csv_backend::CSVBackend()));
  auto query = Query(query_str, &runtime);

  query.execute();
  EXPECT(query.getNumResultLists() == 4);

  for (int i = 0; i < query.getNumResultLists(); i++) {
    auto results = query.getResultList(i);
    EXPECT(results->getNumRows() == 10);
    EXPECT(results->getColumns().size() == 4);
    EXPECT(results->getRow(0)[1] == "USA")
    EXPECT(results->getRow(9)[1] == "IND")
  }
});


