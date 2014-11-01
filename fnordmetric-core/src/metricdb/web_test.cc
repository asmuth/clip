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
