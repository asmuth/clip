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
  printf("hello fnordmetric!\n");
  fnordmetric::ev::Acceptor acceptor;
  acceptor.listen(8080);
}
