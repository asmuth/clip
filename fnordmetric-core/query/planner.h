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
#include <assert.h>
#include "token.h"
#include "astnode.h"

namespace fnordmetric {
namespace query {
class Executable;

class Planner {
public:
  Planner(ASTNode* select_statement);
  Planner(const Planner& copy) = delete;
  Planner& operator=(const Planner& copy) = delete;
  std::unique_ptr<Executable> getExecutable();

protected:
  Executable* plan(ASTNode*);
  Executable* planTablelessSelect(ASTNode*);
  Executable* executable_;
};

}
}
#endif
