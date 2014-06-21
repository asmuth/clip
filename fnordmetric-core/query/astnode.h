/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_ASTNODE_H
#define _FNORDMETRIC_QUERY_ASTNODE_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace query {

struct DerivedColumnASTNode {
  size_t column_name_token;
};

struct WildcardSelectSublistASTNode {
  size_t table_name_token;
};

union SelectSublistASTNode {
  bool is_wildcard;
  WildcardSelectSublistASTNode wildcard;
  DerivedColumnASTNode derived;
};

struct SelectListASTNode {
  bool is_wildcard;
  std::vector<std::unique_ptr<SelectSublistASTNode>> select_sublists;
};

struct SelectASTNode {
  std::vector<std::unique_ptr<SelectListASTNode>> select_lists;
};

}
}
#endif
