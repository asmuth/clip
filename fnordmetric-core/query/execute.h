/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_EXECUTE_H
#define _FNORDMETRIC_QUERY_EXECUTE_H
#include <stdlib.h>
#include <vector>

namespace fnordmetric {
namespace query {
class SValue;

bool executeExpression(
    CompiledExpression* expr,
    int argc,
    const SValue* argv,
    int* outc,
    SValue* outv);

}
}
#endif
