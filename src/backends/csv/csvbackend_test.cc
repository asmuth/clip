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
#include "csvfile.h"
//#include "csv_backend.h"
#include "../../util/runtimeexception.h"
#include "../../util/unittest.h"

using namespace fnordmetric::csv_backend;

UNIT_TEST(CSVFileTest);

TEST_CASE(CSVFileTest, TestOpenFile, [] () {
  auto csv_file = CSVFile::openFile("test/fixtures/gbp_per_country.csv");
  assert(csv_file.get() != nullptr);
});

TEST_CASE(CSVFileTest, TestInvalidFileName, [] () {
  EXPECT(1 == 2);
  //auto csv_file = CSVFile::openFile("test/fixtures/invalid.csv");
  //assert(csv_file.get() != nullptr);
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
