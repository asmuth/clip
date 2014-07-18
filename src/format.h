/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FORMAT_H
#define _FNORDMETRIC_FORMAT_H
#include <string.h>
#include "query/svalue.h"

namespace fnordmetric {
namespace format {

std::string numberToHuman(double value);
std::string svalueToHuman(const query::SValue& value);

}
}
#endif
