/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include "httpserver.h"

namespace fnordmetric {
namespace web {

void ThreadedHTTPServer::onConnection(int fd) {
  printf("got connection!\n");
}

}
}
