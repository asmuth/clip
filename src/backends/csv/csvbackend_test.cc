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

namespace fnordmetric {
namespace csv_backend {

class CSVBackendTest {
public:
  CSVBackendTest() {}

  void run() {
    testOpenFile();
  }

  void testOpenFile() {
    auto csv_file = CSVFile::openFile("test/fixtures/gbp_per_country.csv");
    assert(csv_file.get() != nullptr);
    assert(csv_file->fd_ > 0);
  }

};

}
}

int main() {
  fnordmetric::csv_backend::CSVBackendTest test;

  try {
    test.run();
  } catch (fnordmetric::util::RuntimeException e) {
    printf("test fail :(\n\n");
    e.debugPrint();
    exit(1);
  }

  printf("all tests passed! :)\n");
}
