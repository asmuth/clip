/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include "ev/acceptor.h"

int main(int argc, char** argv) {
  fnordmetric::ev::Acceptor acceptor;

  acceptor.setHandler([] (int fd) {
    printf("got con %i\n", fd);
  });

  acceptor.listen(8080);
}
