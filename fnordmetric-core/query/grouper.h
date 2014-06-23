/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_GROUPER_H
#define _FNORDMETRIC_QUERY_GROUPER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "svalue.h"

namespace fnordmetric {
namespace query {

class Grouper {
public:

  Grouper(int levels);

  void addRow(std::vector<SValue*> values);

protected:
  int levels_;
};

}
}
#endif
