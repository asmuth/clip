/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnord-base/exception.h>
#include <fnordmetric/util/unittest.h>
#include <fnordmetric/sql/backends/csv/csvinputstream.h>
#include <fnordmetric/sql/backends/csv/csvtableref.h>
#include <fnordmetric/sql/svalue.h>

using namespace fnordmetric::query::csv_backend;

UNIT_TEST(CSVInputStreamTest);

TEST_CASE(CSVInputStreamTest, TestOpenFile, [] () {
  auto csv_file = CSVInputStream::openFile(
      "test/fixtures/gbp_per_country.csv");

  EXPECT(csv_file.get() != nullptr);
});

TEST_CASE(CSVInputStreamTest, TestInvalidFileName, [] () {
  auto errmsg = "error opening file 'test/fixtures/invalid.csv': "
      "No such file or directory";

  EXPECT_EXCEPTION(errmsg, [] () {
    auto csv_file = CSVInputStream::openFile("test/fixtures/invalid.csv");
  });
});

TEST_CASE(CSVInputStreamTest, TestReadHeaders, [] () {
  auto csv_file = CSVInputStream::openFile(
      "test/fixtures/gbp_per_country_simple.csv");

  EXPECT(csv_file.get() != nullptr);
  std::vector<std::string> headers;
  csv_file->readNextRow(&headers);
  EXPECT_EQ(headers.size(), 2);
  EXPECT_EQ(headers[0], "country");
  EXPECT_EQ(headers[1], "gbp");
});

TEST_CASE(CSVInputStreamTest, TestReadSimpleRows, [] () {
  auto csv_file = CSVInputStream::openFile(
      "test/fixtures/gbp_per_country_simple.csv");
  EXPECT(csv_file.get() != nullptr);

  std::vector<std::string> headers;
  csv_file->readNextRow(&headers);
  EXPECT_EQ(headers.size(), 2);

  std::vector<std::string> row1;
  EXPECT(csv_file->readNextRow(&row1));
  EXPECT_EQ(row1.size(), 2);
  EXPECT_EQ(row1[0], "USA");
  EXPECT_EQ(row1[1], "16800000");

  std::vector<std::string> row2;
  EXPECT(csv_file->readNextRow(&row2));
  EXPECT_EQ(row2.size(), 2);
  EXPECT_EQ(row2[0], "CHN");
  EXPECT_EQ(row2[1], "9240270");
});

TEST_CASE(CSVInputStreamTest, TestReadSimpleRowsEOF, [] () {
  auto csv_file = CSVInputStream::openFile(
      "test/fixtures/gbp_per_country_simple.csv");
  EXPECT(csv_file.get() != nullptr);
  int num_rows = 0;

  for (;; ++num_rows) {
    std::vector<std::string> row;

    if (!csv_file->readNextRow(&row)) {
      break;
    }

    EXPECT_EQ(row.size(), 2);
  }

  EXPECT_EQ(num_rows, 192);
});

// CSVTableRefTest
TEST_CASE(CSVInputStreamTest, TestGetColumnIndexWithHeaders, [] () {
  CSVTableRef table_ref(
      CSVInputStream::openFile("test/fixtures/gbp_per_country_simple.csv"),
      true);

  EXPECT_EQ(table_ref.getColumnIndex("country"), 0);
});

// CSVTableRefTest
TEST_CASE(CSVInputStreamTest, TestReadRowsWithHeaders, [] () {
  CSVTableRef table_ref(
      CSVInputStream::openFile("test/fixtures/gbp_per_country_simple.csv"),
      true);

  for (int n = 0; n < 2; n++) {
    int num_rows = 0;
    for (;; ++num_rows) {
      std::vector<fnordmetric::query::SValue> target;
      if (!table_ref.readNextRow(&target)) {
        break;
      }
    }

    EXPECT_EQ(num_rows, 191);
    table_ref.rewind();
  }
});

// CSVTableRefTest
TEST_CASE(CSVInputStreamTest, TestGetColumnIndexWithoutHeaders, [] () {
  CSVTableRef table_ref(
      CSVInputStream::openFile(
          "test/fixtures/gbp_per_country_simple_noheaders.csv"),
      false);

  EXPECT_EQ(table_ref.getColumnIndex("col1"), 0);
  EXPECT_EQ(table_ref.getColumnIndex("col2"), 1);
  EXPECT_EQ(table_ref.getColumnIndex("col3"), 2);
  EXPECT_EQ(table_ref.getColumnIndex("col99"), 98);
});

// CSVTableRefTest
TEST_CASE(CSVInputStreamTest, TestReadRowsWithoutHeaders, [] () {
  CSVTableRef table_ref(
      CSVInputStream::openFile(
          "test/fixtures/gbp_per_country_simple_noheaders.csv"),
      false);

  int num_rows = 0;
  for (;; ++num_rows) {
    std::vector<fnordmetric::query::SValue> target;
    if (!table_ref.readNextRow(&target)) {
      break;
    }
  }

  EXPECT_EQ(num_rows, 191);
});



