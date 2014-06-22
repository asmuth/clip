/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "executable.h"

namespace fnordmetric {
namespace query {

Executable::Executable() : target_(nullptr) {}
Executable::~Executable() {}

}
}
