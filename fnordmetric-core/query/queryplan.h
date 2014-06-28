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
class TableRepository;

class QueryPlan {
public:

  static Executable* buildQueryPlan(
      ASTNode* select_statement, TableRepository* repo);

protected:

  static bool hasGroupByClause(ASTNode* ast);
  static Executable* buildGroupBy(ASTNode* ast, TableRepository* repo);

  /**
   * Recursively walk the provided ast and search for column references. For
   * each found column reference, add the column reference to the provided
   * select list and replace the original column reference with an index into
   * the new select list.
   *
   * This is used to create child select lists for nested query plan nodes.
   */
  static bool buildInternalSelectList(ASTNode* ast, ASTNode* select_list);

};

}
}
#endif
