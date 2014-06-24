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

int main(int argc, char** argv) {
  fnordmetric::ev::EventLoop ev_loop;
  fnordmetric::ev::Acceptor acceptor(&ev_loop);

  auto handler = [] (int fd) {
    printf("got con 8080 %i\n", fd);
  };

  acceptor.listen(8080, handler);
  acceptor.listen(8081,  [] (int fd) {
    printf("got con 8081 %i\n", fd);
  });

  ev_loop.loop();
}
