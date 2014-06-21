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
  Token* column_name;
};

struct SelectListASTNode {
  bool is_table_wildcard;
  union {
    Token* table_wildcard_name;
    DerivedColumnASTNode derived;
  } sublist;
};

struct SelectASTNode {
  bool is_wildcard;
  std::vector<std::unique_ptr<SelectListASTNode>> select_list;
};

}
}
#endif
