/**
 * This file is part of the "FnordMetric" project
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
#include "httpparser.h"

namespace fnordmetric {
namespace web {

class WebTest {
public:
  WebTest() {}

  void run() {
    testHTTPParser();
  }

  void testHTTPParser() {
    HTTPParser parser(HTTPParser::REQUEST);
    printf("testing!\n");
  }

};

}
}

int main() {
  fnordmetric::web::WebTest test;
  test.run();
  printf("all tests passed! :)\n");
}
