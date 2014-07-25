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

UNIT_TEST(CSVFileTest);

TEST_CASE(CSVFileTest, TestOpenFile, [] () {
  //printf("run test!\n");
});

/*
static fnordmetric::util::UnitTest::TestCase __case(
    &csv_file_test,
    "testOpenFile",
    [] () {
    });
*/

  //void testOpenFile() {
  //  auto csv_file = CSVFile::openFile("test/fixtures/gbp_per_country.csv");
  //  assert(csv_file.get() != nullptr);
  //  assert(csv_file->fd_ > 0);
  //}

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
