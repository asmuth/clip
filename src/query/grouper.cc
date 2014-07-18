/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "grouper.h"

namespace fnordmetric {
namespace query {

Grouper::Grouper(int levels) : levels_(levels) {
  printf("new grouper with %i levels\n", levels);
}

}
}
