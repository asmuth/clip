/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_PLANER_H
#define _FNORDMETRIC_QUERY_PLANER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include "token.h"
#include "astnode.h"

namespace fnordmetric {
namespace query {

class Planner {
public:

  Planner(ASTNode* select_statement);

protected:

};

}
}
#endif
