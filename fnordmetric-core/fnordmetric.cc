/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include "ev/acceptor.h"
#include "ev/eventloop.h"
#include "web/httpserver.h"

int main(int argc, char** argv) {
  fnordmetric::ev::EventLoop ev_loop;
  fnordmetric::ev::Acceptor acceptor(&ev_loop);
  fnordmetric::web::ThreadedHTTPServer http;
  acceptor.listen(8080, &http);
  ev_loop.loop();
}
