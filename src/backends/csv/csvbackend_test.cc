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
#include "csvinputstream.h"
//#include "csv_backend.h"
#include "../../util/runtimeexception.h"
#include "../../util/unittest.h"

using namespace fnordmetric::csv_backend;

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
    EXPECT(csv_file.get() != nullptr);
  });
});

TEST_CASE(CSVInputStreamTest, TestReadHeaders, [] () {
  auto csv_file = CSVInputStream::openFile(
      "test/fixtures/gbp_per_country_simple.csv");

  EXPECT(csv_file.get() != nullptr);
  std::vector<std::string> headers;
  csv_file->readNextRow(&headers);
  EXPECT(headers.size() == 2);
  EXPECT(headers[0] == "country");
  EXPECT(headers[1] == "gbp");
});


/*
int main() {
  try {
    csv_file_test.run();
  } catch (fnordmetric::util::RuntimeException e) {
    printf("test fail :(\n\n");
    e.debugPrint();
    exit(1);
  }

  printf("all tests passed! :)\n");
}
*/
