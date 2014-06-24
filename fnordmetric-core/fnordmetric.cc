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

class ConnCb : public fnordmetric::ev::Acceptor::CallbackInterface {
  void onConnection(int fd) override {
    printf("got con %i\n", fd);
  }
};

int main(int argc, char** argv) {
  fnordmetric::ev::EventLoop ev_loop;
  fnordmetric::ev::Acceptor acceptor(&ev_loop);

  auto handler = [] (int fd) {
  };

  ConnCb cb;
  acceptor.listen(8080, &cb);
  acceptor.listen(8081, &cb);

  ev_loop.loop();
}
